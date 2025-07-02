
#include <Arduino.h>
#include <Wire.h>
/**
 * @brief
 *
 * 0 joyx
 * 1 joyy
 * 2 joypress
 * 3 button r1 r2
 * 4 button r3 r4
 * 5 joy2x
 * 6 joy2y
 * 7 joy2pressed
 */
uint8_t dataWrite[8]; // define array used to save the write data
// pin
const int joystickXPin = A4, // 35 define pin for direction X of joystick
    joystickYPin = A5,       // 32 define pin for direction Y of joystick
    joystickZPin = 25;       // 33 define pin for direction Z of joystick

const int joystick2XPin = 36, //
    joystick2YPin = 39,       //
    joystick2ZPin = 21;

const int rows[4] = {15, 2, 4, 16};  // Row pins
const int cols[4] = {17, 5, 18, 19}; // Column pins

void setup()
{
    Serial.begin(115200);
    delay(100);
    Serial.println("start serial");

    // pin
    pinMode(joystickZPin, INPUT);  // set led1Pin to input mode
    pinMode(joystick2ZPin, INPUT); // set led1Pin to input mode
    Serial.println("joystick setup");
    // pinMode(s1Pin, INPUT);        // set s1Pin to input mode
    // pinMode(s2Pin, INPUT);        // set s2Pin to input mode
    // pinMode(s2Pin, INPUT);        // set s3Pin to input mode

    delay(50); // Wait for ESP32 to get ready

    for (int i = 0; i < 4; i++)
    {
        pinMode(rows[i], OUTPUT);
        digitalWrite(rows[i], HIGH); // Default to HIGH (inactive)
    }

    // Set columns as INPUT_PULLUP
    for (int i = 0; i < 4; i++)
    {
        pinMode(cols[i], INPUT_PULLUP);
    }
    Serial.println("buttons setup");

    Serial.println("setup... i2c");
    Wire.setClock(100000);
    Wire.begin(13, 12, 100000);
    Wire.setTimeout(50);
}

unsigned long last_request = 0;
bool failedRead = false;
bool reconnected = false;

bool readButtons()
{

    bool changed = false;
    for (int r = 0; r < 4; r++)
    {
        digitalWrite(rows[r], LOW);

        for (int c = 0; c < 4; c++)
        {

            int buttonIndex = r * 4 + c; // 0 to 15
            int bitIndex = buttonIndex % 8;
            int byteIndex = 1 - buttonIndex / 8;
            bool wasPressed = (dataWrite[byteIndex + 3] >> bitIndex) & 1;

            bool isPressed = digitalRead(cols[c]) == LOW;

            if (wasPressed != isPressed)
            {
                changed = true;
                if (isPressed)
                {
                    dataWrite[byteIndex + 3] |= (1 << bitIndex); // set bit
                }
                else
                {
                    dataWrite[byteIndex + 3] &= ~(1 << bitIndex); // clear bit
                }
            }
        }
        digitalWrite(rows[r], HIGH);
    }
    return changed;
}

unsigned long lastPrint = -1;

void loop()
{

    bool changed = readButtons();

    uint8_t joystickX = map(analogRead(joystickXPin), 0, 4095, 0, 255);
    uint8_t joystickY = map(analogRead(joystickYPin), 0, 4095, 0, 255);
    uint8_t jZ = digitalRead(joystickZPin);
    if (joystickX != dataWrite[2] || joystickY != dataWrite[3] || jZ != dataWrite[4])
    {
        changed = true;
    }
    uint8_t joystick2X = map(analogRead(joystick2XPin), 0, 4095, 0, 255);
    uint8_t joystick2Y = map(analogRead(joystick2YPin), 0, 4095, 0, 255);
    uint8_t j2Z = digitalRead(joystick2ZPin);
    if (joystick2X != dataWrite[5] || joystick2Y != dataWrite[6] || j2Z != dataWrite[7])
    {
        changed = true;
    }
    dataWrite[0] = joystickX;
    dataWrite[1] = joystickY;
    dataWrite[2] = jZ;
    dataWrite[5] = joystick2X;
    dataWrite[6] = joystick2Y;
    dataWrite[7] = j2Z;

    if (millis() - lastPrint > 1000)
    {
        lastPrint = millis();
        for (int c : dataWrite)
        {
            for (int i = 7; i >= 0; i--)
            {
                Serial.print((c >> i) & 1);
            }
            Serial.print(",");
        }
        Serial.println(" - state");
    }

    if (changed || reconnected)
    {
        /*Serial.print(changed);
        Serial.print(",");
        Serial.print(reconnected);
        Serial.println("sending");*/
        reconnected = false;
        Wire.beginTransmission(0x08); // Address of ESP32
        // Wire.write(1);              // Send a character or string

        Wire.write(dataWrite, 8); // Send a character or string
        Wire.endTransmission();
    }
    /* int bytesRead = Wire.requestFrom(0x08, 1); // Read up to 16 bytes
     Serial.println(bytesRead);
     if (bytesRead == 0)
     {
         failedRead = true;
     }
     else
     {
         if (failedRead)
         {
             reconnected = true;
         }
         failedRead = false;
     }
*/
    delay(100);
}
