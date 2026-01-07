#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <memory> // added for unique_ptr
#include "BaseItem.hpp"

template <class T>
class BaseList
{
    static_assert(std::is_base_of<BaseItem, T>::value, "T must derive from BaseItem");
public:
    BaseList();
    ~BaseList() {};
    BaseList(std::string json);
    virtual T *getItem(uint64_t id);
    virtual std::vector<std::unique_ptr<T>> &getList(); // changed return type
    virtual std::string toJson(std::function<bool(const T*)> filter = nullptr) const;
    virtual void addItem(std::unique_ptr<T> Item); // changed parameter type
    virtual void repopulateWith(std::string json);
    virtual void printList();
    virtual void modifyItem(uint64_t id, std::unique_ptr<T> newItem); // changed parameter type
    virtual void deleteItem(uint64_t id);

    virtual void forEach(std::function<void(T*)> func, std::function<bool(const T*)> filter = nullptr) const;

    template <class I>
    I* getAs(uint64_t id) const ;

    template <class I, class F = T>
    std::vector<I*> getAllAs(std::function<bool(const F*)> filter = nullptr) const;

private:
    std::vector<std::unique_ptr<T>> list; // changed to unique_ptr
};

#include "BaseList.tpp"
