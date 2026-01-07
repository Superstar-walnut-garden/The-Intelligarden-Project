#include "VentDriveItem.hpp"

/**
 * @brief Construct a new VentDriveItem object
 * 
 */
VentDriveItem::VentDriveItem(): FusionBusItem()
{
    registerToJsonCallback([this](JsonDocument &json) -> void
    {
        json["ventingPercent"] = ventingPercent;
        json["length"] = length;
        json["stepPermm"] = stepPermm;
    });
    registerFromJsonCallback([this](JsonDocument &json) -> void
    {
        ventingPercent = json["ventingPercent"];
        length = json["length"].as<double>();
        stepPermm = json["stepPermm"].as<double>();
    });
}

/**
 * @brief Get local signal names of VentDriveItem
 * 
 * @return std::vector<std::string> Vector of local signal names
 */
std::vector<std::string> VentDriveItem::getLocalSignalNames()
{
    return {}; // no signal
}
