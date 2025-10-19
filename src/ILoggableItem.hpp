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

    virtual bool shouldLog(std::chrono::time_point<std::chrono::system_clock> now) const
    {
        return std::chrono::duration_cast<std::chrono::seconds>(now - lastLogTime).count() >= getInterval();
    }

    // updates lastLogTime and returns data
    virtual void setLastLogTime(std::chrono::time_point<std::chrono::system_clock> now)
    {
        lastLogTime = now;
    }

    virtual std::chrono::time_point<std::chrono::system_clock> getLastLogTime() const
    {
        return lastLogTime;
    }

    virtual ~ILoggableItem() = default; // this line is super important. Never forget it!

private:
    std::chrono::time_point<std::chrono::system_clock> lastLogTime;
};
