#include "MPU6050-compFilter.h"
#include <Wire.h>

MPU6050_compFilter MPU;
acc_t_gyro_union accel_t_gyro_data;
euler_angles angles;

//---------------------------------------------------------------------
//  SETUP  ------------------------------------------------------------
//---------------------------------------------------------------------

void setup()
{
  int error;
  uint8_t c;

  Serial.begin(9600);
  Wire.begin();
  MPU.begin();

  // error = MPU.read(MPU6050_WHO_AM_I, &c, 1);
  // Serial.print(F("WHO_AM_I : "));
  // Serial.print(c, HEX);
  // Serial.print(F(", error = "));
  // Serial.println(error, DEC);

  // Initialize the angles
  Serial.print(F("Calibrating... "));
  MPU.calibrate_gyro_offset();
  Serial.println(F("Done"));

  // empty RX buffer
  while (Serial.available())
    Serial.read();
}

//---------------------------------------------------------------------
//  LOOP  ------------------------------------------------------------
//---------------------------------------------------------------------

void loop()
{
  delay(200);
  MPU.compute_angle_estimations();
  MPU.get_euler_angles(&angles);

  Serial.print(angles.roll, DEC);
  Serial.print(F(";"));
  Serial.print(angles.pitch, DEC);
  Serial.print(F(";"));
  Serial.println(angles.yaw, DEC);
}