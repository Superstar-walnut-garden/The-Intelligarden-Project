#ifndef TIME_HPP
#define TIME_HPP
#include <cstdio>
#include <string>
#include <arduino.h>
class Time
{
    public:
    Time(int hour = 0, int minute = 0): hour(hour), minute(minute)
    {
        for(auto i = 0; i <= (minute / 60); i ++)
            if(minute >= 60)
            {
                minute -= 60;
                hour++;
            }
    }
    void setHour(int hour)
    {
        this->hour = hour;
    }
    void setMinute(int minute)
    {
        this->minute = minute;
    }
    void setTimeInMinutes(int minute)
    {
        this->hour = (minute / 60);
        this->minute =  (minute % 60);
    }
    void setTime(int hour, int minute)
    {
        setHour(hour);
        setMinute(minute);
    }
    auto getHour()
    {
        return hour;
    }
    auto getMinute()
    {
        return minute;
    }
    auto getTimeInMinutes()
    {
        return (hour * 60) + minute;
    }
    std::string toString()
    {
        return std::to_string(hour) + ":" + std::to_string(minute);
    }
    static Time parse(const char* timeStr)
    {
        int hours, minutes;
        sscanf(timeStr, "%d:%d", &hours, &minutes);
        return Time(hours, minutes); // Assuming Time takes hours, minutes, and seconds
    }

    auto operator + (Time obj)
    {
        return Time(this->hour + obj.getHour(), this->minute + obj.getMinute());
    }
    auto operator += (Time obj)
    {
        return *this + obj;
    }
    auto operator < (Time obj)
    {
        return (this->getTimeInMinutes() < obj.getTimeInMinutes());
    }
    auto operator > (Time obj)
    {
        return (this->getTimeInMinutes() > obj.getTimeInMinutes());
    }
    auto operator <= (Time obj)
    {
        return (this->getTimeInMinutes() < obj.getTimeInMinutes());
    }
    auto operator >= (Time obj)
    {
        return (this->getTimeInMinutes() > obj.getTimeInMinutes());
    }
    auto operator != (Time obj)
    {
        return (this->getTimeInMinutes() != obj.getTimeInMinutes());
    }
    auto operator == (Time obj)
    {
        return (this->getTimeInMinutes() == obj.getTimeInMinutes());
    }

    private:
    short hour;
    short minute;
};

#endif