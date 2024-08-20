#ifndef TIME_HPP
#define TIME_HPP

#include <Arduino.h>  // Needed for the String class
#include <cstdio>
#include <string>

class Time
{
public:
    Time(int hour = 0, int minute = 0);  // Constructor declaration with default values
    void setHour(int hour);
    void setMinute(int minute);
    void setTimeInMinutes(int minute);
    void setTime(int hour, int minute);
    int getHour();               // Return type specified explicitly as int
    int getMinute();
    int getTimeInMinutes();
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

    Time operator + (Time obj);    // Return type specified as Time
    Time& operator += (Time obj);  // Return type specified as reference to Time
    bool operator < (Time obj);    // Return type specified as bool
    bool operator > (Time obj);
    bool operator <= (Time obj);
    bool operator >= (Time obj);
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

#endif // TIME_HPP
