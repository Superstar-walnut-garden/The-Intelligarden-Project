#include "SchedulerItem.hpp"

SchedulerItem::SchedulerItem(short id, short event_id, std::string name, Time start, Time duration, std::string weekday, bool enabled, bool on)
    :id(id), event_id(event_id), name(name), start(start), duration(duration), weekday(weekday), enabled(enabled), on(on){}

int SchedulerItem::getId() 
{
    return id;
}

int SchedulerItem::getEventId() 
{
    return event_id;
}

void SchedulerItem::setEventId(int event_id)
{
    this->event_id = event_id;
}

std::string SchedulerItem::getName()
{
    return name;
}

void SchedulerItem::setName(std::string name)
{
    this->name = name;
}

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
    return on;
}

bool SchedulerItem::isEnabled() 
{
    return enabled;
}

void SchedulerItem::setEnabled(bool enabled)
{
    this->enabled = enabled;
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