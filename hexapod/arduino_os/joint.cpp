
#include "joint.h"

RobotJoint::RobotJoint() {}

int RobotJoint::firstRotateDelay = 0;

void RobotJoint::Set(int servoPin, float jointZero, bool jointDir, float jointMinAngle, float jointMaxAngle, int offsetAddress)
{
  this->servoPin = servoPin;
  this->jointZero = jointZero;
  this->jointDir = jointDir;
  this->offsetAddress = offsetAddress;
  this->jointMinAngle = jointMinAngle;
  this->jointMaxAngle = jointMaxAngle;

  int offsetInt = EEPROM.read(offsetAddress) * 256 + EEPROM.read(offsetAddress + 1);
  offsetInt = offsetInt / 2 * ((offsetInt % 2) ? 1 : -1);
  float offset = offsetInt * 0.01;
  this->offset = offset;
}

void RobotJoint::SetOffset(float offset)
{
  while (offset > 180)
    offset -= 360;
  while (offset < -180)
    offset += 360;

  int offsetInt = offset * 100;
  offsetInt = abs(offsetInt) * 2 + ((offset > 0) ? 1 : 0);

  if (offsetInt < 0 || offsetInt > 65535)
    return;

  EEPROM.write(offsetAddress, offsetInt / 256);
  EEPROM.write(offsetAddress + 1, offsetInt % 256);
  this->offset = offset;
}

void RobotJoint::SetOffsetEnableState(bool state)
{
  isOffsetEnable = state;
}

void RobotJoint::RotateToDirectly(float jointAngle)
{
  if (!CheckJointAngle(jointAngle))
    return;

  float servoAngle;

  if (isOffsetEnable)
    servoAngle = jointZero + (jointDir ? 1 : -1) * (jointAngle + offset);
  else
    servoAngle = jointZero + (jointDir ? 1 : -1) * jointAngle;

  while (servoAngle > 360)
    servoAngle -= 360;
  while (servoAngle < 0)
    servoAngle += 360;
  if (servoAngle > 180)
    return;

  if (isFirstRotate)
  {
    isFirstRotate = false;
    servo.attach(servoPin);
    servo.write(servoAngle);
    delay(firstRotateDelay);
  }
  else
  {
    servo.write(servoAngle);
  }

  jointAngleNow = jointAngle;
  servoAngleNow = servoAngle;
}

float RobotJoint::GetJointAngle(float servoAngle)
{
  return (jointDir ? 1 : -1) * (servoAngle - jointZero);
}

bool RobotJoint::CheckJointAngle(float jointAngle)
{
  while (jointAngle > jointMaxAngle)
    jointAngle -= 360;
  while (jointAngle < jointMinAngle)
    jointAngle += 360;

  if (jointAngle >= jointMinAngle && jointAngle <= jointMaxAngle)
    return true;
  else
    return false;
}
