#include "GPIOList.hpp"

/**
 * @brief Constructor for GPIOList class.
 * 
 */
GPIOList::GPIOList(): BaseList()
{}

/**
 * @brief Constructor for GPIOList class with JSON input.
 * 
 * @param json JSON string containing GPIO items.
 */
GPIOList::GPIOList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug purposes
}
