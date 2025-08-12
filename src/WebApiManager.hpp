#ifndef WEBAPIMANAGER_HPP
#define WEBAPIMANAGER_HPP

#include <WiFi.h>
#include <WiFiUdp.h>
#include <rom/rtc.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "IObserver.hpp"
#include "IManager.hpp"

class WebApiManager
{
public:
    WebApiManager(); // Constructor declaration
    void init();    // Method to start the web server
    

private:
    void createEndpoint(std::string uri, std::function<std::string(std::string)> handler, bool post = false);
    template <typename ItemType>
    void createIManagerEndpoints(std::string uri, IManager<ItemType>* manager);
    AsyncWebServer server; // Server object
    std::string baseUrl;
};

#endif