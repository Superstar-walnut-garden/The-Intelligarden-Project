#include "SchedulerItem.hpp"

SchedulerItem::SchedulerItem(short id, short event_id, std::string name, Time start, Time duration, std::string weekday, bool enabled, bool on)
    :BaseItem(id, event_id, name, on), start(start), duration(duration), weekday(weekday), enabled(enabled) {}

Time SchedulerItem::getStartTime() 
{
    return start;
}

Time SchedulerItem::getDuration() 
{
    return duration;
}

void SchedulerItem::setStartTime(Time start)
{
    this->start = start;
}
void SchedulerItem::setDuration(Time duration)
{
    this->duration = duration;
}

std::string SchedulerItem::getWeekday() 
{
    return weekday;
}

bool SchedulerItem::isOn() 
{
    return getStatus();
}

bool SchedulerItem::isEnabled() 
{
    return enabled;
}

void SchedulerItem::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

std::string SchedulerItem::toJson()
{
    JsonDocument doc;
    doc["id"] = getId();
    doc["start"] = start.toString();
    doc["duration"] = duration.toString();
    doc["weekday"] = weekday;
    doc["enabled"] = enabled;
    doc["on"] = getStatus();
    std::string output;
    serializeJson(doc, output);
    return output;
}

void SchedulerItem::powerOn() 
{
    setStatus(true);
}

void SchedulerItem::powerOff() 
{
    setStatus(false);
}