#ifndef EVENTITEM_HPP
#define EVENTITEM_HPP

#include "BaseItem.hpp"
#include <string>

class EventItem : public BaseItem
{
public:
    EventItem();
    EventItem(int id, std::string name, bool flag, bool occupied);
    ~EventItem();

    bool getFlag() const;
    void setFlag(bool flag);

    bool isOccupied() const;
    void setOccupied(bool occupied);

    void populateFromJson(std::string json) override;
    std::string toJson() override;

private:
    bool occupied;
    using BaseItem::setEventId; // making setEventId private (because it's not needed)
};

#endif // EVENTITEM_HPP