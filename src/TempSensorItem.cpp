#include "TempSensorItem.hpp"


/**
 * @brief Construct a new Temp Sensor Item:: Temp Sensor Item object
 * 
 */
TempSensorItem::TempSensorItem()
    : BaseItem(-1, "", false) {}

/**
 * @brief Construct a new Temp Sensor Item:: Temp Sensor Item object
 * 
 * @param id 
 * @param name 
 * @param isConnected 
 */
TempSensorItem::TempSensorItem(uint64_t id, std::string name, bool isConnected)
    : BaseItem(id, name, isConnected) {}

/**
 * @brief Overloaded operator to compare two TempSensorItem objects based on their address (id).
 * 
 * @param obj The object to compare with.
 * @return true If the objects have the same address (id).
 * @return false If the objects have different address (id).
 */
bool TempSensorItem::operator == (TempSensorItem &obj)
{
    return this->getId() == obj.getId();
}

/**
 * @brief Check if the TempSensorItem object is connected.
 * 
 * @return true If the object is connected.
 * @return false If the object is not connected.
 */
bool TempSensorItem::isConnected() const
{
    return this->getStatus();
}

/**
 * @brief populate parameters from JSON
 * 
 */
void TempSensorItem::populateDerivedClassFromJson(JsonDocument &json)
{
    this->temp = json["temp"].as<double>();
    this->setId(std::stoull(json["id"].as<std::string>())); // passing 64bit id as string to prevent json and web api js issues with large numbers
}

/**
 * @brief convert the parameters to JSON
 * 
 */
void TempSensorItem::derivedClassToJson(JsonDocument &doc)
{
    doc["temp"] = this->temp;
    doc["id"] = std::to_string(this->getId()); // passing 64bit id as string to prevent json and web api js issues with large numbers
}

/**
 * @brief Set the temperature of the TempSensorItem object.
 * 
 * @param temp The temperature to set.
 */
void TempSensorItem::setTemp(double temp)
{
    this->temp = temp;
}

/**
 * @brief Get the temperature of the TempSensorItem object.
 * 
 * @return double The temperature of the object.
 */
double TempSensorItem::getTemp() const
{
    return this->temp;
}

/**
 * @brief Get the name of the TempSensorItem object (for ambiguity resolving).
 * 
 * @return std::string The name of the object.
 */
std::string TempSensorItem::getName() const
{
    return BaseItem::getName();
}

/**
 * @brief Get the ID of the TempSensorItem object (for ambiguity resolving).
 * 
 * @return uint64_t The ID of the object.
 */
uint64_t TempSensorItem::getId() const
{
    return BaseItem::getId();
}

/**
 * @brief Get the logging interval for the TempSensorItem object.
 * 
 * @return uint64_t The logging interval in milliseconds.
 */
uint64_t TempSensorItem::getInterval() const
{
    return 5000; // log every 5 seconds
}

/**
 * @brief Check if the TempSensorItem object should log only on change.
 * 
 * @return true If the object should log only on change.
 * @return false If the object should log at every interval.
 */
bool TempSensorItem::logOnlyOnChange() const
{
    return false; // log at every interval
}

/**
 * @brief Get the data of the TempSensorItem object as a JSON string.
 * 
 * @return std::optional<std::string> The JSON string of the object's data.
 */
std::optional<std::string> TempSensorItem::getData() const
{
    JsonDocument doc;
    doc["temp"] = this->getTemp();
    std::string jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

/**
 * @brief Check if logging is enabled for the TempSensorItem object.
 * 
 * @return true If logging is enabled.
 * @return false If logging is disabled.
 */
bool TempSensorItem::isLoggingEnabled() const
{
    return true;
}