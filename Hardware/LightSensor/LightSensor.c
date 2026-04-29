#include "LightSensor.h"
#include "string.h"
#include "Motor.h"
#include "Serial.h"

// Sensor order: s1 s2 s3 s4 s5 s6 s7 s8, left to right.
// GPIO map:     C14 C15 A4  A5  A6  A7  B0  B1
void LightSensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

float LightSensor_GetPos(void)
{
    static float last_pos = 0.0f;
    float s1 = (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 0) ? 1.0f : 0.0f;
    float s2 = (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0) ? 1.0f : 0.0f;
    float s3 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4 ) == 0) ? 1.0f : 0.0f;
    float s4 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5 ) == 0) ? 1.0f : 0.0f;
    float s5 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6 ) == 0) ? 1.0f : 0.0f;
    float s6 = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7 ) == 0) ? 1.0f : 0.0f;
    float s7 = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0 ) == 0) ? 1.0f : 0.0f;
    float s8 = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1 ) == 0) ? 1.0f : 0.0f;
    float sensor_sum = s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8;
    float errol = 0.0f;

    Serial_Printf("s1:%.0f s2:%.0f s3:%.0f s4:%.0f s5:%.0f s6:%.0f s7:%.0f s8:%.0f\n",
                  s1, s2, s3, s4, s5, s6, s7, s8);

    if (sensor_sum > 0.0f)
    {
        errol = s1 * (-4.0f) + s2 * (-3.0f) + s3 * (-2.0f) + s4 * (-1.0f)
              + s5 * ( 1.0f) + s6 * ( 2.0f) + s7 * ( 3.0f) + s8 * ( 4.0f);
        last_pos = errol / sensor_sum;
    }

    return last_pos;
}