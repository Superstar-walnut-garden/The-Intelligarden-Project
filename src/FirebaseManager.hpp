#ifndef FIREBASEMANAGER_HPP
#define FIREBASEMANAGER_HPP

#include <FirebaseESP32.h>

#include "FBData.hpp"
#include "SystemTime.hpp"
#include "IObserver.hpp"
#include "Configuration.hpp"
#include "Temperature.hpp"


class FirebaseManager: public IObserver<SystemTime>
{
    public:
    FirebaseManager(FBData fbData);
    void init();
    void update(SystemTime *systemTime);
    private:
    FBData fbData;
    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;
    String uid; // Variable to save USER UID
    bool signupOK;
    bool firebaseOK;

    void FBDataNull(std::string data);
};

#endif