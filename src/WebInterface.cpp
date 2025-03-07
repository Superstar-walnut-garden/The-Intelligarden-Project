#include "WebInterface.hpp"
#include "Configuration.hpp"
#include "EventManager.hpp"

// Constructor implementation
WebInterface::WebInterface() : server(80)
{
    // Configure access point
    auto hotspotCred = Configuration::getInstance()->getHotspotCredentials();
    auto ssid = hotspotCred.getSsid();
    auto password = hotspotCred.getPassword();
    WiFi.softAP(ssid.c_str(), password.c_str());
    Serial.println();
    Serial.print("SoftAP IP address: ");
    Serial.println(WiFi.softAPIP());

    // Serve HTML page to enter WiFi credentials
    server.serveStatic("/", SPIFFS, "/dist/").setDefaultFile("index.html");

    server.on("/api/getCurrentTime", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        JsonDocument doc; // creating a json doc
        doc["time"] = SystemTime::getInstance()->getTime().toString(); // get time and add to json
        doc["weekday"] = SystemTime::getInstance()->getWeekdayString(); // get weekday and add to json
        std::string output; // declaring a string for serializing json data
        serializeJson(doc, output); // convert json data to a string
        request->send(200, "application/json", output.c_str()); // sending data to client
    });

    // Handle form submission and save credentials
    server.on("/api/getWifiState", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        
        auto wifiCred = Configuration::getInstance()->getWifiCredentials();
        Serial.println("Credentials saved successfully.");
        request->send(200, "application/json", wifiCred.toJsonString().c_str());
    });

    server.on("/api/getHotspotConfig", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        
        auto hotspotCred = Configuration::getInstance()->getHotspotCredentials();
        Serial.println("Credentials saved successfully.");
        request->send(200, "application/json", hotspotCred.toJsonString().c_str());
    });

    server.on("/api/setWifiConfig", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        Configuration::getInstance()->setWifiCredentials(WifiHotspotData((char *) data));
    });

    server.on("/api/setHotspotConfig", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        Configuration::getInstance()->setHotspotCredentials(WifiHotspotData((char *) data));
    });

    server.on("/api/getFirebaseData", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        auto firebaseData = Configuration::getInstance()->getFirebaseData();
        Serial.println("FirebaseData get request handled.");
        request->send(200, "application/json", firebaseData.toJsonString().c_str());
    });

    server.on("/api/setFirebaseData", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        Configuration::getInstance()->setFirebaseData(FBData((char *) data));
    });

    // Endpoint to create an event
    server.on("/api/createEvent", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        Serial.println((char*)data);
        auto eventItem = EventItem();
        eventItem.populateFromJson((char *)data);
        EventManager::getInstance()->create(eventItem);
        EventManager::getInstance()->saveState();
    });

    // Endpoint to delete an event
    server.on("/api/deleteEvent", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        JsonDocument json;
        deserializeJson(json, data);
        uint64_t id = json["id"];
        EventManager::getInstance()->remove(id);
        EventManager::getInstance()->saveState();
    });

    // Endpoint to modify an event
    server.on("/api/modifyEvent", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        auto newItem = EventItem();
        newItem.populateFromJson((char *)data);
        EventManager::getInstance()->modify(newItem.getId(), newItem);
        EventManager::getInstance()->saveState();
    });

    // Endpoint to get the entire list of events
    server.on("/api/getEventList", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        std::string eventListJson = EventManager::getInstance()->getListJson();
        request->send(200, "application/json", eventListJson.c_str());
    });

    // Endpoint to create a schedule
    server.on("/api/createSchedule", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        Serial.println((char*)data);
        auto newItem = SchedulerItem();
        newItem.populateFromJson((char*)data);
        Scheduler::getInstance()->create(newItem);
        Scheduler::getInstance()->saveState();
    });

    // Endpoint to delete a schedule
    server.on("/api/deleteSchedule", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        JsonDocument json;
        deserializeJson(json, data);
        uint64_t id = json["id"];
        Scheduler::getInstance()->remove(id);
        Scheduler::getInstance()->saveState();
    });

    // Endpoint to modify a schedule
    server.on("/api/modifySchedule", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        auto newItem = SchedulerItem();
        newItem.populateFromJson((char*)data);
        Scheduler::getInstance()->modify(newItem.getId(), newItem);
        Scheduler::getInstance()->saveState();
    });

    // Endpoint to get the entire list of schedules
    server.on("/api/getScheduleList", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        std::string scheduleListJson = Scheduler::getInstance()->getListJson();
        request->send(200, "application/json", scheduleListJson.c_str());
    });

    // Endpoint to create a GPIO item
    server.on("/api/createGPIO", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        auto newItem = GPIOItem();
        newItem.populateFromJson((char*)data);
        GPIOManager::getInstance()->create(newItem);
    });

    // Endpoint to delete a GPIO item
    server.on("/api/deleteGPIO", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        JsonDocument json;
        deserializeJson(json, data);
        uint64_t id = json["id"];
        GPIOManager::getInstance()->remove(id);
    });

    // Endpoint to modify a GPIO item
    server.on("/api/modifyGPIO", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        auto newItem = GPIOItem();
        newItem.populateFromJson((char*)data);
        GPIOManager::getInstance()->modify(newItem.getId(), newItem);
    });

    // Endpoint to get the entire list of GPIO items
    server.on("/api/getGPIOList", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        std::string gpioListJson = GPIOManager::getInstance()->getListJson();
        request->send(200, "application/json", gpioListJson.c_str());
    });

    // Endpoint to create a Thermostat item
    server.on("/api/createThermostat", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        auto newItem = ThermostatItem();
        newItem.populateFromJson((char*)data);
        ThermostatManager::getInstance()->create(newItem);
    });

    // Endpoint to delete a Thermostat item
    server.on("/api/deleteThermostat", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        JsonDocument json;
        deserializeJson(json, data);
        uint64_t id = json["id"];
        ThermostatManager::getInstance()->remove(id);
    });

    // Endpoint to modify a Thermostat item
    server.on("/api/modifyThermostat", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        auto newItem = ThermostatItem();
        newItem.populateFromJson((char*)data);
        ThermostatManager::getInstance()->modify(newItem.getId(), newItem);
    });

    // Endpoint to get the entire list of Thermostat items
    server.on("/api/getThermostatList", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        std::string listJson = ThermostatManager::getInstance()->getListJson();
        request->send(200, "application/json", listJson.c_str());
    });

    // Endpoint to get the display config
    server.on("/api/getDisplayConfig", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        std::string config = Configuration::getInstance()->getDisplayConfig();
        request->send(200, "application/json", config.c_str());
    });

    // Endpoint to set display config
    server.on("/api/setDisplayConfig", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        Configuration::getInstance()->setDisplayConfig((char*)data);
    });

    // Endpoint to get the temperature sensor list
    server.on("/api/getSensorList", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        std::string json = Temperature::getInstance()->getListJson();
        request->send(200, "application/json", json.c_str());
    });

    // Endpoint to modify a temperature sensor
    server.on("/api/modifySensor", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        auto newItem = TempSensorItem();
        newItem.populateFromJson((char*)data);
        Temperature::getInstance()->modify(newItem.getId(), newItem);
    });

    // Endpoint to delete a temperature sensor
    server.on("/api/deleteSensor", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        JsonDocument json;
        deserializeJson(json, data);
        uint64_t id = std::stoull(json["id"].as<std::string>());
        Serial.println(id);
        Temperature::getInstance()->remove(id);
    });

}

// Method to start the web server
void WebInterface::init()
{
    server.begin();
}
