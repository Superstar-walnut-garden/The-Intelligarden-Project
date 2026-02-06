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

    void create(std::string json) override;
    void remove(uint64_t id) override;
    void update(uint64_t id, std::string json) override;
    std::string getAll() const override;
    std::string get(uint64_t) const override;
    void storeAll() override;
    void restoreAll() override;

    std::string getName() const override;
    std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() override;
    
    void update(TempSensorService* temperature) override;

private:
    ThermostatService();
    static ThermostatService *instance;
    mutable ThermostatList list;

};
