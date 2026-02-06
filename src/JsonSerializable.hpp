#pragma once
#include <string>
#include "IJsonSerializable.hpp"
#include <vector>
#include <functional>
#include <ArduinoJson.h>

/** 
 * @brief JSON serializable base class.
 * 
 */
class JsonSerializable : public IJsonSerializable
{
public:
    JsonSerializable(){}
    virtual ~JsonSerializable() = default;
    virtual void populateFromJson(std::string json) override final
    {
        JsonDocument doc;
        deserializeJson(doc, json);
        for (auto &callback : fromJsonCallbacks)
            if(callback)
                callback(doc);
    }
    virtual std::string toJson() const override final
    {
        JsonDocument doc;
        for (auto &callback : toJsonCallbacks)
            if(callback)
                callback(doc);
        std::string output;
        serializeJson(doc, output);
        return output;
    }

protected:
    void registerToJsonCallback(std::function<void(JsonDocument&)> fn) 
    {
        toJsonCallbacks.push_back(fn);
    }
    void registerFromJsonCallback(std::function<void(JsonDocument&)> fn) 
    {
        fromJsonCallbacks.push_back(fn);
    }

private:
    std::vector<std::function<void(JsonDocument&)>> fromJsonCallbacks;
    std::vector<std::function<void(JsonDocument&)>> toJsonCallbacks;
};