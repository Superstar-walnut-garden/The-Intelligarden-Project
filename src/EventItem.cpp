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

void EventItem::populateDerivedClassFromJson(JsonDocument &doc) 
{
    setOccupied(doc["occupied"]);
}

void EventItem::derivedClassToJson(JsonDocument &doc) 
{
    doc["occupied"] = isOccupied();
}