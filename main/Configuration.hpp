#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP
#include "Time.hpp"

constexpr auto pumpFileAddress ="/schedule.txt";
constexpr auto wifiFileAddress ="/wifi_credentials.txt";
constexpr auto TimeFileAddress ="/backup_time.txt";

class Configuration
{
    private:
    Configuration(){}
    static Configuration *instance;

    public: 
    static Configuration *getInstance()
    {
        if(!instance)
            instance = new Configuration();
        return instance;
    }
    struct PumpSchedule
    {
        Time start; 
        Time duration;
    };
    void setPumpSchedule(Time start, Time duration)
    { // string conversion
        setPumpSchedule(String(start.getHour()) + ":" + String(start.getMinute()), 
            String(duration.getHour()) + ":" + String(duration.getMinute()));
    }
    void setPumpSchedule(String start, String duration)
    {
        File file = SPIFFS.open(pumpFileAddress, FILE_WRITE);
        if (file)
        {
            file.println(start);
            file.println(duration);
            file.close();
            Serial.println("scheduling data saved successfully.");
        } 
        else 
        {
            Serial.println("Failed to open file for writing.");
        }
    }
    void setWifiCredentials(String ssid, String pwd)
    {
        // Save the credentials to SPIFFS
        File file = SPIFFS.open(wifiFileAddress, FILE_WRITE);
        if (file)
        {
            file.println(ssid);
            file.println(pwd);
            file.close();
            Serial.println("Credentials saved successfully.");
        } 
        else 
        {
            Serial.println("Failed to open file for writing.");
        }
    }
    PumpSchedule getPumpSchedule()
    {
        PumpSchedule pSchedule{Time(0, 0), Time(0, 0)};
        auto file = SPIFFS.open(pumpFileAddress, FILE_READ);
        if(file)
        {
            auto startTime = file.readStringUntil('\n');
            auto duration = file.readStringUntil('\n');
            file.close();

            std::string correctedTime(startTime.c_str());
            correctedTime.pop_back();
            std::string correctedDuration(duration.c_str());
            correctedDuration.pop_back();

            if(correctedTime.find(":") != -1)
            {
                Serial.println("schedule data = " + String(correctedTime.c_str()) + " " + String(correctedDuration.c_str()));
                std::size_t timeDelimiterPos = correctedTime.find(':');
                if (timeDelimiterPos != std::string::npos) {
                    pSchedule.start.setHour(std::stoi(correctedTime.substr(0, timeDelimiterPos)));
                    pSchedule.start.setMinute(std::stoi(correctedTime.substr(timeDelimiterPos + 1)));

                }

                // Parse correctedDuration
                std::size_t durationDelimiterPos = correctedDuration.find(':');
                if (durationDelimiterPos != std::string::npos) {
                    pSchedule.duration.setHour(std::stoi(correctedDuration.substr(0, durationDelimiterPos)));
                    pSchedule.duration.setMinute(std::stoi(correctedDuration.substr(durationDelimiterPos + 1)));
                }
            }
        }
        return pSchedule;
    }
    auto getWifiCredentials()
    {
        struct WifiCred{String ssid; String pwd;} wifiCred;
        if (SPIFFS.exists(wifiFileAddress)) 
        {
            File file = SPIFFS.open(wifiFileAddress, FILE_READ);
            if (file)
            {
                
                wifiCred.ssid = file.readStringUntil('\n');
                wifiCred.pwd = file.readStringUntil('\n');
                file.close();
            }
        }
        wifiCred.ssid.remove(wifiCred.ssid.length() - 1); // remove the "/n" from the end
        wifiCred.pwd.remove(wifiCred.pwd.length() - 1);
        return wifiCred;
    }
};
Configuration *Configuration::instance = nullptr;


#endif