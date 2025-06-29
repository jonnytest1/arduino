

#include "esp32input.h"
#include "esp_heap_caps.h"
#include "joystick.h"
#include "page.h"
#include "page_serial.h"
#include "page_time.h"
#include "tftSetup.h"
#include "wire/Wire.h"
#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

Esp32Input esp32Input(&Wire);

TFT_eSPI tft = TFT_eSPI(320, 480);

TFT_eSprite *pages_map = new TFT_eSprite(&tft);

bool map_active = false;

byte red = 31;
byte green = 0;
byte blue = 0;
byte state = 0;
unsigned int colour = red << 11; // Colour order is RGB 5+6+5 bits each

Pages pages(&esp32Input.stick, &tft);

SerialDraw sDraw(&esp32Input);
TimePage timePage(&Wire1);

void rainbow_fill()
{
    // The colours and state are not initialised so the start colour changes each time the function is called

    for (int i = 479; i > 0; i--)
    {
        // Draw a vertical line 1 pixel wide in the selected colour
        tft.drawFastHLine(0, i, tft.width(),
                          colour); // in this example tft.width() returns the pixel width of the display
        // This is a "state machine" that ramps up/down the colour brightnesses in sequence
        switch (state)
        {
        case 0:
            green++;
            if (green == 64)
            {
                green = 63;
                state = 1;
            }
            break;
        case 1:
            red--;
            if (red == 255)
            {
                red = 0;
                state = 2;
            }
            break;
        case 2:
            blue++;
            if (blue == 32)
            {
                blue = 31;
                state = 3;
            }
            break;
        case 3:
            green--;
            if (green == 255)
            {
                green = 0;
                state = 4;
            }
            break;
        case 4:
            red++;
            if (red == 32)
            {
                red = 31;
                state = 5;
            }
            break;
        case 5:
            blue--;
            if (blue == 255)
            {
                blue = 0;
                state = 0;
            }
            break;
        }
        colour = red << 11 | green << 5 | blue;
    }
}

void prepareMap()
{
    ESP_LOGI(L_TAG, "map");
    int width = pages_map->width();
    int height = pages_map->height();
    // ESP_LOGI(L_TAG, "%d", width);
    pages_map->drawRect(0, 0, width, height, TFT_LIGHTGREY);

    int cols = pages.pages.size();
    int spacing = height / cols;
    // ESP_LOGI(L_TAG, "h: %d s:%d cols:%d", height, spacing, cols);

    for (int i = 0; i < cols; i++)
    {

        int yLEvel = (spacing) * (i + 1);
        if (i < cols - 1)
        {
            // ESP_LOGI(L_TAG, "y: %d ", yLEvel);
            pages_map->drawLine(0, yLEvel, width, yLEvel, TFT_LIGHTGREY);
        }

        auto rows = pages.pages[i];

        int rowCt = rows.size();

        int rowSpacing = width / rowCt;
        pages_map->setTextDatum(MC_DATUM);

        for (int j = 0; j < rowCt; j++)
        {

            int xLEvel = (rowSpacing) * (j + 1);
            if (j < rowCt - 1)
            {
                // ESP_LOGI(L_TAG, "y: %d ", yLEvel);
                pages_map->drawLine(xLEvel, yLEvel - spacing, xLEvel, yLEvel, TFT_LIGHTGREY);
            }

            auto element = rows[j];
            int16_t x1, y1;
            uint16_t w, h;
            int width = pages_map->textWidth(element->name());

            int textx = xLEvel - (rowSpacing / 2);
            int texty = yLEvel - (spacing / 2);
            // ESP_LOGI(L_TAG, "printing : %s w: %d x:%d y:%d", element->name(), width, textx, texty);
            if (element->selected)
            {
                pages_map->setTextColor(TFT_GREEN, TFT_BLACK);
            }
            else
            {
                pages_map->setTextColor(TFT_YELLOW, TFT_BLACK);
            }

            pages_map->drawString(element->name(), textx, texty);
        }
        pages_map->setTextDatum(TL_DATUM);
    }
}

