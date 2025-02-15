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
    @brief get a reference to the desired item
    @param id the id of the desired item
    @return returns an item with an id = -1 if couldn't find a match
*/
SchedulerItem &SchedulerList::getItem(int id)
{
    auto nullItem = SchedulerItem(); // item with id = -1
    for(auto &item : getListRef())
    {
        if(item.getId() == id)
            return item;
    }
    return nullItem;
}

/*!
    @brief prints the entire items of the list via std::cout
*/
void SchedulerList::printList()
{
    for(auto & item : getList())
        std::cout <<
            "ID: " << item.getId() << "\n" <<
            "event_ID: " << item.getEventId() << "\n" <<
            "Start: " << item.getStartTime().toString() << "\n" <<
            "Duration: " << item.getDuration().toString() << "\n" <<
            "Weekday: " << item.getWeekday() << "\n" <<
            "enabled: " << item.isEnabled() << "\n" <<
            "Current Status: " << item.getStatus() <<
            "Mode: " << item.getMode() << "\n\n";
}