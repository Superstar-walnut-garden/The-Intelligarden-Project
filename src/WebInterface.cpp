#include "WebInterface.hpp"

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

    server.on("/getSensorList", HTTP_GET, [](AsyncWebServerRequest *request)
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

    server.on("/getPumpSchedule", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        String startTime, duration;
        auto *cfg = Configuration::getInstance();
        cfg->getSchedulerList().printList();
        auto scheduleJson = cfg->getSchedulerList().getListJson().c_str();
        request->send(200, "application/json", scheduleJson);
    });

    server.on("/getPumpStatus", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        auto *pumpObject = Pump::getInstance();

        String json = "{\"state\": \"" + String(pumpObject->getPumpState()) +
            "\" , \"rTime\": \"" + String("0:00") + "\"}";
        request->send(200, "application/json", json);
    });

    server.on("/getCurrentTime", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        JsonDocument doc; // creating a json doc
        doc["time"] = SystemTime::getInstance()->getTime().toString(); // get time and add to json
        doc["weekday"] = SystemTime::getInstance()->getWeekdayString(); // get weekday and add to json
        std::string output; // declaring a string for serializing json data
        serializeJson(doc, output); // convert json data to a string
        request->send(200, "application/json", output.c_str()); // sending data to client
    });

    // Handle form submission and save credentials
    server.on("/getWifiState", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        
        auto wifiCred = Configuration::getInstance()->getWifiCredentials();
        Serial.println("Credentials saved successfully.");
        request->send(200, "application/json", wifiCred.toJsonString().c_str());
    });

    server.on("/getHotspotConfig", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        
        auto hotspotCred = Configuration::getInstance()->getHotspotCredentials();
        Serial.println("Credentials saved successfully.");
        request->send(200, "application/json", hotspotCred.toJsonString().c_str());
    });

    server.on("/setWifiConfig", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        Configuration::getInstance()->setWifiCredentials(WifiHotspotData((char *) data));
    });

    server.on("/setHotspotConfig", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", ""); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        Configuration::getInstance()->setHotspotCredentials(WifiHotspotData((char *) data));
    });

    server.on("/setPumpSchedule", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        Serial.println("setPumpSchedule request handled!////////////////////////");
        request->send(200, "text/plain", "pump schedule set!"); // Response to client
    }, NULL
    , [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        auto *cfg = Configuration::getInstance();
        Serial.println();
        // Parse JSON payload
        JsonDocument json;
        deserializeJson(json, data);
        Serial.println("Payload elements:");
        for (JsonPair jNode : json.as<JsonObject>()) // Extract data from JSON payload
        {
            Serial.print(jNode.key().c_str()); // Print data
            Serial.print(": ");
            Serial.println(jNode.value().as<String>());
        }
        cfg->setSchedulerList((char *)(data));
    });

    server.on("/sendCommand", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL,
        [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
    {
        for (size_t i = 0; i < len; i++) {
            Serial.write(data[i]);
        }

        Serial.println();
        // Parse JSON payload
        DynamicJsonDocument json(128);
        deserializeJson(json, data);
        String command = json["command"].as<String>();

        Serial.print("Command received: ");
        Serial.println(command);

        auto *pump = Pump::getInstance();
        if(command.equals("turn_on"))
        {
            int duration = std::stoi(std::string(json["duration"].as<String>().c_str()));
            // pump->manualSwitch(duration);
        }
        if(command.equals("turn_off"))
        {
            // pump->manualSwitch(); // turn off the pump
        }
        request->send(SPIFFS, "/pumpSetting.html", "text/html");
    });

    server.on("/setSensorList", HTTP_POST, [](AsyncWebServerRequest *request)
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
        DynamicJsonDocument json(len);
        deserializeJson(json, data);
        Serial.println("Payload elements:");
        for (JsonPair jNode : json.as<JsonObject>()) // Extract data from JSON payload
        {
            Serial.println("Before pushback");
            list.push_back(TempSensorNode(std::stoll(jNode.key().c_str())
                , std::string(jNode.value().as<String>().c_str())));
            Serial.println("After pushback");
            Serial.print(jNode.key().c_str()); // Print data
            Serial.print(": ");
            Serial.println(jNode.value().as<String>());
        }
        cfg->storeSensorNames(list);
    });
}

// Method to start the web server
void WebInterface::init()
{
    server.begin();
}
