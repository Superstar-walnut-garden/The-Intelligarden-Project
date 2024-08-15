#include "Configuration.hpp"

Configuration* Configuration::instance = nullptr;

Configuration::Configuration() {}

Configuration* Configuration::getInstance()
{
    if (!instance)
        instance = new Configuration();
    return instance;
}

void Configuration::retriveSavedSensorList()
{
    File file = SPIFFS.open(sensorFileAddress, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    savedDevList.clear();
    while (file.available()) {
        String line = file.readStringUntil('\n');
        uint64_t address;
        char name[100];
        sscanf(line.c_str(), "%llu,%99s", &address, name);
        savedDevList.push_back(TempSensorNode(address, std::string(name)));
    }

    file.close();
    Serial.println("Data read from SPIFFS");
}

void Configuration::setPumpSchedule(Time start, Time duration)
{
    setPumpSchedule(String(start.getHour()) + ":" + String(start.getMinute()), 
                    String(duration.getHour()) + ":" + String(duration.getMinute()));
}

void Configuration::setPumpSchedule(String start, String duration)
{
    File file = SPIFFS.open(pumpFileAddress, FILE_WRITE);
    if (file)
    {
        file.println(start);
        file.println(duration);
        file.close();
        Serial.println("Scheduling data saved successfully.");
    }
    else
    {
        Serial.println("Failed to open file for writing.");
    }
}

void Configuration::setWifiCredentials(String ssid, String pwd)
{
    File file = SPIFFS.open(wifiFileAddress, FILE_WRITE);
    if (file)
    {
        file.println(ssid);
        file.println(pwd);
        file.close();
        Serial.println("Credentials saved successfully.");
    }
    else
    {
        Serial.println("Failed to open file for writing.");
    }
}

Configuration::PumpSchedule Configuration::getPumpSchedule()
{
    PumpSchedule pSchedule{Time(0, 0), Time(0, 0)};
    auto file = SPIFFS.open(pumpFileAddress, FILE_READ);
    if (file)
    {
        auto startTime = file.readStringUntil('\n');
        auto duration = file.readStringUntil('\n');
        file.close();

        std::string correctedTime(startTime.c_str());
        correctedTime.pop_back();
        std::string correctedDuration(duration.c_str());
        correctedDuration.pop_back();

        if (correctedTime.find(":") != std::string::npos)
        {
            Serial.println("Schedule data = " + String(correctedTime.c_str()) + " " + String(correctedDuration.c_str()));
            std::size_t timeDelimiterPos = correctedTime.find(':');
            if (timeDelimiterPos != std::string::npos)
            {
                pSchedule.start.setHour(std::stoi(correctedTime.substr(0, timeDelimiterPos)));
                pSchedule.start.setMinute(std::stoi(correctedTime.substr(timeDelimiterPos + 1)));
            }

            std::size_t durationDelimiterPos = correctedDuration.find(':');
            if (durationDelimiterPos != std::string::npos)
            {
                pSchedule.duration.setHour(std::stoi(correctedDuration.substr(0, durationDelimiterPos)));
                pSchedule.duration.setMinute(std::stoi(correctedDuration.substr(durationDelimiterPos + 1)));
            }
        }
    }
    return pSchedule;
}

Configuration::WifiCred Configuration::getWifiCredentials()
{
    WifiCred wifiCred;
    if (SPIFFS.exists(wifiFileAddress))
    {
        File file = SPIFFS.open(wifiFileAddress, FILE_READ);
        if (file)
        {
            wifiCred.ssid = file.readStringUntil('\n');
            wifiCred.pwd = file.readStringUntil('\n');
            file.close();
        }
    }
    wifiCred.ssid.remove(wifiCred.ssid.length() - 1); // remove the newline character
    wifiCred.pwd.remove(wifiCred.pwd.length() - 1);
    return wifiCred;
}

std::vector<TempSensorNode> Configuration::getSensorList()
{
    notify(); // Notify sensor manager (Temperature) to retrieve a list of connected sensors
    retriveSavedSensorList(); // Retrieve list from flash memory
    TempSensorNode::mergeAndCopy(devList, savedDevList); // Merge and copy saved nodes to devList
    return devList;
}

void Configuration::setSensorList(std::vector<TempSensorNode> devList)
{
    this->devList = devList;
}

void Configuration::storeSensorNames(std::vector<TempSensorNode>& list)
{
    File file = SPIFFS.open(sensorFileAddress, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    for (auto& node : list)
    {
        if (node.getName().length() != 0) // If it has a name
        {
            file.printf("%llu,%s\n", node.getAddress(), node.getName().c_str());
            Serial.println("Named sensor found!");
        }
    }

    file.close();
    Serial.println("Sensor Data saved to SPIFFS");
}
