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
            Serial.println("html file send request");
            
            String htmlFile;
            auto file = SPIFFS.open("/index.html", FILE_READ);
            if (file)
            {
                htmlFile = file.readString();
                file.close();
            }

            

            request->send(200, "text/html", htmlFile);
        });
        server.on("/pumpsetting", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            Serial.println("html file send request");
            
            String htmlFile;
            auto file = SPIFFS.open("/pumpSetting.html", FILE_READ);
            if (file)
            {
                htmlFile = file.readString();
                file.close();
            }

            

            request->send(200, "text/html", htmlFile);
        });

        server.on("/getWifiState", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            String ssid;
            auto credentials = SPIFFS.open("/wifi_credentials.txt", FILE_READ);
            if(credentials)
            {
                ssid = credentials.readStringUntil('\n');
                credentials.close();
            }
            std::string correctedSSID(ssid.c_str());
            correctedSSID.pop_back(); // remove the \n
            String json = "{\"connectionStatus\": " + String((WiFi.status() == WL_CONNECTED)) +
                  ", \"ssidName\": \"" + correctedSSID.c_str() + "\"}";
            request->send(200, "application/json", json);
        });
        server.on("/getPumpSchedule", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            String startTime, duration;
            auto file = SPIFFS.open("/schedule.txt", FILE_READ);
            if(file)
            {
                startTime = file.readStringUntil('\n');
                duration = file.readStringUntil('\n');
                file.close();
            }
            std::string correctedTime(startTime.c_str());
            correctedTime.pop_back();
            std::string correctedDuration(duration.c_str());
            correctedDuration.pop_back();
            String json = "{\"startTime\": \"" + String(correctedTime.c_str()) +
                  "\" , \"duration\": \"" + correctedDuration.c_str() + "\"}";
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
            String input_ssid = request->getParam("ssid")->value();
            String input_password = request->getParam("password")->value();
            request->send(SPIFFS, "/restarting.html", "text/html");
            
            // Save the credentials to SPIFFS
            File file = SPIFFS.open("/wifi_credentials.txt", FILE_WRITE);
            if (file)
            {
                file.println(input_ssid);
                file.println(input_password);
                file.close();
                Serial.println("Credentials saved successfully.");
                request->send(SPIFFS, "/restarting.html", "text/html");
                delay(3000);
                esp_restart(); // restart and connect
            } 
            else 
            {
                Serial.println("Failed to open file for writing.");
            }
        });
        server.on("/submitPump", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            String startTime = request->getParam("startTime")->value();
            String duration = request->getParam("duration")->value();
            // Save the credentials to SPIFFS
            File file = SPIFFS.open("/schedule.txt", FILE_WRITE);
            if (file)
            {
                file.println(startTime);
                file.println(duration);
                file.close();
                Serial.println("scheduling data saved successfully.");
                request->send(SPIFFS, "/pumpSetting.html", "text/html");
            } 
            else 
            {
                Serial.println("Failed to open file for writing.");
            }
        });

        // server.on("/sendCommand", HTTP_POST, [](AsyncWebServerRequest *request)
        // {   
        //     String jsonPayload;
        //     // if (request->hasParam("plain")) 
        //     // {
        //     //     // data
        //     //     // jsonPayload = request->getParam("plain", true)->value();
        //     // } 
        //     // else 
        //     // {
        //     //     Serial.println("Error: No payload received");
        //     //     Serial.println("content length: " + String(request->contentLength()));
        //     //     Serial.println("params: " + String(request->params()));
        //     //     Serial.println("has param json: " + String(request->hasParam("json")? "yes": "no"));
        //     //     Serial.println("has param application/json: " + String(request->hasParam("application/json")? "yes": "no"));
        //     //     Serial.println("has param application/json: " + String(request->hasParam("application/json")? "yes": "no"));
        //     //     Serial.println("has param body: " + String(request->hasParam("body")? "yes": "no"));
        //     //     request->send(400); // Bad request
        //     //     return;
        //     // }
        //     // // Parse JSON payload
        //     // // DynamicJsonDocument json(128);
        //     // // deserializeJson(json, request->getParam());
        //     // // String command = json["command"].as<String>();

        //     // Serial.print("request received: ");
        //     // Serial.println(jsonPayload);
        //     // // Serial.print("command received: ");
        //     // // Serial.println(command);
            
        //     // // Implement your logic for handling the command (turn on/off)
        //     // // ...

        //     // // Respond with success message
        //     request->send(200, "application/json", "{\"status\": \"success\"}");
            
        // });
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
            // request->send(200, "application/json", "{\"status\": \"success\"}");
            String htmlFile;
            auto file = SPIFFS.open("/pumpSetting.html", FILE_READ);
            if (file)
            {
                htmlFile = file.readString();
                file.close();
            }
            request->send(200, "text/html", htmlFile);
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