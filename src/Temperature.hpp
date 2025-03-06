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
#include "TempSensorList.hpp"
#include <mutex>

class Temperature : public Subject<Temperature>
{
public:
    static Temperature *getInstance(); // get singleton instance
    void read(bool doNotify = false); // request a temp conversion from sensors
    double getData(std::string name); // returns sensor data from a registered sensor
    double getData(uint64_t id); // returns sensor data from a registered sensor

    void modify(uint64_t id, TempSensorItem newItem); // modify a sensor
    void remove(uint64_t id); // delete a sensor
    void forEachSensor(std::function<void(TempSensorItem)> callback, bool onlyRegisteredSensors = false); // iterate over each sensor.

    std::string getListJson(); // get a complete list (registered and live sensors) in JSON format
    void saveState();
    void loadState();

private:
    Temperature(); // private constructor for singleton pattern
    void obtainSensors(); // helper function to obtain sensors
    static void mergeAndCopy(TempSensorList &primary, TempSensorList secondary);

    TempSensorList getCompleteList(); // get a complete list of sensors
    double getTempFromSensor(uint64_t address);

    OneWire oneWireBus;
    DallasTemperature sensors;
    TempSensorList registeredSensorList; // sensors with a name associated to them
    TempSensorList liveSensorList; // currently connected sensors (address only)
    std::mutex mtx;

    static Temperature *instance;
};

#endif // TEMPERATURE_HPP
