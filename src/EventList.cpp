#include "EventList.hpp"

EventList::EventList() {}

EventList::EventList(const char* json, int length)
{
    this->repopulateWith(json, length);
    Serial.println(json);
}

EventItem &EventList::getItem(int id)
{
    static EventItem nullItem(-1, "", false, false); // item with id = -1
    for(auto &item : list)
    {
        if(item.getId() == id)
            return item;
    }
    return nullItem;
}

std::vector<EventItem> EventList::getList()
{
    return list;
}

std::string EventList::getListJson()
{
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();

    for (auto& item : list) 
    {
        JsonObject obj = array.createNestedObject();
        obj["id"] = item.getId();
        obj["name"] = item.getName();
        obj["flag"] = item.getFlag();
        obj["occupied"] = item.isOccupied();
    }

    std::string output;
    serializeJson(doc, output);
    return output;
}

void EventList::addItem(EventItem item)
{
    list.push_back(item);
}

void EventList::repopulateWith(const char* json, int length)
{
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    list.clear(); // delete the old items before adding new ones

    for (JsonObject item : doc.as<JsonArray>()) 
    {
        int id = item["id"];
        std::string name = item["name"].as<std::string>();
        bool flag = item["flag"];
        bool occupied = item["occupied"];

        list.emplace_back(id, name, flag, occupied); // add parsed items
    }
}

void EventList::printList()
{
    for (auto& item : list)
    {
        std::cout << "ID: " << item.getId() << ", Name: " << item.getName() << ", Flag: " << item.getFlag() << ", Occupied: " << item.isOccupied() << std::endl;
    }
}

void EventList::modifyItem(int id, const EventItem& newItem)
{
    for (auto& item : list)
    {
        if (item.getId() == id)
        {
            item.setName(newItem.getName());
            item.setFlag(newItem.getFlag());
            item.setOccupied(newItem.isOccupied());
            return;
        }
    }
}

void EventList::deleteItem(int id)
{
    list.erase(std::remove_if(list.begin(), list.end(), [id](const EventItem& item) {
        return item.getId() == id;
    }), list.end());
}