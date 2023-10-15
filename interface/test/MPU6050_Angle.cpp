#include "MPU6050_Angle.h"

MPU6050_Angle::MPU6050_Angle() {}

MPU6050_Angle::~MPU6050_Angle()
{
  Wire.endTransmission(true);
}

void MPU6050_Angle::begin()
{
  // Init module GY-512
  Wire.begin();
  Wire.beginTransmission(MPU_address);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

bool MPU6050_Angle::isReady()
{
  return millis() - this->lastReadingTime > this->restTime;
}

bool MPU6050_Angle::readGY521()
{

  if (!this->isReady())
    return false;

  // Sensor initialization
  Wire.beginTransmission(this->MPU_address);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(this->MPU_address, 14, true);

  // Read data (3 accelerometer axes + temperature + 3 gyroscope axes)
  for (int i = 0; i < NumData; i++)
  {
    if (i != 3)
    {
      this->AccTempGyr[i] = (Wire.read() << 8 | Wire.read()) + this->AccTempGyr_correction[i];
    }
    else
    {
      this->AccTempGyr[i] = (Wire.read() << 8 | Wire.read()) + this->AccTempGyr_correction[i];
      this->AccTempGyr[i] = this->AccTempGyr[i] / 340 + 36.53; // We measure the temperature here and compute it to be in celsius
    }
  }
  return true;
}

void MPU6050_Angle::computeAngles()
{
  double ax = this->AccTempGyr[0];
  double ay = this->AccTempGyr[1];
  double az = this->AccTempGyr[2];

  double g = this->compute_g(ax, ay, az);

  this->pitch = asin(ax / g); // pitch
  this->roll = atan(ay / az); // roll

  // Convert radians to degrees
  this->pitch = this->pitch * (180.0 / this->pi);
  this->roll = this->roll * (180.0 / this->pi);
}

bool MPU6050_Angle::updateAngles()
{
  // Read sensor data
  if (this->readGY521())
  {
    this->computeAngles();
    return true;
  }

  return false;
}

double MPU6050_Angle::compute_g(double ax, double ay, double az)
{
  return sqrt(square(ax) + square(ay) + square(az));
}

double MPU6050_Angle::getPitch()
{
  this->updateAngles();
  return this->pitch;
}

double MPU6050_Angle::getRoll()
{
  this->updateAngles();
  return this->roll;
}

int *MPU6050_Angle::getAccData()
{
  this->readGY521();
  this->accData[0] = this->AccTempGyr[0];
  this->accData[1] = this->AccTempGyr[1];
  this->accData[2] = this->AccTempGyr[2];
  return this->accData;
}

int *MPU6050_Angle::getGyroData()
{
  this->readGY521();
  this->gyroData[0] = this->AccTempGyr[4];
  this->gyroData[1] = this->AccTempGyr[5];
  this->gyroData[2] = this->AccTempGyr[6];
  return this->gyroData;
}
