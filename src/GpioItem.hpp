#pragma once

#include "BaseItem.hpp"
#include "SignalCompatibleBaseItem.hpp"
#include "ILoggableItem.hpp"
#include <string>

class GpioItem : public SignalCompatibleBaseItem, public ILoggableItem
{
public:
    GpioItem();
    GpioItem(int pin, std::string name, bool status, short mode, std::string extraParameters = ""); // pin = id

    int getPin() const;
    int getMode() const;
    std::string getExtraParameters() const;
    bool isInverted() const;
    int getHighDutyCycle() const;
    int getLastHighDutyCycle() const;
    void setLastHighDutyCycle(int lastHighDutyCycle);


    void setMode(short mode);
    void setExtraParameters(std::string extraParameters);

    virtual std::vector<std::string> getLocalSignalNames() override;

    // ILoggableItem interface
    std::string getName() const override;
    uint64_t getId() const override;
    uint64_t getInterval() const override;
    bool logOnlyOnChange() const override;
    std::string getData() const override;
    bool isLoggingEnabled() const override;

private:
    void populateDerivedClassFromJson(JsonDocument &json) override;
    void derivedClassToJson(JsonDocument &doc) const override;
    
    std::string extraParameters;
    short mode;
    bool inverted;
    int highDutyCycle, lastHighDutyCycle; // lastHighDutyCycle used to prevent unnecessary writes and flickers

    bool loggingEnabled;
    bool logOnlyOnDataChange;
    uint64_t logInterval;
};
