#ifndef WEBINTERFACE_HPP
#define WEBINTERFACE_HPP
#include <WiFi.h>
#include <WiFiUdp.h>
#include <rom/rtc.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
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
            request->send(SPIFFS, "/index.html", "text/html");
        });

        // Handle form submission and save credentials
        server.on("/submit", HTTP_GET, [](AsyncWebServerRequest *request)
        {
            String input_ssid = request->getParam("ssid")->value();
            String input_password = request->getParam("password")->value();
            request->send(SPIFFS, "/index.html", "text/html");
            
            // Save the credentials to SPIFFS
            File file = SPIFFS.open("/wifi_credentials.txt", FILE_WRITE);
            if (file) 
            {
                file.println(input_ssid);
                file.println(input_password);
                file.close();
                Serial.println("Credentials saved successfully.");
                esp_restart(); // restart and connect
            } 
            else 
            {
                Serial.println("Failed to open file for writing.");
            }
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