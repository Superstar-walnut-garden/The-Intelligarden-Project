#ifndef PUMP_HPP
#define PUMP_HPP
#include <SPIFFS.h>
#include <string.h>


class Pump
{
    private:
    bool pumpState;
    static Pump *instance;
    Pump(): pumpState(0)
    {
        
    }

    public:
    static Pump *getInstance()
    {
        if(!instance)
            instance = new Pump();
        return instance;
    }
    
    bool getPumpState()
    {
        return pumpState;
    }
    void loop(int currentHours, int currentMinutes)
    {
        String startTime, duration;
        auto file = SPIFFS.open("/schedule.txt", FILE_READ);
        if(file)
        {
            startTime = file.readStringUntil('\n');
            duration = file.readStringUntil('\n');
            file.close();

            std::string correctedTime(startTime.c_str());
            correctedTime.pop_back();
            std::string correctedDuration(duration.c_str());
            correctedDuration.pop_back();

            int hours, minutes, autoDurationHours, autoDurationMinutes;
            if(correctedTime.find(":") != -1)
            {
                Serial.println("schedule data = " + String(correctedTime.c_str()) + " " + String(correctedDuration.c_str()));
                std::size_t timeDelimiterPos = correctedTime.find(':');
                if (timeDelimiterPos != std::string::npos) {
                    hours = std::stoi(correctedTime.substr(0, timeDelimiterPos));
                    minutes = std::stoi(correctedTime.substr(timeDelimiterPos + 1));
                }

                // Parse correctedDuration
                std::size_t durationDelimiterPos = correctedDuration.find(':');
                if (durationDelimiterPos != std::string::npos) {
                    autoDurationHours = std::stoi(correctedDuration.substr(0, durationDelimiterPos));
                    autoDurationMinutes = std::stoi(correctedDuration.substr(durationDelimiterPos + 1));
                }

                Serial.println("pump start time: " + String(hours) + ":" + String(minutes));
                Serial.println("pump duration: " + String(autoDurationHours) + ":" + String(autoDurationMinutes));
                if(currentHours >= hours and currentHours <= hours + autoDurationHours) // hours check
                {
                    if(currentMinutes >= minutes and currentMinutes <= minutes + autoDurationMinutes) // minutes check
                    {
                        Serial.println("Pump is on");
                        pumpState = 1;
                    }
                    else
                    {
                        Serial.println("Pump is off");
                        pumpState = 0;
                    }
                }
            }
        }
    }
};
Pump *Pump::instance = nullptr;

#endif