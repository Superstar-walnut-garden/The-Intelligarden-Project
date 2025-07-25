#include "SignalManager.hpp"
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
    signalList.forEach([&](SignalItem &signalItem) -> void
    {
        if(signalItem.getBroadcaster().getSignalPath() == fullSignalPath)
        {
            signalList.getItem(signalItem.getId())
                .setStatus(signalItem.getBroadcaster().isInverted() ? !value : value);
            found = true;
            // do not break or return here because broadcasting to multiple SignalItems is allowed.
        }
    });

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