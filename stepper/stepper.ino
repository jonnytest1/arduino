#include "Stepper.h"

#define STEPS  32   // Number of steps per revolution of Internal shaft
int  Steps2Take;  // 2048 = 1 Revolution

// https://www.arduino.cc/en/Reference/Stepper

Stepper small_stepper(STEPS, 8, 10, 9, 11);

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  small_stepper.setSpeed(500); //Max seems to be 500
    Steps2Take  =  2048;  // Rotate CW
    small_stepper.step(Steps2Take);
    delay(2000); 
}
