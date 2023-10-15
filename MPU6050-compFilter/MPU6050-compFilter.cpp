#include "MPU6050-compFilter.h"
#include <Arduino.h>

void MPU6050_compFilter::set_last_angles(unsigned long time, double roll_x, double pitch_y, double yaw_z)
{
  this->last_angles.read_time = time;
  this->last_angles.roll = roll_x;
  this->last_angles.pitch = pitch_y;
  this->last_angles.yaw = yaw_z;
}

int MPU6050_compFilter::read_acc_gyro_vals(uint8_t *acc_t_gyro_ptr)
{
  // Read the raw values.
  // Read 14 bytes at once,
  // containing acceleration, temperature and gyro.
  // With the default settings of the MPU-6050,
  // there is no filter enabled, and the values
  // are not very stable.  Returns the error value

  acc_t_gyro_union *acc_t_gyro = (acc_t_gyro_union *)acc_t_gyro_ptr;

  int error = this->read(MPU6050_ACCEL_XOUT_H, (uint8_t *)acc_t_gyro, sizeof(*acc_t_gyro));

  // Swap all high and low bytes.
  // After this, the registers values are swapped,
  // so the structure name like x_accel_l does no
  // longer contain the lower byte.
  uint8_t swap;
#define SWAP(x, y) \
  swap = x;        \
  x = y;           \
  y = swap

  SWAP((*acc_t_gyro).reg.x_acc_h, (*acc_t_gyro).reg.x_acc_l);
  SWAP((*acc_t_gyro).reg.y_acc_h, (*acc_t_gyro).reg.y_acc_l);
  SWAP((*acc_t_gyro).reg.z_acc_h, (*acc_t_gyro).reg.z_acc_l);
  SWAP((*acc_t_gyro).reg.t_h, (*acc_t_gyro).reg.t_l);
  SWAP((*acc_t_gyro).reg.x_gyro_h, (*acc_t_gyro).reg.x_gyro_l);
  SWAP((*acc_t_gyro).reg.y_gyro_h, (*acc_t_gyro).reg.y_gyro_l);
  SWAP((*acc_t_gyro).reg.z_gyro_h, (*acc_t_gyro).reg.z_gyro_l);

  return error;
}

//------------------------------------------------------------------
// Robot angle calculations------------------------------------------
//------------------------------------------------------------------

void MPU6050_compFilter::calibrate_gyro_offset()
{
  int num_readings = 10;
  float x_gyro = 0;
  float y_gyro = 0;
  float z_gyro = 0;
  acc_t_gyro_union acc_t_gyro;

  // Serial.println("Starting Calibration");

  // Discard the first set of values read
  read_acc_gyro_vals((uint8_t *)&acc_t_gyro);

  // Read and average the raw values
  for (int i = 0; i < num_readings; i++)
  {
    read_acc_gyro_vals((uint8_t *)&acc_t_gyro);
    x_gyro += acc_t_gyro.value.x_gyro;
    y_gyro += acc_t_gyro.value.y_gyro;
    z_gyro += acc_t_gyro.value.z_gyro;
    delay(100);
  }
  x_gyro /= num_readings;
  y_gyro /= num_readings;
  z_gyro /= num_readings;

  // Store the raw calibration values
  this->gyro_offset.roll = x_gyro;
  this->gyro_offset.pitch = y_gyro;
  this->gyro_offset.yaw = z_gyro;
}

void MPU6050_compFilter::compute_acc_angles(euler_angles *angles, (uint8_t *)acc_t_gyro_data)
{
  angles->pitch = asin(acc_x / g) * RADIANS_TO_DEGREES;
  angles->roll = atan(acc_y / acc_z) * RADIANS_TO_DEGREES;
  angles->yaw = 0;
}
// The sensor should be motionless on a horizontal surface
//  while calibration is happening

