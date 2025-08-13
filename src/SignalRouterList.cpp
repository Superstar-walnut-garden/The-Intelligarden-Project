#include "SignalList.hpp"

/**
 * @brief Constructor for SignalList class.
 * 
 */
SignalList::SignalList(): BaseList()
{}

/**
 * @brief Constructor for SignalList class with JSON input.
 * 
 * @param json JSON string containing signal items.
 */
SignalList::SignalList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug porpuses
}