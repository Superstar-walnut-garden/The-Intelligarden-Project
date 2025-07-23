#include "SchedulerItem.hpp"

SchedulerItem::SchedulerItem() 
    : SignalCompatibleBaseItem(), start(Time(0, 0)), duration(Time(0, 0)), weekday("0000000"), enabled(false), mode(""), skipped(false) {}

SchedulerItem::SchedulerItem(uint64_t id, std::string name, Time start, Time duration, std::string weekday, bool enabled, bool on, std::string mode, bool skipped)
    : SignalCompatibleBaseItem(id, name, on), start(start), duration(duration), weekday(weekday), enabled(enabled), mode(mode), skipped(skipped) {}

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

void SchedulerItem::setSkipped(bool skipped)
{
    this->skipped = skipped;
}

bool SchedulerItem::isSkipped()
{
    return skipped;
}

std::string SchedulerItem::getMainLocalSignalName() 
{
    return SignalNameResolver::generateLocalSignalName("Main", SignalNameResolver::SignalType::Broadcaster);
}

std::string SchedulerItem::getSkipLocalSignalName() 
{
    return SignalNameResolver::generateLocalSignalName("Skip", SignalNameResolver::SignalType::Listener);
}

std::string SchedulerItem::getPauseLocalSignalName() 
{
    return SignalNameResolver::generateLocalSignalName("Pause", SignalNameResolver::SignalType::Listener);
}

std::vector<std::string> SchedulerItem::getLocalSignalNames()
{
    return 
    {
        getMainLocalSignalName(),
        getSkipLocalSignalName(),
        getPauseLocalSignalName()
    };
}

void SchedulerItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    this->start = Time::parse(doc["start"].as<std::string>().c_str());
    this->duration = Time::parse(doc["duration"].as<std::string>().c_str());
    this->weekday = doc["weekday"].as<std::string>();
    this->enabled = doc["enabled"].as<bool>();
    this->mode = doc["mode"].as<std::string>();
    this->skipped = doc["skipped"].as<bool>();
}

void SchedulerItem::derivedClassToJson(JsonDocument &doc)
{
    doc["start"] = getStartTime().toString();
    doc["duration"] = getDuration().toString();
    doc["weekday"] = getWeekday();
    doc["enabled"] = isEnabled();
    doc["mode"] = getMode();
    doc["skipped"] = isSkipped();
}