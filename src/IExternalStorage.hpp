#pragma once
#include <string>
#include <vector>
#include "FileInfo.hpp"

class IExternalStorage
{
public:
    virtual ~IExternalStorage() = default;

    virtual void writeFile(const std::string path, const std::string data) = 0;
    virtual size_t getFileSize(const std::string path) const = 0;
    virtual bool fileExists(const std::string path) const= 0;
    virtual std::string readFile(const std::string path) const= 0;
    virtual void deleteFile(const std::string path) = 0;
    virtual bool isReady() const= 0;
    virtual size_t getMaxCapacity() const= 0;
    virtual size_t getRemainingCapacity() const= 0;
    virtual std::vector<FileInfo> listEntries(const std::string& path) const = 0;

};
