/*
 * Sketch     Remote
 * Platform   Freenove Remote Control (Compatible with Arduino/Genuino Uno)
 * Brief      This sketch is used to show how to use Freenove Remote Control.
 *            This sketch needs to be uploaded to the board of the remote.
 * Author     Ethan Pan @ Freenove (support@freenove.com)
 * Date       2023/01/07
 * Copyright  Copyright © Freenove (http://www.freenove.com)
 * License    Creative Commons Attribution ShareAlike 3.0
 *            (http://creativecommons.org/licenses/by-sa/3.0/legalcode)
 * -----------------------------------------------------------------------------------------------*/

#ifndef ARDUINO_AVR_UNO
#error Wrong board. Please choose "Arduino/Genuino Uno"
#endif

#include <Wire.h>

// wireless module
             // define the object to control wireless module
//RF24 radio(9, 10);   

uint8_t dataWrite[8];                 // define array used to save the write data
// pin
const int pot1Pin = A0,           // define POT1
          pot2Pin = A1,           // define POT2
          joystickXPin = A2,      // define pin for direction X of joystick
          joystickYPin = A3,      // define pin for direction Y of joystick
          joystickZPin = 7,       // define pin for direction Z of joystick
          s1Pin = 4,              // define pin for S1
          s2Pin = 3,              // define pin for S2
          s3Pin = 2,              // define pin for S3
          led1Pin = 6,            // define pin for LED1 which is close to POT1 and used to indicate the state of POT1
          led2Pin = 5,            // define pin for LED2 which is close to POT2 and used to indicate the state of POT2
          led3Pin = 8;            // define pin for LED3 which is close to wireless module and used to indicate the state of it

void setup() {
   Serial.begin(9600);
  // wireless module
  pinMode(12, INPUT_PULLUP);
  // pin
  pinMode(joystickZPin, INPUT);         // set led1Pin to input mode
  pinMode(s1Pin, INPUT);                // set s1Pin to input mode
  pinMode(s2Pin, INPUT);                // set s2Pin to input mode
  pinMode(s2Pin, INPUT);                // set s3Pin to input mode
  pinMode(led1Pin, OUTPUT);             // set led1Pin to output mode
  pinMode(led2Pin, OUTPUT);             // set led2Pin to output mode
  pinMode(led3Pin, OUTPUT);             // set led3Pin to output mode

  delay(1000); // Wait for ESP32 to get ready
  Wire.setClock(100000);
  Wire.begin();
  Wire.setTimeout(50); 
}

unsigned long last_request=0;
bool failedRead=false;
bool reconnected=false;

void loop() {

  uint8_t pot1= analogRead(pot1Pin)/4;
  uint8_t pot2= analogRead(pot2Pin)/4;
  uint8_t joystickX=analogRead(joystickXPin)/4;
  uint8_t joystickY=analogRead(joystickYPin)/4;
  uint8_t jZ=digitalRead(joystickZPin);
  uint8_t s1=digitalRead(s1Pin);
  uint8_t s2=digitalRead(s2Pin);
  uint8_t s3=digitalRead(s3Pin);

  bool changed=false;

  if(pot1!=dataWrite[0]||pot2!=dataWrite[1]){
     changed=true;
  }
  if(joystickX!=dataWrite[2]||joystickY!=dataWrite[3]||jZ!=dataWrite[4]){
     changed=true;
  }
  if(s1!=dataWrite[5]||s2!=dataWrite[6]||s3!=dataWrite[7]){
     changed=true;
  }

  dataWrite[0] = pot1;
  dataWrite[1] = pot2;
  dataWrite[2] = joystickX;
  dataWrite[3] = joystickY;
  dataWrite[4] = jZ;
  dataWrite[5] = s1;
  dataWrite[6] = s2;
  dataWrite[7] = s3;
  
  // put the values of rocker, switch and potentiometer into the array
  
 
 // digitalWrite(led3Pin, LOW);
  //delay(1000);
  //digitalWrite(led3Pin, HIGH);
  //delay(1000);

  // make LED emit different brightness of light according to analog of potentiometer
  //analogWrite(led1Pin, map(dataWrite[0], 0, 1023, 0, 255));
  //analogWrite(led2Pin, map(dataWrite[1], 0, 1023, 0, 255));


  if(changed||reconnected){
    Serial.print(changed);
    Serial.print(",");
    Serial.print(reconnected);
    Serial.println("sending");
    reconnected=false;
    Wire.beginTransmission(0x08); // Address of ESP32
    //Wire.write(1);              // Send a character or string
    
    Wire.write(dataWrite,8);              // Send a character or string
    Wire.endTransmission();
  }
  int bytesRead=Wire.requestFrom(0x08, 3); // Read up to 16 bytes
  Serial.println(bytesRead);
  if(bytesRead==0){
    failedRead=true;
  }else {
    if(failedRead){
      reconnected=true;
    }
    failedRead=false;
  }


  for(int i=0;i<3;i++){
    int c = Wire.read();

    int pin;
    if(i==0){
      pin=led1Pin;
    }else if(i==1){
      pin=led2Pin;
    }else if(i==2){
      pin= led3Pin ;
    }
    analogWrite(pin, c);
  }
  delay(30);

  
}
