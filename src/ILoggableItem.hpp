#pragma once

#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <ctime>

class ILoggableItem
{
public:
    virtual std::string getName() const = 0;
    virtual uint64_t getId() const = 0;
    virtual uint64_t getInterval() const = 0;
    virtual bool logOnlyOnChange() const = 0;
    virtual std::string getData() const = 0;
    virtual bool isLoggingEnabled() const = 0;

    virtual ~ILoggableItem() = default; // this line is super important. Never forget it!
};
