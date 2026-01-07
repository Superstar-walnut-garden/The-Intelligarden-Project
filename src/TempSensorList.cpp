#include "TempSensorList.hpp"

/**
 * @brief Construct a new Temp Sensor List:: Temp Sensor List object
 * 
 */
TempSensorList::TempSensorList(): BaseList()
{

}

/**
 * @brief construct a new Temp Sensor List:: Temp Sensor List object
 * 
 */
TempSensorList::TempSensorList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug purposes
}

/**
 * @brief Clear the list
 * 
 */
void TempSensorList::clearList()
{
    getList().clear();
}

/**
 * @brief Get a TempSensorItem object by its name.
 * 
 * @param name The name of the object to get.
 * @return TempSensorItem* The object with the specified name.
 */
TempSensorItem *TempSensorList::getItem(std::string name)
{
    TempSensorItem *foundItem = nullptr;
    forEach([name, &foundItem](TempSensorItem *item)
    {
        if (item->getName() == name)
            foundItem = item;
    });
        
    return foundItem; // return an empty object if not found
}