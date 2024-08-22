#include "FBData.hpp"

FBData::FBData()
{

}

FBData::FBData(const char* json)
{
    JsonDocument doc;
    deserializeJson(doc, json);

    apiKey = doc["apiKey"].as<std::string>();
    databaseURL = doc["databaseURL"].as<std::string>();
    userEmail = doc["userEmail"].as<std::string>();
    userPassword = doc["userPassword"].as<std::string>();
    databaseRootName = doc["databaseRootName"].as<std::string>();
}

std::string FBData::toJsonString()
{
    JsonDocument doc;
    doc["apiKey"] = apiKey;
    doc["databaseURL"] = databaseURL;
    doc["userEmail"] = userEmail;
    doc["userPassword"] = userPassword;
    doc["databaseRootName"] = databaseRootName;
    std::string output;
    serializeJson(doc, output);
    return output;
}

std::string FBData::getApiKey()
{
    return apiKey;
}

std::string FBData::getDatabaseURL()
{
    return databaseURL;
}

std::string FBData::getUserEmail()
{
    return userEmail;
}

std::string FBData::getUserPassword()
{
    return userPassword;
}

std::string FBData::getDatabaseRootName()
{
    return databaseRootName;
}

