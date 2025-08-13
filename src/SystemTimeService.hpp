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
#include <RTClib.h>
#include <chrono>
#include "esp_sntp.h"
#include <WiFiClient.h>
#include "SystemTimeConfig.hpp"

class SystemTimeService: public Subject<SystemTimeService>
{
public:
    static SystemTimeService *getInstance();

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
        int weekbit = (1 << rtc.getDayofWeek());
        return weekbit;
    }
    bool isCurrentWeekdayPresentIn(int selectedWeekdays)
    {
        if(getWeekday() & selectedWeekdays) // check using: bit-and
            return true;
        return false;
    }
    static int parseWeekday(const std::string& weekdayString)
    {   
        int index = 0;
        uint8_t parsedWeekday = 0;
        for(auto &letter : weekdayString)
        {
            if(letter == '1')
                parsedWeekday |= (1 << index);
            index++;
        }
        return parsedWeekday;
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

    std::string getConfig();
    void setConfig(const std::string& configJson);

private:
    SystemTimeService();
    void lostTrackOfTime();
    void saveState();
    void loadState();
    int getTimezoneOffset();

    // Date and time variables
    String formattedDate;
    String dayStamp;
    String timeStamp;
    ESP32Time rtc;
    RTC_DS1307 externalRTC;
    SystemTimeConfig currentConfigData;

    static SystemTimeService *instance;
};

#endif // SYSTEMTIME_HPP
