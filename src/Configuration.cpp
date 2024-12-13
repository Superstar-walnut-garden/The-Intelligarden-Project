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
void Configuration::setSchedulerList(const char *json, int length)
{
    //auto schedulerList = SchedulerList(json, length);
    File file = SPIFFS.open(pumpFileAddress, FILE_WRITE);
    if (file)
    {
        file.println(json);
        file.close();
        Serial.println("scheduling data saved successfully.");
    } 
    else 
    {
        Serial.println("Failed to open file for writing.");
    }
}
SchedulerList Configuration::getSchedulerList()
{
    auto schedulerList = SchedulerList();
    auto file = SPIFFS.open(pumpFileAddress, FILE_READ);
    if(file)
    {
        auto json = file.readString(); // read raw data from file
        schedulerList.repopulateWith(json.c_str(), json.length()); // parse data and repopulate the SchedulerList
        Scheduler scheduler(schedulerList);
        scheduler.determineStatusofItems(); // check with current date and time to indicate which item is on
        file.close();
    }
    return schedulerList;
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
