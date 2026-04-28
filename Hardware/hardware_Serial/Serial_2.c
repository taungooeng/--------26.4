#include "stm32f10x.h"
#include "serial_2.h"
#include <string.h>

// 定义串口2的全局状态变量
Serial_TypeDef_2 Serial2;

// 内部静态函数声明
static uint32_t Serial_Pow(uint32_t X, uint32_t Y);
static void Serial2_SendData(void);

/*-----------------------------------------------------------*/
/**
  * 函    数：串口2初始化
  * 参    数：无
  * 返 回 值：无
  * 说    明：初始化USART2，配置GPIO、波特率、中断等
  */
void Serial2_Init(void)
{
    // 初始化串口数据结构
    memset(&Serial2, 0, sizeof(Serial2));
    
    /* 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    /* GPIO初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // PA2: USART2_TX 复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // PA3: USART2_RX 浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* USART初始化 */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = USART2_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    
    /* 中断配置 */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);        // 使能接收中断
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);       // 初始关闭发送中断
    
    /* NVIC配置 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* 使能USART */
    USART_Cmd(USART2, ENABLE);
}

/*-----------------------------------------------------------*/
/**
  * 函    数：检查接收缓冲区是否有数据
  * 参    数：无
  * 返 回 值：接收缓冲区中的字节数
  */
uint8_t Serial2_Available(void)
{
    return Serial2.rxCount;
}

/*-----------------------------------------------------------*/
/**
  * 函    数：从接收缓冲区读取一个字节
  * 参    数：无
  * 返 回 值：读取到的字节，若无数据则返回0
  */
uint8_t Serial2_ReceiveByte(void)
{
    uint8_t data = 0;
    
    if(Serial2.rxCount > 0)
    {
        data = Serial2.rxBuffer[Serial2.rxReadIndex];
        
        // 更新读取索引
        Serial2.rxReadIndex++;
        if(Serial2.rxReadIndex >= SERIAL_RX_BUFFER_SIZE)
        {
            Serial2.rxReadIndex = 0;
        }
        
        // 更新计数（原子操作）
        __disable_irq();
        Serial2.rxCount--;
        __enable_irq();
    }
    
    return data;
}

/*-----------------------------------------------------------*/
/**
  * 函    数：清空接收缓冲区
  * 参    数：无
  * 返 回 值：无
  */
void Serial2_Flush(void)
{
    __disable_irq();
    Serial2.rxReadIndex = 0;
    Serial2.rxWriteIndex = 0;
    Serial2.rxCount = 0;
    __enable_irq();
}

/*-----------------------------------------------------------*/
/**
  * 函    数：计算X的Y次方（内部辅助函数）
  * 参    数：X - 底数，Y - 指数
  * 返 回 值：X^Y
  */
static uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while(Y--)
    {
        Result *= X;
    }
    return Result;
}

/*-----------------------------------------------------------*/
/**
  * 函    数：将一个字节放入发送缓冲区，并尝试启动发送
  * 参    数：Byte - 要发送的字节
  * 返 回 值：无
  */
void Serial2_SendByte(uint8_t Byte)
{
    // 等待发送缓冲区有空闲位置（忙等待）
    while(Serial2.txCount >= SERIAL_TX_BUFFER_SIZE)
    {
        // 如果缓冲区满，在此循环等待
    }
    
    // 写入发送缓冲区
    Serial2.txBuffer[Serial2.txWriteIndex] = Byte;
    Serial2.txWriteIndex++;
    if(Serial2.txWriteIndex >= SERIAL_TX_BUFFER_SIZE)
    {
        Serial2.txWriteIndex = 0;
    }
    
    // 更新计数（原子操作）
    __disable_irq();
    Serial2.txCount++;
    __enable_irq();
    
    // 使能发送中断，以启动或继续发送
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

/*-----------------------------------------------------------*/
/**
  * 函    数：从发送缓冲区取出一个字节并发送（在发送中断中调用）
  * 参    数：无
  * 返 回 值：无
  */
static void Serial2_SendData(void)
{
    if(Serial2.txCount > 0)
    {
        // 从缓冲区读取一个字节并写入发送数据寄存器
        uint8_t data = Serial2.txBuffer[Serial2.txReadIndex];
        USART_SendData(USART2, data);
        
        // 更新读取索引
        Serial2.txReadIndex++;
        if(Serial2.txReadIndex >= SERIAL_TX_BUFFER_SIZE)
        {
            Serial2.txReadIndex = 0;
        }
        
        // 更新计数（原子操作）
        __disable_irq();
        Serial2.txCount--;
        __enable_irq();
    }
    else
    {
        // 发送缓冲区为空，关闭发送中断
        USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    }
}

/*-----------------------------------------------------------*/
/**
  * 函    数：发送字节数组
  * 参    数：Array - 数组首地址，Length - 数组长度
  * 返 回 值：无
  */
void Serial2_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for(i = 0; i < Length; i++)
    {
        Serial2_SendByte(Array[i]);
    }
}

