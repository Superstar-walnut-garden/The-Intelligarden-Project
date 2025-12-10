#pragma once
#include <string>
#include <ArduinoJson.h>
#include <optional>

class JsonUtils
{
public:
    struct LastEntry 
    {
        std::string key;
        std::string value;
    };

    static std::optional<LastEntry> getLastEntry(const JsonDocument& doc) 
    {
    // Cast the const document to a const variant, then to a const object
    JsonObjectConst obj = doc.as<JsonVariantConst>().as<JsonObjectConst>();
    if (obj.isNull() || obj.begin() == obj.end()) 
    {
        // Empty or invalid document
        return std::nullopt;
    }

    // Walk forward, keep track of the last valid iterator
    auto it = obj.begin();
    auto lastIt = it;
    for (; it != obj.end(); ++it) 
    {
        lastIt = it;
    }

    LastEntry result;

    // Key is stored internally; copy into std::string for safety
    result.key = std::string(lastIt->key().c_str());

    // Serialize the value to a std::string
    std::string buffer;
    serializeJson(lastIt->value(), buffer);
    result.value = std::move(buffer);

    return result;
}


    static bool jsonEquals(const JsonObject& a, const JsonObject& b) 
    {
        // Serialize both objects into std::string
        std::string sa, sb;
        serializeJson(a, sa);
        serializeJson(b, sb);

        // Compare the serialized strings
        return sa == sb;
    }

};