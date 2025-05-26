#ifndef WIFISETUP_HPP
#define WIFISETUP_HPP

#include "SPIFFS.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include "Configuration.hpp"
#include <functional>
#include <atomic>
#include <thread>



class WifiSetup
{
public:
    static WifiSetup *getInstance();
    void connect(WifiHotspotData wifiCred);
    bool isConnected();
    void onConnect(std::function<void()>);
    void loop();

private:
    WifiSetup();
    static WifiSetup *instance;
    std::function<void()> onConnectCallback;
    std::atomic<bool> connectFlag{false};
};

#endif // WIFISETUP_HPP