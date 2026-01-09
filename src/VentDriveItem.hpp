#pragma once

#include "FusionBusItem.hpp"
#include "ILoggableItem.hpp"
#include <string>

class VentDriveItem : public FusionBusItem
{
public:
    VentDriveItem();
    enum class State
    {
        Unknown,
        Closing,
        Opening,
        Idle,
        Error,
        Uninitialized
    };
    
    virtual std::vector<std::string> getLocalSignalNames() override;

    double getLength() const;
    double getStepPermm() const;
    double getSpeed() const;
    double getMaxCompensation() const;
    double getAcceleration() const;
    double getEndstopMinDistance() const;
    State getCurrentState() const;
    int getVentingPercent() const;
    
    void setCurrentState(State state);
    void setCurrentVentingPercent(std::optional<int> currentVentingPercent);
    void setVentingPercent(int ventingPercent);

private:
    int ventingPercent;
    std::optional<int> currentVentingPercent;
    double length, stepPermm, speed, maxCompensation, acceleration, endstopMinDistance;
    State currentState;
};
