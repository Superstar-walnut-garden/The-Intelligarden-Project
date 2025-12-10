#pragma once
#include "IConfigController.hpp"
#include "IJsonSerializable.hpp"
#include <string>

class DisplayConfig: public IJsonSerializable
{
public:
    DisplayConfig();
    DisplayConfig(std::string json);

    enum class DisplayType { Oled, CharLcd };

    std::string toJson() const override;
    void populateFromJson(std::string json) override;


    DisplayType getDisplayType();

private:
    DisplayType displayType; // e.g., "OLED", "LCD"
};
