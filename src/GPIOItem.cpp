#include "GPIOItem.hpp"
#include "SignalNameResolver.hpp"

/**
 * @brief Construct a new GPIOItem::GPIOItem object
 * 
 */
GPIOItem::GPIOItem()
    : SignalCompatibleBaseItem(), mode(0), extraParameters("")
{
}

/** * @brief Construct a new GPIOItem::GPIOItem object
 * 
 * @param pin Pin number
 * @param name Name of the GPIO item
 * @param status Initial status of the GPIO item
 * @param mode Mode of the GPIO item (0 for input, 1 for output, etc.)
 * @param extraParameters Extra parameters for the GPIO item
 */
GPIOItem::GPIOItem(int pin, std::string name, bool status, short mode, std::string extraParameters)
    : SignalCompatibleBaseItem(pin, name, status), mode(mode), extraParameters(extraParameters)
{
}

/** * @brief Get the pin number of the GPIO item
 * 
 * @return int Pin number
 */ 
int GPIOItem::getPin()
{
    return this->getId();
}

/** * @brief Get the mode of the GPIO item
 * @return int Mode of the GPIO item
 */
int GPIOItem::getMode()
{
    return this->mode;
}

/** * @brief Set the mode of the GPIO item
 * 
 * @param mode Mode to set (0 for input, 1 for output, etc.)
 */
void GPIOItem::setMode(short mode)
{
    this->mode = mode;
}

/** * @brief Get the extra parameters of the GPIO item
 * 
 * @return std::string Extra parameters as a string
 */
std::string GPIOItem::getExtraParameters()
{
    return this->extraParameters;
}

/** * @brief Set the extra parameters of the GPIO item
 * 
 * @param extraParameters Extra parameters to set as a string
 */
void GPIOItem::setExtraParameters(std::string extraParameters)
{
    this->extraParameters = extraParameters;
}

/** * @brief Populate the derived class from JSON document
 * 
 * @param doc JSON document to populate from
 */
void GPIOItem::populateDerivedClassFromJson(JsonDocument &doc)
{
    this->extraParameters = doc["extraParameters"].as<std::string>();
    this->mode = doc["mode"].as<short>();
}

/** * @brief Convert the derived class to JSON document
 * 
 * @param doc JSON document to convert to
 */
void GPIOItem::derivedClassToJson(JsonDocument &doc)
{
    doc["extraParameters"] = this->extraParameters;
    doc["mode"] = this->mode;
}

/** * @brief Get the local signal names for the GPIO item
 * 
 * @return std::vector<std::string> Vector of local signal names
 */
std::vector<std::string> GPIOItem::getLocalSignalNames()
{
    // Dynamic SignalType and Name Generation
    SignalNameResolver::SignalType signalType;
    if(mode == 0) // if input, it broadcasts the pin status to a signal
        signalType = SignalNameResolver::SignalType::Broadcaster;
    else
        signalType = SignalNameResolver::SignalType::Listener;

    return {SignalNameResolver::generateLocalSignalName("default", signalType)};
}