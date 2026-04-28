#include "stm32f10x.h"
#include "serial_3.h"
#include <string.h>

// 定义串口3的全局状态变量
Serial_TypeDef_3 Serial3;

// 内部静态函数声明
static void Serial3_SendData(void);

/*-----------------------------------------------------------*/
/**
  * 函    数：串口3初始化
  * 参    数：无
  * 返 回 值：无
  * 说    明：初始化USART3，配置GPIO、波特率、中断等
  */
void Serial3_Init(void)
{
    // 初始化串口数据结构
    memset(&Serial3, 0, sizeof(Serial3));
    
    /* 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    /* GPIO初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // PB10: USART3_TX 复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // PB11: USART3_RX 浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* USART初始化 */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = USART3_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
    
    /* 中断配置 */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);        // 使能接收中断
    USART_ITConfig(USART3, USART_IT_TXE, DISABLE);       // 初始关闭发送中断
    
    /* NVIC配置 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; // 优先级可根据系统调整
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* 使能USART */
    USART_Cmd(USART3, ENABLE);
}

/*-----------------------------------------------------------*/
/**
  * 函    数：检查接收缓冲区是否有数据
  */
uint8_t Serial3_Available(void)
{
    return Serial3.rxCount;
}

/*-----------------------------------------------------------*/
/**
  * 函    数：从接收缓冲区读取一个字节
  */
uint8_t Serial3_ReceiveByte(void)
{
    uint8_t data = 0;
    
    if(Serial3.rxCount > 0)
    {
        data = Serial3.rxBuffer[Serial3.rxReadIndex];
        Serial3.rxReadIndex++;
        if(Serial3.rxReadIndex >= SERIAL_RX_BUFFER_SIZE)
        {
            Serial3.rxReadIndex = 0;
        }
        __disable_irq();
        Serial3.rxCount--;
        __enable_irq();
    }
    return data;
}

/*-----------------------------------------------------------*/
/**
  * 函    数：清空接收缓冲区
  */
void Serial3_Flush(void)
{
    __disable_irq();
    Serial3.rxReadIndex = 0;
    Serial3.rxWriteIndex = 0;
    Serial3.rxCount = 0;
    __enable_irq();
}

/*-----------------------------------------------------------*/
/**
  * 函    数：将一个字节放入发送缓冲区，并尝试启动发送
  */
void Serial3_SendByte(uint8_t Byte)
{
    while(Serial3.txCount >= SERIAL_TX_BUFFER_SIZE)
    {
        // 发送缓冲区满，忙等待
    }
    
    Serial3.txBuffer[Serial3.txWriteIndex] = Byte;
    Serial3.txWriteIndex++;
    if(Serial3.txWriteIndex >= SERIAL_TX_BUFFER_SIZE)
    {
        Serial3.txWriteIndex = 0;
    }
    __disable_irq();
    Serial3.txCount++;
    __enable_irq();
    
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

/*-----------------------------------------------------------*/
/**
  * 函    数：从发送缓冲区取出一个字节并发送（在发送中断中调用）
  */
static void Serial3_SendData(void)
{
    if(Serial3.txCount > 0)
    {
        uint8_t data = Serial3.txBuffer[Serial3.txReadIndex];
        USART_SendData(USART3, data);
        Serial3.txReadIndex++;
        if(Serial3.txReadIndex >= SERIAL_TX_BUFFER_SIZE)
        {
            Serial3.txReadIndex = 0;
        }
        __disable_irq();
        Serial3.txCount--;
        __enable_irq();
    }
    else
    {
        USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
    }
}



/*-----------------------------------------------------------*/
/**
  * 函    数：发送字符串（以'\0'结尾）
  */
void Serial3_SendString(char *String)
{
    while(*String != '\0')
    {
        Serial3_SendByte((uint8_t)(*String));
        String++;
    }
}




/*-----------------------------------------------------------*/
/**
  * 函    数：格式化输出（类似printf）
  */
void Serial3_Printf(char *format, ...)
{
    char String[256];
    va_list arg;
    va_start(arg, format);
    vsnprintf(String, sizeof(String) - 1, format, arg);
    String[sizeof(String) - 1] = '\0';
    va_end(arg);
    Serial3_SendString(String);
}

/*-----------------------------------------------------------*/
/**
  * 函    数：USART3全局中断服务函数
  */
void USART3_IRQHandler(void)
{
    uint8_t ucData;
    
    /* 接收中断处理 */
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        ucData = USART_ReceiveData(USART3);
        if(Serial3.rxCount < SERIAL_RX_BUFFER_SIZE)
        {
            Serial3.rxBuffer[Serial3.rxWriteIndex] = ucData;
            Serial3.rxWriteIndex++;
            if(Serial3.rxWriteIndex >= SERIAL_RX_BUFFER_SIZE)
            {
                Serial3.rxWriteIndex = 0;
            }
            Serial3.rxCount++;
        }
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
    
    /* 发送中断处理 */
    if(USART_GetITStatus(USART3, USART_IT_TXE) == SET)
    {
        Serial3_SendData();
    }
}

