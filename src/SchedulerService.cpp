#include "SchedulerService.hpp"
#include "Configuration.hpp"
#include <ArduinoJson.h>
#include "SignalRouterService.hpp"
#include "SignalNameResolver.hpp"

/**
 * @brief Singleton instance of the SchedulerService class.
 * 
 */
SchedulerService* SchedulerService::instance = nullptr;

/**
 * @brief Construct a new SchedulerService object.
 * 
 */
SchedulerService::SchedulerService()
{
    restoreAll();
}

/**
 * @brief Destroy the SchedulerService object.
 * 
 */
SchedulerService::~SchedulerService() 
{
    storeAll();
}

/**
 * @brief Get the instance of the SchedulerService (singleton pattern).
 * 
 * @return SchedulerService* The instance of the SchedulerService.
 */
SchedulerService* SchedulerService::getInstance() 
{
    if (!instance) 
    {
        instance = new SchedulerService();
    }
    return instance;
}

/**
 * @brief determine the status of items based on the current time and weekday.
 * 
 */
void SchedulerService::determineStatusofItems() 
{
    auto systemTime = SystemTimeService::getInstance(); // get direct access to system time
    auto currentTime = systemTime->getTime();
    for (auto& item : list.getList()) // iterate over the list of items
    {
        auto setItemStatus = [this](SchedulerItem& item, bool status) // lambda to set the status of item
        { 
            auto skipSignal = SignalRouterService::getInstance()->
                getSignalValue(SignalNameResolver::toString(
                    SignalNameResolver::SignalNameParameters(this->getName(), item.getId(), item.getSkipLocalSignalName())));
            if(!status) // if the item is off, cancell the skip
                item.setSkipped(false);
            else if(skipSignal.has_value()) // if the item is on, and associated with a signal.
                item.setSkipped(skipSignal.value()); // skip the item if the signal value is "true".
            
            item.setStatus(status and !item.isSkipped()); 
            Serial.println(("item" + std::to_string(item.getId()) + ": is" + std::to_string(item.getStatus())).c_str());
        };
        auto& itemRef = list.getItem(item.getId());
        if(item.getMode() == "weekly")
        {
            auto untilTime = item.getStartTime() + item.getDuration();
            auto isCurrentTimeBetweenStartAndEnd = currentTime >= item.getStartTime() && currentTime <= untilTime;
            auto newStatus = (systemTime->isCurrentWeekdayPresentIn(SystemTimeService::parseWeekday(item.getWeekday())) and isCurrentTimeBetweenStartAndEnd); // check weekday and time
            setItemStatus(itemRef, newStatus);
            broadcastItem(itemRef); // broadcast the real item (because only the reference gets updated in this "for" statement)
        }
        else if(item.getMode() == "hourly")
        {
            auto interval = item.getStartTime().getTimeInMinutes(), duration = item.getDuration().getTimeInMinutes();
            auto ctime = currentTime.getTimeInMinutes(); // current time in minutes
            int cyclePosition = 0;
            if(interval != 0) // prevent division by zero
                cyclePosition = ctime % interval;
            
            auto newStatus = (cyclePosition < duration);
            setItemStatus(itemRef, newStatus);
            broadcastItem(itemRef);
        }
        else
        {
            Serial.println("Unsupported mode");
        }
    }
}

/**
 * @brief Create a new SchedulerItem and add it to the list.
 * 
 * @param schedulerItem The SchedulerItem to be created.
 */
void SchedulerService::create(SchedulerItem schedulerItem) 
{
    list.addItem(schedulerItem);
    Serial.printf("Schedule %d created\n", schedulerItem.getId());
    storeAll();
}

/**
 * @brief Remove a SchedulerItem from the list by its ID.
 * 
 * @param id The ID of the SchedulerItem to be removed.
 */
void SchedulerService::remove(uint64_t id) 
{
    list.deleteItem(id);
    notify();
    storeAll();
}

/**
 * @brief update an existing SchedulerItem in the list.
 * 
 * @param id The ID of the SchedulerItem to be modified.
 * @param newItem The new SchedulerItem with updated values.
 */
void SchedulerService::update(uint64_t id, SchedulerItem newItem) 
{
    list.modifyItem(id, newItem);
    notify();
    storeAll();
}

/**
 * @brief Save the current state of the SchedulerService to the internal storage (SPIFFS).
 * 
 */
void SchedulerService::storeAll()
{
    Configuration::getInstance()->setSchedulerList(list.toJson());
}

/**
 * @brief Load the state of the SchedulerService from the internal storage (SPIFFS).
 * 
 */
void SchedulerService::restoreAll() 
{
    auto data = Configuration::getInstance()->getSchedulerList();
    if (data.empty()) return;
        list.repopulateWith(data);
}

/**
 * @brief Get the JSON representation of the SchedulerService list (for web-api).
 * 
 * @return std::string The SchedulerService list in JSON format.
 */
std::string SchedulerService::getAll()
{
    return list.toJson();
}

/**
 * @brief Get an item from JSON representation of the SchedulerService list (for web-api).
 * 
 * @return std::string an item in JSON format.
 */
std::string SchedulerService::get(uint64_t id)
{
    return list.getItem(id).toJson();
}

/**
 * @brief Get the list of signal-compatible items in the SchedulerService (for the CentralizedSignalHub).
 * 
 * @return std::vector<ISignalCompatibleItem*> A vector of pointers to signal-compatible items.
 */
std::vector<ISignalCompatibleItem *> SchedulerService::getSignalCompatibleItems()
{
    std::vector<ISignalCompatibleItem *> items;
    for (auto& item : list.getList())
    {
        items.push_back(&list.getItem(item.getId())); // pushback real reference of the item, not a copy
        // Note: This assumes that the list's items are not going to be deleted or modified in a way that invalidates the pointers.
        // If they are, you may need to handle that case to avoid dangling pointers.
    }
    return items;
}

/**
 * @brief The main loop of the SchedulerService, call periodically to update the status of items.
 * 
 */
void SchedulerService::loop()
{
    determineStatusofItems(); // determine the status of items based on the current time and weekday
}

/**
 * @brief Broadcast the status of a SchedulerItem to SignalRouterService
 * 
 * @param item The SchedulerItem to be broadcasted.
 */
void SchedulerService::broadcastItem(SchedulerItem &item)
{
    SignalRouterService::getInstance()->setSignalValue(SignalNameResolver::toString(
        SignalNameResolver::SignalNameParameters(
            this->getName(), 
            item.getId(), 
            item.getMainLocalSignalName()
        )), item.getStatus());
}