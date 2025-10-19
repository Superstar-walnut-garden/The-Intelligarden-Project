#pragma once

#include <vector>
#include <string>
#include "BaseItem.hpp"
#include "ILoggableItem.hpp"

class TempSensorItem: public BaseItem, public ILoggableItem
{
public:
    TempSensorItem();
    TempSensorItem(uint64_t id, std::string name = "", bool isConnected = true);

    bool operator == (TempSensorItem &obj);
    bool isConnected() const;
    double getTemp() const;
    void setTemp(double temp);

    // ILoggableItem interface
    std::string getName() const override;
    uint64_t getId() const override;
    uint64_t getInterval() const override;
    bool logOnlyOnChange() const override;
    std::string getData() const override;
    bool isLoggingEnabled() const override;

    void populateDerivedClassFromJson(JsonDocument &json) override;
    void derivedClassToJson(JsonDocument &doc) override;

private:
    double temp;
};
