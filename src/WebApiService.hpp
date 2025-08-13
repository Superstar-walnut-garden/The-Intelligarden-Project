#ifndef WEBAPIMANAGER_HPP
#define WEBAPIMANAGER_HPP

#include <WiFi.h>
#include <WiFiUdp.h>
#include <rom/rtc.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "IObserver.hpp"
#include "IResourceController.hpp"
#include "IConfigController.hpp"
#include "IResourcePersistenceService.hpp"
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
    
private:
    void createEndpoint(std::string uri, std::function<std::string(std::string)> handler, bool post = false);
    template <typename ItemType>
    void createIManagerEndpoints(std::string uri, IResourceController<ItemType>* manager);
    AsyncWebServer server; // Server object
    std::string baseUrl;
    WifiHotspotConfig hotspotCred;
};

#endif