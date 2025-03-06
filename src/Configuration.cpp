#include "Configuration.hpp"
#include <SPIFFS.h>

Configuration* Configuration::instance = nullptr;

Configuration::Configuration() {}

Configuration* Configuration::getInstance()
{
    if (!instance)
        instance = new Configuration();
    return instance;
}

void Configuration::setWifiCredentials(WifiHotspotData data)
{
    File file = SPIFFS.open(wifiFileAddress, FILE_WRITE);
    if (file)
    {
        file.println(data.toJsonString().c_str());
        file.close();
        Serial.println("wifi credential data saved successfully.");
    } 
    else 
    {
        Serial.println("Failed to open file for writing.");
    }
}
void Configuration::setHotspotCredentials(WifiHotspotData data)
{
    File file = SPIFFS.open(hotspotFileAddress, FILE_WRITE);
    if (file)
    {
        file.println(data.toJsonString().c_str());
        file.close();
        Serial.println("hotspot credential data saved successfully.");
    } 
    else 
    {
        Serial.println("Failed to open file for writing.");
    }
}

WifiHotspotData Configuration::getWifiCredentials()
{
    auto file = SPIFFS.open(wifiFileAddress, FILE_READ);
    if(file)
    {
        auto json = file.readString(); // read raw data from file
        file.close();
        return WifiHotspotData(json.c_str());
    }
    return WifiHotspotData(); // return empty
}
WifiHotspotData Configuration::getHotspotCredentials()
{
    auto file = SPIFFS.open(hotspotFileAddress, FILE_READ);
    if(file)
    {
        auto json = file.readString(); // read raw data from file
        file.close();
        return WifiHotspotData(json.c_str());
    }
    return WifiHotspotData(); // return empty
}

void Configuration::setSchedulerList(std::string json)
{
    //auto schedulerList = SchedulerList(json, length);
    File file = SPIFFS.open(pumpFileAddress, FILE_WRITE);
    if (file)
    {
        file.println(json.c_str());
        file.close();
        Serial.println("scheduling data saved successfully.");
    } 
    else 
    {
        Serial.println("Failed to open file for writing.");
    }
}
std::string Configuration::getSchedulerList()
{
    auto file = SPIFFS.open(pumpFileAddress, FILE_READ);
    std::string json;
    if(file)
    {
        json = file.readString().c_str(); // read raw data from file
        file.close();
    }
    return json;
}
void Configuration::setFirebaseData(FBData data)
{
    File file = SPIFFS.open(firebaseDataFileAddress, FILE_WRITE);
    if (file)
    {
        file.println(data.toJsonString().c_str());
        file.close();
        Serial.println("firebase data saved successfully.");
    } 
    else 
    {
        Serial.println("Failed to open file for writing.");
    }
}
FBData Configuration::getFirebaseData()
{
    auto file = SPIFFS.open(firebaseDataFileAddress, FILE_READ);
    if(file)
    {
        auto json = file.readString(); // read raw data from file
        auto firebaseData = FBData(json.c_str());
        file.close();
        return firebaseData;
    }
    return FBData();
}
    

void Configuration::update(SystemTime *systemTime)
{
    currentTime = systemTime->getTime();
    currentWeekday = systemTime->getWeekday();
}

std::string Configuration::getEventList() 
{
    File file = SPIFFS.open("/eventList.txt", FILE_READ);
    if (!file) {
        Serial.println("Failed to open state file for reading");
        return "";
    }

    std::string state = file.readString().c_str();
    file.close();
    return state;
}

void Configuration::setEventList(const std::string& state) 
{
    File file = SPIFFS.open("/eventList.txt", FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open state file for writing");
        return;
    }

    file.print(state.c_str());
    file.close();
}

void Configuration::setGPIOList(std::string json)
{
    File file = SPIFFS.open(gpioFileAddress, FILE_WRITE);
    if (file)
    {
        file.println(json.c_str());
        file.close();
        Serial.println("GPIO data saved successfully.");
    } 
    else 
    {
        Serial.println("Failed to open file for writing.");
    }
}

std::string Configuration::getGPIOList()
{
    auto file = SPIFFS.open(gpioFileAddress, FILE_READ);
    std::string jsonData;
    if (file)
    {
        jsonData = file.readString().c_str(); // read raw data from file
        file.close();
        return jsonData;
    }
    return ""; // return empty
}

void Configuration::setDisplayConfig(const std::string& config)
{
    File file = SPIFFS.open(displayFileAddress, FILE_WRITE);
    if (file)
    {
        file.println(config.c_str());
        file.close();
        Serial.println("Display data saved successfully.");
    } 
    else 
    {
        Serial.println("Failed to open file for writing.");
    }
}

std::string Configuration::getDisplayConfig()
{
    auto file = SPIFFS.open(displayFileAddress, FILE_READ);
    std::string jsonData;
    if (file)
    {
        jsonData = file.readString().c_str(); // read raw data from file
        file.close();
        return jsonData;
    }
    return ""; // return empty
}

std::string Configuration::getThermostatList() 
{
    File file = SPIFFS.open(thermostatFileAddress, FILE_READ);
    if (!file) {
        Serial.println("Failed to open state file for reading");
        return "";
    }

    std::string state = file.readString().c_str();
    file.close();
    return state;
}

void Configuration::setThermostatList(const std::string& state) 
{
    File file = SPIFFS.open(thermostatFileAddress, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open state file for writing");
        return;
    }

    file.print(state.c_str());
    file.close();
}

std::string Configuration::getRegisteredTempSensorList()
{
    auto file = SPIFFS.open(registeredSensorFileAddress, FILE_READ);
    std::string jsonData;
    if (file)
    {
        jsonData = file.readString().c_str(); // read raw data from file
        file.close();
        return jsonData;
    }
    return ""; // return empty
}

void Configuration::setRegisteredTempSensorList(const std::string& json)
{
    File file = SPIFFS.open(registeredSensorFileAddress, FILE_WRITE);
    if (file)
    {
        file.println(json.c_str());
        file.close();
        Serial.println("Registered sensor data saved successfully.");
    } 
    else 
    {
        Serial.println("Failed to open file for writing.");
    }
}