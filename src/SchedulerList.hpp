#ifndef SCHEDULERLIST_HPP
#define SCHEDULERLIST_HPP
#include "SchedulerItem.hpp"
#include <ArduinoJson.h>
#include "Time.hpp"
#include <string>
#include <vector>
#include <iostream>

class SchedulerList // SchedulerItem Manager Class
{
    public:
    SchedulerList();
    SchedulerList(const char* json, int length);
    SchedulerItem &getItem(int id);
    std::vector<SchedulerItem> getList();
    std::string getListJson();
    void addItem(SchedulerItem Item);
    void repopulateWith(const char* json, int length);
    void printList(); // not tested
    private:
    std::vector<SchedulerItem> list;
};

#endif