#ifndef EVENTITEM_HPP
#define EVENTITEM_HPP
#include <string>

class EventItem 
{
public:
    EventItem(int id, std::string name, bool flag, bool occupied);
    ~EventItem();

    int getId() const;
    std::string getName() const;
    void setName(std::string name);
    bool getFlag() const;
    void setFlag(bool flag);

    bool isOccupied() const;
    void setOccupied(bool occupied);

private:
    int id;
    std::string name;
    bool flag;
    bool occupied;
};

#endif // EVENTITEM_HPP