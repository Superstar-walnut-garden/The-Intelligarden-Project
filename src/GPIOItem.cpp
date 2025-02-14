#include "GPIOItem.hpp"

GPIOItem::GPIOItem()
    : BaseItem(), mode(0), extraParameters("")
{
}

GPIOItem::GPIOItem(int pin, int event_id, std::string name, bool status, short mode, std::string extraParameters)
    : BaseItem(pin, event_id, name, status), mode(mode), extraParameters(extraParameters)
{
}

int GPIOItem::getPin()
{
    return this->getId();
}

int GPIOItem::getMode()
{
    return this->mode;
}

void GPIOItem::setMode(short mode)
{
    this->mode = mode;
}

std::string GPIOItem::getExtraParameters()
{
    return this->extraParameters;
}

void GPIOItem::setExtraParameters(std::string extraParameters)
{
    this->extraParameters = extraParameters;
}