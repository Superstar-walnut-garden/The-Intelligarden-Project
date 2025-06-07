#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "Time.hpp"
#include "SystemTime.hpp"
#include "Subject.hpp"
#include <vector>
#include <SPIFFS.h>
#include "WifiHotspotData.hpp"
#include "FBData.hpp"
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

class Configuration : public Subject<Configuration>, public IObserver<SystemTime>
{
private:
    Configuration();
    static Configuration *instance;
    Time currentTime;
    int currentWeekday;

    void retriveSavedSensorList();

public:
    static Configuration *getInstance();

    struct PumpSchedule
    {
        Time start;
        Time duration;
    };
    struct WifiCred
    {       
        String ssid;
        String pwd;
    };
    WifiHotspotData getWifiCredentials();
    void setWifiCredentials(WifiHotspotData data);

    WifiHotspotData getHotspotCredentials();
    void setHotspotCredentials(WifiHotspotData data);

    FBData getFirebaseData();
    void setFirebaseData(FBData data);
    
    void update(SystemTime *systemTime);

    std::string getSchedulerList();
    void setSchedulerList(std::string json);

    std::string getGPIOList();
    void setGPIOList(std::string json);

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

};

#endif // CONFIGURATION_HPP
