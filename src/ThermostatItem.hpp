#ifndef THERMOSTATITEM_HPP
#define THERMOSTATITEM_HPP

#include "BaseItem.hpp"
#include <string>

class ThermostatItem : public BaseItem
{
public:
    ThermostatItem();
    ThermostatItem(int id, int event_id, int heaterEvent_id, int coolerEvent_id, std::string name, double setpoint, double altSetpoint, double hysteresis, bool enabled, uint64_t sensor); // pin = id

    int getHeaterEvent_id();
    int getCoolerEvent_id();
    double getSetpoint();
    double getAltSetpoint();
    double getHysteresis();
    bool isEnabled();
    uint64_t getSensor();

private:
    void populateDerivedClassFromJson(JsonDocument &json) override;
    void derivedClassToJson(JsonDocument &doc) override;
    
    int heaterEvent_id, coolerEvent_id;
    double setpoint, altSetpoint, hysteresis;
    bool enabled;
    uint64_t sensor;
};

#endif // GPIOITEM_HPP