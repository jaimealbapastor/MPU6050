#include "MPU_Serial.h"

MPU_Serial::MPU_Serial() {}

MPU_Serial::~MPU_Serial() {
  this->end();
}

void MPU_Serial::begin(int baudRate) {
  this->mpu_.begin();
  Serial.begin(baudRate);
}

void MPU_Serial::end() {
  if (Serial)
    Serial.end();
}

void MPU_Serial::printAngles(char sep = ';') {
  if (!Serial) return;

  Serial.print(this->mpu_.getPitch());
  Serial.print(sep);
  Serial.print(this->mpu_.getRoll());
  Serial.print(sep);
  Serial.println(0);
}