#ifndef GPIOITEM_HPP
#define GPIOITEM_HPP

#include "BaseItem.hpp"
#include <string>

class GPIOItem : public BaseItem
{
public:
    GPIOItem();
    GPIOItem(int pin, int event_id, std::string name, bool status, short mode, std::string extraParameters = ""); // pin = id

    int getPin();
    int getMode();
    std::string getExtraParameters();

    void setMode(short mode);
    void setExtraParameters(std::string extraParameters);

private:
    void populateDerivedClassFromJson(JsonDocument &json) override;
    void derivedClassToJson(JsonDocument &doc) override;
    
    std::string extraParameters;
    short mode;

};

#endif // GPIOITEM_HPP