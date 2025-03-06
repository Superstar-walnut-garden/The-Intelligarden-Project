#ifndef TEMPSENSORITEM_HPP
#define TEMPSENSORITEM_HPP

#include <vector>
#include <string>
#include "BaseItem.hpp"

class TempSensorItem: public BaseItem
{
public:
    TempSensorItem();
    TempSensorItem(uint64_t id, std::string name = "", bool isConnected = true);

    bool operator == (TempSensorItem &obj);
    bool isConnected();
    double getTemp();
    void setTemp(double temp);

    void populateDerivedClassFromJson(JsonDocument &json) override;
    void derivedClassToJson(JsonDocument &doc) override;

private:
    double temp;
};

#endif // TEMPSENSORITEM_HPP
