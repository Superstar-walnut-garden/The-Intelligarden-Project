#include "SystemTime.hpp"
#include "Configuration.hpp"

/**
 * @brief instance initialization for the singleton pattern
 * 
 */
SystemTime *SystemTime::instance = nullptr;

/**
 * @brief get the singleton instance of SystemTime.
 * 
 * @return SystemTime* 
 */
SystemTime *SystemTime::getInstance()
{
    if (!instance)
        instance = new SystemTime();
    return instance;
}

/**
 * @brief Construct a new SystemTime::SystemTime object
 * 
 */
SystemTime::SystemTime() 
: rtc(0)
{
    loadState(); // load the saved configuraion state from SPIFFS
    // set realtime variables startup default values
    currentConfigData.setExternalRTCAvailability(false);
    currentConfigData.setTimeSubsystemInitialized(false);
    currentConfigData.setNtpUpdated(false);
    currentConfigData.setManualTimeSetFlag(false);

    if(externalRTC.begin()) // try to initialize module
    {
        Serial.println("External RTC module found!");
        Serial.println("External RTC sub-system initialized successfully!");
        currentConfigData.setExternalRTCAvailability(true);
        DateTime now = externalRTC.now();
        rtc.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), now.year());
        setenv("TZ", currentConfigData.getTimezone().c_str(), 1); // Set time zone
        tzset(); // Apply time zone
        currentConfigData.setTimeSubsystemInitialized(true);
    }
}

/**
 * @brief try to obtain the current time from an NTP server.
 * 
 */
void SystemTime::obtainTime()
{
    auto& cfg = currentConfigData;
    if(cfg.isSetTimeAutomatically() and !cfg.isNtpUpdated())
    {
        auto ntpServer = currentConfigData.getNtpServer();
        Serial.println("Obtaining time from NTP server...");
        WiFiClient client;
        configTime(0, 0, ntpServer.c_str()); // get utc time
        int attempt = 0;
        bool ntpSuccess = true;
        // Wait for the SNTP sync status to be completed
        while(!sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED)
        {
            if (attempt++ > 15) // wait for 5 seconds max
            {
                Serial.println("Err: Timeout while waiting for NTP sync status.");
                ntpSuccess = false;
                sntp_stop();  // Stops automatic retries
                break;
            }
            delay(500); // wait for 0.5 second before checking again
        }
        if (ntpSuccess)
        {
            setenv("TZ", "IRST-3:30", 1); // Set time zone
            tzset(); // Apply time zone
            struct tm timeinfo;
            getLocalTime(&timeinfo);
            Serial.println("Time obtained successfully!");
            Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
            Serial.println("-----------------------");
            cfg.setTimeSubsystemInitialized(true);
            cfg.setNtpUpdated(true);
            if(cfg.isExternalRTCAvailable())
            {
                time_t now;
                struct tm *timeinfo;
                time(&now); // Get current system time (epoch)
                timeinfo = gmtime(&now); // Convert to UTC time structure
                externalRTC.adjust(DateTime(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec));
                Serial.println("External RTC sub-system updated successfully!");
            }
        }
        else
        {
            if(cfg.isExternalRTCAvailable()) // resync with external RTC if available
            {
                DateTime now = externalRTC.now();
                rtc.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), now.year());
                setenv("TZ", cfg.getTimezone().c_str(), 1); // Set time zone
                tzset(); // Apply time zone
            }
            Serial.println("Failed to obtain time");
            lostTrackOfTime();
        }
    }
}

/**
 * @brief check if the time has been updated from ntp server, internal rtc or external rtc successfully.
 * 
 * @return true 
 * @return false 
 */
bool SystemTime::isTimeUpdated()
{
    return currentConfigData.isTimeSubsystemInitialized();
}

/**
 * @brief get current minute.
 * 
 * @return short 
 */
short SystemTime::getMinute()
{
    return rtc.getMinute();
}

/**
 * @brief get current hour in 24-hour format.
 * 
 * @return short 
 */
short SystemTime::getHour()
{
    return rtc.getHour(true);
}

/**
 * @brief Get the current day of the month.
 * 
 * @return short 
 */
short SystemTime::getDay()
{
    return rtc.getDay();
}

/**
 * @brief Get the current month (1-12).
 * 
 * @return short 
 */
short SystemTime::getMonth()
{
    return rtc.getMonth() + 1;
}

