#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"                  // Device header

//===============================================================
//Base   TIM3_CH1(PA67)  And  TIM4_CH1(PB67)
//===============================================================

void Encoder1_TIM3_Init(void);         //TIM3_CH1(PA67)
void Encoder2_TIM4_Init(void);         //TIM4_CH1(PB67)
int16_t Encoder1_TIM3_Encoder_Get(void);
int16_t Encoder2_TIM4_Encoder_Get(void);


#endif


    
            //OLED显示Encoder的计数
            //    OLED_ShowSignedNum(3 , 1 ,Encoder1_TIM3_Encoder_Get(),6,OLED_8X16);
            //    OLED_ShowSignedNum(3 , 9 ,Encoder2_TIM4_Encoder_Get(),6,OLED_8X16);





////////===================================================================================================
//////// Encoder测速任务               (2x25ms)
////////===================================================================================================
//void Encoder_get_speed(void)
//{
//////    static uint16_t count_Encoder_get = 0;
//////    
//////    if(count_Encoder_get < 25)
//////    {
//////        Speed_A += Encoder1_TIM3_Encoder_Get();
//////        Speed_B += Encoder2_TIM4_Encoder_Get();
//////        count_Encoder_get++;
//////    }
//////    else
//////    {
//////        Speed_A /= 25;
//////        Speed_B /= 25;
//////        
//////        Speed_A_0=Speed_A;
//////        Speed_B_0=Speed_B;
//////        
//////        average_speed = (Speed_A_0 + Speed_B_0)/2;
//////        differential_speed = Speed_A_0 - Speed_B_0;
//////        
//////        Speed_A = 0;
//////        Speed_B = 0;
//////        count_Encoder_get = 0;
//////    }
//////    
////////    Serial_Printf("[plot,%d,%d]",Speed_A_0,Speed_B_0);
//////////    Serial_Printf("[plot,%f,%f]",PID_differential_speed.Target,PID_differential_speed.Actual);    
////////    Serial_Printf("[plot,%d]",Speed_A_0);
//}

