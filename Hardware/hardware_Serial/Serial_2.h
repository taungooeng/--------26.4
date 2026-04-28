#ifndef __SERIAL_2_H
#define __SERIAL_2_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

#define USART2_BaudRate      9600          //波特率

// 缓冲区大小定义
#define SERIAL_RX_BUFFER_SIZE 128
#define SERIAL_TX_BUFFER_SIZE 128

// 串口2状态数据结构
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
} Serial_TypeDef_2;

// 声明串口2的全局状态变量
extern Serial_TypeDef_2 Serial2;

// 初始化函数
void Serial2_Init(void);

// 接收相关函数
uint8_t Serial2_Available(void);
uint8_t Serial2_ReceiveByte(void);//=====
void Serial2_Flush(void);


// 发送相关函数
void Serial2_SendByte(uint8_t Byte);//=====
void Serial2_SendArray(uint8_t *Array, uint16_t Length);
void Serial2_SendString(char *String);
void Serial2_SendNumber(uint32_t Number, uint8_t Length);
void Serial2_SendStruct(void *pStruct, uint16_t Size);
void Serial2_Printf(char *format, ...);

#endif

