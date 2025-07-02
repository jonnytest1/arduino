
#include <Arduino.h>

#ifndef jstick
#define jstick
#include "event_emitter.h"

template <typename T> using ChangeCallback = std::function<void(T newVal)>;

template <typename T> class ChangeRef : public EventEmitter
{
  public:
    inline static auto CHANGE_EVENT = EventType<T>::of("change");
    ChangeRef(T initial) : value(initial)
    {
    }

    T value;

    void update(T newValue)
    {
        if (newValue != value)
        {
            ESP_LOGI(L_TAG, "change evt");
            emit(ChangeRef::CHANGE_EVENT, newValue);
        }
        value = newValue;
    };
};

class BoolChangeRef : public ChangeRef<bool>
{
  public:
    inline static auto const TRUE_EVENT = EventType<bool>::of("true");
    inline static auto const FALSE_EVENT = EventType<bool>::of("false");

    BoolChangeRef() : ChangeRef(false)
    {

        on<bool>(ChangeRef<bool>::CHANGE_EVENT, [this](bool newValue) {
            if (newValue)
            {
                emit(BoolChangeRef::TRUE_EVENT, true);
            }
            if (!newValue)
            {
                emit(BoolChangeRef::FALSE_EVENT, true);
            }
        });
    }
};

class Joystick
{
  private:
    int last_x = -1;
    int last_y = -1;

  public:
    int deadZone = 40;
    BoolChangeRef x_left;
    BoolChangeRef x_right;

    BoolChangeRef y_up;
    BoolChangeRef y_down;
    Joystick()
    {
    }

    void update(int x, int y)
    {
        x = x - 126;
        y = y - 125;
        // ESP_LOGI(L_TAG, "joystick %d %d", x, y);

        if (x != last_x)
        {
            x_left.update(x < -deadZone);
            x_right.update(x > deadZone);
        }

        if (y != last_y)
        {
            y_up.update(y < -deadZone);
            y_down.update(y > deadZone);
        }

        last_x = x;
        last_y = y;
    };
};

#endif