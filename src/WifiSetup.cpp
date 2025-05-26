#include "WifiSetup.hpp"
#include <thread>


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
    // Start the Wi-Fi reconnect thread
    std::thread wifiThread([this, wifiCred]()
    {
        while(true)
        {
            if(!isConnected()) // if not connected
                connect(wifiCred); // retry
            std::this_thread::sleep_for(std::chrono::minutes(2)); // Check every 2 minutes
        }
    });
    wifiThread.detach(); // Run independently
}

void WifiSetup::connect(WifiHotspotData wifiCred)
{
    auto ssid = wifiCred.getSsid();
    auto password = wifiCred.getPassword();
    auto local_IP = wifiCred.getIP();
    auto gateway = wifiCred.getGateway();
    auto subnet = wifiCred.getSubnet();
    auto primaryDNS = wifiCred.getPrimaryDNS();
    auto secondaryDNS = wifiCred.getSecondaryDNS();
    auto wifiOn = wifiCred.isOn();


    // Connect to WiFi with the stored credentials
    WiFi.setAutoReconnect(false);
    WiFi.setSleep(false);
    if(wifiOn)
    {
        Serial.println("wifi is on!");
        Serial.print("Connecting to ");
        Serial.println(ssid.c_str());
        Serial.println(password.c_str());
        WiFi.mode(WIFI_AP_STA);
        WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
        WiFi.begin(ssid.c_str(), password.c_str());
        while (WiFi.status() != WL_CONNECTED)
        {
            static int localCounter = 0;
            delay(500);
            if (localCounter++ > 15)
            {
                localCounter = 0;
                Serial.println(" Failed to connect!");
                WiFi.setAutoReconnect(false);
                WiFi.disconnect();
                break;
            }
        }
    }
    else
    {
        Serial.println("wifi is of :(((((((");
        WiFi.disconnect();
        WiFi.mode(WIFI_AP); // turn off wifi and only work with hotspot
    }
    if(isConnected())
        connectFlag.store(true); // Raise flag
}

void WifiSetup::onConnect(std::function<void()> callback)
{
    onConnectCallback = callback;
}

void WifiSetup::loop()
{
    if (connectFlag.exchange(false)) // Check and reset flag atomically
        if (onConnectCallback) 
            onConnectCallback();
}