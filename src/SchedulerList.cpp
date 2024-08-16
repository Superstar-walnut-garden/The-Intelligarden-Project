#include "SchedulerList.hpp"

SchedulerList::SchedulerList()
{
}

/*!
    @brief get the time and date as an Arduino String object
    @param json string of json 
    @param length length of the json string
*/
SchedulerList::SchedulerList(const char* json, int length)
{
    this->repopulateWith(json, length);
    Serial.println(json);
}

/*!
    @brief get a reference to the desired item
    @param id the id of the desired item
    @return returns an item with an id = -1 if couldn't find a match
*/
SchedulerItem &SchedulerList::getItem(int id)
{
    static SchedulerItem nullItem(-1, Time(0, 0), Time(0, 0), "", false, false); // item with id = -1
    for(auto &item : list)
    {
        if(item.getId() == id)
            return item;
    }
    return nullItem;
}
/*!
    @brief get a copy of the list
*/
std::vector<SchedulerItem> SchedulerList::getList()
{
    return list;
}
/*!
    @brief get list in json format
*/
std::string SchedulerList::getListJson()
{
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();

    for (auto& item : list) 
    {
        JsonObject obj = array.createNestedObject();
        obj["id"] = item.getId();
        obj["start"] = item.getStartTime().toString();
        obj["duration"] = item.getDuration().toString();
        obj["weekday"] = item.getWeekday();
        obj["enabled"] = item.isEnabled();
        obj["on"] = item.isOn();
    }

    std::string output;
    serializeJson(doc, output);
    return output;
}

/*!
    @brief add a new item to the end of the item-list
*/
void SchedulerList::addItem(SchedulerItem item)
{
    list.push_back(item);
}

/*!
    @brief get the time and date as an Arduino String object
    @param json string of json 
    @param length length of the json string
*/
void SchedulerList::repopulateWith(const char* json, int length)
{
    JsonDocument doc;
    deserializeJson(doc, json);
    list.clear(); // delete the old items before adding new ones

    for (JsonObject item : doc.as<JsonArray>()) 
    {
        short id = item["id"];
        Time start = Time::parse(item["start"].as<const char*>());
        Time duration = Time::parse(item["duration"].as<const char*>());
        std::string weekday = item["weekday"].as<std::string>();
        bool enabled = item["enabled"];
        bool on = item["on"];

        list.emplace_back(id, start, duration, weekday, enabled, on); // add parsed items
    }
}
/*!
    @brief prints the entire items of the list via std::cout
*/
void SchedulerList::printList()
{
    for(auto & item : list)
        std::cout <<
            "ID: " << item.getId() << "\n" <<
            "Start: " << item.getStartTime().toString() << "\n" <<
            "Duration: " << item.getDuration().toString() << "\n" <<
            "Weekday: " << item.getWeekday() << "\n" <<
            "enabled: " << item.isEnabled() << "\n" <<
            "Current Status: " << item.isOn() << "\n\n";
}