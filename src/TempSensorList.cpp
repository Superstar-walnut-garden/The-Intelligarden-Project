#include "TempSensorList.hpp"
#include "../EnumCrafter.hpp"
#include "VentDriveItem.hpp"
#include "TempSensorItem.hpp"

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
 * @brief Populate the list with items from a JSON string.
 * 
 * @param json json list of items to populate the list with
 */
void TempSensorList::repopulateWith(std::string json)
{
    JsonDocument doc;
    deserializeJson(doc, json);
    auto &list = getList(); // get list reference
    list.clear(); // delete the old items before adding new ones

    for (JsonObject item : doc.as<JsonArray>()) 
    {
        std::string jsonStr;
        serializeJson(item, jsonStr);
        if(auto fusionItem = createObjectFromType(jsonStr))
        {
            fusionItem->populateFromJson(jsonStr);
            list.push_back(std::move(fusionItem));
        }
    }
}

/**
 * @brief create a FusionBusItem derived object using device type
 * @details currently supported types are TempSensor and VentDrive 
 * 
 * @param devType type of FusionBus device to be created
 * @return std::unique_ptr<FusionBusItem> 
 */
std::unique_ptr<FusionBusItem> TempSensorList::createObjectFromType(FusionBusItem::DeviceType devType)
{
    switch (devType)
    {
        case FusionBusItem::DeviceType::TempSensor: return std::make_unique<TempSensorItem>();
        case FusionBusItem::DeviceType::VentDrive: return std::make_unique<VentDriveItem>();
        case FusionBusItem::DeviceType::SoilSensor: return std::make_unique<FusionBusItem>(); // not implemented yet
        case FusionBusItem::DeviceType::Unknown: return std::make_unique<FusionBusItem>();
    
        default: return std::make_unique<FusionBusItem>();
    }
}

/**
 * @brief create a FusionBusItem derived object using device type ("type" json key)
 * 
 * @param json type of FusionBus device to be created ("type" json key parsed using EnumCrafter)
 * @return std::unique_ptr<FusionBusItem> if json is invalid or type is unsupported, a FusionBusItem with DeviceType::Unknown type will be created
 */
std::unique_ptr<FusionBusItem> TempSensorList::createObjectFromType(std::string json)
{
    std::unique_ptr<FusionBusItem> item = nullptr;
    JsonDocument doc;
    FusionBusItem::DeviceType devType = FusionBusItem::DeviceType::Unknown;
    if(!deserializeJson(doc, json))
    {
        devType = EnumCrafter::parse<FusionBusItem::DeviceType>(doc["type"].as<std::string>()).value_or(FusionBusItem::DeviceType::Unknown);
    }
    item = TempSensorList::createObjectFromType(devType);
    return item;
}