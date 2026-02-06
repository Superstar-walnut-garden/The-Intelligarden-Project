#include "FusionBusItem.hpp"
#include "../EnumCrafter.hpp"

FusionBusItem::FusionBusItem(DeviceType type): SignalCompatibleBaseItem(), type(type)
{
    registerToJsonCallback([this](JsonDocument &json) -> void
    {
        Serial.println(("fusionbusitem created with type of:" + std::string(EnumCrafter::toString(this->type))).c_str());
        json["type"] = EnumCrafter::toString(this->type);
        json["id"] = std::to_string(this->getId()); // passing 64bit id as string to prevent json and web api js issues with large numbers
    });
    registerFromJsonCallback([this](JsonDocument &json) -> void
    {
        // this->type = EnumCrafter::parse<FusionBusItem::DeviceType>(json["type"].as<std::string>()).value_or(FusionBusItem::DeviceType::Unknown);
        this->setId(std::stoull(json["id"].as<std::string>())); // passing 64bit id as string to prevent json and web api js issues with large numbers
    });
}
