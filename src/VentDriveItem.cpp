#include "VentDriveItem.hpp"
#include "SignalNameResolver.hpp"
#include "../EnumCrafter.hpp"

/**
 * @brief Construct a new VentDriveItem object
 * 
 */
VentDriveItem::VentDriveItem(): 
    FusionBusItem(FusionBusItem::DeviceType::VentDrive), ventingPercent(50), currentVentingPercent(std::nullopt),
        length(100), stepPermm(200), speed(8), maxCompensation(10), acceleration(5), autoHomeFlag(false), 
            endstopMinDistance(10), currentState(VentDriveItem::State::Unknown)
{
    Serial.println("VentDriveItem created!!!!!!!!!!!");
    registerToJsonCallback([this](JsonDocument &json) -> void
    {
        json["ventingPercent"] = ventingPercent;
        json["length"] = length;
        json["stepPermm"] = stepPermm;
        json["speed"] = speed;
        json["maxCompensation"] = maxCompensation;
        json["acceleration"] = acceleration;
        json["endstopMinDistance"] = endstopMinDistance;

        json["currentState"] = EnumCrafter::toString(currentState);
        if(currentVentingPercent.has_value())
            json["currentVentingPercent"] = currentVentingPercent.value();
    });
    registerFromJsonCallback([this](JsonDocument &json) -> void
    {
        ventingPercent = json["ventingPercent"].as<int>();
        length = json["length"].as<double>();
        stepPermm = json["stepPermm"].as<double>();
        speed = json["speed"].as<double>();
        maxCompensation = json["maxCompensation"].as<double>();
        acceleration = json["acceleration"].as<double>();
        endstopMinDistance = json["endstopMinDistance"].as<double>();
        if((json["AutoHomeFlag"].as<bool>() | false)) // if autohome flag is true
            autoHomeFlag = true; // raise autoHomeFlag
    });
}

/**
 * @brief get vent length in mm
 * 
 * @return double length
 */
double VentDriveItem::getLength() const
{
    return length;
}

/**
 * @brief get step per mm
 * 
 * @return double stepPermm
 */
double VentDriveItem::getStepPermm() const
{
    return stepPermm;
}

/**
 * @brief get speed in mm/s
 * 
 * @return double speed
 */
double VentDriveItem::getSpeed() const
{
    return speed;
}

/**
 * @brief get the maximum amount of missed steps compensation when closing (reaching home or endstop)
 * 
 * @return double maxCompensation
 */
double VentDriveItem::getMaxCompensation() const
{
    return maxCompensation;
}

/**
 * @brief get acceleration in mm/s2
 * 
 * @return double acceleration
 */
double VentDriveItem::getAcceleration() const
{
    return acceleration;
}

/**
 * @brief get minimum distance to get from the endstop to expect endstop release
 * 
 * @return double endstopMinDistance
 */
double VentDriveItem::getEndstopMinDistance() const
{
    return endstopMinDistance;
}

/**
 * @brief get current state of the actual device (default value is State::Unknown)
 * 
 * @return VentDriveItem::State current state
 */
VentDriveItem::State VentDriveItem::getCurrentState() const
{
    return currentState;
}

/**
 * @brief get autoHomeFlag
 * 
 * @return bool flag
 */
bool VentDriveItem::getAutoHomeFlag() const
{
    return autoHomeFlag;
}

/**
 * @brief drop autoHomeFlag
 * 
 */
void VentDriveItem::dropAutoHomeFlag()
{
    autoHomeFlag = false;
}

/**
 * @brief get venting percent (between 0 to 100)
 * 
 * @return int percent
 */
int VentDriveItem::getVentingPercent() const
{
    return ventingPercent;
}

/**
 * @brief set current state of the device
 * 
 * @param state new state
 */
void VentDriveItem::setCurrentState(State state)
{
    this->currentState = state;
}

/**
 * @brief set current venting percentage of the device
 * 
 * @param std::optional<int> updated currentVentingPercent
 */
void VentDriveItem::setCurrentVentingPercent(std::optional<int> currentVentingPercent)
{
    this->currentVentingPercent = currentVentingPercent;
}

/**
 * @brief set venting percentage (setpoint) of the device
 * 
 * @param int updated VentingPercent (setpoint)
 */
void VentDriveItem::setVentingPercent(int ventingPercent)
{
    this->ventingPercent = ventingPercent;
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
