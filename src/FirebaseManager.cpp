#include "FirebaseManager.hpp"
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

FirebaseManager::FirebaseManager(FBData fbData): signupOK(false), firebaseOK(false), updateTimestamp(-1), firebaseAbnormalityID(-1)
{
    this->fbData = fbData;
}

void FirebaseManager::init()
{
    if(fbData.isNull() or !fbData.isEnabled())
        return;
    
    firebaseOK = true;
    // Assign the api key (required)
    config.api_key = fbData.getApiKey().c_str();
    // Assign the RTDB URL (required) 
    config.database_url = fbData.getDatabaseURL().c_str();
    // Assign the user sign in credentials
    auth.user.email = fbData.getUserEmail().c_str();

    auth.user.password = fbData.getUserPassword().c_str();

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
            if (firebaseAbnormalityID == -1) // Abnormality not yet reported 
            { 
                firebaseAbnormalityID = SystemMaintainer::getInstance().createTrackableAbnormality("firebase uid Issue", 2); // Trigger restart in 2 minutes 
            }
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

void FirebaseManager::update(SystemTime *systemTime)
{
    auto hour = systemTime->getHour();
    auto minute = systemTime->getMinute();
    Serial.printf("Internal RTC Time: %.2d:%.2d\n", hour, minute);
    if(firebaseOK and hour != updateTimestamp) // if firebase is ok and the data for this hour is not already uploaded
    {
        auto temperature = Temperature::getInstance();
        auto databasePath = fbData.getDatabaseRootName() + std::to_string(systemTime->getYear()) + "/" + std::to_string(systemTime->getMonth()) + "/" 
                        + std::to_string(systemTime->getDay()) + "/" + std::to_string(systemTime->getHour());

        temperature->read();
        JsonDocument doc; // Adding sensor data to JSON 
        temperature->forEachSensor([&doc, temperature](TempSensorItem sensor)
        { 
            doc[sensor.getName()] = temperature->getData(sensor.getName()); 
        }, true);
        // Serialize JSON to string and print 
        String mergedSensorData; 
        serializeJson(doc, mergedSensorData); 
        Serial.println(mergedSensorData.c_str());

        FirebaseJson fbJson(mergedSensorData.c_str());

        // Convert std::string to Arduino String
        //String databasePathStr = String(databasePath.c_str());

        if (!Firebase.RTDB.get(&fbdo, databasePath)) 
        {
            Serial.println("New data is about to be registered on the database!");
            if (Firebase.RTDB.setJSON(&fbdo, databasePath, &fbJson)) 
            {
                updateTimestamp = hour;
                Serial.println("PASSED");
                Serial.printf("PATH: %s\n", fbdo.dataPath().c_str());
                Serial.printf("TYPE:  %s\n", fbdo.dataType().c_str());
                if (firebaseAbnormalityID != -1) // Reported abnormality now resolved 
                { 
                    SystemMaintainer::getInstance().resolveAbnormality(firebaseAbnormalityID); 
                    Serial.println(firebaseAbnormalityID); 
                }
            } else 
            {
                Serial.println("FAILED");
                Serial.printf("REASON:  %s\n", fbdo.errorReason().c_str());
                if (firebaseAbnormalityID == -1) // Abnormality not yet reported 
                { 
                    firebaseAbnormalityID = SystemMaintainer::getInstance().createTrackableAbnormality("firebase data upload Issue", 2); // Trigger restart in 2 minutes 
                }
            }
        } else 
        {
            updateTimestamp = hour;
            Serial.println("Warning: The data for the current time and date is already registered on the database!");
        }
    }
}