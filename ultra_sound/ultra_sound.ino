

#include "ultrasound.h"

UltraSound ultrasound;
// vcc => 5V
// Gnd => Gnd
//

void setup() {
  Serial.begin(9600);
  //Serial.println("init");
  ultrasound.Start(2);

  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");
}

void loop() {
  int distance = ultrasound.getDistance();
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}
