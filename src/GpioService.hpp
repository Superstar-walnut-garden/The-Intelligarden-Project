#pragma once

#include "GpioList.hpp"
#include "IObserver.hpp"
#include "IResourceController.hpp"
#include "ISignalCompatibleService.hpp"
#include "IResourcePersistenceService.hpp"
#include "ILoggableService.hpp"
#include <vector>
#include <string>

class GpioService
    : public IResourcePersistenceService, 
    public IResourceController<GpioItem>, 
    public ISignalCompatibleService,
    public ILoggableService
{
public:
    static GpioService* getInstance();
    

    void create(GpioItem newItem) override;
    void remove(uint64_t id) override;
    void update(uint64_t id, GpioItem newItem) override;
    std::string get(uint64_t id) override;
    std::string getAll() override;

    void storeAll() override;
    void restoreAll() override;

    std::string getName() const override;
    std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() override;

    std::vector<std::unique_ptr<ILoggableItem>> getLoggableItems() const override;

    void syncHardware();

private:
    GpioService();
    ~GpioService();
    static GpioService *instance;
    GpioList list;

};
