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

void Scheduler::update(EventManager* eventManager) 
{
    // for (auto& item : list.getList())
    // {
    //     if(item.getSkipEventId() != -1) // if the item is associated with an event.
    //     {
    //         bool flag = false;
    //         if (eventManager->hasEventFlagChanged(item.getSkipEventId(), flag)) // if the event flag has changed update the status of the item
    //         {
    //             if(flag) // Only Skip on Rising-Edge of The Event
    //                 list.getItem(item.getId()).setSkipped(flag); // use reference to skip the actual item.
    //         }
    //     }
    // }
}

void Scheduler::determineStatusofItems() 
{
    auto systemTime = SystemTime::getInstance(); // get direct access to system time
    auto currentTime = systemTime->getTime();
    for (auto& item : list.getList()) // iterate over the list of items
    {
        auto setItemStatus = [](SchedulerItem& item, bool status) // lambda to set the status of item
        { 
            if(!status) // if the item is off, cancell the skip
                item.setSkipped(false);
            else if(item.getSkipEventId() != -1) // if the item is on, and associated with an event.
                item.setSkipped(EventManager::getInstance()->getEventFlag(item.getSkipEventId())); // skip the item if the event status is "true".
            
            item.setStatus(status and !item.isSkipped()); 
            Serial.println(("item" + std::to_string(item.getId()) + ": is" + std::to_string(item.getStatus())).c_str());
        };
        auto& itemRef = list.getItem(item.getId());
        if(item.getMode() == "weekly")
        {
            auto untilTime = item.getStartTime() + item.getDuration();
            auto isCurrentTimeBetweenStartAndEnd = currentTime >= item.getStartTime() && currentTime <= untilTime;
            auto newStatus = (systemTime->isCurrentWeekdayPresentIn(SystemTime::parseWeekday(item.getWeekday())) and isCurrentTimeBetweenStartAndEnd); // check weekday and time
            setItemStatus(itemRef, newStatus);
            broadcastItem(itemRef); // broadcast the real item (because only the reference gets updated in this "for" statement)
        }
        else if(item.getMode() == "hourly")
        {
            auto interval = item.getStartTime().getTimeInMinutes(), duration = item.getDuration().getTimeInMinutes();
            auto ctime = currentTime.getTimeInMinutes(); // current time in minutes
            int cyclePosition = 0;
            if(interval != 0) // prevent division by zero
                cyclePosition = ctime % interval;
            
            auto newStatus = (cyclePosition < duration);
            setItemStatus(itemRef, newStatus);
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