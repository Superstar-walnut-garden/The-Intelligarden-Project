#ifndef PUMP_HPP
#define PUMP_HPP
#include <SPIFFS.h>
#include <string.h>
#include "Configuration.hpp"
#include "SystemTime.hpp"


class Pump
{
    private:
    static Pump *instance;
    Pump()
    {
        
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
        return digitalRead(25);
    }
    void update(SystemTime *systemTime)
    {
        auto schList = Configuration::getInstance()->getSchedulerList(); // get scheduler list from configuration
        Scheduler sch(schList);
        digitalWrite(25, sch.isAnyItemOn());
        Serial.println("pump pin is " + getPumpState() ? "on" : "off");
    }
};
Pump *Pump::instance = nullptr;

#endif