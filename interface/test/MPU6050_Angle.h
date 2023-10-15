#ifndef MPU6050_ANGLE_H
#define MPU6050_ANGLE_H

#include <Arduino.h>
#include <Wire.h>
#include <math.h>

class MPU6050_Angle
{
private:
  static const int NumData = 7;
  static const int MPU_address = 0x68;                        // Define MPU constant
  static const double pi = 3.1415926535897932384626433832795; // Define pi constant

  const unsigned long restTime = 20; // In milliseconds
  unsigned long lastReadingTime = 0;

  int AccTempGyr[NumData];                                    // Table with the raw values of the gyro, acc and temperature
  int AccTempGyr_correction[NumData] = {0, 0, 0, 0, 0, 0, 0}; // Table with a correction that can be added to the read values

  double pitch, roll;
  int accData[3];
  int gyroData[3];

  bool readGY521();
  bool isReady();
  void computeAngles();
  bool updateAngles();
  double compute_g(double ax, double ay, double az);

public:
  MPU6050_Angle();
  ~MPU6050_Angle();
  void begin();
  double getPitch();
  double getRoll();
  int *getAccData();
  int *getGyroData();
};

#endif