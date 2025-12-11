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
void GpioService::create(GpioItem newItem)
{
    list.addItem(newItem);
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
void GpioService::update(uint64_t id, GpioItem newItem)
{
    list.modifyItem(id, newItem);
    notify();
    storeAll();
}

/**
 * @brief Get the GPIO list in JSON format.
 * 
 * @return std::string The GPIO list in JSON format.
 */
std::string GpioService::getAll()
{
    return list.toJson();
}

/**
 * @brief Get a GPIO item in JSON format.
 * 
 * @return std::string The Desired GPIO item in JSON format.
 */
std::string GpioService::get(uint64_t id)
{
    return list.getItem(id).toJson();
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
 * @brief get a list of signal compatible items
 * @return list of all signal compatible items
 */
std::vector<ISignalCompatibleItem *> GpioService::getSignalCompatibleItems()
{
    std::vector<ISignalCompatibleItem *> signalCompatibleList;
    for(auto &item : list.getList()) // copy list
        signalCompatibleList.push_back(&list.getItem(item.getId()));
    return signalCompatibleList;
}

/**
 * @brief sync the actual GPIO pins to the status of the items and vice versa.
 * 
 */
void GpioService::syncHardware()
{
    SignalNameResolver::SignalNameParameters signalNameParameters;
    signalNameParameters.subsystemName = this->getName();
    
    for (auto& item : list.getList())
    {
        signalNameParameters.id = item.getId();
        signalNameParameters.localSignalName = item.getLocalSignalNames()[0];
        auto& itemRef = list.getItem(item.getPin());
        if (item.getMode() == 1) // if the item is an output pin
        {
            auto signalValue = SignalRouterService::getInstance()->getSignalValue(SignalNameResolver::toString(signalNameParameters));
            if (signalValue.has_value()) // if registered signal found
                itemRef.setStatus(signalValue.value());
            
            auto rawStatus = itemRef.getStatus();               // true = ON, false = OFF
            auto pwm = itemRef.getHighDutyCycle() * 255 / 100;
            bool inverted = itemRef.isInverted();

            auto myDigitalWrite = [](int pin, bool value)
            {
                pinMode(pin, OUTPUT);
                digitalWrite(pin, value);
            };

            if(rawStatus) // real status ON (not affected by inversion)
            { 
                // Apply PWM or full HIGH/LOW, respecting inversion
                if(pwm == 0) 
                    myDigitalWrite(item.getPin(), inverted ? HIGH : LOW);
                else if(pwm == 255)
                    myDigitalWrite(item.getPin(), inverted ? LOW : HIGH);
                else 
                {
                    // invert duty cycle if needed
                    auto effectivePwm = inverted ? (255 - pwm) : pwm;
                    if(effectivePwm != itemRef.getLastHighDutyCycle()) // prevent unnecessary writes
                    {
                        itemRef.setLastHighDutyCycle(effectivePwm);
                        analogWrite(item.getPin(), effectivePwm);
                    }
                    
                }
            } 
            else // real status OFF
            { 
                // Ignore PWM, force pin LOW or HIGH depending on inversion
                myDigitalWrite(item.getPin(), inverted ? HIGH : LOW);
            }

        } 
        else // if the item is an input pin
        {
            pinMode(item.getPin(), INPUT);
            itemRef.setStatus(digitalRead(item.getPin())); // update the status of the item from pin
            SignalRouterService::getInstance()->
                setSignalValue(SignalNameResolver::toString(signalNameParameters),
                     itemRef.isInverted() ? !itemRef.getStatus() : itemRef.getStatus());
        }
    }
}

/**
 * @brief Get the list of loggable items.
 * 
 * @return std::vector<std::unique_ptr<ILoggableItem>> The list of loggable items.
 */
std::vector<std::unique_ptr<ILoggableItem>> GpioService::getLoggableItems() const
{
    std::vector<std::unique_ptr<ILoggableItem>> loggableItems;
    for(const auto item : list.getList())
    {
        if(!item.getName().empty() and item.isLoggingEnabled()) // only add if it has a name and logging is enabled
            loggableItems.push_back(std::make_unique<GpioItem>(item)); // add to list
    }
    return loggableItems;
}