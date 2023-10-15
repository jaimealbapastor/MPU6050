#include "MPU6050-compFilter.h"
#include <Wire.h>

MPU6050_compFilter MPU;

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
  MPU.calibrate_sensors();
  MPU.set_last_read_angle_data(millis(), 0, 0, 0, 0, 0, 0);
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
  int error;
  uint8_t c;

  accel_t_gyro_union accel_t_gyro;
  error = MPU.read_gyro_accel_vals((uint8_t *)&accel_t_gyro);

  unsigned long t_now = millis();

  // Print the raw acceleration values
  // Serial.print(F("accel x,y,z: "));
  // Serial.print(accel_t_gyro.value.x_accel, DEC);
  // Serial.print(F(", "));
  // Serial.print(accel_t_gyro.value.y_accel, DEC);
  // Serial.print(F(", "));
  // Serial.println(accel_t_gyro.value.z_accel, DEC);

  // Convert gyro values to degrees/sec
  float FS_SEL = 131;

  float gyro_x = (accel_t_gyro.value.x_gyro - base_x_gyro) / FS_SEL;
  float gyro_y = (accel_t_gyro.value.y_gyro - base_y_gyro) / FS_SEL;
  float gyro_z = (accel_t_gyro.value.z_gyro - base_z_gyro) / FS_SEL;

  // Get raw acceleration values
  float acc_x = accel_t_gyro.value.x_accel;
  float acc_y = accel_t_gyro.value.y_accel;
  float acc_z = accel_t_gyro.value.z_accel;

  // Get angle values from accelerometer
  float RADIANS_TO_DEGREES = 180 / 3.14159;
  float g = sqrt(square(acc_x) + square(acc_y) + square(acc_z));

  float pitch_acc = asin(acc_x / g) * RADIANS_TO_DEGREES;
  float roll_acc = atan(acc_y / acc_z) * RADIANS_TO_DEGREES;
  float yaw_acc = 0;

  // Compute the filtered gyro angles
  float dt = (t_now - MPU.get_last_time()) / 1000.0;
  float pitch_gyro = gyro_x * dt + get_last_x_angle();
  float roll_gyro = gyro_y * dt + get_last_y_angle();
  float yaw_gyro = gyro_z * dt + get_last_z_angle();
}