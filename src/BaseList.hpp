#ifndef BASELIST_HPP
#define BASELIST_HPP

#include <string>
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
    virtual T &getItem(uint64_t id);
    virtual std::vector<T> getList();
    virtual std::string toJson();
    virtual void addItem(T Item);
    virtual void repopulateWith(std::string json);
    virtual void printList();
    virtual void modifyItem(uint64_t id, T& newItem);
    virtual void deleteItem(uint64_t id);

    virtual void forEach(std::function<void(T&)> func);

protected:
    virtual std::vector<T> &getListRef();

private:
    std::vector<T> list;
};

#include "BaseList.tpp"
#endif // BASELIST_HPP