#include "stm32f10x.h"                  // Device header

void TIM2_PWM_Init(void)
{
	    GPIO_InitTypeDef GPIO_InitStructure;
      TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
      TIM_OCInitTypeDef TIM_OCInitStructure;

      //使能定时器TIM2时钟，注意TIM2时钟为APB1
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
      //使能PWM输出GPIO时钟
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
                                                                          
      //初始化PA0为TIM2_CH1 PWM输出
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO

      //初始化PA1为TIM2_CH2 PWM输出
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO

      //初始化PB6 PB7 PB8 PB9为GPIO输出
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

      //TIM2时间基准初始化
      TIM_TimeBaseStructure.TIM_Period = 100 - 1;//arr;//自动重装值
      TIM_TimeBaseStructure.TIM_Prescaler =36 - 1;//psc; //时钟预分频数
      TIM_TimeBaseStructure.TIM_ClockDivision = 0;
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
      TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //初始化TIM2
     
      //初始化TIM2_CH1的PWM模式
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//??PWM??1
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//??????
      TIM_OCInitStructure.TIM_Pulse = 0; //
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//??????
      TIM_OC1Init(TIM2, &TIM_OCInitStructure);//???TIM2_CH1

      //初始化TIM2_CH2的PWM模式
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
      TIM_OC2Init(TIM2, &TIM_OCInitStructure);

      //使能TIM2的预装载寄存器
      TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);//OC1
      TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);//OC2
      TIM_ARRPreloadConfig(TIM2, ENABLE); //使能重装寄存器

      TIM_Cmd(TIM2, ENABLE);//使能定时器TIM2,准备工作 
}


void TIM2_SetAllChannels(uint16_t ch1, uint16_t ch2)        //chx (0-999)
{
    TIM_SetCompare1(TIM2, ch1);
    TIM_SetCompare2(TIM2, ch2);
}
