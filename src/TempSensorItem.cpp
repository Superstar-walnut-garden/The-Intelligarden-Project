#include "TempSensorItem.hpp"


/**
 * @brief Construct a new Temp Sensor Item:: Temp Sensor Item object
 * 
 */
TempSensorItem::TempSensorItem(uint64_t id, std::string name, bool isConnected)
    : FusionBusItem(FusionBusItem::DeviceType::TempSensor), loggingEnabled(false), logInterval(60), logOnlyOnDataChange(false)
{
    this->setId(id);
    this->setName(name);
    this->setStatus(isConnected);
    Serial.println("TempSensorItem Constructor Called!!!");
    registerToJsonCallback([this](JsonDocument &json) -> void
    {
        Serial.println("TempSensorItem ToJson Callback Called!!!");
        json["temp"] = this->temp;

        json["logInterval"] = this->logInterval;
        json["logOnlyOnChange"] = this->logOnlyOnDataChange;
        json["loggingEnabled"] = this->loggingEnabled;
    });
    registerFromJsonCallback([this](JsonDocument &json) -> void
    {
        Serial.println("TempSensorItem FromJson Callback Called!!!");
        this->temp = json["temp"].as<double>();
        
        this->logInterval = json["logInterval"].as<uint64_t>();
        this->logOnlyOnDataChange = json["logOnlyOnChange"].as<bool>();
        this->loggingEnabled = json["loggingEnabled"].as<bool>();
    });
}

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
 * @return uint64_t The logging interval in seconds.
 */
uint64_t TempSensorItem::getInterval() const
{
    return logInterval;
}

/**
 * @brief Check if the TempSensorItem object should log only on change.
 * 
 * @return true If the object should log only on change.
 * @return false If the object should log at every interval.
 */
bool TempSensorItem::logOnlyOnChange() const
{
    return logOnlyOnDataChange;
}

/**
 * @brief Get the data of the TempSensorItem object as a JSON string.
 * 
 * @return std::string The JSON string of the object's data.
 */
std::string TempSensorItem::getData() const
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
    return loggingEnabled;
}

/**
 * @brief Set the logging interval of the TempSensorItem object.
 * 
 * @param interval The interval in seconds to set.
 */
void TempSensorItem::setInterval(uint64_t interval)
{
    this->logInterval = interval;
}

/**
 * @brief Set the status of shouldLogOnlyOnChange parameter of the TempSensorItem object.
 * 
 * @param status should log only on change???
 */
void TempSensorItem::setLogOnlyOnChange(bool status)
{
    this->logOnlyOnDataChange = status;
}

/**
 * @brief Set the status of dataLogging of the TempSensorItem object.
 * 
 * @param status set logging enabled or not.
 */
void TempSensorItem::setLoggingEnabled(bool status)
{
    this->loggingEnabled = status;
}
