#include "SchedulerItem.hpp"

SchedulerItem::SchedulerItem() 
    : BaseItem(), start(Time(0, 0)), duration(Time(0, 0)), weekday("0000000"), enabled(false), mode("") {}

SchedulerItem::SchedulerItem(short id, short event_id, std::string name, Time start, Time duration, std::string weekday, bool enabled, bool on, std::string mode)
    :BaseItem(id, event_id, name, on), start(start), duration(duration), weekday(weekday), enabled(enabled), mode(mode) {}

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

bool SchedulerItem::isEnabled() 
{
    return enabled;
}

void SchedulerItem::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

void SchedulerItem::setMode(std::string mode)
{
    this->mode = mode;
}

std::string SchedulerItem::getMode()
{
    return mode;
}

void SchedulerItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    this->start = Time::parse(doc["start"].as<std::string>().c_str());
    this->duration = Time::parse(doc["duration"].as<std::string>().c_str());
    this->weekday = doc["weekday"].as<std::string>();
    this->enabled = doc["enabled"].as<bool>();
    this->mode = doc["mode"].as<std::string>();
}

void SchedulerItem::derivedClassToJson(JsonDocument &doc)
{
    doc["start"] = getStartTime().toString();
    doc["duration"] = getDuration().toString();
    doc["weekday"] = getWeekday();
    doc["enabled"] = isEnabled();
    doc["mode"] = getMode();
}