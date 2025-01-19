#include "SystemMaintainer.hpp"
#include <ESP.h>

SystemMaintainer& SystemMaintainer::getInstance() {
  static SystemMaintainer instance;
  return instance;
}

SystemMaintainer::SystemMaintainer() : running(false), abnormalCondition(false), nextRestartTime(std::chrono::steady_clock::now() + std::chrono::minutes(3)), lastCycleTime(std::chrono::steady_clock::now()) {}

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

void SystemMaintainer::monitorCycle() 
{
  std::lock_guard<std::mutex> lock(mtx);
  auto now = std::chrono::steady_clock::now();
  if (now - lastCycleTime > std::chrono::minutes(2)) 
  {
    ESP.restart();
  }
  lastCycleTime = now;
}

void SystemMaintainer::loop() 
{
  while (running) 
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (std::chrono::steady_clock::now() >= nextRestartTime) 
    {
      nextRestartTime = std::chrono::steady_clock::now() + std::chrono::minutes(abnormalCondition ? 2 : 480);
      Serial.println("Sys-Maintainer: restarting...");
      ESP.restart();
    }
  }
}
