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

class Temperature: public Subject<Temperature>, public IObserver<Configuration>
{
    public:
    static Temperature *getInstance()
    {
        if(!instance)
            instance = new Temperature();
        return instance;
    }
    void read(bool doNotify = false)
    {
        sensors.requestTemperatures();
        if(doNotify)
            notify(); // notify the observers when data is ready
    }
    double getData(std::string name)
    {
        return sensors.getTempCByIndex(0);
    }
    // std::vector<std::map<std::string, double>> getAllData()
    // {
    //     std::vector<std::map<std::string, double>> list;
    //     for(auto sensor : sensorList)
    //     {
    //         sensors.getAddress()
    //         list.push_back(std::map<std::string, double>("ambient", sensors.getTempCByIndex(0)));
            
    //     }
    //     return sensors.getTempCByIndex(0);
    // }
    auto getSensorList()
    {
        return sensorList;
    }
    void update(Configuration *cfg)
    {
        Serial.println("notified");
        cfg->setSensorList(obtainSensors());
    }



    private:
    std::vector<uint64_t> obtainSensors()
    {
        oneWireBus.begin(15);
        oneWireBus.reset();
        std::vector<uint64_t> devList;
        byte addr[8];
        Serial.println("obtaining sensors");
        while(oneWireBus.search(addr))
        {
            for (uint8_t i = 0; i < 8; i++)
            {
                Serial.print("0x");
                if (addr[i] < 0x10) Serial.print("0");
                Serial.print(addr[i], HEX);
                if (i < 7) Serial.print(", ");
            }
            Serial.println("\n an address found!");
            uint64_t addr64;
            std::memcpy(&addr64, addr, sizeof(addr64));
            devList.push_back(addr64);
            Serial.println(devList.at(0), HEX);
        }
        oneWireBus.reset_search();
        Serial.println("obtaining sensors completed");
        return devList;
    }
    Temperature(): oneWireBus(15), sensors(&oneWireBus)
    {
        sensors.begin();
    }
    OneWire oneWireBus;
    DallasTemperature sensors;
    std::vector<std::map<std::string, uint8_t>> sensorList;

    static Temperature *instance;

};
Temperature *Temperature::instance = nullptr;

#endif