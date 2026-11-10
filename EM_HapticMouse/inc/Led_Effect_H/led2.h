#ifndef __LED1_H__		
#define __LED1_H__

#include "device.h"
//#include "Timer.h"
//#include "color_cycle.h"
//#include "static.h"
#include "gloab.h"
//#include "Timer.h"
//#include "test.h"
#include "led_effect_group_code.h"
#include "led1.h"
#include "led_effect.h"

//typedef struct
//{
//	UINT8 red;
//	UINT8 green;
//	UINT8 blue;        
//}RGB_STRU;

//typedef struct
//{
//	BOOL control;
//	UINT8 pwm;
//	UINT8 pwmIndex;//pwm register index
//	UINT8 controlGroup;//control register index
//	UINT8 controlMask; //which in control bit
//}LED_STRU;

//typedef struct
//{
//	LED_STRU red;
//	LED_STRU green;
//	LED_STRU blue;        
//}LED_RGB_STRU;

//typedef union
//{
//	struct  
//	{
//		UINT8 transparency;
//		UINT8 stop;
//		UINT8 speed;	
//		UINT8 reserve[8];
//	}colorCycle;
//	struct  
//	{
//		UINT8 transparency;
//		BOOL random;
//		UINT8 speed;
//		UINT16 customer_mask;
//		RGB_STRU rgb[2];
//	}breathe;
//	struct  
//	{
//		UINT8 transparency;
//		BOOL random;
//		UINT8 speed;	
//		RGB_STRU rgb[2];
//		UINT8 two;
//		UINT8 reserve[1];
//	}reaction;
//	struct  
//	{
//		UINT8 transparency;
//		BOOL random;
//		RGB_STRU rgb;
//		UINT8 reserve[6];
//	}staticLed;

//}LED_SETTING_PARA; //size 11


//typedef struct  
//{
//	UINT8 runMode;
//	LED_SETTING_PARA modeValue;
//}LED_SETTING_STRU;   //size 12




//typedef struct  
//{
//	BOOL complete;
//	BOOL hadData;
//}RETURN_STRUCT;

//extern BOOL led_init_flag;
//extern BOOL led_pwmData_change;
//extern RGB_STRU led_RGB_Stru;
//extern LE_Group_Value ledSetting_stru;
//extern LED_Property_Stru LE_G_rV;
//extern UINT8 LED_rundata_ram[32];

//void LightEffect_ActionEvent_Set(void);
//BOOL LightEffect_ActionEvent_Get(void);
//void LightEffect_ActionEvent_Clear(void);

//void Led_Init(void);
//void Led_Init1(void);
//void Led_Scan(void);
//void LedDriver_Effect_Scan(void);
//void LedDriver_CycleCheck(void);
//void Led_Update_Mode(void);
//void Led_EffectCode_Assign(LE_Group_Value sV);
//void Led_EffectCode_GenetateStart(void);
//void Led_EffectCode_Generate(void);
//void Led_EffectCode_Update_Random_Color(RGB_STRU *rgb);
//void LedEffect_Start(void);
//RGB_STRU Led_Effect_Transparency(RGB_STRU rgb, UINT8 transparency);
//void Led_EffectCode_Start(void);
//void LightEffect_AssignedLedStru(RGB_STRU dat);
//void LightEffect_Chang(void);
//void Led_EffectCode_Stop(void);
//void LedEffect_Dpi_Start(uint8_t dpi_gear);
//void Led_DPI_Scan(void);
#endif
