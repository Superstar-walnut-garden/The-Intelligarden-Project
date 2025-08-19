#pragma once

#include <FirebaseESP32.h>

#include "FirebaseServiceConfig.hpp"
#include "SystemTimeService.hpp"
#include "IObserver.hpp"
#include "Configuration.hpp"
#include "TempSensorService.hpp"
#include "SystemMaintainer.hpp"
#include "IResourcePersistenceService.hpp"
#include "IConfigController.hpp"


class FirebaseService: 
    public IConfigController<FirebaseServiceConfig>,
    public IResourcePersistenceService,
    public IObserver<SystemTimeService>
{
public:
    static FirebaseService *getInstance();
    void init();
    void update(SystemTimeService *systemTime);

    void updateConfig(FirebaseServiceConfig cfg) override;
    std::string getConfig() override;

    void storeAll() override;
    void restoreAll() override;

private:
    FirebaseServiceConfig fbData; // the actual service config
    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config; // config of the library
    String uid; // Variable to save USER UID
    bool signupOK;
    bool firebaseOK;
    signed updateTimestamp;
    int firebaseAbnormalityID;

    FirebaseService();
    static FirebaseService *instance;
};
