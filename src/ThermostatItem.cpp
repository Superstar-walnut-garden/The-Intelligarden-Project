#include "ThermostatItem.hpp"
#include "SignalNameResolver.hpp"

/**
 * @brief Construct a new ThermostatItem object
 * 
 */
ThermostatItem::ThermostatItem()
    : SignalCompatibleBaseItem(), setpoint(0), altSetpoint(0), hysteresis(0), enabled(true)
{
    registerToJsonCallback([this](JsonDocument &json) -> void
    {
        json["setpoint"] = this->setpoint;
        json["altSetpoint"] = this->altSetpoint;
        json["hysteresis"] = this->hysteresis;
        json["enabled"] = this->enabled;
        json["sensor"] = std::to_string(this->sensor); // use string in json to handle 64bit integers
    });
    registerFromJsonCallback([this](JsonDocument &json) -> void
    {
        this->setpoint = json["setpoint"].as<double>();
        this->altSetpoint = json["altSetpoint"].as<double>();
        this->hysteresis = json["hysteresis"].as<double>();
        this->enabled = json["enabled"].as<bool>();
        this->sensor = std::stoull(json["sensor"].as<std::string>()); // use string in json to handle 64bit integers
    });
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
