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
#include "ILoggableService.hpp"

class TempSensorService : 
    public IResourceController<TempSensorItem>, 
    public IResourcePersistenceService, 
    public Subject<TempSensorService>, 
    public ILoggableService
{
public:
    static TempSensorService *getInstance(); // get singleton instance
    void read(bool doNotify = false); // request a temp conversion from sensors
    double getData(std::string name); // returns sensor data from a registered sensor
    double getData(uint64_t id); // returns sensor data from a registered sensor
    void forEachSensor(std::function<void(TempSensorItem)> callback, bool onlyRegisteredSensors = false); // iterate over each sensor.

    std::string getAll() override;
    std::string get(uint64_t id) override;
    void create(TempSensorItem newItem) override;
    void update(uint64_t id, TempSensorItem newItem) override; // modify a sensor
    void remove(uint64_t id) override; // delete a sensor
    void storeAll() override;
    void restoreAll() override;

    std::string getName() const override;
    std::vector<std::unique_ptr<ILoggableItem>> getLoggableItems() const override;
    void setLastLogTime(uint64_t itemId, std::chrono::time_point<std::chrono::system_clock> time) override;

private:
    TempSensorService(); // private constructor for singleton pattern
    void obtainSensors(); // helper function to obtain sensors
    static void mergeAndCopy(TempSensorList &primary, TempSensorList secondary);

    TempSensorList getCompleteList() const; // get a complete list of sensors
    double getTempFromSensor(uint64_t address);

    OneWire oneWireBus;
    DallasTemperature sensors;
    TempSensorList registeredSensorList; // sensors with a name associated to them
    TempSensorList liveSensorList; // currently connected sensors (address only)
    std::mutex mtx;

    static TempSensorService *instance;
};
