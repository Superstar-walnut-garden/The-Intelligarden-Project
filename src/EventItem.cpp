#include "EventItem.hpp"

EventItem::EventItem() 
    : BaseItem(), occupied(false), logic(""), prevState(false)
{
}

EventItem::EventItem(int id, int event_id, std::string name, bool flag, bool occupied, std::string logic) 
    : BaseItem(id, event_id, name, flag), occupied(occupied), logic(logic), prevState(flag)
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

std::string EventItem::getLogic() const
{
    return logic;
}

void EventItem::setLogic(std::string logic)
{
    this->logic = logic;
}

/**
 * @brief get the previous state (flag). 
 * 
 * @return previous status (flag)
 */
bool EventItem::getPrevState()
{
    return prevState; // return the previous state
}

/**
 * @brief Alter the previous state (flag)
 * 
 * @param prevState The new prevState
 */
void EventItem::setPrevState(bool prevState)
{
    this->prevState = prevState;
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
    setLogic(doc["logic"]);
}

void EventItem::derivedClassToJson(JsonDocument &doc) 
{
    doc["occupied"] = isOccupied();
    doc["logic"] = getLogic();
}