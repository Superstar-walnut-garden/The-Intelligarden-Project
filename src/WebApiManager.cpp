#include "WebApiManager.hpp"
#include "Configuration.hpp"
#include "EventManager.hpp"

/**
 * @brief Construct a new WebApiManager object
 * 
 */
WebApiManager::WebApiManager() : server(80), baseUrl("/api")
{
    // Configure access point
    auto hotspotCred = Configuration::getInstance()->getHotspotCredentials();
    auto ssid = hotspotCred.getSsid();
    auto password = hotspotCred.getPassword();
    WiFi.softAP(ssid.c_str(), password.c_str());
    Serial.println();
    Serial.print("SoftAP IP address: ");
    Serial.println(WiFi.softAPIP());
}

/**
 * @brief Method to start the web server and create api endpoints
 * 
 */
void WebApiManager::init()
{
    // Serve HTML page to enter WiFi credentials
    server.serveStatic("/", SPIFFS, "/dist/").setDefaultFile("index.html");

    createEndpoint("/getCurrentTime", [](std::string data) -> std::string
    {
        JsonDocument doc;
        doc["time"] = SystemTime::getInstance()->getTime().toString();
        doc["weekday"] = SystemTime::getInstance()->getWeekdayString();
        std::string output;
        serializeJson(doc, output);
        return output;
    });

    createEndpoint("/getWifiState", [](std::string data) -> std::string
    {
        auto wifiCred = Configuration::getInstance()->getWifiCredentials();
        return wifiCred.toJsonString();
    });

    createEndpoint("/getHotspotConfig", [](std::string data) -> std::string
    {
        auto hotspotCred = Configuration::getInstance()->getHotspotCredentials();
        return hotspotCred.toJsonString();
    });

    createEndpoint("/setWifiConfig", [](std::string data) -> std::string
    {
        Configuration::getInstance()->setWifiCredentials(WifiHotspotData(data.c_str()));
        Serial.println("Credentials saved successfully.");
        return "";
    }, true);

    createEndpoint("/setHotspotConfig", [](std::string data) -> std::string
    {
        Configuration::getInstance()->setHotspotCredentials(WifiHotspotData(data.c_str()));
        Serial.println("Credentials saved successfully.");
        return "";
    }, true);

    createEndpoint("/getFirebaseData", [](std::string data) -> std::string
    {
        auto firebaseData = Configuration::getInstance()->getFirebaseData();
        Serial.println("FirebaseData get request handled.");
        return firebaseData.toJsonString();
    });

    createEndpoint("/setFirebaseData", [](std::string data) -> std::string
    {
        Configuration::getInstance()->setFirebaseData(FBData(data.c_str()));
        Serial.println("FirebaseData set request handled.");
        return "";
    }, true);

    createEndpoint("/getDisplayConfig", [](std::string data) -> std::string
    {
        auto displayConfig = Configuration::getInstance()->getDisplayConfig();
        Serial.println("DisplayConfig get request handled.");
        return displayConfig;
    });

    createEndpoint("/setDisplayConfig", [](std::string data) -> std::string
    {
        Configuration::getInstance()->setDisplayConfig(data.c_str());
        Serial.println("DisplayConfig set request handled.");
        return "";
    }, true);

    createEndpoint("/restart", [](std::string data) -> std::string
    {
        Serial.println("restarting...");
        delay(3000);
        ESP.restart();
        return "";
    }, true);

    createIManagerEndpoints<TempSensorItem>("/TempSensor", Temperature::getInstance());
    createIManagerEndpoints<GPIOItem>("/GPIO", GPIOManager::getInstance());
    createIManagerEndpoints<ThermostatItem>("/Thermostat", ThermostatManager::getInstance());
    createIManagerEndpoints<EventItem>("/Event", EventManager::getInstance());
    createIManagerEndpoints<SchedulerItem>("/Scheduler", Scheduler::getInstance());
    server.begin();
}

/**
 * @brief Method to create an endpoint
 * 
 * @param uri URI of the endpoint
 * @param handler Handler function for the endpoint
 * @param post Flag to indicate if the endpoint is a POST request (true) or GET request (false)
 */
void WebApiManager::createEndpoint(std::string uri, std::function<std::string(std::string)> handler, bool post)
{
    if (post)
    {
        server.on((baseUrl + uri).c_str(), HTTP_POST, [](AsyncWebServerRequest *request)
        {
            request->send(200, "text/plain", ""); // Response to client
        }, NULL,
        [handler](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            std::string jsonData(reinterpret_cast<const char*>(data), len);
            std::string response = handler(jsonData);
        });
    }
    else
    {
        server.on((baseUrl + uri).c_str(), HTTP_GET, [handler](AsyncWebServerRequest *request)
        {
            std::string response = handler("");
            request->send(200, "application/json", response.c_str());
        });
    }
}

/**
 * @brief Method to create endpoints for IManager objects
 * 
 * @tparam ItemType Type of the item managed by the IManager object
 * @param uri URI of the endpoint
 * @param manager IManager object
 */
template <typename ItemType>
void WebApiManager::createIManagerEndpoints(std::string uri, IManager<ItemType>* manager)
{
    auto populator = [] (std::string data) -> ItemType
    {
        auto item = ItemType();
        item.populateFromJson(data);
        return item;
    };
    createEndpoint(uri + "/get", [manager] (std::string data) -> std::string
    {
        return manager->getListJson();
    });
    createEndpoint(uri + "/create", [manager, populator] (std::string data) -> std::string
    {
        manager->create(populator(data));
        return "";
    }, true);
    createEndpoint(uri + "/modify", [manager, populator] (std::string data) -> std::string
    {
        auto item = populator(data);
        manager->modify(item.getId(), item);
        return "";
    }, true);
    createEndpoint(uri + "/delete", [manager, populator] (std::string data) -> std::string
    {
        auto item = BaseItem(); // only use BaseItem because it only needs the ID
        item.populateFromJson(data);
        manager->remove(item.getId());
        return "";
    }, true);
}