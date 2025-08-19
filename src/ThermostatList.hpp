#pragma once

#include "ThermostatItem.hpp"
#include "BaseList.hpp"

class ThermostatList : public BaseList<ThermostatItem>
{
public:
    ThermostatList();
    ThermostatList(std::string json);
};
