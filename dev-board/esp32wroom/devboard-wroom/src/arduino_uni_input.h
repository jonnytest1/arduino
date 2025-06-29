
/*
#include "event_emitter.h"
#include "joystick.h"
#include <Arduino.h>
#include <algorithm>
#include <functional>
#ifndef ardref
#define ardref
class ArduinoUno : public EventEmitter
{

  private:
    // TwoWire i2cinput;
    uint8_t arduinoClientAddress = 0x08;

  public:
    bool i2cCheck = false;
    bool goti2c = false;
    Joystick stick;
    /**
     *  pot1
     *  pot2
     *  stickx default 126
     *  sticky default 125
     *  stickPress default 1
     *  switch1 default 1
     *  switch2 default 1
     *  switch3 default 1
     *
    uint8_t dataWrite[8];
    uint8_t led_control[3] = {0, 0, 0};

    inline static auto EVENT_POT_1 = EventType<char>::of("evt0");

    inline static auto EVENT_1 = EventType<char>::of("evt1");

    inline static auto EVENT_2 = EventType<char>::of("evt2");
    inline static auto EVENT_3 = EventType<char>::of("evt3");
    inline static auto EVENT_4 = EventType<char>::of("evt4");
    inline static auto EVENT_SWITCH_1 = EventType<char>::of("evt5");
    inline static auto EVENT_SWITCH_2 = EventType<char>::of("evt6");

    inline static auto EVENT_SWITCH_3 = EventType<char>::of("evt7");

    unsigned long last_received = -1;

    ArduinoUno()
    {
        // : i2cinput(0)
    }

    void begini2c()
    {
        for (int i = 0; i < 10; i++)
        {
            //  if (i2cinput.begin(arduinoClientAddress, 17, 46, 100000))
            //  {
            //     ESP_LOGI(L_TAG, "i2c begin sucess");
            //      return;
            // }
            //  delay(50);
        }
    }

    void setup()
    {
        /*i2cinput.onReceive([this](int bytesCt) {
            last_received = millis();

            if (!i2cCheck)
            {
                ESP_LOGI(L_TAG, "i2c received");
                i2cCheck = true;
                goti2c = true;
            }

            for (int i = 0; i < std::min(bytesCt, 8); i++)
            {
                char c = i2cinput.read();

                if (c != dataWrite[i])
                {
                    emit(EventType<char>::of((String("evt") + i).c_str()), c);
                }

                /* if (i == 0 && c != dataWrite[i])
                {
                    redraw = true;
                }

                *
        dataWrite[i] = c;
    }
    stick.update(dataWrite[2], dataWrite[3]);

    led_control[0] = dataWrite[2];
    led_control[1] = dataWrite[3];
    // led 2 is binary >127 is on
    led_control[2] = dataWrite[7];
        });

        i2cinput.onRequest([this]() { i2cinput.write(led_control, 3); });

        i2cinput.setBufferSize(512);
        ESP_LOGI(L_TAG, "i2c begin uno");
        begini2c();
        ESP_LOGI(L_TAG, "i2c begin end uno");
        i2cinput.setTimeout(50);
        *
    }
};

#endif*/