#pragma once

#include "SignalCompatibleBaseItem.hpp"
#include <string>

class ThermostatItem : public SignalCompatibleBaseItem
{
public:
    ThermostatItem();
    ThermostatItem(int id, std::string name, double setpoint, double altSetpoint, double hysteresis, bool enabled, uint64_t sensor); // pin = id

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
    void populateDerivedClassFromJson(JsonDocument &json) override;
    void derivedClassToJson(JsonDocument &doc) override;
    
    double setpoint, altSetpoint, hysteresis;
    bool enabled;
    uint64_t sensor;
};
