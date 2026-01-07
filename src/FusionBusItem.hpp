#pragma once

#include "BaseItem.hpp"
#include "SignalCompatibleBaseItem.hpp"
#include "ILoggableItem.hpp"
#include <string>

class FusionBusItem : public SignalCompatibleBaseItem, public ILoggableItem
{
public:
    FusionBusItem();

    virtual std::vector<std::string> getLocalSignalNames() override {};

    // ILoggableItem interface
    std::string getName() const override {return SignalCompatibleBaseItem::getName(); }
    uint64_t getId() const override {return SignalCompatibleBaseItem::getId(); }
    uint64_t getInterval() const override { return 0; }
    bool logOnlyOnChange() const override { return false; }
    std::string getData() const override { return ""; }
    bool isLoggingEnabled() const override { return false; }

private:
    std::string type;
};
