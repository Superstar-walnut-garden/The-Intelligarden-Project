#include "SystemMaintainer.hpp"
#include <ESP.h>

SystemMaintainer& SystemMaintainer::getInstance()
{
    static SystemMaintainer instance;
    return instance;
}

SystemMaintainer::SystemMaintainer() : running(false), generalAbnormalCondition(false), nextRestartTime(std::chrono::steady_clock::now() + std::chrono::minutes(480)), cycleTimeInitialized(false), abnormalityCounter(0), restartDelayed(false) {}

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

void SystemMaintainer::setGeneralAbnormalCondition(bool condition)
{
    std::lock_guard<std::mutex> lock(mtx);
    generalAbnormalCondition = condition;
    if (generalAbnormalCondition)
    {
        nextRestartTime = std::chrono::steady_clock::now() + std::chrono::minutes(2);
    }
}

int SystemMaintainer::createTrackableAbnormality(const std::string& description, int restartTimeMinutes)
{
    std::lock_guard<std::mutex> lock(mtx);
    int id = ++abnormalityCounter;
    abnormalities[id] = { description, std::chrono::steady_clock::now(), restartTimeMinutes, false };
    return id;
}

void SystemMaintainer::resolveAbnormality(int &id)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (abnormalities.find(id) != abnormalities.end())
    {
        abnormalities[id].resolved = true;
        id = -1;
    }
}

void SystemMaintainer::postponeRestart(int minutes)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto now = std::chrono::steady_clock::now();

    // Disable restarts for the specified duration (e.g., 2 minutes)
    disableRestartUntil = now + std::chrono::minutes(2);
    restartDelayed = true;

    // Postpone the scheduled restart
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

        // Perform regular checks
        if (!restartDelayed)
        {
            if (cycleTimeInitialized && (now - lastCycleTime > std::chrono::minutes(1)))
            {
                Serial.println("Sys-Maintainer: restarting... (due to main-thread not responding)");
                delay(1000);
                ESP.restart();
            }

            if (now >= nextRestartTime)
            {
                Serial.println("Sys-Maintainer: restarting... (due to scheduled restart or general abnormality)");
                delay(1000);
                ESP.restart();
            }

            // Check trackable abnormalities
            for (const auto& pair : abnormalities)
            {
                if (!pair.second.resolved && (now >= pair.second.timestamp + std::chrono::minutes(pair.second.restartTimeMinutes)))
                {
                    Serial.println("Sys-Maintainer: restarting... (due to trackable abnormality)");
                    Serial.print("abnormality description: ");
                    Serial.println(pair.second.description.c_str());
                    delay(1000);
                    ESP.restart();
                }
            }
        }

        // Manage the postponement period
        if (restartDelayed && now >= disableRestartUntil)
        {
            restartDelayed = false; // Reset the flag
        }
    }
}
