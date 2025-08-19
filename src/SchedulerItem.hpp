#pragma once

#include "Time.hpp"
#include <ArduinoJson.h>
#include <string>
#include "SignalCompatibleBaseItem.hpp"
#include "SignalNameResolver.hpp"

class SchedulerItem: public SignalCompatibleBaseItem
{
    public:
    SchedulerItem();
    SchedulerItem(uint64_t id, std::string name, Time start, Time duration, std::string weekday, bool enabled, bool on, std::string mode = "weekly", bool skipped = false);
    Time getStartTime();
    Time getDuration();
    void setStartTime(Time start);
    void setDuration(Time duration);
    std::string getWeekday();
    bool isEnabled();
    void setEnabled(bool enabled);
    void setMode(std::string mode);
    std::string getMode();
    void setSkipped(bool skipped);
    bool isSkipped();

    std::string getMainLocalSignalName();
    std::string getSkipLocalSignalName();
    std::string getPauseLocalSignalName();
    std::vector<std::string> getLocalSignalNames() override;

    private:
    void populateDerivedClassFromJson(JsonDocument &doc) override;
    void derivedClassToJson(JsonDocument &doc) override;

    std::string mode;
    Time start;
    Time duration;
    std::string weekday;
    bool enabled;
    short skipEvent_id;
    bool skipped;
};
