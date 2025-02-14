#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include "EventList.hpp"
#include "Subject.hpp"
#include "Configuration.hpp"
#include <functional>
#include <unordered_map>
#include <vector>

class EventManager : public Subject<EventManager> {
public:
    static EventManager* getInstance();

    void createEvent(int id, std::string& name, bool flag, bool occupied);
    void removeEvent(int id);
    void modifyEvent(int id, EventItem& newItem);
    void modifyEventFlag(int id, bool flag);
    std::string getEventListJson();

    void registerListener(IObserver<EventManager>* listener);
    // void registerBroadcaster(int eventId, std::function<void(bool)> broadcaster);

    bool hasEventFlagChanged(int id, bool& newFlag);

    void saveState();
    void loadState();

    void loop();

private:
    EventManager();
    ~EventManager();
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    static EventManager* instance;
    EventList eventList;
    std::unordered_map<int, bool> previousFlags;
};

#endif // EVENTMANAGER_HPP