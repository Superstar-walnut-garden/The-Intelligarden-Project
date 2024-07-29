#include "CppVirtualInterface.hpp"
#include <ESP32Time.h>
#include <FirebaseESP32.h>
#include <time.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define API_KEY "AIzaSyAsGvcYkkIhaLzNqt6YIt7DVEskcAKUHig"
#define DATABASE_URL "https://the-intelligarden-project-default-rtdb.firebaseio.com/" 
#define USER_EMAIL "IntelligardenNodeAznab@walnut.garden"
// #define USER_EMAIL "demo@walnut.garden"
#define USER_PASSWORD "ThisIsThePasscode"
#define DATABASE_ROOT_NAME "AznabNode1/"

#include <OneWire.h>
#include <DallasTemperature.h>

#include <NTPClient.h>
#include <rom/rtc.h>
#include <SPIFFS.h>

#include "WifiSetup.hpp"
#include "WebInterface.hpp"
#include "SystemTime.hpp"
#include "Pump.hpp"
#include <Wire.h>
#include <U8g2lib.h>
#include "Temperature.hpp"
#include "Display.hpp"

constexpr double nightTemp = 23.0, dayTemp = 25.0;
constexpr double threshold = 0.05;






FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
// Variable to save USER UID
String uid;

bool signupOK = false;
bool firebaseOK = true;



int virtualMain()
{
    
    pinMode(25, OUTPUT);
    // while(true) // TEST the moc and triak
    // {
    //     digitalWrite(25, HIGH);
    //     delay(5000);
    //     digitalWrite(25, LOW);
    //     delay(5000);
    // }
    WifiSetup *wifiSetup = WifiSetup::getInstance();
    WebInterface *webInterface = new WebInterface();
    auto *systemTime = SystemTime::getInstance();
    auto *pump = Pump::getInstance();
    auto *temperature = Temperature::getInstance();
    auto *configuration = Configuration::getInstance();
    auto *display = Display::getInstance();
    configuration->attach(temperature); // attach temperature as an observer
    temperature->attach(display); // attach display as an observer
    display->drawUI();
    
    webInterface->init();
    Serial.println("Sensors and web interface are initialized!");
    
    if(wifiSetup->isConnected())
    {
        systemTime->obtainTime();
        if(systemTime->isTimeUpdated())
        {
            Serial.println("");
            Serial.println("WiFi connected.");
            // ------------- firebase setup ----------------
            // Assign the api key (required)
            config.api_key = API_KEY;
            /* Assign the RTDB URL (required) */
            config.database_url = DATABASE_URL;
            // Assign the user sign in credentials
            auth.user.email = USER_EMAIL;
            auth.user.password = USER_PASSWORD;
            fbdo.setResponseSize(4096);
            // Assign the callback function for the long running token generation task
            config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

            // Assign the maximum retry of token generation
            config.max_token_generation_retry = 5;
            // Initialize the library with the Firebase authen and config
            Firebase.begin(&config, &auth);
            // Getting the user UID might take a few seconds
            Serial.println("Getting User UID");
            while ((auth.token.uid) == "")
            {
                Serial.print('.');
                delay(1000);
                static int localCounter = 0;
                if(localCounter ++ > 6)
                {
                    localCounter = 0;
                    Serial.println("Error: Couldn't get user's UID");
                    firebaseOK = false;
                    break;
                }
            }
            // Print user UID
            uid = auth.token.uid.c_str();
            Serial.print("User UID: ");
            Serial.println(uid);
            Firebase.reconnectWiFi(true);
        }
    }

    while(true)
    {
        delay(1); // For other threads to work.this should be 1ms in the main setup
        static int cnt = 20;
        static int softdog = 0;
        if(softdog ++ > 80)
        {
            softdog = 0;
            // esp_restart(); // Continuesly reset to support the system stability;
        }
        Serial.println(WiFi.status() == WL_CONNECTED ? "Wifi is Connected!" : "Fatal Error: Wifi is disconnected!!!");
        display->drawUI();
        delay(100);

        temperature->read(true); // read and notify the observers
        auto *cfg = Configuration::getInstance();
        auto list = cfg->getSensorList();
        Serial.println("Sensor Data:");
        for(auto sensor : list)
        {
            const auto name = sensor.getName();
            const auto data = temperature->getData(name);
            Serial.print((name + "= " + std::to_string(data) + "C").c_str());
        }
        if(systemTime->isTimeUpdated())
        {
            int hour = systemTime->getHour();
            int minute = systemTime->getMinute();
            Serial.printf("Internal RTC Time: %.2d:%.2d\n", hour, minute);
            pump->loop(hour, minute);
            if((minute % 10) == 0)
            {
                if(firebaseOK)
                {
                    auto *cfg = Configuration::getInstance();
                    auto sensorList = cfg->getSensorList();
                    for(auto sensor : sensorList)
                    {
                        auto name = sensor.getName();
                        auto value = temperature->getData(name);
                        auto databasePath = DATABASE_ROOT_NAME + std::to_string(systemTime->getYear()) + "/" + std::to_string(systemTime->getMonth()) + "/" 
                                    + std::to_string(systemTime->getDay()) + "/" + std::to_string(systemTime->getHour());
                        temperature->read();
                        if(Firebase.RTDB.getFloat(&fbdo, databasePath + "/" + name) == NULL)
                        {
                            Serial.println("new data is about to be registered on the database!");
                            if (Firebase.RTDB.setFloat(&fbdo, databasePath + "/" + name, temperature->getData(name)))
                            {
                                Serial.println("PASSED");
                                Serial.println("PATH: " + fbdo.dataPath());
                                Serial.println("TYPE: " + fbdo.dataType());
                            }
                            else
                            {
                                Serial.println("FAILED");
                                Serial.println("REASON: " + fbdo.errorReason());
                            }
                        }
                        else
                        {
                            Serial.println("warning: the data for the current time and date is already registered on the database!");
                        }
                    }
                }
            }
        }
        else
            Serial.println("warning: time is not available due to connection error at the system startup!");

    }
    return 0;
}