#pragma once

#include <vector>
#include <string>
#include "BaseItem.hpp"
#include "ILoggableItem.hpp"

class TempSensorItem: public BaseItem, public ILoggableItem
{
public:
    TempSensorItem(uint64_t id = -1, std::string name = "", bool isConnected = false);
    
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

    void setInterval(uint64_t interval);
    void setLogOnlyOnChange(bool status);
    void setLoggingEnabled(bool status);

private:
    double temp;

    bool loggingEnabled;
    bool logOnlyOnDataChange;
    uint64_t logInterval;
};
