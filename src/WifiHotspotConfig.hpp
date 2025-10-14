#pragma once
#include <ArduinoJson.h>
#include <string>
#include <iostream>
#include <IJsonSerializable.hpp>

class WifiHotspotConfig: public IJsonSerializable
{
    public:
    WifiHotspotConfig();
    WifiHotspotConfig(std::string json);
    std::string toJson() override;
    void populateFromJson(std::string json) override;

    std::string getSsid();
    std::string getPassword();
    IPAddress getGateway();
    IPAddress getIP();
    IPAddress getSubnet();
    IPAddress getPrimaryDNS();
    IPAddress getSecondaryDNS();
    bool isOn();

    private:
    std::string ssid;
    std::string password;
    IPAddress localIP;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress primaryDNS;
    IPAddress secondaryDNS;
    bool on;
};
