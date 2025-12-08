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

    std::string getWifiCredentials();
    void setWifiCredentials(std::string data);

    std::string getHotspotCredentials();
    void setHotspotCredentials(std::string json);

    std::string getFirebaseData();
    void setFirebaseData(std::string json);

    std::string getSchedulerList();
    void setSchedulerList(std::string json);

    std::string getGpioList();
    void setGpioList(std::string json);

    std::string getEventList();
    void setEventList(const std::string& state);

    std::string getDisplayConfig();
    void setDisplayConfig(const std::string& config);

    std::string getThermostatList();
    void setThermostatList(const std::string& json);

    std::string getRegisteredTempSensorList();
    void setRegisteredTempSensorList(const std::string& json);

    std::string getTimeConfig();
    void setTimeConfig(const std::string& json);

    std::string getLogDispatcherConfig();
    void setLogDispatcherConfig(const std::string& json);

};
