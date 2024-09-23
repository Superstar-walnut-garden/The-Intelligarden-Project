#include "CppVirtualInterface.hpp"
#include <ESP32Time.h>
#include <FirebaseESP32.h>
#include <time.h>

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

#include "FirebaseManager.hpp"

constexpr double nightTemp = 23.0, dayTemp = 25.0;
constexpr double threshold = 0.05;

FirebaseAuth auth;
FirebaseConfig config;
// Variable to save USER UID
String uid;

bool signupOK = false;
bool firebaseOK = true;


//FirebaseManager fbm(Configuration::getInstance()->getFirebaseData());

int virtualMain()
{
    
    pinMode(25, OUTPUT);
    if (!SPIFFS.begin(true))
    {
        Serial.println("Fatal Error: An Error has occurred while mounting SPIFFS!");
    }
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
    
    FirebaseManager fbm(Configuration::getInstance()->getFirebaseData());

    if(wifiSetup->isConnected())
    {
        systemTime->obtainTime();
        if(systemTime->isTimeUpdated())
        {
            Serial.println("");
            Serial.println("WiFi connected.");
            fbm.init();
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
            fbm.update(systemTime);
        }
        else
            Serial.println("warning: time is not available due to connection error at the system startup!");

    }
    return 0;
}