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
const char* APssid = "ESP32-Access-Point";
const char* APpassword = "12345654321";

class WebInterface
{
    public:
    WebInterface(): server(80)
    {
        // Configure access point
        WiFi.softAP(APssid, APpassword);
        Serial.println();
        Serial.print("SoftAP IP address: ");
        Serial.println(WiFi.softAPIP());

        // Serve HTML page to enter WiFi credentials
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            Serial.println("opening wifi setup page");
            request->send(SPIFFS, "/index.html", "text/html");
        });
        server.on("/pumpsetting", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            Serial.println("opening pump settings!");
            request->send(SPIFFS, "/pumpSetting.html", "text/html");
        });
        server.on("/tempSensors", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            Serial.println("opening sensor settings!");
            request->send(SPIFFS, "/tempSensors.html", "text/html");
        });
        server.on("/getSensorList", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            String json;
            auto *cfg = Configuration::getInstance();
            json += "{";
            for(auto dev : cfg->getSensorList())
                json += "\"" + String(std::to_string((uint64_t)dev).c_str()) +
                  "\": \"" + "sensor" + "\",";
            json.remove(json.length() - 1); // remove the final ","
            json += "}";
            request->send(200, "application/json", json);
        });

        server.on("/getWifiState", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            auto *cfg = Configuration::getInstance();
            String json = "{\"connectionStatus\": " + String((WiFi.status() == WL_CONNECTED)) +
                  ", \"ssidName\": \"" + cfg->getWifiCredentials().ssid + "\"}";
            request->send(200, "application/json", json);
        });
        server.on("/getPumpSchedule", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            String startTime, duration;
            auto *cfg = Configuration::getInstance();
            auto pSchedule = cfg->getPumpSchedule();
            String json = "{\"startTime\": \"" + String(pSchedule.start.toString()) +
                  "\" , \"duration\": \"" + pSchedule.duration.toString() + "\"}";
            request->send(200, "application/json", json);
        });
        server.on("/getPumpStatus", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            auto *pumpObject = Pump::getInstance();

            String json = "{\"state\": \"" + String(pumpObject->getPumpState()) +
                  "\" , \"rTime\": \"" + String("0:00") + "\"}";
            request->send(200, "application/json", json);
        });

        // Handle form submission and save credentials
        server.on("/submit", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            auto input_ssid = request->getParam("ssid")->value();
            auto input_password = request->getParam("password")->value();
            
            // Save the credentials to SPIFFS
            auto *cfg = Configuration::getInstance();
            cfg->setWifiCredentials(input_ssid, input_password); // save credentials
            Serial.println("Credentials saved successfully.");
            request->send(SPIFFS, "/restarting.html", "text/html");
            delay(3000);
            esp_restart(); // restart and connect
        });
        server.on("/submitPump", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            auto startTime = request->getParam("startTime")->value();
            auto duration = request->getParam("duration")->value();
            auto *cfg = Configuration::getInstance();
            cfg->setPumpSchedule(startTime, duration);
            request->send(SPIFFS, "/pumpSetting.html", "text/html");
        });
        server.on("/sendCommand", HTTP_POST, [](AsyncWebServerRequest * request){}, NULL,
            [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            for (size_t i = 0; i < len; i++) {
                Serial.write(data[i]);
            }
        
            Serial.println();
            // Parse JSON payload
            DynamicJsonDocument json(128);
            deserializeJson(json, data);
            String command = json["command"].as<String>();
            
            Serial.print("command received: ");
            Serial.println(command);

            auto *pump = Pump::getInstance();
            if(command.equals("turn_on"))
            {
                int duration = std::stoi(std::string(json["duration"].as<String>().c_str()));
                pump->manualSwitch(duration);
            }
            if(command.equals("turn_off"))
            {
                pump->manualSwitch(); // turn off the pump
            }
            request->send(SPIFFS, "/pumpSetting.html", "text/html");
        });
        
    }
    void init()
    {
        server.begin();
    }

    private:
    AsyncWebServer server;
    
};

#endif