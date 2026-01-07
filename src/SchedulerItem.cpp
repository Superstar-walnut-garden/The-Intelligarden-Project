#include "SchedulerItem.hpp"

/**
 * @brief Construct a new SchedulerItem object.
 * 
 */
SchedulerItem::SchedulerItem() 
    : SignalCompatibleBaseItem(), start(Time(0, 0)), duration(Time(0, 0)), weekday("0000000"), enabled(false), mode(""), skipped(false) 
{
    registerToJsonCallback([this](JsonDocument &json) -> void
    {
        json["start"] = getStartTime().toString();
        json["duration"] = getDuration().toString();
        json["weekday"] = getWeekday();
        json["enabled"] = isEnabled();
        json["mode"] = getMode();
        json["skipped"] = isSkipped();
    });
    registerFromJsonCallback([this](JsonDocument &json) -> void
    {
        this->start = Time::parse(json["start"].as<std::string>().c_str());
        this->duration = Time::parse(json["duration"].as<std::string>().c_str());
        this->weekday = json["weekday"].as<std::string>();
        this->enabled = json["enabled"].as<bool>();
        this->mode = json["mode"].as<std::string>();
        this->skipped = json["skipped"].as<bool>();
    });
}

/**
 * @brief Get the start time of the SchedulerItem.
 * 
 * @return Time 
 */
Time SchedulerItem::getStartTime() const
{
    return start;
}

/**
 * @brief Get the duration of the SchedulerItem.
 * 
 * @return Time 
 */
Time SchedulerItem::getDuration() const
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
std::string SchedulerItem::getWeekday() const
{
    return weekday;
}

/**
 * @brief Set the weekday of the SchedulerItem.
 * 
 * @param weekday The weekday to set as a binary string.
 */
bool SchedulerItem::isEnabled()  const
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
std::string SchedulerItem::getMode() const
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
bool SchedulerItem::isSkipped() const
{
    return skipped;
}

/**
 * @brief Get the main local signal name for the SchedulerItem.
 * 
 * @return std::string The main local signal name.
 */
std::string SchedulerItem::getMainLocalSignalName() const
{
    return SignalNameResolver::generateLocalSignalName("Main", SignalNameResolver::SignalType::Broadcaster);
}

/**
 * @brief Get the local signal name for skipping the SchedulerItem.
 * 
 * @return std::string The local signal name for skipping.
 */
std::string SchedulerItem::getSkipLocalSignalName() const
{
    return SignalNameResolver::generateLocalSignalName("Skip", SignalNameResolver::SignalType::Listener);
}

/**
 * @brief Get the local signal name for pausing the SchedulerItem.
 * 
 * @return std::string The local signal name for pausing.
 */
std::string SchedulerItem::getPauseLocalSignalName() const
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
