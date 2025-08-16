#include "WebApiService.hpp"
#include <SPIFFS.h>
#include <ArduinoJson.h>

#include "Configuration.hpp"
#include "CentralizedSignalHub.hpp"
#include "SystemTimeService.hpp"
#include "GpioService.hpp"
#include "ThermostatService.hpp"
#include "SignalRouterService.hpp"
#include "SchedulerService.hpp"
#include "WifiService.hpp"
#include "DisplayService.hpp"
#include "FirebaseService.hpp"
#include "WifiHotspotConfig.hpp"

/**
 * @brief Construct a new WebApiService object
 * 
 */
WebApiService::WebApiService() : server(80), baseUrl("/api"), hotspotCred()
{
    // Configure access point
    hotspotCred.populateFromJson(Configuration::getInstance()->getHotspotCredentials());
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
void WebApiService::init()
{
    // Serve HTML page to enter WiFi credentials
    server.serveStatic("/", SPIFFS, "/dist/")
        .setDefaultFile("index.html")
        .setCacheControl("max-age=0"); // disable browser cache due to hash-free assets

    createEndpoint("/getCurrentTime", [](std::string data) -> std::string
    {
        JsonDocument doc;
        doc["time"] = SystemTimeService::getInstance()->getTime().toString();
        doc["weekday"] = SystemTimeService::getInstance()->getWeekdayString();
        std::string output;
        serializeJson(doc, output);
        return output;
    });

    createEndpoint("/getWifiState", [](std::string data) -> std::string
    {
        return WifiService::getInstance()->getConfig();
    });

    createEndpoint("/getHotspotConfig", [this](std::string data) -> std::string
    {
        return this->getConfig();
    });

    createEndpoint("/setWifiConfig", [](std::string data) -> std::string
    {
        WifiService::getInstance()->updateConfig(WifiHotspotConfig(data));
        Serial.println("Credentials saved successfully.");
        return "";
    }, true);

    createEndpoint("/setHotspotConfig", [this](std::string data) -> std::string
    {
        this->updateConfig(WifiHotspotConfig(data));
        Serial.println("Credentials saved successfully.");
        return "";
    }, true);

    createEndpoint("/getFirebaseData", [](std::string data) -> std::string
    {
        
        Serial.println("FirebaseData get request handled.");
        return FirebaseService::getInstance()->getConfig();
    });

    createEndpoint("/setFirebaseData", [](std::string data) -> std::string
    {
        FirebaseService::getInstance()->updateConfig(FirebaseServiceConfig(data));
        Serial.println("FirebaseData set request handled.");
        return "";
    }, true);

    createEndpoint("/getDisplayConfig", [](std::string data) -> std::string
    {
        Serial.println("DisplayConfig get request handled.");
        return DisplayService::getInstance()->getConfig();
    });

    createEndpoint("/setDisplayConfig", [](std::string data) -> std::string
    {
        DisplayService::getInstance()->updateConfig(DisplayConfig(data));
        Serial.println("DisplayConfig set request handled.");
        return "";
    }, true);

    createEndpoint("/time-config", [](std::string data) -> std::string
    {
        return SystemTimeService::getInstance()->getConfig();
    }); // get request

    createEndpoint("/time-config", [](std::string data) -> std::string
    {
        SystemTimeService::getInstance()->setConfig(SystemTimeConfig(data).toJson());
        Serial.println("Time configuration saved successfully.");
        return "";
    }, true); // post request

    createEndpoint("/signal-hub-items", [](std::string data) -> std::string
    {
        auto signalHubItems = CentralizedSignalHub::getInstance()->getListJson();
        return signalHubItems;
    }); // get request

    createEndpoint("/restart", [](std::string data) -> std::string
    {
        Serial.println("restarting...");
        delay(3000);
        ESP.restart();
        return "";
    }, true);

    createIManagerEndpoints<TempSensorItem>("/TempSensor", TempSensorService::getInstance());
    createIManagerEndpoints<GpioItem>("/GPIO", GpioService::getInstance());
    createIManagerEndpoints<ThermostatItem>("/Thermostat", ThermostatService::getInstance());
    createIManagerEndpoints<SchedulerItem>("/Scheduler", SchedulerService::getInstance());
    createIManagerEndpoints<SignalRouterItem>("/signal", SignalRouterService::getInstance());
    server.begin();
}

/**
 * @brief Method to create an endpoint
 * 
 * @param uri URI of the endpoint
 * @param handler Handler function for the endpoint
 * @param post Flag to indicate if the endpoint is a POST request (true) or GET request (false)
 */
void WebApiService::createEndpoint(std::string uri, std::function<std::string(std::string)> handler, bool post)
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
 * @brief Method to create endpoints for IResourceController objects
 * 
 * @tparam ItemType Type of the item managed by the IResourceController object
 * @param uri URI of the endpoint
 * @param manager IResourceController object
 */
template <typename ItemType>
void WebApiService::createIManagerEndpoints(std::string uri, IResourceController<ItemType>* manager)
{
    auto populator = [] (std::string data) -> ItemType
    {
        auto item = ItemType();
        item.populateFromJson(data);
        return item;
    };
    createEndpoint(uri + "/get", [manager] (std::string data) -> std::string
    {
        return manager->getAll();
    });
    createEndpoint(uri + "/create", [manager, populator] (std::string data) -> std::string
    {
        manager->create(populator(data));
        return "";
    }, true);
    createEndpoint(uri + "/modify", [manager, populator] (std::string data) -> std::string
    {
        auto item = populator(data);
        manager->update(item.getId(), item);
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

/**
 * @brief store config in storage
 * 
 */
void WebApiService::storeAll()
{
    Configuration::getInstance()->setHotspotCredentials(hotspotCred.toJson());
}

/**
 * @brief restore config from storage
 * 
 */
void WebApiService::restoreAll()
{
    hotspotCred.populateFromJson(Configuration::getInstance()->getHotspotCredentials());
}

/**
 * @brief update config (for web-api)
 * @param cfg new configuration to replace the old one
 */
void WebApiService::updateConfig(WifiHotspotConfig cfg)
{
    this->hotspotCred = cfg;
    storeAll();
}

/**
 * @brief get config in json string format (for web-api)
 * @return std::string configuration json string
 */
std::string WebApiService::getConfig()
{
    return hotspotCred.toJson();
}