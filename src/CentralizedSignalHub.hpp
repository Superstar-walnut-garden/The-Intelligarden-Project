#ifndef CENTRALIZEDSIGNALHUB_HPP
#define CENTRALIZEDSIGNALHUB_HPP
#include <vector>
#include "ISignalCompatibleService.hpp"
#include "Subject.hpp"

class CentralizedSignalHub: public Subject<CentralizedSignalHub>, public IObserver<ISignalCompatibleService>
{
public:
    static CentralizedSignalHub* getInstance();
    void registerManager(ISignalCompatibleService *manager);
    std::vector<ISignalCompatibleService *> getManagers();
    bool isSignalPathValid(std::string fullSignalPath);
    std::string getListJson();
    void update(ISignalCompatibleService *scm);

private:
    std::vector<ISignalCompatibleService *> managers;

    CentralizedSignalHub(){}; // private constructor
    static CentralizedSignalHub *instance; // singleton instance
};

#endif