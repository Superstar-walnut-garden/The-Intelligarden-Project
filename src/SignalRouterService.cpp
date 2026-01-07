#include "SignalRouterService.hpp"
#include "SignalNameResolver.hpp"
#include "CentralizedSignalHubService.hpp"
#include <ArduinoJson.h>

SignalRouterService* SignalRouterService::instance = nullptr;

/** 
 * @brief Construct a new SignalRouterService object.
 * 
 */
SignalRouterService::SignalRouterService() 
{
    restoreAll(); // load the state from the internal storage (SPIFFS)
    CentralizedSignalHubService::getInstance()->attach(this); // get attached to the CentralizedSignalHubService to receive updates (via observer pattern)
}

/** 
 * @brief Destroy the SignalRouterService object.
 * 
 */
SignalRouterService::~SignalRouterService() 
{
    storeAll(); // save the state before deleting the instance
    delete instance;
}

/** 
 * @brief Get the instance of the SignalRouterService (singleton pattern).
 * 
 * @return SignalRouterService* The instance of the SignalRouterService.
 */
SignalRouterService* SignalRouterService::getInstance() 
{
    if (!instance) {
        instance = new SignalRouterService();
    }
    return instance;
}

/** 
 * @brief Create a new signal and add it to the signal list.
 * 
 * @param item The new item to be added to the list
 */
void SignalRouterService::create(std::unique_ptr<SignalRouterItem> item) 
{
    signalList.addItem(std::move(item));
    storeAll();
}

/** 
 * @brief Remove a signal from the signal list.
 * 
 * @param id The ID of the signal to remove.
 */
void SignalRouterService::remove(uint64_t id) 
{
    signalList.deleteItem(id);
    notify();
    storeAll();
}

/** 
 * @brief Update a signal in the signal list.
 * 
 * @param id The ID of the signal to update.
 * @param newItem The new item to replace the old one.
 */
void SignalRouterService::update(uint64_t id, std::unique_ptr<SignalRouterItem> newItem) 
{
    signalList.modifyItem(id, std::move(newItem));
    notify();
    storeAll();
}

/** 
 * @brief Modify the value of a signal in the signal list (for broadcasting).
 * 
 * @param fullSignalPath example: "Thermostat_3_cooler_B"
 * @param Value The new signal value.
 * @param StatusCode if signal is found "StatusCode::SUCSESS" else "StatusCode::NOT_FOUND"
 */
StatusCode SignalRouterService::setSignalValue(std::string fullSignalPath, bool value)
{
    // search and find the signal path in signal list (broadcaster parameter)
    bool found = false;
    static std::vector<std::string> cycleDetection; // to keep trak of chain calls and prevent endless cycle (prevents stack overflow)
    auto detectCycle = [&](const std::string &signalPath) // this function searches signal paths in cycleDetection and detects cycles.
    {
        if (cycleDetection.size() <= 1)
            return false;

        for (auto i = cycleDetection.begin(); i != cycleDetection.end() - 1; ++i)
        {
            if (*i == signalPath)
            {
                Serial.print("Error: Cycle detected in signal path: ");
                for(auto & path : cycleDetection)
                    Serial.print((path + " -> ").c_str());
                Serial.println("end");
                return true; // cycle detected
            }
        }

        return false; // no cycle detected
    };
    signalList.forEach([&](SignalRouterItem *signalItem) -> void
    {
        auto primaryBroadcasterMatched = signalItem->getBroadcaster().getSignalPath() == fullSignalPath;
        auto auxiliaryBroadcasterMatched = signalItem->getAuxiliaryBroadcaster().getSignalPath() == fullSignalPath;
        if(primaryBroadcasterMatched or auxiliaryBroadcasterMatched)
        {
            if(primaryBroadcasterMatched)
                signalItem->setBroadcasterStatus(value);
            else if(auxiliaryBroadcasterMatched and (signalItem->getMode() != SignalRouterItem::Mode::SingleSource))
                signalItem->setAuxiliaryBroadcasterStatus(value);
            else
                return; // skip a forEach cycle (like "continue" keyword)

            cycleDetection.push_back(fullSignalPath);
            // emit self as a broadcaster
            if(detectCycle(fullSignalPath)) return; // skip (continue) "forEach" to prevent infinite loop
            this->setSignalValue(SignalNameResolver::toString(
                SignalNameResolver::SignalNameParameters(
                    this->getName(), 
                    signalItem->getId(), 
                    signalItem->getEmittedSignalLocalSignalName()
                )), signalItem->getStatus());
            found = true;
            // do not break or return here because broadcasting to multiple SignalItems is allowed.
        }
    });
    cycleDetection.clear();
    if(!found)
        return StatusCode::NOT_FOUND;

    return StatusCode::SUCCESS;
}

