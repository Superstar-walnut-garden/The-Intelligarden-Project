#ifndef TEMPSENSORLIST_HPP
#define TEMPSENSORLIST_HPP

#include "TempSensorItem.hpp"
#include "BaseList.hpp"

class TempSensorList : public BaseList<TempSensorItem>
{
public:
    TempSensorList();
    TempSensorList(std::string json);
    void clearList();
    bool doesExist(TempSensorItem &desiredItem);
    TempSensorItem &getItem(std::string name);
    using BaseList::getItem;
};

#endif // TEMPSENSORLIST_HPP