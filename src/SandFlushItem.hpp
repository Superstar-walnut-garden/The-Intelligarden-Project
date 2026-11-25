#pragma once

#include "SignalCompatibleBaseItem.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "ILoggableItem.hpp"

class SandFlushItem : public SignalCompatibleBaseItem, public ILoggableItem
{
public:
    enum class OperatingState {Idle, Filtering, Backwashing};
    enum class TriggerMode {DP, Interval, Hybrid};

    SandFlushItem();

    double getValveDelay() const;
    short getParallelFilterCount() const;
    int getMinFlushInterval() const;
    int getFlushInterval() const;
    int getFlushDuration() const;
    int getDwellTime() const;
    double getDpSetpoint() const;
    double getDp() const;
    bool isSwitchSequence() const;
    bool isFlushSequenceReversed() const;
    OperatingState getOperatingState() const;
    short getBackwashingFilterNumber() const;
    int getRemainingBackwashTime() const;
    short getAutoBackwashRetryIn() const;
    bool isAutoBackwashInterrupted() const;
    bool isBackwashTriggeredManually() const;
    bool isManualFlush();
    bool isCancelFlush();
    long int getFiltrationElapsedTimeSinceBackwash() const;
    long int getFiltrationElapsedTimeUntilCurrentSession() const;
    long int getStartupDelayRemaining() const;
    long int getStartupDelay() const;
    TriggerMode getTriggerMode() const;
    uint64_t getPressureSensorId() const;

    void setDp(double dp);
    void setFlushSequenceReversed(bool reversed);
    void toggleFlushSequenceReversed();
    void setOperatingState(OperatingState operatingState);
    void setBackwashingFilterNumber(short filterNumber);
    void setRemainingBackwashTime(int remainingBackwashTime);
    void setAutoBackwashRetryIn(short retryIn);
    void setAutoBackwashInterrupted(bool interrupted);
    void setBackwashTriggeredManually(bool triggeredManually);
    void setFiltrationElapsedTimeSinceBackwash(long int filtrationElapsedTimeSinceBackwash);
    void setFiltrationElapsedTimeUntilCurrentSession(long int filtrationElapsedTimeUntilCurrentSession);
    void setStartupDelayRemaining(long int startupDelayRemaining);

    std::string getFiltrationSystemRunningSignalName() const;
    std::vector<std::string> getFilterBackwashSignalNames() const;
    std::vector<std::string> getLocalSignalNames() override;

    // ILoggableItem interface (should be implemented properly later)
    std::string getName() const override { return BaseItem::getName(); }
    uint64_t getId() const override { return BaseItem::getId(); }
    uint64_t getInterval() const override { return 0; }
    bool logOnlyOnChange() const override { return true; }
    std::string getData() const override { return "";};
    bool isLoggingEnabled() const override { return false;};

    std::chrono::steady_clock::time_point backwashStartTime;
    std::chrono::steady_clock::time_point filtrationStartTime;
    std::chrono::steady_clock::time_point idleToFiltrationTransitionTimeStamp;
    std::optional<std::chrono::steady_clock::time_point> autoBackwashCancelTime = std::nullopt;

private:
    double valveDelay;
    short parallelFilterCount;
    int minFlushInterval;
    int flushInterval;
    int flushDuration;
    int dwellTime;
    double dpSetpoint;
    double dp;
    bool switchSequence;
    bool flushSequenceReversed;
    OperatingState operatingState;
    short backwashingFilterNumber;
    int remainingBackwashTime; // seconds
    bool manualFlush;
    bool cancelFlush;
    bool backwashTriggeredManually;
    short autoBackwashRetryIn; // minutes
    bool autoBackwashInterrupted;
    long int filtrationElapsedTimeSinceBackwash; // minutes
    long int filtrationElapsedTimeUntilCurrentSession; // minutes
    long int startupDelayRemaining; // seconds
    long int startupDelay; // seconds
    TriggerMode triggerMode;
    uint64_t pressureSensorId;
};
