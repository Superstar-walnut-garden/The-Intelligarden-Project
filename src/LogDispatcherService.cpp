#include "LogDispatcherService.hpp"
#include "SpiMicroSd.hpp"
#include "LogUtils.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "Configuration.hpp"
#include <functional>
#include "JsonUtils.hpp"

/**
 * @brief static member initialization
 */
LogDispatcherService* LogDispatcherService::instance = nullptr;

/**
 * @brief Get the singleton instance of the LogDispatcherService.
 * 
 * @return LogDispatcherService* Pointer to the singleton instance.
 */
LogDispatcherService* LogDispatcherService::getInstance()
{
    if (!instance)
    {
        instance = new LogDispatcherService();
    }
    return instance;
}

/**
 * @brief Constructor for LogDispatcherService.
 * 
 */
LogDispatcherService::LogDispatcherService(): storage(SpiMicroSd::getInstance())
{
    restoreAll();
    determineStatus();
}

/**
 * @brief Attach a loggable service to the dispatcher.
 * 
 * @param service Pointer to the loggable service to attach.
 */
void LogDispatcherService::attachLoggableService(ILoggableService* service)
{
    loggableServices.push_back(service);
}

/**
 * @brief Main loop of the LogDispatcherService.
 * 
 * This function iterates through all attached loggable services and processes their loggable items.
 */
void LogDispatcherService::loop()
{
    determineStatus();
    if(config.getStatus() == LogDispatcherStatus::Running) // only log if status is Running (everything is ok)
    {
        for (const auto& service : loggableServices) // iterate through services
        {
            auto items = service->getLoggableItems();
            for (const auto& item : items) // iterate through loggable items
            {
                int fileRotationCount = 0;
                do // try to find a suitable file to log into
                {                
                    std::string path = config.getBasePath() + "/" + LogUtils::buildPath(service->getName(), item->getName(), item->getId(), fileRotationCount++);
                    if(storage->getFileSize(path) < config.getMaxFileSizeBytes()) // check file size limit
                    {
                        bool fileUpdated = false;
                        auto fileContents = storage->readFile(path); // read existing file (to keep previous logs)
                        JsonDocument doc, nested;
                        deserializeJson(doc, fileContents);
                        if (!deserializeJson(nested, item->getData())) // if data is valid JSON and deserialized successfully
                        {
                            auto addNewLogEntry = [&]()
                            {
                                fileUpdated = true;
                                doc[LogUtils::buildTimestamp()] = nested; // add new log entry with current timestamp
                            };
                            auto lastEntry = JsonUtils::getLastEntry(doc);
                            if(lastEntry) // if there is a last entry (a data point already logged for this day).
                            {
                                // compare their timestamps and based on the interval decide whether to log or not
                                auto lastTimestamp = LogUtils::parseTimestamp(lastEntry->key);
                                auto currentTimestamp = LogUtils::parseTimestamp(LogUtils::buildTimestamp());
                                auto interval = std::chrono::seconds(item->getInterval());
                                if(currentTimestamp - lastTimestamp >= interval) // if should log now (based on interval)
                                {
                                    if(item->logOnlyOnChange())
                                    {
                                        // compare last logged value with current value
                                        JsonDocument lastValueDoc, currentValueDoc;
                                        deserializeJson(lastValueDoc, lastEntry->value);
                                        deserializeJson(currentValueDoc, item->getData());
                                        if(!JsonUtils::jsonEquals(lastValueDoc.as<JsonObject>(), currentValueDoc.as<JsonObject>()))
                                            addNewLogEntry();
                                    }
                                    else
                                        addNewLogEntry();
                                } // else do not log (interval not reached)
                            }
                            else
                            {
                                addNewLogEntry();
                            }
                        }
                        else
                            Serial.println("error: logDispatcherService: Failed to parse JSON");
                        if(fileUpdated)
                        {
                            serializeJson(doc, fileContents); // serialize back to string
                            storage->writeFile(path, fileContents); // write updated contents back to file
                        }
                        break; // exit the loop after finding a suitable file
                    }
                } while(fileRotationCount < config.getMaxFileRotationCount()); // limit to 128 rotated files

            }
        }
    }
}

/**
 * @brief Get the current configuration as a JSON string.
 * 
 * @return std::string JSON string representation of the current configuration.
 */
std::string LogDispatcherService::getConfig()
{
    return config.toJson();
}

/**
 * @brief Update the configuration from a LogDispatcherConfig object.
 * 
 * @param config The new configuration to set.
 */
void LogDispatcherService::updateConfig(LogDispatcherConfig config)
{
    config.setStatus(this->config.getStatus()); // preserve current status
    this->config = config;
    storeAll(); // save updated config to storage
}

/**
 * @brief Store the current configuration to persistent storage.
 * 
 */
void LogDispatcherService::storeAll()
{
    Configuration::getInstance()->setLogDispatcherConfig(config.toJson());
}

/**
 * @brief Restore the configuration from persistent storage.
 * 
 */
void LogDispatcherService::restoreAll()
{
    auto cfg = Configuration::getInstance()->getLogDispatcherConfig();
    if(!cfg.empty())
    {
        config.populateFromJson(cfg);
    }
}

/**
 * @brief Check if the storage is full based on the configured maximum file size.
 * 
 * @return true If the storage is considered full (less than 2 full-sized(max log file size) files), false otherwise.
 */
bool LogDispatcherService::isStorageFull()
{
    return storage->getRemainingCapacity() < (config.getMaxFileSizeBytes() * 2);
}

/**
 * @brief Determine and update the current status of the LogDispatcherService.
 * 
 */
void LogDispatcherService::determineStatus()
{
    if(!storage->isReady())
        config.setStatus(LogDispatcherStatus::StorageNotReadyError);
    else if(isStorageFull())
        config.setStatus(LogDispatcherStatus::StorageFullError);
    else if(!config.isEnabled())
        config.setStatus(LogDispatcherStatus::Idle);
    else
        config.setStatus(LogDispatcherStatus::Running);
}