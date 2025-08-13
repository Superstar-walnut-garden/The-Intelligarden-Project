#include "Display.hpp"

/**
 * @brief Singleton instance of the Display class.
 * 
 */
Display* Display::instance = nullptr;

/**
 * @brief Construct a new Display object.
 * 
 */
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

/**
 * @brief Get the singleton instance of the Display class.
 * 
 */
Display* Display::getInstance()
{
    if (!instance)
        instance = new Display();
    return instance;
}

/**
 * @brief Draw multi-line text on the display at specified coordinates.
 * 
 * @param x horizontal position
 * @param y vertical position
 * @param text the text to be displayed, can contain newlines(/n)
 */
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

/**
 * @brief Convert a double value to a string with specified precision.
 * 
 * @param value the double value to be converted
 * @param precision the number of decimal places
 * @return std::string the formatted string
 */
std::string Display::to_string_with_precision(double value, int precision)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

/**
 * @brief Draw the user interface on the display.
 * 
 * This function clears the display buffer, draws a frame, sets the font,
 * and displays the text. It supports both OLED and character LCD displays.
 */
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

/**
 * @brief Update the display with temperature data.
 * 
 * @param temp pointer to a Temperature object containing sensor data
 */
void Display::update(Temperature *temp)
{
    Serial.println("display got an update!");
    text.clear();
    temp->forEachSensor([this](TempSensorItem sensor) 
    {
        auto name = sensor.getName();
        text += name + ": " + to_string_with_precision(sensor.getTemp(), 2) + "\n";
    }, false);
}
