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
    gateway.fromString(doc["gateway"].as<std::string>().c_str());
    localIP.fromString(doc["ip"].as<std::string>().c_str());
    subnet.fromString(doc["subnet"].as<std::string>().c_str());
    primaryDNS.fromString(doc["primaryDNS"].as<std::string>().c_str());
    secondaryDNS.fromString(doc["secondaryDNS"].as<std::string>().c_str());
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
    return gateway;
}

/**
 * @brief Get the local IP address
 * 
 * @return IPAddress Local IP address
 */
IPAddress WifiHotspotConfig::getIP()
{
    return localIP;
}

/**
 * @brief Get the subnet mask
 * 
 * @return IPAddress Subnet mask
 */
IPAddress WifiHotspotConfig::getSubnet()
{
    return subnet;
}

/**
 * @brief Get the primary DNS server IP address
 * 
 * @return IPAddress Primary DNS server IP address
 */
IPAddress WifiHotspotConfig::getPrimaryDNS()
{
    return primaryDNS;
}

/**
 * @brief Get the secondary DNS server IP address
 * 
 * @return IPAddress Secondary DNS server IP address
 */
IPAddress WifiHotspotConfig::getSecondaryDNS()
{
    return secondaryDNS;
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