#ifndef ISIGNALCOMPATIBLEITEM_HPP
#define ISIGNALCOMPATIBLEITEM_HPP
#include <string>
#include <vector>

class ISignalCompatibleItem
{
public:
    virtual std::string getName() = 0;
    virtual std::string getSubsystemName() = 0;
    virtual int getId() = 0;
    virtual std::vector<std::string> getLocalSignalNames() = 0;
};

#endif