/*-----------------------------------------------------------*/
/**
  * 函    数：发送字符串（以'\0'结尾）
  * 参    数：String - 字符串首地址
  * 返 回 值：无
  */
void Serial2_SendString(char *String)
{
    while(*String != '\0')
    {
        Serial2_SendByte((uint8_t)(*String));
        String++;
    }
}

/*-----------------------------------------------------------*/
/**
  * 函    数：以十进制形式发送数字
  * 参    数：Number - 要发送的数字，Length - 数字显示的总位数（不足位左补0）
  * 返 回 值：无
  */
void Serial2_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for(i = 0; i < Length; i++)
    {
        // 计算当前位的数字并转换为ASCII码发送
        Serial2_SendByte((uint8_t)(Number / Serial_Pow(10, Length - i - 1) % 10 + '0'));
    }
}

/*-----------------------------------------------------------*/
/**
  * 函    数：发送结构体（按字节流发送）
  * 参    数：pStruct - 结构体指针，Size - 结构体大小（字节数）
  * 返 回 值：无
  */
void Serial2_SendStruct(void *pStruct, uint16_t Size)
{
    uint8_t *pByte = (uint8_t *)pStruct;
    uint16_t i;
    
    for(i = 0; i < Size; i++)
    {
        Serial2_SendByte(pByte[i]);
    }
}

/*-----------------------------------------------------------*/
/**
  * 函    数：格式化输出（类似printf）
  * 参    数：format - 格式字符串，... - 可变参数列表
  * 返 回 值：无
  */
void Serial2_Printf(char *format, ...)
{
    char String[256];
    va_list arg;
    
    va_start(arg, format);
    vsnprintf(String, sizeof(String) - 1, format, arg);
    String[sizeof(String) - 1] = '\0'; // 确保字符串终止
    va_end(arg);
    
    Serial2_SendString(String);
}

/*-----------------------------------------------------------*/
/**
  * 函    数：USART2全局中断服务函数
  * 参    数：无
  * 返 回 值：无
  */
void USART2_IRQHandler(void)
{
    uint8_t ucData;
    
    /* 接收中断处理（接收到数据） */
    if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
    {
        // 读取接收到的数据
        ucData = USART_ReceiveData(USART2);
        
        // 检查接收缓冲区是否已满
        if(Serial2.rxCount < SERIAL_RX_BUFFER_SIZE)
        {
            // 将数据写入环形缓冲区
            Serial2.rxBuffer[Serial2.rxWriteIndex] = ucData;
            Serial2.rxWriteIndex++;
            if(Serial2.rxWriteIndex >= SERIAL_RX_BUFFER_SIZE)
            {
                Serial2.rxWriteIndex = 0;
            }
            
            // 更新缓冲区中的数据计数
            Serial2.rxCount++;
        }
        // 如果缓冲区已满，新数据将被丢弃
        
        // 清除接收中断标志
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
    
    /* 发送中断处理（发送数据寄存器为空，可写入新数据） */
    if(USART_GetITStatus(USART2, USART_IT_TXE) == SET)
    {
        // 从发送缓冲区取出下一个字节并发送
        Serial2_SendData();
    }
}

