#include "Pump.hpp"

Pump* Pump::instance = nullptr;

Pump::Pump()
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
    return digitalRead(25);
}
void Pump::update(SystemTime *systemTime)
{
    auto schList = Configuration::getInstance()->getSchedulerList(); // get scheduler list from configuration
    Scheduler sch(schList);
    digitalWrite(25, sch.isAnyItemOn());
    Serial.println("pump pin is " + getPumpState() ? "on" : "off");
}