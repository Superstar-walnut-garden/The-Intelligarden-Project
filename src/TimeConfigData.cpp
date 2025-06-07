#include "TimeConfigData.hpp"

/**
 * @brief Construct a new Time Config Data:: Time Config Data object
 * 
 */
TimeConfigData::TimeConfigData()
    : timezone(""), ntpServer("pool.ntp.org"), timeSubsystemInitialized(false), 
      externalRTCAvailable(false), setTimeAutomatically(true), ntpUpdated(false), 
      manualTimeSetFlag(false), manualTimeEpoch(0) 
{

}

/**
 * @brief Construct a new Time Config Data:: Time Config Data object from JSON string
 * 
 * @param json JSON string containing time configuration data
 */
TimeConfigData::TimeConfigData(std::string json) 
{
    populateFromJson(json);
}

/**
 * @brief Populate the TimeConfigData object from a JSON string
 * 
 * @param json JSON string containing time configuration data
 */
void TimeConfigData::populateFromJson(std::string json)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        Serial.println("Fatal Err: Failed to parse TimeConfigData JSON");
        return;
    }
    
    timezone = doc["timezone"].as<std::string>();
    ntpServer = doc["ntpServer"].as<std::string>();
    timeSubsystemInitialized = doc["timeSubsystemInitialized"].as<bool>();
    externalRTCAvailable = doc["externalRTCAvailable"].as<bool>();
    setTimeAutomatically = doc["setTimeAutomatically"].as<bool>();
    ntpUpdated = doc["ntpUpdated"].as<bool>();
    manualTimeSetFlag = doc["manualTimeSetFlag"].as<bool>();
    manualTimeEpoch = doc["manualTimeEpoch"].as<unsigned long>();
}

/**
 * @brief Convert the TimeConfigData object to a JSON string
 * 
 * @return std::string JSON string representation of the TimeConfigData object
 */
std::string TimeConfigData::toJson()
{
    JsonDocument doc;
    doc["timezone"] = timezone;
    doc["ntpServer"] = ntpServer;
    doc["timeSubsystemInitialized"] = timeSubsystemInitialized;
    doc["externalRTCAvailable"] = externalRTCAvailable;
    doc["setTimeAutomatically"] = setTimeAutomatically;
    doc["ntpUpdated"] = ntpUpdated;
    doc["manualTimeSetFlag"] = manualTimeSetFlag;
    doc["manualTimeEpoch"] = manualTimeEpoch;

    std::string json;
    serializeJson(doc, json);
    return json;
}

/**
 * @brief Get the timezone
 * 
 * @return std::string Timezone string
 */
std::string TimeConfigData::getTimezone()
{
    return timezone;
}

/**
 * @brief Set the timezone
 * 
 * @param tz Timezone string to set
 */
void TimeConfigData::setTimezone(std::string& tz)
{
    timezone = tz;
}

/**
 * @brief Get the NTP server
 * 
 * @return std::string NTP server string
 */
std::string TimeConfigData::getNtpServer()
{
    return ntpServer;
}

/**
 * @brief Set the NTP server
 * 
 * @param server NTP server string to set
 */
void TimeConfigData::setNtpServer(std::string& server)
{
    ntpServer = server;
}

/**
 * @brief Check if the time subsystem is initialized
 * 
 * @return true if initialized, false otherwise
 */
bool TimeConfigData::isTimeSubsystemInitialized()
{
    return timeSubsystemInitialized;
}

/**
 * @brief Set the time subsystem initialized status
 * 
 * @param initialized true if initialized, false otherwise
 */
void TimeConfigData::setTimeSubsystemInitialized(bool initialized)
{
    timeSubsystemInitialized = initialized;
}

/**
 * @brief Check if external RTC is available
 * 
 * @return true if enabled, false otherwise
 */
bool TimeConfigData::isExternalRTCAvailable()
{
    return externalRTCAvailable;
}

/**
 * @brief Set the external RTC enabled status
 * 
 * @param enabled true if enabled, false otherwise
 */
void TimeConfigData::setExternalRTCAvailability(bool available)
{
    externalRTCAvailable = available;
}

/**
 * @brief Check if time should be set automatically
 * 
 * @return true if set automatically, false otherwise
 */
bool TimeConfigData::isSetTimeAutomatically()
{
    return setTimeAutomatically;
}

/**
 * @brief Set the set time automatically status
 * 
 * @param setAutomatically true if set automatically, false otherwise
 */
void TimeConfigData::setSetTimeAutomatically(bool setAutomatically)
{
    setTimeAutomatically = setAutomatically;
}

/**
 * @brief Check if NTP time is updated
 * 
 * @return true if updated, false otherwise
 */
bool TimeConfigData::isNtpUpdated()
{
    return ntpUpdated;
}

/**
 * @brief Set the NTP updated status
 * 
 * @param updated true if updated, false otherwise
 */
void TimeConfigData::setNtpUpdated(bool updated)
{
    ntpUpdated = updated;
}

/**
 * @brief Check if manual time set flag is active
 * 
 * @return true if manual time set flag is active, false otherwise
 */
bool TimeConfigData::isManualTimeSetFlag()
{
    return manualTimeSetFlag;
}

/**
 * @brief Set the manual time set flag
 * 
 * @param flag true to set the flag, false to clear it
 */
void TimeConfigData::setManualTimeSetFlag(bool flag)
{
    manualTimeSetFlag = flag;
}

/**
 * @brief Get the manual time epoch
 * 
 * @return unsigned long Manual time epoch in seconds
 */
unsigned long TimeConfigData::getManualTimeEpoch()
{
    return manualTimeEpoch;
}

/**
 * @brief Set the manual time epoch
 * 
 * @param epoch Manual time epoch in seconds to set
 */
void TimeConfigData::setManualTimeEpoch(unsigned long epoch)
{
    manualTimeEpoch = epoch;
}
