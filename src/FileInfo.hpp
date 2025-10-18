#pragma once
#include <string>

/**
 * @brief Struct to hold file information
 */
struct FileInfo 
{
    std::string name;
    bool isDirectory;
    size_t size;
};