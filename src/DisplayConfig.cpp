#include "DisplayConfig.hpp"
#include <ArduinoJson.h>
#include "../EnumCrafter.hpp"

/** 
 * @brief Default constructor for DisplayConfig class. Initializes the display type to Oled. 
 * @details This constructor sets the default display type to None, which can be changed later by populating from a JSON string.
 * 
 * @note The display type can be set to either Oled or CharLcd.
 */
DisplayConfig::DisplayConfig() : displayType(DisplayType::None), address(0x3C) {}

/**
 * @brief Constructor for DisplayConfig class that populates the configuration from a JSON string.
 * @param json JSON string containing the display configuration data.
 * @details This constructor initializes the display type based on the provided JSON string.
 * 
 * @note The JSON string should contain a field for the display type, which can be either "Oled" or "CharLcd".
 */
DisplayConfig::DisplayConfig(std::string json) {
    populateFromJson(json);
}

/**
 * @brief Populate the DisplayConfig object from a JSON string.
 * 
 * @return std::string 
 */
std::string DisplayConfig::toJson() const
{
    JsonDocument doc;
    doc["type"] = EnumCrafter::toString(displayType);
    doc["address"] = address;
    std::string output;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief Populate the confg from a JSON string.
 * @param json The JSON string to populate the config from.
 */
void DisplayConfig::populateFromJson(std::string json)
{
    JsonDocument doc;
    deserializeJson(doc, json);
    std::string typeStr = doc["type"].as<std::string>();
    this->address = doc["address"] | 0x3F; // Default address 0x3F if not specified

    auto type = EnumCrafter::parse<DisplayType>(typeStr);
    if (type.has_value()) 
    {
        displayType = type.value();
    } else 
    {
        Serial.println("Error: Invalid display type in JSON. Supported types are 'Oled' and 'CharLcd'.");
        displayType = DisplayType::CharLcd; // Default to Oled if invalid type
    }
}

/**
 * @brief get display type.
 * 
 * @return DisplayConfig::DisplayType 
 */
DisplayConfig::DisplayType DisplayConfig::getDisplayType() 
{
    return displayType;
}

/**
 * @brief get display i2c address.
 * 
 * @return int i2c address
 */
int DisplayConfig::getAddress() const
{
    return address;
}