#include "joystick.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

#include "page.h"
#include <vector>

std::vector<int> baudRates = {300,   1200,  2400,   4800,   9600,   14400,  19200,
                              38400, 57600, 115200, 230400, 250000, 460800, 921600};

class SerialDraw : public Page
{
  private:
    HardwareSerial mySerial;

    int baudIndex = 0;

  public:
    SerialDraw(Esp32Input *esp32) : mySerial(1)
    {
        esp32->on<bool>(Esp32Input::Increment, [this](bool newV) {
            ESP_LOGI(L_TAG, "increment %d", newV);
            if (newV)
            {
                baudIndex--;
                drawBaudrate();
            }
        });
        esp32->on<bool>(Esp32Input::Decrement, [this](bool newV) {
            ESP_LOGI(L_TAG, "decrement %d", newV);
            if (newV)
            {
                baudIndex++;
                drawBaudrate();
            }
        });
    }

    bool frame()
    {
        return true;
    }

    String name()
    {
        return "SerialDraw";
    }

    void onSelected() override
    {
        baudIndex = 9;
        ESP_LOGI(L_TAG, "onSelected");
        screen->fillRect(0, 0, screen->width() - 1, 16, TFT_BLACK);

        drawBaudrate();
    }

    void drawBaudrate()
    {
        if (!selected)
        {
            return;
        }

        int baudrate = baudRates[baudIndex % baudRates.size()];
        ESP_LOGI(L_TAG, "baudrate draw %d", baudrate);
        screen->fillSprite(TFT_BLACK);
        int x = screen->drawString("baud rate:", 0, 0);
        screen->drawNumber(baudrate, x, 0);
        screen->setCursor(0, 10);
        mySerial.begin(baudRates[baudIndex], SERIAL_8N1, 41);
    }

    void draw()
    {

        while (mySerial.available() > 0)
        {
            // Read one byte
            char c = mySerial.read();
            // Print the received character back to serial

            screen->print(c);
        }
        int16_t cursorY = screen->getCursorY();
        if (cursorY > screen->height())
        {
            int16_t fontHeight = screen->fontHeight();
            screen->scroll(0, -fontHeight);
            screen->setCursor(0, cursorY - fontHeight);
        }
    }
};