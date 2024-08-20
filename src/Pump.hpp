#ifndef PUMP_HPP
#define PUMP_HPP

#include <SPIFFS.h>
#include <string.h>
#include "Configuration.hpp"
#include "Time.hpp"
#include <Arduino.h>
#include "IObserver.hpp"
#include "SystemTime.hpp"
#include <string.h>

class Pump: public IObserver<SystemTime>
{
public:
    static Pump *getInstance();
    bool getPumpState();
    void manualSwitch(signed manualTime = -1);
    void loop(int currentHours, int currentMinutes);
    void update(SystemTime *systemTime)
    {
        auto schList = Configuration::getInstance()->getSchedulerList(); // get scheduler list from configuration
        Scheduler sch(schList);
        digitalWrite(25, sch.isAnyItemOn());
        Serial.println("pump pin is " + getPumpState() ? "on" : "off");
    }
    bool getPumpState()
    {
        return digitalRead(25);
    }

private:
    static Pump *instance;
    Pump();
};

#endif // PUMP_HPP
