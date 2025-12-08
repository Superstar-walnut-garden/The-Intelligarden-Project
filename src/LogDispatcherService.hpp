#pragma once
#include "ILoggableService.hpp"
#include "IExternalStorage.hpp"
#include "IConfigController.hpp"
#include "IResourcePersistenceService.hpp"
#include "IReadOnlyResourceController.hpp"
#include "LogDispatcherConfig.hpp"
#include <vector>


class LogDispatcherService: 
    public IConfigController<LogDispatcherConfig>, 
    public IResourcePersistenceService,
    public IReadOnlyResourceController
{
public:
    static LogDispatcherService* getInstance();
    void attachLoggableService(ILoggableService* service);
    void loop();

    std::string get(std::string path); // the path-based get is supported and should be manually added as api endpoint.

private:
    LogDispatcherService();
    bool isStorageFull();
    void determineStatus();

    std::string getConfig() override;
    void updateConfig(LogDispatcherConfig config) override;

    std::string getAll() override;
    std::string get(uint64_t id) override { return "{}"; }; // the default get is unsupported

    void storeAll() override;
    void restoreAll() override;

    static LogDispatcherService* instance;
    std::vector<ILoggableService*> loggableServices;
    IExternalStorage* storage;
    LogDispatcherConfig config;
};