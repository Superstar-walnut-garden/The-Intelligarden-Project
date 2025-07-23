#ifndef ISIGNALCOMPATIBLEITEM_HPP
#define ISIGNALCOMPATIBLEITEM_HPP
#include <string>
#include <vector>

class ISignalCompatibleItem
{
public:
    virtual std::vector<std::string> getLocalSignalNames() = 0;
    virtual std::string getName() const = 0;
    virtual uint64_t getId() const = 0;
};

#endif