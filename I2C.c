#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define GW_GRAY_I2C I2C2
#define GW_GRAY_ADDR 0x9E // 假设传感器硬件地址跳线帽AD1, AD0都未插，软件地址为默认。根据7.6节计算。

void I2C_GraySensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    // 2. 配置PB10(I2C2_SCL), PB11(I2C2_SDA) 为复用开漏模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 复用开漏
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 3. 配置I2C
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00; // 主设备地址，可任意设置，不冲突即可
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000; // 100kHz，标准模式
    I2C_Init(GW_GRAY_I2C, &I2C_InitStructure);

    // 4. 使能I2C
    I2C_Cmd(GW_GRAY_I2C, ENABLE);
}

uint8_t GraySensor_ReadDigitalData(void) {
    uint8_t data = 0;
    // 等待总线空闲
    while(I2C_GetFlagStatus(GW_GRAY_I2C, I2C_FLAG_BUSY));
    // 发送起始条件
    I2C_GenerateSTART(GW_GRAY_I2C, ENABLE);
    while(!I2C_CheckEvent(GW_GRAY_I2C, I2C_EVENT_MASTER_MODE_SELECT));
    // 发送从机地址（写模式，用于发送命令）
    I2C_Send7bitAddress(GW_GRAY_I2C, GW_GRAY_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(GW_GRAY_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    // 发送命令 0xDD
    I2C_SendData(GW_GRAY_I2C, 0xDD);
    while(!I2C_CheckEvent(GW_GRAY_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    // 发送重复起始条件，转为读模式
    I2C_GenerateSTART(GW_GRAY_I2C, ENABLE);
    while(!I2C_CheckEvent(GW_GRAY_I2C, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(GW_GRAY_I2C, GW_GRAY_ADDR, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(GW_GRAY_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    // 等待数据接收完成（单字节，非应答后停止）
    I2C_AcknowledgeConfig(GW_GRAY_I2C, DISABLE); // 读完一个字节后发NACK
    I2C_GenerateSTOP(GW_GRAY_I2C, ENABLE);
    while(!I2C_CheckEvent(GW_GRAY_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED));
    data = I2C_ReceiveData(GW_GRAY_I2C);
    I2C_AcknowledgeConfig(GW_GRAY_I2C, ENABLE); // 恢复ACK
    return data;
}

float LightSensor_GetPos_I2C(void) {
    static float last_pos = 0.0f;
    uint8_t sensor_byte;
    // 通过I2C一次性读取8路状态
    sensor_byte = GraySensor_ReadDigitalData();
    // 位提取，注意：bit0对应s1(OUT1)，与你的代码顺序一致
    // 传感器输出低电平(0)为黑线，对应位为0。我们保持原判断逻辑。
    float s1 = ((sensor_byte & 0x01) == 0) ? 1.0f : 0.0f; // bit0
    float s2 = ((sensor_byte & 0x02) == 0) ? 1.0f : 0.0f; // bit1
    float s3 = ((sensor_byte & 0x04) == 0) ? 1.0f : 0.0f; // bit2
    float s4 = ((sensor_byte & 0x08) == 0) ? 1.0f : 0.0f; // bit3
    float s5 = ((sensor_byte & 0x10) == 0) ? 1.0f : 0.0f; // bit4
    float s6 = ((sensor_byte & 0x20) == 0) ? 1.0f : 0.0f; // bit5
    float s7 = ((sensor_byte & 0x40) == 0) ? 1.0f : 0.0f; // bit6
    float s8 = ((sensor_byte & 0x80) == 0) ? 1.0f : 0.0f; // bit7
    // --- 这里开始，后面的if判断逻辑与你原来的LightSensor_GetPos函数完全一样 ---
    if ( s1==0 && s2==1 && s3==1 && s4==1 && s5==1 && s6==1 && s7==1 && s8==1 ) { last_pos=-light_3; }
    // ... 中间所有if条件判断保持不变 ...
    if ( s1==0 && s2==0 && s3==0 && s4==0 && s5==0 && s6==0 && s7==0 && s8==0 ) { last_pos=0; }
    return last_pos;
}