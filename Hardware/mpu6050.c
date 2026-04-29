#include "stm32f10x.h"		//Device header
#include "MYI2C.h"
#include "mpu6050_Reg.h"

#define mpu6050_Address      0xD0

void mpu6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
		MyI2C_Start();
		MyI2C_SendByte(mpu6050_Address);
		MyI2C_ReceiveAck();
		MyI2C_SendByte(RegAddress);
		MyI2C_ReceiveAck();
		MyI2C_SendByte(Data);
		MyI2C_ReceiveAck();
	  MyI2C_Stop();
}

uint8_t mpu6050_ReadReg(uint8_t RegAddress)
{
		uint8_t Data;
	
		MyI2C_Start();
		MyI2C_SendByte(mpu6050_Address);
		MyI2C_ReceiveAck();
		MyI2C_SendByte(RegAddress);
		MyI2C_ReceiveAck();

		MyI2C_Start();
		MyI2C_SendByte(mpu6050_Address | 0x01);
		MyI2C_ReceiveAck();
		Data = MyI2C_ReceiveByte();
		MyI2C_SendAck(1);
		MyI2C_Stop();
	
		return Data;
}

void mpu6050_Init(void)
{
		MyI2C_Init();
		mpu6050_WriteReg(mpu6050_PWR_MGMT_1, 0x01);
		mpu6050_WriteReg(mpu6050_PWR_MGMT_2, 0x00);
		mpu6050_WriteReg(mpu6050_SMPLRT_DIV, 0x09);
		mpu6050_WriteReg(mpu6050_CONFIG, 0x06);
		mpu6050_WriteReg(mpu6050_GYRO_CONFIG, 0x18);
		mpu6050_WriteReg(mpu6050_ACCEL_CONFIG, 0x18);
}

void mpu6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
														int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
		uint8_t DataH, DataL;
	  
		DataH = mpu6050_ReadReg(mpu6050_ACCEL_XOUT_H);
		DataL = mpu6050_ReadReg(mpu6050_ACCEL_XOUT_L);
		*AccX = (DataH << 8) | DataL;
	
		DataH = mpu6050_ReadReg(mpu6050_ACCEL_YOUT_H);
		DataL = mpu6050_ReadReg(mpu6050_ACCEL_YOUT_L);
		*AccY = (DataH << 8) | DataL;
	
	  DataH = mpu6050_ReadReg(mpu6050_ACCEL_ZOUT_H);
		DataL = mpu6050_ReadReg(mpu6050_ACCEL_ZOUT_L);
		*AccZ = (DataH << 8) | DataL;
	
		DataH = mpu6050_ReadReg(mpu6050_GYRO_XOUT_H );
		DataL = mpu6050_ReadReg(mpu6050_GYRO_XOUT_L);
		*GyroX = (DataH << 8) | DataL;
	
		DataH = mpu6050_ReadReg(mpu6050_GYRO_YOUT_H );
		DataL = mpu6050_ReadReg(mpu6050_GYRO_YOUT_L);
		*GyroY = (DataH << 8) | DataL;
		
		DataH = mpu6050_ReadReg(mpu6050_GYRO_ZOUT_H );
		DataL = mpu6050_ReadReg(mpu6050_GYRO_ZOUT_L);
		*GyroZ = (DataH << 8) | DataL;
		
}
