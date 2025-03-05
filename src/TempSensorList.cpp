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
    getListRef().clear();
}

/**
 * @brief Check if a TempSensorItem object exists in the list.
 * 
 * @param list The list to search in.
 * @param node The object to search for.
 * @return true If the object exists in the list.
 * @return false If the object does not exist in the list.
 */
bool TempSensorList::doesExist(TempSensorItem &desiredItem)
{
    for (auto &item : getList())
        if (item == desiredItem) // compare the addresses
            return true;
    return false;
}

/**
 * @brief Get a TempSensorItem object by its name.
 * 
 * @param name The name of the object to get.
 * @return TempSensorItem& The object with the specified name.
 */
TempSensorItem &TempSensorList::getItem(std::string name)
{
    static auto nullItem = TempSensorItem();
    for (auto &item : getList())
        if (item.getName() == name)
            return item;
    return nullItem; // return an empty object if not found
}