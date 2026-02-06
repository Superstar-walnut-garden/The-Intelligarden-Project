#pragma once

#include "SignalCompatibleBaseItem.hpp"
#include <string>

class ThermostatItem : public SignalCompatibleBaseItem
{
public:
    ThermostatItem();
    
    static std::string getHeaterLocalSignalName();
    static std::string getCoolerLocalSignalName();
    static std::string getAltSetpointLocalSignalName();
    double getSetpoint();
    double getAltSetpoint();
    double getHysteresis();
    bool isEnabled();
    uint64_t getSensor();

    std::vector<std::string> getLocalSignalNames() override;

private:
    double setpoint, altSetpoint, hysteresis;
    bool enabled;
    uint64_t sensor;
};
