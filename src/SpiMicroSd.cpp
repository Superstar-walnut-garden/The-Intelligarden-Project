#include "SpiMicroSd.hpp"
#include <SD.h>
#include <SPI.h>
#include <FS.h>

/**
 * @brief Static member initialization
 */
SpiMicroSd* SpiMicroSd::instance = nullptr;

/**
 * @brief Get the singleton instance of SpiMicroSd
 * 
 * @return SpiMicroSd* Pointer to the singleton instance
 */
SpiMicroSd* SpiMicroSd::getInstance()
{
    if (instance == nullptr)
    {
        instance = new SpiMicroSd();
    }
    return instance;
}

/**
 * @brief Private constructor for SpiMicroSd
 */
SpiMicroSd::SpiMicroSd(): sdInitialized(false)
{
    initializeSdCard();
}

/**
 * @brief Write data to a file, creating necessary directories
 * @details This method creates any necessary directories in the specified path and writes the provided data to the file.
 * 
 * @param fullPath Full path of the file to write
 * @param data Data to write to the file
 */
void SpiMicroSd::writeFile(const std::string fullPath, const std::string data)
{
    if(!isReady())
    {
        Serial.println("warning: SD card is not initialized. Cannot write file.");
        return;
    }
    String path = String(fullPath.c_str());

    // Create folders recursively
    int lastSlash = 0;
    while ((lastSlash = path.indexOf('/', lastSlash + 1)) != -1) // Find next slash
    {
        String folder = path.substring(0, lastSlash); // Get folder path
        if (!SD.exists(folder)) // Check if folder exists
        {
            SD.mkdir(folder); // Create folder
        }
    }

    // Create and write the file
    File file = SD.open(path.c_str(), FILE_WRITE);
    if (file) 
    {
        file.print(data.c_str());
        file.close();
        Serial.println("info: File created: " + path);
    } else 
    {
        Serial.println("warning: Failed to create file: " + path);
    }
}

/**
 * @brief Get the size of a file
 * 
 * @param path Path of the file
 * @return size_t Size of the file in bytes, or 0 if the file does not exist or cannot be opened
 */
size_t SpiMicroSd::getFileSize(const std::string path) const
{
    if(!isReady())
    {
        Serial.println("warning: SD card is not initialized. Cannot get file size.");
        return 0;
    }

    if (!SD.exists(path.c_str()))
    {
        Serial.println("File does not exist: " + String(path.c_str()));
        return 0;
    }

    File file = SD.open(path.c_str(), FILE_READ);
    if (file)
    {
        size_t size = file.size();
        file.close();
        return size;
    }
    else
    {
        Serial.println("Failed to open file for reading: " + String(path.c_str()));
        return 0;
    }
}

/**
 * @brief Check if a file exists
 * 
 * @param path Path of the file
 * @return true If the file exists, false otherwise (e.g. missing, inaccessible, or deleted)
 */
bool SpiMicroSd::fileExists(const std::string path) const
{
    if(!isReady())
    {
        Serial.println("warning: SD card is not initialized. Cannot get file size.");
        return false;
    }
    return SD.exists(path.c_str());
}

/**
 * @brief Read the contents of a file
 * 
 * @param path Path of the file
 * @return std::string Contents of the file, or an empty string if the file does not exist or cannot be opened
 */
std::string SpiMicroSd::readFile(const std::string path) const
{
    if(!isReady())
    {
        Serial.println("warning: SD card is not initialized. Cannot read file.");
        return "";
    }

    if (!SD.exists(path.c_str()))
    {
        Serial.println("File does not exist: " + String(path.c_str()));
        return "";
    }

    File file = SD.open(path.c_str(), FILE_READ);
    if (file)
    {
        std::string content;
        while (file.available())
        {
            content += static_cast<char>(file.read());
        }
        file.close();
        return content;
    }
    else
    {
        Serial.println("Failed to open file for reading: " + String(path.c_str()));
        return "";
    }
}

/**
 * @brief Delete a file
 * 
 * @param path Path of the file to delete
 */
void SpiMicroSd::deleteFile(const std::string path)
{
    if(!isReady())
    {
        Serial.println("warning: SD card is not initialized. Cannot delete file.");
        return;
    }
    if (SD.exists(path.c_str()))
    {
        if (SD.remove(path.c_str()))
        {
            Serial.println("info: File deleted: " + String(path.c_str()));
        }
        else
        {
            Serial.println("warning: Failed to delete file: " + String(path.c_str()));
        }
    }
    else
    {
        Serial.println("warning: File does not exist: " + String(path.c_str()));
    }
}

/**
 * @brief Check if the SD card is initialized and ready for operations
 * 
 * @return true If the SD card is initialized, false otherwise
 */
bool SpiMicroSd::isReady() const
{
    return sdInitialized;
}

/**
 * @brief Get the maximum capacity of the SD card
 * 
 * @return size_t Maximum capacity in bytes, or 0 if the SD card is not initialized
 */
size_t SpiMicroSd::getMaxCapacity() const
{
    if(!isReady())
    {
        Serial.println("warning: SD card is not initialized. Cannot get max capacity.");
        return 0;
    }
    return SD.totalBytes();
}

/**
 * @brief Get the remaining capacity of the SD card
 * 
 * @return size_t Remaining capacity in bytes, or 0 if the SD card is not initialized
 */
size_t SpiMicroSd::getRemainingCapacity() const
{
    if(!isReady())
    {
        Serial.println("warning: SD card is not initialized. Cannot get remaining capacity.");
        return 0;
    }
    return SD.totalBytes() - SD.usedBytes();
}

/**
 * @brief Initialize the SD card subsystem
 */
void SpiMicroSd::initializeSdCard()
{
    if (!SD.begin(5))
        Serial.println("warning: SD card initialization failed!");
    else
        Serial.println("info: SD card subsystem initialized.");

    if(!SD.cardType() == CARD_NONE)
    {
        Serial.println("info: SD card mounted successfully");
        sdInitialized = true;
    }
    else
    {
        Serial.println("warning: SD card mount failed");
    }
}

/**
 * @brief Deinitialize the SD card subsystem
 */
void SpiMicroSd::deinitializeSdCard()
{
    SD.end();
    sdInitialized = false;
    Serial.println("info: SD card subsystem deinitialized.");
}

std::vector<FileInfo> SpiMicroSd::listEntries(const std::string& path) const
{
    std::vector<FileInfo> entries;
    if(!isReady())
    {
        Serial.println("warning: SD card is not initialized. Cannot list entries.");
        return entries;
    }

    File dir = SD.open(path.c_str());
    if (!dir)
    {
        Serial.println("warning: Failed to open directory: " + String(path.c_str()));
        return entries;
    }
    if (!dir.isDirectory())
    {
        Serial.println("warning: Path is not a directory: " + String(path.c_str()));
        dir.close();
        return entries;
    }

    File entry;
    while (entry = dir.openNextFile())
    {
        FileInfo info;
        info.name = entry.name();
        info.isDirectory = entry.isDirectory();
        info.size = entry.size();
        entries.push_back(info);
        entry.close();
    }

    dir.close();
    return entries;
}