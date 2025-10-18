#pragma once
#include "ILoggableService.hpp"
#include "IExternalStorage.hpp"
#include <vector>


class LogDispatcherService
{
public:
    static LogDispatcherService* getInstance();
    void attachLoggableService(ILoggableService* service);
    void loop();

private:
    LogDispatcherService();
    static LogDispatcherService* instance;
    std::vector<ILoggableService*> loggableServices;
    IExternalStorage* storage;
};