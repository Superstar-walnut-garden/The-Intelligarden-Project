#include "SystemTimeConfig.hpp"

/**
 * @brief Construct a new Time Config Data:: Time Config Data object
 * 
 */
SystemTimeConfig::SystemTimeConfig()
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
SystemTimeConfig::SystemTimeConfig(std::string json) 
{
    populateFromJson(json);
}

/**
 * @brief Populate the SystemTimeConfig object from a JSON string
 * 
 * @param json JSON string containing time configuration data
 */
void SystemTimeConfig::populateFromJson(std::string json)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        Serial.println("Fatal Err: Failed to parse SystemTimeConfig JSON");
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
 * @brief Convert the SystemTimeConfig object to a JSON string
 * 
 * @return std::string JSON string representation of the SystemTimeConfig object
 */
std::string SystemTimeConfig::toJson()
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
std::string SystemTimeConfig::getTimezone()
{
    return timezone;
}

/**
 * @brief Set the timezone
 * 
 * @param tz Timezone string to set
 */
void SystemTimeConfig::setTimezone(std::string& tz)
{
    timezone = tz;
}

/**
 * @brief Get the NTP server
 * 
 * @return std::string NTP server string
 */
std::string SystemTimeConfig::getNtpServer()
{
    return ntpServer;
}

/**
 * @brief Set the NTP server
 * 
 * @param server NTP server string to set
 */
void SystemTimeConfig::setNtpServer(std::string& server)
{
    ntpServer = server;
}

/**
 * @brief Check if the time subsystem is initialized
 * 
 * @return true if initialized, false otherwise
 */
bool SystemTimeConfig::isTimeSubsystemInitialized()
{
    return timeSubsystemInitialized;
}

/**
 * @brief Set the time subsystem initialized status
 * 
 * @param initialized true if initialized, false otherwise
 */
void SystemTimeConfig::setTimeSubsystemInitialized(bool initialized)
{
    timeSubsystemInitialized = initialized;
}

/**
 * @brief Check if external RTC is available
 * 
 * @return true if enabled, false otherwise
 */
bool SystemTimeConfig::isExternalRTCAvailable()
{
    return externalRTCAvailable;
}

/**
 * @brief Set the external RTC enabled status
 * 
 * @param enabled true if enabled, false otherwise
 */
void SystemTimeConfig::setExternalRTCAvailability(bool available)
{
    externalRTCAvailable = available;
}

/**
 * @brief Check if time should be set automatically
 * 
 * @return true if set automatically, false otherwise
 */
bool SystemTimeConfig::isSetTimeAutomatically()
{
    return setTimeAutomatically;
}

/**
 * @brief Set the set time automatically status
 * 
 * @param setAutomatically true if set automatically, false otherwise
 */
void SystemTimeConfig::setSetTimeAutomatically(bool setAutomatically)
{
    setTimeAutomatically = setAutomatically;
}

/**
 * @brief Check if NTP time is updated
 * 
 * @return true if updated, false otherwise
 */
bool SystemTimeConfig::isNtpUpdated()
{
    return ntpUpdated;
}

/**
 * @brief Set the NTP updated status
 * 
 * @param updated true if updated, false otherwise
 */
void SystemTimeConfig::setNtpUpdated(bool updated)
{
    ntpUpdated = updated;
}

/**
 * @brief Check if manual time set flag is active
 * 
 * @return true if manual time set flag is active, false otherwise
 */
bool SystemTimeConfig::isManualTimeSetFlag()
{
    return manualTimeSetFlag;
}

/**
 * @brief Set the manual time set flag
 * 
 * @param flag true to set the flag, false to clear it
 */
void SystemTimeConfig::setManualTimeSetFlag(bool flag)
{
    manualTimeSetFlag = flag;
}

/**
 * @brief Get the manual time epoch
 * 
 * @return unsigned long Manual time epoch in seconds
 */
unsigned long SystemTimeConfig::getManualTimeEpoch()
{
    return manualTimeEpoch;
}

/**
 * @brief Set the manual time epoch
 * 
 * @param epoch Manual time epoch in seconds to set
 */
void SystemTimeConfig::setManualTimeEpoch(unsigned long epoch)
{
    manualTimeEpoch = epoch;
}
