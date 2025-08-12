#ifndef SIGNALLIST_HPP
#define SIGNALLIST_HPP

#include "SignalItem.hpp"
#include "BaseList.hpp"
#include <ArduinoJson.h>
#include <string>
#include <vector>
#include <iostream>

/**
 * @brief EventList class
 * 
 */
class SignalList: public BaseList<SignalItem>
{
public:
    SignalList();
    SignalList(std::string json);

private:
};

#endif // EVENTLIST_HPP