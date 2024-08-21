#ifndef WIFISETUP_HPP
#define WIFISETUP_HPP

#include "SPIFFS.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include "Configuration.hpp"

class WifiSetup
{
public:
    static WifiSetup *getInstance();
    bool isConnected();

private:
    WifiSetup();
    static WifiSetup *instance;
};

#endif // WIFISETUP_HPP