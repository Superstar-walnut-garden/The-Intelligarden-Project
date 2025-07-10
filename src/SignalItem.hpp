#ifndef SIGNALITEM_HPP
#define SIGNALITEM_HPP
#include <ArduinoJson.h>
#include <string>
#include <vector>
#include "BaseItem.hpp"

class SignalItem: public BaseItem
{
public:
    SignalItem();
    // SignalItem(uint64_t id, std::string name);
    std::vector<std::string> getListeners();
    std::string getBroadcaster();

private:
    void populateDerivedClassFromJson(JsonDocument &doc) override;
    void derivedClassToJson(JsonDocument &doc) override;

    std::vector<std::string> listeners;
    std::string broadcaster;
};

#endif