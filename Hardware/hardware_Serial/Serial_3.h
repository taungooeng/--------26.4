#ifndef __SERIAL_3_H
#define __SERIAL_3_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

// 波特率定义 (可根据需要修改)
#define USART3_BaudRate      9600

// 缓冲区大小定义 (与先前模块保持一致)
#define SERIAL_RX_BUFFER_SIZE 128
#define SERIAL_TX_BUFFER_SIZE 128

// 串口3状态数据结构
typedef struct {
    uint8_t rxBuffer[SERIAL_RX_BUFFER_SIZE];
    uint16_t rxWriteIndex;
    uint16_t rxReadIndex;
    uint16_t rxCount;
    
    uint8_t txBuffer[SERIAL_TX_BUFFER_SIZE];
    uint16_t txWriteIndex;
    uint16_t txReadIndex;
    uint16_t txCount;
    
    uint8_t dmaTxBusy;  // DMA发送忙标志 (保留，当前未使用)
} Serial_TypeDef_3;

// 声明串口3的全局状态变量
extern Serial_TypeDef_3 Serial3;

// 初始化函数
void Serial3_Init(void);

// 接收相关函数
uint8_t Serial3_Available(void);
uint8_t Serial3_ReceiveByte(void);//读取一个字节
void Serial3_Flush(void);

// 发送相关函数
void Serial3_SendByte(uint8_t Byte);//发送一个字节
void Serial3_SendString(char *String);
void Serial3_Printf(char *format, ...);

#endif /* __SERIAL_3_H */

