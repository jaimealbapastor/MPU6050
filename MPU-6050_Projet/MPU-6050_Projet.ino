#include <Wire.h>
#include "MPU6050_Angle.h"

MPU6050_Angle angleSensor;

void setup() {
  Serial.begin(9600);
  angleSensor.begin();
}

void loop() {
  angleSensor.readAngle();
}
