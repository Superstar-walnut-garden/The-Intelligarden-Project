#include "GpioItem.hpp"
#include "SignalNameResolver.hpp"

/**
 * @brief Construct a new GpioItem::GpioItem object
 * 
 */
GpioItem::GpioItem()
    : SignalCompatibleBaseItem(), mode(0), extraParameters(""), highDutyCycle(100), lastHighDutyCycle(-1)
{
    registerToJsonCallback([this](JsonDocument &json) -> void
    {
        json["extraParameters"] = this->extraParameters;
        json["mode"] = this->mode;
        json["inverted"] = this->inverted;
        json["highDutyCycle"] = this->highDutyCycle;
        
        json["loggingEnabled"] = this->loggingEnabled;
        json["logOnlyOnChange"] = this->logOnlyOnDataChange;
        json["logInterval"] = this->logInterval;
    });
    registerFromJsonCallback([this](JsonDocument &json) -> void
    {
        this->extraParameters = json["extraParameters"].as<std::string>();
        this->mode = json["mode"].as<short>();
        this->inverted = json["inverted"] | false;
        this->highDutyCycle = json["highDutyCycle"] | 100;

        this->loggingEnabled = json["loggingEnabled"] | false;
        this->logOnlyOnDataChange = json["logOnlyOnChange"] | false;
        this->logInterval = json["logInterval"] | 60;
    });
}

/** * @brief Get the pin number of the GPIO item
 * 
 * @return int Pin number
 */ 
int GpioItem::getPin() const
{
    return this->getId();
}

/** * @brief Get the mode of the GPIO item
 * @return int Mode of the GPIO item
 */
int GpioItem::getMode() const
{
    return this->mode;
}

/** * @brief Set the mode of the GPIO item
 * 
 * @param mode Mode to set (0 for input, 1 for output, etc.)
 */
void GpioItem::setMode(short mode)
{
    this->mode = mode;
}

/** * @brief Get the extra parameters of the GPIO item
 * 
 * @return std::string Extra parameters as a string
 */
std::string GpioItem::getExtraParameters() const
{
    return this->extraParameters;
}

/** * @brief Set the extra parameters of the GPIO item
 * 
 * @param extraParameters Extra parameters to set as a string
 */
void GpioItem::setExtraParameters(std::string extraParameters)
{
    this->extraParameters = extraParameters;
}

/** * @brief Get HighDutyCycle of the GPIO item
 * 
 * @return int HighDutyCycle value
 */
int GpioItem::getHighDutyCycle() const
{
    return this->highDutyCycle;
}

/** * @brief Check if the GPIO item is inverted
 * 
 * @return true If inverted, false otherwise
 */
bool GpioItem::isInverted() const
{
    return this->inverted;
}

/** * @brief get Last high dutycycle 
 * 
 * @return int last high dutycycle
 */
int GpioItem::getLastHighDutyCycle() const
{
    return this->lastHighDutyCycle;
}

/** * @brief set Last high dutycycle 
 * 
 * @param lastHighDutyCycle last high dutycycle to set
 */
void GpioItem::setLastHighDutyCycle(int lastHighDutyCycle)
{
    this->lastHighDutyCycle = lastHighDutyCycle;
}

/** * @brief Get the local signal names for the GPIO item
 * 
 * @return std::vector<std::string> Vector of local signal names
 */
std::vector<std::string> GpioItem::getLocalSignalNames()
{
    // Dynamic SignalType and Name Generation
    SignalNameResolver::SignalType signalType;
    if(mode == 0) // if input, it broadcasts the pin status to a signal
        signalType = SignalNameResolver::SignalType::Broadcaster;
    else
        signalType = SignalNameResolver::SignalType::Listener;

    return {SignalNameResolver::generateLocalSignalName("default", signalType)};
}

/**
 * @brief Get the name of the GPIO item (for ambiguity resolving).
 * 
 * @return std::string The name of the object.
 */
std::string GpioItem::getName() const
{
    return BaseItem::getName();
}

/**
 * @brief Get the ID of the GPIO item (for ambiguity resolving).
 * 
 * @return uint64_t The ID of the object.
 */
uint64_t GpioItem::getId() const
{
    return BaseItem::getId();
}

/**
 * @brief Get the logging interval for the GPIO item.
 * 
 * @return uint64_t The logging interval in seconds.
 */
uint64_t GpioItem::getInterval() const
{
    return logInterval;
}

/**
 * @brief Check if the GpioItem object should log only on change.
 * 
 * @return true If the object should log only on change, false otherwise.
 */
bool GpioItem::logOnlyOnChange() const
{
    return logOnlyOnDataChange;
}

/**
 * @brief Get the data of the GpioItem object as a JSON string.
 * 
 * @return std::string The JSON string of the object's data.
 */
std::string GpioItem::getData() const
{
    JsonDocument doc;
    // doc["pin"] = this->getPin();
    doc["status"] = this->getStatus();
    // doc["mode"] = this->getMode();
    // doc["extraParameters"] = this->getExtraParameters();
    
    std::string output;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief Check if logging is enabled for the GpioItem object.
 * 
 * @return true If logging is enabled, false otherwise.
 */
bool GpioItem::isLoggingEnabled() const
{
    return loggingEnabled;
}