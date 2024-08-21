#include "WifiSetup.hpp"

// Static member variable initialization
WifiSetup *WifiSetup::instance = nullptr;

// Singleton instance getter
WifiSetup *WifiSetup::getInstance()
{
    if (!instance)
        instance = new WifiSetup();
    return instance;
}

// Check if connected to WiFi
bool WifiSetup::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

// Constructor
WifiSetup::WifiSetup()
{
    auto wifiCred = Configuration::getInstance()->getWifiCredentials();
    auto ssid = wifiCred.getSsid();
    auto password = wifiCred.getPassword();
    auto local_IP = wifiCred.getIP();
    auto gateway = wifiCred.getGateway();
    auto subnet = wifiCred.getSubnet();
    auto primaryDNS = wifiCred.getPrimaryDNS();
    auto secondaryDNS = wifiCred.getSecondaryDNS();

    Serial.print("Connecting to ");
    Serial.println(ssid.c_str());
    Serial.println(password.c_str());

    // Connect to WiFi with the stored credentials
    WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
        static int localCounter = 0;
        delay(500);
        Serial.print(".");
        if (localCounter++ > 15)
        {
            localCounter = 0;
            Serial.println(" Failed to connect!");
            break;
        }
    }
}
