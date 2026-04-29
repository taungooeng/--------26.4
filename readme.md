# 校赛小车26.4 - 基于STM32F103C8T6的智能循迹小车

## 项目概述

这是一个基于**STM32F103C8T6**微控制器的智能竞速小车项目，集成了多传感器融合算法、PID控制系统和实时任务调度。小车通过光电传感器和陀螺仪IMU实现精准的黑线循迹和转向控制。

## 硬件资源配置

| 存储类型 | 计算方式 | 占用大小 | 总容量 | 使用率 |
| :--- | :--- | :--- | :--- | :--- |
| **Flash (ROM)** | Code + RO-data | 47.00 KB | 64 KB | **73.44%** |
| **RAM** | RW-data + ZI-data | 3.45 KB | 20 KB | **17.25%** |

## 核心模块

### 1. **传感器模块**
- **光电传感器** - 黑线检测（LightSensor）
- **MPU6050 IMU** - 3轴加速度计 + 3轴陀螺仪
- **编码器** - 轮速反馈（可选）

### 2. **控制模块**
- **PID系统** - 差速转向控制
- **GyroZ误差积分** - 陀螺仪Z轴偏差补偿 ⭐ **新增**
- **电机驱动** - PWM差速控制（PWMA/PWMB）

### 3. **通信模块**
- **USART1/2/3** - 串口通信
- **I2C硬件接口** - IMU传感器通信
- **实时调度器** - systick任务调度

### 4. **调试工具**
- **PC13 LED** - 状态指示
- **DWT延时** - 精确延时计数
- **Serial调试** - 实时数据输出


```c




#include "stm32f10x.h"                  //Device header
#include "systick_scheduler.h"          //Base    systick
#include "DWT_Delay.h"                  //Base    DWT
#include "math.h"
#include "string.h"
#include "stdlib.h" 

#include "PC13_LED.h"                   //Base    PC13

#include "Serial.h"                     //Base    hardware_Serial   USART   TTL

#include "hardware_I2C.h"               //Base    hardware_I2Cx        (I2C1_I2C2)
    
    #include "OLED.h"                       //Base    (PB11) (PB10)   I2C2.h         OLED_2          //--no-multibyte-chars

    //====mpu6050dmp====//                  
    #include "mpu6050.h"                    //Base    (PB11) (PB10)   I2C2.h    inv_mpu.h
    #include "inv_mpu.h"                    //Base                    I2C2.h    inv_mpu.h    mpu6050.h
    
    
#include "PWM.h"                        //Base    TIM2_CH1(PA0)_CH2(PA1)
#include "Motor.h"                      //Base    PWM.h     PWMA    (PA0)(PB4)(PB5)(PA1)(PA15)(PB3)
#include "Encoder.h"                    //Base    TIM3_CH1(PA67)    TIM4_CH1(PB67)    IC_PWMI.h 与 Encoder.h  只能起用一个
#include "PID_system.h"                 //base     






```

