void MPU6050_compFilter::compute_angle_estimations()
{
  acc_t_gyro_union acc_t_gyro;
  this->read_acc_gyro_vals((uint8_t *)&acc_t_gyro);

  unsigned long t_now = millis();

  // Accelerometer data
  euler_angles acc_angles;
  this->compute_acc_angles(&acc_angles);

  // Gyrometer Data
  // Convert gyro values to degrees/sec
  float FS_SEL = 131;
  double gyro_x = (acc_t_gyro.value.x_gyro - this->gyro_offset.roll) / FS_SEL;
  double gyro_y = (acc_t_gyro.value.y_gyro - this->gyro_offset.pitch) / FS_SEL;
  double gyro_z = (acc_t_gyro.value.z_gyro - this->gyro_offset.yaw) / FS_SEL;

  // Compute the (filtered) gyro angles
  double dt = (t_now - get_last_read_time()) / 1000.0;
  double gyro_angle_x = gyro_x * dt + this->get_last_roll();
  double gyro_angle_y = gyro_y * dt + this->get_last_pitch();
  double gyro_angle_z = gyro_z * dt + this->get_last_yaw();

  // Apply the complementary filter to figure out the change in angle - choice of alpha is
  // estimated now.  Alpha depends on the sampling rate...
  double alpha = 0.986; // because my sampling rate is 0.014 s long
  double angle_x = alpha * gyro_angle_x + (1.0 - alpha) * acc_angles.roll;
  double angle_y = alpha * gyro_angle_y + (1.0 - alpha) * acc_angles.pitch;
  double angle_z = gyro_angle_z; // Accelerometer doesn't give z-angle

  this->set_last_angles(t_now, angle_x, angle_y, angle_z);
}

//------------------------------------------------------------------
// Micro-controller communication------------------------------------
//------------------------------------------------------------------

// --------------------------------------------------------
// MPU6050_read
//
// This is a common function to read multiple bytes
// from an I2C device.
//
// It uses the boolean parameter for Wire.endTransMission()
// to be able to hold or release the I2C-bus.
// This is implemented in Arduino 1.0.1.
//
// Only this function is used to read.
// There is no function for a single byte.
//
int MPU6050_compFilter::read(int start, uint8_t *buffer, int size)
{
  if (!this->is_ready())
    return (1);

  int i, n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);

  n = Wire.write(start);
  if (n != 1)
    return (-10);

  n = Wire.endTransmission(false); // hold the I2C-bus
  if (n != 0)
    return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  i = 0;
  // Serial.print(F("ad: 0x"));
  // Serial.print(start, HEX);
  while (Wire.available() && i < size)
  {
    // Serial.print(F("\t"));
    buffer[i++] = Wire.read();
    // Serial.print(Wire.read());
    // Serial.print(buffer[i++]);
  }
  // Serial.println(F(""));
  if (i != size)
    return (-11);

  this->last_read_time = millis();
  return (0); // return : no error
}

// --------------------------------------------------------
// MPU6050_write
//
// This is a common function to write multiple bytes to an I2C device.
//
// If only a single register is written,
// use the function MPU_6050_write_reg().
//
// Parameters:
//   start : Start address, use a define for the register
//   pData : A pointer to the data to write.
//   size  : The number of bytes to write.
//
// If only a single register is written, a pointer
// to the data has to be used, and the size is
// a single byte:
//   int data = 0;        // the data to write
//   MPU6050_write (MPU6050_PWR_MGMT_1, &c, 1);
//
int MPU6050_compFilter::write(int start, const uint8_t *pData, int size)
{
  if (!this->is_ready())
    return (1);

  int n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start); // write the start address
  if (n != 1)
    return (-20);

  n = Wire.write(pData, size); // write data bytes
  if (n != 1)
  {
    Serial.print(n);
    return (-21);
  }

  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
    return (error);

  this->last_read_time = millis();
  return (0); // return : no error
}

// --------------------------------------------------------
// MPU6050_write_reg
//
// An extra function to write a single register.
// It is just a wrapper around the MPU_6050_write()
// function, and it is only a convenient function
// to make it easier to write a single register.
//
int MPU6050_compFilter::write_reg(int reg, uint8_t data)
{
  int error;

  error = this->write(reg, &data, 1);

  return (error);
}

bool MPU6050_compFilter::is_ready()
{
  return millis() - this->get_last_time() > this->rest_time;
}

void MPU6050_compFilter::begin()
{
  Wire.begin();
  // Reset the sensor
  this->write_reg(MPU6050_PWR_MGMT_1, (uint8_t)0);
  // Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  // Wire.write(MPU6050_PWR_MGMT_1);
  // Wire.write(0);
  // Wire.endTransmission(true);
}
