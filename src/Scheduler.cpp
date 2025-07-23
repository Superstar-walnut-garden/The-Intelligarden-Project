#include "Scheduler.hpp"
#include "Configuration.hpp"
#include <ArduinoJson.h>
#include "SignalManager.hpp"
#include "SignalNameResolver.hpp"

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

void Scheduler::determineStatusofItems() 
{
    auto systemTime = SystemTime::getInstance(); // get direct access to system time
    auto currentTime = systemTime->getTime();
    for (auto& item : list.getList()) // iterate over the list of items
    {
        auto setItemStatus = [this](SchedulerItem& item, bool status) // lambda to set the status of item
        { 
            auto skipSignal = SignalManager::getInstance()->
                getSignalValue(SignalNameResolver::toString(
                    SignalNameResolver::SignalNameParameters(this->getName(), item.getId(), item.getSkipLocalSignalName())));
            if(!status) // if the item is off, cancell the skip
                item.setSkipped(false);
            else if(skipSignal.has_value()) // if the item is on, and associated with a signal.
                item.setSkipped(skipSignal.value()); // skip the item if the signal value is "true".
            
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

std::vector<ISignalCompatibleItem *> Scheduler::getSignalCompatibleItems()
{
    std::vector<ISignalCompatibleItem *> items;
    for (auto& item : list.getList())
    {
        items.push_back(&list.getItem(item.getId())); // pushback real reference of the item, not a copy
        // Note: This assumes that the list's items are not going to be deleted or modified in a way that invalidates the pointers.
        // If they are, you may need to handle that case to avoid dangling pointers.
    }
    return items;
}

void Scheduler::loop()
{
    determineStatusofItems(); // determine the status of items based on the current time and weekday
}

void Scheduler::broadcastItem(SchedulerItem &item)
{
    SignalManager::getInstance()->setSignalValue(SignalNameResolver::toString(
        SignalNameResolver::SignalNameParameters(
            this->getName(), 
            item.getId(), 
            item.getMainLocalSignalName()
        )), item.getStatus());
}