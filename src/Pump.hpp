#ifndef PUMP_HPP
#define PUMP_HPP
#include <SPIFFS.h>
#include <string.h>
#include "Configuration.hpp"
#include "SystemTime.hpp"
#include "IObserver.hpp"

class Pump
{
private:
    static Pump *instance;
    Pump();

public:
    static Pump *getInstance();
    bool getPumpState();
    void update(SystemTime *systemTime);
};

#endif