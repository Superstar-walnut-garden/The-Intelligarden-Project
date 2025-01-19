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
#include "SystemMaintainer.hpp"

int virtualMain()
{
    delay(500); // delay for hardware stablization
    pinMode(25, OUTPUT);
    if (!SPIFFS.begin(true))
        Serial.println("Fatal Error: An Error has occurred while mounting SPIFFS!");
    
    SystemMaintainer::getInstance().start();
    WifiSetup *wifiSetup = WifiSetup::getInstance();
    WebInterface *webInterface = new WebInterface();
    delay(500); // waiting utill reaching system stability
    auto *systemTime = SystemTime::getInstance();
    auto *pump = Pump::getInstance();
    auto *temperature = Temperature::getInstance();
    auto *configuration = Configuration::getInstance();
    auto *display = Display::getInstance();
    auto *fbm = new FirebaseManager(Configuration::getInstance()->getFirebaseData());
    configuration->attach(temperature); // attach temperature as an observer
    temperature->attach(display); // attach display as an observer
    systemTime->attach(fbm);
    display->drawUI();
    
    webInterface->init();
    SystemMaintainer::getInstance().postponeRestart(480);
    Serial.println("Sys-OK: All of the system components are initialized and the next restart postponed to 8 hours later!");

    

    if(wifiSetup->isConnected())
    {
        systemTime->obtainTime();
        if(systemTime->isTimeUpdated())
        {
            Serial.println("");
            Serial.println("WiFi connected.");
            fbm->init();
            fbm->update(systemTime);
        }
    }

    if(!systemTime->isTimeUpdated() or !wifiSetup->isConnected())
    {
        SystemMaintainer::getInstance().setAbnormalCondition(true); // system abnormality reported!
        Serial.println("Sys-Error: No internet access. Check your router! System will be rebooted 2 minutes later!");
    }

    while(true)
    {
        SystemMaintainer::getInstance().monitorCycle(); // software implemented watchdog
        
        delay(1); // For other threads to work.this should be 1ms in the main setup
        Serial.println(WiFi.status() == WL_CONNECTED ? "Wifi is Connected!" : "Fatal Error: Wifi is disconnected!!!");
        display->drawUI();
        delay(100);

        temperature->read(true); // read and notify the observers
        systemTime->notifierEngine();
        if(systemTime->isTimeUpdated())
        {
            //fbm->update(systemTime);
        }
        else
            Serial.println("warning: time is not available due to connection error at the system startup!");

    }
    return 0;
}