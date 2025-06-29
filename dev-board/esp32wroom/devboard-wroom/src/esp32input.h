
#include "bitmask.h"
#include "event_emitter.h"
#include "joystick.h"
#include "wire/Wire.h"
#include <Arduino.h>
#include <algorithm>
#include <functional>
#ifndef esp32ref
#define esp32ref
class Esp32Input : public EventEmitter
{

  private:
    TwoWire *i2cinput;
    uint8_t arduinoClientAddress = 0x08;

  public:
    bool i2cCheck = false;
    bool goti2c = false;

    Joystick stick;
    /**
     *  stickx default 126
     *  sticky default 125
     *  stickPress default 1
     *  switch mask1
     *  switch mask2
     */
    uint8_t dataWrite[5];
    uint8_t led_control[3] = {0, 0, 0};

    unsigned long last_received = -1;

    Bitmask top2Rows;
    Bitmask bottom2Rows;

    static inline auto Unknown = EventType<bool>::of("genericbutton");
    static inline auto ArrowUp = EventType<bool>::of("arrowup");
    static inline auto ArrowDown = EventType<bool>::of("arrowdown");
    static inline auto ArrowRight = EventType<bool>::of("arrowright");
    static inline auto ArrowLeft = EventType<bool>::of("arrowleft");
    static inline auto ArrowCenter = EventType<bool>::of("arrowcenter");

    static inline auto Increment = EventType<bool>::of("inc");
    static inline auto Decrement = EventType<bool>::of("dec");

    static inline auto Map = EventType<bool>::of("map");

    const std::vector<EventType<bool>> topRowEvents = //
        {
            //
            Esp32Input::Map,     Esp32Input::Unknown, Esp32Input::Unknown, Esp32Input::Increment, //
            Esp32Input::Unknown, Esp32Input::ArrowUp, Esp32Input::Unknown, Esp32Input::Decrement,
        };
    const std::vector<EventType<bool>> bottomRowEvents = //
        {
            //
            Esp32Input::ArrowLeft, Esp32Input::ArrowCenter, Esp32Input::ArrowRight, Esp32Input::Unknown, //
            Esp32Input::Unknown,   Esp32Input::ArrowDown,   Esp32Input::Unknown,    Esp32Input::Unknown,
        };

    Esp32Input(TwoWire *w) : i2cinput(w)
    {

        //
    }

    void setup()
    {
        stick.deadZone = 70;

        top2Rows.on<BitChanged>(Bitmask::BitChangedEvent,
                                [this](BitChanged evt) { emit(topRowEvents[evt.index], evt.currentValue); });
        bottom2Rows.on<BitChanged>(Bitmask::BitChangedEvent,
                                   [this](BitChanged evt) { emit(bottomRowEvents[evt.index], evt.currentValue); });

        i2cinput->onReceive([this](int bytesCt) {
            last_received = millis();

            if (!i2cCheck)
            {
                ESP_LOGI(L_TAG, "i2c received");
                i2cCheck = true;
                goti2c = true;
            }

            for (int i = 0; i < std::min(bytesCt, 5); i++)
            {
                char c = i2cinput->read();

                if (c != dataWrite[i])
                {
                    if (i == 3)
                    {
                        top2Rows.update(c);
                        // button bitmasks
                        // emit(EventType<char>::of((String("evt") + i).c_str()), c);
                    }
                    else if (i == 4)
                    {
                        bottom2Rows.update(c);
                    }
                    else
                    {
                        emit(EventType<char>::of((String("evt") + i).c_str()), c);
                    }
                }

                dataWrite[i] = c;
            }
            String buff = "";

            /*for (int c : dataWrite)
            {
                for (int i = 7; i >= 0; i--)
                {
                    buff += String((c >> i) & 1);
                }
                buff += String(",");
            }
            ESP_LOGI(L_TAG, "state %s", buff.c_str());*/
            // for (int i = 0; i <5;i++){
            // ESP_LOGI(L_TAG, "i2c begin esp");
            // }
            stick.update(dataWrite[1] + 10, 255 - dataWrite[0] - 20);
        });

        i2cinput->onRequest([this]() { i2cinput->write(1); });

        ESP_LOGI(L_TAG, "i2c begin esp");

        i2cinput->begin(arduinoClientAddress, 15, 16, 100000);
        i2cinput->setBufferSize(512);
        i2cinput->setTimeout(50);
        ESP_LOGI(L_TAG, "i2c end esp");
    }
};
#endif