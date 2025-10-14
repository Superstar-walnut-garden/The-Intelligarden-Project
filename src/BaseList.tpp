#include "BaseList.hpp" // for intellisence to work and recognize the BaseList class
#include <ArduinoJson.h>
#include <vector>
#include <iostream>

/**
 * @brief Construct a new Base List< T>:: Base List object  (default constructor).
 * 
 * @tparam T 
 */
template <class T>
BaseList<T>::BaseList()
{
}

/**
 * @brief Construct a new Base List< T>:: Base List object.
 * 
 * @tparam T 
 * @param json 
 */
template <class T>
BaseList<T>::BaseList(std::string json)
{
    this->repopulateWith(json);
}

/**
 * @brief Get an item from the list by its ID.
 * 
 * @tparam T 
 * @param id
 * @return T& 
 */
template <class T>
T &BaseList<T>::getItem(uint64_t id)
{
    static auto nullItem = T(); // item with default parameters (id = -1)
    for(auto &item : list)
    {
        if(item.getId() == id)
            return item;
    }
    return nullItem;
}

/**
 * @brief Get the list of items.
 * 
 * @tparam T 
 * @return std::vector<T> 
 */
template <class T>
std::vector<T> BaseList<T>::getList()
{
    return list;
}

/**
 * @brief Convert the list to a JSON string.
 * 
 * @tparam T 
 * @return std::string of the list in JSON format
 */
template <class T>
std::string BaseList<T>::toJson()
{
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();

    for (auto& item : list) 
    {
        JsonObject obj = array.add<JsonObject>();
        deserializeJson(obj, item.toJson()); // add items
    }

    std::string output;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief Add an item to the list.
 * 
 * @tparam T 
 * @param item new item to add to the list
 */
template <class T>
void BaseList<T>::addItem(T item)
{
    list.push_back(item);
}

/**
 * @brief Populate the list with items from a JSON string.
 * 
 * @tparam T 
 * @param json json list of items to populate the list with
 */
template <class T>
void BaseList<T>::repopulateWith(std::string json)
{
    JsonDocument doc;
    deserializeJson(doc, json);
    list.clear(); // delete the old items before adding new ones

    for (JsonObject item : doc.as<JsonArray>()) 
    {
        std::string jsonStr;
        serializeJson(item, jsonStr);
        T populatedItem = T();
        populatedItem.populateFromJson(jsonStr);
        list.push_back(populatedItem);
    }
}

/**
 * @brief Print the list of items (only BaseItem parameters id, name and status).
 * 
 * @tparam T 
 */
template <class T>
void BaseList<T>::printList()
{
    for(auto & item : list)
        std::cout <<
            "id: " << item.getId() << std::endl <<
            "name: " << item.getName() << std::endl <<
            "status: " << item.getStatus() << std::endl;
}

/**
 * @brief Modify an item in the list.
 * 
 * @tparam T 
 * @param id id of the item to modify
 * @param newItem new item to replace the old one
 */
template <class T>
void BaseList<T>::modifyItem(uint64_t id, T& newItem)
{
    for (auto& item : list)
    {
        if (item.getId() == id)
        {
            item = newItem;
            return;
        }
    }
}

/**
 * @brief Delete an item in the list
 * 
 * @tparam T 
 * @param id  id of the item to delete
 */
template <class T>
void BaseList<T>::deleteItem(uint64_t id)
{
    list.erase(std::remove_if(list.begin(), list.end(), [id](T& item) {
        return item.getId() == id;
    }), list.end());
}

/**
 * @brief for each item in the list, call the given function.
 * 
 * @tparam T
 * @param func function to call for each item in the list
 */
template <class T>
void BaseList<T>::forEach(std::function<void(T&)> func)
    {
        for (auto &item : this->list)
        {
            func(item);
        }
    }

/**
 * @brief Get a reference to the list.
 * 
 * @tparam T 
 * @return std::vector<T>& 
 */
template <class T>
std::vector<T> &BaseList<T>::getListRef()
{
    return list;
}