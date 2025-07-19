#include "GPIOList.hpp"

GPIOList::GPIOList(): BaseList()
{}

GPIOList::GPIOList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug purposes
}
