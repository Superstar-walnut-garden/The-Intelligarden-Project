#include "SchedulerList.hpp"

SchedulerList::SchedulerList()
{
}

/*!
    @brief get the time and date as an Arduino String object
    @param json string of json 
    @param length length of the json string
*/
SchedulerList::SchedulerList(std::string json)
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug porpuses
}

/*!
    @brief prints the entire items of the list via std::cout
*/
void SchedulerList::printList()
{
    forEach([](SchedulerItem *item)
    {
        std::cout <<
            "ID: " << item->getId() << "\n" <<
            "Start: " << item->getStartTime().toString() << "\n" <<
            "Duration: " << item->getDuration().toString() << "\n" <<
            "Weekday: " << item->getWeekday() << "\n" <<
            "enabled: " << item->isEnabled() << "\n" <<
            "Current Status: " << item->getStatus() <<
            "Mode: " << item->getMode() << "\n\n";
    });
}