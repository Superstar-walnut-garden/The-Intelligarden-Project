#include "ThermostatItem.hpp"
#include "SignalNameResolver.hpp"

/**
 * @brief Construct a new ThermostatItem object
 * 
 */
ThermostatItem::ThermostatItem()
    : SignalCompatibleBaseItem(), setpoint(0), altSetpoint(0), hysteresis(0), enabled(true)
{
}

/**
 * @brief Construct a new ThermostatItem object
 * 
 * @param id The ID of the thermostat item.
 * @param name The name of the thermostat item.
 * @param setpoint The temperature setpoint for the thermostat.
 * @param altSetpoint The alternative temperature setpoint for the thermostat.
 * @param hysteresis The hysteresis value for the thermostat.
 * @param enabled Whether the thermostat is enabled or not.
 * @param sensor The ID of the temperature sensor associated with the thermostat.
 */
ThermostatItem::ThermostatItem(int id, std::string name, double setpoint, double altSetpoint, double hysteresis, bool enabled, uint64_t sensor)
    : SignalCompatibleBaseItem(id, name, true)
    , setpoint(setpoint), altSetpoint(altSetpoint), hysteresis(hysteresis), enabled(enabled), sensor(sensor)
{
}

/**
 * @brief Get the local signal name for the heater.
 * 
 * @return std::string The local signal name for the heater.
 */
std::string ThermostatItem::getHeaterLocalSignalName()
{
    return SignalNameResolver::generateLocalSignalName("heater", SignalNameResolver::SignalType::Broadcaster);
}

/**
 * @brief Get the local signal name for the cooler.
 * 
 * @return std::string The local signal name for the cooler.
 */
std::string ThermostatItem::getCoolerLocalSignalName()
{
    return SignalNameResolver::generateLocalSignalName("cooler", SignalNameResolver::SignalType::Broadcaster);
}

/**
 * @brief Get the local signal name for the alternative setpoint.
 * 
 * @return std::string The local signal name for the alternative setpoint.
 */
std::string ThermostatItem::getAltSetpointLocalSignalName()
{
    return SignalNameResolver::generateLocalSignalName("altSetpoint", SignalNameResolver::SignalType::Listener);
}

/**
 * @brief Get the setpoint of the ThermostatItem.
 * 
 * @return double The setpoint temperature.
 */
double ThermostatItem::getSetpoint()
{
    return this->setpoint;
}

/**
 * @brief Get the alternative setpoint of the ThermostatItem.
 * 
 * @return double The alternative setpoint temperature.
 */
double ThermostatItem::getAltSetpoint()
{
    return this->altSetpoint;
}

/**
 * @brief Get the hysteresis value of the ThermostatItem.
 * 
 * @return double The hysteresis value.
 */
double ThermostatItem::getHysteresis()
{
    return this->hysteresis;
}

/**
 * @brief Check if the ThermostatItem is enabled.
 * 
 * @return true if the ThermostatItem is enabled, false otherwise.
 */
bool ThermostatItem::isEnabled()
{
    return this->enabled;
}

/**
 * @brief Get the sensor ID associated with the ThermostatItem.
 * 
 * @return uint64_t The ID of the temperature sensor.
 */
uint64_t ThermostatItem::getSensor()
{
    return this->sensor;
}

/**
 * @brief Get the local signal names associated with the ThermostatItem (for CentralizedSignalHub).
 * 
 * @return std::vector<std::string> A vector of local signal names.
 */
std::vector<std::string> ThermostatItem::getLocalSignalNames()
{
    return 
    {
        getCoolerLocalSignalName(),
        getHeaterLocalSignalName(),
        getAltSetpointLocalSignalName()
    };
}

/**
 * @brief Populate the ThermostatItem from a JSON document.
 * 
 * @param doc The JSON document containing the ThermostatItem data.
 */
void ThermostatItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    this->setpoint = doc["setpoint"].as<double>();
    this->altSetpoint = doc["altSetpoint"].as<double>();
    this->hysteresis = doc["hysteresis"].as<double>();
    this->enabled = doc["enabled"].as<bool>();
    this->sensor = std::stoull(doc["sensor"].as<std::string>()); // use string in json to handle 64bit integers
}

/**
 * @brief Convert the ThermostatItem to a JSON document.
 * 
 * @param doc The JSON document to populate with the ThermostatItem data.
 */
void ThermostatItem::derivedClassToJson(JsonDocument &doc)
{
    doc["setpoint"] = this->setpoint;
    doc["altSetpoint"] = this->altSetpoint;
    doc["hysteresis"] = this->hysteresis;
    doc["enabled"] = this->enabled;
    doc["sensor"] = std::to_string(this->sensor); // use string in json to handle 64bit integers
}