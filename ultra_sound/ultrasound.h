#pragma once
#include <Arduino.h>
class UltraSound {

  public:
    UltraSound();

    /*
      Brief     Start the robot
      Param     commFunction  Whether to open communication function
                  true        The robot will be controlled by remote, Android APP or Processing APP
                  false       You can only use the member functions of this class to control the robot
      Retval    None
      -----------------------------------------------------------------------------------------------*/
    void Start(int echoPin); // triggerPin = echoPin +1
    void Start(int echoPin, int trigPin);

    int getDistance();

  private:
    int echoPin;
    int triggerPin;
};
