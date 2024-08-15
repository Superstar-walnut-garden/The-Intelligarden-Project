#ifndef SYSTEMTIME_HPP
#define SYSTEMTIME_HPP

#include <ESP32Time.h>
#include <time.h>
#include <NTPClient.h>
#include <rom/rtc.h>
#include <SPIFFS.h>
#include <WiFiUdp.h>
#include <Arduino.h>

class SystemTime
{
public:
    static SystemTime *getInstance();

    void obtainTime();
    bool isTimeUpdated();
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
