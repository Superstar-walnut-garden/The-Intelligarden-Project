#include "SignalItem.hpp"

SignalItem::SignalItem() : BaseItem(), broadcaster(""), listeners() {}

std::vector<std::string> SignalItem::getListeners()
{
    return listeners;
}

std::string SignalItem::getBroadcaster()
{
    return broadcaster;
}

void SignalItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    this->broadcaster = doc["broadcaster"].as<std::string>();
    JsonArray listenersArray = doc["listeners"].as<JsonArray>();

    for(auto item : listenersArray)
        this->listeners.push_back(item);
}

void SignalItem::derivedClassToJson(JsonDocument &doc)
{
    JsonArray listenersArray = doc.createNestedArray("listeners");
    for(auto &listener : listeners)
        listenersArray.add(listener);
    doc["broadcaster"] = broadcaster;
}