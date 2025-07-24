#include "SchedulerItem.hpp"

/**
 * @brief Construct a new SchedulerItem object.
 * 
 */
SchedulerItem::SchedulerItem() 
    : SignalCompatibleBaseItem(), start(Time(0, 0)), duration(Time(0, 0)), weekday("0000000"), enabled(false), mode(""), skipped(false) {}

/**
 * @brief Construct a new Scheduler Item:: Scheduler Item object
 * 
 * @param id the ID of the SchedulerItem
 * @param name the name of the SchedulerItem
 * @param start the start time of the SchedulerItem
 * @param duration the duration of the SchedulerItem
 * @param weekday the weekday of the SchedulerItem (binary string)
 * @param enabled whether the SchedulerItem is enabled
 * @param on whether the SchedulerItem is on (status)
 * @param mode the mode of the SchedulerItem (weekly/hourly)
 * @param skipped whether the SchedulerItem is skipped
 */
SchedulerItem::SchedulerItem(uint64_t id, std::string name, Time start, Time duration, std::string weekday, bool enabled, bool on, std::string mode, bool skipped)
    : SignalCompatibleBaseItem(id, name, on), start(start), duration(duration), weekday(weekday), enabled(enabled), mode(mode), skipped(skipped) {}

/**
 * @brief Get the start time of the SchedulerItem.
 * 
 * @return Time 
 */
Time SchedulerItem::getStartTime() 
{
    return start;
}

/**
 * @brief Get the duration of the SchedulerItem.
 * 
 * @return Time 
 */
Time SchedulerItem::getDuration() 
{
    return duration;
}

/**
 * @brief Set the start time of the SchedulerItem.
 * 
 * @param start The start time to set.
 */
void SchedulerItem::setStartTime(Time start)
{
    this->start = start;
}

/**
 * @brief Set the duration of the SchedulerItem.
 * 
 * @param duration The duration to set.
 */
void SchedulerItem::setDuration(Time duration)
{
    this->duration = duration;
}

/**
 * @brief Get the weekday of the SchedulerItem.
 * 
 * @return std::string The weekday as a binary string (e.g., "0000001" for Sunday).
 */
std::string SchedulerItem::getWeekday() 
{
    return weekday;
}

/**
 * @brief Set the weekday of the SchedulerItem.
 * 
 * @param weekday The weekday to set as a binary string.
 */
bool SchedulerItem::isEnabled() 
{
    return enabled;
}

/**
 * @brief Set the enabled status of the SchedulerItem.
 * 
 * @param enabled The enabled status to set.
 */
void SchedulerItem::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

/**
 * @brief Set the weekday of the SchedulerItem.
 * 
 * @param weekday The weekday to set as a binary string.
 */
void SchedulerItem::setMode(std::string mode)
{
    this->mode = mode;
}

/**
 * @brief Get the mode of the SchedulerItem (weekly/hourly).
 * 
 * @return std::string The mode of the SchedulerItem.
 */
std::string SchedulerItem::getMode()
{
    return mode;
}

/**
 * @brief Set the skipped status of the SchedulerItem.
 * 
 * @param skipped The skipped status to set.
 */
void SchedulerItem::setSkipped(bool skipped)
{
    this->skipped = skipped;
}

/**
 * @brief Get the skipped status of the SchedulerItem.
 * 
 * @return true if the SchedulerItem is skipped, false otherwise.
 */
bool SchedulerItem::isSkipped()
{
    return skipped;
}

/**
 * @brief Get the main local signal name for the SchedulerItem.
 * 
 * @return std::string The main local signal name.
 */
std::string SchedulerItem::getMainLocalSignalName() 
{
    return SignalNameResolver::generateLocalSignalName("Main", SignalNameResolver::SignalType::Broadcaster);
}

/**
 * @brief Get the local signal name for skipping the SchedulerItem.
 * 
 * @return std::string The local signal name for skipping.
 */
std::string SchedulerItem::getSkipLocalSignalName() 
{
    return SignalNameResolver::generateLocalSignalName("Skip", SignalNameResolver::SignalType::Listener);
}

/**
 * @brief Get the local signal name for pausing the SchedulerItem.
 * 
 * @return std::string The local signal name for pausing.
 */
std::string SchedulerItem::getPauseLocalSignalName() 
{
    return SignalNameResolver::generateLocalSignalName("Pause", SignalNameResolver::SignalType::Listener);
}

/**
 * @brief Get the local signal names associated with the SchedulerItem (for CentralizedSignalHub).
 * 
 * @return std::vector<std::string> A vector of local signal names.
 */
std::vector<std::string> SchedulerItem::getLocalSignalNames()
{
    return 
    {
        getMainLocalSignalName(),
        getSkipLocalSignalName(),
        getPauseLocalSignalName()
    };
}

/**
 * @brief Populate the SchedulerItem from a JSON document.
 * 
 * @param doc The JSON document containing the SchedulerItem data.
 */
void SchedulerItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    this->start = Time::parse(doc["start"].as<std::string>().c_str());
    this->duration = Time::parse(doc["duration"].as<std::string>().c_str());
    this->weekday = doc["weekday"].as<std::string>();
    this->enabled = doc["enabled"].as<bool>();
    this->mode = doc["mode"].as<std::string>();
    this->skipped = doc["skipped"].as<bool>();
}

/**
 * @brief Convert the SchedulerItem to a JSON document.
 * 
 * @param doc The JSON document to populate with the SchedulerItem data.
 */
void SchedulerItem::derivedClassToJson(JsonDocument &doc)
{
    doc["start"] = getStartTime().toString();
    doc["duration"] = getDuration().toString();
    doc["weekday"] = getWeekday();
    doc["enabled"] = isEnabled();
    doc["mode"] = getMode();
    doc["skipped"] = isSkipped();
}