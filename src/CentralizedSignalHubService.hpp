#pragma once

#include <vector>
#include "ISignalCompatibleService.hpp"
#include "Subject.hpp"
#include "IReadOnlyResourceController.hpp"

class CentralizedSignalHubService: 
    public IReadOnlyResourceController, 
    public Subject<CentralizedSignalHubService>, 
    public IObserver<ISignalCompatibleService>
{
public:
    static CentralizedSignalHubService* getInstance();
    void registerService(ISignalCompatibleService *service);
    std::vector<ISignalCompatibleService *> getServices();
    bool isSignalPathValid(std::string fullSignalPath);
    std::string get(uint64_t id) override { return "{}"; }; // get is unsupported
    std::string getAll() override;
    void update(ISignalCompatibleService *scm) override;

private:
    std::vector<ISignalCompatibleService *> services;

    CentralizedSignalHubService(){}; // private constructor
    static CentralizedSignalHubService *instance; // singleton instance
};
