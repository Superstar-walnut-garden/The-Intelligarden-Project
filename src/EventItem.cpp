#include "EventItem.hpp"

EventItem::EventItem() 
    : BaseItem() 
{
}

EventItem::EventItem(int id, std::string name, bool flag, bool occupied) 
    : BaseItem(id, 0, name, flag), occupied(occupied) 
{
}

EventItem::~EventItem() 
{
}

bool EventItem::getFlag() const 
{
    return getStatus();
}

void EventItem::setFlag(bool flag) 
{
    setStatus(flag);
}

bool EventItem::isOccupied() const 
{
    return occupied;
}

void EventItem::setOccupied(bool occupied) 
{
    this->occupied = occupied;
}

void EventItem::populateFromJson(std::string json) 
{
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    setId(doc["id"]);
    setName(doc["name"]);
    setFlag(doc["status"]);
    setOccupied(doc["occupied"]);
}

std::string EventItem::toJson() 
{
    DynamicJsonDocument doc(1024);
    doc["id"] = getId();
    doc["name"] = getName();
    doc["status"] = getFlag();
    doc["occupied"] = isOccupied();
    std::string output;
    serializeJson(doc, output);
    return output;
}