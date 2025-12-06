#include "TempSensorService.hpp"

/**
 * @brief Initialize the instance of the TempSensorService to null.
 * 
 */
TempSensorService *TempSensorService::instance = nullptr;

/**
 * @brief Get the singleton instance of the TempSensorService.
 * 
 * @return TempSensorService* The singleton instance of the TempSensorService.
 */
TempSensorService *TempSensorService::getInstance()
{
    if (!instance)
        instance = new TempSensorService();
    return instance;
}

/**
 * @brief Construct a new TempSensorService object.
 * 
 */
TempSensorService::TempSensorService() : oneWireBus(15), sensors(&oneWireBus)
{
    restoreAll();
    sensors.begin();
}

/**
 * @brief It's not possible to create a sensor directly.
 * 
 * @param newItem The new sensor to create.
 */
void TempSensorService::create(TempSensorItem newItem)
{
    Serial.println("Error: Cannot create a sensor!");
}

/**
 * @brief update a sensor.
 * 
 * @param id id of the desired sensor.
 * @param newItem new sensor data.
 */
void TempSensorService::update(uint64_t id, TempSensorItem newItem)
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    if(registeredSensorList.doesExist(newItem))
        registeredSensorList.modifyItem(id, newItem);
    else
        registeredSensorList.addItem(newItem);
    storeAll();
}

/**
 * @brief Remove a sensor.
 * 
 * @param id id of the desired sensor.
 */
void TempSensorService::remove(uint64_t id)
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    registeredSensorList.deleteItem(id);
    storeAll();
}

/**
 * @brief Iterate over each sensor.
 * 
 * @param callback The callback function to call for each sensor.
 * @param onlyRegisteredSensors If true, only iterate over registered sensors.
 */
void TempSensorService::forEachSensor(std::function<void(TempSensorItem)> callback, bool onlyRegisteredSensors)
{
    TempSensorList list;
    if(onlyRegisteredSensors)
    {
        forEachSensor([this, &list](TempSensorItem item) // search for registered sensors in the live list
        { 
            if(registeredSensorList.doesExist(item)) 
                list.addItem(item); // add only sensors that have a registered name in "registeredSensorList"
        }, false);
    }
    else
        list = getCompleteList();

    for (auto item : list.getList())
    {
        callback(item);
    }
}

/**
 * @brief Request a temperature conversion from sensors.
 * 
 * @param doNotify If true, notify the observers when data is ready (through observer pattern).
 */
void TempSensorService::read(bool doNotify)
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    obtainSensors(); // update the list of connected sensors (liveSensorList)
    sensors.requestTemperatures();
    for (auto item : liveSensorList.getList()) // update the temperature data
    {
        auto temp = getTempFromSensor(item.getId());
        liveSensorList.getItem(item.getId()).setTemp(temp);
    }
    if (doNotify)
        notify(); // notify the observers when data is ready
}

/**
 * @brief Get the temperature from a sensor.
 * 
 * @param address The address(id) of the sensor.
 * @return double The temperature of the sensor.
 */
double TempSensorService::getTempFromSensor(uint64_t address)
{
    uint8_t formattedAddress[8];
    for (int i = 0; i < 8; ++i)
    {
        formattedAddress[i] = (address >> (i * 8)) & 0xFF;
    }
    if (address != 0)
        return sensors.getTempC(formattedAddress);
    else
        return -127; // an error code
}

/**
 * @brief Get temperature data by sensor name.
 * 
 * @param name The name of the sensor.
 * @return double The temperature of the sensor.
 */
double TempSensorService::getData(std::string name)
{
    auto list = getCompleteList();
    auto item = list.getItem(name);
    return item.getTemp();
}

/**
 * @brief Get temperature data by sensor id.
 * 
 * @param id The id of the sensor.
 * @return double The temperature of the sensor.
 */
double TempSensorService::getData(uint64_t id)
{
    auto list = getCompleteList();
    auto item = list.getItem(id);
    return item.getTemp();
}

/**
 * @brief Get a complete list of sensors (merged names from registered sensors into live sensors list).
 * 
 * @return TempSensorList The complete list of sensors.
 */
TempSensorList TempSensorService::getCompleteList() const
{
    auto completeList = liveSensorList; // take a copy of the live list
    mergeAndCopy(completeList, registeredSensorList); // copy registered sensor names to the live list
    return completeList;
}

/**
 * @brief Get the list of sensors in JSON format.
 * 
 * @return std::string The list of sensors in JSON format.
 */
std::string TempSensorService::getAll()
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    return getCompleteList().toJson();
}

