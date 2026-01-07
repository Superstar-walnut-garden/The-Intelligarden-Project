#pragma once
#include <string>

/** @brief Interface for JSON serializable classes.
 *
 * Classes implementing this interface should provide methods to populate their data from a JSON string
 * and to convert their data to a JSON string.
 */
class IJsonSerializable 
{
public:
    virtual ~IJsonSerializable() = default;
    virtual void populateFromJson(std::string json) = 0;
    virtual std::string toJson() const = 0;
};