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
        item.setFlag(flag);
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
    auto &item = eventList.getItem(id);
    if(item.getId() != -1) // if item exist
    {
        return (getEventFlag(item.getId()) != item.getPrevState());
    }
    return false;
}

/** 
 * @brief Get the flag (status) of an event.
 * 
 * @param id The ID of the event.
 * @return true If the event is triggered.
 * @return false If the event is not triggered.
 */
bool EventManager::getEventFlag(uint64_t id)
{
    auto item = eventList.getItem(id); // get a copy of desired item
    if(item.getId() == id) // check if the event ID is found
    {
        auto logic = item.getLogic();
        auto flag = item.getFlag(); // self is the default case
        auto pairedEventExist = (eventList.getItem(item.getEventId()).getId() != -1);
        //Serial.printf("Info: event_%d: logic=%s, pEvent=%d", item.getId(), logic, pairedEventExist);
        if(logic == "self")
        {
            flag = flag;
            Serial.print("self for ---- event:");
            Serial.print(item.getId());
            Serial.print(" flag:");
            Serial.println(flag);
        }
        else if(logic == "selfInverted")
            flag = !flag;
        else if(pairedEventExist)
        {
            if(logic == "pairedOnly")
                flag = getEventFlag(item.getEventId());
            else if(logic == "pairedInverted")
                flag = getEventFlag(item.getEventId());
            else if(logic == "andWith")
            {
                flag = (flag and getEventFlag(item.getEventId()));
                Serial.print("andWith for ---- event:");
                Serial.print(item.getId());
                Serial.print(" flag:");
                Serial.println(flag);
            }
            else if(logic == "orWith")
                flag = (flag or getEventFlag(item.getEventId()));
            else if(logic == "nandWith")
                flag = !(flag and getEventFlag(item.getEventId()));
            else if(logic == "norWith")
                flag = !(flag or getEventFlag(item.getEventId()));
            else
                Serial.println("Error: unsupported logic operation");
        }
        else
            Serial.println("Error: unsupported logic operation");
        return flag; // return the flag (status) of the event
    }
    return false; // Return false if the event ID is not found
}

/** 
 * @brief Initialize the listeners by triggering a dummy event change (by inverting the previous event status) to notify all the listeners.
 * 
 */
void EventManager::initializeListeners()
{
    for (auto& item : eventList.getList()) 
        item.setPrevState(!item.getFlag()); // make flags opposite to trigger all the listeners at startup
    notify();
    for (auto& item : eventList.getList()) 
        item.setPrevState(item.getFlag()); // revert to their original state
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
}

/**
 * @brief Loop through the event list and check if any events are triggered.
 * 
 */
void EventManager::loop() 
{
    for (auto& item : eventList.getList()) 
    {
        bool currentFlag = getEventFlag(item.getId());
        if (currentFlag != item.getPrevState()) // Check if the flag has changed
        {
            // Notify all listeners
            notify();
            eventList.getItem(item.getId()).setPrevState(currentFlag);
        }
    }
}
