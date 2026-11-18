/*
 * SandFlushItem.cpp
 *
 *  Created on: Mar 30, 2024
 * 
 */
#include "SandFlushItem.hpp"
#include "SignalNameResolver.hpp"
#include "../EnumCrafter.hpp"

/**
 * @brief Construct a new SandFlushItem and initialize its JSON serialization hooks.
 *
 * The constructor sets all configuration defaults to zero/false values and
 * registers callback handlers for reading and writing configuration data.
 */
SandFlushItem::SandFlushItem()
    : SignalCompatibleBaseItem(),
      valveDelay(0),
      parallelFilterCount(0),
      minFlushInterval(0),
      flushInterval(0),
      flushDuration(0),
      dwellTime(0),
      dpSetpoint(0),
      dp(0),
      switchSequence(false),
      flushSequenceReversed(false),
      operatingState{},
      backwashingFilterNumber(0),
      remainingBackwashTime(0),
      manualFlush(false),
      cancelFlush(false),
      filtrationTimeSinceBackwash(0),
      triggerMode{},
      pressureSensorId(0)
{
    registerToJsonCallback([this](JsonDocument &json) -> void
    {
        json["valveDelay"] = this->valveDelay;
        json["parallelFilterCount"] = this->parallelFilterCount;
        json["minFlushInterval"] = this->minFlushInterval;
        json["flushInterval"] = this->flushInterval;
        json["flushDuration"] = this->flushDuration;
        json["dwellTime"] = this->dwellTime;
        json["dpSetpoint"] = this->dpSetpoint;
        json["switchSequence"] = this->switchSequence;
        json["triggerMode"] = EnumCrafter::toString(this->triggerMode);
        json["pressureSensorId"] = std::to_string(this->pressureSensorId);

        //runtime live parameters
        json["dp"] = this->dp;
        json["operatingState"] = EnumCrafter::toString(this->operatingState);
        json["remainingBackwashTime"] = this->remainingBackwashTime;
        json["flushSequenceReversed"] = this->flushSequenceReversed;
        json["backwashingFilterNumber"] = this->backwashingFilterNumber;
        json["filtrationTimeSinceBackwash"] = this->filtrationTimeSinceBackwash;

        // Enforce command-type parameters to stay false
        json["manualFlush"] = false;
        json["cancelFlush"] = false;
    });
    registerFromJsonCallback([this](JsonDocument &json) -> void
    {
        this->valveDelay = json["valveDelay"].as<double>();
        this->parallelFilterCount = json["parallelFilterCount"].as<short>();
        this->minFlushInterval = json["minFlushInterval"].as<int>();
        this->flushInterval = json["flushInterval"].as<int>();
        this->flushDuration = json["flushDuration"].as<int>();
        this->dwellTime = json["dwellTime"].as<int>();
        this->dpSetpoint = json["dpSetpoint"].as<double>();
        this->switchSequence = json["switchSequence"].as<bool>();
        this->triggerMode = static_cast<TriggerMode>(json["triggerMode"].as<int>());
        this->pressureSensorId = std::stoull(json["pressureSensorId"].as<std::string>());

        // runtime live parameters (do nothing with them!!!)
        this->dp;
        this->flushSequenceReversed;
        this->operatingState;
        this->backwashingFilterNumber;
        this->remainingBackwashTime;
        this->filtrationTimeSinceBackwash; // maybe later save and load this separately to EEPROM or SPIFFS to keep track of filtration time since last backwash even after power loss, but for now it is just a runtime parameter

        // commands
        if((json["manualFlush"].as<bool>() | false)) // if manual flush is true
            this->manualFlush = true; // raise manual flush

        if((json["cancelFlush"].as<bool>() | false)) // if cancel flush is true
            this->cancelFlush = true; // raise cancel flush
    });
}

/**
 * @brief Get the configured valve delay.
 *
 * @return double The valve delay in seconds.
 */
double SandFlushItem::getValveDelay() const
{
    return this->valveDelay;
}

/**
 * @brief Get the number of filters operating in parallel.
 *
 * @return short The configured parallel filter count.
 */
short SandFlushItem::getParallelFilterCount() const
{
    return this->parallelFilterCount;
}

/**
 * @brief Get the minimum interval allowed between flush cycles.
 *
 * @return int The minimum flush interval.
 */
int SandFlushItem::getMinFlushInterval() const
{
    return this->minFlushInterval;
}

/**
 * @brief Get the configured flush interval.
 *
 * @return int The flush interval value.
 */
int SandFlushItem::getFlushInterval() const
{
    return this->flushInterval;
}

/**
 * @brief Get the configured flush duration.
 *
 * @return int The flush duration.
 */
int SandFlushItem::getFlushDuration() const
{
    return this->flushDuration;
}

/**
 * @brief Get the configured dwell time between sequence steps.
 *
 * @return int The dwell time.
 */
int SandFlushItem::getDwellTime() const
{
    return this->dwellTime;
}

/**
 * @brief Get the differential-pressure setpoint used for activation.
 *
 * @return double The setpoint value.
 */
double SandFlushItem::getDpSetpoint() const
{
    return this->dpSetpoint;
}

/**
 * @brief Get the current measured differential pressure.
 *
 * @return double The live differential pressure value.
 */
double SandFlushItem::getDp() const
{
    return this->dp;
}

/**
 * @brief Check whether the flush sequence alternates between filters.
 *
 * @return true if the switch sequence is enabled, otherwise false.
 */
bool SandFlushItem::isSwitchSequence() const
{
    return this->switchSequence;
}

