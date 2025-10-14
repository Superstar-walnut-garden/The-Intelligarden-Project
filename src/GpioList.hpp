#pragma once

#include "GpioItem.hpp"
#include "BaseList.hpp"
#include <string>

class GpioList : public BaseList<GpioItem>
{
public:
    GpioList();
    GpioList(std::string json);
};
