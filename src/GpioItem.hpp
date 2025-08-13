#pragma once

#include "BaseItem.hpp"
#include "SignalCompatibleBaseItem.hpp"
#include <string>

class GpioItem : public SignalCompatibleBaseItem
{
public:
    GpioItem();
    GpioItem(int pin, std::string name, bool status, short mode, std::string extraParameters = ""); // pin = id

    int getPin();
    int getMode();
    std::string getExtraParameters();

    void setMode(short mode);
    void setExtraParameters(std::string extraParameters);

    virtual std::vector<std::string> getLocalSignalNames() override;

private:
    void populateDerivedClassFromJson(JsonDocument &json) override;
    void derivedClassToJson(JsonDocument &doc) override;
    
    std::string extraParameters;
    short mode;

};
