#include "EventManager.hpp"
#include <ArduinoJson.h>

EventManager* EventManager::instance = nullptr;

EventManager::EventManager() {
    loadState();
}

EventManager::~EventManager() {
    saveState();
}

EventManager* EventManager::getInstance() {
    if (!instance) {
        instance = new EventManager();
    }
    return instance;
}

void EventManager::createEvent(int id, const std::string& name, bool flag, bool occupied) {
    EventItem newItem(id, name, flag, occupied);
    eventList.addItem(newItem);
    previousFlags[id] = flag;
    notify();
}

void EventManager::removeEvent(int id) {
    eventList.deleteItem(id);
    previousFlags.erase(id);
    notify();
}

void EventManager::modifyEvent(int id, const EventItem& newItem) {
    eventList.modifyItem(id, newItem);
    previousFlags[id] = newItem.getFlag();
    notify();
}

void EventManager::modifyEventFlag(int id, bool flag) {
    EventItem& item = eventList.getItem(id);
    if (item.getId() != -1) { // Check if the item exists
        item.setFlag(flag);
        previousFlags[id] = flag;
        notify();
    }
}

std::string EventManager::getEventListJson() {
    return eventList.getListJson();
}

void EventManager::registerListener(IObserver<EventManager>* listener) {
    attach(listener);
}

void EventManager::registerBroadcaster(int eventId, std::function<void(bool)> broadcaster) {
    if (broadcaster) {
        broadcasters[eventId] = broadcaster;
    } else {
        broadcasters.erase(eventId);
    }
}

bool EventManager::hasEventFlagChanged(int id, bool& newFlag) {
    auto it = previousFlags.find(id);
    if (it != previousFlags.end()) {
        newFlag = eventList.getItem(id).getFlag();
        return it->second != newFlag;
    }
    return false;
}

void EventManager::saveState() {
    DynamicJsonDocument doc(2048);
    JsonArray events = doc.createNestedArray("events");

    for (const auto& item : eventList.getList()) {
        JsonObject event = events.createNestedObject();
        event["id"] = item.getId();
        event["name"] = item.getName();
        event["flag"] = item.getFlag();
        event["occupied"] = item.isOccupied();
    }

    JsonObject broadcastersJson = doc.createNestedObject("broadcasters");
    for (const auto& broadcaster : broadcasters) {
        broadcastersJson[String(broadcaster.first)] = true; // Just a placeholder to indicate the broadcaster is registered
    }

    std::string output;
    serializeJson(doc, output);
    Configuration::getInstance()->setEventList(output);
}

void EventManager::loadState() {
    std::string state = Configuration::getInstance()->getEventList();
    if (state.empty()) return;

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, state);

    JsonArray events = doc["events"];
    for (JsonObject event : events) {
        int id = event["id"];
        std::string name = event["name"].as<std::string>();
        bool flag = event["flag"];
        bool occupied = event["occupied"];
        createEvent(id, name, flag, occupied);
    }

    JsonObject broadcastersJson = doc["broadcasters"];
    for (JsonPair kv : broadcastersJson) {
        int id = atoi(kv.key().c_str());
        registerBroadcaster(id, nullptr); // Placeholder, actual function should be set dynamically
    }
}

void EventManager::loop() {
    for (auto& item : eventList.getList()) {
        bool currentFlag = item.getFlag();
        // Check if the flag has changed
        if (currentFlag != previousFlags[item.getId()]) {
            previousFlags[item.getId()] = currentFlag;
            // Notify all listeners
            notify();
        }
    }
}