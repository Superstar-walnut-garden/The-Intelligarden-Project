#ifndef ISIGNALCOMPATIBLEMANAGER_HPP
#define ISIGNALCOMPATIBLEMANAGER_HPP
#include <string>
#include <vector>
#include "ISignalCompatibleItem.hpp"
#include "Subject.hpp"

class ISignalCompatibleService: public Subject<ISignalCompatibleService>
{
public:
    virtual std::string getName() = 0;
    virtual std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() = 0;
};

#endif