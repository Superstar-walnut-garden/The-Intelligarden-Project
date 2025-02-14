#ifndef SCHEDULERITEM_HPP
#define SCHEDULERITEM_HPP
#include "Time.hpp"
#include <ArduinoJson.h>
#include <string>
#include "BaseItem.hpp"

class SchedulerItem: public BaseItem
{
    public:
    SchedulerItem(short id, short event_id, std::string name, Time start, Time duration, std::string weekday, bool enabled, bool on);
    Time getStartTime();
    Time getDuration();
    void setStartTime(Time start);
    void setDuration(Time duration);
    std::string getWeekday();
    bool isOn();
    bool isEnabled();
    void setEnabled(bool enabled);
    std::string toJson();
    
    void powerOn(); // same as setStatus(true) for backward compatibility
    void powerOff(); // same as setStatus(fase) for backward compatibility
    private:
    Time start;
    Time duration;
    std::string weekday;
    bool enabled;
};

#endif