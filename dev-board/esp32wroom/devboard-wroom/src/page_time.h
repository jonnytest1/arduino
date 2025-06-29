#include "esp_log.h"
#include "page.h"
#include <Arduino.h>
#include <Preferences.h>
#include <RTClib.h>
String padLeft(int num, int width, char padChar = '0')
{
    String str = String(num);
    while (str.length() < width)
    {
        str = String(padChar) + str;
    }
    return str;
}

static uint8_t conv2d(const char *p)
{
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

class TimePage : public Page
{

  private:
    uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3),
            ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time

    byte omm = 99, oss = 99;
    byte xcolon = 0, xsecs = 0;

  public:
    uint32_t targetTime = 0; // for next 1 second timeout
    TwoWire *i2c;
    RTC_DS1307 rtc;

    bool rtcFailed = false;

    bool redraw = false;

    TimePage(TwoWire *twire) : i2c(twire)
    {
    }

    void beginRtc()
    {
        for (int i = 0; i < 5; i++)
        {
            if (rtc.begin(i2c))
            {
                return;
            }
            ESP_LOGI(L_TAG, "rtc:retrying");
            delay(50);
        }
        rtcFailed = true;
    }

    void setup()
    {
        i2c->setTimeout(50);
        i2c->begin(20, 21);
        beginRtc();
        if (rtcFailed)
        {
            return;
        }

        if (!rtc.isrunning())
        {
            Serial.println("RTC is NOT running, let's set the time!");
            rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC to compile time
        }

        Preferences preferences;
        preferences.begin("app", false);
        String storedBuild = preferences.getString("buildDateTime", "");
        String currentBuild = String(__DATE__) + " " + String(__TIME__);

        bool firstRun = (storedBuild != currentBuild);

        if (firstRun)
        {
            preferences.putString("buildDateTime", currentBuild);
            DateTime compileTime(F(__DATE__), F(__TIME__));
            DateTime adjustedTime = compileTime + TimeSpan(10);
            rtc.adjust(adjustedTime);
        }
        preferences.end();

        DateTime now = rtc.now();
        // DateTime local = now + TimeSpan(tz_offset_hours * 3600);
        ESP_LOGI(L_TAG, "rtc:%s %d::%d::%d - y%d", rtc.isrunning() ? "running" : "stopped", now.hour(), now.minute(),
                 now.second(), now.year());
    }

    bool frame()
    {
        return true;
    }

    String name()
    {
        return "Time";
    }

    void drawDate(DateTime *dt)
    {
        int textHeight = 72;

        int ypos = screen->height() / 2 - (textHeight / 2) - 30; // Top left corner ot clock text, about half way down

        auto month = padLeft(dt->month(), 2, '0');
        auto day = padLeft(dt->day(), 2, '0');

        screen->drawString(String("") + dt->year() + "-" + month + "-" + day, 0, ypos, 4);
    }
    void increaseTime()
    {

        ss++;
        if (ss >= 60)
        {
            ss = 0;
            mm++;
            if (mm >= 60)
            {
                mm = 0;
                hh++;
            }
        }
    }
    void draw()
    {
        if (targetTime < millis() || redraw)
        {
            // Set next update for 1 second later
            targetTime = millis() + 1000;

            if (rtcFailed)
            {
                increaseTime();
                mm = 55;
                hh = 88;
                screen->setTextColor(TFT_ORANGE, TFT_BLACK);
            }
            else
            {

                DateTime now = rtc.now();
                drawDate(&now);

                ss = now.second();
                mm = now.minute();
                hh = now.hour();

                screen->setTextColor(TFT_YELLOW, TFT_BLACK);
            }

            //

            // Update digital time
            int textHeight = 72;
            int xpos = 0;

            int ypos = screen->height() / 2 - (textHeight / 2); // Top left corner ot clock text, about half way down
            int ysecs = ypos + 24;

            screen->fillRect(0, ypos - 2, screen->width(), textHeight + 4, TFT_BLACK); // Hour:minute colon

            // Redraw hours and minutes time every minute

            omm = mm;
            // Draw hours and minutes
            if (hh < 10)
                xpos += screen->drawChar('0', xpos, ypos, 8); // Add hours leading zero for 24 hr clock
            xpos += screen->drawNumber(hh, xpos, ypos, 8);    // Draw hours
            xcolon = xpos;                                    // Save colon coord for later to flash on/off later
            xpos += screen->drawChar(':', xpos, ypos - 8, 8);
            if (mm < 10)
                xpos += screen->drawChar('0', xpos, ypos, 8); // Add minutes leading zero
            xpos += screen->drawNumber(mm, xpos, ypos, 8);    // Draw minutes

            // xsecs = xpos; // Sae seconds 'x' position for later display updates

            if (oss != ss || redraw)
            { // Redraw seconds time every second

                oss = ss;
                // xpos = xsecs;

                if (ss % 2)
                {                                                  // Flash the colons on/off
                    screen->setTextColor(0x39C4, TFT_BLACK);       // Set colour to grey to dim colon
                    screen->drawChar(':', xcolon, ypos - 8, 8);    // Hour:minute colon
                    xpos += screen->drawChar(':', xpos, ysecs, 6); // Seconds colon
                    screen->setTextColor(TFT_YELLOW, TFT_BLACK);   // Set colour back to yellow
                }
                else
                {
                    screen->drawChar(':', xcolon, ypos - 8, 8);    // Hour:minute colon
                    xpos += screen->drawChar(':', xpos, ysecs, 6); // Seconds colon
                }

                // Draw seconds
                if (ss < 10)
                    xpos += screen->drawChar('0', xpos, ysecs, 6); // Add leading zero
                screen->drawNumber(ss, xpos, ysecs, 6);            // Draw seconds
            }
        }
        redraw = false;
    }
};