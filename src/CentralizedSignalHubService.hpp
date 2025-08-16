#pragma once

#include <vector>
#include "ISignalCompatibleService.hpp"
#include "Subject.hpp"

class CentralizedSignalHubService: public Subject<CentralizedSignalHubService>, public IObserver<ISignalCompatibleService>
{
public:
    static CentralizedSignalHubService* getInstance();
    void registerManager(ISignalCompatibleService *manager);
    std::vector<ISignalCompatibleService *> getManagers();
    bool isSignalPathValid(std::string fullSignalPath);
    std::string getListJson();
    void update(ISignalCompatibleService *scm);

private:
    std::vector<ISignalCompatibleService *> managers;

    CentralizedSignalHubService(){}; // private constructor
    static CentralizedSignalHubService *instance; // singleton instance
};
