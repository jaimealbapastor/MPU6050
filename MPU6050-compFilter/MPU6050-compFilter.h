#ifndef MPU6050_COMPFILTER_H
#define MPU6050_COMPFILTER_H

#include <Wire.h>

#define PI 3.14159265
#define RADIANS_TO_DEGREES (180.0 / PI)

// Some datasheet adresses
// https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf

#define MPU6050_ACCEL_XOUT_H 0x3B // R
#define MPU6050_ACCEL_XOUT_L 0x3C // R
#define MPU6050_ACCEL_YOUT_H 0x3D // R
#define MPU6050_ACCEL_YOUT_L 0x3E // R
#define MPU6050_ACCEL_ZOUT_H 0x3F // R
#define MPU6050_ACCEL_ZOUT_L 0x40 // R
#define MPU6050_TEMP_OUT_H 0x41   // R
#define MPU6050_TEMP_OUT_L 0x42   // R
#define MPU6050_GYRO_XOUT_H 0x43  // R
#define MPU6050_GYRO_XOUT_L 0x44  // R
#define MPU6050_GYRO_YOUT_H 0x45  // R
#define MPU6050_GYRO_YOUT_L 0x46  // R
#define MPU6050_GYRO_ZOUT_H 0x47  // R
#define MPU6050_GYRO_ZOUT_L 0x48  // R

#define MPU6050_SIGNAL_PATH_RESET 0x68
// Bit7 -             | Bit6 -            | Bit5 -        | Bit4 -        | Bit3 -        | Bit2 GYRO_RESET | Bit1 ACCEL_RESET  | Bit0 TEMP_RESET
#define MPU6050_PWR_MGMT_1 0x6B
// Bit7 DEVICE_RESET  | Bit6 SLEEP        | Bit5 CYCLE    | Bit4 -        | Bit3 TEMP_DIS | Bit2 CLK_SEL    | Bit1 CLK_SEL      | Bit0 CLK_SEL
#define MPU6050_PWR_MGMT_2 0x6C
// Bit7 LP_WAKE_CTRL  | Bit6 LP_WAKE_CTRL | Bit5 STBY_XA  | Bit4 STBY_YA  | Bit3 STBY_ZA  | Bit2 STBY_XG    | Bit1 STBY_YG      | Bit0 STBY_ZG

// Default I2C address for the MPU-6050 is 0x68.
// But only if the AD0 pin is low.
// Some sensor boards have AD0 high, and the
// I2C address thus becomes 0x69.
#define MPU6050_I2C_ADDRESS 0x68
#define MPU6050_WHO_AM_I 0x75 // R

typedef union acc_t_gyro_union
{
  struct
  {
    uint8_t x_acc_h;
    uint8_t x_acc_l;
    uint8_t y_acc_h;
    uint8_t y_acc_l;
    uint8_t z_acc_h;
    uint8_t z_acc_l;
    uint8_t t_h;
    uint8_t t_l;
    uint8_t x_gyro_h;
    uint8_t x_gyro_l;
    uint8_t y_gyro_h;
    uint8_t y_gyro_l;
    uint8_t z_gyro_h;
    uint8_t z_gyro_l;
  } reg;
  struct
  {
    int x_acc;
    int y_acc;
    int z_acc;
    int temperature;
    int x_gyro;
    int y_gyro;
    int z_gyro;
  } value;
};

typedef struct euler_angles
{
  double roll;
  double pitch;
  double yaw;
};

class MPU6050_compFilter
{
private:
  // Use the following global variables and access functions to help store the overall
  // rotation angle of the sensor
  unsigned long last_read_time = 0;
  inline void set_last_read_time(unsigned long t) { this->last_read_time = t; }
  inline unsigned long get_last_read_time() { return this->last_read_time; }

  euler_angles last_angles;
  void set_last_angles(double roll_x, double pitch_y, double yaw_z);

  inline double get_last_roll() { return this->last_angles.roll; }
  inline double get_last_pitch() { return this->last_angles.pitch; }
  inline double get_last_yaw() { return this->last_angles.yaw; }

  int read_acc_gyro_vals(uint8_t *accel_t_gyro_ptr);

  const unsigned long rest_time = 20; // msec
  inline unsigned long get_rest_time()
  {
    return this->rest_time;
  }

  //  Used the following variables to remove the gyro offset
  euler_angles gyro_offset;

  // Robot angle calculations--------------------------------------------
  double g_;
  inline void set_g(acc_t_gyro_union *data);
  inline double g() { return this->g_; }

  void compute_acc_angles(euler_angles *angles, acc_t_gyro_union *acc_t_gyro_data);

  // Micro-controller communication -------------------------------------

  int read(int start, uint8_t *buffer, int size);
  int write(int start, const uint8_t *pData, int size);
  int write_reg(int reg, uint8_t data);
  bool is_ready();

public:
  MPU6050_compFilter() {}
  ~MPU6050_compFilter(){};

  void begin();
  void calibrate_gyro_offset();
  void compute_angle_estimations();
  euler_angles get_euler_angles(euler_angles *angles) const;
  acc_t_gyro_union get_acc_t_gyro_data(acc_t_gyro_union *acc_t_gyro_data) const;
};

#endif