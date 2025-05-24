#include "EventManager.hpp"
#include <ArduinoJson.h>

EventManager* EventManager::instance = nullptr;

/** 
 * @brief Construct a new EventManager object.
 * 
 */
EventManager::EventManager() 
{
    loadState(); // load the state from the internal storage (SPIFFS)
}

/** 
 * @brief Destroy the EventManager object.
 * 
 */
EventManager::~EventManager() 
{
    saveState(); // save the state before deleting the instance
    delete instance;
}

/** 
 * @brief Get the instance of the EventManager (singleton pattern).
 * 
 * @return EventManager* The instance of the EventManager.
 */
EventManager* EventManager::getInstance() 
{
    if (!instance) {
        instance = new EventManager();
    }
    return instance;
}

/** 
 * @brief Create a new event and add it to the event list.
 * 
 * @param id The ID of the event.
 * @param name The name of the event.
 * @param flag The flag(status) of the event.
 * @param occupied The occupied status of the event.
 */
void EventManager::create(EventItem eventItem) 
{
    eventList.addItem(eventItem);
    previousFlags[eventItem.getId()] = eventItem.getStatus();
    saveState();
    notify();
}

/** 
 * @brief Remove an event from the event list.
 * 
 * @param id The ID of the event to remove.
 */
void EventManager::remove(uint64_t id) 
{
    eventList.deleteItem(id);
    previousFlags.erase(id);
    saveState();
}

/** 
 * @brief Modify an event in the event list.
 * 
 * @param id The ID of the event to modify.
 * @param newItem The new item to replace the old one.
 */
void EventManager::modify(uint64_t id, EventItem newItem) 
{
    eventList.modifyItem(id, newItem);
    previousFlags[id] = newItem.getFlag();
    saveState();
}

/** 
 * @brief Modify the flag of an event in the event list. Broadcasters must only use this method to update the event status (flag).
 * 
 * 
 * @param id The ID of the event to modify.
 * @param flag The new flag of the event.
 */
void EventManager::modifyEventFlag(uint64_t id, bool flag) 
{
    EventItem& item = eventList.getItem(id); // get a reference to the item
    if (item.getId() != -1) // Check if the item exists
    { 
        item.setFlag(flag);
        notify();
        previousFlags[id] = flag;
    }
}

/** 
 * @brief Check if the flag of an event has changed (Listeners must use this method to check if their desired event is triggered or not).
 * 
 * @param id The ID of the event.
 * @param newFlag The new flag of the event.
 * @return true If the flag has changed.
 * @return false If the flag has not changed.
 */
bool EventManager::hasEventFlagChanged(uint64_t id, bool& newFlag) 
{
    auto it = previousFlags.find(id);
    if (it != previousFlags.end()) 
    {
        newFlag = eventList.getItem(id).getFlag();
        auto realFlag = newFlag; // copy flag 
        if(eventList.getItem(id).isInvert())
            newFlag = !newFlag; // invert the flag ref that is returned through the method arg
        return it->second != realFlag;
    }
    return false;
}

/** 
 * @brief Initialize the listeners by triggering a dummy event change (by inverting the previous event status) to notify all the listeners.
 * 
 */
void EventManager::initializeListeners()
{
    for (auto& item : eventList.getList()) 
        previousFlags[item.getId()] = !item.getFlag(); // make flags opposite to trigger all the listeners at startup
    notify();
    for (auto& item : eventList.getList()) 
        previousFlags[item.getId()] = item.getFlag(); // revert to their original state
}

/** 
 * @brief Get the realtime event list in JSON format.
 * 
 * @return std::string The event list in JSON format.
 */
std::string EventManager::getListJson() 
{
    return eventList.toJson();
}

/** 
 * @brief Register a listener to the event manager (same as the "attach" method).
 * @param listener The listener to register.
 */
void EventManager::registerListener(IObserver<EventManager>* listener) 
{
    attach(listener);
}

/**
 * @brief save the state of the event list to the internal storage (SPIFFS).
 * 
 */
void EventManager::saveState() 
{
    std::string jsonList = eventList.toJson();
    Configuration::getInstance()->setEventList(jsonList);
}

/**
 * @brief Load the state of the event list from the internal storage (SPIFFS).
 * 
 */
void EventManager::loadState() 
{
    std::string state = Configuration::getInstance()->getEventList();
    if (state.empty()) return;

    eventList.repopulateWith(state);
    for (auto& item : eventList.getList()) // Save the flags to the previousFlags map
        previousFlags[item.getId()] = item.getFlag();
}

/**
 * @brief Loop through the event list and check if any events are triggered.
 * 
 */
void EventManager::loop() 
{
    for (auto& item : eventList.getList()) 
    {
        bool currentFlag = item.getFlag();
        if(item.getEventId() != -1) // if mated to another event
            eventList.getItem(item.getId()).setFlag(eventList.getItem(item.getEventId()).getFlag()); // link (mate) to another event status

        if (currentFlag != previousFlags[item.getId()]) // Check if the flag has changed
        {
            previousFlags[item.getId()] = currentFlag;
            // Notify all listeners
            notify();
        }
    }
}
