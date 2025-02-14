#include "GPIOList.hpp"

GPIOList::GPIOList(): BaseList()
{}

GPIOList::GPIOList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug purposes
}

std::string GPIOList::toJson()
{
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();

    for (auto& item : getListRef()) // get a reference to the list and iterate over it
    {
        JsonObject obj = array.createNestedObject();
        obj["id"] = item.getId();
        obj["name"] = item.getName();
        obj["status"] = item.getStatus();
        obj["event_id"] = item.getEventId();
        obj["mode"] = item.getMode();
        obj["extraParameters"] = item.getExtraParameters();
    }

    std::string output;
    serializeJson(doc, output);
    return output;
}

void GPIOList::repopulateWith(std::string json)
{
    auto list = getListRef(); // get a reference to the list
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);

    list.clear(); // delete the old items before adding new ones

    for (JsonObject item : doc.as<JsonArray>()) 
    {
        int id = item["id"].as<int>();
        std::string name = item["name"].as<std::string>();
        bool status = item["status"].as<bool>();
        int event_id = item["event_id"].as<int>();
        short mode = item["mode"].as<short>();
        std::string extraParameters = item["extraParameters"].as<std::string>();

        list.emplace_back(id, event_id, name, status, mode, extraParameters); // add parsed items
    }
}

void GPIOList::printList()
{
    for (auto& item : getList())
    {
        std::cout <<
         "ID: " << item.getId() << 
         ", Name: " << item.getName() << 
         ", Status: " << item.getStatus() << 
         ", Mode: " << item.getMode() <<
         ", Event_ID: " << item.getEventId() <<
         ", ExtraParameters: " << item.getExtraParameters() << std::endl;
    }
}