#include "Scheduler.hpp"

Scheduler::Scheduler(SchedulerList &list)
    : list(list), time(0, 0) // dummy time initialization
{
    
}
void Scheduler::determineStatusofItems()
{
    auto systemTime = SystemTime::getInstance(); // get direct access to system time
    auto currentTime = systemTime->getTime();
    for(auto &item : list.getList())
    {
        if(systemTime->isCurrentWeekdayPresentIn(SystemTime::parseWeekday(item.getWeekday()))) // check weekday
        {
            const Time untilTime = item.getStartTime() + item.getDuration();
            auto &itemRef = list.getItem(item.getId());
            if(currentTime >= item.getStartTime() and currentTime <= untilTime)
            {
                itemRef.powerOn(); // get a reference to item (because getList() returns a copy of the list)
                Serial.printf("item %d is on (with schedule)", item.getId());
            }
            else
            {
                itemRef.powerOff(); // get a reference to item (because getList() returns a copy of the list)
                Serial.printf("item %d is on (with schedule)", item.getId());
            }
        }
    }
}

bool Scheduler::isAnyItemOn()
{
    for(SchedulerItem &item : list.getList())
        if(item.isOn())
            return true;
    return false;

}