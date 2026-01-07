#include "TempSensorService.hpp"
#include <driver/uart.h>

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
TempSensorService::TempSensorService() : oneWireBus(config.getSensorPin()), sensors(&oneWireBus), fSerial(1)
{
    restoreAll();
    sensors.begin();
}

/**
 * @brief It's not possible to create a sensor directly.
 * 
 * @param newItem The new sensor to create.
 */
void TempSensorService::create(std::unique_ptr<TempSensorItem> newItem)
{
    Serial.println("Error: Cannot create a sensor!");
}

/**
 * @brief update a sensor.
 * 
 * @param id id of the desired sensor.
 * @param newItem new sensor data.
 */
void TempSensorService::update(uint64_t id, std::unique_ptr<TempSensorItem> newItem)
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    if(sensorList.getItem(id)) // if item exists
        sensorList.modifyItem(id, std::move(newItem));
    else
        sensorList.addItem(std::move(newItem));
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
    sensorList.deleteItem(id);
    storeAll();
}

/**
 * @brief Iterate over each sensor.
 * 
 * @param callback The callback function to call for each sensor.
 * @param onlyRegisteredSensors If true, only iterate over registered sensors.
 */
void TempSensorService::forEachSensor(std::function<void(const TempSensorItem *)> callback, bool onlyRegisteredSensors)
{
    sensorList.forEach([&callback](TempSensorItem * item)
    {
        callback(item);
    }, [onlyRegisteredSensors](const TempSensorItem * item) -> bool
    {
        return (!item->getName().empty() or !onlyRegisteredSensors);
    });
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
    sensorList.forEach([this](TempSensorItem *item) -> void 
    {
        auto temp = getTempFromSensor(item->getId());
        item->setTemp(temp);
    });
    if (doNotify)
        notify(); // notify the observers when data is ready
    delay(500);
    fSerial.begin(38400, SERIAL_8N1, -1, 15); // tx only
    uart_set_mode(UART_NUM_1, UART_MODE_RS485_HALF_DUPLEX); // tweak serial to become halfduplex opendrain
    delay(100);
    fSerial.println("FusionBusPair");
    fSerial.flush(); // wait for full transmition
    fSerial.end(); // end Tx
    fSerial.begin(38400, SERIAL_8N1, 15, -1); // begin rx
    fSerial.read(); // flush out the initial null terminator byte (0x00)
    delay(100); // wait for response
    std::string rawResponse;
    while(fSerial.available())
        rawResponse += static_cast<char>(fSerial.read()); // receive slave response bytes
    fSerial.end(); // end rx
    for (unsigned char c : rawResponse) 
    {
        std::cout << std::hex << (int)c << ' ';
    }
    std::cout << std::endl;

    std::cout << "raw response:" << rawResponse << std::endl;
    std::cout << "raw response.c_str():" << rawResponse.c_str() << std::endl;
    // rawResponse = "{\"UUID\": 123456789}"; 
    JsonDocument doc;
    if(deserializeJson(doc, rawResponse.c_str()) == DeserializationError::Ok) // if response is a valid json
    {
        std::cout << "json valid!" << std::endl;
        if(doc.containsKey("UUID"))
        {
            auto uuid = doc["UUID"].as<uint32_t>(); // Extract uuid parameter
            std::cout << "FusionBus Device Found:" << std::to_string(uuid) << std::endl;
            if(liveUartList.getItem(uuid)) // if item ain't already present
                this->liveUartList.addItem(std::make_unique<BaseItem>(uuid, "", true)); // add device to live list
        }
    }
    else 
        std::cout << "Deserialization failed of this rawResponse.c_str(): " << rawResponse.c_str() << std::endl;
    delay(200); // wait for slave stablization

    auto checkPresences = [&]() -> void
    {
        for(const auto& device : liveUartList.getList())
        {
            fSerial.begin(38400, SERIAL_8N1, -1, 15); // tx only
            delay(100); // wait for Tx stablization
            JsonDocument doc;
            std::string txStr;
            doc["UUID"] = device->getId();
            serializeJson(doc, txStr);
            fSerial.println(("FusionBusCommunicate" + txStr).c_str());
            fSerial.flush(); // wait for full transmition
            fSerial.end(); // end Tx

            fSerial.begin(38400, SERIAL_8N1, 15, -1); // begin Rx
            fSerial.read(); // flush out the initial null terminator byte (0x00)
            delay(500); // wait for response
            std::string rawResponse;
            while(fSerial.available())
                rawResponse += static_cast<char>(fSerial.read()); // receive slave response bytes
            fSerial.end(); // end Rx
            std::cout << "raw presense check response:" << rawResponse << std::endl;
            JsonDocument docRx;
            if(!deserializeJson(docRx, rawResponse)) // if response is a valid json
            {
                std::cout << "FusionBus Device" << std::to_string(device->getId()) << " is present!!!" << std::endl;
            }
            else
                liveUartList.deleteItem(device->getId());
        }
    };
    checkPresences();
    pinMode(15, OUTPUT_OPEN_DRAIN);
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
    if(auto item = sensorList.getItem(name))
        return item->getTemp();
    return -127;
}

