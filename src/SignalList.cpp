#include "SignalList.hpp"

SignalList::SignalList(): BaseList()
{}

SignalList::SignalList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug porpuses
}