#include "Display.hpp"

Display* Display::instance = nullptr;

Display::Display()
    : oled(U8G2_R0, U8X8_PIN_NONE, 22, 21), text("Booting up..."), charLcd(0x3F, 2, 16), displayType("char")
{
    JsonDocument doc;
    deserializeJson(doc, Configuration::getInstance()->getDisplayConfig());
    displayType = doc["type"].as<std::string>();
    Serial.println(displayType.c_str());
    if(displayType == "oled")
    {
        oled.begin();
    }
    else if(displayType == "char")
    {
        charLcd.init();
        charLcd.clear();         
        charLcd.backlight(); // Make sure backlight is on

        charLcd.setCursor(4, 1);
        charLcd.print("Loading!");

        charLcd.setCursor(1, 0);
        charLcd.print("Intelligarden");
    }
    else
    {
        Serial.println("Error: Invalid display type. supported types are 'oled' and 'char'");
    }
}

Display* Display::getInstance()
{
    if (!instance)
        instance = new Display();
    return instance;
}

void Display::drawMultiLineText(int x, int y, const char* text)
{
    int lineHeight = 10; // for OLED
    if(displayType == "char")
        lineHeight = 1; // for char lcd
    char* line = strtok((char*)text, "\n");
    while (line != NULL)
    {   if(displayType == "oled")
            oled.drawStr(x, y, line);
        else if(displayType == "char")
        {
            charLcd.setCursor(0, y);
            charLcd.print(line);
        }
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
    if(displayType == "oled")
    {
        drawMultiLineText(2, 10, text.c_str());
        oled.sendBuffer();
    }
    else if(displayType == "char")
    {
        charLcd.clear();
        drawMultiLineText(0, 0, text.c_str());
    }
    
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
