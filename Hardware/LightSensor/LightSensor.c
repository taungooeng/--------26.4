#include "LightSensor.h"
#include "string.h"
#include "Motor.h"
#include "Serial.h"  
//taungoo: sensor   2   1   3   4
//taungoo: sensor   A3	A2	A4	A5
void LightSensor_Init(void)
{
    
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

GPIO_InitTypeDef GPIO_InitStructure;


// 初始化GPIOA
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 |GPIO_Pin_4 | GPIO_Pin_3| GPIO_Pin_2;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStructure);
/*
// 初始化GPIOB
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOB, &GPIO_InitStructure);

// 初始化GPIOC
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOC, &GPIO_InitStructure);
*/


}


//===================================================================================================
// 光敏传感器量化函数
//===================================================================================================
#define light_0 8.00
#define light_1 30.00

float LightSensor_GetPos(void)
{
    static float last_pos = 0.0f;
    
//taungoo: sensor   2   1   3   4
//taungoo: sensor   B5	B4	B3	A12
	float s1 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) ? 0.0f : 1.0f;   // middle-left
    float s2 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0) ? 0.0f : 1.0f;   // outer-left
    float s3 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0) ? 0.0f : 1.0f;   // middle-right
    float s4 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0) ? 0.0f : 1.0f;   // outer-right


    // 打印传感器值
    Serial_Printf("s1:%.0f s2:%.0f s3:%.0f s4:%.0f\n", s1, s2, s3, s4);

    // 双传感器为0的情况（线在两个传感器之间）
    if ( s2==0 && s1==0 && s3==1 && s4==1 )
    {
        last_pos = (-light_1 + -light_0) * 0.5f;
        return last_pos;
    }
    if ( s2==1 && s1==0 && s3==0 && s4==1 )
    {
        last_pos = (-light_0 + light_0) * 0.5f;
        return last_pos;
    }
    if ( s2==1 && s1==1 && s3==0 && s4==0 )
    {
        last_pos = (light_0 + light_1) * 0.5f;
        return last_pos;
    }

    // 三传感器为0的情况（线在三个传感器上）
    if ( s2==0 && s1==0 && s3==0 && s4==1 )
    {
        last_pos = (-light_1 + -light_0 + light_0) / 3.0f;
        return last_pos;
    }
    if ( s2==1 && s1==0 && s3==0 && s4==0 )
    {
        last_pos = (-light_0 + light_0 + light_1) / 3.0f;
        return last_pos;
    }

    // 单传感器为0的情况（线在单个传感器上）
    if ( s2==1 && s1==0 && s3==1 && s4==1 )
    {
        last_pos = -light_0;
        return last_pos;
    }
    
    if ( s2==0 && s1==1 && s3==1 && s4==1 )
    {
        last_pos = -light_1;
        return last_pos;
    }
    
    if ( s2==1 && s1==1 && s3==0 && s4==1 )
    {
        last_pos = light_0;
        return last_pos;
    }
    
    if ( s2==1 && s1==1 && s3==1 && s4==0 )
    {
        last_pos = light_1;
        return last_pos;
    }
    
    // 全为0的情况
    if ( s1==0 && s2==0 && s3==0 && s4==0 )
    {
        last_pos=0;
        return last_pos;
    }
    
    // 默认情况
    return last_pos;
}

//float LightSensor_GetPos(void)
//{
//    static float last_pos = 0.0f;
//    
//    // 读取8个传感器状态
//    
//    float s1 = (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 0) ? 1.0f : 0.0f;
//    float s2 = (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0) ? 1.0f : 0.0f;
//    float s3 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4 ) == 0) ? 1.0f : 0.0f;
//    float s4 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5 ) == 0) ? 1.0f : 0.0f;
//    float s5 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6 ) == 0) ? 1.0f : 0.0f;
//    float s6 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7 ) == 0) ? 1.0f : 0.0f;
//    float s7 = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0 ) == 0) ? 1.0f : 0.0f;
//    float s8 = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1 ) == 0) ? 1.0f : 0.0f;

//    if ( s1==0 && s2==1 && s3==1 && s4==1 && s5==1 && s6==1 && s7==1 && s8==1 )
//    {
//        last_pos=-15;
//    }
//    
//    if ( s1==1 && s2==0 && s3==1 && s4==1 && s5==1 && s6==1 && s7==1 && s8==1 )
//    {
//        last_pos=-10;
//    }
//    
//    if ( s1==1 && s2==1 && s3==0 && s4==1 && s5==1 && s6==1 && s7==1 && s8==1 )
//    {
//        last_pos=-5.6;
//    }
//    
//    if ( s1==1 && s2==1 && s3==1 && s4==0 && s5==1 && s6==1 && s7==1 && s8==1 )
//    {
//        last_pos=-1;
//        //return -1;
//    }
//    
//    if ( s1==1 && s2==1 && s3==1 && s4==1 && s5==0 && s6==1 && s7==1 && s8==1 )
//    {
//        last_pos=1;
//        //return  1;
//    }
//    
//    
//    if ( s1==1 && s2==1 && s3==1 && s4==1 && s5==1 && s6==0 && s7==1 && s8==1 )
//    {
//        last_pos=5.6;
//    }
//    
//    if ( s1==1 && s2==1 && s3==1 && s4==1 && s5==1 && s6==1 && s7==0 && s8==1 )
//    {
//        last_pos=10;
//    }
//    
//    if ( s1==1 && s2==1 && s3==1 && s4==1 && s5==1 && s6==1 && s7==1 && s8==0 )
//    {
//        last_pos=15;
//    }
//    
//    if ( s1==0 && s2==0 && s3==0 && s4==0 && s5==0 && s6==0 && s7==0 && s8==0 )
//    {
//        last_pos=0;
//    }
//    

//    
//    return last_pos;

//}


