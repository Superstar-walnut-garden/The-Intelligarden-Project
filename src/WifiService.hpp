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
#include "WifiHotspotConfig.hpp"
#include "IConfigController.hpp"
#include "IResourcePersistenceService.hpp"



class WifiService: public IConfigController<WifiHotspotConfig>, public IResourcePersistenceService
{
public:
    static WifiService *getInstance();
    void connect();
    bool isConnected();
    bool isOnline();
    void onConnect(std::function<void()>);
    void onOnline(std::function<void()>);
    void onDisconnect(std::function<void()>);
    void onOffline(std::function<void()>);
    void loop();

    void updateConfig(WifiHotspotConfig cfg) override;
    std::string getConfig() override;

    void storeAll() override;
    void restoreAll() override;

private:
    WifiService();
    static WifiService *instance;
    std::function<void()> onConnectCallback;
    std::function<void()> onOnlineCallback;
    std::function<void()> onDisconnectCallback;
    std::function<void()> onOfflineCallback;
    std::atomic<bool> connectFlag{false};
    std::atomic<bool> onlineFlag{false};
    std::atomic<bool> disconnectFlag{false};
    std::atomic<bool> offlineFlag{false};

    WifiHotspotConfig wifiCred;
};

#endif // WIFISETUP_HPP