/** 
 * @brief Get the value of a signal in the signal list.
 * 
 * @param fullSignalPath example: "Thermostat_3_cooler_B"
 * @return std::optional<bool> The value of the signal if found, otherwise std::nullopt.
 */
std::optional<bool> SignalRouterService::getSignalValue(std::string fullSignalPath)
{
    // search and find the signal path in signal list (listeners parameter)
    std::optional<bool> result = std::nullopt;
    signalList.forEach([&](SignalRouterItem *signalItem) -> void
    {
        for(auto &listener : signalItem->getListeners()) // loop through listeners
            if(listener.getSignalPath() == fullSignalPath)
                result = listener.getStatus();
    });
    return result;
}

/** 
 * @brief Get the realtime signal list in JSON format.
 * 
 * @return std::string The signal list in JSON format.
 */
std::string SignalRouterService::getAll() 
{
    return signalList.toJson();
}

/** 
 * @brief Get a signal item in JSON format.
 * 
 * @return std::string The signal item in JSON format.
 */
std::string SignalRouterService::get(uint64_t id) 
{
    auto item = signalList.getItem(id);
    return item ? item->toJson() : "{}";
}

/**
 * @brief save the state of the signal list to the internal storage (SPIFFS).
 * 
 */
void SignalRouterService::storeAll() 
{
    std::string jsonList = signalList.toJson();
    Configuration::getInstance()->setEventList(jsonList);
}

/**
 * @brief Load the state of the signal list from the internal storage (SPIFFS).
 * 
 */
void SignalRouterService::restoreAll() 
{
    std::string state = Configuration::getInstance()->getEventList();
    if (state.empty()) return;

    signalList.repopulateWith(state);
}

/**
 * @brief this is automaticaly called when any of the CentralizedSignalHubService changes.
 * 
 */
void SignalRouterService::update(CentralizedSignalHubService *signalHub)
{
    Serial.println("SignalRouterService received update from CentralizedSignalHubService.");
    Serial.println("checking signal paths...");
    // scan the signal list and check all the broadcaster names and listeners names
    // if there wasn't a match, remove that signalpath.
    signalList.forEach([&](SignalRouterItem *item) -> void
    {
        // check if the broadcaster signal path is valid
        if(!CentralizedSignalHubService::getInstance()->isSignalPathValid(item->getBroadcaster().getSignalPath()))
            item->removeBroadcaster();

        // check if the auxiliary broadcaster signal path is valid
        if(!CentralizedSignalHubService::getInstance()->isSignalPathValid(item->getAuxiliaryBroadcaster().getSignalPath()))
            item->removeBroadcaster(true); // remove auxiliary broadcaster

        // check if the listeners signal paths are valid
        for(auto &listener : item->getListeners())
            if(!CentralizedSignalHubService::getInstance()->isSignalPathValid(listener.getSignalPath()))
                item->removeListener(listener.getSignalPath());
    });
    storeAll(); // save the state after removing invalid signal paths
}

/**
 * @brief Get the list of signal-compatible items in the SignalRouterService (for the CentralizedSignalHubService).
 * @note Never store this std::vector for future use because it may contain dangling pointers if the items are modified or deleted.
 * 
 * @return std::vector<ISignalCompatibleItem*> A vector of pointers to signal-compatible items.
 */
std::vector<ISignalCompatibleItem *> SignalRouterService::getSignalCompatibleItems()
{
    return signalList.getAllAs<ISignalCompatibleItem>();
}