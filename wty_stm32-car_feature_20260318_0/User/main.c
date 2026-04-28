//===================================================================================================
//#include
//===================================================================================================

#include "stm32f10x.h"                  //Device header
#include "systick_scheduler.h"          //Base    systick
#include "DWT_Delay.h"                  //Base    DWT
#include "math.h"
#include "string.h"
#include "stdlib.h" 

#include "PC13_LED.h"                   //Base    PC13
#include "Serial.h"                     //Base    hardware_Serial   USART   TTL
#include "Serial_2.h"                   //Base    hardware_Serial   USART   TTL
#include "Serial_3.h"                   //Base    hardware_Serial   USART   TTL

#include "jy60.h"                       //Base    Serial_3.h

#include "PWM.h"                        //Base    TIM2_CH1(PA0)_CH2(PA1)
#include "Motor.h"                      //Base    PWM.h     PWMA    (PA0)(PB4)(PB5)(PA1)(PA15)(PB3)
#include "PID_system.h"                 //base     
#include "LightSensor.h"


//===================================================================================================
// 变量定义
//===================================================================================================

//typedef struct
//{
//    float Pitch;
//    float Roll;
//    float Yaw;
//} pose_of_Pitch_Roll_Yaw;




int16_t recv_Yaw;
int16_t recv_Pitch; 


// 串口接收帧解析状态变量
uint8_t     rxBuffer[64];                   // 帧解析缓冲区大小 64
uint8_t     inFrame;                        // 帧接收状态：0-未在帧内，1-正在接收帧  
uint16_t    rxIndex;                        // 帧数据索引
uint8_t     RE_tast;


int16_t Speed_A_0;
int16_t Speed_B_0;
int16_t Speed_A;
int16_t Speed_B;
int16_t average_speed;
int16_t differential_speed;



int16_t LH ;
int16_t LV ;
int16_t RH ;
int16_t RV ;

float FloatValue_1 ;



//float c_differential_speed ;
//PID_t PID_average_speed;

PID_t PID_differential_speed_track;

//===================================================================================================
// PID参数初始化
//===================================================================================================
void PID_System_Init(void)
{


    
    
    // PID_differential_speed_track
    PID_differential_speed_track.Target = 0.0f;       // 目标值
    PID_differential_speed_track.Actual = 0.0f;       // 实际值
    PID_differential_speed_track.Out = 0.0f;          // 输出值
    PID_differential_speed_track.Kp = 3.723f;         // 比例系数
    PID_differential_speed_track.Ki = 0.0002f;        // 积分系数
    PID_differential_speed_track.Kd = 17.33102f;      // 微分系数
    PID_differential_speed_track.Error0 = 0.0f;       // 本次误差
    PID_differential_speed_track.Error1 = 0.0f;       // 上次误差
    PID_differential_speed_track.ErrorInt = 0.0f;     // 误差积分
    PID_differential_speed_track.OutMax = 500.0f;     // 输出最大值
    PID_differential_speed_track.OutMin = -500.0f;    // 输出最小值
    
}
//===================================================================================================
// 串口3接收jy60 任务            (50ms)
//===================================================================================================
void jy60_PoseTask(void)
{
    
    if(JY60_DataReady()) 
    {
        
        JY60_ParseData();
        
        
        JY60_AngleData_t* angle = JY60_GetAngleData();
        
        
        Serial_Printf("[plot,%.1f]",angle->yaw);
    }
    
}

