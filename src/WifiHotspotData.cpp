#include "WifiHotspotData.hpp"

WifiHotspotData::WifiHotspotData()
{

}
WifiHotspotData::WifiHotspotData(const char* json)
{
    JsonDocument doc;
    deserializeJson(doc, json);

    ssid = doc["ssid"].as<std::string>();
    password = doc["password"].as<std::string>();
    gateway = doc["gateway"].as<std::string>();
    localIP = doc["ip"].as<std::string>();
    subnet = doc["subnet"].as<std::string>();
    primaryDNS = doc["primaryDNS"].as<std::string>();
    secondaryDNS = doc["secondaryDNS"].as<std::string>();
}
std::string WifiHotspotData::toJsonString()
{
    JsonDocument doc;
    doc["ssid"] = ssid;
    doc["password"] = password;
    doc["gateway"] = gateway;
    doc["ip"] = localIP;
    doc["subnet"] = subnet;
    doc["primaryDNS"] = primaryDNS;
    doc["secondaryDNS"] = secondaryDNS;
    std::string output;
    serializeJson(doc, output);
    return output;
}
std::string WifiHotspotData::getSsid()
{
    return ssid;
}
std::string WifiHotspotData::getPassword()
{
    return password;
}
IPAddress WifiHotspotData::getGateway()
{
    IPAddress ip;
    if(!gateway.empty())
        ip.fromString(gateway.c_str());
    return ip;
}
IPAddress WifiHotspotData::getIP()
{
    IPAddress ip;
    if(!localIP.empty())
        ip.fromString(localIP.c_str());
    return ip;
}
IPAddress WifiHotspotData::getSubnet()
{
    IPAddress ip;
    if(!subnet.empty())
        ip.fromString(subnet.c_str());
    return ip;
}
IPAddress WifiHotspotData::getPrimaryDNS()
{
    IPAddress ip;
    if(!primaryDNS.empty())
        ip.fromString(primaryDNS.c_str());
    return ip;
}
IPAddress WifiHotspotData::getSecondaryDNS()
{
    IPAddress ip;
    if(!secondaryDNS.empty())
        ip.fromString(secondaryDNS.c_str());
    return ip;
}