#include "SandFlushService.hpp"
#include "Configuration.hpp"
#include <ArduinoJson.h>
#include "SignalRouterService.hpp"
#include "SignalNameResolver.hpp"

/**
 * @brief Singleton instance of the SandFlushService class.
 * 
 */
SandFlushService* SandFlushService::instance = nullptr;

/**
 * @brief Construct a new SandFlushService object.
 * 
 */
SandFlushService::SandFlushService()
{
    restoreAll();
}

/**
 * @brief Destroy the SandFlushService object.
 * 
 */
SandFlushService::~SandFlushService() 
{
    storeAll();
}

/**
 * @brief Get the instance of the SandFlushService (singleton pattern).
 * 
 * @return SandFlushService* The instance of the SandFlushService.
 */
SandFlushService* SandFlushService::getInstance() 
{
    if (!instance) 
    {
        instance = new SandFlushService();
    }
    return instance;
}

/**
 * @brief determine the status of items based on the current time and weekday.
 * 
 */
void SandFlushService::determineStatusofItems() 
{
    auto systemTime = SystemTimeService::getInstance(); // get direct access to system time
    short postponeTime = 30; // minutes
    list.forEach([&](SandFlushItem *item)
    {
        // lambda helpers
        auto calculateAutoBackwashRetryInTime = [&]() -> void
        {
            auto elapsedMinutes = item->autoBackwashCancelTime.has_value() ? std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now() - item->autoBackwashCancelTime.value()).count() : postponeTime;
            if(elapsedMinutes > postponeTime) 
                elapsedMinutes = postponeTime; // limit elapsed to max (postpone time)
            item->setAutoBackwashRetryIn(postponeTime - elapsedMinutes); // calculate remaining time to perform a retry
        };
        auto cancelAutoBackwash = [&]() -> void
        {
            item->autoBackwashCancelTime = std::chrono::steady_clock::now();
            item->setAutoBackwashInterrupted(true); // set auto backwash interrupted flag
        };
        auto calculateFiltrationTimeSinceBackwash =  [&]() -> void
        {
            // increment filteration timer (item->setFiltrationTimeSinceBackwash(x)) for every minute of filteration
            const auto elapsedMinutes = std::chrono::duration_cast<std::chrono::minutes>(
                std::chrono::steady_clock::now() - item->filtrationStartTime).count();
            item->setFiltrationElapsedTimeSinceBackwash(item->getFiltrationElapsedTimeUntilCurrentSession() + elapsedMinutes);
        };
        auto calculateStartupDelayRemaining = [&]() -> void
        {
            const auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - item->idleToFiltrationTransitionTimeStamp
            ).count();

            item->setStartupDelayRemaining(std::max(
                0LL,
                item->getStartupDelay() - elapsedSeconds
            ));
        };
        auto startBackwash = [&]() -> void
        {
            item->setOperatingState(SandFlushItem::OperatingState::Backwashing);
            item->setBackwashingFilterNumber(0);
            item->backwashStartTime = std::chrono::steady_clock::now();
        };

        auto filtrationSystemRunningSignal = SignalRouterService::getInstance()->
                getSignalValue(SignalNameResolver::toString(
                    SignalNameResolver::SignalNameParameters(this->getName(), item->getId(), item->getFiltrationSystemRunningSignalName())));
        auto filtrationSystemRunning = (filtrationSystemRunningSignal.has_value() and filtrationSystemRunningSignal.value());
        if(!filtrationSystemRunning or !item->getStatus()) // idle operating state
        {
            if((item->getOperatingState() == SandFlushItem::OperatingState::Backwashing) and !item->isBackwashTriggeredManually())
                item->setAutoBackwashInterrupted(true);
            item->setBackwashTriggeredManually(false);
            item->setOperatingState(SandFlushItem::OperatingState::Idle); // transition to idle 
            item->setBackwashingFilterNumber(-1); // reset backwashing filter number
            item->setRemainingBackwashTime(0); // reset backwashing remaining time
            item->setStartupDelayRemaining(0); // reset startup delay remaining time
            item->isCancelFlush(); // consume and reject cancel flag
            item->isManualFlush(); // consume and reject manual flag
        }
        else if (item->getOperatingState() == SandFlushItem::OperatingState::Idle) // transition from idle to filtration operating state
        {
            item->setOperatingState(SandFlushItem::OperatingState::Filtering);
            item->filtrationStartTime = std::chrono::steady_clock::now();
            item->idleToFiltrationTransitionTimeStamp = std::chrono::steady_clock::now();
            item->setFiltrationElapsedTimeUntilCurrentSession(item->getFiltrationElapsedTimeSinceBackwash());
        }

        if (item->getOperatingState() == SandFlushItem::OperatingState::Filtering) // filtration operating state
        {
            calculateFiltrationTimeSinceBackwash();
            calculateStartupDelayRemaining();
            // if something triggers backwashing
            auto isAutoBackwashTriggered = [&] () -> auto
            {
                // get trigger mode
                auto triggerMode = item->getTriggerMode();

                // assess which trigger modes are allowed
                auto autoTriggerAllowed = (item->getStartupDelayRemaining() == 0) and (item->getAutoBackwashRetryIn() == 0) and (triggerMode != SandFlushItem::TriggerMode::ManualOnly);
                auto intervalTriggerAllowed = ((triggerMode == SandFlushItem::TriggerMode::Interval) or (triggerMode == SandFlushItem::TriggerMode::Hybrid));
                auto dPTriggerAllowed = ((triggerMode == SandFlushItem::TriggerMode::DP) or (triggerMode == SandFlushItem::TriggerMode::Hybrid));

                auto intervalTrigger = (item->getFiltrationElapsedTimeSinceBackwash() > item->getFlushInterval()) and intervalTriggerAllowed;
                auto pendingAutoBackwash = item->isAutoBackwashInterrupted();
                auto dPTrigger = false and dPTriggerAllowed;
                auto autoTrigger = (intervalTrigger or pendingAutoBackwash or dPTrigger) and autoTriggerAllowed;
                return autoTrigger;
            };
            auto manualTrigger = item->isManualFlush();
            if(isAutoBackwashTriggered() or manualTrigger) // transition to backwashing
            {
                item->setBackwashTriggeredManually(manualTrigger);
                startBackwash();
            }
            
        }

        if (item->getOperatingState() == SandFlushItem::OperatingState::Backwashing) // backwashing operating state
        {
            const double valveDelay = item->getValveDelay();
            const double flushDuration = item->getFlushDuration();
            const double dwellTime = item->getDwellTime();
            const int filterCount = item->getParallelFilterCount();

            const double totalBackwashTime =
                filterCount * (2 * valveDelay + flushDuration) +
                (filterCount - 1) * dwellTime;

            const double elapsedSeconds = std::chrono::duration<double>(
                std::chrono::steady_clock::now() - item->backwashStartTime
            ).count();
            
            if(!item->isSwitchSequence()) // if switch sequence is disabled
                item->setFlushSequenceReversed(false); // do not use reversed sequence

            if (elapsedSeconds < totalBackwashTime) // ongoing backwash sequence
            {
                item->setRemainingBackwashTime(
                    static_cast<int>(std::ceil(totalBackwashTime - elapsedSeconds))
                );

                double timeOffset = 0;
                const bool reversed = item->isFlushSequenceReversed();
                for (int i = 0; i < filterCount; i++)
                {
                    // Valve opening delay + actual flushing
                    const double activeDuration = valveDelay + flushDuration;
                    const int filterIndex = reversed ? filterCount - 1 - i : i;

                    if (elapsedSeconds < timeOffset + activeDuration)
                    {
                        item->setBackwashingFilterNumber(filterIndex);
                        break;
                    }

                    timeOffset += activeDuration;

                    // Valve closing delay + dwell time
                    const double inactiveDuration =
                        valveDelay + (i < filterCount - 1 ? dwellTime : 0);

                    if (elapsedSeconds < timeOffset + inactiveDuration)
                    {
                        item->setBackwashingFilterNumber(-1);
                        break;
                    }

                    timeOffset += inactiveDuration;
                }
                if(item->isCancelFlush()) // cancel backwash
                {
                    if(!item->isBackwashTriggeredManually()) // if trigger source was automatic
                        cancelAutoBackwash();
                    item->setBackwashTriggeredManually(false);
                    item->setBackwashingFilterNumber(-1);
                    item->setOperatingState(SandFlushItem::OperatingState::Filtering); // transition back to filtering
                }
            }
            else // total backwash sequence finished successfully
            {
                if(item->isSwitchSequence()) // if switch sequence is enabled
                    item->setFlushSequenceReversed(!item->isFlushSequenceReversed()); // toggle reversed sequence
                item->setBackwashTriggeredManually(false);
                item->setAutoBackwashInterrupted(false);
                item->autoBackwashCancelTime = std::nullopt;
                item->setRemainingBackwashTime(0);
                item->setBackwashingFilterNumber(-1);
                item->setFiltrationElapsedTimeSinceBackwash(0); // reset the total filteration timer
                item->setFiltrationElapsedTimeUntilCurrentSession(0); // reset the filteration timer until current session
                item->filtrationStartTime = std::chrono::steady_clock::now(); // reset the filtration start time
                item->setOperatingState(SandFlushItem::OperatingState::Filtering); // transition back to filtering
            }
        }
        broadcastItem(item); // broadcast backwash signals
        calculateAutoBackwashRetryInTime();
    });
}

