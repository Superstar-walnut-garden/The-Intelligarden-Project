#include "ThermostatService.hpp"
#include "SignalRouterService.hpp"
#include "SignalNameResolver.hpp"

/**
 * @brief initialize the instance of the ThermostatService to null
 * 
 */
ThermostatService* ThermostatService::instance = nullptr;

/**
 * @brief Construct a new ThermostatService object
 * 
 */
ThermostatService::ThermostatService()
{
    restoreAll();
}

/**
 * @brief Destroy the ThermostatService object.
 * 
 */
ThermostatService::~ThermostatService()
{
    storeAll(); // save before destruction
}

/**
 * @brief Get the instance of the ThermostatService (singleton pattern).
 * 
 * @return ThermostatService* The instance of the ThermostatService.
 */
ThermostatService* ThermostatService::getInstance()
{
    if (!instance)
    {
        instance = new ThermostatService();
    }
    return instance;
}

/**
 * @brief Create a new Thermostat item and add it to the Thermostat List.
 * 
 * @param newItem The new Thermostat item to add.
 */
void ThermostatService::create(ThermostatItem newItem)
{
    list.addItem(newItem);
    storeAll();
}

/**
 * @brief Remove a Thermostat item from the Thermostat list.
 * 
 * @param id The ID of the Thermostat item to remove.
 */
void ThermostatService::remove(uint64_t id)
{
    list.deleteItem(id);
    notify();
    storeAll();
}

/**
 * @brief Update a Thermostat item in the Thermostat list.
 * 
 * @param id The ID of the Thermostat item to update.
 * @param newItem The new Thermostat item to replace the old one.
 */
void ThermostatService::update(uint64_t id, ThermostatItem newItem)
{
    list.modifyItem(id, newItem);
    notify();
    storeAll();
}

/**
 * @brief Get the Thermostat list in JSON format.
 * 
 * @return std::string The Thermostat list in JSON format.
 */
std::string ThermostatService::getAll()
{
    return list.toJson();
}

/**
 * @brief Get the desired Thermostat item in JSON format.
 * @param id id of the desired item
 * @return std::string The Thermostat item in JSON format.
 */
std::string ThermostatService::get(uint64_t id)
{
    return list.getItem(id).toJson();
}

/**
 * @brief Save the Thermostat list to the configuration.
 * 
 */
void ThermostatService::storeAll()
{
    Configuration::getInstance()->setThermostatList(list.toJson());
}

/**
 * @brief Load the Thermostat list from the configuration.
 * 
 */
void ThermostatService::restoreAll()
{
    auto state = Configuration::getInstance()->getThermostatList();
    if (state.empty())
        return;
    list.repopulateWith(state.c_str());
}

std::string ThermostatService::getName()
{
    return "Thermostat";
}

std::vector<ISignalCompatibleItem *> ThermostatService::getSignalCompatibleItems()
{
    std::vector<ISignalCompatibleItem *> signalCompatibleList;
    for(auto &item : list.getList()) // copy list
        signalCompatibleList.push_back(&list.getItem(item.getId()));
    return signalCompatibleList;
}

/**
 * @brief Update the ThermostatService when the TempSensorService changes.
 * 
 * @param temperature The TempSensorService value that changed.
 */
void ThermostatService::update(TempSensorService* temperature)
{
    SignalNameResolver::SignalNameParameters signalNameParameters;
    signalNameParameters.subsystemName = this->getName();

    for(auto &item : list.getList())
    {
        signalNameParameters.id = item.getId();

        auto temp = temperature->getData(item.getSensor()); // retrive temp value
        auto setpoint = 0.00;
        auto hysteresis = item.getHysteresis();
        auto altTempSignal = SignalRouterService::getInstance()->getSignalValue(SignalNameResolver::toString(
            SignalNameResolver::SignalNameParameters(this->getName(), item.getId(), item.getAltSetpointLocalSignalName())));
        
        if(altTempSignal.has_value()) // if it's associated with a signal
        {
            if(altTempSignal.value() == true) // if signal value is true
                setpoint = item.getAltSetpoint(); // use the secondary setpoint
        }
        else
            setpoint = item.getSetpoint(); // use the main setpoint
        
        // TempSensorService control algorithm
        if(temp > (setpoint + hysteresis)) // if temperature rises
        {
            signalNameParameters.localSignalName = item.getCoolerLocalSignalName();
            SignalRouterService::getInstance()->setSignalValue(SignalNameResolver::toString(signalNameParameters), true); // turn on cooler
        }
        if(temp <= (setpoint - (hysteresis / 2)))
        {
            signalNameParameters.localSignalName = item.getCoolerLocalSignalName();
            SignalRouterService::getInstance()->setSignalValue(SignalNameResolver::toString(signalNameParameters), false); // turn off cooler
        }
        if(temp < (setpoint - hysteresis))
        {
            signalNameParameters.localSignalName = item.getHeaterLocalSignalName();
            SignalRouterService::getInstance()->setSignalValue(SignalNameResolver::toString(signalNameParameters), true); // turn on heater
        }
        if(temp >= (setpoint + (hysteresis / 2)))
        {
            signalNameParameters.localSignalName = item.getHeaterLocalSignalName();
            SignalRouterService::getInstance()->setSignalValue(SignalNameResolver::toString(signalNameParameters), false); // turn off heater
        }
    }
}