/**
 * @brief Get a sensor in JSON format.
 * 
 * @return std::string The list of sensors in JSON format.
 */
std::string TempSensorService::get(uint64_t id)
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    return getCompleteList().getItem(id).toJson();
}

/**
 * @brief Scan for ds18b20 sensors on the 1wire bus.
 * 
 */
void TempSensorService::obtainSensors()
{
    oneWireBus.begin(15); // restart the bus
    oneWireBus.reset(); // reset the bus
    auto newSensorList = TempSensorList();
    byte addr[8]; // address buffer
    while (oneWireBus.search(addr)) // start the search (scan)
    {
        // for (uint8_t i = 0; i < 8; i++)
        // {
        //     Serial.print("0x");
        //     if (addr[i] < 0x10)
        //         Serial.print("0");
        //     Serial.print(addr[i], HEX);
        //     if (i < 7)
        //         Serial.print(", ");
        // }
        // Serial.println("\n an address found!");
        uint64_t addr64;
        std::memcpy(&addr64, addr, sizeof(addr64)); // convert the address to standard 64-bit format
        newSensorList.addItem(TempSensorItem(addr64, "", true)); // add the sensor to the list
        // Serial.println(devList.at(0).getAddress(), HEX);
    }
    liveSensorList = newSensorList; // update the live list
    oneWireBus.reset_search(); // finish the search (scan)
    // Serial.println("Sys-Ok: Obtaining sensors completed!");
}

/**
 * @brief Save the registered sensors in SPIFFS.
 * 
 */
void TempSensorService::storeAll()
{
    auto *cfg = Configuration::getInstance();
    for(auto item : registeredSensorList.getList())
    {
        registeredSensorList.getItem(item.getId()).setTemp(-127);
        registeredSensorList.getItem(item.getId()).setStatus(false);
    }
    cfg->setRegisteredTempSensorList(registeredSensorList.toJson());
}

/**
 * @brief Load the registered sensors from SPIFFS.
 * 
 */
void TempSensorService::restoreAll()
{
    auto *cfg = Configuration::getInstance();
    auto state = cfg->getRegisteredTempSensorList();
    if (state.empty())
        return;
    registeredSensorList.repopulateWith(state);
}

/**
 * @brief Merge two lists of TempSensorItem objects and copy the unique elements.
 * 
 * @param primary The primary list to merge into.
 * @param secondary The secondary list to merge from.
 */
void TempSensorService::mergeAndCopy(TempSensorList &primary, TempSensorList secondary)
{
    bool alreadyExist = false;
    for (auto sDev : secondary.getList())
    {
        alreadyExist = false;
        for (auto &pDev : primary.getList()) // Search and compare
        {
            if (pDev == sDev)
            {
                alreadyExist = true;
                primary.getItem(pDev.getId()).setName(sDev.getName()); // copy name
                primary.getItem(pDev.getId()).setLastLogTime(sDev.getLastLogTime()); // copy last log time
                primary.getItem(pDev.getId()).setLoggingEnabled(sDev.isLoggingEnabled());
                primary.getItem(pDev.getId()).setInterval(sDev.getInterval());
                primary.getItem(pDev.getId()).setLogOnlyOnChange(sDev.logOnlyOnChange());
            }
        }
        if (!alreadyExist)
            primary.addItem(sDev);
    }
}

/**
 * @brief Get the name of the TempSensorService.
 * 
 * @return std::string The name of the TempSensorService.
 */
std::string TempSensorService::getName() const
{
    return "TempSensor";
}

/**
 * @brief Get the list of loggable items (data logging enabled, connected and registered sensors).
 * 
 * @return std::vector<ILoggableItem*> The list of loggable items.
 */
std::vector<std::unique_ptr<ILoggableItem>> TempSensorService::getLoggableItems() const
{
    std::vector<std::unique_ptr<ILoggableItem>> loggableItems;
    for(const auto item : getCompleteList().getList())
    {
        if(item.isConnected() and !item.getName().empty() and item.isLoggingEnabled()) // only add if sensor is connected and has a name (registered)
            loggableItems.push_back(std::make_unique<TempSensorItem>(item)); // add to list
    }
    return loggableItems;
}

/**
 * @brief Set the last log time for a loggable item.
 * 
 * @param itemId The ID of the loggable item.
 * @param time The last log time to set.
 */
void TempSensorService::setLastLogTime(uint64_t itemId, std::chrono::time_point<std::chrono::system_clock> time)
{
    registeredSensorList.getItem(itemId).setLastLogTime(time);
}
