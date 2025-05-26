#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "SchedulerList.hpp"
#include "EventManager.hpp"
#include "Time.hpp"
#include "SystemTime.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "IManager.hpp"

class Scheduler: public IManager<SchedulerItem>, public IObserver<SystemTime>, public IObserver<EventManager> // SchedulerItem Manager Class
{
public:
    static Scheduler* getInstance();
    bool isAnyItemOn();
    void update(SystemTime* systemTime) override;
    void update(EventManager* eventManager) override;

    void create(SchedulerItem newItem) override;
    void remove(uint64_t id) override;
    void modify(uint64_t id, SchedulerItem newItem) override;

    void saveState() override;
    void loadState() override;

    std::string getListJson() override;

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