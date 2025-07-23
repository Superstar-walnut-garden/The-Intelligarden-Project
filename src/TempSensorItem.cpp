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
bool TempSensorItem::isConnected()
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
double TempSensorItem::getTemp()
{
    return this->temp;
}