//===================================================================================================
// 串口1接收处理任务
//===================================================================================================
void Serial_ProcessRxData(void)
{
    static uint8_t rxBuffer[128];
    static uint16_t rxIndex = 0;
    static uint8_t inFrame = 0;
    
    // 循环处理所有接收到的数据
    while(Serial_Available() > 0)
    {
        uint8_t ucRxData = Serial_ReceiveByte();
        
        // 检查是否为帧起始标记 '['
        if(ucRxData == '[')
        {
            inFrame = 1;
            rxIndex = 0;
            continue;
        }
        // 检查是否为帧结束标记 ']'
        else if (ucRxData == ']')
        {
            if (inFrame && rxIndex > 0)
            {
                rxBuffer[rxIndex] = '\0';
                
                // 打印接收到的完整帧内容
                Serial_Printf("[%s]\r\n", rxBuffer);
                //Serial3_Printf("[%s]\r\n", rxBuffer);
                
                // 解析帧内容
                char *Tag = strtok((char *)rxBuffer, ",");
                
                if (strcmp(Tag, "key") == 0)
                {
                    char *Name = strtok(NULL, ",");
                    char *Action = strtok(NULL, ",");
                    
                    if (strcmp(Name, "1") == 0 && strcmp(Action, "up") == 0)
                    {
                        Serial_Printf("key,1,up\r\n");
                        RE_tast++;
                        Serial_Printf("%d\r\n", RE_tast);
                    }
                    else if (strcmp(Name, "2") == 0 && strcmp(Action, "down") == 0)
                    {
                        Serial_Printf("key,2,down\r\n");
                    }
                }
                else if (strcmp(Tag, "slider") == 0)
                {
                    char *Name = strtok(NULL, ",");
                    char *Value = strtok(NULL, ",");
                    
                    if (strcmp(Name, "1") == 0)
                    {
                        //float FloatValue_1 = atoi(Value);
                        FloatValue_1 = atoi(Value);
                        Serial_Printf("slider,1,%d\r\n", FloatValue_1);
                        //PID_differential_speed_track.Kp=FloatValue_1;
                        
                    }
                    else if (strcmp(Name, "2") == 0)
                    {
                        float FloatValue_2 = atof(Value);
                        Serial_Printf("slider,2,%f\r\n", FloatValue_2);
                        PID_differential_speed_track.Kp=FloatValue_2;
                        
                        
                    }
                    else if (strcmp(Name, "3") == 0)
                    {
                        float FloatValue_3 = atof(Value);
                        Serial_Printf("slider,3,%f\r\n", FloatValue_3);
                        PID_differential_speed_track.Ki = FloatValue_3;
                    }
                    else if (strcmp(Name, "4") == 0)
                    {
                        float FloatValue_4 = atof(Value);
                        Serial_Printf("slider,4,%f\r\n", FloatValue_4);
                        PID_differential_speed_track.Kd = FloatValue_4;
                    }
                }
                else if (strcmp(Tag, "joystick") == 0)
                {
                     LH = atoi(strtok(NULL, ","));
                     LV = atoi(strtok(NULL, ","));
                     RH = atoi(strtok(NULL, ","));
                     RV = atoi(strtok(NULL, ","));
                    
                    
//                    Motor_Set_TIM2_ch1_PWMA( LV-RH);
//                    Motor_Set_TIM2_ch2_PWMB( LV+RH);
//                    PID_average_speed.Target      = LV ;

//                    PID_differential_speed_track.Target = RH ;

                    Serial_Printf("joystick,%d,%d,%d,%d\r\n", LH, LV, RH, RV);
                }
                else if (strcmp(Tag, "Pose") == 0)
                {
                    recv_Yaw= atoi(strtok(NULL, ","));
                    recv_Pitch = atoi(strtok(NULL, ","));
                }
                
                inFrame = 0;
                rxIndex = 0;
            }
            else
            {
                inFrame = 0;
                rxIndex = 0;
            }
        }
        // 如果正在接收帧且不是帧结束标记，则存储数据
        else if (inFrame && rxIndex < (sizeof(rxBuffer) - 1))
        {
            rxBuffer[rxIndex++] = ucRxData;
        }
    }
}


//===================================================================================================
// PC13 LED任务
//===================================================================================================
void Test_PC13_LED(void)
{
    PC13_LED_Turn();
    //Serial_Printf("%c\r\n", Serial3_ReceiveByte() );
    
}
//===================================================================================================
// APP 任务                     (50ms)
//===================================================================================================
void APP (void)
{
    PID_differential_speed_track.Actual = LightSensor_GetPos();
    PID_Update(&PID_differential_speed_track);
    
    Motor_Set_TIM2_ch1_PWMA(FloatValue_1+ PID_differential_speed_track.Out);
    Motor_Set_TIM2_ch2_PWMB(FloatValue_1- PID_differential_speed_track.Out);

}

void APP_track (void)
{
    PID_differential_speed_track.Actual = LightSensor_GetPos();
    PID_Update(&PID_differential_speed_track);
    
    Motor_Set_TIM2_ch1_PWMA(FloatValue_1+ PID_differential_speed_track.Out);
    Motor_Set_TIM2_ch2_PWMB(FloatValue_1- PID_differential_speed_track.Out);
}


int main(void)
{
// 设置NVIC优先级分组================================================================================
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

//===================================================================================================
// 模块初始化
//===================================================================================================

    DWT_Delay_Init();
    SCH_Init();
    PC13_LED_Init();
    
    Serial_Init();
    Serial2_Init();
    Serial3_Init();
    
    JY60_Init();
    
    TIM2_PWM_Init();
    Motor_Init();
    PID_System_Init();
    LightSensor_Init();


//    Encoder1_TIM3_Init();           OLED_ShowNum(0, 3, 8, 2, OLED_8X16);OLED_Update();
//    Encoder2_TIM4_Init();           OLED_ShowNum(0, 3, 9, 2, OLED_8X16);OLED_Update();
//    MPU6050_Init();                 OLED_ShowNum(0, 3, 3, 2, OLED_8X16);OLED_Update();
//    MPU6050_DMP_Init();             OLED_ShowNum(0, 3, 4, 2, OLED_8X16);OLED_Update();
//    I2C_QuickInit(I2C2, 400*1000);
//    OLED_Init();                    OLED_ShowNum(0, 3, 1, 2, OLED_8X16);OLED_Update();


//===================================================================================================
//systick 调度器                                (0->1->2)
//=============|函数====================|周期===|优先级=|============================================

    SCH_AddTask(jy60_PoseTask           ,10     ,7      );
    SCH_AddTask(Serial_ProcessRxData    ,10     ,8      );
    SCH_AddTask(APP                     ,20     ,9      );
    SCH_AddTask(Test_PC13_LED           ,20     ,10     );

//===================================================================================================

    while(1)
    {
        //===========================
        SCH_Dispatch();// 启动调度器
        //__WFI();
        //===========================
        
        
    }
}







