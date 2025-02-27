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

    server.on("/api/getSensorList", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        String json;
        auto *cfg = Configuration::getInstance();
        json += "{";
        for(auto dev : cfg->getSensorList())
            json += "\"" + String(std::to_string((uint64_t)dev.getAddress()).c_str()) +
                "\": \"" + String(dev.getName().c_str()) + "\",";
        json.remove(json.length() - 1); // remove the final ","
        json += "}";
        request->send(200, "application/json", json);
    });

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

    server.on("/api/setSensorList", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        auto *cfg = Configuration::getInstance();
        std::vector<TempSensorNode> list;
        for (size_t i = 0; i < len; i++)
            Serial.write(data[i]);

        Serial.println();
        // Parse JSON payload
        JsonDocument json;
        deserializeJson(json, data);
        Serial.println("Payload elements:");
        for (JsonPair jNode : json.as<JsonObject>()) // Extract data from JSON payload
        {
            Serial.println("Before pushback");
            list.push_back(TempSensorNode(std::stoull(std::string(jNode.key().c_str()))
                , std::string(jNode.value().as<String>().c_str())));
            Serial.println("After pushback");
            Serial.print(jNode.key().c_str()); // Print data
            Serial.print(": ");
            Serial.println(jNode.value().as<String>());
        }
        cfg->storeSensorNames(list);
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
        EventManager::getInstance()->createEvent(eventItem);
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
        int id = json["id"];
        EventManager::getInstance()->removeEvent(id);
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
        EventManager::getInstance()->modifyEvent(newItem.getId(), newItem);
        EventManager::getInstance()->saveState();
    });

    // Endpoint to get the entire list of events
    server.on("/api/getEventList", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        std::string eventListJson = EventManager::getInstance()->getEventListJson();
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
        Scheduler::getInstance()->createSchedule(newItem.getId(), newItem);
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
        int id = json["id"];
        Scheduler::getInstance()->removeSchedule(id);
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
        Scheduler::getInstance()->modifySchedule(newItem.getId(), newItem);
        Scheduler::getInstance()->saveState();
    });

    // Endpoint to get the entire list of schedules
    server.on("/api/getScheduleList", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        std::string scheduleListJson = Scheduler::getInstance()->getSchedulerList().toJson();
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
        GPIOManager::getInstance()->createIO(newItem);
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
        int id = json["id"];
        GPIOManager::getInstance()->removeIO(id);
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
        GPIOManager::getInstance()->modifyIO(newItem.getId(), newItem);
    });

    // Endpoint to get the entire list of GPIO items
    server.on("/api/getGPIOList", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        std::string gpioListJson = GPIOManager::getInstance()->getGPIOListJson();
        request->send(200, "application/json", gpioListJson.c_str());
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
}

// Method to start the web server
void WebInterface::init()
{
    server.begin();
}
