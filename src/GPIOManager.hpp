#ifndef GPIOMANAGER_HPP
#define GPIOMANAGER_HPP

#include "GPIOList.hpp"
#include "EventManager.hpp"
#include "IObserver.hpp"
#include "IManager.hpp"
#include "ISignalCompatibleManager.hpp"

class GPIOManager: public IManager<GPIOItem>, public ISignalCompatibleManager, public IObserver<EventManager>
{
public:
    static GPIOManager* getInstance();
    ~GPIOManager();
    void create(GPIOItem newItem) override;
    void remove(uint64_t id) override;
    void modify(uint64_t id, GPIOItem newItem) override;
    void modifyIOStatus(uint64_t id, bool status);
    void modifyIOExtraParameters(uint64_t id, std::string extraParameters);
    std::string getListJson() override;
    void saveState() override;
    void loadState() override;

    std::string getName() override;
    std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() override;

    void syncHardware();
    void update(EventManager* eventManager) override;

private:
    GPIOManager();
    static GPIOManager *instance;
    GPIOList list;

};

#endif // GPIOMANAGER_HPP