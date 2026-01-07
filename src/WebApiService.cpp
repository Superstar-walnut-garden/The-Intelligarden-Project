#include "WebApiService.hpp"
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <charconv>

#include "Configuration.hpp"
#include "CentralizedSignalHubService.hpp"
#include "SystemTimeService.hpp"
#include "GpioService.hpp"
#include "ThermostatService.hpp"
#include "SignalRouterService.hpp"
#include "SchedulerService.hpp"
#include "WifiService.hpp"
#include "DisplayService.hpp"
#include "FirebaseService.hpp"
#include "WifiHotspotConfig.hpp"
#include "LogDispatcherService.hpp"
#include "FileExplorerService.hpp"

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

    createEndpoint("/getCurrentTime", [](std::optional<uint64_t> id, std::string data) -> std::string
    {
        JsonDocument doc;
        doc["time"] = SystemTimeService::getInstance()->getTime().toString();
        doc["weekday"] = SystemTimeService::getInstance()->getWeekdayString();
        std::string output;
        serializeJson(doc, output);
        return output;
    });

    createEndpoint("/restart", [](std::optional<uint64_t> id, std::string data) -> std::string
    {
        Serial.println("restarting...");
        delay(3000);
        ESP.restart();
        return "";
    }, Method::Post);

    // create custom endpoint for retriving a file using path
    server.on((baseUrl + "/file").c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
    {
        if (!request->hasParam("path")) 
        {
            request->send(400, "text/plain", "Missing 'path' parameter");
            return;
        }
        std::string path = request->getParam("path")->value().c_str();
        std::string fileContent = FileExplorerService::getInstance()->get(path);
        request->send(200, "application/json", fileContent.c_str());
    });

    createEndpoint<WifiHotspotConfig>("/wifi-config", WifiService::getInstance());
    createEndpoint<WifiHotspotConfig>("/hotspot-config", this);
    createEndpoint<FirebaseServiceConfig>("/firebase-config", FirebaseService::getInstance());
    createEndpoint<DisplayConfig>("/display-config", DisplayService::getInstance());
    createEndpoint<SystemTimeConfig>("/time-config", SystemTimeService::getInstance());
    createEndpoint<LogDispatcherConfig>("/log-config", LogDispatcherService::getInstance());
    createEndpoint<TempSensorConfig>("/temp-sensor-config", TempSensorService::getInstance());
    createEndpoint("/signal-hub", CentralizedSignalHubService::getInstance());

    createEndpoint<TempSensorItem>("/temp-sensor", TempSensorService::getInstance());
    createEndpoint<GpioItem>("/gpio", GpioService::getInstance());
    createEndpoint<ThermostatItem>("/thermostat", ThermostatService::getInstance());
    createEndpoint<SchedulerItem>("/scheduler", SchedulerService::getInstance());
    createEndpoint<SignalRouterItem>("/signal", SignalRouterService::getInstance());
    createEndpoint("/files", FileExplorerService::getInstance());
    server.begin();
}

/**
 * @brief Method to create an endpoint
 * 
 * @param uri URI of the endpoint
 * @param handler Handler function for the endpoint
 * @param method To indicate if the endpoint is a POST, PUT, DELETE or GET request
 */
