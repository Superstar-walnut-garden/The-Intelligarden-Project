#ifndef TIME_HPP
#define TIME_HPP

#include <Arduino.h>  // Needed for the String class
#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

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
    std::string toString();
    static Time parse(const char* timeStr);

    Time operator + (Time obj);    // Return type specified as Time
    Time& operator += (Time obj);  // Return type specified as reference to Time
    bool operator < (Time obj);    // Return type specified as bool
    bool operator > (Time obj);
    bool operator <= (Time obj);
    bool operator >= (Time obj);
    bool operator != (Time obj);
    bool operator == (Time obj);

private:
    short hour;
    short minute;
};

#endif // TIME_HPP
