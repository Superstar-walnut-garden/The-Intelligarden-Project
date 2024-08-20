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
#include <Arduino.h>

class SystemTime: public Subject<SystemTime>
{
public:
    static SystemTime *getInstance();

    void obtainTime();
    bool isTimeUpdated();
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
    short getMinute();
    short getHour();
    short getDay();
    short getMonth();
    short getYear();

private:
    SystemTime();
    void lostTrackOfTime();

    // NTP Client to get time
    WiFiUDP ntpUDP;
    NTPClient timeClient;
    // Date and time variables
    String formattedDate;
    String dayStamp;
    String timeStamp;
    bool timeUpdated;
    ESP32Time rtc;

    static SystemTime *instance;
};

#endif // SYSTEMTIME_HPP