void setup()
{
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    Serial.begin(115200);
    delay(100);
    ESP_LOGI(L_TAG, "serial c++17");
    // unoRef.setup();

    esp32Input.on<bool>(Esp32Input::Unknown, [](bool pressed) { ESP_LOGI(L_TAG, "button changed"); });

    esp32Input.on<bool>(Esp32Input::Map, [](bool pressed) {
        if (pressed)
        {
            ESP_LOGI(L_TAG, "map active");
            map_active = true;
        }
        else
        {
            ESP_LOGI(L_TAG, "map deactivate");
            map_active = false;
        }
    });
    esp32Input.stick.x_right.on<bool>(BoolChangeRef::TRUE_EVENT, [](bool val) { ESP_LOGI(L_TAG, "stick x_right"); });
    esp32Input.stick.x_left.on<bool>(BoolChangeRef::TRUE_EVENT, [](bool val) { ESP_LOGI(L_TAG, "stick x_left"); });
    esp32Input.stick.y_up.on<bool>(BoolChangeRef::TRUE_EVENT, [](bool val) { ESP_LOGI(L_TAG, "stick y_up"); });
    esp32Input.stick.y_down.on<bool>(BoolChangeRef::TRUE_EVENT, [](bool val) { ESP_LOGI(L_TAG, "stick y_down"); });

    ESP_LOGI(L_TAG, "heap %d", esp_get_free_heap_size());

    ESP_LOGI(L_TAG, "tft begin");
    tft.begin();
    ESP_LOGI(L_TAG, "tft rotation");
    tft.setRotation(3);
    ESP_LOGI(L_TAG, "tft fill");
    tft.fillScreen(TFT_BLACK);

    tft.setTextSize(1);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    timePage.setup();
    timePage.targetTime = millis() + 1000;
    ESP_LOGI(L_TAG, "setting pages ...");
    pages.set(0, 0, &timePage);
    pages.set(0, 1, new CallbackPage(rainbow_fill));
    pages.set(1, 0, &sDraw);
    pages_map->createSprite(tft.width() - 24, tft.height() - 24);
    pages_map->setTextSize(1);
    pages_map->setTextColor(TFT_YELLOW, TFT_BLACK);
    pages_map->setTextDatum(TL_DATUM);
    pages_map->setTextFont(1);
    ESP_LOGI(L_TAG, "sprite created heap:%d", esp_get_free_heap_size());
    ESP_LOGI(L_TAG, "total heap %d psram:%d", heap_caps_get_total_size(MALLOC_CAP_8BIT),
             heap_caps_get_total_size(MALLOC_CAP_SPIRAM));

    prepareMap();
    ESP_LOGI(L_TAG, "heap %d", esp_get_free_heap_size());

    pages.on<bool>(Pages::PAGE_CHANGE, [](bool evt) { prepareMap(); });

    esp32Input.setup();
}
uint16_t interpolateColor(uint16_t color1, uint16_t color2, float t)
{
    // Extract RGB components from color1
    uint8_t r1 = (color1 >> 11) & 0x1F; // 5 bits red
    uint8_t g1 = (color1 >> 5) & 0x3F;  // 6 bits green
    uint8_t b1 = color1 & 0x1F;         // 5 bits blue

    // Extract RGB components from color2
    uint8_t r2 = (color2 >> 11) & 0x1F;
    uint8_t g2 = (color2 >> 5) & 0x3F;
    uint8_t b2 = color2 & 0x1F;

    // Interpolate each component
    uint8_t r = r1 + (r2 - r1) * t;
    uint8_t g = g1 + (g2 - g1) * t;
    uint8_t b = b1 + (b2 - b1) * t;

    // Recombine into 16-bit color
    return (r << 11) | (g << 5) | b;
}
void loop()
{

    delay(20);

    tft.setTextDatum(TR_DATUM);

    if (esp32Input.goti2c)
    {
        float duration = millis() - esp32Input.last_received;
        float percentage = std::min(((duration / 5000)), 1.0f);
        if (percentage == 1.0f && ((int)duration % 5) == 0)
        {
            // ESP_LOGI(L_TAG, "ic percent esp32 %f %f %f", percentage, duration, unoRef.last_received);
        }
        uint16_t fg = interpolateColor(TFT_GREEN, TFT_BROWN, percentage);
        tft.setTextColor(fg, TFT_BLACK);
    }
    else
    {
        tft.setTextColor(TFT_RED, TFT_BLACK);
    }
    tft.drawString("i2c_esp", tft.width(), 0);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);

    if (!map_active)
    {
        pages.update();
    }
    else
    {
        if (pages_map != nullptr)
        {
            pages_map->pushSprite(12, 12);
        }
    }
}
