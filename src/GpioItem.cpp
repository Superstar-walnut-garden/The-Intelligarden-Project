#include "GpioItem.hpp"
#include "SignalNameResolver.hpp"

/**
 * @brief Construct a new GpioItem::GpioItem object
 * 
 */
GpioItem::GpioItem()
    : SignalCompatibleBaseItem(), mode(0), extraParameters("")
{
}

/** * @brief Construct a new GpioItem::GpioItem object
 * 
 * @param pin Pin number
 * @param name Name of the GPIO item
 * @param status Initial status of the GPIO item
 * @param mode Mode of the GPIO item (0 for input, 1 for output, etc.)
 * @param extraParameters Extra parameters for the GPIO item
 */
GpioItem::GpioItem(int pin, std::string name, bool status, short mode, std::string extraParameters)
    : SignalCompatibleBaseItem(pin, name, status), mode(mode), extraParameters(extraParameters)
{
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

/** * @brief Populate the derived class from JSON document
 * 
 * @param doc JSON document to populate from
 */
void GpioItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    this->extraParameters = doc["extraParameters"].as<std::string>();
    this->mode = doc["mode"].as<short>();
    this->loggingEnabled = doc["loggingEnabled"] | false;
    this->logOnlyOnDataChange = doc["logOnlyOnChange"] | false;
    this->logInterval = doc["logInterval"] | 60;
}

/** * @brief Convert the derived class to JSON document
 * 
 * @param doc JSON document to convert to
 */
void GpioItem::derivedClassToJson(JsonDocument &doc) const
{
    doc["extraParameters"] = this->extraParameters;
    doc["mode"] = this->mode;
    doc["loggingEnabled"] = this->loggingEnabled;
    doc["logOnlyOnChange"] = this->logOnlyOnDataChange;
    doc["logInterval"] = this->logInterval;
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