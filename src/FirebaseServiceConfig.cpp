#include "FirebaseServiceConfig.hpp"

/**
 * @brief Constructor for FirebaseServiceConfig class.
 * 
 */
FirebaseServiceConfig::FirebaseServiceConfig()
{

}

/**
 * @brief Constructor for FirebaseServiceConfig class with JSON input.
 * 
 * @param json JSON string containing Firebase configuration data.
 */
FirebaseServiceConfig::FirebaseServiceConfig(std::string json)
{
    populateFromJson(json);
}

/**
 * @brief Populate the confg from a JSON string.
 * @param json The JSON string to populate the config from.
 */
void FirebaseServiceConfig::populateFromJson(std::string json)
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

/**
 * @brief Convert the Firebase configuration data to a JSON string.
 * 
 * @return std::string JSON string representation of the Firebase configuration.
 */
std::string FirebaseServiceConfig::toJson()
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

/**
 * @brief Check if the Firebase configuration data has null or empty parameters.
 * 
 * @return true if any of the configuration fields are null or empty, false otherwise.
 */
bool FirebaseServiceConfig::isNull()
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

/**
 * @brief Check if Firebase is enabled.
 * 
 * @return true if Firebase is enabled, false otherwise.
 */
bool FirebaseServiceConfig::isEnabled()
{
    return enabled;
}

/**
 * @brief Check if the Firebase configuration data is null or empty.
 * 
 * @param data The string data to check.
 * @return true if the data is null or empty, false otherwise.
 */
bool FirebaseServiceConfig::FBDataNull(std::string data)
{
    if(data == "" or data == "null" or data.empty())
    {
        Serial.println("Error: Couldn't get FirebaseServiceConfig");
        return true;
    }

    return false; 
}

/**
 * @brief Get the Firebase API key.
 * 
 * @return std::string The Firebase API key.
 */
std::string FirebaseServiceConfig::getApiKey()
{
    return apiKey;
}

/**
 * @brief Get the Firebase database URL.
 * 
 * @return std::string The Firebase database URL.
 */
std::string FirebaseServiceConfig::getDatabaseURL()
{
    return databaseURL;
}

/**
 * @brief Get the Firebase user email.
 * 
 * @return std::string The Firebase user email.
 */
std::string FirebaseServiceConfig::getUserEmail()
{
    return userEmail;
}

/**
 * @brief Get the Firebase user password.
 * 
 * @return std::string The Firebase user password.
 */
std::string FirebaseServiceConfig::getUserPassword()
{
    return userPassword;
}

/**
 * @brief Get the Firebase database root name.
 * 
 * @return std::string The Firebase database root name.
 */
std::string FirebaseServiceConfig::getDatabaseRootName()
{
    return databaseRootName;
}

