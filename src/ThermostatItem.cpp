#include "ThermostatItem.hpp"

ThermostatItem::ThermostatItem()
    : BaseItem(), heaterEvent_id(-1), coolerEvent_id(-1), setpoint(0), altSetpoint(0), hysteresis(0), enabled(true)
{
}

ThermostatItem::ThermostatItem(int id, int event_id, int heaterEvent_id, int coolerEvent_id, std::string name, double setpoint, double altSetpoint, double hysteresis, bool enabled, uint64_t sensor)
    : BaseItem(id, event_id, name, true), 
    heaterEvent_id(heaterEvent_id), coolerEvent_id(coolerEvent_id), setpoint(setpoint), altSetpoint(altSetpoint), hysteresis(hysteresis), enabled(enabled), sensor(sensor)
{
}

int ThermostatItem::getHeaterEvent_id()
{
    return this->heaterEvent_id;
}

int ThermostatItem::getCoolerEvent_id()
{
    return this->coolerEvent_id;
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

void ThermostatItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    this->coolerEvent_id = doc["coolerEvent_id"].as<int>();
    this->heaterEvent_id = doc["heaterEvent_id"].as<int>();
    this->setpoint = doc["setpoint"].as<double>();
    this->altSetpoint = doc["altSetpoint"].as<double>();
    this->hysteresis = doc["hysteresis"].as<double>();
    this->enabled = doc["enabled"].as<bool>();
    this->sensor = std::stoull(doc["sensor"].as<std::string>()); // use string in json to handle 64bit integers
}

void ThermostatItem::derivedClassToJson(JsonDocument &doc)
{
    doc["coolerEvent_id"] = this->coolerEvent_id;
    doc["heaterEvent_id"] = this->heaterEvent_id;
    doc["setpoint"] = this->setpoint;
    doc["altSetpoint"] = this->altSetpoint;
    doc["hysteresis"] = this->hysteresis;
    doc["enabled"] = this->enabled;
    doc["sensor"] = std::to_string(this->sensor); // use string in json to handle 64bit integers
}