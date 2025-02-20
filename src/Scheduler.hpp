#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "SchedulerList.hpp"
#include "EventManager.hpp"
#include "Time.hpp"
#include "SystemTime.hpp"
#include <string>
#include <vector>
#include <iostream>

class Scheduler: public IObserver<SystemTime> // SchedulerItem Manager Class
{
public:
    static Scheduler* getInstance();
    bool isAnyItemOn();
    void update(SystemTime* systemTime) override;

    void createSchedule(int id, SchedulerItem newItem);
    void removeSchedule(int id);
    void modifySchedule(int id, SchedulerItem& newItem);

    void saveState();
    void loadState();

    SchedulerList getSchedulerList();

private:
    Scheduler();
    ~Scheduler();
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    void determineStatusofItems();
    void broadcastItem(SchedulerItem &item);

    static Scheduler* instance;
    SchedulerList list;
};

#endif // SCHEDULER_HPP