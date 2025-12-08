#pragma once
#include <ArduinoJson.h>
#include <string>
#include <iostream>
#include <IJsonSerializable.hpp>
#include "LogDispatcherStatus.hpp"

class LogDispatcherConfig: public IJsonSerializable
{
    public:
    LogDispatcherConfig();
    LogDispatcherConfig(std::string json);
    std::string toJson() override;
    void populateFromJson(std::string json) override;

    std::string getBasePath();
    bool isEnabled();
    int getMaxFileRotationCount();
    size_t getMaxFileSizeBytes();
    LogDispatcherStatus getStatus();
    void setStatus(LogDispatcherStatus status);

    private:
    size_t maxFileSizeBytes; // maximum size of a log file in bytes
    int maxFileRotationCount; // maximum number of rotated log files
    bool enabled; // logging enabled or not
    std::string basePath; // base path for log files
    LogDispatcherStatus status; // current status of the log dispatcher
};