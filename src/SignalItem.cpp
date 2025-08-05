#include "SignalItem.hpp"
#include "SignalNameResolver.hpp"
#include "../EnumCrafter.hpp"

/**
 * @brief Construct a new Signal Item:: Signal Item object
 * 
 */
SignalItem::SignalItem() 
    : SignalCompatibleBaseItem(), broadcaster(), auxiliaryBroadcaster(), listeners(), mode(Mode::SingleSource) {}

/**
 * @brief Get the listeners of the SignalItem.
 * 
 * @return std::vector<SignalEndpoint> A vector of SignalEndpoint objects representing the listeners.
 */
std::vector<SignalItem::SignalEndpoint> SignalItem::getListeners()
{
    return listeners;
}

/**
 * @brief Get the broadcaster of the SignalItem.
 * 
 * @return SignalEndpoint The broadcaster of the SignalItem.
 */
SignalItem::SignalEndpoint SignalItem::getBroadcaster()
{
    return broadcaster;
}

/**
 * @brief Get the auxiliary broadcaster of the SignalItem.
 * 
 * @return SignalEndpoint The auxiliary broadcaster of the SignalItem.
 */
SignalItem::SignalEndpoint SignalItem::getAuxiliaryBroadcaster()
{
    return auxiliaryBroadcaster;
}

/**
 * @brief set the status of the primary broadcaster.
 * 
 * @param value The status value to set for the primary broadcaster.
 */
void SignalItem::setBroadcasterStatus(bool value)
{
    this->broadcaster.setStatus(value);
    this->evaluateStatus(); // evaluate the main status based on the broadcaster
}

/**
 * @brief set the status of the auxiliary broadcaster.
 * 
 * @param value The status value to set for the auxiliary broadcaster.
 */
void SignalItem::setAuxiliaryBroadcasterStatus(bool value)
{
    this->auxiliaryBroadcaster.setStatus(value);
    this->evaluateStatus(); // evaluate the main status based on the broadcaster
}

/**
 * @brief Get the local signal path for emitted signals.
 * 
 * @return std::string The local signal path for emitted signals.
 */
std::string SignalItem::getEmittedSignalLocalSignalName()
{
    return SignalNameResolver::generateLocalSignalName("emittedSignal", SignalNameResolver::SignalType::Broadcaster);
}

/**
 * @brief Get the local signal names associated with the SignalItem (for CentralizedSignalHub).
 * 
 * @return std::vector<std::string> A vector of local signal names.
 */
std::vector<std::string> SignalItem::getLocalSignalNames()
{
    return { getEmittedSignalLocalSignalName() };
}

/**
 * @brief Get mode (SingleSource, DualSource(andWithAuxilary, orWithAuxilary) and etc...)
 * 
 * @return SignalItem::Mode The mode of the SignalItem.
 */
SignalItem::Mode SignalItem::getMode()
{
    return mode;
}

/**
 * @brief evaluate the status of the SignalItem based on its broadcaster and mode
 * 
 */
void SignalItem::evaluateStatus()
{
    if(mode == Mode::SingleSource)
        this->setStatus(broadcaster.getStatus());
    else if(mode == Mode::AndWithAuxiliary)
        this->setStatus(broadcaster.getStatus() and auxiliaryBroadcaster.getStatus());
    else if(mode == Mode::OrWithAuxiliary)
        this->setStatus(broadcaster.getStatus() or auxiliaryBroadcaster.getStatus());
}

/**
 * @brief Populate the SignalItem from a JSON document.
 * 
 * @param doc The JSON document containing the SignalItem data.
 */
void SignalItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    // broadcaster
    auto broadcasterJsonObject = doc["broadcaster"].as<JsonObject>();
    this->broadcaster = SignalEndpoint(
        broadcasterJsonObject["signalPath"].as<std::string>(),
        broadcasterJsonObject["inverted"].as<bool>()
    );

    // auxiliaryBroadcaster
    auto auxiliaryBroadcasterJsonObject = doc["auxiliaryBroadcaster"].as<JsonObject>();
    this->auxiliaryBroadcaster = SignalEndpoint(
        auxiliaryBroadcasterJsonObject["signalPath"].as<std::string>(),
        auxiliaryBroadcasterJsonObject["inverted"].as<bool>()
    );

    // listeners
    JsonArray listenersArray = doc["listeners"].as<JsonArray>();
    for(auto item : listenersArray) // get all listeners
    {
        std::string name = item["signalPath"].as<std::string>();
        bool inverted = item["inverted"].as<bool>();

        this->listeners.push_back(SignalEndpoint(name, inverted));
    }

    std::optional<Mode> modeOpt = EnumCrafter::parse<Mode>(doc["mode"].as<std::string>());
    if (modeOpt.has_value())
        mode = modeOpt.value();
    else
    {
        Serial.println("error: unsupported signal mod. switching to default 'SingleSource' mode");
        mode = Mode::SingleSource; // default value if not found
    }

}

/**
 * @brief Convert the SignalItem to a JSON document.
 * 
 * @param doc The JSON document to populate with the SignalItem data.
 */
void SignalItem::derivedClassToJson(JsonDocument &doc)
{
    // Create a nested array for listeners
    auto listenersArray = doc.createNestedArray("listeners");
    for(auto &listener : listeners) // add all listeners
    {
        JsonObject obj = listenersArray.createNestedObject();
        obj["signalPath"] = listener.getSignalPath();
        obj["inverted"] = listener.isInverted();
    }
    // broadcaster
    // Create a nested object for broadcaster
    auto broadcasterJsonObject = doc.createNestedObject("broadcaster");
    broadcasterJsonObject["signalPath"] = broadcaster.getSignalPath();
    broadcasterJsonObject["inverted"] = broadcaster.isInverted();
    broadcasterJsonObject["status"] = broadcaster.getStatus();

    // auxiliaryBroadcaster
    // Create a nested object for auxiliaryBroadcaster
    auto auxiliaryBroadcasterJsonObject = doc.createNestedObject("auxiliaryBroadcaster");
    auxiliaryBroadcasterJsonObject["signalPath"] = auxiliaryBroadcaster.getSignalPath();
    auxiliaryBroadcasterJsonObject["inverted"] = auxiliaryBroadcaster.isInverted();
    auxiliaryBroadcasterJsonObject["status"] = auxiliaryBroadcaster.getStatus();

    doc["mode"] = EnumCrafter::toString(mode);  // Converts enum to string
}