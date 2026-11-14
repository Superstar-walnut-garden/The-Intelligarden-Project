#include "TempSensorService.hpp"
#include <driver/uart.h>
#include "../EnumCrafter.hpp"
#include "VentDriveItem.hpp"
#include "CustomMathUtils.hpp"
#include "esp_rom_gpio.h"
#include "soc/uart_periph.h"
#include "driver/uart.h"

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
void TempSensorService::create(std::string json)
{
    Serial.println("Error: Cannot create a sensor!");
}

/**
 * @brief update a sensor.
 * 
 * @param id id of the desired sensor.
 * @param newItem new sensor data.
 */
void TempSensorService::update(uint64_t id, std::string json)
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    auto fusionItem = TempSensorList::createObjectFromType(json);
    fusionItem->populateFromJson(json);
    
    if(sensorList.getItem(id)) // if item exists
        sensorList.modifyItem(id, std::move(fusionItem));
    else
        sensorList.addItem(std::move(fusionItem));
    storeAll();
    Subject<ISignalCompatibleService>::notify(); // notify the centralized signal hub for item update/removal
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
    Subject<ISignalCompatibleService>::notify(); // notify the centralized signal hub for item update/remova
}

/**
 * @brief Iterate over each sensor.
 * 
 * @param callback The callback function to call for each sensor.
 * @param onlyRegisteredSensors If true, only iterate over registered sensors.
 */
void TempSensorService::forEachSensor(std::function<void(const TempSensorItem *)> callback, bool onlyRegisteredSensors)
{
    sensorList.forEach([&callback](FusionBusItem * item)
    {
        if(item->getType() == FusionBusItem::DeviceType::TempSensor)
            callback(static_cast<TempSensorItem*>(item));
    }, [onlyRegisteredSensors](const FusionBusItem * item) -> bool
    {
        return (!item->getName().empty() or !onlyRegisteredSensors);
    });
}

/**
 * @brief handle alll devices and requests
 * 
 * @param doNotify If true, notify the temp sensor observers when data is ready (through observer pattern).
 */
void TempSensorService::loop(bool doNotify)
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    obtainOneWireDevices(); // update the list of connected onewire devices (ds18b20 sensors)
    handleOneWireDevices(); // retrive temperature data from sensors
    delay(10); // wait for bus stablization
    obtainUartDevices();
    delay(10); // wait for slave stablization
    handleUartDevices();
    if (doNotify)
        Subject<TempSensorService>::notify(); // notify the observers when data is ready
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
 * @brief Get temperature data by sensor id.
 * 
 * @param id The id of the sensor.
 * @return double The temperature of the sensor (-127 on not found or failure).
 */
double TempSensorService::getData(uint64_t id)
{
    if(auto item = sensorList.getAs<TempSensorItem>(id))
    {
        return item->getTemp();
    }
    return -127;
}

/**
 * @brief Get the list of sensors in JSON format.
 * 
 * @return std::string The list of sensors in JSON format.
 */
std::string TempSensorService::getAll() const
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    return sensorList.toJson();
}

/**
 * @brief Get a sensor in JSON format.
 * 
 * @return std::string The list of sensors in JSON format.
 */
std::string TempSensorService::get(uint64_t id) const
{
    std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
    return sensorList.toJson();
}

/**
 * @brief get temperature data from ds18b20 temp sensors on onewire bus
 * 
 */
void TempSensorService::handleOneWireDevices()
{
    sensors.requestTemperatures();
    sensorList.forEach([this](FusionBusItem *item) -> void 
    {
        if(item->getType() == FusionBusItem::DeviceType::TempSensor)
        {
            auto castedItem = static_cast<TempSensorItem*>(item);
            auto temp = getTempFromSensor(castedItem->getId());
            castedItem->setTemp(temp);
        }
    });
}

/**
 * @brief Handle uart devices and update their connection status
 * 
 */
