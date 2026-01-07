#pragma once

#include "FusionBusItem.hpp"
#include "ILoggableItem.hpp"
#include <string>

class VentDriveItem : public FusionBusItem
{
public:
    VentDriveItem();
    
    virtual std::vector<std::string> getLocalSignalNames() override;

private:
    int ventingPercent;
    double length;
    double stepPermm;
};
