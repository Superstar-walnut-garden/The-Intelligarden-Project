#ifndef PUMP_HPP
#define PUMP_HPP
#include <SPIFFS.h>
#include <string.h>


class Pump
{
    private:
    bool pumpState;
    static Pump *instance;
    int manualTime = 0, manualMinutes = 0, manualHours = 0;
    bool manualOn;
    Pump(): pumpState(0), manualOn(false)
    {
        
    }
    int getMinutesFrom(int hours, int minutes)
    {
        return (hours * 60) + minutes;
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
    void manualSwitch(signed manualTime = -1)
    {
        if(manualTime >= 0)
        {
            this->manualTime = manualTime;
            manualOn = true;
        }
        else if(manualTime == -1)
        {
            manualOn = false;
            pumpState = 0;
        }
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

                auto untilMinutes = minutes + autoDurationMinutes;
                auto untilHours = hours + autoDurationHours;
                if(untilMinutes >= 60)
                {
                    untilMinutes -= 60;
                    untilHours ++;
                }
                Serial.println("pump start time: " + String(hours) + ":" + String(minutes));
                Serial.println("pump end time: " + String(untilHours) + ":" + String(untilMinutes));
                Serial.println("pump duration: " + String(autoDurationHours) + ":" + String(autoDurationMinutes));

                Serial.println("checking if the schedule is ready");
                auto currentTimeInMin = getMinutesFrom(currentHours, currentMinutes);
                auto startTimeInMin = getMinutesFrom(hours, minutes);
                auto untilTimeInMin = getMinutesFrom(untilHours, untilMinutes);
                if(currentTimeInMin >= startTimeInMin and currentTimeInMin <= untilTimeInMin)
                {
                    Serial.println("Pump is on (with schedule)");
                    pumpState = 1;
                }
                else
                {
                    Serial.println("Pump is off (with schedule)");
                    pumpState = 0;
                }
            }
        }
        if(manualOn)
        {
            manualMinutes = manualTime + (currentHours * 60) + currentMinutes;

            if(getMinutesFrom(currentHours, currentMinutes) <= manualMinutes) // minutes check
            {
                Serial.println("Pump is manually on");
                pumpState = 1;
            }
            else
            {
                Serial.println("Pump is manually off");
                manualOn = false;
                pumpState = 0;
            }
        }
    }
};
Pump *Pump::instance = nullptr;

#endif