void TempSensorService::handleUartDevices()
{
    std::vector<uint64_t> pendingRemoves;
    fSerial.begin(38400, SERIAL_8N1, -1, 15);
    uart_set_hw_flow_ctrl(UART_NUM_1, UART_HW_FLOWCTRL_DISABLE, 0);
    uart_set_mode(UART_NUM_1, UART_MODE_RS485_HALF_DUPLEX);
    gpio_set_direction(static_cast<gpio_num_t>(15), GPIO_MODE_INPUT_OUTPUT_OD);
    esp_rom_gpio_connect_out_signal(15, UART_PERIPH_SIGNAL(UART_NUM_1, SOC_UART_TX_PIN_IDX), false, false);
    esp_rom_gpio_connect_in_signal(15, UART_PERIPH_SIGNAL(UART_NUM_1, SOC_UART_RX_PIN_IDX), false);
    if(fSerial.available()) fSerial.read(); // flush out rx garbage
    delay(100); // wait for Tx stablization
    sensorList.forEach([&](FusionBusItem *device) 
    {
        Serial.println(("------- item type ======>> " + std::string(EnumCrafter::toString(device->getType()))).c_str());
        if(device->getType() != FusionBusItem::DeviceType::TempSensor) // if it wasn't temp sensor (wasn't on onewire bus)
        {
            JsonDocument doc;
            std::string txStr;
            doc["id"] = device->getId();
            device->appendResponse(doc);
            serializeJson(doc, txStr);
            fSerial.println(("FusionBusCommunicate" + txStr).c_str());
            fSerial.flush(); // wait for full transmition
            delay(250); // wait for response
            std::string rawResponse;
            bool dataStarted = false;
            while(fSerial.available())
            {
                auto data = static_cast<char>(fSerial.read());
                if(!dataStarted && data != 0x00) // ignore the initial null-byte garbages
                    dataStarted = true;
                if(dataStarted)
                    rawResponse += data; // receive slave response bytes
            }
            std::cout << "raw response :" << rawResponse << std::endl;
            JsonDocument docRx;
            if(!deserializeJson(docRx, rawResponse)) // if response is a valid json
            {
                std::cout << "FusionBus Device" << std::to_string(device->getId()) << " is present!!!" << std::endl;
                device->setStatus(true); // set connection status to true
                if(device->getType() == FusionBusItem::DeviceType::VentDrive)
                {
                    auto castedDevice = sensorList.getAs<VentDriveItem>(device->getId());
                    if(auto state = EnumCrafter::parse<VentDriveItem::State>(docRx["state"]))
                    {
                        if(castedDevice->isAutoTempControlEnabled()) // override venting percent if auto-temp-control is enabled
                        {
                            auto temp = getData(castedDevice->getSensor());
                            if(temp != -127) // trying to implement hystresis
                            {
                                auto prevTemp = CustomMathUtils::map(castedDevice->getVentingPercent(), 0, 100,  castedDevice->getCloseStateTemp(), castedDevice->getOpenStateTemp());
                                auto ventingPercent = CustomMathUtils::map(temp, castedDevice->getCloseStateTemp(), castedDevice->getOpenStateTemp(), 0, 100);
                                auto tempDiff = std::abs(prevTemp - temp);
                                if((tempDiff >= (castedDevice->getHysteresis() / 2.00)) or ventingPercent == 100 or ventingPercent == 0)
                                    castedDevice->setVentingPercent(ventingPercent);
                            }
                        }
                        castedDevice->setCurrentState(state.value());
                        if(!docRx["ventingPercent"].isNull())
                            castedDevice->setCurrentVentingPercent(docRx["ventingPercent"]);
                        else
                            castedDevice->setCurrentVentingPercent(std::nullopt);
                        auto isUninitialized = (state.value() == VentDriveItem::State::Uninitialized);
                        castedDevice->addResponseApender([isUninitialized, castedDevice](JsonDocument &json) -> void
                        {
                            json["length"] = castedDevice->getLength();
                            json["acceleration"] = castedDevice->getAcceleration();
                            json["speed"] = castedDevice->getSpeed();
                            json["stepPermm"] = castedDevice->getStepPermm();
                            json["endstopExtraDistance"] = castedDevice->getEndstopExtraDistance();
                            json["maxCompensation"] = castedDevice->getMaxCompensation();
                            json["ventingPercent"] = castedDevice->getVentingPercent();
                            json["invertDir"] = castedDevice->isDirInverted();
                            json["invertEndstopPin"] = castedDevice->isEndstopPinInverted();

                            if(isUninitialized or castedDevice->getAutoHomeFlag())
                            {
                                json["autoHomeFlag"] = true;
                                castedDevice->dropAutoHomeFlag(); // drop flag
                            }
                        });
                    }
                }
            }
            else
            {
                device->setStatus(false); // set connection status to false
                if(device->getName().empty()) // if not registered (don't have a name)
                    pendingRemoves.push_back(device->getId());
            }
        }
    });
    sensorList.deleteItemIf([pendingRemoves](std::unique_ptr<FusionBusItem>& item) 
    {   // delete unregistered devices that didn't responded (become disconnected)
        for(const auto& id : pendingRemoves)
        {
            if(id == item->getId())
                return true;
        }
        return false;
    });
    fSerial.end(); // end Rx
    pinMode(15, OUTPUT_OPEN_DRAIN); // switch back to opendrain for onewire bus
}

