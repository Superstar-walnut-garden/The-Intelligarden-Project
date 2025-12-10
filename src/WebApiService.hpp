#pragma once

#include <WiFi.h>
#include <WiFiUdp.h>
#include <rom/rtc.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "IObserver.hpp"
#include "IResourceController.hpp"
#include "IConfigController.hpp"
#include "IResourcePersistenceService.hpp"
#include "IReadOnlyResourceController.hpp"
#include "WifiHotspotConfig.hpp"

class WebApiService: public IConfigController<WifiHotspotConfig>, public IResourcePersistenceService
{
public:
    WebApiService(); // Constructor declaration
    void init();    // Method to start the web server

    void updateConfig(WifiHotspotConfig cfg) override;
    std::string getConfig() override;
    void storeAll() override;
    void restoreAll() override;
    enum class Method{ Get, Post, Put, Delete };
    
private:
    void createEndpoint(std::string uri, std::function<std::string(std::optional<uint64_t>, std::string)> handler, Method method = Method::Get);
    
    template <typename ItemType>
    void createEndpoint(std::string uri, IConfigController<ItemType>* configController);
    
    template <typename ItemType>
    void createEndpoint(std::string uri, IResourceController<ItemType>* resourceController);
    
    void createEndpoint(std::string uri, IReadOnlyResourceController* resourceController);
    
    AsyncWebServer server; // Server object
    std::string baseUrl;
    WifiHotspotConfig hotspotCred;
};
