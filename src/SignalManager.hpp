#ifndef SIGNALMANAGER_HPP
#define SIGNALMANAGER_HPP

#include "SignalList.hpp"
#include "Subject.hpp"
#include "Configuration.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
#include <optional>
#include "IManager.hpp"
#include "CentralizedSignalHub.hpp"
#include "StatusCode.hpp"


class SignalManager : public IManager<SignalItem>, public IObserver<CentralizedSignalHub>
{
public:
    static SignalManager* getInstance();

    void create(SignalItem item) override;
    void remove(uint64_t id) override;
    void modify(uint64_t id, SignalItem newItem) override;
    void modifyEventFlag(uint64_t id, bool flag);
    std::string getListJson() override;

    StatusCode setSignalValue(std::string fullSignalPath, bool value);
    std::optional<bool> getSignalValue(std::string fullSignalPath);

    void update(CentralizedSignalHub *signalHub) override;

    void saveState() override;
    void loadState() override;

    void loop();

private:
    SignalManager();
    ~SignalManager();
    SignalManager(const SignalManager&) = delete;
    SignalManager& operator=(const SignalManager&) = delete;

    static SignalManager* instance;
    SignalList signalList;
};

#endif // EVENTMANAGER_HPP