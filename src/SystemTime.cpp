#include "SystemTime.hpp"


// Initialize the static member variable
SystemTime *SystemTime::instance = nullptr;

// Singleton instance getter
SystemTime *SystemTime::getInstance()
{
    if (!instance)
        instance = new SystemTime();
    return instance;
}

// Constructor
SystemTime::SystemTime() 
: rtc(0), timeClient(ntpUDP), timeUpdated(false), externalRTCEnabled(false), setTimeAutomatically(true), ntpUpdated(false)
{
    if(externalRTC.begin()) // try to initialize module
    {
        Serial.println("External RTC module found!");
        Serial.println("External RTC sub-system initialized successfully!");
        externalRTCEnabled = true;
        DateTime now = externalRTC.now();
        rtc.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), now.year());
        timeUpdated = true;
    }
}

// Obtain time from NTP server and update the RTC
void SystemTime::obtainTime()
{
    if(setTimeAutomatically and !ntpUpdated)
    {
        const char* ntpServer = "pool.ntp.org";
        const long  gmtOffset_sec = 12600;
        const int   daylightOffset_sec = 0;
        Serial.println("Obtaining time from NTP server...");
        WiFiClient client;
        if(client.connect(ntpServer, 123)) // check if ntp server is reachable
        {
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
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
                struct tm timeinfo;
                getLocalTime(&timeinfo);
                Serial.println("Time obtained successfully!");
                Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
                Serial.println("-----------------------");
                timeUpdated = true;
                ntpUpdated = true;
                if(externalRTCEnabled)
                {
                    externalRTC.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
                    Serial.println("External RTC sub-system updated successfully!");
                }
            }
            else
            {
                if(externalRTCEnabled) // resync with external RTC if available
                {
                    DateTime now = externalRTC.now();
                    rtc.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), now.year());
                }
                Serial.println("Failed to obtain time");
                lostTrackOfTime();
            }
        }
        else
        {
            Serial.println("Failed to connect to NTP server");
            lostTrackOfTime();
        }
    }
}

// Check if time was updated successfully
bool SystemTime::isTimeUpdated()
{
    return timeUpdated;
}

// Get the current minute
short SystemTime::getMinute()
{
    return rtc.getMinute();
}

// Get the current hour
short SystemTime::getHour()
{
    return rtc.getHour(true);
}

// Get the current day
short SystemTime::getDay()
{
    return rtc.getDay();
}

// Get the current month
short SystemTime::getMonth()
{
    return rtc.getMonth() + 1;
}

// Get the current year
short SystemTime::getYear()
{
    return rtc.getYear();
}

// Handle time update failure
void SystemTime::lostTrackOfTime()
{
    Serial.println("Error: Couldn't reach time server!!!");
    Serial.printf("reset reason core 0 code: %d\n", rtc_get_reset_reason(0));
    Serial.printf("reset reason core 1 code: %d\n", rtc_get_reset_reason(1));
    if (!(rtc_get_reset_reason(0) == 0xc || rtc_get_reset_reason(1) == 0xc)) // if reset cause is not from the watchdog
    {
        if(!externalRTCEnabled) // if there is no external rtc module
        {
            Serial.println("reset cause is power failure and the internal rtc is reset");
            timeUpdated = false;
            Serial.println("Warning: Lost track of time. Time may be inaccurate!");
            //rtc.setTime(0, 0, 5, 1, 1, 2024); // estimate time.
        }
    }
}
