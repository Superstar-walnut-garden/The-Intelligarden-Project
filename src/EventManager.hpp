#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include "EventList.hpp"
#include "Subject.hpp"
#include "Configuration.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
#include "IManager.hpp"

class EventManager : public IManager<EventItem>, public Subject<EventManager> {
public:
    static EventManager* getInstance();

    void create(EventItem eventItem) override;
    void remove(uint64_t id) override;
    void modify(uint64_t id, EventItem newItem) override;
    void modifyEventFlag(uint64_t id, bool flag);
    std::string getListJson() override;

    void registerListener(IObserver<EventManager>* listener);
    // void registerBroadcaster(int eventId, std::function<void(bool)> broadcaster);

    bool hasEventFlagChanged(uint64_t id, bool& newFlag);
    bool getEventFlag(uint64_t id);
    void initializeListeners();

    void saveState() override;
    void loadState() override;

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