#include "GPIOItem.hpp"
#include "SignalNameResolver.hpp"

GPIOItem::GPIOItem()
    : SignalCompatibleBaseItem(), mode(0), extraParameters("")
{
}

GPIOItem::GPIOItem(int pin, std::string name, bool status, short mode, std::string extraParameters)
    : SignalCompatibleBaseItem(pin, name, status), mode(mode), extraParameters(extraParameters)
{
}

int GPIOItem::getPin()
{
    return this->getId();
}

int GPIOItem::getMode()
{
    return this->mode;
}

void GPIOItem::setMode(short mode)
{
    this->mode = mode;
}

std::string GPIOItem::getExtraParameters()
{
    return this->extraParameters;
}

void GPIOItem::setExtraParameters(std::string extraParameters)
{
    this->extraParameters = extraParameters;
}

void GPIOItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    this->extraParameters = doc["extraParameters"].as<std::string>();
    this->mode = doc["mode"].as<short>();
}

void GPIOItem::derivedClassToJson(JsonDocument &doc)
{
    doc["extraParameters"] = this->extraParameters;
    doc["mode"] = this->mode;
}

std::vector<std::string> GPIOItem::getLocalSignalNames()
{
    // Dynamic SignalType and Name Generation
    SignalNameResolver::SignalType signalType;
    if(mode == 0) // if input, it broadcasts the pin status to a signal
        signalType = SignalNameResolver::SignalType::Broadcaster;
    else
        signalType = SignalNameResolver::SignalType::Listener;

    return {SignalNameResolver::generateLocalSignalName("default", signalType)};
}