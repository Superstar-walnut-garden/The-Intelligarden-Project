#pragma once

#include "SignalRouterList.hpp"
#include "Subject.hpp"
#include "Configuration.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
#include <optional>
#include "IResourceController.hpp"
#include "ISignalCompatibleService.hpp"
#include "IResourcePersistenceService.hpp"
#include "CentralizedSignalHubService.hpp"
#include "StatusCode.hpp"


class SignalRouterService: 
    public IResourceController<SignalRouterItem>, 
    public IResourcePersistenceService,
    public ISignalCompatibleService, 
    public IObserver<CentralizedSignalHubService>
{
public:
    static SignalRouterService* getInstance();

    StatusCode setSignalValue(std::string fullSignalPath, bool value);
    std::optional<bool> getSignalValue(std::string fullSignalPath);
    void update(CentralizedSignalHubService *signalHub) override;

    void create(SignalRouterItem item) override;
    void remove(uint64_t id) override;
    void update(uint64_t id, SignalRouterItem newItem) override;
    std::string getAll() override;
    std::string get(uint64_t id) override;

    void storeAll() override;
    void restoreAll() override;

private:
    SignalRouterService();
    ~SignalRouterService();
    SignalRouterService(const SignalRouterService&) = delete;
    SignalRouterService& operator=(const SignalRouterService&) = delete;

    std::string getName() override { return "SignalRouter"; };
    std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() override;

    static SignalRouterService* instance;
    SignalRouterList signalList;
};
