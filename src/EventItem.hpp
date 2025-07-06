#ifndef EVENTITEM_HPP
#define EVENTITEM_HPP

#include "BaseItem.hpp"
#include <string>

class EventItem : public BaseItem
{
public:
    EventItem();
    EventItem(int id, int event_id, std::string name, bool flag, bool occupied, std::string logic);
    ~EventItem();

    bool getFlag() const;
    void setFlag(bool flag);

    std::string getLogic() const;
    void setLogic(std::string logic);

    bool getPrevState();
    void setPrevState(bool prevState);

    bool isOccupied() const;
    void setOccupied(bool occupied);

private:
    void populateDerivedClassFromJson(JsonDocument &doc) override;
    void derivedClassToJson(JsonDocument &doc) override;
    
    bool occupied;
    bool prevState;
    std::string logic;
    using BaseItem::setEventId; // making setEventId private (because it's not needed)
};

#endif // EVENTITEM_HPP