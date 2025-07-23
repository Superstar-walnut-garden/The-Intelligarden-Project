#include "ThermostatItem.hpp"
#include "SignalNameResolver.hpp"

ThermostatItem::ThermostatItem()
    : SignalCompatibleBaseItem(), setpoint(0), altSetpoint(0), hysteresis(0), enabled(true)
{
}

ThermostatItem::ThermostatItem(int id, std::string name, double setpoint, double altSetpoint, double hysteresis, bool enabled, uint64_t sensor)
    : SignalCompatibleBaseItem(id, name, true)
    , setpoint(setpoint), altSetpoint(altSetpoint), hysteresis(hysteresis), enabled(enabled), sensor(sensor)
{
}

std::string ThermostatItem::getHeaterLocalSignalName()
{
    return SignalNameResolver::generateLocalSignalName("heater", SignalNameResolver::SignalType::Broadcaster);
}

std::string ThermostatItem::getCoolerLocalSignalName()
{
    return SignalNameResolver::generateLocalSignalName("cooler", SignalNameResolver::SignalType::Broadcaster);
}

std::string ThermostatItem::getAltSetpointLocalSignalName()
{
    return SignalNameResolver::generateLocalSignalName("altSetpoint", SignalNameResolver::SignalType::Listener);
}

double ThermostatItem::getSetpoint()
{
    return this->setpoint;
}

double ThermostatItem::getAltSetpoint()
{
    return this->altSetpoint;
}

double ThermostatItem::getHysteresis()
{
    return this->hysteresis;
}

bool ThermostatItem::isEnabled()
{
    return this->enabled;
}

uint64_t ThermostatItem::getSensor()
{
    return this->sensor;
}

std::vector<std::string> ThermostatItem::getLocalSignalNames()
{
    return 
    {
        getCoolerLocalSignalName(),
        getHeaterLocalSignalName(),
        getAltSetpointLocalSignalName()
    };
}

void ThermostatItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    this->setpoint = doc["setpoint"].as<double>();
    this->altSetpoint = doc["altSetpoint"].as<double>();
    this->hysteresis = doc["hysteresis"].as<double>();
    this->enabled = doc["enabled"].as<bool>();
    this->sensor = std::stoull(doc["sensor"].as<std::string>()); // use string in json to handle 64bit integers
}

void ThermostatItem::derivedClassToJson(JsonDocument &doc)
{
    doc["setpoint"] = this->setpoint;
    doc["altSetpoint"] = this->altSetpoint;
    doc["hysteresis"] = this->hysteresis;
    doc["enabled"] = this->enabled;
    doc["sensor"] = std::to_string(this->sensor); // use string in json to handle 64bit integers
}