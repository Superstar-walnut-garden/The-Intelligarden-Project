#ifndef THERMOSTATMANAGER_HPP
#define THERMOSTATMANAGER_HPP

#include "ThermostatList.hpp"
#include "IObserver.hpp"
#include "Temperature.hpp"
#include "IManager.hpp"
#include "ISignalCompatibleManager.hpp"

class ThermostatManager: public IManager<ThermostatItem>, 
    public ISignalCompatibleManager,
    public IObserver<Temperature>
{
public:
    static ThermostatManager* getInstance();
    ~ThermostatManager();
    void create(ThermostatItem newItem) override;
    void remove(uint64_t id) override;
    void modify(uint64_t id, ThermostatItem newItem) override;
    std::string getListJson() override;
    void saveState() override;
    void loadState() override;

    std::string getName() override;
    std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() override;
    
    void update(Temperature* temperature) override;

private:
    ThermostatManager();
    static ThermostatManager *instance;
    ThermostatList list;

};

#endif // GPIOMANAGER_HPP