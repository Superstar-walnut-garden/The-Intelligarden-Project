#ifndef WEBINTERFACE_HPP
#define WEBINTERFACE_HPP

#include <WiFi.h>
#include <WiFiUdp.h>
#include <rom/rtc.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "Pump.hpp"
#include "IObserver.hpp"

class WebInterface
{
public:
    WebInterface();  // Constructor declaration
    void init();     // Method to start the web server

private:
    AsyncWebServer server; // Server object
};

#endif // WEBINTERFACE_HPP
