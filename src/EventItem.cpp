#include "EventItem.hpp"

EventItem::EventItem() 
    : BaseItem() 
{
}

EventItem::EventItem(int id, int event_id, std::string name, bool flag, bool occupied, bool invert) 
    : BaseItem(id, event_id, name, flag), occupied(occupied), invert(invert)
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

bool EventItem::isInvert() const
{
    return invert;
}

void EventItem::setInvert(bool invert)
{
    this->invert = invert;
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
    setInvert(doc["invert"]);
}

void EventItem::derivedClassToJson(JsonDocument &doc) 
{
    doc["occupied"] = isOccupied();
    doc["invert"] = isInvert();
}