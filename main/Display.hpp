#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <U8g2lib.h>
#include "Configuration.hpp"
#include "Temperature.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

typedef U8G2_SSD1309_128X64_NONAME2_F_HW_I2C OLedPanel;
class Display: public IObserver<Temperature>
{
    public:
    static Display *getInstance()
    {
        if(!instance)
            instance = new Display();
        return instance;
    }
    void drawMultiLineText(int x, int y, const char* text) {
        int lineHeight = 10; // Adjust line height as needed
        char* line = strtok((char*)text, "\n");
        while (line != NULL)
        {
            oled.drawStr(x, y, line);
            y += lineHeight; // Move to the next line
            line = strtok(NULL, "\n");
        }
    }
    std::string to_string_with_precision(double value, int precision)
    {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
    }

    void drawUI()
    {
        oled.clearBuffer();
        oled.drawFrame(0, 0, 125, 63);
        oled.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
        drawMultiLineText(2, 10, text.c_str());
        oled.sendBuffer();
    }
    void update(Temperature *temp)
    {
        auto *cfg = Configuration::getInstance();
        auto sensorList = cfg->getSensorList();
        Serial.println("display got an update!");
        text.clear();
        for(auto sensor : sensorList)
        {
            auto name = sensor.getName();
            text += name + ": " + to_string_with_precision(temp->getData(name), 2) + "\n";
        }
    }

    private:
    Display(): oled(U8G2_R0, U8X8_PIN_NONE, 22, 21), text("No data!")
    {
        oled.begin();
    }
    std::string text;
    OLedPanel oled;
    static Display *instance;
};
Display *Display::instance = nullptr;


#endif
