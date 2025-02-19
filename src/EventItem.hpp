#ifndef EVENTITEM_HPP
#define EVENTITEM_HPP

#include "BaseItem.hpp"
#include <string>

class EventItem : public BaseItem
{
public:
    EventItem();
    EventItem(int id, int event_id, std::string name, bool flag, bool occupied, bool invert = false);
    ~EventItem();

    bool getFlag() const;
    void setFlag(bool flag);

    bool isInvert() const;
    void setInvert(bool invert);

    bool isOccupied() const;
    void setOccupied(bool occupied);

private:
    void populateDerivedClassFromJson(JsonDocument &doc) override;
    void derivedClassToJson(JsonDocument &doc) override;
    
    bool occupied;
    bool invert;
    using BaseItem::setEventId; // making setEventId private (because it's not needed)
};

#endif // EVENTITEM_HPP