#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <U8g2lib.h>
#include "Configuration.hpp"
#include "Temperature.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>  // For strtok

typedef U8G2_SSD1309_128X64_NONAME2_F_HW_I2C OLedPanel;

class Display: public IObserver<Temperature>
{
public:
    static Display *getInstance();

    void drawMultiLineText(int x, int y, const char* text);
    std::string to_string_with_precision(double value, int precision);
    void drawUI();
    void update(Temperature *temp);

private:
    Display();
    std::string text;
    OLedPanel oled;
    static Display *instance;
};

#endif // DISPLAY_HPP
