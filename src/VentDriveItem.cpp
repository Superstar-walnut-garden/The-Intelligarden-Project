#include "VentDriveItem.hpp"
#include "SignalNameResolver.hpp"

/**
 * @brief Construct a new VentDriveItem object
 * 
 */
VentDriveItem::VentDriveItem(): FusionBusItem(FusionBusItem::DeviceType::VentDrive)
{
    Serial.println("VentDriveItem created!!!!!!!!!!!");
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
    return {SignalNameResolver::generateLocalSignalName("VentState", SignalNameResolver::SignalType::Listener)};
}
