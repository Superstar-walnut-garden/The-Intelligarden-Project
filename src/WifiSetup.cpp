#include "WifiSetup.hpp"

// Static member variable initialization
WifiSetup *WifiSetup::instance = nullptr;

// Singleton instance getter
WifiSetup *WifiSetup::getInstance()
{
    if (!instance)
        instance = new WifiSetup();
    return instance;
}

// Check if connected to WiFi
bool WifiSetup::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

// Constructor
WifiSetup::WifiSetup()
{
    IPAddress local_IP(192, 168, 1, 184);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 0, 0);
    IPAddress primaryDNS(10, 202, 10, 202);
    IPAddress secondaryDNS(10, 202, 10, 102);
    initSPIFFS();
    String stored_ssid, stored_password;
    if (readWiFiCredentials(stored_ssid, stored_password))
    {
        std::string correctedSSID(stored_ssid.c_str());
        std::string correctedPassword(stored_password.c_str());
        correctedSSID.pop_back(); // remove the \n
        correctedPassword.pop_back();
        Serial.print("Connecting to ");
        Serial.println(correctedSSID.c_str());
        Serial.println(correctedPassword.c_str());

        // Connect to WiFi with the stored credentials
        WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
        WiFi.begin(correctedSSID.c_str(), correctedPassword.c_str());
        while (WiFi.status() != WL_CONNECTED)
        {
            static int localCounter = 0;
            delay(500);
            Serial.print(".");
            if (localCounter++ > 15)
            {
                localCounter = 0;
                break;
            }
        }
    }
    else
    {
        // Handle the case where no credentials are found
        Serial.println("No stored WiFi credentials found. Please connect to the AP to configure.");
    }
}

// Function to read WiFi credentials from SPIFFS
bool WifiSetup::readWiFiCredentials(String &ssid, String &password)
{
    if (!SPIFFS.exists("/wifi_credentials.txt"))
    {
        Serial.println("Credentials file does not exist.");
        return false;
    }

    File file = SPIFFS.open("/wifi_credentials.txt", FILE_READ);
    if (!file)
    {
        Serial.println("Failed to open credentials file.");
        return false;
    }

    ssid = file.readStringUntil('\n');
    password = file.readStringUntil('\n');
    file.close();
    return true;
}

// Function to initialize SPIFFS
void WifiSetup::initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("Fatal Error: An Error has occurred while mounting SPIFFS!");
        return;
    }
}
