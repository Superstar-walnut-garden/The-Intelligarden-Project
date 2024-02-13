#include <ESP32Time.h>
#include <FirebaseESP32.h>
#include <time.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define API_KEY "AIzaSyAsGvcYkkIhaLzNqt6YIt7DVEskcAKUHig"
#define DATABASE_URL "https://the-intelligarden-project-default-rtdb.firebaseio.com/" 
#define USER_EMAIL "IntelligardenNodeAznab@walnut.garden"
#define USER_PASSWORD "ThisIsThePasscode"
#define DATABASE_ROOT_NAME "AznabNode1/"

#include <OneWire.h>
#include <DallasTemperature.h>

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <rom/rtc.h>

constexpr double nightTemp = 23.0, dayTemp = 25.0;
constexpr double threshold = 0.05;

// Replace with your network credentials
const char* ssid     = "Superstar-Garden 2.4";
const char* password = "12345654321";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

OneWire oneWireBus(15);
DallasTemperature sensors(&oneWireBus);

bool isTimeUpdated = true;
ESP32Time rtc(0);

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
// Variable to save USER UID
String uid;

bool signupOK = false;

IPAddress local_IP(192, 168, 1, 184);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(178, 22, 122, 100);
IPAddress secondaryDNS;

void lostTrackOfTime()
{
    Serial.println("Error: cannot connect to the network!!!");
    Serial.printf("reset reason core 0 code: %d\n", rtc_get_reset_reason(0));
    Serial.printf("reset reason core 1 code: %d\n", rtc_get_reset_reason(1));
    if(!(rtc_get_reset_reason(0) == 0xc or rtc_get_reset_reason(1) == 0xc)) // if reset cause is not from the softdog
    {
        Serial.println("reset reason is not from soft dog");
        isTimeUpdated = false;
        Serial.println("\n\n Warning: Lost track of time. Time is set to 05:00!");
        rtc.setTime(0, 0, 5, 1, 1, 2024);
    }
}

void setup()
{
    Serial.begin(115200);
    sensors.begin();
    Serial.println("Sensors initialized!");
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        static int localCounter = 0;
        delay(500);
        Serial.print(".");
        if(localCounter ++ > 10)
        {
            localCounter = 0;
            break;
        }
    }
    if(WiFi.status() == WL_CONNECTED)
    {
        bool firebaseOK = true;
        // Print local IP address and start web server
        Serial.println("");
        Serial.println("WiFi connected.");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        // ------------- firebase setup ----------------
        // Assign the api key (required)
        config.api_key = API_KEY;
        /* Assign the RTDB URL (required) */
        config.database_url = DATABASE_URL;
        // Assign the user sign in credentials
        auth.user.email = USER_EMAIL;
        auth.user.password = USER_PASSWORD;
        fbdo.setResponseSize(4096);
        // Assign the callback function for the long running token generation task
        config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

        // Assign the maximum retry of token generation
        config.max_token_generation_retry = 5;
        // Initialize the library with the Firebase authen and config
        Firebase.begin(&config, &auth);
        // Getting the user UID might take a few seconds
        Serial.println("Getting User UID");
        while ((auth.token.uid) == "") {
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

        // ------------- time server setup ----------------
        Serial.println("Connecting to time server!");
        Serial.println("time offset: 3:30 GMT");
        timeClient.setTimeOffset(12600);
        timeClient.begin();
        Serial.print("Retriving time and date...  ");
            //timeClient.update();
        isTimeUpdated = true;
        while(!timeClient.update())
        {
            if(timeClient.forceUpdate())
            {
                isTimeUpdated = true;
                break;
            }
            else
            {
                Serial.println("Failed!");
                isTimeUpdated = false;
                static int localCounter = 0;
                if(localCounter ++ > 5)
                {
                    localCounter = 0;
                    Serial.println("Fatal Error: can't reach NTP server. Check your internet connection.");
                    lostTrackOfTime();
                    break;
                }
            }
        }
        if(isTimeUpdated)
        {
            Serial.println("Passed!");
            auto hour = timeClient.getHours();
            auto minute = timeClient.getMinutes();
            time_t c;
            c = timeClient.getEpochTime();
            Serial.println(ctime(&c));
            struct tm* timeinfo = localtime(&c);
            auto day = timeinfo->tm_mday;
            auto month = timeinfo->tm_mon + 1;
            auto year = timeinfo->tm_year + 1900;
            Serial.println(("year = " + std::to_string(year)).c_str());
            Serial.println(("month = " + std::to_string(month)).c_str());
            Serial.println(("day = " + std::to_string(day)).c_str());
            rtc.setTime(0, minute, hour, day, month, year);
            Serial.println("RTC sub-system updated successfully!");
            Serial.print("HOUR: ");
            Serial.println(hour);
            Serial.print("MINUTE: ");
            Serial.println(minute);

            if(firebaseOK)
            {
                auto databasePath = DATABASE_ROOT_NAME + std::to_string(year) + "/" + std::to_string(month) + "/" 
                              + std::to_string(day) + "/" + std::to_string(hour);
                sensors.requestTemperatures();
                if(Firebase.RTDB.getFloat(&fbdo, databasePath) == NULL)
                {
                    Serial.println("new data is about to be registered on the database!");
                    if (Firebase.RTDB.setFloat(&fbdo, databasePath, sensors.getTempCByIndex(0)))
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
                }
                else
                {
                    Serial.println("warning: the data for the current time and date is already registered on the database!");
                }
            }
            else
                Serial.println("Error: User's UID is not available, database registration skipped!");
        }
    }
    else
    {
        lostTrackOfTime();
    }
}

void loop()
{
    delay(1); // For other threads to work.this should be 1ms in the main setup
    static int cnt = 20;
    static int softdog = 0;
    if(softdog ++ > 100)
    {
        softdog = 0;
        esp_restart(); // Continuesly reset to support the system stability;
    }
    if(WiFi.status() == WL_CONNECTED)
    {

    }
    Serial.println(WiFi.status() == WL_CONNECTED ? "Wifi is Connected!" : "Fatal Error: Wifi is disconnected!!!");


    sensors.requestTemperatures();
    double temp = sensors.getTempCByIndex(0);
    Serial.print("temp =");
    Serial.println(temp);
    
    int hour = rtc.getHour(true);
    int minute = rtc.getMinute();
    Serial.printf("Internal RTC Time: %.2d:%.2d\n", hour, minute);

}
