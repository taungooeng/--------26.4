/**
  ******************************************************************************
  * @file    jy60.h
  * @brief   基于WT61协议的JY60模块驱动头文件
  *          适用于STM32F103C8T6，假设串口底层读写（UART_ReadByte, UART_WriteByte）已实现
  ******************************************************************************
  */
#ifndef __JY60_H
#define __JY60_H



#include <stdint.h>
#include "Serial_3.h"


/* 加速度数据结构体 */
typedef struct {
    float ax; // X轴加速度，单位: m/s²
    float ay; // Y轴加速度，单位: m/s²
    float az; // Z轴加速度，单位: m/s²
    float temperature; // 温度，单位: °C
} JY60_AccData_t;

/* 角速度数据结构体 */
typedef struct {
    float wx; // X轴角速度，单位: °/s
    float wy; // Y轴角速度，单位: °/s
    float wz; // Z轴角速度，单位: °/s
    float voltage; // 电压，单位: V (非蓝牙产品可能无效)
} JY60_GyroData_t;

/* 角度数据结构体 */
typedef struct {
    float roll;  // 滚转角X，单位: °
    float pitch; // 俯仰角Y，单位: °
    float yaw;   // 偏航角Z，单位: °
    uint16_t version; // 版本号
} JY60_AngleData_t;

/* 模块工作模式 */
typedef enum {
    JY60_MODE_SLEEP = 0,
    JY60_MODE_WORK
} JY60_Mode_t;

/* 模块安装方向 */
typedef enum {
    JY60_INSTALL_HORIZONTAL = 0, // 水平安装
    JY60_INSTALL_VERTICAL        // 垂直安装
} JY60_Install_t;

/* 模块通信接口 */
typedef enum {
    JY60_IF_UART = 0, // 串口模式
    JY60_IF_IIC       // I2C模式
} JY60_Interface_t;

/* 串口波特率与回传速率 */
typedef enum {
    JY60_BAUD_9600 = 0,   // 波特率9600, 回传20Hz
    JY60_BAUD_115200      // 波特率115200, 回传100Hz
} JY60_BaudRate_t;

/* 数据包类型 (文档中的TYPE) */
typedef enum {
    JY60_PACKET_ACC   = 0x51, // 加速度包
    JY60_PACKET_GYRO  = 0x52, // 角速度包
    JY60_PACKET_ANGLE = 0x53  // 角度包
} JY60_PacketType_t;

/* 函数声明 */
void JY60_Init(void);
uint8_t JY60_DataReady(void);
void JY60_ParseData(void);

JY60_AccData_t*  JY60_GetAccData(void);
JY60_GyroData_t* JY60_GetGyroData(void);
JY60_AngleData_t* JY60_GetAngleData(void);

void JY60_SendCommand(uint8_t cmd);
void JY60_ZeroZ(void);
void JY60_CalibrateAcc(void);
void JY60_ToggleSleep(void);
void JY60_SetInterface(JY60_Interface_t iface);
void JY60_SetBaudRate(JY60_BaudRate_t baud);
void JY60_SetInstallDirection(JY60_Install_t dir);



#endif 



