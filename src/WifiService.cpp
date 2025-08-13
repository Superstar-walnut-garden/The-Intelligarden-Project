#include "WifiService.hpp"
#include <thread>


/**
 * @brief instance initialization for the singleton pattern
 * 
 */
WifiService *WifiService::instance = nullptr;

/**
 * @brief retrieve the singleton instance of WifiService.
 * 
 * @return WifiService* 
 */
WifiService *WifiService::getInstance()
{
    if (!instance)
        instance = new WifiService();
    return instance;
}


/**
 * @brief Constructor for WifiService class.
 * Initializes the Wi-Fi connection and starts an internal async thread to monitor the connection status.
 */
WifiService::WifiService()
{
    restoreAll();
    // Start the Wi-Fi reconnect thread
    std::thread wifiThread([this]()
    {
        bool lastOnlineStatus = false; // Track last online status
        bool lastConnectedStatus = false; // Track last wifi connection status
        while(true)
        {
            if(!isConnected()) // if not connected
                connect(); // retry
            if(isConnected() && !lastConnectedStatus) // if connected to wifi
            {
                Serial.println("Wifi Thread: Connected to WiFi!");
                connectFlag.store(true); // Invoke onConnect callback
                lastConnectedStatus = true; // Update last connected status
            }
            else if(!isConnected() && lastConnectedStatus) // if disconnected from wifi
            {
                Serial.println("Wifi Thread: Disconnected from WiFi!");
                disconnectFlag.store(true); // Invoke onDisconnect callback
                lastConnectedStatus = false; // Update last connected status
            }
            if(isOnline() && !lastOnlineStatus) // if connected to wifi and online
            {
                Serial.println("Wifi Thread: Internet is available now!");
                onlineFlag.store(true); // Invoke onOnline callback
                lastOnlineStatus = true; // Update last online status
            }
            else if(!isOnline() && lastOnlineStatus) // if disconnected from internet
            {
                Serial.println("Wifi Thread: Internet is disconnected!");
                offlineFlag.store(true); // Invoke onOffline callback
                lastOnlineStatus = false; // Update last online status
            }
            std::this_thread::sleep_for(std::chrono::seconds(15)); // Check every 15 seconds
        }
    });
    wifiThread.detach(); // Run independently
}

/**
 * @brief Connect to Wi-Fi using the provided credentials.
 * 
 * @param wifiCred The Wi-Fi credentials to use for connection.
 */
void WifiService::connect()
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
        Serial.println("wifi is off :(((((((");
        WiFi.disconnect(true);
        delay(100);
        if(WiFi.getMode() != WIFI_AP)
            WiFi.mode(WIFI_AP); // turn off wifi and only work with hotspot
    }
}

/**
 * @brief Set the callback function to be called when connected to Wi-Fi.
 * 
 * @param callback The function to call when connected.
 */
void WifiService::onConnect(std::function<void()> callback)
{
    onConnectCallback = callback;
}

/**
 * @brief Set the callback function to be called when online (internet available).
 * 
 * @param callback The function to call when online.
 */
void WifiService::onOnline(std::function<void()> callback)
{
    onOnlineCallback = callback;
}

/**
 * @brief Set the callback function to be called when disconnected from Wi-Fi.
 * 
 * @param callback The function to call when disconnected.
 */
void WifiService::onDisconnect(std::function<void()> callback)
{
    onDisconnectCallback = callback;
}

/**
 * @brief Set the callback function to be called when offline (internet not available).
 * 
 * @param callback The function to call when offline.
 */
void WifiService::onOffline(std::function<void()> callback)
{
    onOfflineCallback = callback;
}

/**
 * @brief Check if the device is online (connected to the internet).
 * 
 * @return true if online, false otherwise.
 */
bool WifiService::isOnline()
{
    if(isConnected()) // If connected to wifi, check internet connectivity
        if (Ping.ping(IPAddress(8, 8, 8, 8), 3)) // Ping Google's dns server
            return true;
    return false;
}

/**
 * @brief Check if the device is connected to Wi-Fi (regardless of internet availability).
 * 
 * @return true if connected, false otherwise.
 */
bool WifiService::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

/**
 * @brief Main loop function to manage connection callbacks and flags.
 * @details this function should be called periodically to handle Wi-Fi events.
 */
void WifiService::loop()
{
    // manage the connection callbacks and flags
    if (connectFlag.exchange(false)) // Check and reset wifi connect flag atomically
        if (onConnectCallback) 
            onConnectCallback();
    if(onlineFlag.exchange(false)) // Check and reset online flag atomically
        if (onOnlineCallback) 
            onOnlineCallback();
    if (disconnectFlag.exchange(false)) // Check and reset disconnect flag atomically
        if (onDisconnectCallback) 
            onDisconnectCallback();
    if (offlineFlag.exchange(false)) // Check and reset offline flag atomically
        if (onOfflineCallback) 
            onOfflineCallback();
}

/**
 * @brief store config in storage
 * 
 */
void WifiService::storeAll()
{
    Configuration::getInstance()->setWifiCredentials(wifiCred.toJson());
}

/**
 * @brief restore config from storage
 * 
 */
void WifiService::restoreAll()
{
    wifiCred.populateFromJson(Configuration::getInstance()->getWifiCredentials());
}

/**
 * @brief update config (for web-api)
 * @param cfg new configuration to replace the old one
 */
void WifiService::updateConfig(WifiHotspotConfig cfg)
{
    this->wifiCred = cfg;
    storeAll();
}

/**
 * @brief get config in json string format (for web-api)
 * @return std::string configuration json string
 */
std::string WifiService::getConfig()
{
    return wifiCred.toJson();
}