#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP
#include "SchedulerList.hpp"
#include "Time.hpp"
#include "SystemTime.hpp"
#include <string>
#include <vector>
#include <iostream>

class Scheduler // SchedulerItem Manager Class
{
    public:
    Scheduler(SchedulerList &list);
    void determineStatusofItems();
    bool isAnyItemOn();

    private:
    SchedulerList &list;
    Time time;
    int weekday;
};

#endif