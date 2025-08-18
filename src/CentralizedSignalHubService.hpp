#pragma once

#include <vector>
#include "ISignalCompatibleService.hpp"
#include "Subject.hpp"
#include "IReadOnlyResourceController.hpp"

class CentralizedSignalHubService: 
    public IReadOnlyResourceController<ISignalCompatibleService>, 
    public Subject<CentralizedSignalHubService>, 
    public IObserver<ISignalCompatibleService>
{
public:
    static CentralizedSignalHubService* getInstance();
    void registerManager(ISignalCompatibleService *manager);
    std::vector<ISignalCompatibleService *> getManagers();
    bool isSignalPathValid(std::string fullSignalPath);
    std::string get(uint64_t id) override { return "{}"; }; // get is unsupported
    std::string getAll() override;
    void update(ISignalCompatibleService *scm) override;

private:
    std::vector<ISignalCompatibleService *> managers;

    CentralizedSignalHubService(){}; // private constructor
    static CentralizedSignalHubService *instance; // singleton instance
};
