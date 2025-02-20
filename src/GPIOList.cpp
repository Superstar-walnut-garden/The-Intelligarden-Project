#include "GPIOList.hpp"

GPIOList::GPIOList(): BaseList()
{}

GPIOList::GPIOList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug purposes
}

void GPIOList::printList()
{
    for (auto& item : getList())
    {
        std::cout <<
         "ID: " << item.getId() << 
         ", Name: " << item.getName() << 
         ", Status: " << item.getStatus() << 
         ", Mode: " << item.getMode() <<
         ", Event_ID: " << item.getEventId() <<
         ", ExtraParameters: " << item.getExtraParameters() << std::endl;
    }
}