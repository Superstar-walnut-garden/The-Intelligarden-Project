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
    Time getStartTime() const;
    Time getDuration() const;
    void setStartTime(Time start);
    void setDuration(Time duration);
    std::string getWeekday() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    void setMode(std::string mode);
    std::string getMode() const;
    void setSkipped(bool skipped);
    bool isSkipped() const;

    std::string getMainLocalSignalName() const;
    std::string getSkipLocalSignalName() const;
    std::string getPauseLocalSignalName() const;
    std::vector<std::string> getLocalSignalNames() override;

    private:
    void populateDerivedClassFromJson(JsonDocument &doc) override;
    void derivedClassToJson(JsonDocument &doc) const override;

    std::string mode;
    Time start;
    Time duration;
    std::string weekday;
    bool enabled;
    short skipEvent_id;
    bool skipped;
};
