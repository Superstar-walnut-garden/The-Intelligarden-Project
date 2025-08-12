#include "WifiHotspotData.hpp"

/**
 * @brief Construct a new WifiHotspotData object
 * 
 */
WifiHotspotData::WifiHotspotData()
{

}

/**
 * @brief Construct a new WifiHotspotData object from JSON string
 * 
 * @param json JSON string containing WiFi hotspot configuration data.
 */
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
    on = doc["on"].as<bool>();
}

/**
 * @brief Convert the WifiHotspotData object to a JSON string
 * 
 * @return std::string JSON string representation of the WifiHotspotData object
 */
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
    doc["on"] = on;
    std::string output;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief Get the WiFi hotspot SSID
 * 
 * @return std::string SSID of the WiFi hotspot
 */
std::string WifiHotspotData::getSsid()
{
    return ssid;
}

/**
 * @brief Get the WiFi hotspot password
 * 
 * @return std::string Password of the WiFi hotspot
 */
std::string WifiHotspotData::getPassword()
{
    return password;
}

/**
 * @brief Get the gateway IP address
 * 
 * @return IPAddress Gateway IP address
 */
IPAddress WifiHotspotData::getGateway()
{
    IPAddress ip;
    if(!gateway.empty())
        ip.fromString(gateway.c_str());
    return ip;
}

/**
 * @brief Get the local IP address
 * 
 * @return IPAddress Local IP address
 */
IPAddress WifiHotspotData::getIP()
{
    IPAddress ip;
    if(!localIP.empty())
        ip.fromString(localIP.c_str());
    return ip;
}

/**
 * @brief Get the subnet mask
 * 
 * @return IPAddress Subnet mask
 */
IPAddress WifiHotspotData::getSubnet()
{
    IPAddress ip;
    if(!subnet.empty())
        ip.fromString(subnet.c_str());
    return ip;
}

/**
 * @brief Get the primary DNS server IP address
 * 
 * @return IPAddress Primary DNS server IP address
 */
IPAddress WifiHotspotData::getPrimaryDNS()
{
    IPAddress ip;
    if(!primaryDNS.empty())
        ip.fromString(primaryDNS.c_str());
    return ip;
}

/**
 * @brief Get the secondary DNS server IP address
 * 
 * @return IPAddress Secondary DNS server IP address
 */
IPAddress WifiHotspotData::getSecondaryDNS()
{
    IPAddress ip;
    if(!secondaryDNS.empty())
        ip.fromString(secondaryDNS.c_str());
    return ip;
}

/**
 * @brief Check if the WiFi hotspot is enabled
 * 
 * @return true if the WiFi/hotspot is enabled, false otherwise
 */
bool WifiHotspotData::isOn()
{
    return on;
}