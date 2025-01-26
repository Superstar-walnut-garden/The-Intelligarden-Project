#ifndef EVENTLIST_HPP
#define EVENTLIST_HPP

#include "EventItem.hpp"
#include <ArduinoJson.h>
#include <string>
#include <vector>
#include <iostream>

class EventList // EventItem Manager Class
{
public:
    EventList();
    EventList(const char* json, int length);
    EventItem &getItem(int id);
    std::vector<EventItem> getList();
    std::string getListJson();
    void addItem(EventItem item);
    void repopulateWith(const char* json, int length);
    void printList(); // not tested
    void modifyItem(int id, const EventItem& newItem);
    void deleteItem(int id);

private:
    std::vector<EventItem> list;
};

#endif // EVENTLIST_HPP