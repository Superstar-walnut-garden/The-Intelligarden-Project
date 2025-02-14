#ifndef EVENTLIST_HPP
#define EVENTLIST_HPP

#include "EventItem.hpp"
#include "BaseList.hpp"
#include <ArduinoJson.h>
#include <string>
#include <vector>
#include <iostream>

/**
 * @brief EventList class
 * 
 */
class EventList: public BaseList<EventItem>
{
public:
    EventList();
    EventList(std::string json);

    std::string toJson() override;
    void repopulateWith(std::string json) override;
    void printList() override;

private:
};

#endif // EVENTLIST_HPP