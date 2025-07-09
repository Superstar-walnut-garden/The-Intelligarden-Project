#ifndef CENTRALIZEDSIGNALHUB_HPP
#define CENTRALIZEDSIGNALHUB_HPP
#include <vector>
#include "ISignalCompatibleManager.hpp"
#include "Subject.hpp"

class CentralizedSignalHub: public Subject<CentralizedSignalHub>, public IObserver<ISignalCompatibleManager>
{
public:
    void registerManager(ISignalCompatibleManager &manager);
    std::vector<ISignalCompatibleManager &> getManagers();
    std::string getListJson();
    void update(ISignalCompatibleManager *scm);

private:
    std::vector<ISignalCompatibleManager &> managers;
};

#endif