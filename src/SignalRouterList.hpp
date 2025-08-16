#pragma once

#include "SignalRouterItem.hpp"
#include "BaseList.hpp"
#include <ArduinoJson.h>
#include <string>
#include <vector>
#include <iostream>

/**
 * @brief EventList class
 * 
 */
class SignalRouterList: public BaseList<SignalRouterItem>
{
public:
    SignalRouterList();
    SignalRouterList(std::string json);

private:
};
