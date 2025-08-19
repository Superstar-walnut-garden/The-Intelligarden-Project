#pragma once

#include "SchedulerList.hpp"
#include "Time.hpp"
#include "SystemTimeService.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "IResourceController.hpp"
#include "ISignalCompatibleService.hpp"
#include "IResourcePersistenceService.hpp"

class SchedulerService: public IResourceController<SchedulerItem>, public IResourcePersistenceService, public ISignalCompatibleService // SchedulerItem Manager Class
{
public:
    static SchedulerService* getInstance();

    std::string getName() override { return "Scheduler"; }
    std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() override;
    void loop();

    std::string getAll() override;
    std::string get(uint64_t id) override;
    void create(SchedulerItem newItem) override;
    void remove(uint64_t id) override;
    void update(uint64_t id, SchedulerItem newItem) override;

    void storeAll() override;
    void restoreAll() override;

private:
    SchedulerService();
    ~SchedulerService();
    SchedulerService(const SchedulerService&) = delete;
    SchedulerService& operator=(const SchedulerService&) = delete;
    void determineStatusofItems();
    void broadcastItem(SchedulerItem &item);

    static SchedulerService* instance;
    SchedulerList list;
};
