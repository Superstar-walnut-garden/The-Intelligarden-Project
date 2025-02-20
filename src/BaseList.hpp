#ifndef BASELIST_HPP
#define BASELIST_HPP

#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include "BaseItem.hpp"

template <class T>
class BaseList
{
    static_assert(std::is_base_of<BaseItem, T>::value, "T must derive from BaseItem");
public:
    BaseList();
    BaseList(std::string json);
    virtual T &getItem(int id);
    virtual std::vector<T> getList();
    virtual std::string toJson();
    virtual void addItem(T Item);
    virtual void repopulateWith(std::string json);
    virtual void printList();
    virtual void modifyItem(int id, T& newItem);
    virtual void deleteItem(int id);

protected:
    virtual std::vector<T> &getListRef();

private:
    std::vector<T> list;
};

#include "BaseList.tpp"
#endif // BASELIST_HPP