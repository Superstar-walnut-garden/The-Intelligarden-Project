#ifndef SCHEDULERITEM_HPP
#define SCHEDULERITEM_HPP
#include "Time.hpp"
#include <ArduinoJson.h>
#include <string>
#include "BaseItem.hpp"

class SchedulerItem: public BaseItem
{
    public:
    SchedulerItem();
    SchedulerItem(short id, short event_id, std::string name, Time start, Time duration, std::string weekday, bool enabled, bool on, std::string mode = "weekly");
    Time getStartTime();
    Time getDuration();
    void setStartTime(Time start);
    void setDuration(Time duration);
    std::string getWeekday();
    bool isEnabled();
    void setEnabled(bool enabled);
    void setMode(std::string mode);
    std::string getMode();
    
    private:
    void populateDerivedClassFromJson(JsonDocument &doc) override;
    void derivedClassToJson(JsonDocument &doc) override;

    std::string mode;
    Time start;
    Time duration;
    std::string weekday;
    bool enabled;
};

#endif