/**
 * @brief Create a new SandFlushItem and add it to the list.
 * 
 * @param SandFlushItem The SandFlushItem to be created.
 */
void SandFlushService::create(std::string json) 
{
    auto newItem = std::make_unique<SandFlushItem>();
    newItem->populateFromJson(json);
    Serial.printf("SandFlushItem %d created\n", newItem->getId());
    list.addItem(std::move(newItem));
    storeAll();
}

/**
 * @brief Remove a SandFlushItem from the list by its ID.
 * 
 * @param id The ID of the SandFlushItem to be removed.
 */
void SandFlushService::remove(uint64_t id) 
{
    list.deleteItem(id);
    notify();
    storeAll();
}

/**
 * @brief update an existing SandFlushItem in the list.
 * 
 * @param id The ID of the SandFlushItem to be modified.
 * @param newItem The new SandFlushItem with updated values.
 */
void SandFlushService::update(uint64_t id, std::string json) 
{
    list.getItem(id)->populateFromJson(json);
    notify();
    storeAll();
}

/**
 * @brief Save the current state of the SandFlushService to the internal storage (SPIFFS).
 * 
 */
void SandFlushService::storeAll()
{
    Configuration::getInstance()->setSandFlush(list.toJson());
}

/**
 * @brief Load the state of the SandFlushService from the internal storage (SPIFFS).
 * 
 */
