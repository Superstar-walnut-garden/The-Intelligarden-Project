#include "SystemMaintainer.hpp"
#include <ESP.h>

SystemMaintainer& SystemMaintainer::getInstance()
{
    static SystemMaintainer instance;
    return instance;
}

SystemMaintainer::SystemMaintainer() : running(false), abnormalCondition(false), nextRestartTime(std::chrono::steady_clock::now() + std::chrono::minutes(3)), cycleTimeInitialized(false) {}

SystemMaintainer::~SystemMaintainer()
{
    stop();
}

void SystemMaintainer::start()
{
    std::lock_guard<std::mutex> lock(mtx);
    if (!running)
    {
        running = true;
        loopThread = std::thread(&SystemMaintainer::loop, this);
    }
}

void SystemMaintainer::stop()
{
    std::lock_guard<std::mutex> lock(mtx);
    running = false;
    if (loopThread.joinable())
    {
        loopThread.join();
    }
}

void SystemMaintainer::setAbnormalCondition(bool condition)
{
    std::lock_guard<std::mutex> lock(mtx);
    abnormalCondition = condition;
    if (abnormalCondition)
    {
        nextRestartTime = std::chrono::steady_clock::now() + std::chrono::minutes(2);
    }
}

void SystemMaintainer::postponeRestart(int minutes)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto now = std::chrono::steady_clock::now();
    if (now >= nextRestartTime - std::chrono::minutes(minutes))
    {
        nextRestartTime = now + std::chrono::minutes(minutes);
    }
}

void SystemMaintainer::refreshCycleTime()
{
    std::lock_guard<std::mutex> lock(mtx);
    if (!cycleTimeInitialized)
    {
      lastCycleTime = std::chrono::steady_clock::now();
      cycleTimeInitialized = true;
    }
    else
    {
      lastCycleTime = std::chrono::steady_clock::now();
    }
}

void SystemMaintainer::loop()
{
    while (running)
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));

      auto now = std::chrono::steady_clock::now();
      
      // Check cycle time
      if (cycleTimeInitialized && (now - lastCycleTime > std::chrono::minutes(1)))
      {
        Serial.println("Sys-Maintainer: restarting... (Main-thread not responding)");
        delay(1000);
        ESP.restart();
      }

      // Check scheduled restart
      if (now >= nextRestartTime)
      {
        Serial.println("Sys-Maintainer: restarting... (due to general abnormality reported or a scheduled restart)");
        delay(1000);
        ESP.restart();
      }
    }
}
