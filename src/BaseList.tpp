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
 * @return T* The desired item (nullptr if not found)
 */
template <class T>
T *BaseList<T>::getItem(uint64_t id)
{
    for(auto &item : list)
    {
        if(item->getId() == id)
            return item.get();
    }
    return nullptr;
}

/**
 * @brief Get the list of items.
 * 
 * @tparam T 
 * @return std::vector<std::unique_ptr<T>>& 
 */
template <class T>
std::vector<std::unique_ptr<T>> &BaseList<T>::getList() 
{
    return list;
}

/**
 * @brief Convert the list to a JSON string.
 *
 * @tparam T
 * @param filter Optional filter function to decide which items to include
 * @return std::string of the list in JSON format
 */
template <class T>
std::string BaseList<T>::toJson(std::function<bool(const T*)> filter) const
{
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();

    // Reuse forEach with optional filter
    this->forEach([&](T* item) 
    {
            JsonObject obj = array.add<JsonObject>();
            deserializeJson(obj, item->toJson());
    }, filter);

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
void BaseList<T>::addItem(std::unique_ptr<T> item)
{
    list.push_back(std::move(item));
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
        auto populatedItem = std::make_unique<T>();
        populatedItem->populateFromJson(jsonStr);
        list.push_back(std::move(populatedItem));
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
            "id: " << item->getId() << std::endl <<
            "name: " << item->getName() << std::endl <<
            "status: " << item->getStatus() << std::endl;
}

/**
 * @brief Modify an item in the list.
 * 
 * @tparam T 
 * @param id id of the item to modify
 * @param newItem new item to replace the old one
 */
template <class T>
void BaseList<T>::modifyItem(uint64_t id, std::unique_ptr<T> newItem)
{
    for (auto& item : list)
    {
        if (item->getId() == id)
        {
            item = std::move(newItem);
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
    list.erase(std::remove_if(list.begin(), list.end(), [id](std::unique_ptr<T>& item) {
        return item->getId() == id;
    }), list.end());
}

/**
 * @brief For each item in the list, call the given function.
 *
 * @tparam T
 * @param func   Function to call for each item in the list
 * @param filter Optional filter function to decide which items to include
 */
template <class T>
void BaseList<T>::forEach(std::function<void(T*)> func, std::function<bool(const T*)> filter) const
{
    for (auto &item : this->list)
    {
        T* ptr = item.get();
        if (!filter || filter(ptr)) {
            func(ptr);
        }
    }
}


/**
 * @brief Get an item from the list by its ID, casted to a specific interface.
 * 
 * @tparam I Interface type to cast to
 * @param id ID of the item
 * @return I* Pointer to the interface, or nullptr if not found / not castable
 */
template <class T>
template <class I>
I* BaseList<T>::getAs(uint64_t id) const
{
    for (auto &item : list)
    {
        if (item->getId() == id)
            return static_cast<I*>(item.get());
    }
    return nullptr;
}

/**
 * @brief Get all items that implement a specific interface, with optional filtering.
 *
 * @tparam I Interface type
 * @tparam F Filter type (defaults to T)
 * @param filter Optional filter function to decide which items to include
 * @return std::vector<I*> Non-owning pointers valid while items exist in the list
 */
template <class T>
template <class I, class F>
std::vector<I*> BaseList<T>::getAllAs(std::function<bool(const F*)> filter) const
{
    std::vector<I*> result;
    for (auto &item : list)
    {
        T* ptr = item.get();
        if (auto* casted = static_cast<I*>(ptr))
        {
            if (!filter) {
                result.push_back(casted);
            } else {
                // Compile-time branch: choose correct pointer type
                if constexpr (std::is_same_v<F, I>) {
                    if (filter(casted)) {
                        result.push_back(casted);
                    }
                } else if constexpr (std::is_same_v<F, T>) {
                    if (filter(ptr)) {
                        result.push_back(casted);
                    }
                } else {
                    static_assert(std::is_base_of_v<T, F> || std::is_base_of_v<I, F>,
                                  "Filter type F must be related to T or I");
                }
            }
        }
    }
    return result;
}
