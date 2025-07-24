#ifndef SIGNALITEM_HPP
#define SIGNALITEM_HPP
#include <ArduinoJson.h>
#include <string>
#include <vector>
#include "BaseItem.hpp"

class SignalItem: public BaseItem
{
public: 
    class SignalEndpoint; // prototype declaration
    SignalItem();
    std::vector<SignalEndpoint> getListeners();
    SignalEndpoint getBroadcaster();

    class SignalEndpoint // nested class for signal endpoints
    {
        public:
        SignalEndpoint() : signalPath(""), inverted(false) {}
        SignalEndpoint(std::string signalPath, bool inverted) : signalPath(signalPath), inverted(inverted) {}
        std::string getSignalPath() { return signalPath; }
        bool isInverted() { return inverted; }

        private:
        std::string signalPath;
        bool inverted;
    };

private:
    void populateDerivedClassFromJson(JsonDocument &doc) override;
    void derivedClassToJson(JsonDocument &doc) override;

    std::vector<SignalEndpoint> listeners;
    SignalEndpoint broadcaster;

};

#endif