#pragma once

#include "BaseList.hpp"
#include "FusionBusItem.hpp"

class TempSensorList : public BaseList<FusionBusItem>
{
public:
    TempSensorList();
    TempSensorList(std::string json);
    void repopulateWith(std::string json) override;
};
