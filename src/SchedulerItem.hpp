#ifndef SCHEDULERITEM_HPP
#define SCHEDULERITEM_HPP
#include "Time.hpp"
#include <ArduinoJson.h>
#include <string>

class SchedulerItem
{
    public:
    SchedulerItem(short id, short event_id, std::string name, Time start, Time duration, std::string weekday, bool enabled, bool on);
    int getId();
    int getEventId();
    void setEventId(int event_id);
    std::string getName();
    void setName(std::string name);
    Time getStartTime();
    Time getDuration();
    void setStartTime(Time start);
    void setDuration(Time duration);
    std::string getWeekday();
    bool isOn();
    bool isEnabled();
    void setEnabled(bool enabled);
    std::string toJson();

    void enable();
    void disable();
    void powerOn();
    void powerOff();
    private:
    short id, event_id;
    std::string name;
    Time start;
    Time duration;
    std::string weekday;
    bool enabled, on;
};

#endif