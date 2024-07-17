#ifndef TIME_HPP
#define TIME_HPP

class Time
{
    public:
    Time(int hour, int minute): hour(hour), minute(minute)
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
    auto toString()
    {
        return String(hour) + ":" + String(minute);
    }

    auto operator + (Time& obj)
    {
        return Time(this->hour + obj.getHour(), this->minute + obj.getMinute());
    }
    auto operator += (Time& obj)
    {
        return *this + obj;
    }
    auto operator < (Time& obj)
    {
        return (this->getTimeInMinutes() < obj.getTimeInMinutes());
    }
    auto operator > (Time& obj)
    {
        return (this->getTimeInMinutes() > obj.getTimeInMinutes());
    }
    auto operator <= (Time& obj)
    {
        return (this->getTimeInMinutes() < obj.getTimeInMinutes());
    }
    auto operator >= (Time& obj)
    {
        return (this->getTimeInMinutes() > obj.getTimeInMinutes());
    }

    private:
    short hour;
    short minute;
};

#endif