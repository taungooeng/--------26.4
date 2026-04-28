/**
  ******************************************************************************
  * @file    jy60.c
  * @brief   基于WT61协议的JY60模块驱动源文件
  ******************************************************************************
  */
#include "jy60.h"
#include "string.h"

// 包含串口3驱动头文件
#include "serial_3.h"

/* 模块内部状态与数据 */
static JY60_AccData_t   acc_data;
static JY60_GyroData_t  gyro_data;
static JY60_AngleData_t angle_data;

static uint8_t data_buf[11]; // 用于存储一帧数据 (0x55 + TYPE + 8字节数据 + SUM)
static uint8_t buf_index = 0;
static uint8_t packet_ready = 0;
static uint8_t in_frame = 0; // 增加：是否正在接收一帧



/**
  * @brief  检查并组装JY60数据包
  * @retval 1: 有完整数据包, 0: 无
  * @note   此函数应在任务中高频调用
  */
uint8_t JY60_DataReady(void)
{
    // 使用while循环，处理所有可用的字节
    while(Serial3_Available() > 0)
    {
        uint8_t byte = Serial3_ReceiveByte(); // 只读取一次！
        
        if (!in_frame) 
        {
            // 不在帧中，寻找帧头0x55
            if (byte == 0x55) 
            {
                in_frame = 1;
                buf_index = 0;
                data_buf[buf_index++] = byte; // 存储帧头
            }
            // 忽略非帧头字节
        } 
        else 
        {
            // 正在接收帧，存储字节
            if (buf_index < 11) 
            {
                data_buf[buf_index++] = byte;
            }
            
            // 检查是否收到完整帧
            if (buf_index >= 11) 
            {
                in_frame = 0;
                buf_index = 0;
                
                // 校验和检查
                uint8_t sum = 0;
                for(int i = 0; i < 10; i++) 
                {
                    sum += data_buf[i];
                }
                
                if (sum == data_buf[10]) 
                {
                    packet_ready = 1;
                    return 1; // 立即返回，避免在一次调用中处理多帧
                }
                // 校验失败，丢弃本帧，继续寻找下一帧
            }
        }
    }
    
    return 0; // 没有完整帧
}

/**
  * @brief  解析准备好的数据包
  */
void JY60_ParseData(void)
{
    if (!packet_ready) 
    {
        return;
    }
    packet_ready = 0;

    uint8_t type = data_buf[1];
    int16_t raw_data[4];

    // 组合16位数据
    for(int i = 0; i < 4; i++) 
    {
        raw_data[i] = (int16_t)(((int16_t)data_buf[2*i+3] << 8) | data_buf[2*i+2]);
    }

    switch(type) 
    {
        case JY60_PACKET_ACC: // 0x51
            acc_data.ax = (float)raw_data[0] / 32768.0f * 16.0f * 9.8f;
            acc_data.ay = (float)raw_data[1] / 32768.0f * 16.0f * 9.8f;
            acc_data.az = (float)raw_data[2] / 32768.0f * 16.0f * 9.8f;
            acc_data.temperature = (float)raw_data[3] / 32768.0f * 96.38f + 36.53f;
            break;

        case JY60_PACKET_GYRO: // 0x52
            gyro_data.wx = (float)raw_data[0] / 32768.0f * 2000.0f;
            gyro_data.wy = (float)raw_data[1] / 32768.0f * 2000.0f;
            gyro_data.wz = (float)raw_data[2] / 32768.0f * 2000.0f;
            gyro_data.voltage = (float)raw_data[3] / 100.0f;
            break;

        case JY60_PACKET_ANGLE: // 0x53
            angle_data.roll  = (float)raw_data[0] / 32768.0f * 180.0f;
            angle_data.pitch = (float)raw_data[1] / 32768.0f * 180.0f;
            angle_data.yaw   = (float)raw_data[2] / 32768.0f * 180.0f;
            angle_data.version = (uint16_t)raw_data[3];
            break;

        default:
            // 未知数据类型
            break;
    }
}

/* 数据获取接口 */
JY60_AccData_t* JY60_GetAccData(void)   { return &acc_data; }
JY60_GyroData_t* JY60_GetGyroData(void)  { return &gyro_data; }
JY60_AngleData_t* JY60_GetAngleData(void) { return &angle_data; }

/**
  * @brief  发送命令帧
  */
void JY60_SendCommand(uint8_t cmd)
{
    Serial3_SendByte(0xFF);
    Serial3_SendByte(0xAA);
    Serial3_SendByte(cmd);
}

/* 具体的命令封装 */
void JY60_ZeroZ(void)            { JY60_SendCommand(0x52); }
void JY60_CalibrateAcc(void)     { JY60_SendCommand(0x67); }
void JY60_ToggleSleep(void)      { JY60_SendCommand(0x60); }
void JY60_SetInterface(JY60_Interface_t iface) {
    JY60_SendCommand(iface == JY60_IF_UART ? 0x61 : 0x62);
}
void JY60_SetBaudRate(JY60_BaudRate_t baud) {
    JY60_SendCommand(baud == JY60_BAUD_9600 ? 0x64 : 0x63);
}
void JY60_SetInstallDirection(JY60_Install_t dir) {
    JY60_SendCommand(dir == JY60_INSTALL_HORIZONTAL ? 0x65 : 0x66);
}

/**
  * @brief  JY60模块初始化
  */
void JY60_Init(void)
{
    memset(&acc_data, 0, sizeof(acc_data));
    memset(&gyro_data, 0, sizeof(gyro_data));
    memset(&angle_data, 0, sizeof(angle_data));
    memset(data_buf, 0, sizeof(data_buf));
    buf_index = 0;
    packet_ready = 0;
    in_frame = 0;
    JY60_CalibrateAcc();
    JY60_ZeroZ();
}

