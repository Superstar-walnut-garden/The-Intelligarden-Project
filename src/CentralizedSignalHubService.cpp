#include "CentralizedSignalHub.hpp"
#include "ArduinoJson.h"
#include "SignalNameResolver.hpp"

/**
 * @brief initialize the instance of the CentralizedSignalHub to null
 * 
 */
CentralizedSignalHub* CentralizedSignalHub::instance = nullptr;
/**
 * @brief Get the instance of the CentralizedSignalHub (singleton pattern).
 * 
 * @return CentralizedSignalHub* The instance of the CentralizedSignalHub.
 */
CentralizedSignalHub* CentralizedSignalHub::getInstance()
{
    if (!instance)
    {
        instance = new CentralizedSignalHub();
    }
    return instance;
}

/**
 * @brief register a signal compatible subsystem (manager) 
 * @param manager a refrence of the desired manager to be added to the manager registry
 */
void CentralizedSignalHub::registerManager(ISignalCompatibleService *manager)
{
    managers.push_back(manager);
    manager->attach(this); // listen to changes in manager items
}

/**
 * @brief get manager registry
 * @return a list (vector) of managers
 */
std::vector<ISignalCompatibleService *> CentralizedSignalHub::getManagers()
{
    return managers;
}

/**
 * @brief check if a signal path is valid
 * @param fullSignalPath the full signal path to be checked
 * @return true if the signal path is valid, false otherwise
 */
bool CentralizedSignalHub::isSignalPathValid(std::string fullSignalPath)
{
    for(auto *manager : managers) // iterate over managers
    {
        for(auto &item : manager->getSignalCompatibleItems()) // iterate over items
        {
            for(auto &localPath : item->getLocalSignalNames()) // iterate over local signals
            {
                SignalNameResolver::SignalNameParameters signalParams;
                signalParams.subsystemName = manager->getName();
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
std::string CentralizedSignalHub::getListJson()
{
    JsonDocument doc;
    JsonArray managersArray = doc.to<JsonArray>();
    for(auto *manager : managers) // iterate over managers
    {
        JsonObject managerObj = managersArray.createNestedObject();
        managerObj["name"] = manager->getName();
        JsonArray itemsArray = managerObj.createNestedArray("items");
        for(auto &item : manager->getSignalCompatibleItems()) // iterate over items
        {
            JsonObject itemObj = itemsArray.createNestedObject();
            itemObj["name"] = item->getName();
            JsonArray signalPathArray = itemObj.createNestedArray("signals");
            for(auto &localPath : item->getLocalSignalNames())
            {
                SignalNameResolver::SignalNameParameters signalParams;
                signalParams.subsystemName = manager->getName();
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
 * @brief this is automaticaly called when any of the registered managers changes (item modification or removal).
 * 
 */
void CentralizedSignalHub::update(ISignalCompatibleService *scm)
{
    notify(); // do nothing and pass it to the observer (SignalRouterService in this case).
}