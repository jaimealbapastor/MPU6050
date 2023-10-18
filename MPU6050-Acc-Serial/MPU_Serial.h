#ifndef MPU_SERIAL_H
#define MPU_SERIAL_H

#include "MPU6050_Angle.h"

class MPU_Serial {
private:
  MPU6050_Angle mpu_;

public:
  MPU_Serial();
  ~MPU_Serial();

  void begin(int baudRate);
  void end();
  
  void printAngles(char sep=';');
};

#endif
