#include "SchedulerItem.hpp"

SchedulerItem::SchedulerItem(short id, Time start, Time duration, std::string weekday, bool enabled, bool on)
    :id(id), start(start), duration(duration), weekday(weekday), enabled(enabled), on(on){}

int SchedulerItem::getId() 
{
    return id;
}

Time SchedulerItem::getStartTime() 
{
    return start;
}

Time SchedulerItem::getDuration() 
{
    return duration;
}

std::string SchedulerItem::getWeekday() 
{
    return weekday;
}

bool SchedulerItem::isOn() 
{
    return on;
}

bool SchedulerItem::isEnabled() 
{
    return enabled;
}

void SchedulerItem::enable() 
{
    this->enabled = true;
}

std::string SchedulerItem::toJson()
{
    JsonDocument doc;
    doc["id"] = id;
    doc["start"] = start.toString();
    doc["duration"] = duration.toString();
    doc["weekday"] = weekday;
    doc["enabled"] = enabled;
    doc["on"] = on;
    std::string output;
    serializeJson(doc, output);
    return output;
}

void SchedulerItem::powerOn() 
{
    this->on = true;
}

void SchedulerItem::powerOff() 
{
    this->on = false;
}