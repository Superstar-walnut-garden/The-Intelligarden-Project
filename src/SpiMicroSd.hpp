#pragma once
#include "IExternalStorage.hpp"

class SpiMicroSd: public IExternalStorage
{
public:
    static SpiMicroSd* getInstance();
    void writeFile(const std::string path, const std::string data) override;
    size_t getFileSize(const std::string path) const override;
    bool fileExists(const std::string path) const override;
    std::string readFile(const std::string path) const override;
    void deleteFile(const std::string path) override;
    bool isReady() const override;
    size_t getMaxCapacity() const override;
    size_t getRemainingCapacity() const override;
    std::vector<FileInfo> listEntries(const std::string& path) const override;

private:
    SpiMicroSd();
    void initializeSdCard();
    void deinitializeSdCard();
    static SpiMicroSd* instance;

    bool sdInitialized;
};
