#include "Time.hpp"

// Constructor implementation
Time::Time(int hour, int minute) : hour(hour), minute(minute)
{
    for (auto i = 0; i <= (minute / 60); i++)
    {
        if (minute >= 60)
        {
            minute -= 60;
            hour++;
        }
    }
}

// Setter methods implementation
void Time::setHour(int hour)
{
    this->hour = hour;
}

void Time::setMinute(int minute)
{
    this->minute = minute;
}

void Time::setTimeInMinutes(int minute)
{
    this->hour = (minute / 60);
    this->minute = (minute % 60);
}

void Time::setTime(int hour, int minute)
{
    setHour(hour);
    setMinute(minute);
}

// Getter methods implementation
int Time::getHour()
{
    return hour;
}

int Time::getMinute()
{
    return minute;
}

int Time::getTimeInMinutes()
{
    return (hour * 60) + minute;
}
std::string Time::toString()
{
    return std::to_string(hour) + ":" + std::to_string(minute);
}
Time Time::parse(const char* timeStr)
{
    int hours, minutes;
    sscanf(timeStr, "%d:%d", &hours, &minutes);
    return Time(hours, minutes); // Assuming Time takes hours, minutes, and seconds
}

// Operator overloading methods
Time Time::operator + (Time obj)
{
    return Time(this->hour + obj.getHour(), this->minute + obj.getMinute());
}

Time& Time::operator += (Time obj)
{
    *this = *this + obj;
    return *this;
}

bool Time::operator < (Time obj)
{
    return (this->getTimeInMinutes() < obj.getTimeInMinutes());
}

bool Time::operator > (Time obj)
{
    return (this->getTimeInMinutes() > obj.getTimeInMinutes());
}

bool Time::operator <= (Time obj)
{
    return (this->getTimeInMinutes() <= obj.getTimeInMinutes());
}

bool Time::operator >= (Time obj)
{
    return (this->getTimeInMinutes() >= obj.getTimeInMinutes());
}
bool Time::operator != (Time obj)
{
    return (this->getTimeInMinutes() != obj.getTimeInMinutes());
}
bool Time::operator == (Time obj)
{
    return (this->getTimeInMinutes() == obj.getTimeInMinutes());
}