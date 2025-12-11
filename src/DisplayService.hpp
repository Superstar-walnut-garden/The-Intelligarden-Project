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
#include "DisplayConfig.hpp"
#include "IResourcePersistenceService.hpp"

typedef U8G2_SSD1309_128X64_NONAME2_F_HW_I2C OLedPanel;

class DisplayService: public IResourcePersistenceService, public IConfigController<DisplayConfig>, public IObserver<TempSensorService>
{
public:
    static DisplayService *getInstance();

    void drawMultiLineText(int x, int y, const char* text);
    std::string to_string_with_precision(double value, int precision);
    void drawUI();
    void update(TempSensorService *temp);

    void updateConfig(DisplayConfig newConfig) override;
    std::string getConfig() override;

    void storeAll() override;
    void restoreAll() override;

private:
    DisplayService();
    std::string text;
    OLedPanel oled;
    bool displayAvailable;
    LiquidCrystal_I2C charLcd;
    static DisplayService *instance;
    DisplayConfig displayConfig;
};
