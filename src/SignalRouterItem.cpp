#include "SignalRouterItem.hpp"
#include "SignalNameResolver.hpp"
#include "../EnumCrafter.hpp"
#include <algorithm>

/**
 * @brief Construct a new Signal Item:: Signal Item object
 * 
 */
SignalRouterItem::SignalRouterItem() 
    : SignalCompatibleBaseItem(), broadcaster(), auxiliaryBroadcaster(), listeners(), mode(Mode::SingleSource) {}

/**
 * @brief Get the listeners of the SignalRouterItem.
 * 
 * @return std::vector<SignalEndpoint> A vector of SignalEndpoint objects representing the listeners.
 */
std::vector<SignalRouterItem::SignalEndpoint> SignalRouterItem::getListeners() const
{
    return listeners;
}

/**
 * @brief Get the broadcaster of the SignalRouterItem.
 * 
 * @return SignalEndpoint The broadcaster of the SignalRouterItem.
 */
SignalRouterItem::SignalEndpoint SignalRouterItem::getBroadcaster() const
{
    return broadcaster;
}

/**
 * @brief Get the auxiliary broadcaster of the SignalRouterItem.
 * 
 * @return SignalEndpoint The auxiliary broadcaster of the SignalRouterItem.
 */
SignalRouterItem::SignalEndpoint SignalRouterItem::getAuxiliaryBroadcaster() const
{
    return auxiliaryBroadcaster;
}

/**
 * @brief set the status of the primary broadcaster.
 * 
 * @param value The status value to set for the primary broadcaster.
 */
void SignalRouterItem::setBroadcasterStatus(bool value)
{
    this->broadcaster.setStatus(value);
    this->evaluateStatus(); // evaluate the main status based on the broadcaster
}

/**
 * @brief set the status of the auxiliary broadcaster.
 * 
 * @param value The status value to set for the auxiliary broadcaster.
 */
void SignalRouterItem::setAuxiliaryBroadcasterStatus(bool value)
{
    this->auxiliaryBroadcaster.setStatus(value);
    this->evaluateStatus(); // evaluate the main status based on the broadcaster
}

/**
 * @brief Remove a listener from the SignalRouterItem by its signal path.
 * 
 * @param signalPath The signal path of the listener to remove.
 */
void SignalRouterItem::removeListener(std::string signalPath)
{
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [&](SignalEndpoint& listener) 
    {
        return (listener.getSignalPath() == signalPath);
    }), listeners.end());
}

/**
 * @brief Remove the broadcaster and optionally the auxiliary broadcaster from the SignalRouterItem.
 * 
 * @param removeAuxiliary If true, the auxiliary broadcaster will be removed otherwise primary broadcaster will be removed.
 */
void SignalRouterItem::removeBroadcaster(bool removeAuxiliary)
{
    if(removeAuxiliary)
        this->auxiliaryBroadcaster = SignalRouterItem::SignalEndpoint(); // reset the auxiliary broadcaster to a default state
    else
        this->broadcaster = SignalRouterItem::SignalEndpoint(); // reset the primary broadcaster to a default state
    this->evaluateStatus(); // evaluate the main status based on the broadcaster
}

/**
 * @brief Get the local signal path for emitted signals.
 * 
 * @return std::string The local signal path for emitted signals.
 */
std::string SignalRouterItem::getEmittedSignalLocalSignalName()
{
    return SignalNameResolver::generateLocalSignalName("emittedSignal", SignalNameResolver::SignalType::Broadcaster);
}

/**
 * @brief Get the local signal names associated with the SignalRouterItem (for CentralizedSignalHub).
 * 
 * @return std::vector<std::string> A vector of local signal names.
 */
std::vector<std::string> SignalRouterItem::getLocalSignalNames()
{
    return { getEmittedSignalLocalSignalName() };
}

/**
 * @brief Get mode (SingleSource, DualSource(andWithAuxilary, orWithAuxilary) and etc...)
 * 
 * @return SignalRouterItem::Mode The mode of the SignalRouterItem.
 */
SignalRouterItem::Mode SignalRouterItem::getMode() const
{
    return mode;
}

/**
 * @brief evaluate the status of the SignalRouterItem and its listeners based on its broadcaster(s) and mode
 * 
 */
void SignalRouterItem::evaluateStatus()
{
    // broadcasters evaluation
    if(mode == Mode::SingleSource)
        this->setStatus(broadcaster.getStatus());
    else if(mode == Mode::AndWithAuxiliary)
        this->setStatus(broadcaster.getStatus() and auxiliaryBroadcaster.getStatus());
    else if(mode == Mode::OrWithAuxiliary)
        this->setStatus(broadcaster.getStatus() or auxiliaryBroadcaster.getStatus());
    
    // listeners evaluation
    for(auto &listener : listeners) // loop through listeners
        listener.setStatus(this->getStatus());
}

/**
 * @brief Populate the SignalRouterItem from a JSON document.
 * 
 * @param doc The JSON document containing the SignalRouterItem data.
 */
void SignalRouterItem::populateDerivedClassFromJson(JsonDocument &doc)
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
 * @brief Convert the SignalRouterItem to a JSON document.
 * 
 * @param doc The JSON document to populate with the SignalRouterItem data.
 */
void SignalRouterItem::derivedClassToJson(JsonDocument &doc) const
{
    // Create a nested array for listeners
    auto listenersArray = doc.createNestedArray("listeners");
    for(const auto &listener : listeners) // add all listeners
    {
        JsonObject obj = listenersArray.createNestedObject();
        obj["signalPath"] = listener.getSignalPath();
        obj["inverted"] = listener.isInverted();
        obj["status"] = listener.getStatus();
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