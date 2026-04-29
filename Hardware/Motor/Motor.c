#include "stm32f10x.h"
#include "PWM.h"
void Motor_Init(void)
{
	TIM2_PWM_Init();
}

void Motor_Set_TIM2_ch1_PWMA(int16_t ch1)
{
    if (ch1 > 0)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_6);    // 正转
        GPIO_ResetBits(GPIOB, GPIO_Pin_7);
        TIM_SetCompare1(TIM2, ch1);
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_6);
        GPIO_SetBits(GPIOB, GPIO_Pin_7);    // 反转
        TIM_SetCompare1(TIM2, -ch1);
    }
}

void Motor_Set_TIM2_ch2_PWMB(int16_t ch2)
{
    if (ch2 > 0)
    {
		GPIO_SetBits(GPIOB, GPIO_Pin_8); // IN3 置高
		GPIO_ResetBits(GPIOB, GPIO_Pin_9); // IN4 置低
        TIM_SetCompare2(TIM2, ch2);
    }
    else
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_9); //
		GPIO_ResetBits(GPIOB, GPIO_Pin_8);    // 反转
        TIM_SetCompare2(TIM2, -ch2);
    }
}

/*void Motor_Set_TIM2_ch1_PWMA(int16_t ch1)//只考虑前进
{
     		if(ch1 > 100)
			{
				ch1 = 100;
			}
			if(ch1 < 0)
			{
				ch1 = 0;
			}
			if (ch1<=100 && ch1 >=0) {
				GPIO_SetBits(GPIOB, GPIO_Pin_7); // IN2 置高
				GPIO_ResetBits(GPIOB, GPIO_Pin_6); // IN1 置低
				TIM_SetCompare1(TIM2, ch1);
}
}
void Motor_Set_TIM2_ch2_PWMB(int16_t ch2)
{
 			if(ch2 > 100)
			{
				ch2 = 100;
			}
			if(ch2 < 0)
			{
				ch2 = 0;	
			}
			if(ch2 <= 100 && ch2 >= 0) 
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_8); // IN3 置高
				GPIO_ResetBits(GPIOB, GPIO_Pin_9); // IN4 置低
				TIM_SetCompare2(TIM2, ch2);    
			}
}
*/

