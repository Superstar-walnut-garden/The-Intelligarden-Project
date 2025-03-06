#include "ThermostatList.hpp"

ThermostatList::ThermostatList(): BaseList()
{}

ThermostatList::ThermostatList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug purposes
}