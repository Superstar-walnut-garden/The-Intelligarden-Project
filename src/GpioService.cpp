#include "GpioService.hpp"
#include "SignalRouterService.hpp"
#include "SignalNameResolver.hpp"
#include "Configuration.hpp"

/**
 * @brief initialize the instance of the GpioService to null
 * 
 */
GpioService* GpioService::instance = nullptr;

/**
 * @brief Construct a new GpioService::GpioService object
 * 
 */
GpioService::GpioService()
{
    restoreAll();
}

/**
 * @brief Destroy the GpioService object.
 * 
 */
GpioService::~GpioService()
{
    storeAll();
}

/**
 * @brief Get the instance of the GpioService (singleton pattern).
 * 
 * @return GpioService* The instance of the GpioService.
 */
GpioService* GpioService::getInstance()
{
    if (!instance)
    {
        instance = new GpioService();
    }
    return instance;
}

/**
 * @brief Create a new GPIO item and add it to the GPIO list.
 * 
 * @param newItem The new GPIO item to add.
 */
void GpioService::create(std::string json)
{
    auto newItem = std::make_unique<GpioItem>();
    newItem->populateFromJson(json);
    list.addItem(std::move(newItem));
    storeAll();
}

/**
 * @brief Remove a GPIO item from the GPIO list.
 * 
 * @param id The ID of the GPIO item to remove.
 */
void GpioService::remove(uint64_t id)
{
    list.deleteItem(id);
    notify();
    storeAll();
}

/**
 * @brief Update a GPIO item in the GPIO list.
 * 
 * @param id The ID of the GPIO item to update.
 * @param newItem The new GPIO item to replace the old one.
 */
void GpioService::update(uint64_t id, std::string json)
{
    auto newItem = std::make_unique<GpioItem>();
    newItem->populateFromJson(json);
    list.modifyItem(id, std::move(newItem));
    notify();
    storeAll();
}

/**
 * @brief Get the GPIO list in JSON format.
 * 
 * @return std::string The GPIO list in JSON format.
 */
std::string GpioService::getAll() const
{
    return list.toJson();
}

/**
 * @brief Get a GPIO item in JSON format.
 * 
 * @return std::string The Desired GPIO item in JSON format.
 */
std::string GpioService::get(uint64_t id) const
{
    auto item = list.getItem(id);
    return item ? item->toJson() : "{}";
}

/**
 * @brief Save the GPIO list to the configuration and update the hardware.
 * 
 */
void GpioService::storeAll()
{
    Configuration::getInstance()->setGpioList(list.toJson());
}

/**
 * @brief Load the GPIO list from the configuration.
 * 
 */
void GpioService::restoreAll()
{
    auto state = Configuration::getInstance()->getGpioList();
    if (state.empty())
        return;
    list.repopulateWith(state.c_str());
    syncHardware();
}

/**
 * @brief get unique name of subsystem (manager)
 * @return Subsystem Name
 */
std::string GpioService::getName() const
{
    return "Gpio";
}

/**
 * @brief sync the actual GPIO pins to the status of the items and vice versa.
 * 
 */
void GpioService::syncHardware()
{
    SignalNameResolver::SignalNameParameters signalNameParameters;
    signalNameParameters.subsystemName = this->getName();
    
    list.forEach([&](GpioItem *item)
    {
        signalNameParameters.id = item->getId();
        signalNameParameters.localSignalName = item->getLocalSignalNames()[0];
        if (item->getMode() == 1) // if the item is an output pin
        {
            auto signalValue = SignalRouterService::getInstance()->getSignalValue(SignalNameResolver::toString(signalNameParameters));
            if (signalValue.has_value()) // if registered signal found
                item->setStatus(signalValue.value());
            
            auto rawStatus = item->getStatus();               // true = ON, false = OFF
            auto pwm = item->getHighDutyCycle() * 255 / 100;
            bool inverted = item->isInverted();

            auto myDigitalWrite = [](int pin, bool value)
            {
                pinMode(pin, OUTPUT);
                digitalWrite(pin, value);
            };

            if(rawStatus) // real status ON (not affected by inversion)
            { 
                // Apply PWM or full HIGH/LOW, respecting inversion
                if(pwm == 0) 
                    myDigitalWrite(item->getPin(), inverted ? HIGH : LOW);
                else if(pwm == 255)
                    myDigitalWrite(item->getPin(), inverted ? LOW : HIGH);
                else 
                {
                    // invert duty cycle if needed
                    auto effectivePwm = inverted ? (255 - pwm) : pwm;
                    if(effectivePwm != item->getLastHighDutyCycle()) // prevent unnecessary writes
                    {
                        item->setLastHighDutyCycle(effectivePwm);
                        analogWrite(item->getPin(), effectivePwm);
                    }
                    
                }
            } 
            else // real status OFF
            { 
                // Ignore PWM, force pin LOW or HIGH depending on inversion
                myDigitalWrite(item->getPin(), inverted ? HIGH : LOW);
            }

        } 
        else // if the item is an input pin
        {
            pinMode(item->getPin(), INPUT);
            item->setStatus(digitalRead(item->getPin())); // update the status of the item from pin
            SignalRouterService::getInstance()->
                setSignalValue(SignalNameResolver::toString(signalNameParameters),
                     item->isInverted() ? !item->getStatus() : item->getStatus());
        }
    });
}

/**
 * @brief Get a temporary view of loggable items.
 * 
 * @return std::vector<std::unique_ptr<ILoggableItem>> The list of loggable items.
 */
std::vector<ILoggableItem *> GpioService::getLoggableItems() const
{
    return list.getAllAs<ILoggableItem>();
}

/**
 * @brief Get a temporary view of signal compatible items
 * @return list of all signal compatible items
 */
std::vector<ISignalCompatibleItem *> GpioService::getSignalCompatibleItems()
{
    return list.getAllAs<ISignalCompatibleItem>();
}
