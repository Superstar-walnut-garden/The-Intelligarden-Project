#include "Time.hpp"


/**
 * @brief Construct a new Time:: Time object
 * 
 * @param hour 
 * @param minute 
 */
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

/**
 * @brief Set the hour of the Time object.
 * 
 * @param hour 
 */
void Time::setHour(int hour)
{
    this->hour = hour;
}

/**
 * @brief Set the minute of the Time object.
 * 
 * @param minute 
 */
void Time::setMinute(int minute)
{
    this->minute = minute;
}

/**
 * @brief Set the time in minutes.
 * 
 * @param minute The total number of minutes to set.
 */
void Time::setTimeInMinutes(int minute)
{
    this->hour = (minute / 60);
    this->minute = (minute % 60);
}

/**
 * @brief Set the time of the Time object.
 * 
 * @param hour 
 * @param minute 
 */
void Time::setTime(int hour, int minute)
{
    setHour(hour);
    setMinute(minute);
}

/**
 * @brief Get the hour of the Time object.
 * 
 * @return int The hour.
 */
int Time::getHour()
{
    return hour;
}

/**
 * @brief Get the minute of the Time object.
 * 
 * @return int The minute.
 */
int Time::getMinute()
{
    return minute;
}

/**
 * @brief Get the time in minutes.
 * 
 * @return int The total time in minutes (e.g. 3:40 is 220 minutes).
 */
int Time::getTimeInMinutes()
{
    return (hour * 60) + minute;
}

/**
 * @brief Convert the Time object to a string representation (HH:MM).
 * 
 * @return std::string The time in HH:MM format.
 */
std::string Time::toString()
{
    auto toStringTwoDigits = [](int number)
    {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << number;
        return oss.str();
    };
    return toStringTwoDigits(hour) + ":" + toStringTwoDigits(minute);
}

/**
 * @brief Parse a time string in HH:MM format to a Time object.
 * 
 * @param timeStr The time string to parse.
 * @return Time The parsed Time object.
 */
Time Time::parse(const char* timeStr)
{
    int hours, minutes;
    sscanf(timeStr, "%d:%d", &hours, &minutes);
    return Time(hours, minutes); // Assuming Time takes hours, minutes, and seconds
}

/**
 * @brief Add another Time object to this Time object.
 * 
 * @param obj The Time object to add.
 * @return Time The resulting Time object after addition.
 */
Time Time::operator + (Time obj)
{
    return Time(this->hour + obj.getHour(), this->minute + obj.getMinute());
}

/**
 * @brief Add another Time object to this Time object and update the current object.
 * 
 * @param obj The Time object to add.
 * @return Time& The updated Time object after addition.
 */
Time& Time::operator += (Time obj)
{
    *this = *this + obj;
    return *this;
}

/**
 * @brief Compare this Time object with another Time object.
 * 
 * @param obj The Time object to compare with.
 * @return true if this Time object is less than the other, false otherwise.
 * @return false if this Time object is greater than or equal to the other.
 */
bool Time::operator < (Time obj)
{
    return (this->getTimeInMinutes() < obj.getTimeInMinutes());
}

/**
 * @brief Compare this Time object with another Time object.
 * 
 * @param obj The Time object to compare with.
 * @return true if this Time object is greater than the other, false otherwise.
 * @return false if this Time object is less than or equal to the other.
 */
bool Time::operator > (Time obj)
{
    return (this->getTimeInMinutes() > obj.getTimeInMinutes());
}

/**
 * @brief Compare this Time object with another Time object for less than or equal to.
 * 
 * @param obj The Time object to compare with.
 * @return true if this Time object is less than or equal to the other, false otherwise.
 */
bool Time::operator <= (Time obj)
{
    return (this->getTimeInMinutes() <= obj.getTimeInMinutes());
}

/**
 * @brief Compare this Time object with another Time object for greater than or equal to.
 * 
 * @param obj The Time object to compare with.
 * @return true if this Time object is greater than or equal to the other, false otherwise.
 */
bool Time::operator >= (Time obj)
{
    return (this->getTimeInMinutes() >= obj.getTimeInMinutes());
}

/**
 * @brief Compare this Time object with another Time object for inequality.
 * 
 * @param obj The Time object to compare with.
 * @return true if both Time objects are inequal, false otherwise.
 */
bool Time::operator != (Time obj)
{
    return (this->getTimeInMinutes() != obj.getTimeInMinutes());
}

/**
 * @brief Compare this Time object with another Time object for equality.
 * 
 * @param obj The Time object to compare with.
 * @return true if both Time objects are equal, false otherwise.
 */
bool Time::operator == (Time obj)
{
    return (this->getTimeInMinutes() == obj.getTimeInMinutes());
}