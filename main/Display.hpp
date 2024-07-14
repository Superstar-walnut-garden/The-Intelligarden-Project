#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <U8g2lib.h>
#include "Configuration.hpp"
#include "Temperature.hpp"
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
    
    void drawUI()
    {
        oled.clearBuffer();
        oled.drawFrame(0, 0, 125, 63);
        oled.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
        oled.drawStr(2, 20, "test text");	// write something to the internal memory
        oled.sendBuffer();
    }
    void update(Temperature *temp)
    {
        ambientTemp = temp->getData("ambient");
    }

    private:
    Display(): oled(U8G2_R0, U8X8_PIN_NONE, 22, 21), ambientTemp(0)
    {
        oled.begin();
    }
    double ambientTemp;
    OLedPanel oled;
    static Display *instance;
};
Display *Display::instance = nullptr;


#endif
