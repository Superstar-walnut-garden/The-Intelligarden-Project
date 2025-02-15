#include "EventList.hpp"

EventList::EventList(): BaseList()
{}

EventList::EventList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug porpuses
}

void EventList::printList()
{
    for (auto& item : getList())
    {
        std::cout << "ID: " << item.getId() << ", Name: " << item.getName() << ", Status: " << item.getStatus() << ", Occupied: " << item.isOccupied() << std::endl;
    }
}