/**
 * @brief Get temperature data by sensor id.
 * 
 * @param id The id of the sensor.
 * @return double The temperature of the sensor (-127 on not found or failure).
 */
double TempSensorService::getData(uint64_t id)
{
    if(auto item = sensorList.getItem(id))
        return item->getTemp();
    return -127;
}

/**
 * @brief Get the list of sensors in JSON format.
 * 
 * @return std::string The list of sensors in JSON format.
 */
std::string TempSensorService::getAll()
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    return sensorList.toJson();
}

/**
 * @brief Get a sensor in JSON format.
 * 
 * @return std::string The list of sensors in JSON format.
 */
std::string TempSensorService::get(uint64_t id)
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    return sensorList.toJson();
}

/**
 * @brief Scan for ds18b20 sensors on the 1wire bus.
 * 
 */
void TempSensorService::obtainSensors()
{
    oneWireBus.begin(config.getSensorPin()); // restart the bus
    oneWireBus.reset(); // reset the bus
    sensorList.forEach([this](TempSensorItem *item) -> void 
    {
        item->setStatus(false); // reset the connection status of all items to false 
        
        if(item->getName().empty()) // if not registered
            sensorList.deleteItem(item->getId()); // delete the non-registered item from the list
    });
    byte addr[8]; // address buffer
    while (oneWireBus.search(addr)) // start the search (scan)
    {
        uint64_t addr64;
        std::memcpy(&addr64, addr, sizeof(addr64)); // convert the address to standard 64-bit format
        if(auto item = sensorList.getItem(addr64)) // if sensor already exist
            item->setStatus(true); // update connection status
        else
            sensorList.addItem(std::make_unique<TempSensorItem>(addr64, "", true)); // add the sensor to the list
        // Serial.println(devList.at(0).getAddress(), HEX);
    }

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
    cfg->setRegisteredTempSensorList(sensorList.toJson([](const TempSensorItem *item) -> bool
    {
        return !(item->getName().empty()); // filter out non-registered items (items without a name)
    }));
    cfg->setTempSensorConfig(config.toJson());
}

/**
 * @brief Load the registered sensors from SPIFFS.
 * 
 */
void TempSensorService::restoreAll()
{
    auto *cfg = Configuration::getInstance();

    auto state = cfg->getRegisteredTempSensorList();
    if (!state.empty())
        sensorList.repopulateWith(state);

    auto cfgJson = cfg->getTempSensorConfig();
    if (!cfgJson.empty())
        config.populateFromJson(cfgJson);
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
std::vector<ILoggableItem *> TempSensorService::getLoggableItems() const
{
    return sensorList.getAllAs<ILoggableItem, TempSensorItem>([](const TempSensorItem * item) -> bool
    {
        return (item->isConnected() and !item->getName().empty() and item->isLoggingEnabled()); // only add if sensor is connected and has a name (registered)
    });
}

/**
 * @brief Get the temperature sensor configuration in JSON format.
 * 
 * @return std::string The temperature sensor configuration in JSON format.
 */
std::string TempSensorService::getConfig()
{
    return config.toJson();
}

/**
 * @brief Update the temperature sensor configuration.
 * 
 * @param config The new temperature sensor configuration.
 */
void TempSensorService::updateConfig(TempSensorConfig config)
{
    this->config = config;
    storeAll();
}
