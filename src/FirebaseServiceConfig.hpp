#pragma once

#include <ArduinoJson.h>
#include <string>
#include <iostream>
#include "IJsonSerializable.hpp"


class FirebaseServiceConfig: public IJsonSerializable
{
    public:
    FirebaseServiceConfig();
    FirebaseServiceConfig(std::string json);

    std::string toJson() const override;
    void populateFromJson(std::string) override;

    std::string getApiKey();
    std::string getDatabaseURL();
    std::string getUserEmail();
    std::string getUserPassword();
    std::string getDatabaseRootName();
    bool isEnabled();
    bool isNull();

    private:
    bool FBDataNull(std::string data);

    std::string apiKey;
    std::string databaseURL;
    std::string userEmail;
    std::string userPassword;
    std::string databaseRootName;
    bool enabled;
};
