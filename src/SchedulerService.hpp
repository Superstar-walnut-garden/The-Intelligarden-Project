#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "SchedulerList.hpp"
#include "Time.hpp"
#include "SystemTime.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "IManager.hpp"
#include "ISignalCompatibleManager.hpp"

class Scheduler: public IManager<SchedulerItem>, public ISignalCompatibleManager // SchedulerItem Manager Class
{
public:
    static Scheduler* getInstance();

    void create(SchedulerItem newItem) override;
    void remove(uint64_t id) override;
    void modify(uint64_t id, SchedulerItem newItem) override;

    void saveState() override;
    void loadState() override;

    std::string getListJson() override;
    std::string getName() override { return "Scheduler"; }
    std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() override;
    void loop();

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