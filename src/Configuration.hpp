#pragma once

#include "Time.hpp"
#include "SystemTimeService.hpp"
#include "Subject.hpp"
#include <vector>
#include <SPIFFS.h>
#include "WifiHotspotConfig.hpp"
#include "FirebaseServiceConfig.hpp"
#include <string>

constexpr auto pumpFileAddress = "/schedule.txt";
constexpr auto wifiFileAddress = "/wifi_credentials.txt";
constexpr auto hotspotFileAddress = "/hotspot_credentials.txt";
constexpr auto TimeFileAddress = "/backup_time.txt";
constexpr auto sensorFileAddress = "/sensors.txt";
constexpr auto tempSensorConfigFileAddress = "/tsensor_cfg.json";
constexpr auto firebaseDataFileAddress = "/firebase_data.txt";
constexpr auto gpioFileAddress = "/gpio.txt";
constexpr auto displayFileAddress = "/display.json";
constexpr auto thermostatFileAddress = "/thermostat.json";
constexpr auto registeredSensorFileAddress = "/registered_sensors.json";
constexpr auto timeConfigFileAddress = "/time_config.json";
constexpr auto logDispatcherConfigFileAddress = "/log_config.json";

class Configuration
{
private:
    Configuration();
    static Configuration *instance;

public:
    static Configuration *getInstance();

    std::string getWifiCredentials() const;
    void setWifiCredentials(std::string data);

    std::string getHotspotCredentials() const;
    void setHotspotCredentials(std::string json);

    std::string getFirebaseData() const;
    void setFirebaseData(std::string json);

    std::string getSchedulerList() const;
    void setSchedulerList(std::string json);

    std::string getGpioList() const;
    void setGpioList(std::string json);

    std::string getEventList() const;
    void setEventList(const std::string& state);

    std::string getDisplayConfig() const;
    void setDisplayConfig(const std::string& config);

    std::string getThermostatList() const;
    void setThermostatList(const std::string& json);

    std::string getRegisteredTempSensorList() const;
    void setRegisteredTempSensorList(const std::string& json);

    std::string getTimeConfig() const;
    void setTimeConfig(const std::string& json);

    std::string getLogDispatcherConfig() const;
    void setLogDispatcherConfig(const std::string& json);

    std::string getTempSensorConfig() const;
    void setTempSensorConfig(const std::string& json);

};