/**
 * @brief Get the current year (e.g. 2024).
 * 
 * @return short 
 */
short SystemTime::getYear()
{
    return rtc.getYear();
}

/**
 * @brief Handle the case when the system loses track of time.
 * This function is called when the system fails to reach the time server or loses power.
 * It resets the timeUpdated flag and prints an error message.
 */
void SystemTime::lostTrackOfTime()
{
    Serial.println("Error: Couldn't reach time server!!!");
    Serial.printf("reset reason core 0 code: %d\n", rtc_get_reset_reason(0));
    Serial.printf("reset reason core 1 code: %d\n", rtc_get_reset_reason(1));
    if (!(rtc_get_reset_reason(0) == 0xc || rtc_get_reset_reason(1) == 0xc)) // if reset cause is not from the watchdog
    {
        if(!currentConfigData.isExternalRTCAvailable()) // if there is no external rtc module
        {
            Serial.println("reset cause is power failure and the internal rtc is reset");
            currentConfigData.setTimeSubsystemInitialized(false);
            Serial.println("Warning: Lost track of time. Time may be inaccurate!");
            //rtc.setTime(0, 0, 5, 1, 1, 2024); // estimate time.
        }
    }
}

/**
 * @brief Get the current configuration of the system time (for web-api purposes).
 * 
 * @return std::string 
 */
std::string SystemTime::getConfig()
{
    return currentConfigData.toJson();
}

/**
 * @brief Set the configuration of the system time (for web-api purposes).
 * 
 * @param configJson 
 */
void SystemTime::setConfig(const std::string& configJson)
{
    auto newConfig = TimeConfigData(configJson);
    // keeping realtime variables intact to prevent wrong
    newConfig.setTimeSubsystemInitialized(currentConfigData.isTimeSubsystemInitialized());
    newConfig.setExternalRTCAvailability(currentConfigData.isExternalRTCAvailable());
    newConfig.setNtpUpdated(currentConfigData.isNtpUpdated());
    setenv("TZ", newConfig.getTimezone().c_str(), 1); // Set time zone
    tzset(); // Apply time zone
    if(newConfig.isManualTimeSetFlag())
    {
        newConfig.setManualTimeSetFlag(false); // drop the flag
        if(!newConfig.isSetTimeAutomatically())
        {
            auto localTimeEpoch = newConfig.getManualTimeEpoch();
            struct timeval tv;
            tv.tv_sec = localTimeEpoch - getTimezoneOffset(); // Convert to UTC and set seconds (epoch time)
            tv.tv_usec = 0; // Microseconds (not needed)
            settimeofday(&tv, NULL); // Apply local time without affecting timezone config
            if(newConfig.isExternalRTCAvailable()) // if external RTC is available
            {
                time_t now;
                struct tm *timeinfo;
                time(&now); // Get current system time (epoch)
                timeinfo = gmtime(&now); // Convert to UTC time structure
                externalRTC.adjust(DateTime(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec));
                Serial.println("External RTC sub-system updated successfully!");
            }
            newConfig.setTimeSubsystemInitialized(true); // set the time subsystem initialized flag
            Serial.println("Manual time set successfully!");
        }
    }
    currentConfigData = newConfig; // update the time configuration data
    saveState();
}

/**
 * @brief Save the current state of the system time to internal storage.
 * This function is called to save the current time and configuration to persistent storage.
 */
void SystemTime::saveState()
{
    Configuration::getInstance()->setTimeConfig(currentConfigData.toJson());   
}

/**
 * @brief load the saved state of the system time from internal storage.
 * This function is called to load the previously saved time and configuration from persistent storage.
 */
void SystemTime::loadState()
{
    auto state = Configuration::getInstance()->getTimeConfig();
    if (state.empty())
        return;
    currentConfigData.populateFromJson(state);
}

/**
 * @brief calculate the timezone offset in seconds.
 * This function calculates the timezone offset by comparing the local time and UTC time (i.e. it depends on system's timezone configuration).
 */
int SystemTime::getTimezoneOffset() 
{
    time_t now = time(NULL);
    struct tm utc_tm, local_tm;

    gmtime_r(&now, &utc_tm);  // Get UTC time structure
    localtime_r(&now, &local_tm);  // Get Local time structure

    int offsetSeconds = difftime(mktime(&local_tm), mktime(&utc_tm)); // Correct offset calculation
    return offsetSeconds;

}
