#include "Display.hpp"

Display* Display::instance = nullptr;

Display::Display()
    : oled(U8G2_R0, U8X8_PIN_NONE, 22, 21), text("Booting up...")
{
    oled.begin();
}

Display* Display::getInstance()
{
    if (!instance)
        instance = new Display();
    return instance;
}

void Display::drawMultiLineText(int x, int y, const char* text)
{
    int lineHeight = 10; // Adjust line height as needed
    char* line = strtok((char*)text, "\n");
    while (line != NULL)
    {
        oled.drawStr(x, y, line);
        y += lineHeight; // Move to the next line
        line = strtok(NULL, "\n");
    }
}

std::string Display::to_string_with_precision(double value, int precision)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

void Display::drawUI()
{
    oled.clearBuffer();
    oled.drawFrame(0, 0, 125, 63);
    oled.setFont(u8g2_font_ncenB08_tr);    // choose a suitable font
    drawMultiLineText(2, 10, text.c_str());
    oled.sendBuffer();
}

void Display::update(Temperature *temp)
{
    auto *cfg = Configuration::getInstance();
    auto sensorList = cfg->getSensorList();
    Serial.println("display got an update!");
    text.clear();
    for (auto sensor : sensorList)
    {
        auto name = sensor.getName();
        text += name + ": " + to_string_with_precision(temp->getData(name), 2) + "\n";
    }
}
