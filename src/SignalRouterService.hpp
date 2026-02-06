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

    void create(std::string json) override;
    void remove(uint64_t id) override;
    void update(uint64_t id, std::string json) override;
    std::string getAll() const override;
    std::string get(uint64_t id) const override;

    void storeAll() override;
    void restoreAll() override;

private:
    SignalRouterService();
    ~SignalRouterService();
    SignalRouterService(const SignalRouterService&) = delete;
    SignalRouterService& operator=(const SignalRouterService&) = delete;

    std::string getName() const override { return "SignalRouter"; };
    std::vector<ISignalCompatibleItem *> getSignalCompatibleItems() override;

    static SignalRouterService* instance;
    mutable SignalRouterList signalList;
};
