#include "CentralizedSignalHub.hpp"
#include "ArduinoJson.h"
#include "SignalNameResolver.hpp"

/**
 * @brief register a signal compatible subsystem (manager) 
 * @param manager a refrence of the desired manager to be added to the manager registry
 */
void CentralizedSignalHub::registerManager(ISignalCompatibleManager &manager)
{
    managers.push_back(manager);
}

/**
 * @brief get manager registry
 * @return a list (vector) of managers
 */
std::vector<ISignalCompatibleManager &> CentralizedSignalHub::getManagers()
{
    return managers;
}

/**
 * @brief get a complete list of signal-compatible items
 * @return json list of signal-compatible items including their local signals
 */
std::string CentralizedSignalHub::getListJson()
{
    JsonDocument doc;
    JsonArray managersArray = doc.to<JsonArray>();
    for(auto &manager : managers) // iterate over managers
    {
        JsonObject managerObj = managersArray.createNestedObject();
        managerObj["name"] = manager.getName();
        JsonArray itemsArray = managerObj.createNestedArray("items");
        for(auto &item : manager.getSignalCompatibleItems()) // iterate over items
        {
            JsonObject itemObj = itemsArray.createNestedObject();
            itemObj["name"] = item.getName();
            JsonArray signalPathArray = itemObj.createNestedArray("signals");
            for(auto &localPath : item.getLocalSignalNames())
            {
                SignalNameResolver::SignalNameParameters signalParams;
                signalParams.subsystemName = manager.getName();
                signalParams.id = item.getId();
                signalParams.localSignalName = localPath;
                signalPathArray.add(SignalNameResolver::toString(signalParams));
            }
        }
    }
    std::string serializedJson;
    serializeJson(doc, serializedJson);
    return serializedJson;
}