#include "SystemTime.hpp"

SystemTime *SystemTime::instance = nullptr;
SystemTime *SystemTime::getInstance()
{
    if(!instance)
        instance = new SystemTime();
    return instance;
}