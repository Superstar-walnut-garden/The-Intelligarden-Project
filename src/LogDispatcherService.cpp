#include "LogDispatcherService.hpp"
#include "SpiMicroSd.hpp"
#include "LogPathBuilder.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "Configuration.hpp"
#include <functional>

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
    if(config.getStatus() == LogDispatcherStatus::Running)
    {
        for (const auto& service : loggableServices)
        {
            auto items = service->getLoggableItems();
            for (const auto& item : items)
            {
                int fileRotationCount = 0;
                do
                {                
                    std::string path = config.getBasePath() + "/" + LogPathBuilder::build(service->getName(), item->getName(), item->getId(), fileRotationCount++);
                    if(storage->getFileSize(path) < config.getMaxFileSizeBytes()) // limit file size to 10KB
                    {
                        auto nowTime = std::chrono::system_clock::now();
                        if(item->shouldLog(nowTime)) // if item should be logged now (based on interval and last log time and non-redundant data)
                        {
                            auto fileContents = storage->readFile(path); // read existing file (to keep previous logs)
                            JsonDocument doc, nested;
                            deserializeJson(doc, fileContents);
                            if (!deserializeJson(nested, item->getData())) // if data is valid JSON and deserialized successfully
                            { 
                                doc[LogPathBuilder::buildTimestamp()] = nested; // add new log entry with current timestamp
                                service->setLastLogTime(item->getId(), nowTime); // update last log time for the item
                            }
                            else
                                Serial.println("error: logDispatcherService: Failed to parse JSON");
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
 * @brief Get all log data as a JSON string.
 * 
 * @return std::string JSON string representation of all log data.
 */
std::string LogDispatcherService::getAll()
{
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    root["path"] = config.getBasePath();

    auto ensureTrailingSlash = [](const std::string& p) {
        if (p.empty()) return std::string("/");
        if (p.back() == '/') return p;
        return p + '/';
    };

    std::function<void(const std::string&, JsonObject)> recurse;
    recurse = [&](const std::string& currentPath, JsonObject parentObj)
    {
        JsonArray foldersArray = parentObj.createNestedArray("folders");
        JsonArray filesArray   = parentObj.createNestedArray("files");

        auto list = storage->listEntries(currentPath);
        for (const auto& entry : list)
        {
            std::string fullPath = ensureTrailingSlash(currentPath) + entry.name;

            if (entry.isDirectory)
            {
                JsonObject folderObj = foldersArray.createNestedObject();
                folderObj["name"] = entry.name;       // raw string
                folderObj["path"] = fullPath;         // raw string
                recurse(fullPath, folderObj);         // recurse into folder
            }
            else
            {
                JsonObject fileObj = filesArray.createNestedObject();
                fileObj["name"] = entry.name;         // raw string
                fileObj["path"] = fullPath;           // raw string
                fileObj["size"] = storage->getFileSize(fullPath);
                // fileObj["lastModified"] = storage->getLastModified(fullPath);
            }
        }
    };

    recurse(config.getBasePath(), root);

    std::string output;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief Get log data for a specific path.
 * 
 * @param path The path to retrieve log data from.
 * @return std::string JSON string representation of the log data at the specified path.
 */
std::string LogDispatcherService::get(std::string path)
{
    // read file contents from storage based on the provided path
    return storage->readFile(path);
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