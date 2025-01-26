#include "EventItem.hpp"

EventItem::EventItem(int id, std::string name, bool flag, bool occupied) 
    : id(id), name(name), flag(flag), occupied(occupied) 
{
}

EventItem::~EventItem() 
{
}

int EventItem::getId() const 
{
    return id;
}

std::string EventItem::getName() const 
{
    return name;
}

void EventItem::setName(std::string name)
{
    this->name = name;
}

bool EventItem::getFlag() const 
{
    return flag;
}

void EventItem::setFlag(bool flag) 
{
    this->flag = flag;
}

bool EventItem::isOccupied() const 
{
    return occupied;
}

void EventItem::setOccupied(bool occupied) 
{
    this->occupied = occupied;
}