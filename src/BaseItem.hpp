#pragma once

#include <string>
#include <ArduinoJson.h>
#include "IJsonSerializable.hpp"

class BaseItem: public IJsonSerializable
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
    virtual void populateFromJson(std::string json) override;
    virtual std::string toJson() const override;

protected:
    virtual void populateDerivedClassFromJson(JsonDocument &doc) {};
    virtual void derivedClassToJson(JsonDocument &doc) const {};

private:
    uint64_t id;
    std::string name;
    bool status;
};
