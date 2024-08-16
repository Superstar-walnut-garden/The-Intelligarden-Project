#ifndef SYSTEMTIME_HPP
#define SYSTEMTIME_HPP
#include <ESP32Time.h>
#include <time.h>
#include <NTPClient.h>
#include <rom/rtc.h>
#include <SPIFFS.h>
#include <WiFiUdp.h>
#include "Subject.hpp"
#include <bitset>
#include "Time.hpp"

class SystemTime: public Subject<SystemTime>
{
    public:
    static SystemTime *getInstance();
    
    void obtainTime()
    {

        Serial.println("Connecting to time server!");
        Serial.println("time offset: 3:30 GMT");
        timeClient.setTimeOffset(12600);
        timeClient.begin();
        Serial.print("Retriving time and date...  ");
            //timeClient.update();
        timeUpdated = true;
        while(!timeClient.update())
        {
            if(timeClient.forceUpdate())
            {
                timeUpdated = true;
                break;
            }
            else
            {
                Serial.println("Failed!");
                timeUpdated = false;
                static int localCounter = 0;
                if(localCounter ++ > 5)
                {
                    localCounter = 0;
                    Serial.println("Fatal Error: can't reach NTP server. Check your internet connection.");
                    lostTrackOfTime();
                    break;
                }
            }
        }
        if(isTimeUpdated())
        {
            Serial.println("Passed!");
            auto hour = timeClient.getHours();
            auto minute = timeClient.getMinutes();
            time_t c;
            c = timeClient.getEpochTime();
            Serial.println(ctime(&c));
            struct tm* timeinfo = localtime(&c);
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
            notify();
        }
        else
        {
            lostTrackOfTime();
        }
    }
    void notifierEngine()
    {
        static auto previousTime = getTime();
        auto newTime = getTime();
        if(previousTime != newTime) // if time is changed (i.e. this happens every minute)
        {
            notify();
            previousTime = newTime;
        }
    }
    bool isTimeUpdated()
    {
        return timeUpdated;
    }
    std::string getWeekdayString()
    {
        auto weekday = rtc.getDayofWeek();
        std::string weekdayStr;
        for (int i = 0; i < 7; i++)
            if(i == weekday)
                weekdayStr.push_back('1');
            else
                weekdayStr.push_back('0');
        return weekdayStr;
    }
    int getWeekday()
    {
        return rtc.getDayofWeek();
    }
    bool isCurrentWeekdayPresentIn(int selectedWeekdays)
    {
        if(getWeekday() & selectedWeekdays) // check using: bit-and
            return true;
        return false;
    }
    static int parseWeekday(const std::string& weekdayString)
    {
        std::bitset<7> bits(weekdayString);
        return static_cast<int>(bits.to_ulong());
    }
    Time getTime()
    {
        return Time(getHour(), getMinute());
    }
    short getMinute()
    {
        return rtc.getMinute();
    }
    short getHour()
    {
        return rtc.getHour(true);
    }
    short getDay()
    {
        return rtc.getDay();
    }
    short getMonth()
    {
        return rtc.getMonth();
    }
    short getYear()
    {
        return rtc.getYear();
    }

    private:
    // Define NTP Client to get time
    WiFiUDP ntpUDP;
    NTPClient timeClient;
    // Variables to save date and time
    String formattedDate;
    String dayStamp;
    String timeStamp;
    bool timeUpdated;
    ESP32Time rtc;

    void lostTrackOfTime()
    {
        Serial.println("Error: cannot connect to the network!!!");
        Serial.printf("reset reason core 0 code: %d\n", rtc_get_reset_reason(0));
        Serial.printf("reset reason core 1 code: %d\n", rtc_get_reset_reason(1));
        if(!(rtc_get_reset_reason(0) == 0xc or rtc_get_reset_reason(1) == 0xc)) // if reset cause is not from the softdog
        {
            Serial.println("reset reason is not from soft dog");
            timeUpdated = false;
            Serial.println("Warning: Lost track of time. Time may be inacccurate!");
            rtc.setTime(0, 0, 5, 1, 1, 2024);
        }
    }
    SystemTime(): rtc(0), timeClient(ntpUDP), timeUpdated(false)
    {

    }
    static SystemTime *instance;
};

#endif