/**
 * @brief Check whether the flush sequence is running in reverse order.
 *
 * @return true if the sequence is reversed, otherwise false.
 */
bool SandFlushItem::isFlushSequenceReversed() const
{
    return this->flushSequenceReversed;
}

/**
 * @brief Get the current operating state of the filter controller.
 *
 * @return OperatingState The current operating state.
 */
SandFlushItem::OperatingState SandFlushItem::getOperatingState() const
{
    return this->operatingState;
}

/**
 * @brief Get the filter currently being backwashed.
 *
 * @return short The active backwashing filter number.
 */
short SandFlushItem::getBackwashingFilterNumber() const
{
    return this->backwashingFilterNumber;
}

/**
 * @brief Get the remaining time (seconds) left in the backwash phase.
 *
 * @return int The remaining backwash time.
 */
int SandFlushItem::getRemainingBackwashTime() const
{
    return this->remainingBackwashTime;
}

/**
 * @brief Consume a queued manual flush request.
 *
 * The method reads the pending flag and clears it so the request is only acted
 * on once.
 *
 * @return true if a manual flush request was pending, otherwise false.
 */
bool SandFlushItem::isManualFlush()
{
    if(this->manualFlush)
    {
        this->manualFlush = false; // reset manual flush after reading it
        return true;
    }
    return false;
}

/**
 * @brief Consume a queued cancel-flush request.
 *
 * The method reads the pending cancel signal and clears it to avoid repeated
 * processing.
 *
 * @return true if a cancel request was pending, otherwise false.
 */
bool SandFlushItem::isCancelFlush()
{
    if(this->cancelFlush)
    {
        this->cancelFlush = false; // reset cancel flush after reading it
        return true;
    }
    return false;
}

/**
 * @brief Get the elapsed filtration time since the last backwash event.
 *
 * @return long int The filtration time (minutes) since the last backwash.
 */
long int SandFlushItem::getFiltrationTimeSinceBackwash() const
{
    return this->filtrationTimeSinceBackwash;
}

/**
 * @brief Get the trigger mode that starts the flush sequence.
 *
 * @return TriggerMode The configured trigger mode.
 */
SandFlushItem::TriggerMode SandFlushItem::getTriggerMode() const
{
    return this->triggerMode;
}

/**
 * @brief Get the identifier of the pressure sensor associated with this item.
 *
 * @return uint64_t The pressure sensor ID.
 */
uint64_t SandFlushItem::getPressureSensorId() const
{
    return this->pressureSensorId;
}

/**
 * @brief Set the current measured differential pressure.
 *
 * @param dp The measured differential pressure value.
 */
void SandFlushItem::setDp(double dp)
{
    this->dp = dp;
}

/**
 * @brief Set whether the flush sequence runs in reverse order.
 *
 * @param reversed true to reverse the flush sequence, false for normal order.
 */
void SandFlushItem::setFlushSequenceReversed(bool reversed)
{
    this->flushSequenceReversed = reversed;
}

/**
 * @brief Toggle whether the flush sequence runs in reverse order.
 */
void SandFlushItem::toggleFlushSequenceReversed()
{
    this->flushSequenceReversed = !this->flushSequenceReversed;
}

/**
 * @brief Set the current operating state of the sand-flush controller.
 *
 * @param operatingState The new operating state.
 */
void SandFlushItem::setOperatingState(OperatingState operatingState)
{
    this->operatingState = operatingState;
}

/**
 * @brief Set the filter currently being backwashed.
 *
 * @param filterNumber The backwashing filter number.
 */
void SandFlushItem::setBackwashingFilterNumber(short filterNumber)
{
    this->backwashingFilterNumber = filterNumber;
}

/**
 * @brief Set the remaining time (seconds) left in the backwash phase.
 *
 * @param remainingBackwashTime The remaining backwash time.
 */
void SandFlushItem::setRemainingBackwashTime(int remainingBackwashTime)
{
    this->remainingBackwashTime = remainingBackwashTime;
}

/**
 * @brief Set the elapsed filtration time since the last backwash.
 *
 * @param filtrationTimeSinceBackwash The filtration time (minutes) since the last backwash.
 */
void SandFlushItem::setFiltrationTimeSinceBackwash(long int filtrationTimeSinceBackwash)
{
    this->filtrationTimeSinceBackwash = filtrationTimeSinceBackwash;
}

/**
 * @brief Get the FiltrationSystemRunning signal name (listener)
 *
 * @return std::string signal names
 */
std::string SandFlushItem::getFiltrationSystemRunningSignalName() const
{
    return SignalNameResolver::generateLocalSignalName("filtrationSystemRunning", SignalNameResolver::SignalType::Listener);
}

/**
 * @brief Get the signal names generated for each filter backwash state.
 *
 * @return std::vector<std::string> The list of generated backwash signal names.
 */
std::vector<std::string> SandFlushItem::getFilterBackwashSignalNames() const
{
    std::vector<std::string> signalNames;
    for(int i = 0; i < this->parallelFilterCount; i++) // generate new signal names based on the updated parallelFilterCount
    {
        auto signal = SignalNameResolver::generateLocalSignalName("backwashingFilter" + std::to_string(i), SignalNameResolver::SignalType::Broadcaster);
        signalNames.push_back(signal);
    }
    return signalNames;
}

/**
 * @brief Get the local signal names exposed by this item.
 *
 * @return std::vector<std::string> The local signal list; currently empty.
 */
std::vector<std::string> SandFlushItem::getLocalSignalNames()
{
    auto signalList = getFilterBackwashSignalNames();
    signalList.push_back(getFiltrationSystemRunningSignalName());
    
    return signalList;
}
