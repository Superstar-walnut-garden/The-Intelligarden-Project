#include "CppVirtualInterface.hpp"
#include <ESP32Time.h>
#include <FirebaseESP32.h>
#include <time.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <NTPClient.h>
#include <rom/rtc.h>
#include <SPIFFS.h>

#include "WifiService.hpp"
#include "SystemTimeService.hpp"
#include <Wire.h>
#include <U8g2lib.h>
#include "TempSensorService.hpp"
#include "DisplayService.hpp"

#include "FirebaseService.hpp"
#include "SystemMaintainer.hpp"
#include "ThermostatService.hpp"
#include "WebApiService.hpp"
#include "GpioService.hpp"
#include "SchedulerService.hpp"
#include "CentralizedSignalHubService.hpp"
#include "SignalRouterService.hpp"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "IExternalStorage.hpp"
#include "SpiMicroSd.hpp"
#include "LogDispatcherService.hpp"


int virtualMain()
{
    delay(500); // delay for hardware stablization
    pinMode(25, OUTPUT);
    if (!SPIFFS.begin(true))
        Serial.println("Fatal Error: An Error has occurred while mounting SPIFFS!");
    auto &systemMaintainer = SystemMaintainer::getInstance();
    systemMaintainer.start();
    systemMaintainer.refreshCycleTime(); // software implemented watchdog
    auto *wifiSetup = WifiService::getInstance();
    auto *webApiManager = new WebApiService();
    delay(500); // waiting utill reaching system stability
    auto *systemTime = SystemTimeService::getInstance();
    auto *configuration = Configuration::getInstance();
    auto *display = DisplayService::getInstance();
    auto *fbm = FirebaseService::getInstance();;
    auto *scheduler = SchedulerService::getInstance();
    auto *ioManager = GpioService::getInstance();
    auto *thermostatManager = ThermostatService::getInstance();
    auto *centralizedSignalHub = CentralizedSignalHubService::getInstance();
    auto *storage = SpiMicroSd::getInstance();
    auto *logDispatcher = LogDispatcherService::getInstance();
    auto *temperature = TempSensorService::getInstance();
    if(storage->isReady())
    {
        Serial.println("SD Card is ready!");
    }

    temperature->attach(display); // attach display as an observer
    temperature->attach(thermostatManager); // attach ThermostatService as an observer
    // systemTime->attach(fbm); // attach firebase-manager as an observer
    // systemTime->attach(scheduler); // attach scheduler as an observer
    // eventManager->registerListener(scheduler); // attach scheduler as an observer

    centralizedSignalHub->registerService(SignalRouterService::getInstance());
    centralizedSignalHub->registerService(ioManager);
    centralizedSignalHub->registerService(thermostatManager);
    centralizedSignalHub->registerService(scheduler);

    logDispatcher->attachLoggableService(temperature);
    logDispatcher->attachLoggableService(ioManager);

    display->drawUI();
    systemMaintainer.refreshCycleTime(); // software implemented watchdog
    
    webApiManager->init();
    

    // when internet is available
    wifiSetup->onOnline([systemTime, fbm]() 
    {
        systemTime->obtainTime();
        if(systemTime->isTimeUpdated())
        {
            fbm->init();
            fbm->update(systemTime);
        }
    });

    Serial.println("Sys-OK: All of the system components are initialized!");

    // if(!systemTime->isTimeUpdated() or !wifiSetup->isConnected())
    // {
    //     auto networkAbnormalityID = systemMaintainer.createTrackableAbnormality("network issue", 2); // system abnormality reported!
    //     Serial.println("Sys-Error: No internet access. Check your router! System will be rebooted 2 minutes later!");
    // }

    // eventManager->initializeListeners(); // initialize the listeners (should be after time retrival to ensure schedulers are correctly initialized).

    while(true)
    {
        systemMaintainer.refreshCycleTime(); // software implemented watchdog
        delay(1); // For other threads to work.this should be 1ms in the main setup
        Serial.print(wifiSetup->isConnected() ? "Wifi is Connected!" : "Wifi is disconnected!");
        display->drawUI();
        delay(100);

        temperature->loop(true); // read and notify the observers
        systemTime->notifierEngine();
        if(systemTime->isTimeUpdated()) // scheduler should only run if the time is updated (time subsystem initialized)
        {
            SchedulerService::getInstance()->loop();
        }
        else
            Serial.println("warning: time is not available due to connection error at the system startup!");
        
        logDispatcher->loop();
        wifiSetup->loop();
        ioManager->syncHardware();
        Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
        Serial.printf("Free Flash: %d bytes\n", SPIFFS.totalBytes() - SPIFFS.usedBytes());
        if(storage->isReady())
            Serial.printf("Free SD Card: %d bytes\n", storage->getRemainingCapacity());

        // Convert to time_t for formatting
        std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        // Format and print the time
        std::cout << "Current time: " << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S") << std::endl;
    }
    return 0;
}