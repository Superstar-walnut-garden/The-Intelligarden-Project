#pragma once

#include <string>
#include <ArduinoJson.h>
#include "JsonSerializable.hpp"

class BaseItem: public JsonSerializable
{
public:
    BaseItem();
    BaseItem(uint64_t id, std::string name, bool status);

    virtual ~BaseItem();

    virtual uint64_t getId() const;
    virtual void setId(uint64_t id);
    virtual std::string getName() const;
    virtual void setName(std::string name);
    virtual bool getStatus() const;
    virtual void setStatus(bool status);

private:
    uint64_t id;
    std::string name;
    bool status;

    void registerSerializationCallbacks();
};
