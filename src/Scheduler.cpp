#include "Scheduler.hpp"
#include "Configuration.hpp"
#include <ArduinoJson.h>

Scheduler* Scheduler::instance = nullptr;

Scheduler::Scheduler()
{
    loadState();
}

Scheduler::~Scheduler() 
{
    saveState();
}

Scheduler* Scheduler::getInstance() 
{
    if (!instance) 
    {
        instance = new Scheduler();
    }
    return instance;
}

void Scheduler::update(SystemTime* systemTime) 
{
    determineStatusofItems();
}

void Scheduler::determineStatusofItems() 
{
    auto systemTime = SystemTime::getInstance(); // get direct access to system time
    auto currentTime = systemTime->getTime();
    for (auto& item : list.getList()) 
    {
        if (systemTime->isCurrentWeekdayPresentIn(SystemTime::parseWeekday(item.getWeekday()))) // check weekday
        {
            Time untilTime = item.getStartTime() + item.getDuration();
            auto& itemRef = list.getItem(item.getId());
            if (currentTime >= item.getStartTime() && currentTime <= untilTime) 
            {
                itemRef.powerOn(); // get a reference to item (because getList() returns a copy of the list)
                Serial.printf("item %d is on (with schedule)\n", item.getId());
            } else 
            {
                itemRef.powerOff(); // get a reference to item (because getList() returns a copy of the list)
                Serial.printf("item %d is off (with schedule)\n", item.getId());
            }
            if(item.getEventId() != -1) // if the item is associated with an event
                EventManager::getInstance()->modifyEventFlag(item.getEventId(), item.isOn()); // broadcast the status of the item
        }
    }
}

bool Scheduler::isAnyItemOn() 
{
    for (SchedulerItem& item : list.getList())
        if (item.isOn())
            return true;
    return false;
}

void Scheduler::createSchedule(int id, int eventId, std::string& name, const std::string& start, const std::string& duration, std::string& weekday, bool enabled, bool on) 
{
    SchedulerItem newItem(id, eventId, name, Time::parse(start.c_str()), Time::parse(duration.c_str()), weekday, enabled, on);
    list.addItem(newItem);
    Serial.printf("Schedule %d created\n", id);
    saveState();
}

void Scheduler::removeSchedule(int id) 
{
    list.deleteItem(id);
    Serial.printf("Schedule %d removed\n", id);
    saveState();
}

void Scheduler::modifySchedule(int id, SchedulerItem& newItem) 
{
    list.modifyItem(id, newItem);
    Serial.printf("Schedule %d modified\n", id);
    saveState();
}

void Scheduler::saveState()
{
    Configuration::getInstance()->setSchedulerList(list.getListJson().c_str());
}

void Scheduler::loadState() 
{
    std::string state = Configuration::getInstance()->getSchedulerList().getListJson();
    if (state.empty()) return;
        list.repopulateWith(state.c_str(), state.length());
}

SchedulerList Scheduler::getSchedulerList()
{
    return list;
}