#pragma once

#include "LogDispatcherConfig.hpp"
#include <ArduinoJson.h>
#include "../EnumCrafter.hpp"

/**
 * @brief Construct a new Log Dispatcher Config:: Log Dispatcher Config object
 * 
 */
LogDispatcherConfig::LogDispatcherConfig(): 
    maxFileSizeBytes(1048576), // default 1MB
    maxFileRotationCount(5),   // default 5 files
    enabled(true),
    basePath("/dataLogs"),
    status(LogDispatcherStatus::Idle)
{}

/**
 * @brief Construct a new Log Dispatcher Config:: Log Dispatcher Config object
 * 
 * @param json JSON string to populate the configuration from.
 */
LogDispatcherConfig::LogDispatcherConfig(std::string json)
{
    this->populateFromJson(json);
}

/**
 * @brief Populate the LogDispatcherConfig from a JSON string (override the method in case of having more parameters).
 * @param json The JSON string to populate the config from.
 */
void LogDispatcherConfig::populateFromJson(std::string json)
{
    JsonDocument doc;
    deserializeJson(doc, json);
    maxFileSizeBytes = doc["maxFileSizeBytes"].as<size_t>();
    maxFileRotationCount = doc["maxFileRotationCount"].as<int>();
    enabled = doc["enabled"].as<bool>();
    basePath = doc["basePath"].as<std::string>();
}

/**
 * @brief Convert the LogDispatcherConfig object to a JSON string
 * 
 * @return std::string JSON string representation of the LogDispatcherConfig object
 */
std::string LogDispatcherConfig::toJson()
{
    JsonDocument doc;
    doc["maxFileSizeBytes"] = maxFileSizeBytes;
    doc["maxFileRotationCount"] = maxFileRotationCount;
    doc["enabled"] = enabled;
    doc["basePath"] = basePath;
    doc["status"] = EnumCrafter::toString(status);
    std::string output;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief Get the base path for log files.
 * 
 * @return std::string The base path for log files.
 */
std::string LogDispatcherConfig::getBasePath()
{
    return basePath;
}

/**
 * @brief Check if logging is enabled.
 * 
 * @return true If logging is enabled, false otherwise.
 */
bool LogDispatcherConfig::isEnabled()
{
    return enabled;
}

/**
 * @brief Get the maximum number of rotated log files.
 * 
 * @return int The maximum number of rotated log files.
 */
int LogDispatcherConfig::getMaxFileRotationCount()
{
    return maxFileRotationCount;
}

/**
 * @brief Get the maximum size of a log file in bytes.
 * 
 * @return size_t The maximum size of a log file in bytes.
 */
size_t LogDispatcherConfig::getMaxFileSizeBytes()
{
    return maxFileSizeBytes;
}

/**
 * @brief Get the current status of the log dispatcher.
 * 
 * @return LogDispatcherStatus The current status of the log dispatcher.
 */
LogDispatcherStatus LogDispatcherConfig::getStatus()
{
    return status;
}

/**
 * @brief Set the current status of the log dispatcher.
 * 
 * @param status The new status to set.
 */
void LogDispatcherConfig::setStatus(LogDispatcherStatus status)
{
    this->status = status;
}