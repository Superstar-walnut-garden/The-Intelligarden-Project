#include "ThermostatManager.hpp"
#include "SignalManager.hpp"
#include "SignalNameResolver.hpp"

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
void ThermostatManager::remove(uint64_t id)
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
void ThermostatManager::modify(uint64_t id, ThermostatItem newItem)
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

std::string ThermostatManager::getName()
{
    return "Thermostat";
}

std::vector<ISignalCompatibleItem *> ThermostatManager::getSignalCompatibleItems()
{
    std::vector<ISignalCompatibleItem *> signalCompatibleList;
    for(auto &item : list.getList()) // copy list
        signalCompatibleList.push_back(&list.getItem(item.getId()));
    return signalCompatibleList;
}

/**
 * @brief Update the ThermostatManager when the Temperature changes.
 * 
 * @param temperature The Temperature value that changed.
 */
void ThermostatManager::update(Temperature* temperature)
{
    SignalNameResolver::SignalNameParameters signalNameParameters;
    signalNameParameters.subsystemName = this->getName();

    for(auto &item : list.getList())
    {
        signalNameParameters.id = item.getId();

        auto temp = temperature->getData(item.getSensor()); // retrive temp value
        auto setpoint = 0.00;
        auto hysteresis = item.getHysteresis();
        auto altTempSignal = SignalManager::getInstance()->getSignalValue(SignalNameResolver::toString(
            SignalNameResolver::SignalNameParameters(this->getName(), item.getId(), item.getAltSetpointLocalSignalName())));
        
        if(altTempSignal.has_value()) // if it's associated with a signal
        {
            if(altTempSignal.value() == true) // if signal value is true
                setpoint = item.getAltSetpoint(); // use the secondary setpoint
        }
        else
            setpoint = item.getSetpoint(); // use the main setpoint
        
        // Temperature control algorithm
        if(temp > (setpoint + hysteresis)) // if temperature rises
        {
            signalNameParameters.localSignalName = item.getCoolerLocalSignalName();
            SignalManager::getInstance()->setSignalValue(SignalNameResolver::toString(signalNameParameters), true); // turn on cooler
        }
        if(temp <= (setpoint - (hysteresis / 2)))
        {
            signalNameParameters.localSignalName = item.getCoolerLocalSignalName();
            SignalManager::getInstance()->setSignalValue(SignalNameResolver::toString(signalNameParameters), false); // turn off cooler
        }
        if(temp < (setpoint - hysteresis))
        {
            signalNameParameters.localSignalName = item.getHeaterLocalSignalName();
            SignalManager::getInstance()->setSignalValue(SignalNameResolver::toString(signalNameParameters), true); // turn on heater
        }
        if(temp >= (setpoint + (hysteresis / 2)))
        {
            signalNameParameters.localSignalName = item.getHeaterLocalSignalName();
            SignalManager::getInstance()->setSignalValue(SignalNameResolver::toString(signalNameParameters), false); // turn off heater
        }
    }
}