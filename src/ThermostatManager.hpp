#ifndef THERMOSTATMANAGER_HPP
#define THERMOSTATMANAGER_HPP

#include "ThermostatList.hpp"
#include "EventManager.hpp"
#include "IObserver.hpp"
#include "Temperature.hpp"

class ThermostatManager: public IObserver<EventManager>, public IObserver<Temperature>
{
public:
    static ThermostatManager* getInstance();
    ~ThermostatManager();
    void create(ThermostatItem newItem);
    void remove(int id);
    void modify(int id, ThermostatItem newItem);
    std::string getListJson();
    void saveState();
    void loadState();

    void syncHardware();
    void update(EventManager* eventManager) override;
    void update(Temperature* temperature) override;

private:
    ThermostatManager();
    static ThermostatManager *instance;
    ThermostatList list;

};

#endif // GPIOMANAGER_HPP