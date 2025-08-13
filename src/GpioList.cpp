#include "GpioList.hpp"

/**
 * @brief Constructor for GpioList class.
 * 
 */
GpioList::GpioList(): BaseList()
{}

/**
 * @brief Constructor for GpioList class with JSON input.
 * 
 * @param json JSON string containing GPIO items.
 */
GpioList::GpioList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug purposes
}
