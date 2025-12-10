#pragma once

#include <string>
#include <cstdint>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <chrono>

class LogUtils
{
public:
    static std::string buildPath(std::string subsystem, std::string itemName, uint64_t itemId, int index = 0)
    {
        std::string id = std::to_string(itemId);
        std::string currentDate = getCurrentDate();
        if(itemId > 65535) // if larger than 16bits
            id = encode64BitNumberToBase62(itemId); // encode to base62 to shorten the string length (for ds18b20 temp sensor addresses)
        if(itemName.empty())
            itemName = "untitled";
        
        return subsystem + "/" + itemName + "-id(" + id + ")" + "/" + currentDate + "(" + std::to_string(index) + ")" + ".json";
    }
    static std::string buildTimestamp() 
    {
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        char buffer[9]; // HH:MM:SS + null terminator
        std::strftime(buffer, sizeof(buffer), "%H:%M:%S", localTime);
        return buffer;
    }

    static std::chrono::seconds parseTimestamp(const std::string& timestamp) 
    {
        int h, m, s;
        char sep1, sep2;
        std::istringstream iss(timestamp);
        if (!(iss >> h >> sep1 >> m >> sep2 >> s) || sep1 != ':' || sep2 != ':') 
        {
            throw std::invalid_argument("Invalid timestamp format: " + timestamp);
        }
        return std::chrono::hours(h) + std::chrono::minutes(m) + std::chrono::seconds(s);
    }


private:
    static std::string encode64BitNumberToBase62(uint64_t num) 
    {
        const std::string BASE62 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        // Apply the same bitmask: shift right 8 bits, then mask to 48 bits 
        // because ds18b20 temp sensor addresses differ only in their middle 48 bits
        uint64_t masked = (num >> 8) & ((1ULL << 48) - 1);

        std::string base62String;
        do 
        {
            uint64_t remainder = masked % 62;
            base62String = BASE62[remainder] + base62String;
            masked /= 62;
        } while (masked > 0);

        return base62String;
    }

    static std::string getCurrentDate() 
    {
        std::time_t now = std::time(nullptr);               // Get current time
        std::tm* localTime = std::localtime(&now);          // Convert to local time

        std::ostringstream oss;
        oss << std::put_time(localTime, "%Y-%m-%d");        // Format as YYYY-MM-DD
        return oss.str();
    }

};