#include "EventList.hpp"

EventList::EventList(): BaseList()
{}

EventList::EventList(std::string json): BaseList()
{
    this->repopulateWith(json);
    Serial.println(json.c_str()); // for debug porpuses
}


std::string EventList::toJson()
{
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();

    for (auto& item : getListRef()) // get a reference to the list and iterate over it
    {
        JsonObject obj = array.createNestedObject();
        obj["id"] = item.getId();
        obj["name"] = item.getName();
        obj["status"] = item.getStatus(); // status is the same as flag
        obj["occupied"] = item.isOccupied();
    }

    std::string output;
    serializeJson(doc, output);
    return output;
}

void EventList::repopulateWith(std::string json)
{
    auto list = getListRef(); // get a reference to the list
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);

    list.clear(); // delete the old items before adding new ones

    for (JsonObject item : doc.as<JsonArray>()) 
    {
        int id = item["id"];
        std::string name = item["name"].as<std::string>();
        bool status = item["status"];
        bool occupied = item["occupied"];

        list.emplace_back(id, name, status, occupied); // add parsed items
    }
}

void EventList::printList()
{
    for (auto& item : getList())
    {
        std::cout << "ID: " << item.getId() << ", Name: " << item.getName() << ", Status: " << item.getStatus() << ", Occupied: " << item.isOccupied() << std::endl;
    }
}