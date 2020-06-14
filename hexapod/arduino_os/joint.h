

#include <Arduino.h>

#include <Servo.h>
#include <EEPROM.h>


class RobotJoint
{
public:
  RobotJoint();
  void Set(int servoPin, float jointZero, bool jointDir, float jointMinAngle, float jointMaxAngle, int offsetAddress);

  void SetOffset(float offset);
  void SetOffsetEnableState(bool state);

  void RotateToDirectly(float jointAngle);

  float GetJointAngle(float servoAngle);

  bool CheckJointAngle(float jointAngle);

  volatile float jointAngleNow;
  volatile float servoAngleNow;

  static int firstRotateDelay;

private:
  Servo servo;
  int servoPin;
  float jointZero;
  bool jointDir;
  float jointMinAngle;
  float jointMaxAngle;
  int offsetAddress;
  volatile float offset = 0;
  volatile bool isOffsetEnable = true;
  volatile bool isFirstRotate = true;
};
