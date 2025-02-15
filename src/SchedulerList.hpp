#ifndef SCHEDULERLIST_HPP
#define SCHEDULERLIST_HPP
#include "SchedulerItem.hpp"
#include "BaseList.hpp"
#include <ArduinoJson.h>
#include "Time.hpp"
#include <string>
#include <vector>
#include <iostream>

class SchedulerList: public BaseList<SchedulerItem> // SchedulerItem Manager Class
{
    public:
    SchedulerList();
    SchedulerList(std::string json);
    SchedulerItem &getItem(int id);
    void printList() override;
    private:
};

#endif