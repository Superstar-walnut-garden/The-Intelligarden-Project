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
SystemTime::SystemTime() : rtc(0), timeClient(ntpUDP), timeUpdated(false)
{
}

// Obtain time from NTP server and update the RTC
void SystemTime::obtainTime()
{
    Serial.println("Connecting to time server!");
    Serial.println("time offset: 3:30 GMT");
    timeClient.setTimeOffset(12600);
    timeClient.begin();
    Serial.print("Retrieving time and date...  ");
    timeUpdated = true;
    while (!timeClient.update())
    {
        if (timeClient.forceUpdate())
        {
            timeUpdated = true;
            break;
        }
        else
        {
            Serial.println("Failed!");
            timeUpdated = false;
            static int localCounter = 0;
            if (localCounter++ > 5)
            {
                localCounter = 0;
                Serial.println("Fatal Error: can't reach NTP server. Check your internet connection.");
                lostTrackOfTime();
                break;
            }
        }
    }
    if (isTimeUpdated())
    {
        Serial.println("Passed!");
        auto hour = timeClient.getHours();
        auto minute = timeClient.getMinutes();
        time_t c = timeClient.getEpochTime();
        Serial.println(ctime(&c));
        struct tm *timeinfo = localtime(&c);
        auto day = timeinfo->tm_mday;
        auto month = timeinfo->tm_mon + 1;
        auto year = timeinfo->tm_year + 1900;
        Serial.println(("year = " + std::to_string(year)).c_str());
        Serial.println(("month = " + std::to_string(month)).c_str());
        Serial.println(("day = " + std::to_string(day)).c_str());
        rtc.setTime(0, minute, hour, day, month, year);
        Serial.println("RTC sub-system updated successfully!");
        Serial.print("HOUR: ");
        Serial.println(hour);
        Serial.print("MINUTE: ");
        Serial.println(minute);
    }
    else
    {
        lostTrackOfTime();
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
    return rtc.getMonth();
}

// Get the current year
short SystemTime::getYear()
{
    return rtc.getYear();
}

// Handle time update failure
void SystemTime::lostTrackOfTime()
{
    Serial.println("Error: cannot connect to the network!!!");
    Serial.printf("reset reason core 0 code: %d\n", rtc_get_reset_reason(0));
    Serial.printf("reset reason core 1 code: %d\n", rtc_get_reset_reason(1));
    if (!(rtc_get_reset_reason(0) == 0xc || rtc_get_reset_reason(1) == 0xc)) // if reset cause is not from the watchdog
    {
        Serial.println("reset reason is not from watchdog");
        timeUpdated = false;
        Serial.println("Warning: Lost track of time. Time may be inaccurate!");
        rtc.setTime(0, 0, 5, 1, 1, 2024);
    }
}
