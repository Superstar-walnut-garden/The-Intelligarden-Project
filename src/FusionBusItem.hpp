#pragma once

#include "BaseItem.hpp"
#include "SignalCompatibleBaseItem.hpp"
#include "ILoggableItem.hpp"
#include <string>

class FusionBusItem : public SignalCompatibleBaseItem, public ILoggableItem
{
public:
    enum class DeviceType
    {
        VentDrive,
        TempSensor,
        SoilSensor,
        Unknown
    };
    FusionBusItem(DeviceType type = DeviceType::Unknown);
    DeviceType getType() const { return type; }
    
    virtual std::vector<std::string> getLocalSignalNames() override { return {}; };

    // ILoggableItem interface
    std::string getName() const override {return SignalCompatibleBaseItem::getName(); }
    uint64_t getId() const override {return SignalCompatibleBaseItem::getId(); }
    uint64_t getInterval() const override { return 0; }
    bool logOnlyOnChange() const override { return false; }
    std::string getData() const override { return ""; }
    bool isLoggingEnabled() const override { return false; }

private:
    DeviceType type;
};
