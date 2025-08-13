#include "WifiHotspotConfig.hpp"

/**
 * @brief Construct a new WifiHotspotConfig object
 * 
 */
WifiHotspotConfig::WifiHotspotConfig()
{

}

/**
 * @brief Construct a new WifiHotspotConfig object from JSON string
 * 
 * @param json JSON string containing WiFi hotspot configuration.
 */
WifiHotspotConfig::WifiHotspotConfig(std::string json)
{
    this->populateFromJson(json);
}

/**
 * @brief Populate the BaseItem from a JSON string (override the method in case of having more parameters).
 * @param json The JSON string to populate the item from.
 */
void WifiHotspotConfig::populateFromJson(std::string json)
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
 * @brief Convert the WifiHotspotConfig object to a JSON string
 * 
 * @return std::string JSON string representation of the WifiHotspotConfig object
 */
std::string WifiHotspotConfig::toJson()
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
std::string WifiHotspotConfig::getSsid()
{
    return ssid;
}

/**
 * @brief Get the WiFi hotspot password
 * 
 * @return std::string Password of the WiFi hotspot
 */
std::string WifiHotspotConfig::getPassword()
{
    return password;
}

/**
 * @brief Get the gateway IP address
 * 
 * @return IPAddress Gateway IP address
 */
IPAddress WifiHotspotConfig::getGateway()
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
IPAddress WifiHotspotConfig::getIP()
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
IPAddress WifiHotspotConfig::getSubnet()
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
IPAddress WifiHotspotConfig::getPrimaryDNS()
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
IPAddress WifiHotspotConfig::getSecondaryDNS()
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
bool WifiHotspotConfig::isOn()
{
    return on;
}