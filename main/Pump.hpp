#ifndef PUMP_HPP
#define PUMP_HPP
#include <SPIFFS.h>
#include <string.h>
#include "Configuration.hpp"


class Pump
{
    private:
    enum class PumpState {off, manualOn, autoOn};
    PumpState pumpState;
    static Pump *instance;
    Time manualTime;
    bool manualOn;
    Pump(): pumpState(PumpState::off), manualOn(false), manualTime(0, 0)
    {
        
    }
    int getMinutesFrom(int hours, int minutes)
    {
        return (hours * 60) + minutes;
    }

    public:
    static Pump *getInstance()
    {
        if(!instance)
            instance = new Pump();
        return instance;
    }
    
    bool getPumpState()
    {
        return (pumpState != PumpState::off);
    }
    void manualSwitch(signed manualTime = -1)
    {
        if(pumpState != PumpState::autoOn)
        {
            if(manualTime >= 0)
            {
                this->manualTime = Time(0, manualTime);
                manualOn = true;
            }
            else if(manualTime == -1)
            {
                manualOn = false;
                this->manualTime = Time(0, 0);
                pumpState = PumpState::off;
            }
        }
    }
    void loop(int currentHours, int currentMinutes)
    {
        auto *cfg = Configuration::getInstance();
        auto pSchedule = cfg->getPumpSchedule();
        auto untilTime = pSchedule.start + pSchedule.duration;
        auto currentTime = Time(currentHours, currentMinutes);
        
        Serial.println("pump start time: " + String(pSchedule.start.getHour()) + ":" + String(pSchedule.start.getMinute()));
        Serial.println("pump end time: " + String(untilTime.getHour()) + ":" + String(untilTime.getMinute()));
        Serial.println("pump duration: " + String(pSchedule.duration.getHour()) + ":" + String(pSchedule.duration.getMinute()));

        Serial.println("checking if the schedule is ready");
        if(pumpState != PumpState::manualOn) // don't care about the schedule if manual mode is selected.
        {
            if(currentTime >= pSchedule.start and currentTime <= untilTime)
            {
                Serial.println("Pump is on (with schedule)");
                pumpState = PumpState::autoOn;
            }
            else
            {
                Serial.println("Pump is off (with schedule)");
                pumpState = PumpState::off;
            }
        }

        if(manualOn) // manual command flag
        {
            manualOn = 0; // drop the flag
            manualTime += currentTime;
        }
        if(currentTime < manualTime) // manual time check
        {
            Serial.println("Pump is manually on");
            pumpState = PumpState::manualOn;
        }
        else
        {
            Serial.println("Pump is manually off");
            manualTime = Time(0, 0); // reset the manual time
            if(pumpState == PumpState::manualOn)
                pumpState = PumpState::off;
        }
    }
};
Pump *Pump::instance = nullptr;

#endif