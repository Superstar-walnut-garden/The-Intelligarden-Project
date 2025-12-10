#pragma once

#include <string>
#include <vector>
#include "ISignalCompatibleItem.hpp"
#include "Subject.hpp"

class ISignalCompatibleService: public Subject<ISignalCompatibleService>
{
public:
    virtual std::string getName() const = 0;
    virtual std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() = 0;
};
