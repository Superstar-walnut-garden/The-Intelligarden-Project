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
    for (auto& item : list.getList()) // iterate over the list of items
    {
        auto& itemRef = list.getItem(item.getId());
        if(item.getMode() == "weekly")
        {
            auto untilTime = item.getStartTime() + item.getDuration();
            auto isCurrentTimeBetweenStartAndEnd = currentTime >= item.getStartTime() && currentTime <= untilTime;
            if (systemTime->isCurrentWeekdayPresentIn(SystemTime::parseWeekday(item.getWeekday())) and isCurrentTimeBetweenStartAndEnd) // check weekday and time
            {
                itemRef.setStatus(true);
                Serial.printf("item %d is on (with schedule)\n", item.getId());
            } 
            else
            {
                itemRef.setStatus(false);
                Serial.printf("item %d is off (with schedule)\n", item.getId());
            }
            broadcastItem(itemRef); // broadcast the real item (because only the reference gets updated in this "for" statement)
        }
        else if(item.getMode() == "hourly")
        {
            auto interval = item.getStartTime().getTimeInMinutes(), duration = item.getDuration().getTimeInMinutes();
            auto ctime = currentTime.getTimeInMinutes(); // current time in minutes
            int cyclePosition = 0;
            if(interval != 0) // prevent division by zero
                cyclePosition = ctime % interval;
    
            if (cyclePosition < duration)
            {
                itemRef.setStatus(true);
                Serial.printf("item %d is on (hourly schedule)\n", item.getId());
            }
            else
            {
                itemRef.setStatus(false);
                Serial.printf("item %d is off (hourly schedule)\n", item.getId());
            }
            broadcastItem(itemRef);
        }
        else
        {
            Serial.println("Unsupported mode");
        }
    }
}

bool Scheduler::isAnyItemOn() 
{
    for (SchedulerItem& item : list.getList())
        if (item.getStatus())
            return true;
    return false;
}

void Scheduler::create(SchedulerItem schedulerItem) 
{
    list.addItem(schedulerItem);
    Serial.printf("Schedule %d created\n", schedulerItem.getId());
    saveState();
}

void Scheduler::remove(uint64_t id) 
{
    list.deleteItem(id);
    Serial.printf("Schedule %d removed\n", id);
    saveState();
}

void Scheduler::modify(uint64_t id, SchedulerItem newItem) 
{
    list.modifyItem(id, newItem);
    Serial.printf("Schedule %d modified\n", id);
    saveState();
}

void Scheduler::saveState()
{
    Configuration::getInstance()->setSchedulerList(list.toJson());
}

void Scheduler::loadState() 
{
    auto data = Configuration::getInstance()->getSchedulerList();
    if (data.empty()) return;
        list.repopulateWith(data);
}

std::string Scheduler::getListJson()
{
    return list.toJson();
}

void Scheduler::broadcastItem(SchedulerItem &item)
{
    if(item.getEventId() != -1) // if the item is associated with an event
        EventManager::getInstance()->modifyEventFlag(item.getEventId(), item.getStatus()); // broadcast the status of the item
}