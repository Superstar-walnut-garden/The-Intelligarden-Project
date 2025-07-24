#include "SignalItem.hpp"

SignalItem::SignalItem() : BaseItem(), broadcaster(), listeners() {}

std::vector<SignalItem::SignalEndpoint> SignalItem::getListeners()
{
    return listeners;
}

SignalItem::SignalEndpoint SignalItem::getBroadcaster()
{
    return broadcaster;
}

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