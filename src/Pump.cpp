#include "Pump.hpp"

Pump* Pump::instance = nullptr;

Pump::Pump() : pumpState(PumpState::off), manualOn(false), manualTime(0, 0)
{
}

Pump* Pump::getInstance()
{
    if (!instance)
        instance = new Pump();
    return instance;
}

bool Pump::getPumpState()
{
    return (pumpState != PumpState::off);
}

void Pump::manualSwitch(signed manualTime)
{
    if (pumpState != PumpState::autoOn)
    {
        if (manualTime >= 0)
        {
            this->manualTime = Time(0, manualTime);
            manualOn = true;
        }
        else if (manualTime == -1)
        {
            manualOn = false;
            this->manualTime = Time(0, 0);
            pumpState = PumpState::off;
        }
    }
}

void Pump::loop(int currentHours, int currentMinutes)
{
    auto *cfg = Configuration::getInstance();
    auto pSchedule = cfg->getPumpSchedule();
    auto untilTime = pSchedule.start + pSchedule.duration;
    auto currentTime = Time(currentHours, currentMinutes);

    Serial.println("pump start time: " + String(pSchedule.start.getHour()) + ":" + String(pSchedule.start.getMinute()));
    Serial.println("pump end time: " + String(untilTime.getHour()) + ":" + String(untilTime.getMinute()));
    Serial.println("pump duration: " + String(pSchedule.duration.getHour()) + ":" + String(pSchedule.duration.getMinute()));

    Serial.println("checking if the schedule is ready");
    if (pumpState != PumpState::manualOn) // don't care about the schedule if manual mode is selected.
    {
        if (currentTime >= pSchedule.start && currentTime <= untilTime)
        {
            digitalWrite(25, HIGH);
            Serial.println("Pump is on (with schedule)");
            pumpState = PumpState::autoOn;
        }
        else
        {
            digitalWrite(25, LOW);
            Serial.println("Pump is off (with schedule)");
            pumpState = PumpState::off;
        }
    }

    if (manualOn) // manual command flag
    {
        manualOn = false; // drop the flag
        manualTime += currentTime;
    }
    if (currentTime < manualTime) // manual time check
    {
        digitalWrite(25, HIGH);
        Serial.println("Pump is manually on");
        pumpState = PumpState::manualOn;
    }
    else
    {
        Serial.println("Pump is manually off");
        manualTime = Time(0, 0); // reset the manual time
        if (pumpState == PumpState::manualOn)
        {
            pumpState = PumpState::off;
            digitalWrite(25, LOW);
        }
    }
}

int Pump::getMinutesFrom(int hours, int minutes)
{
    return (hours * 60) + minutes;
}
