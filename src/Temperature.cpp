#include "Temperature.hpp"

// Initialize the static member variable
Temperature *Temperature::instance = nullptr;

// Singleton instance getter
Temperature *Temperature::getInstance()
{
    if (!instance)
        instance = new Temperature();
    return instance;
}

// Constructor
Temperature::Temperature() : oneWireBus(15), sensors(&oneWireBus)
{
    loadState();
    sensors.begin();
}

// Modify a sensor
void Temperature::modify(uint64_t id, TempSensorItem newItem)
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    if(registeredSensorList.doesExist(newItem))
        registeredSensorList.modifyItem(id, newItem);
    else
        registeredSensorList.addItem(newItem);
    saveState();
}

// Remove a sensor
void Temperature::remove(uint64_t id)
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    registeredSensorList.deleteItem(id);
    saveState();
}

void Temperature::forEachSensor(std::function<void(TempSensorItem)> callback, bool onlyRegisteredSensors)
{
    TempSensorList list;
    if(onlyRegisteredSensors)
        list = registeredSensorList;
    else
        list = getCompleteList();

    for (auto item : list.getList())
    {
        callback(item);
    }
}

// Read temperature data from sensors
void Temperature::read(bool doNotify)
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

double Temperature::getTempFromSensor(uint64_t address)
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

// Get temperature data by sensor name
double Temperature::getData(std::string name)
{
    auto list = getCompleteList();
    auto item = list.getItem(name);
    return item.getTemp();
}
// Get temperature data by sensor address
double Temperature::getData(uint64_t id)
{
    auto list = getCompleteList();
    auto item = list.getItem(id);
    return item.getTemp();
}

TempSensorList Temperature::getCompleteList()
{
    auto completeList = liveSensorList; // take a copy of the live list
    mergeAndCopy(completeList, registeredSensorList); // copy registered sensor names to the live list
    return completeList;
}

// get the list in json format
std::string Temperature::getListJson()
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    Serial.println(getCompleteList().getItem(721432011450663464).getTemp());
    return getCompleteList().toJson();
}

// Obtain the list of sensors connected to the bus
void Temperature::obtainSensors()
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

void Temperature::saveState()
{
    auto *cfg = Configuration::getInstance();
    for(auto item : registeredSensorList.getList())
    {
        registeredSensorList.getItem(item.getId()).setTemp(-127);
        registeredSensorList.getItem(item.getId()).setStatus(false);
    }
    cfg->setRegisteredTempSensorList(registeredSensorList.toJson());
}

void Temperature::loadState()
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
void Temperature::mergeAndCopy(TempSensorList &primary, TempSensorList secondary)
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
            }
        }
        if (!alreadyExist)
            primary.addItem(sDev);
    }
}
