#pragma once
#include "Arduino.h"
#include "event_emitter.h"

struct BitChanged
{
    uint8_t index;
    bool currentValue;
};

class Bitmask : public EventEmitter
{
  private:
    uint8_t previous;

  public:
    static inline auto BitChangedEvent = EventType<BitChanged>::of("bitchanged");
    void update(uint8_t bits)
    {
        for (int i = 7; i >= 0; i--)
        {
            bool current = (bits >> i) & 1;
            bool prev = (previous >> i) & 1;

            uint8_t maskIndex = 7 - i;
            if (current != prev)
            {

                emit(BitChangedEvent, {maskIndex, current});
                // ESP_LOGI(L_TAG, "bit changed %d %s", maskIndex, current ? "pressed" : "released");
            }
        }
        previous = bits;
    }
};