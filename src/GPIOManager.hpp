#ifndef GPIOMANAGER_HPP
#define GPIOMANAGER_HPP

#include "GPIOList.hpp"
#include "EventManager.hpp"
#include "IObserver.hpp"

class GPIOManager: public IObserver<EventManager>
{
public:
    static GPIOManager* getInstance();
    ~GPIOManager();
    void createIO(GPIOItem newItem);
    void removeIO(int id);
    void modifyIO(int id, GPIOItem newItem);
    void modifyIOStatus(int id, bool status);
    void modifyIOExtraParameters(int id, std::string extraParameters);
    std::string getGPIOListJson();
    void saveState();
    void loadState();

    void syncHardware();
    void update(EventManager* eventManager) override;

private:
    GPIOManager();
    static GPIOManager *instance;
    GPIOList list;

};

#endif // GPIOMANAGER_HPP