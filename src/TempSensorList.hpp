#pragma once

#include "BaseList.hpp"
#include "FusionBusItem.hpp"

class TempSensorList : public BaseList<FusionBusItem>
{
public:
    TempSensorList();
    TempSensorList(std::string json);
    void repopulateWith(std::string json) override;
    static std::unique_ptr<FusionBusItem> createObjectFromType(FusionBusItem::DeviceType devType);
    static std::unique_ptr<FusionBusItem> createObjectFromType(std::string json);
};
