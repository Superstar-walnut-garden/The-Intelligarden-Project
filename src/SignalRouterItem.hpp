#pragma once

#include <ArduinoJson.h>
#include <string>
#include <vector>
#include "SignalCompatibleBaseItem.hpp"

class SignalRouterItem: public SignalCompatibleBaseItem
{
public: 
    class SignalEndpoint; // prototype declaration
    enum class Mode; // prototype declaration
    SignalRouterItem();
    std::vector<SignalEndpoint> getListeners();
    SignalEndpoint getBroadcaster();
    SignalEndpoint getAuxiliaryBroadcaster();
    static std::string getEmittedSignalLocalSignalName();
    Mode getMode();

    void setBroadcasterStatus(bool value);
    void setAuxiliaryBroadcasterStatus(bool value);
    void removeListener(std::string signalPath);
    void removeBroadcaster(bool removeAuxiliary = false);

    class SignalEndpoint // nested class for signal endpoints
    {
        public:
        // constructors
        SignalEndpoint() : signalPath(""), status(false), inverted(false) {}
        SignalEndpoint(std::string signalPath, bool inverted) : signalPath(signalPath), status(false), inverted(inverted) {}

        // getters and setters
        std::string getSignalPath() { return signalPath; }
        bool isInverted() { return inverted; }
        bool getStatus() { return isInverted() ? !status : status; }
        void setStatus(bool value) { status = value; }

        private:
        std::string signalPath;
        bool status, inverted;
    };
    enum class Mode // Signal Routing Mode
    {
        SingleSource, 
        AndWithAuxiliary,
        OrWithAuxiliary
    };

private:
    void populateDerivedClassFromJson(JsonDocument &doc) override;
    void derivedClassToJson(JsonDocument &doc) override;
    std::vector<std::string> getLocalSignalNames() override;
    void evaluateStatus();
    using SignalCompatibleBaseItem::setStatus; // hide the base class setStatus method

    std::vector<SignalEndpoint> listeners;
    SignalEndpoint broadcaster, auxiliaryBroadcaster;
    Mode mode;
};
