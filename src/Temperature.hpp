#ifndef TEMPERATURE_HPP
#define TEMPERATURE_HPP

#include "Subject.hpp"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <map>
#include "Configuration.hpp"
#include <vector>
#include <array>
#include <cstdint>
#include <cstring>
#include <Arduino.h>

class Temperature : public Subject<Temperature>, public IObserver<Configuration>
{
public:
    static Temperature *getInstance(); // get singleton instance
    void read(bool doNotify = false); // request a temp conversion from sensors
    double getData(std::string name); // returns sensor data from a registered sensor
    auto getSensorList(); // returns a list of connected sensors (address only)
    void update(Configuration *cfg); // updates configuration with sensors

private:
    Temperature(); // private constructor for singleton pattern
    std::vector<TempSensorNode> obtainSensors(); // helper function to obtain sensors

    OneWire oneWireBus;
    DallasTemperature sensors;
    std::vector<std::map<std::string, uint8_t>> sensorList;

    static Temperature *instance;
};

#endif // TEMPERATURE_HPP
