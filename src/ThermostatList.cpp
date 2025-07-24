#include "ThermostatList.hpp"

/**
 * @brief Construct a new Thermostat List:: Thermostat List object
 * 
 */
ThermostatList::ThermostatList(): BaseList()
{}

/**
 * @brief Construct a new Thermostat List:: Thermostat List object
 * 
 * @param json JSON string containing thermostat items.
 */
ThermostatList::ThermostatList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug purposes
}