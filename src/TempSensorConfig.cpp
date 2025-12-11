#include "TempSensorConfig.hpp"

/**
 * @brief Default constructor initializing sensorPin to 15
 */
TempSensorConfig::TempSensorConfig(): sensorPin(15) {}

/**
 * @brief Constructor that populates the configuration from a JSON string.
 * 
 * @param json JSON string representing the configuration.
 */
TempSensorConfig::TempSensorConfig(std::string json)
{
    populateFromJson(json);
}

/**
 * @brief Serialize the configuration to a JSON string.
 * 
 * @return std::string JSON representation of the configuration.
 */
std::string TempSensorConfig::toJson() const
{
    JsonDocument doc;
    doc["sensorPin"] = sensorPin;

    std::string output;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief Populate the configuration from a JSON string.
 * 
 * @param json JSON string representing the configuration.
 */
void TempSensorConfig::populateFromJson(std::string json)
{
    JsonDocument doc;
    deserializeJson(doc, json);
    sensorPin = doc["sensorPin"].as<int>();
}

/**
 * @brief Get the sensor pin number.
 * 
 * @return int Pin number where the temperature sensor is connected.
 */
int TempSensorConfig::getSensorPin() const
{
    return sensorPin;
}