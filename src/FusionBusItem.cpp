#include "FusionBusItem.hpp"

FusionBusItem::FusionBusItem(): SignalCompatibleBaseItem()
    {
        registerToJsonCallback([this](JsonDocument &json) -> void
        {
            json["type"] = type;
        });
        registerFromJsonCallback([this](JsonDocument &json) -> void
        {
            type = json["type"].as<std::string>();
        });
    }
