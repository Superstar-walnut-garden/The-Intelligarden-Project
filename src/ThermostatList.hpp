#ifndef THERMOSTATLIST_HPP
#define THERMOSTATLIST_HPP

#include "ThermostatItem.hpp"
#include "BaseList.hpp"

class ThermostatList : public BaseList<ThermostatItem>
{
public:
    ThermostatList();
    ThermostatList(std::string json);
};

#endif // GPIOLIST_HPP