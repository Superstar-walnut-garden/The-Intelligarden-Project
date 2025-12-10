#pragma once

#include <string>
#include <vector>
#include "ILoggableItem.hpp"
#include <memory>

class ILoggableService
{
public:
    virtual ~ILoggableService() = default;

    virtual std::string getName() const = 0;
    virtual std::vector<std::unique_ptr<ILoggableItem>> getLoggableItems() const = 0;
};