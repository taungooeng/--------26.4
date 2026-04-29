#ifndef __mpu6050_H
#define __mpu6050_H
#include "Delay.h"
#include "MYI2C.h"
#include "mpu6050_Reg.h"
void mpu6050_WriteReg(uint8_t RegAddre, uint8_t Data);
uint8_t mpu6050_ReadReg(uint8_t RegAddress);

void mpu6050_Init(void);
void mpu6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
														int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);

#endif
