#pragma once

#include "ThermostatList.hpp"
#include "IObserver.hpp"
#include "TempSensorService.hpp"
#include "IResourceController.hpp"
#include "ISignalCompatibleService.hpp"
#include "IResourcePersistenceService.hpp"

class ThermostatService: public IResourceController<ThermostatItem>, 
    public IResourcePersistenceService,
    public ISignalCompatibleService,
    public IObserver<TempSensorService>
{
public:
    static ThermostatService* getInstance();
    ~ThermostatService();

    void create(std::unique_ptr<ThermostatItem> newItem) override;
    void remove(uint64_t id) override;
    void update(uint64_t id, std::unique_ptr<ThermostatItem> newItem) override;
    std::string getAll() override;
    std::string get(uint64_t) override;
    void storeAll() override;
    void restoreAll() override;

    std::string getName() const override;
    std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() override;
    
    void update(TempSensorService* temperature) override;

private:
    ThermostatService();
    static ThermostatService *instance;
    ThermostatList list;

};
