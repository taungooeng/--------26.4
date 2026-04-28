#ifndef __OLED_H
#define __OLED_H

////===================================================================================================
//// OLED显示任务
////===================================================================================================
//void OLED_DisplayTask(void)
//{
//    OLED_Clear(); 
////------------------------------------
//    // OLED显示数字滚动                                                                         
//    static uint16_t i = 0;
//    OLED_ShowNum(i*8, 0, i, 1, OLED_8X16);
//    i++;
//    i = (i > 9 ? 0 : i);

//    // OLED显示mpu6050数据
//    OLED_ShowString(6*16, 8*3, "Pitch", OLED_6X8);
//    OLED_ShowFloatNum(6*15, 8*4, recv_pose_mpu6050.Pitch, 2, 2, OLED_6X8);
//    OLED_ShowString(6*0, 8*3, "Roll", OLED_6X8);
//    OLED_ShowFloatNum(6*0, 8*4, recv_pose_mpu6050.Roll, 2, 2, OLED_6X8);
//    OLED_ShowString(6*7 , 8*7, "Yaw", OLED_6X8);
//    OLED_ShowFloatNum(6*11, 8*7, recv_pose_mpu6050.Yaw, 2, 2, OLED_6X8);
//    
//    
//    
//    OLED_ShowNum(6*0, 8*0   , recv_Yaw  ,3  ,OLED_6X8);
//    OLED_ShowNum(6*0, 8*1   ,recv_Pitch ,3  ,OLED_6X8);
//    
//    
////------------------------------------
//    OLED_Update();
//}



#include <stdint.h>
#include "OLED_Data.h"
#include "stm32f10x.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

//#include "I2C2.h"
#include "hardware_I2C.h"   
  

/*参数宏定义*********************/

/*FontSize参数取值*/
/*此参数值不仅用于判断，而且用于计算横向字符偏移，默认值为字体像素宽度*/
#define OLED_8X16           8
#define OLED_6X8            6

/*IsFilled参数数值*/
#define OLED_UNFILLED       0
#define OLED_FILLED         1

/*********************参数宏定义*/


/*函数声明*********************/

/*初始化函数*/
void OLED_Init(void);

/*更新函数*/
void OLED_Update(void);
void OLED_UpdateArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

/*显存控制函数*/
void OLED_Clear(void);
void OLED_ClearArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void OLED_Reverse(void);
void OLED_ReverseArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

/*显示函数*/
void OLED_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize);
void OLED_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize);
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize);
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image);
void OLED_Printf(int16_t X, int16_t Y, uint8_t FontSize, char *format, ...);

/*绘图函数*/
void OLED_DrawPoint(int16_t X, int16_t Y);
uint8_t OLED_GetPoint(int16_t X, int16_t Y);
void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1);
void OLED_DrawRectangle(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled);
void OLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint8_t IsFilled);
void OLED_DrawCircle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled);
void OLED_DrawEllipse(int16_t X, int16_t Y, uint8_t A, uint8_t B, uint8_t IsFilled);
void OLED_DrawArc(int16_t X, int16_t Y, uint8_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled);

/*********************函数声明*/

#endif



