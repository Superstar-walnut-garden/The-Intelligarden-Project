#ifndef WIFISETUP_HPP
#define WIFISETUP_HPP

#include "SPIFFS.h"
#include <WiFi.h>
#include <WiFiUdp.h>

class WifiSetup
{
public:
    static WifiSetup *getInstance();
    bool isConnected();

private:
    WifiSetup();
    static WifiSetup *instance;
    bool readWiFiCredentials(String &ssid, String &password);
    void initSPIFFS();
};

#endif // WIFISETUP_HPP