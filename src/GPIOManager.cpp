#include "GPIOManager.hpp"

/**
 * @brief initialize the instance of the GPIOManager to null
 * 
 */
GPIOManager* GPIOManager::instance = nullptr;

/**
 * @brief Construct a new GPIOManager::GPIOManager object
 * 
 */
GPIOManager::GPIOManager()
{
    loadState();
}

/**
 * @brief Destroy the GPIOManager object.
 * 
 */
GPIOManager::~GPIOManager()
{
    saveState();
}

/**
 * @brief Get the instance of the GPIOManager (singleton pattern).
 * 
 * @return GPIOManager* The instance of the GPIOManager.
 */
GPIOManager* GPIOManager::getInstance()
{
    if (!instance)
    {
        instance = new GPIOManager();
    }
    return instance;
}

/**
 * @brief Create a new GPIO item and add it to the GPIO list.
 * 
 * @param newItem The new GPIO item to add.
 */
void GPIOManager::createIO(GPIOItem newItem)
{
    list.addItem(newItem);
    saveState();
}

/**
 * @brief Remove a GPIO item from the GPIO list.
 * 
 * @param id The ID of the GPIO item to remove.
 */
void GPIOManager::removeIO(int id)
{
    list.deleteItem(id);
    saveState();
}

/**
 * @brief Modify a GPIO item in the GPIO list.
 * 
 * @param id The ID of the GPIO item to modify.
 * @param newItem The new GPIO item to replace the old one.
 */
void GPIOManager::modifyIO(int id, GPIOItem newItem)
{
    list.modifyItem(id, newItem);
    saveState();
}

/**
 * @brief Modify the status of a GPIO item in the GPIO list.
 * 
 * @param id The ID of the GPIO item to modify.
 * @param status The new status of the GPIO item.
 */
void GPIOManager::modifyIOStatus(int id, bool status)
{
    GPIOItem& item = list.getItem(id);
    if (item.getId() != -1) // Check if the item exists
    {
        item.setStatus(status);
        saveState();
    }
}

/**
 * @brief Modify the extra parameters of a GPIO item in the GPIO list.
 * 
 * @param id The ID of the GPIO item to modify.
 * @param extraParameters The new extra parameters of the GPIO item.
 */
void GPIOManager::modifyIOExtraParameters(int id, std::string extraParameters)
{
    GPIOItem& item = list.getItem(id);
    if (item.getId() != -1) // Check if the item exists
    {
        item.setExtraParameters(extraParameters);
        saveState();
    }
}

/**
 * @brief Get the GPIO list in JSON format.
 * 
 * @return std::string The GPIO list in JSON format.
 */
std::string GPIOManager::getGPIOListJson()
{
    return list.toJson();
}

/**
 * @brief Save the GPIO list to the configuration and update the hardware.
 * 
 */
void GPIOManager::saveState()
{
    Configuration::getInstance()->setGPIOList(list.toJson());
    syncHardware();
}

/**
 * @brief Load the GPIO list from the configuration.
 * 
 */
void GPIOManager::loadState()
{
    auto state = Configuration::getInstance()->getGPIOList();
    if (state.empty())
        return;
    list.repopulateWith(state.c_str());
    syncHardware();
}

/**
 * @brief sync the actual GPIO pins to the status of the items and vice versa.
 * 
 */
void GPIOManager::syncHardware()
{
    for (auto& item : list.getList())
    {
        if (item.getMode() == 1) // if the item is an output pin
        {
            pinMode(item.getPin(), OUTPUT);
            digitalWrite(item.getPin(), item.getStatus()); // update the pin from item status
        } else // if the item is an input pin
        {
            pinMode(item.getPin(), INPUT);
            item.setStatus(digitalRead(item.getPin())); // update the status of the item from pin
        }
    }
}

/**
 * @brief Update the GPIOManager when the EventManager changes.
 * 
 * @param eventManager The EventManager that changed.
 */
void GPIOManager::update(EventManager* eventManager)
{
    for (auto& item : list.getList())
    {
        if(item.getEventId() != -1) // if the pin is associated with an event.
        {
            bool flag = false;
            if (eventManager->hasEventFlagChanged(item.getEventId(), flag)) // if the event flag has changed update the status of the item
            {
                list.getItem(item.getId()).setStatus(flag); // use reference to set the status of the actual item.
            }
        }
    }
    syncHardware();
}