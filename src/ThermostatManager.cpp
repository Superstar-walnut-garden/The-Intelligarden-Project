#include "ThermostatManager.hpp"

/**
 * @brief initialize the instance of the ThermostatManager to null
 * 
 */
ThermostatManager* ThermostatManager::instance = nullptr;

/**
 * @brief Construct a new ThermostatManager object
 * 
 */
ThermostatManager::ThermostatManager()
{
    loadState();
}

/**
 * @brief Destroy the ThermostatManager object.
 * 
 */
ThermostatManager::~ThermostatManager()
{
    saveState(); // save before destruction
}

/**
 * @brief Get the instance of the ThermostatManager (singleton pattern).
 * 
 * @return ThermostatManager* The instance of the ThermostatManager.
 */
ThermostatManager* ThermostatManager::getInstance()
{
    if (!instance)
    {
        instance = new ThermostatManager();
    }
    return instance;
}

/**
 * @brief Create a new Thermostat item and add it to the Thermostat List.
 * 
 * @param newItem The new Thermostat item to add.
 */
void ThermostatManager::create(ThermostatItem newItem)
{
    list.addItem(newItem);
    saveState();
}

/**
 * @brief Remove a Thermostat item from the Thermostat list.
 * 
 * @param id The ID of the Thermostat item to remove.
 */
void ThermostatManager::remove(int id)
{
    list.deleteItem(id);
    saveState();
}

/**
 * @brief Modify a Thermostat item in the Thermostat list.
 * 
 * @param id The ID of the Thermostat item to modify.
 * @param newItem The new Thermostat item to replace the old one.
 */
void ThermostatManager::modify(int id, ThermostatItem newItem)
{
    list.modifyItem(id, newItem);
    saveState();
}

/**
 * @brief Get the Thermostat list in JSON format.
 * 
 * @return std::string The Thermostat list in JSON format.
 */
std::string ThermostatManager::getListJson()
{
    return list.toJson();
}

/**
 * @brief Save the Thermostat list to the configuration.
 * 
 */
void ThermostatManager::saveState()
{
    Configuration::getInstance()->setThermostatList(list.toJson());
}

/**
 * @brief Load the Thermostat list from the configuration.
 * 
 */
void ThermostatManager::loadState()
{
    auto state = Configuration::getInstance()->getThermostatList();
    if (state.empty())
        return;
    list.repopulateWith(state.c_str());
}


/**
 * @brief Update the ThermostatManager when the EventManager changes.
 * 
 * @param eventManager The EventManager that changed.
 */
void ThermostatManager::update(EventManager* eventManager)
{
    for (auto& item : list.getList())
    {
        if(item.getEventId() != -1) // if the item is associated with an event.
        {
            bool flag = false;
            if (eventManager->hasEventFlagChanged(item.getEventId(), flag)) // if the event flag has changed update the status of the item
            {
                list.getItem(item.getId()).setStatus(flag); // use reference to set the status of the actual item.
            }
        }
    }
}

/**
 * @brief Update the ThermostatManager when the Temperature changes.
 * 
 * @param temperature The Temperature value that changed.
 */
void ThermostatManager::update(Temperature* temperature)
{
    for(auto &item : list.getList())
    {
        auto temp = temperature->getData(item.getSensor()); // retrive temp value
        auto setpoint = 0.00;
        auto hysteresis = item.getHysteresis();
        if(item.getEventId() != -1 and item.getStatus()) // if it's associated with an event and the event made its status true:
            setpoint = item.getAltSetpoint(); // use the secondary setpoint
        else
            setpoint = item.getSetpoint(); // use the main setpoint
        
        // Temperature control algorithm
        if(temp > (setpoint + hysteresis)) // if temperature rises
        {
            EventManager::getInstance()->modifyEventFlag(item.getCoolerEvent_id(), true); // turn on cooler
        }
        if(temp <= (setpoint - (hysteresis / 2)))
        {
            EventManager::getInstance()->modifyEventFlag(item.getCoolerEvent_id(), false); // turn off cooler
        }
        if(temp < (setpoint - hysteresis))
        {
            EventManager::getInstance()->modifyEventFlag(item.getHeaterEvent_id(), true); // turn on heater
        }
        if(temp >= (setpoint + (hysteresis / 2)))
        {
            EventManager::getInstance()->modifyEventFlag(item.getHeaterEvent_id(), false); // turn off heater
        }
    }
}