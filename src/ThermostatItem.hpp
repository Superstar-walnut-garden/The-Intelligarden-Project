#ifndef THERMOSTATITEM_HPP
#define THERMOSTATITEM_HPP

#include "SignalCompatibleBaseItem.hpp"
#include <string>

class ThermostatItem : public SignalCompatibleBaseItem
{
public:
    ThermostatItem();
    ThermostatItem(int id, int event_id, int heaterEvent_id, int coolerEvent_id, std::string name, double setpoint, double altSetpoint, double hysteresis, bool enabled, uint64_t sensor); // pin = id

    static std::string getHeaterSignalName();
    static std::string getCoolerSignalName();
    static std::string getAltSetpointSignalName();
    double getSetpoint();
    double getAltSetpoint();
    double getHysteresis();
    bool isEnabled();
    uint64_t getSensor();

    std::vector<std::string> getLocalSignalNames() override;

private:
    void populateDerivedClassFromJson(JsonDocument &json) override;
    void derivedClassToJson(JsonDocument &doc) override;
    
    int heaterEvent_id, coolerEvent_id;
    double setpoint, altSetpoint, hysteresis;
    bool enabled;
    uint64_t sensor;
};

#endif // GPIOITEM_HPP