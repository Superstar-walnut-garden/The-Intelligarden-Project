#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "Time.hpp"
#include "SystemTime.hpp"
#include "Subject.hpp"
#include "TempSensorNode.hpp"
#include <vector>
#include "SchedulerList.hpp"
#include "Scheduler.hpp"
#include "Pump.hpp"
#include <SPIFFS.h>

constexpr auto pumpFileAddress = "/schedule.txt";
constexpr auto wifiFileAddress = "/wifi_credentials.txt";
constexpr auto TimeFileAddress = "/backup_time.txt";
constexpr auto sensorFileAddress = "/sensors.txt";

class Configuration : public Subject<Configuration>, public IObserver<SystemTime>
{
private:
    Configuration();
    static Configuration *instance;
    std::vector<TempSensorNode> devList;
    std::vector<TempSensorNode> savedDevList;
    SchedulerList schedulerList;
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

    void setPumpSchedule(Time start, Time duration);
    void setPumpSchedule(String start, String duration);
    void setWifiCredentials(String ssid, String pwd);
    PumpSchedule getPumpSchedule();
    SchedulerList getSchedulerList();
    void setSchedulerList(const char *json, int length = 1024);
    void update(SystemTime *systemTime);
    WifiCred getWifiCredentials();
    std::vector<TempSensorNode> getSensorList();
    void setSensorList(std::vector<TempSensorNode> devList);
    void storeSensorNames(std::vector<TempSensorNode>& list);
};

#endif // CONFIGURATION_HPP
