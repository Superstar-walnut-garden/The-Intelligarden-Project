#pragma once

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
#include "IResourceController.hpp"
#include "IResourcePersistenceService.hpp"
#include "IConfigController.hpp"
#include "TempSensorConfig.hpp"
#include "ILoggableService.hpp"
#include "TempSensorItem.hpp"

class TempSensorService : 
    public IResourceController<TempSensorItem>, 
    public IConfigController<TempSensorConfig>,
    public IResourcePersistenceService, 
    public Subject<TempSensorService>, 
    public ILoggableService
{
public:
    static TempSensorService *getInstance(); // get singleton instance
    void read(bool doNotify = false); // request a temp conversion from sensors
    double getData(uint64_t id); // returns sensor data from a registered sensor
    void forEachSensor(std::function<void(const TempSensorItem *)> callback, bool onlyRegisteredSensors = false); // iterate over each sensor.

    std::string getAll() const override;
    std::string get(uint64_t id) const override;
    void create(std::string newItem) override;
    void update(uint64_t id, std::string newItem) override; // modify a sensor
    void remove(uint64_t id) override; // delete a sensor

    std::string getConfig() override;
    void updateConfig(TempSensorConfig config) override;

    void storeAll() override;
    void restoreAll() override;

    std::string getName() const override;
    std::vector<ILoggableItem*> getLoggableItems() const override;

private:
    TempSensorService(); // private constructor for singleton pattern
    void obtainSensors(); // helper function to obtain sensors
    double getTempFromSensor(uint64_t address);

    OneWire oneWireBus;
    DallasTemperature sensors;
    mutable TempSensorList sensorList; // sensors with a name associated to them
    TempSensorConfig config;
    mutable std::mutex mtx;

    HardwareSerial fSerial;
    
    static TempSensorService *instance;
};
