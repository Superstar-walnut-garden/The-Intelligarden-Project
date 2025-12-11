#pragma once
#include <ArduinoJson.h>
#include <string>
#include <iostream>
#include <IJsonSerializable.hpp>

class TempSensorConfig: public IJsonSerializable
{
    public:
    TempSensorConfig();
    TempSensorConfig(std::string json);
    std::string toJson() const override;
    void populateFromJson(std::string json) override;

    int getSensorPin() const;

    private:
    int sensorPin; // Pin where the temperature sensor is connected
};
