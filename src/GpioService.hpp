#pragma once

#include "GpioList.hpp"
#include "IObserver.hpp"
#include "IResourceController.hpp"
#include "ISignalCompatibleService.hpp"
#include "IStateful.hpp"
#include <vector>
#include <string>

class GpioService: public IStateful, public IResourceController<GpioItem>, public ISignalCompatibleService
{
public:
    static GpioService* getInstance();
    

    void create(GpioItem newItem) override;
    void remove(uint64_t id) override;
    void update(uint64_t id, GpioItem newItem) override;
    std::string get(uint64_t) override;
    std::string getAll() override;

    void storeAll() override;
    void restoreAll() override;

    std::string getName() override;
    std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() override;

    void syncHardware();

private:
    GpioService();
    ~GpioService();
    static GpioService *instance;
    GpioList list;

};
