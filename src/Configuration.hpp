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

    void setPumpSchedule(Time start, Time duration);
    void setPumpSchedule(String start, String duration);
    void setWifiCredentials(String ssid, String pwd);
    PumpSchedule getPumpSchedule();
    struct WifiCred
    {       
        String ssid;
        String pwd;
    };
    auto getSchedulerList()
    {
        auto schedulerList = SchedulerList();
        auto file = SPIFFS.open(pumpFileAddress, FILE_READ);
        if(file)
        {
            auto json = file.readString(); // read raw data from file
            schedulerList.repopulateWith(json.c_str(), json.length()); // parse data and repopulate the SchedulerList
            Scheduler scheduler(schedulerList);
            scheduler.determineStatusofItems(); // check with current date and time to indicate which item is on
            file.close();
        }
        return schedulerList;
    }
    void setSchedulerList(const char *json, int length = 1024)
    {
        //auto schedulerList = SchedulerList(json, length);
        File file = SPIFFS.open(pumpFileAddress, FILE_WRITE);
        if (file)
        {
            file.println(json);
            file.close();
            Serial.println("scheduling data saved successfully.");
        } 
        else 
        {
            Serial.println("Failed to open file for writing.");
        }
    }
    void update(SystemTime *systemTime)
    {
        currentTime = systemTime->getTime();
        currentWeekday = systemTime->getWeekday();
    }
    WifiCred getWifiCredentials();
    std::vector<TempSensorNode> getSensorList();
    void setSensorList(std::vector<TempSensorNode> devList);
    void storeSensorNames(std::vector<TempSensorNode>& list);
};

#endif // CONFIGURATION_HPP
