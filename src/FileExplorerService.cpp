#include "FileExplorerService.hpp"

/**
 * @brief static member initialization
 * 
 */
FileExplorerService* FileExplorerService::instance = nullptr;

/**
 * @brief Get the singleton instance of the FileExplorerService.
 * 
 * @return FileExplorerService* Pointer to the singleton instance.
 */
FileExplorerService* FileExplorerService::getInstance()
{
    if (!instance)
    {
        instance = new FileExplorerService();
    }
    return instance;
}

/**
 * @brief Construct a new FileExplorerService object.
 * 
 */
FileExplorerService::FileExplorerService(): storage(SpiMicroSd::getInstance())
{
}

/**
 * @brief Get file or directory information based on the provided path.
 * 
 * @param path The path to the file or directory.
 * @return std::string JSON string representation of the file or directory information.
 */
std::string FileExplorerService::get(std::string path)
{
    return storage->readFile(path);
}

/**
 * @brief Get the list of files and directories in JSON format.
 * 
 * @return std::string JSON string representation of the list of files and directories.
 */
std::string FileExplorerService::getAll()
{
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    root["path"] = "/";

    auto ensureTrailingSlash = [](const std::string& p) {
        if (p.empty()) return std::string("/");
        if (p.back() == '/') return p;
        return p + '/';
    };

    std::function<void(const std::string&, JsonObject)> recurse;
    recurse = [&](const std::string& currentPath, JsonObject parentObj)
    {
        JsonArray foldersArray = parentObj.createNestedArray("folders");
        JsonArray filesArray   = parentObj.createNestedArray("files");

        auto list = storage->listEntries(currentPath);
        for (const auto& entry : list)
        {
            std::string fullPath = ensureTrailingSlash(currentPath) + entry.name;

            if (entry.isDirectory)
            {
                JsonObject folderObj = foldersArray.createNestedObject();
                folderObj["name"] = entry.name;       // raw string
                folderObj["path"] = fullPath;         // raw string
                recurse(fullPath, folderObj);         // recurse into folder
            }
            else
            {
                JsonObject fileObj = filesArray.createNestedObject();
                fileObj["name"] = entry.name;         // raw string
                fileObj["path"] = fullPath;           // raw string
                fileObj["size"] = storage->getFileSize(fullPath);
                // fileObj["lastModified"] = storage->getLastModified(fullPath);
            }
        }
    };

    recurse("/", root);

    std::string output;
    serializeJson(doc, output);
    return output;
};