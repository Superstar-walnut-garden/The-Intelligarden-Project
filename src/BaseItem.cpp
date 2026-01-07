/**
 * @file BaseItem.cpp
 * @brief Implementation of the BaseItem class.
 */

#include "BaseItem.hpp"

/**
 * @brief Default constructor for BaseItem.
 * Initializes id to -1, name to an empty string, and status to false.
 */
BaseItem::BaseItem() : id(-1), name(""), status(false) 
{
    registerSerializationCallbacks();
}

/**
 * @brief Parameterized constructor for BaseItem.
 * @param id The ID of the item.
 * @param name The name of the item.
 * @param status The status of the item.
 */
BaseItem::BaseItem(uint64_t id, std::string name, bool status) 
    : id(id), name(name), status(status) 
{
    registerSerializationCallbacks();
}

/**
 * @brief Destructor for BaseItem.
 */
BaseItem::~BaseItem() 
{
    Serial.printf("BaseItem dtor this=%p\n", this);
}

/**
 * @brief Getter for the ID of the item.
 * @return The ID of the item.
 */
uint64_t BaseItem::getId() const 
{
    return id;
}

void BaseItem::setId(uint64_t id)
{
    this->id = id;
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
 * @brief Register Serialization Callbacks
 */ 
void BaseItem::registerSerializationCallbacks()
{
    registerToJsonCallback([this](JsonDocument &json) -> void
    {
        json["id"] = id;
        json["name"] = name;
        json["status"] = status;
        Serial.println("BaseItem::registerToJsonCallback() lambda called from JsonSerializable 1");
        Serial.printf("vtable=%p\n", *(void**)this);
        derivedClassToJson(json); // for backward compatibility
        Serial.println("BaseItem::registerToJsonCallback() lambda called from JsonSerializable 2");
    });
    registerFromJsonCallback([this](JsonDocument &json) -> void
    {
        id = json["id"].as<uint64_t>();
        name = json["name"].as<std::string>();
        status = json["status"].as<bool>();
        Serial.printf("vtable=%p\n", *(void**)this);
        populateDerivedClassFromJson(json); // for backward compatibility
        Serial.println("BaseItem::registerFromJsonCallback() lambda called from JsonSerializable");
    });
}