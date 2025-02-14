/**
 * @file BaseItem.cpp
 * @brief Implementation of the BaseItem class.
 */

#include "BaseItem.hpp"

/**
 * @brief Default constructor for BaseItem.
 * Initializes id and event_id to 0, name to an empty string, and status to false.
 */
BaseItem::BaseItem() : id(-1), event_id(-1), name(""), status(false) {}

/**
 * @brief Parameterized constructor for BaseItem.
 * @param id The ID of the item.
 * @param event_id The event ID associated with the item.
 * @param name The name of the item.
 * @param status The status of the item.
 */
BaseItem::BaseItem(int id, int event_id, std::string name, bool status) 
    : id(id), event_id(event_id), name(name), status(status) {}

/**
 * @brief Destructor for BaseItem.
 */
BaseItem::~BaseItem() {}

/**
 * @brief Getter for the ID of the item.
 * @return The ID of the item.
 */
int BaseItem::getId() const 
{
    return id;
}

void BaseItem::setId(int id)
{
    this->id = id;
}

/**
 * @brief Getter for the event ID of the item.
 * 
 * @return The event ID of the item. 
 */
int BaseItem::getEventId() const 
{
    return event_id;
}

/**
 * @brief Setter for the event ID of the item.
 * @param event_id The new event ID of the item.
 */
void BaseItem::setEventId(int event_id) 
{
    this->event_id = event_id;
}

/**
 * @brief Getter for the name of the item.
 * @return The name of the item.
 */
std::string BaseItem::getName() const 
{
    return name;
}

/**
 * @brief Setter for the name of the item.
 * @param name The new name of the item.
 */
void BaseItem::setName(std::string name) 
{
    this->name = name;
}

/**
 * @brief Getter for the status of the item.
 * @return The status of the item.
 */
bool BaseItem::getStatus() const 
{
    return status;
}

/**
 * @brief Setter for the status of the item.
 * @param status The new status of the item.
 */
void BaseItem::setStatus(bool status) 
{
    this->status = status;
}

/**
 * @brief Populate the BaseItem from a JSON string (override the method in case of having more parameters).
 * @param json The JSON string to populate the item from.
 */
void BaseItem::populateFromJson(std::string json)
{
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    this->id = doc["id"].as<int>(); 
    this->event_id = doc["event_id"].as<int>();
    this->name = doc["name"].as<std::string>();
    this->status = doc["status"].as<bool>();
}

/**
 * @brief Convert the BaseItem to a JSON string (override the method in case of having more parameters).
 * @return The JSON string representation of the item.
 */ 
std::string BaseItem::toJson()
{
    DynamicJsonDocument doc(1024);
    doc["id"] = this->id;
    doc["event_id"] = this->event_id;
    doc["name"] = this->name;
    doc["status"] = this->status;
    std::string output;
    serializeJson(doc, output);
    return output;
}