void WebApiService::createEndpoint(std::string uri, std::function<std::string(std::optional<uint64_t>, std::string)> handler, WebApiService::Method method)
{
    auto extractId = [](AsyncWebServerRequest* request) -> std::optional<uint64_t> 
    {
        if (!request->hasParam("id")) return std::nullopt;

        std::string idStr = request->getParam("id")->value().c_str();
        uint64_t parsedId;
        auto result = std::from_chars(idStr.data(), idStr.data() + idStr.size(), parsedId);

        if (result.ec == std::errc() && result.ptr == idStr.data() + idStr.size()) 
            return parsedId;

        return std::nullopt;
    };

    if (method == Method::Post)
    {
        server.on((baseUrl + uri).c_str(), HTTP_POST, [](AsyncWebServerRequest *request)
        {
            request->send(200, "text/plain", ""); // Response to client
        }, NULL,
        [handler, extractId](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            std::string jsonData(reinterpret_cast<const char*>(data), len);
            std::string response = handler(extractId(request), jsonData);
        });
    }
    else if (method == Method::Get)
    {
        server.on((baseUrl + uri).c_str(), HTTP_GET, [handler, extractId](AsyncWebServerRequest *request)
        {
            std::string response = handler(extractId(request), "");
            request->send(200, "application/json", response.c_str());
        });
    }
    else if (method == Method::Put)
    {
        server.on((baseUrl + uri).c_str(), HTTP_PUT, [](AsyncWebServerRequest *request)
        {
            request->send(200, "text/plain", ""); // Response to client
        }, NULL,
        [handler, extractId](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            std::string jsonData(reinterpret_cast<const char*>(data), len);
            std::string response = handler(extractId(request), jsonData);
        });
    }
    else if (method == Method::Delete)
    {
        server.on((baseUrl + uri).c_str(), HTTP_DELETE, [handler, extractId](AsyncWebServerRequest *request)
        {
            handler(extractId(request), "");
            request->send(200, "text/plain", ""); // Response to client
        });
    }
}

/**
 * @brief Method to create endpoints for IResourceController objects
 * 
 * @tparam ItemType Type of the item managed by the IResourceController object
 * @param uri URI of the endpoint
 * @param resourceController IResourceController object
 */
template <typename ItemType>
void WebApiService::createEndpoint(std::string uri, IResourceController<ItemType>* resourceController)
{
    createEndpoint(uri, [resourceController] (std::optional<uint64_t> id, std::string data) -> std::string
    {
        if(id)
            return resourceController->get(id.value());
        return resourceController->getAll();
    }, Method::Get);
    createEndpoint(uri, [resourceController] (std::optional<uint64_t>  id, std::string data) -> std::string
    {
        auto item = std::make_unique<ItemType>();
        item->populateFromJson(data);
        resourceController->create(std::move(item));
        return "";
    }, Method::Post);
    createEndpoint(uri, [resourceController] (std::optional<uint64_t>  id, std::string data) -> std::string
    {
        auto item = std::make_unique<ItemType>();
        item->populateFromJson(data);
        if(id)
            resourceController->update(id.value(), std::move(item));
        return "";
    }, Method::Put);
    createEndpoint(uri, [resourceController] (std::optional<uint64_t> id, std::string data) -> std::string
    {
        if(id)
            resourceController->remove(id.value());
        return "";
    }, Method::Delete);
}

/**
 * @brief Method to create endpoints for IReadOnlyResourceController objects
 * 
 * @param uri URI of the endpoint
 * @param resourceController IReadOnlyResourceController object
 */
void WebApiService::createEndpoint(std::string uri, IReadOnlyResourceController* resourceController)
{
    createEndpoint(uri, [resourceController] (std::optional<uint64_t> id, std::string data) -> std::string
    {
        if(id)
            return resourceController->get(id.value());
        return resourceController->getAll();
    }, Method::Get);
}

/**
 * @brief Method to create endpoints for IConfigController objects
 * 
 * @tparam ItemType Type of the item managed by the IConfigController object
 * @param uri URI of the endpoint
 * @param configController IConfigController object
 */
template <typename ItemType>
void WebApiService::createEndpoint(std::string uri, IConfigController<ItemType>* configController)
{
    auto populator = [] (std::string data) -> ItemType
    {
        auto item = ItemType();
        item.populateFromJson(data);
        return item;
    };
    createEndpoint(uri, [configController] (std::optional<uint64_t> id, std::string data) -> std::string
    {
        return configController->getConfig();
    }, Method::Get);
    createEndpoint(uri, [configController, populator] (std::optional<uint64_t>, std::string data) -> std::string
    {
        configController->updateConfig(ItemType(data));
        return "";
    }, Method::Put);
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