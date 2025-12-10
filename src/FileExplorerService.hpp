#pragma once

#include <U8g2lib.h>
#include <LiquidCrystal_I2C.h>
#include "Configuration.hpp"
#include "TempSensorService.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>  // For strtok
#include "IConfigController.hpp"
#include "IReadOnlyResourceController.hpp"
#include "IExternalStorage.hpp"
#include "SpiMicroSd.hpp"


class FileExplorerService: public IReadOnlyResourceController
{
public:
    static FileExplorerService *getInstance();

    std::string get(std::string path); // only the path-based get is supported and should be manually added as api endpoint.

private:
    FileExplorerService();
    std::string getAll() override;
    std::string get(uint64_t id) override {return "{}";} // the default get is unsupported

    IExternalStorage* storage;
    static FileExplorerService *instance;
};
