#include "FBData.hpp"

FBData::FBData()
{

}

FBData::FBData(const char* json)
{
    JsonDocument doc;
    deserializeJson(doc, json);

    apiKey = doc["apiKey"].as<String>().c_str();
    databaseURL = doc["databaseURL"].as<std::string>();
    userEmail = doc["userEmail"].as<std::string>();
    userPassword = doc["userPassword"].as<std::string>();
    databaseRootName = doc["databaseRootName"].as<std::string>();
    enabled = doc["enabled"].as<bool>();
}

std::string FBData::toJsonString()
{
    JsonDocument doc;
    doc["apiKey"] = apiKey;
    doc["databaseURL"] = databaseURL;
    doc["userEmail"] = userEmail;
    doc["userPassword"] = userPassword;
    doc["databaseRootName"] = databaseRootName;
    doc["enabled"] = enabled;
    std::string output;
    serializeJson(doc, output);
    return output;
}
/*! @brief checks if any of the parameters are null
*/
bool FBData::isNull()
{
    if(FBDataNull(apiKey) or 
       FBDataNull(databaseURL) or
       FBDataNull(userEmail) or
       FBDataNull(userPassword) or
       FBDataNull(databaseRootName))
       {
            return true;
       }
       return false;
}
bool FBData::isEnabled()
{
    return enabled;
}

bool FBData::FBDataNull(std::string data)
{
    if(data == "" or data == "null" or data.empty())
    {
        Serial.println("Error: Couldn't get FBData");
        return true;
    }

    return false; 
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

