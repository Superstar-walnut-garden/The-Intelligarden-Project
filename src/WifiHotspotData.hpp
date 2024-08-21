#ifndef WIFIHOTSPOTDATA_HPP
#define WIFIHOTSPOTDATA_HPP
#include <ArduinoJson.h>
#include <string>
#include <iostream>

class WifiHotspotData // Wifi Setup Data Parser
{
    public:
    WifiHotspotData();
    WifiHotspotData(const char* json);
    std::string toJsonString();
    std::string getSsid();
    std::string getPassword();
    IPAddress getGateway();
    IPAddress getIP();
    IPAddress getSubnet();
    IPAddress getPrimaryDNS();
    IPAddress getSecondaryDNS();
    private:
    std::string ssid;
    std::string password;
    std::string localIP;
    std::string gateway;
    std::string subnet;
    std::string primaryDNS;
    std::string secondaryDNS;
};

#endif