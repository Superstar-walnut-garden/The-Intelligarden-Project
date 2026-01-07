#pragma once

#include "TempSensorItem.hpp"
#include "BaseList.hpp"

class TempSensorList : public BaseList<TempSensorItem>
{
public:
    TempSensorList();
    TempSensorList(std::string json);
    void clearList();
    TempSensorItem *getItem(std::string name);
    using BaseList::getItem;
};
