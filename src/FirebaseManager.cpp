#include "FirebaseManager.hpp"
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

FirebaseManager::FirebaseManager(FBData fbData): signupOK(false), firebaseOK(true)
{
    this->fbData = fbData;
}

void FirebaseManager::init()
{
    // Assign the api key (required)
    config.api_key = fbData.getApiKey();
    FBDataNull(config.api_key.c_str());
    // Assign the RTDB URL (required) 
    config.database_url = fbData.getDatabaseURL();
    FBDataNull(config.database_url.c_str());
    // Assign the user sign in credentials
    auth.user.email = fbData.getUserEmail();
    FBDataNull(auth.user.email.c_str());

    auth.user.password = fbData.getUserPassword();
    FBDataNull (auth.user.password.c_str());

    fbdo.setResponseSize(4096);
    // Assign the callback function for the long running token generation task
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    // Assign the maximum retry of token generation
    config.max_token_generation_retry = 5;
    // Initialize the library with the Firebase authen and config
    Firebase.begin(&config, &auth);
    // Getting the user UID might take a few seconds
    Serial.println("Getting User UID");
    while ((auth.token.uid) == "")
    {
        Serial.print('.');
        delay(1000);
        static int localCounter = 0;
        if(localCounter ++ > 6)
        {
            localCounter = 0;
            Serial.println("Error: Couldn't get user's UID");
            firebaseOK = false;
            break;
        }
    }
    // Print user UID
    uid = auth.token.uid.c_str();
    Serial.print("User UID: ");
    Serial.println(uid);
    Firebase.reconnectWiFi(true);
}

void FirebaseManager::FBDataNull(std::string data)
{
    if(data == "")
    {
        firebaseOK = false;
        char buffer[100];  // Adjust the buffer size as needed
        sprintf(buffer, "Error: Couldn't get user's %s", data.c_str());
        Serial.println(buffer); 
        //Serial.println("buffer" + data.c_str());
    }
}

void FirebaseManager::update(SystemTime *systemTime)
{
    auto hour = systemTime->getHour();
    auto minute = systemTime->getMinute();
    Serial.printf("Internal RTC Time: %.2d:%.2d\n", hour, minute);
    systemTime->notifierEngine();
    if((minute % 10) == 0)
    {
        if(firebaseOK)
        {
            auto sensorList = Configuration::getInstance()->getSensorList();
            auto temperature = Temperature::getInstance();
            auto databasePath = fbData.getDatabaseRootName() + std::to_string(systemTime->getYear()) + "/" + std::to_string(systemTime->getMonth()) + "/" 
                            + std::to_string(systemTime->getDay()) + "/" + std::to_string(systemTime->getHour());
            std::string mergedSensorData;
            JsonDocument doc;
            JsonArray array = doc.to<JsonArray>();
            temperature->read();
            for(auto sensor : sensorList)
            {
                JsonObject obj = array.createNestedObject();
                auto sensorName = sensor.getName();
                obj[sensorName.c_str()] = temperature->getData(sensorName);

            }
            serializeJson(doc, mergedSensorData);
            FirebaseJson fbJson(mergedSensorData);

            // Convert std::string to Arduino String
            //String databasePathStr = String(databasePath.c_str());

            if(Firebase.RTDB.getJSON(&fbdo, databasePath) != NULL) return; // skip sensor
            
            Serial.println("new data is about to be registered on the database!");
            if (Firebase.RTDB.setJSON(&fbdo, databasePath + "/", &fbJson))
            {
                Serial.println("PASSED");
                Serial.println("PATH: " + fbdo.dataPath());
                Serial.println("TYPE: " + fbdo.dataType());
            }
            else
            {
                Serial.println("FAILED");
                Serial.println("REASON: " + fbdo.errorReason());
            }
            // else
            // {
            //     Serial.println("warning: the data for the current time and date is already registered on the database!");
            // }
            
        }
    }
}