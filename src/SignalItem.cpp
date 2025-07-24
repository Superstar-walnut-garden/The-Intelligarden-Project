#include "SignalItem.hpp"

/**
 * @brief Construct a new Signal Item:: Signal Item object
 * 
 */
SignalItem::SignalItem() : BaseItem(), broadcaster(), listeners() {}

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

    // listeners
    JsonArray listenersArray = doc["listeners"].as<JsonArray>();
    for(auto item : listenersArray) // get all listeners
    {
        std::string name = item["signalPath"].as<std::string>();
        bool inverted = item["inverted"].as<bool>();

        this->listeners.push_back(SignalEndpoint(name, inverted));
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
}