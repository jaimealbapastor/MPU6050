#ifndef MPU6050_ANGLE_H
#define MPU6050_ANGLE_H

#include <Arduino.h>
#include <Wire.h>
#include <math.h>

class MPU6050_Angle {
public:
  MPU6050_Angle();
  void begin();
  void readAngle();

private:
  static const int NumData = 7;
  int GyAccTemp[NumData];
  int GATCorr[NumData] = {0, 0, 0, 0, 0, 0, 0};
  double PitchRoll[3];

  void ReadGY521(int *GyAccTempp, int *GATCorrr);
  void ComputeAngle(int *GyAccTempp, double *PitchRol);
};

#endif