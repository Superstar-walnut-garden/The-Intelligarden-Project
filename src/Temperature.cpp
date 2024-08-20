#include "Temperature.hpp"
#include "TempSensorNode.hpp"

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
    sensors.begin();
}

// Read temperature data from sensors
void Temperature::read(bool doNotify)
{
    sensors.requestTemperatures();
    if (doNotify)
        notify(); // notify the observers when data is ready
}

// Get temperature data by sensor name
double Temperature::getData(std::string name)
{
    auto *cfg = Configuration::getInstance();
    auto list = cfg->getSensorList();
    auto node = TempSensorNode::findByName(list, name);
    uint8_t formattedAddress[8];
    auto address = node.getAddress();
    for (int i = 0; i < 8; ++i)
    {
        formattedAddress[i] = (address >> (i * 8)) & 0xFF;
    }
    if (address != 0)
        return sensors.getTempC(formattedAddress);
    else
        return -128; // an error code
}

// Get the list of connected sensors
auto Temperature::getSensorList()
{
    return sensorList;
}

// Update the configuration with sensors
void Temperature::update(Configuration *cfg)
{
    Serial.println("notified");
    cfg->setSensorList(obtainSensors());
}

// Obtain the list of sensors connected to the bus
std::vector<TempSensorNode> Temperature::obtainSensors()
{
    oneWireBus.begin(15);
    oneWireBus.reset();
    std::vector<TempSensorNode> devList;
    byte addr[8];
    Serial.println("obtaining sensors");
    while (oneWireBus.search(addr))
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            Serial.print("0x");
            if (addr[i] < 0x10)
                Serial.print("0");
            Serial.print(addr[i], HEX);
            if (i < 7)
                Serial.print(", ");
        }
        Serial.println("\n an address found!");
        uint64_t addr64;
        std::memcpy(&addr64, addr, sizeof(addr64));
        devList.push_back(TempSensorNode(addr64, "", true));
        Serial.println(devList.at(0).getAddress(), HEX);
    }
    oneWireBus.reset_search();
    Serial.println("obtaining sensors completed");
    return devList;
}
