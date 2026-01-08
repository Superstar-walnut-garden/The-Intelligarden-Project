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
            auto devType = EnumCrafter::parse<FusionBusItem::DeviceType>(item["type"].as<std::string>()).value_or(FusionBusItem::DeviceType::Unknown);
            std::string jsonStr;
            serializeJson(item, jsonStr);
            std::unique_ptr<FusionBusItem> fusionItem = nullptr;
            if(devType == FusionBusItem::DeviceType::TempSensor)
                fusionItem = std::make_unique<TempSensorItem>();
            else if(devType == FusionBusItem::DeviceType::VentDrive)
                fusionItem = std::make_unique<VentDriveItem>();
            else if(devType == FusionBusItem::DeviceType::SoilSensor)
                fusionItem = std::make_unique<FusionBusItem>();
            else if(devType == FusionBusItem::DeviceType::Unknown)
                fusionItem = std::make_unique<FusionBusItem>();
            else
                fusionItem = std::make_unique<FusionBusItem>();
            fusionItem->populateFromJson(jsonStr);
            list.push_back(std::move(fusionItem));
        }
    }