#pragma once

#include "SandFlushList.hpp"
#include "Time.hpp"
#include "SystemTimeService.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "IResourceController.hpp"
#include "ISignalCompatibleService.hpp"
#include "IResourcePersistenceService.hpp"

class SandFlushService: public IResourceController<SandFlushItem>, public IResourcePersistenceService, public ISignalCompatibleService
{
public:
    static SandFlushService* getInstance();

    std::string getName() const override { return "SandFlush"; }
    std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() override;
    void loop();

    std::string getAll() const override;
    std::string get(uint64_t id) const override;
    void create(std::string json) override;
    void remove(uint64_t id) override;
    void update(uint64_t id, std::string json) override;

    void storeAll() override;
    void restoreAll() override;

private:
    SandFlushService();
    ~SandFlushService();
    SandFlushService(const SandFlushService&) = delete;
    SandFlushService& operator=(const SandFlushService&) = delete;
    void determineStatusofItems();
    void broadcastItem(SandFlushItem *item);

    static SandFlushService* instance;
    mutable SandFlushList list;
};
