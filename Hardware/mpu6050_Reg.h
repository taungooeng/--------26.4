#ifndef __mpu6050_REG_H
#define __mpu6050_REG_H

#define mpu6050_SMPLRT_DIV        0x19
#define mpu6050_CONFIG            0x1A
#define mpu6050_GYRO_CONFIG       0x1B
#define mpu6050_ACCEL_CONFIG      0x1C

#define mpu6050_ACCEL_XOUT_H      0x3B
#define mpu6050_ACCEL_XOUT_L      0x3C
#define mpu6050_ACCEL_YOUT_H      0x3D
#define mpu6050_ACCEL_YOUT_L      0x3E
#define mpu6050_ACCEL_ZOUT_H      0x3F
#define mpu6050_ACCEL_ZOUT_L      0x40
#define mpu6050_TEMP_OUT_H        0x41
#define mpu6050_TEMP_OUT_L        0x42
#define mpu6050_GYRO_XOUT_H       0x43
#define mpu6050_GYRO_XOUT_L       0x44
#define mpu6050_GYRO_YOUT_H       0x45
#define mpu6050_GYRO_YOUT_L       0x46
#define mpu6050_GYRO_ZOUT_H       0x47
#define mpu6050_GYRO_ZOUT_L       0x48

#define mpu6050_PWR_MGMT_1        0x6B
#define mpu6050_PWR_MGMT_2        0x6C
#define mpu6050_WHO_AM_I          0x75

#endif
