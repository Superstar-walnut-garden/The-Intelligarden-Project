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
#include "WebApiManager.hpp"
#include "SystemTime.hpp"
#include <Wire.h>
#include <U8g2lib.h>
#include "Temperature.hpp"
#include "Display.hpp"

#include "FirebaseManager.hpp"
#include "SystemMaintainer.hpp"
#include "EventManager.hpp"
#include "ThermostatManager.hpp"


int virtualMain()
{
    delay(500); // delay for hardware stablization
    pinMode(25, OUTPUT);
    if (!SPIFFS.begin(true))
        Serial.println("Fatal Error: An Error has occurred while mounting SPIFFS!");
    auto &systemMaintainer = SystemMaintainer::getInstance();
    systemMaintainer.start();
    systemMaintainer.refreshCycleTime(); // software implemented watchdog
    auto *wifiSetup = WifiSetup::getInstance();
    auto *webApiManager = new WebApiManager();
    delay(500); // waiting utill reaching system stability
    auto *systemTime = SystemTime::getInstance();
    auto *temperature = Temperature::getInstance();
    auto *configuration = Configuration::getInstance();
    auto *display = Display::getInstance();
    auto *fbm = new FirebaseManager(Configuration::getInstance()->getFirebaseData());
    auto *eventManager = EventManager::getInstance();
    auto *scheduler = Scheduler::getInstance();
    auto *ioManager = GPIOManager::getInstance();
    auto *thermostatManager = ThermostatManager::getInstance();

    temperature->attach(display); // attach display as an observer
    temperature->attach(thermostatManager); // attach ThermostatManager as an observer
    systemTime->attach(fbm); // attach firebase-manager as an observer
    systemTime->attach(scheduler); // attach scheduler as an observer
    eventManager->registerListener(ioManager); // attach GPIOManager as an observer
    eventManager->registerListener(thermostatManager); // attach ThermostatManager as an observer
    eventManager->registerListener(scheduler); // attach scheduler as an observer

    display->drawUI();
    systemMaintainer.refreshCycleTime(); // software implemented watchdog
    
    webApiManager->init();
    

    

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
    Serial.println("Sys-OK: All of the system components are initialized!");

    if(!systemTime->isTimeUpdated() or !wifiSetup->isConnected())
    {
        auto networkAbnormalityID = systemMaintainer.createTrackableAbnormality("network issue", 2); // system abnormality reported!
        Serial.println("Sys-Error: No internet access. Check your router! System will be rebooted 2 minutes later!");
    }

    eventManager->initializeListeners(); // initialize the listeners (should be after time retrival to ensure schedulers are correctly initialized).

    while(true)
    {
        systemMaintainer.refreshCycleTime(); // software implemented watchdog
        delay(1); // For other threads to work.this should be 1ms in the main setup
        Serial.println(WiFi.status() == WL_CONNECTED ? "Wifi is Connected!" : "Fatal Error: Wifi is disconnected!!!");
        display->drawUI();
        delay(100);

        temperature->read(true); // read and notify the observers
        systemTime->notifierEngine();
        if(systemTime->isTimeUpdated())
        {
            Scheduler::getInstance()->update(systemTime); // manualy updating the scheduler for debug purposes
        }
        else
            Serial.println("warning: time is not available due to connection error at the system startup!");
        
        eventManager->loop();
        Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    }
    return 0;
}