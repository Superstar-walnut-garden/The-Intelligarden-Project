#include "CentralizedSignalHubService.hpp"
#include "ArduinoJson.h"
#include "SignalNameResolver.hpp"

/**
 * @brief initialize the instance of the CentralizedSignalHubService to null
 * 
 */
CentralizedSignalHubService* CentralizedSignalHubService::instance = nullptr;
/**
 * @brief Get the instance of the CentralizedSignalHubService (singleton pattern).
 * 
 * @return CentralizedSignalHubService* The instance of the CentralizedSignalHubService.
 */
CentralizedSignalHubService* CentralizedSignalHubService::getInstance()
{
    if (!instance)
    {
        instance = new CentralizedSignalHubService();
    }
    return instance;
}

/**
 * @brief register a signal compatible subsystem (service) 
 * @param service a refrence of the desired service to be added to the service registry
 */
void CentralizedSignalHubService::registerService(ISignalCompatibleService *service)
{
    services.push_back(service);
    service->attach(this); // listen to changes in service items
}

/**
 * @brief get service registry
 * @return a list (vector) of services
 */
std::vector<ISignalCompatibleService *> CentralizedSignalHubService::getServices()
{
    return services;
}

/**
 * @brief check if a signal path is valid
 * @param fullSignalPath the full signal path to be checked
 * @return true if the signal path is valid, false otherwise
 */
bool CentralizedSignalHubService::isSignalPathValid(std::string fullSignalPath)
{
    for(auto *service : services) // iterate over services
    {
        for(auto &item : service->getSignalCompatibleItems()) // iterate over items
        {
            for(auto &localPath : item->getLocalSignalNames()) // iterate over local signals
            {
                SignalNameResolver::SignalNameParameters signalParams;
                signalParams.subsystemName = service->getName();
                signalParams.id = item->getId();
                signalParams.localSignalName = localPath;
                if(SignalNameResolver::toString(signalParams) == fullSignalPath)
                    return true; // found a match
            }
        }
    }
    return false; // no match found
}

/**
 * @brief get a complete list of signal-compatible items
 * @return json list of signal-compatible items including their local signals
 */
std::string CentralizedSignalHubService::getAll()
{
    JsonDocument doc;
    JsonArray servicesArray = doc.to<JsonArray>();
    for(auto *service : services) // iterate over services
    {
        JsonObject serviceObj = servicesArray.createNestedObject();
        serviceObj["name"] = service->getName();
        JsonArray itemsArray = serviceObj.createNestedArray("items");
        for(auto &item : service->getSignalCompatibleItems()) // iterate over items
        {
            JsonObject itemObj = itemsArray.createNestedObject();
            itemObj["name"] = item->getName();
            JsonArray signalPathArray = itemObj.createNestedArray("signals");
            for(auto &localPath : item->getLocalSignalNames())
            {
                SignalNameResolver::SignalNameParameters signalParams;
                signalParams.subsystemName = service->getName();
                signalParams.id = item->getId();
                signalParams.localSignalName = localPath;
                signalPathArray.add(SignalNameResolver::toString(signalParams));
            }
        }
    }
    std::string serializedJson;
    serializeJson(doc, serializedJson);
    return serializedJson;
}

/**
 * @brief this is automaticaly called when any of the registered services changes (item modification or removal).
 * 
 */
void CentralizedSignalHubService::update(ISignalCompatibleService *scm)
{
    notify(); // do nothing and pass it to the observer (SignalRouterService in this case).
}