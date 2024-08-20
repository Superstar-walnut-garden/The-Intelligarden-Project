#ifndef SCHEDULERITEM_HPP
#define SCHEDULERITEM_HPP
#include "Time.hpp"
#include <ArduinoJson.h>
#include <string>

class SchedulerItem
{
    public:
    SchedulerItem(short id, Time start, Time duration, std::string weekday, bool situation, bool currentStatus);
    int getId();
    Time getStartTime();
    Time getDuration();
    std::string getWeekday();
    bool isOn();
    bool isEnabled();
    std::string toJson();

    void enable();
    void disable();
    void powerOn();
    void powerOff();
    private:
    short id;
    Time start;
    Time duration;
    std::string weekday;
    bool enabled, on;
};

#endif