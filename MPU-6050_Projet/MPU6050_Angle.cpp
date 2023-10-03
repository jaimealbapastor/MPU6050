#include "MPU6050_Angle.h"

MPU6050_Angle::MPU6050_Angle() {
  // Constructor
}

void MPU6050_Angle::begin() {
  // Init module GY-512 
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Init serial port
  Serial.begin(9600);
}

void MPU6050_Angle::readAngle() {
  delay(100);

  // Read sensor data
  ReadGY521(GyAccTemp, GATCorr);

  // Compute pitch, roll, and yaw angles
  ComputeAngle(GyAccTemp, PitchRoll);

  // Display roll, pitch, and yaw angles in degrees
  Serial.print(PitchRoll[0]); Serial.print(",");
  Serial.print(PitchRoll[1]); Serial.print(",");
  Serial.println(PitchRoll[2]);
  delay(100);
}

void MPU6050_Angle::ReadGY521(int *GyAccTempp, int *GATCorrr) {
  // Sensor initialization
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true);

  // Read data (3 accelerometer axes + temperature + 3 gyroscope axes)
  for (int i = 0; i < NumData; i++) {
    if (i != 3) {
      GyAccTempp[i] = (Wire.read() << 8 | Wire.read()) + GATCorrr[i];
    } else {
      GyAccTempp[i] = (Wire.read() << 8 | Wire.read()) + GATCorrr[i];
      GyAccTempp[i] = GyAccTempp[i] / 340 + 36.53;
    }
  }
}

void MPU6050_Angle::ComputeAngle(int *GyAccTempp, double *PitchRol) {
  double x = GyAccTempp[0];
  double y = GyAccTempp[1];
  double z = GyAccTempp[2];

  PitchRol[0] = atan(x / sqrt((y * y) + (z * z))); // pitch
  PitchRol[1] = atan(y / sqrt((x * x) + (z * z))); // roll
  PitchRol[2] = atan(z / sqrt((x * x) + (y * y))); // pitch

  // Convert radians to degrees
  PitchRol[0] = PitchRol[0] * (180.0 / pi);
  PitchRol[1] = PitchRol[1] * (180.0 / pi);
  PitchRol[2] = PitchRol[2] * (180.0 / pi);
}