void SandFlushService::restoreAll() 
{
    auto data = Configuration::getInstance()->getSandFlush();
    if (data.empty()) return;
        list.repopulateWith(data);
}

/**
 * @brief Get the JSON representation of the SandFlushService list (for web-api).
 * 
 * @return std::string The SandFlushService list in JSON format.
 */
std::string SandFlushService::getAll() const
{
    return list.toJson();
}

/**
 * @brief Get an item from JSON representation of the SandFlushService list (for web-api).
 * 
 * @return std::string an item in JSON format.
 */
std::string SandFlushService::get(uint64_t id) const
{
    auto item = list.getItem(id);
    return item ? item->toJson() : "{}";   // return empty JSON object
}

/**
 * @brief Get the list of signal-compatible items in the SandFlushService (for the CentralizedSignalHub).
 * 
 * @return std::vector<ISignalCompatibleItem*> A vector of pointers to signal-compatible items.
 */
std::vector<ISignalCompatibleItem *> SandFlushService::getSignalCompatibleItems()
{
    return list.getAllAs<ISignalCompatibleItem>();
}

/**
 * @brief The main loop of the SandFlushService, call periodically.
 * 
 */
void SandFlushService::loop()
{
    determineStatusofItems();
}

/**
 * @brief Broadcast the status of a SandFlushItem to SignalRouterService
 * 
 * @param item The SandFlushItem to be broadcasted.
 */
void SandFlushService::broadcastItem(SandFlushItem *item)
{
    for(int i = 0; i < item->getParallelFilterCount(); i++)
    {
        SignalRouterService::getInstance()->setSignalValue(SignalNameResolver::toString(
            SignalNameResolver::SignalNameParameters(
                this->getName(), 
                item->getId(), 
                item->getFilterBackwashSignalNames().at(i)
            )), item->getBackwashingFilterNumber() == i ? true : false); // broadcast the backwashing filter number as a boolean signal
    }
}