#include "wi_fi.h"
#include <WiFi.h>
#include "arduinoref.h"
#include "prop.h"
#include "str.h"

std::string localIp = "";

void waitForWifi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }
    char hostname[32];
    strcpy(hostname, "c-esp32-");
    strcat(hostname, getDeviceKey().c_str());

    WiFi.setHostname(hostname);
    Serial.println(String("hostame : ") + hostname);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.println("setting hostname ... ");

    WiFi.begin(getWlanSSID().c_str(), getWlanPWD().c_str());
    int t = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        t++;
        delay(100);
        Serial.println("connecting ... " + WiFi.status());
        if (t > 50)
        {
            Serial.print("Connecting failed");
            return;
        }
    }
    Serial.println("WiFi connected.");
    String ip = WiFi.localIP().toString();
    localIp = ip.c_str();
    Serial.println(ip);
}

std::string getDeviceIp()
{
    return localIp;
}