/**
 * @brief Scan for a discoverable device on the uart bus to pair with (one device at a time).
 * 
 */
void TempSensorService::obtainUartDevices()
{
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
    // rawResponse = "{\"id\": 123456789}"; 
    JsonDocument doc;
    if(deserializeJson(doc, rawResponse.c_str()) == DeserializationError::Ok) // if response is a valid json
    {
        std::cout << "json valid!" << std::endl;
        if(doc.containsKey("id") and doc.containsKey("type"))
        {
            auto id = doc["id"].as<uint32_t>(); // Extract id parameter
            auto devType = EnumCrafter::parse<FusionBusItem::DeviceType>(doc["type"].as<std::string>()).value_or(FusionBusItem::DeviceType::Unknown);
            std::cout << "FusionBus Device Found:" << std::to_string(id) << ", type: " << EnumCrafter::toString(devType) << std::endl;
            if(!sensorList.getItem(id)) // if item ain't already present
            {
                std::unique_ptr<FusionBusItem> item = TempSensorList::createObjectFromType(rawResponse);
                item->setId(id);
                item->setStatus(true);
                this->sensorList.addItem(std::move(item)); // add device to live list
            }
        }
    }
    else 
        std::cout << "Deserialization failed of this rawResponse.c_str(): " << rawResponse.c_str() << std::endl;
    pinMode(15, OUTPUT_OPEN_DRAIN); // switch back to opendrain for onewire operation
}

/**
 * @brief Scan for ds18b20 sensors on the 1wire bus.
 * 
 */
void TempSensorService::obtainOneWireDevices()
{
    oneWireBus.begin(config.getSensorPin()); // restart the bus
    oneWireBus.reset(); // reset the bus
    sensorList.forEach([this](FusionBusItem *item) -> void 
    {
        item->setStatus(false); // reset the connection status of all items to false 
    }, [](const FusionBusItem *item) { return (item->getType() == FusionBusItem::DeviceType::TempSensor); }); // only iterate on temp sensors
    sensorList.deleteItemIf([](std::unique_ptr<FusionBusItem>& item) 
    {   // delete unregistered sensors to allow redescovery if still present on the bus
        return item->getName().empty() and (item->getType() == FusionBusItem::DeviceType::TempSensor);
    });
    byte addr[8]; // address buffer
    while (oneWireBus.search(addr)) // start the search (scan)
    {
        uint64_t addr64;
        std::memcpy(&addr64, addr, sizeof(addr64)); // convert the address to standard 64-bit format
        if(auto item = sensorList.getItem(addr64)) // if sensor already exist
            item->setStatus(true); // update connection status
        else
            sensorList.addItem(std::move(std::make_unique<TempSensorItem>(addr64, "", true))); // add the sensor to the list
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
    cfg->setRegisteredTempSensorList(sensorList.toJson([](const FusionBusItem *item) -> bool
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
    return "FusionBus";
}

/**
 * @brief Get the list of loggable items (data logging enabled, connected and registered devices).
 * 
 * @return std::vector<ILoggableItem*> The list of loggable items.
 */
std::vector<ILoggableItem *> TempSensorService::getLoggableItems() const
{
    return sensorList.getAllAs<ILoggableItem, FusionBusItem>([](const FusionBusItem * item) -> bool
    {
        return (item->isLoggingEnabled() and item->getStatus() and !item->getName().empty()); // only add if sensor is connected and has a name (registered)
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
