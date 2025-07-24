#include "SystemMaintainer.hpp"
#include <ESP.h>

/**
 * @brief get SystemMaintainer singleton instance for managing system state and abnormalities.
 * 
 * @return SystemMaintainer& 
 */
SystemMaintainer& SystemMaintainer::getInstance()
{
    static SystemMaintainer instance;
    return instance;
}

/**
 * @brief Construct a new System Maintainer:: System Maintainer object
 * 
 */
SystemMaintainer::SystemMaintainer() : running(false), generalAbnormalCondition(false), nextRestartTime(std::chrono::steady_clock::now() + std::chrono::minutes(480)), cycleTimeInitialized(false), abnormalityCounter(0), restartDelayed(false) {}

/**
 * @brief Destroy the System Maintainer:: System Maintainer object
 * 
 */
SystemMaintainer::~SystemMaintainer()
{
    stop();
}

/**
 * @brief Start the SystemMaintainer loop in a separate thread.
 * 
 */
void SystemMaintainer::start()
{
    std::lock_guard<std::mutex> lock(mtx);
    if (!running)
    {
        running = true;
        loopThread = std::thread(&SystemMaintainer::loop, this);
    }
}

/**
 * @brief Check if the system is running.
 * 
 * @return true if the system is running, false otherwise.
 */
void SystemMaintainer::stop()
{
    std::lock_guard<std::mutex> lock(mtx);
    running = false;
    if (loopThread.joinable())
    {
        loopThread.join();
    }
}

/**
 * @brief Set the general abnormal condition of the system.
 * 
 * @param condition true if there is a general abnormal condition, false otherwise.
 */
void SystemMaintainer::setGeneralAbnormalCondition(bool condition)
{
    std::lock_guard<std::mutex> lock(mtx);
    generalAbnormalCondition = condition;
    if (generalAbnormalCondition)
    {
        nextRestartTime = std::chrono::steady_clock::now() + std::chrono::minutes(2);
    }
}

/**
 * @brief Get the current general abnormal condition of the system.
 * 
 * @return id of the created trackable abnormality
 */
int SystemMaintainer::createTrackableAbnormality(const std::string& description, int restartTimeMinutes)
{
    std::lock_guard<std::mutex> lock(mtx);
    int id = ++abnormalityCounter;
    abnormalities[id] = { description, std::chrono::steady_clock::now(), restartTimeMinutes, false };
    return id;
}

/**
 * @brief Resolve a trackable abnormality by its ID.
 * 
 * @param id The ID of the abnormality to resolve.
 */
void SystemMaintainer::resolveAbnormality(int &id)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (abnormalities.find(id) != abnormalities.end())
    {
        abnormalities[id].resolved = true;
        id = -1;
    }
}

/**
 * @brief Postpone a scheduled restart for a specified number of minutes.
 * 
 * @param minutes The number of minutes to postpone the restart.
 */
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

/**
 * @brief Get the next scheduled restart time.
 * 
 * @return std::chrono::steady_clock::time_point The next scheduled restart time.
 */
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

/**
 * @brief Main loop for the SystemMaintainer to check for abnormalities and manage system state.
 * 
 */
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
