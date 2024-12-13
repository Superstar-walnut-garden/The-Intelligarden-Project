#ifndef FBDATA_HPP
#define FBDATA_HPP

#include <ArduinoJson.h>
#include <string>
#include <iostream>


class FBData
{
    public:
    FBData();
    FBData(const char* json);

    std::string toJsonString();

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


#endif