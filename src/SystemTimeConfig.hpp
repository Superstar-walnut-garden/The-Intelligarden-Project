#ifndef TIMECONFIGDATA_HPP
#define TIMECONFIGDATA_HPP
#include <ArduinoJson.h>
#include <string>
#include <iostream>
#include "IJsonSerializable.hpp"

/**
 * @brief data structure for storing time configuration data.
 * 
 */
class SystemTimeConfig: public IJsonSerializable
{
    public:
    SystemTimeConfig();
    SystemTimeConfig(std::string json);
    void populateFromJson(std::string json) override;
    std::string toJson() override;

    std::string getTimezone();
    void setTimezone(std::string& tz);
    std::string getNtpServer();
    void setNtpServer(std::string& server);
    bool isTimeSubsystemInitialized();
    void setTimeSubsystemInitialized(bool initialized);
    bool isExternalRTCAvailable();
    void setExternalRTCAvailability(bool available);
    bool isSetTimeAutomatically();
    void setSetTimeAutomatically(bool setAutomatically);
    bool isNtpUpdated();
    void setNtpUpdated(bool updated);
    bool isManualTimeSetFlag();
    void setManualTimeSetFlag(bool flag);
    unsigned long getManualTimeEpoch();
    void setManualTimeEpoch(unsigned long epoch);

    private:
    std::string timezone; // e.g. "Europe/Berlin"
    std::string ntpServer; // e.g. "pool.ntp.org"
    bool timeSubsystemInitialized; // true if time subsystem initialized successfully, false otherwise
    bool externalRTCAvailable; // true if external RTC is enabled, false otherwise
    bool setTimeAutomatically; // true if time should be set automatically, false otherwise
    bool ntpUpdated; // true if NTP time is updated, false otherwise
    bool manualTimeSetFlag; // true temporarily by web api to set time manually, false otherwise
    unsigned long manualTimeEpoch; // epoch time in seconds, used to set time manually
};

#endif