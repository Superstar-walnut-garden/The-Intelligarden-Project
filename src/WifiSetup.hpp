#ifndef WIFISETUP_HPP
#define WIFISETUP_HPP

#include "SPIFFS.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include "Configuration.hpp"
#include <functional>
#include <atomic>
#include <thread>
#include <ESP32Ping.h>



class WifiSetup
{
public:
    static WifiSetup *getInstance();
    void connect(WifiHotspotData wifiCred);
    bool isConnected();
    bool isOnline();
    void onConnect(std::function<void()>);
    void onOnline(std::function<void()>);
    void onDisconnect(std::function<void()>);
    void onOffline(std::function<void()>);
    void loop();

private:
    WifiSetup();
    static WifiSetup *instance;
    std::function<void()> onConnectCallback;
    std::function<void()> onOnlineCallback;
    std::function<void()> onDisconnectCallback;
    std::function<void()> onOfflineCallback;
    std::atomic<bool> connectFlag{false};
    std::atomic<bool> onlineFlag{false};
    std::atomic<bool> disconnectFlag{false};
    std::atomic<bool> offlineFlag{false};
};

#endif // WIFISETUP_HPP