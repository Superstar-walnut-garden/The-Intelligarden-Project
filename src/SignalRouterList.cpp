#include "SignalRouterList.hpp"

/**
 * @brief Constructor for SignalRouterList class.
 * 
 */
SignalRouterList::SignalRouterList(): BaseList()
{}

/**
 * @brief Constructor for SignalRouterList class with JSON input.
 * 
 * @param json JSON string containing signal items.
 */
SignalRouterList::SignalRouterList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug porpuses
}