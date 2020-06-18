
#include "ultrasound.h"
UltraSound::UltraSound() {}

void UltraSound::Start(int echo, int trigger) {
  echoPin = echo;
  triggerPin = trigger;
  pinMode(triggerPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
}
void UltraSound::Start(int echo) {
  Start(echo, echo + 1);
}
int UltraSound::getDistance()
{
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  return duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
}
