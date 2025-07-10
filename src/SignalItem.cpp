#include "SignalItem.hpp"

SignalItem::SignalItem() : BaseItem(), broadcaster(""), listeners() {}


void SignalItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    this->broadcaster = doc["broadcaster"].as<std::string>();
    // this->listeners = doc["listeners"].as<(const char[]*)>();
}

void SignalItem::derivedClassToJson(JsonDocument &doc)
{
    JsonArray listenersArray = doc.createNestedArray("listeners");
    for(auto &listener : listeners)
        listenersArray.add(listener);
    doc["broadcaster"] = broadcaster;
}