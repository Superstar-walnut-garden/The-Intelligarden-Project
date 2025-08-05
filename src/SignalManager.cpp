#include "SignalManager.hpp"
#include "SignalNameResolver.hpp"
#include <ArduinoJson.h>

SignalManager* SignalManager::instance = nullptr;

/** 
 * @brief Construct a new SignalManager object.
 * 
 */
SignalManager::SignalManager() 
{
    loadState(); // load the state from the internal storage (SPIFFS)
}

/** 
 * @brief Destroy the SignalManager object.
 * 
 */
SignalManager::~SignalManager() 
{
    saveState(); // save the state before deleting the instance
    delete instance;
}

/** 
 * @brief Get the instance of the SignalManager (singleton pattern).
 * 
 * @return SignalManager* The instance of the SignalManager.
 */
SignalManager* SignalManager::getInstance() 
{
    if (!instance) {
        instance = new SignalManager();
    }
    return instance;
}

/** 
 * @brief Create a new signal and add it to the signal list.
 * 
 * @param item The new item to be added to the list
 */
void SignalManager::create(SignalItem item) 
{
    signalList.addItem(item);
    saveState();
}

/** 
 * @brief Remove a signal from the signal list.
 * 
 * @param id The ID of the signal to remove.
 */
void SignalManager::remove(uint64_t id) 
{
    signalList.deleteItem(id);
    saveState();
}

/** 
 * @brief Modify a signal in the signal list.
 * 
 * @param id The ID of the signal to modify.
 * @param newItem The new item to replace the old one.
 */
void SignalManager::modify(uint64_t id, SignalItem newItem) 
{
    signalList.modifyItem(id, newItem);
    saveState();
}

/** 
 * @brief Modify the value of a signal in the signal list (for broadcasting).
 * 
 * @param fullSignalPath example: "Thermostat_3_cooler_B"
 * @param Value The new signal value.
 * @param StatusCode if signal is found "StatusCode::SUCSESS" else "StatusCode::NOT_FOUND"
 */
StatusCode SignalManager::setSignalValue(std::string fullSignalPath, bool value)
{
    // search and find the signal path in signal list (broadcaster parameter)
    bool found = false;
    static std::vector<std::string> cycleDetection; // to prevent infinite loop
    auto detectCycle = [&](const std::string &signalPath) 
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
                Serial.println("");
                return true; // cycle detected
            }
        }

        return false; // no cycle detected
    };
    signalList.forEach([&](SignalItem &signalItem) -> void
    {
        auto primaryBroadcasterMatched = signalItem.getBroadcaster().getSignalPath() == fullSignalPath;
        auto auxiliaryBroadcasterMatched = signalItem.getAuxiliaryBroadcaster().getSignalPath() == fullSignalPath;
        if(primaryBroadcasterMatched or auxiliaryBroadcasterMatched)
        {
            if(primaryBroadcasterMatched)
                signalItem.setBroadcasterStatus(value);
            else if(auxiliaryBroadcasterMatched)
                signalItem.setAuxiliaryBroadcasterStatus(value);

            cycleDetection.push_back(fullSignalPath);
            // emit self as a broadcaster
            if(detectCycle(fullSignalPath)) return; // prevent infinite loop
            this->setSignalValue(SignalNameResolver::toString(
                SignalNameResolver::SignalNameParameters(
                    this->getName(), 
                    signalItem.getId(), 
                    signalItem.getEmittedSignalLocalSignalName()
                )), signalItem.getStatus());
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
std::optional<bool> SignalManager::getSignalValue(std::string fullSignalPath)
{
    // search and find the signal path in signal list (listeners parameter)
    std::optional<bool> result = std::nullopt;
    signalList.forEach([&](SignalItem &signalItem) -> void
    {
        for(auto &listener : signalItem.getListeners()) // loop through listeners
            if(listener.getSignalPath() == fullSignalPath)
            {
                auto status = signalList.getItem(signalItem.getId()).getStatus();
                result = listener.isInverted() ? !status : status;
            }
    });
    return result;
}

/** 
 * @brief Get the realtime signal list in JSON format.
 * 
 * @return std::string The signal list in JSON format.
 */
std::string SignalManager::getListJson() 
{
    return signalList.toJson();
}
/**
 * @brief save the state of the signal list to the internal storage (SPIFFS).
 * 
 */
void SignalManager::saveState() 
{
    std::string jsonList = signalList.toJson();
    Configuration::getInstance()->setEventList(jsonList);
}

/**
 * @brief Load the state of the signal list from the internal storage (SPIFFS).
 * 
 */
void SignalManager::loadState() 
{
    std::string state = Configuration::getInstance()->getEventList();
    if (state.empty()) return;

    signalList.repopulateWith(state);
}

/**
 * @brief this is automaticaly called when any of the CentralizedSignalHub changes.
 * 
 */
void SignalManager::update(CentralizedSignalHub *signalHub)
{
    // scan the signal list and check all the broadcaster names and listeners names
    // if there wasn't a match, remove that signalpath.
}

/**
 * @brief Get the list of signal-compatible items in the SignalManager (for the CentralizedSignalHub).
 * @note Never store this std::vector for future use because it may contain dangling pointers if the items are modified or deleted.
 * 
 * @return std::vector<ISignalCompatibleItem*> A vector of pointers to signal-compatible items.
 */
std::vector<ISignalCompatibleItem *> SignalManager::getSignalCompatibleItems()
{
    std::vector<ISignalCompatibleItem *> items;
    signalList.forEach([&](SignalItem &item) -> void
    {
        items.push_back(&signalList.getItem(item.getId())); // pushback real reference of the item, not a copy
    });
    return items;
}