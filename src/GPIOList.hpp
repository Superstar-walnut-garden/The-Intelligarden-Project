#ifndef GPIOLIST_HPP
#define GPIOLIST_HPP

#include "GPIOItem.hpp"
#include "BaseList.hpp"

class GPIOList : public BaseList<GPIOItem>
{
public:
    GPIOList();
    GPIOList(std::string json);

    std::string toJson() override;
    void repopulateWith(std::string json) override;
    void printList() override;
};

#endif // GPIOLIST_HPP