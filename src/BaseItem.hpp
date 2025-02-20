#ifndef BASEITEM_HPP
#define BASEITEM_HPP
#include <string>
#include <ArduinoJson.h>

class BaseItem 
{
public:
    BaseItem();
    BaseItem(int id, int event_id, std::string name, bool status);
    virtual ~BaseItem();

    virtual int getId() const;
    virtual void setId(int id);
    virtual int getEventId() const;
    virtual void setEventId(int event_id);
    virtual std::string getName() const;
    virtual void setName(std::string name);
    virtual bool getStatus() const;
    virtual void setStatus(bool status);
    virtual void populateFromJson(std::string json);
    virtual std::string toJson();

protected:
    virtual void populateDerivedClassFromJson(JsonDocument &doc) {};
    virtual void derivedClassToJson(JsonDocument &doc) {};

private:
    int id, event_id;
    std::string name;
    bool status;
};

#endif // BASEITEM_HPP