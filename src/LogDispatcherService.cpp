#include "LogDispatcherService.hpp"
#include "SpiMicroSd.hpp"
#include "LogPathBuilder.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>

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
    if(storage->isReady() and storage->getRemainingCapacity() > (1024 * 1024)) // ensure storage is ready and has enough space (at least 1MB free)
    {
        for (const auto& service : loggableServices)
        {
            auto items = service->getLoggableItems();
            for (const auto& item : items)
            {
                int fileRotationCount = 0;
                do
                {                
                    std::string path = LogPathBuilder::build(service->getName(), item->getName(), item->getId(), fileRotationCount++);
                    if(storage->getFileSize(path) < 10 * 1024) // limit file size to 10KB
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
                } while(fileRotationCount < 128); // limit to 128 rotated files

            }
        }
    }
}