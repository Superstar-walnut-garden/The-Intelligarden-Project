#include "DisplayService.hpp"
#include "DisplayConfig.hpp"
#include "../EnumCrafter.hpp"
/**
 * @brief Singleton instance of the DisplayService class.
 * 
 */
DisplayService* DisplayService::instance = nullptr;

/**
 * @brief Construct a new DisplayService object.
 * 
 */
DisplayService::DisplayService()
    : oled(U8G2_R0, U8X8_PIN_NONE, 22, 21), text("Booting up..."), charLcd(0x3F, 2, 16), displayConfig()
{
    restoreAll(); // Restore the display configuration from persistent storage
    Serial.println(std::string(EnumCrafter::toString(displayConfig.getDisplayType())).c_str());
    if(displayConfig.getDisplayType() == DisplayConfig::DisplayType::Oled)
    {
        oled.begin();
    }
    else if(displayConfig.getDisplayType() == DisplayConfig::DisplayType::CharLcd)
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
 * @brief Get the singleton instance of the DisplayService class.
 * 
 */
DisplayService* DisplayService::getInstance()
{
    if (!instance)
        instance = new DisplayService();
    return instance;
}

/**
 * @brief Draw multi-line text on the DisplayService at specified coordinates.
 * 
 * @param x horizontal position
 * @param y vertical position
 * @param text the text to be displayed, can contain newlines(/n)
 */
void DisplayService::drawMultiLineText(int x, int y, const char* text)
{
    int lineHeight = 10; // for OLED
    if(displayConfig.getDisplayType() == DisplayConfig::DisplayType::CharLcd)
        lineHeight = 1; // for char lcd
    char* line = strtok((char*)text, "\n");
    while (line != NULL)
    {   if(displayConfig.getDisplayType() == DisplayConfig::DisplayType::Oled)
            oled.drawStr(x, y, line);
        else if(displayConfig.getDisplayType() == DisplayConfig::DisplayType::CharLcd)
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
std::string DisplayService::to_string_with_precision(double value, int precision)
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
void DisplayService::drawUI()
{
    oled.clearBuffer();
    oled.drawFrame(0, 0, 125, 63);
    oled.setFont(u8g2_font_ncenB08_tr);    // choose a suitable font
    if(displayConfig.getDisplayType() == DisplayConfig::DisplayType::Oled)
    {
        drawMultiLineText(2, 10, text.c_str());
        oled.sendBuffer();
    }
    else if(displayConfig.getDisplayType() == DisplayConfig::DisplayType::CharLcd)
    {
        charLcd.clear();
        drawMultiLineText(0, 0, text.c_str());
    }
    
}

/**
 * @brief Update the DisplayService with temperature data.
 * 
 * @param temp pointer to a TempSensorService object containing sensor data
 */
void DisplayService::update(TempSensorService *temp)
{
    Serial.println("DisplayService got an update!");
    text.clear();
    temp->forEachSensor([this](TempSensorItem sensor) 
    {
        auto name = sensor.getName();
        text += name + ": " + to_string_with_precision(sensor.getTemp(), 2) + "\n";
    }, false);
}

/**
 * @brief Update the display configuration.
 * @param newConfig the new display configuration to be applied
 */
void DisplayService::updateConfig(DisplayConfig newConfig)
{
    displayConfig = newConfig;
    storeAll(); // Save the updated configuration
    Serial.println("DisplayService configuration updated.");
    // reset bus and reinitialize display based on type
}

/**
 * @brief Get the current display configuration in JSON format.
 * 
 * @return std::string 
 */
std::string DisplayService::getConfig()
{
    return displayConfig.toJson();
}

/**
 * @brief Store all display configurations to persistent storage.
 * @details This function saves the current display configuration to the configuration file. 
 */
void DisplayService::storeAll()
{
    Configuration::getInstance()->setDisplayConfig(displayConfig.toJson());
}

/**
 * @brief 
 * @details This function retrieves the display configuration from the configuration file and populates the displayConfig object.
 */
void DisplayService::restoreAll()
{
    auto json = Configuration::getInstance()->getDisplayConfig();
    if (!json.empty())
        displayConfig.populateFromJson(json);
    else
        Serial.println("No display configuration found, using default.");
}