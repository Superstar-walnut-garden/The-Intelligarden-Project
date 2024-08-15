#ifndef PUMP_HPP
#define PUMP_HPP

#include <SPIFFS.h>
#include <string.h>
#include "Configuration.hpp"
#include "Time.hpp"
#include <Arduino.h>

class Pump
{
public:
    static Pump *getInstance();
    bool getPumpState();
    void manualSwitch(signed manualTime = -1);
    void loop(int currentHours, int currentMinutes);

private:
    enum class PumpState { off, manualOn, autoOn };
    PumpState pumpState;
    static Pump *instance;
    Time manualTime;
    bool manualOn;
    Pump();
    int getMinutesFrom(int hours, int minutes);
};

#endif // PUMP_HPP
