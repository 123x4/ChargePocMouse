//**********************************
//
//this file for IS31FL3731 use
//
//according calculate I2C Transmission rate ,
//I2C send one byte use 20 us
//
//**********************************

#ifndef __LED2_C__
#define __LED2_C__

#include "led2.h"
//#include ".\Led_Layer\light_effect_layer.h"
#include "custom.h"
//#include "../inc/LedControl.h"
#include "Timer.h"
#include "color_cycle.h"
#include "static.h"
#include "breathe.h"
#include "reaction.h"
#include "production_line_test.h"
#include "pc_communicate.h"
#include "gpio.h"

///for FW running
//LED_Property_Stru LE_G_rV;

//LE_Group_Value ledSetting_stru;

//RGB_STRU led_RGB_Stru; 

//BOOL led_pwmData_change;
//BOOL LED_Start;
//BOOL LE_G_actionEvent;
//BOOL led_init_flag;
//BOOL led_dpi_flag = 0;
//UINT8 led_dpi_index;
//UINT16 led_dpi_timer_count = 0;
//UINT8 LED_rundata_ram[32] = {0};


//RGB_STRU Led_Effect_Transparency(RGB_STRU rgb, UINT8 transparency)
//{
//	RGB_STRU tmp;

//	tmp.red =  (UINT8)((UINT16)(rgb.red * transparency )/100);
//	tmp.green = (UINT8)((UINT16)(rgb.green * transparency )/100);
//	tmp.blue = (UINT8)((UINT16)(rgb.blue * transparency)/100);

//    return tmp;
//}

//RGB_STRU Led_Effect_Transparency(RGB_STRU rgb)
//{
//	RGB_STRU tmp;
////DrvGPIOOutput(0,23,0);
//  tmp.red =  rgb.red; //* transparency / 100;
//	tmp.green = rgb.green; //* transparency / 100;
//	tmp.blue = rgb.blue; //* transparency / 100;
////	tmp.red =  (rgb.red * transparency )/100;
////	tmp.green = (rgb.green * transparency )/100;
////	tmp.blue = (rgb.blue * transparency)/100;
//	if(tmp.red<120)
//	{
//	  if(tmp.green>120)
//		{
//		 tmp.blue=tmp.blue*(0.3);
//		}
//		if(tmp.green<120)
//		{
//		 tmp.red=tmp.red*(0.6);
//		}
//	}
////	else if(tmp.red>120)
////	{
////			if(tmp.green>120)
////		{
////		 tmp.blue=tmp.blue*(0.6);
////		}
////	}
////DrvGPIOOutput(0,23,1);   
//    return tmp;
//}

//RGB_STRU Led_EffectCode_Random_Color(void)
//{
//	UINT8 ii;
//	UINT8 val;
//	Ramdom_Dat threeMask[3] = {{FALSE,0},{FALSE,255},{FALSE,0xAA}};
//	UINT8 resultVal[3];
//	RGB_STRU rgb;

//	for (ii = 0;ii < 3;ii++)
//	{
//		while (1)
//		{
//			val = Tool_Rand(0,2);

//			if (threeMask[val].flag == FALSE)
//			{
//				threeMask[val].flag = TRUE;

//				resultVal[ii] = threeMask[val].dat;

//				break;
//			}
//		}
//	}


//	for (ii = 0;ii < 3;ii++)
//	{
//		if (resultVal[ii] == 0xAA)
//		{
//			val = Tool_Rand(0,255);

//			resultVal[ii] = val;
//		}
//	}

//	rgb.red = resultVal[0];
//	rgb.green = resultVal[1];
//	rgb.blue = resultVal[2];

//	return rgb;
//}

//void LightEffect_ActionEvent_Set(void)
//{
//	LE_G_actionEvent = TRUE;
//}


//BOOL LightEffect_ActionEvent_Get(void)
//{
//	return LE_G_actionEvent;
//}

//void LightEffect_ActionEvent_Clear(void)
//{
//	LE_G_actionEvent = FALSE;
//}

//void Led_EffectCode_Update_Random_Color(RGB_STRU *rgb)
//{
//	*rgb = Led_EffectCode_Random_Color();
//}

//void LightEffect_AssignedLedStru(RGB_STRU dat)
//{
//	led_RGB_Stru.red = dat.red;
//	led_RGB_Stru.green = dat.green;
//	led_RGB_Stru.blue = dat.blue;
//}

//void LightEffect_Chang(void)
//{
//	led_pwmData_change = TRUE;
////	led_init_flag = 1;
//}
//void Led_Update_Mode(void)
//{
//	ledSetting_stru = gloabSetting.settingMode.LE_value;

//	Led_Static_Stop();
//	Led_Breath_Stop();
//	ColorCycle_Stop();
//	Led_React_Stop();


//	led_RGB_Stru.red = 0;
//	led_RGB_Stru.green = 0;
//	led_RGB_Stru.blue = 0;

//	switch(ledSetting_stru.type)
//	{
//	case LED_MODE_LIGHT_ON:
//		Led_Start_LightsOn(ledSetting_stru.typeValue.staticLed.rgb);
//		break;
//	case LED_MODE_BREATH:
//		//Led_SingleColorBreathe_Start(ledSetting_stru.colorBlackboard_index,ledSetting_stru.breatherFrame);
//		Led_Breath_Start(ledSetting_stru.typeValue);
//		break;
//	case LED_MODE_COLORFUL:
//		//Led_Colorful_Cycle_Start(ledSetting_stru.colorFrame);
//		ColorCycle_Start(ledSetting_stru.typeValue.colorCycle.speed);
//		break;
//	case LED_MODE_REACT:
//		Led_React_Start(0);
//		break;
//	default:
//		break;
//	}
//}

//void LedDriver_Effect_Scan(void)
//{
//	Led_Breath_Cycle();
//	ColorCycle_Scan();
//	Led_Static_Scan();
//	Led_React_Scan();
//}

//void LedDriver_CycleCheck(void)
//{
//	if(led_pwmData_change == TRUE)
//	{
//		led_pwmData_change = FALSE;
//		PWM_set_duty(led_RGB_Stru);
//	}	

//	LedDriver_Effect_Scan();
//}

//void Led_EffectCode_Generate(void)
//{
//	UINT8 ii;

//	RamMalloc_Init();

////	mul_interface = LE_G_sV.mul_interface;

//	for (ii = 0;ii < LE_Group_Num;ii++)
//	{
//		switch(ledSetting_stru.type)
//		{
//		case  LED_MODE_COLORFUL:
//			ColorCycle_Generate(ledSetting_stru.typeValue,&LE_G_rV);
//			break;
//		case  LED_MODE_BREATH:
//			Breathe_Generate(ledSetting_stru.typeValue,&(LE_G_rV));
//			break;
//		case  LED_MODE_REACT:
//			Reaction_Generate(ledSetting_stru.typeValue,&LE_G_rV);
//			break;
//		case  LED_MODE_LIGHT_ON:
//			Static_Generate(ledSetting_stru.typeValue,&LE_G_rV);
//			break;
//		default:
//			break;
//		}
//	}
//}

//void Led_Mode_Start(void)
//{
//	LED_Start = TRUE;
//}

//void Led_EffectCode_Start(void)
//{
//	UINT8 ii;

//	for (ii = 0;ii < LE_Group_Num;ii++)
//	{
//		switch(ledSetting_stru.type)
//		{
//		case  LED_MODE_COLORFUL:
//			ColorCycle_Start(ledSetting_stru.typeValue,&(LE_G_rV));
//			break;
//		case  LED_MODE_BREATH:
//			Breathe_Start(ledSetting_stru.typeValue,&(LE_G_rV));
//			break;
//		case  LED_MODE_REACT:
//			Reaction_Start(ledSetting_stru.typeValue,&(LE_G_rV));
//			break;
//		case  LED_MODE_LIGHT_ON:
//			Static_Start(ledSetting_stru.typeValue,&(LE_G_rV));
//			break;
//		default:
//			break;
//		}	
//	}

//	Led_Mode_Start();
//}


//void Led_EffectCode_Stop(void)
//{
//	UINT8 ii;

//	for (ii = 0;ii < LE_Group_Num;ii++)
//	{
//		switch(ledSetting_stru.type)
//		{
//		case  LED_MODE_COLORFUL:
//			ColorCycle_Stop(&(LE_G_rV));
//			break;
//		case  LED_MODE_BREATH:
//			Breathe_Stop(&(LE_G_rV));
//			break;
//		case  LED_MODE_REACT:
//			Reaction_Stop(&(LE_G_rV));
//			break;
//		case  LED_MODE_LIGHT_ON:
//			Static_Stop(&(LE_G_rV));
//			break;
//		default:
//			break;
//		}	
//	}
//}

//void Led_EffectCode_Scan(void)
//{
//	//UINT8 ii,kk;
//	//UINT8 ledIndex;
//	//RGB_STRU rgb;
//	RGB_Event rgbEvent;
//	
//	if (!LED_Start)
//	{
//		return;
//	}

//	rgbEvent.had = FALSE;
//	
//	switch(ledSetting_stru.type)
//	{
//		case  LED_MODE_COLORFUL:
//			rgbEvent = ColorCycle_Scan((&LE_G_rV));

//			break;
//		case  LED_MODE_BREATH:
//			rgbEvent = Breathe_Scan((&LE_G_rV));

//			break;
//		case  LED_MODE_REACT:
//			rgbEvent = Reaction_Scan((&LE_G_rV));

//			break;
//		case  LED_MODE_LIGHT_ON:
//			rgbEvent = Static_Scan((&LE_G_rV));
//		
//			break;
//		default:
//			break;
//	}	


//	if (rgbEvent.had)
//	{
//		LightEffect_AssignedLedStru(rgbEvent.rgb);

//		LightEffect_Chang();
//	}

//	LightEffect_ActionEvent_Clear();
//}

//void Led_Scan(void)
//{
//	UINT8 pwm_f;
//	RGB_STRU rgb = {0,0,0};
//	
//	if(LED_off_cnt)
//	{
//		if(!timer_1s)
//		{
//			timer_1s = 1000;
//			if(LED_off_cycle)LED_off_cycle--;
//		}
//		if(!LED_off_cycle)
//		{
//			PWM_set_duty(rgb, LED_MODE_LIGHT_ON);
//			return;
//		}		
//	}
//	
//	Led_EffectCode_Scan();
//	
//	if (led_pwmData_change)
//	{
//		if(led_init_flag)
//		{
//			Led_Init1();
//			led_init_flag = 0;
//		}
//		
//		if(gloabSetting.settingMode.LE_value.type == LED_MODE_BREATH)
//		{
//			pwm_f = pwm_t >= 128?(255 - pwm_t):(pwm_t);
//			PWM_Config( 1098 + pwm_f * 80 );
//		}

//		PWM_set_duty(led_RGB_Stru, ledSetting_stru.type);
//		led_pwmData_change = FALSE;
//	}
//}

//void Led_EffectCode_GenetateStart(void)
//{
//	Led_EffectCode_Generate();

//	Led_EffectCode_Start();
//}

//void Led_EffectCode_Assign(LE_Group_Value sV)
//{
//	ledSetting_stru = sV;
//} 

//void LedEffect_Start(void)
//{
//	if(!ProductionLine_Start_Get())
//	{
//		Led_EffectCode_Assign(gloabSetting.settingMode.LE_value);

//		Led_EffectCode_GenetateStart();
//		Led_Init();
//	}
//}

//void Led_Init(void)
//{
//	led_init_flag = 1;
//}

//void Led_Init1(void)
//{
//	RGB_STRU rgb = {0,0,0};
//	
//	PWM_Config( 10000 );
//	
//	PWM_set_duty(rgb, LED_MODE_LIGHT_ON);
//	
//	PWM_Enable();
//						
//	led_pwmData_change = FALSE;
//}

//void LedEffect_Dpi_Start(uint8_t dpi_gear)
//{
//	led_dpi_flag = 1;
//	led_dpi_index = dpi_gear;
//}

//void Led_DPI_Scan(void)
//{
//	if( led_dpi_flag )
//	{
//		switch(led_dpi_index)
//		{
//			case 0:
//				DrvGPIOOutput(0, 19, 1);
//				DrvGPIOOutput(0, 12, 1);
//				DrvGPIOOutput(0, 18, 0);
//				break;
//			case 1:
//				DrvGPIOOutput(0, 19, 1);
//				DrvGPIOOutput(0, 12, 0);
//				DrvGPIOOutput(0, 18, 1);
//				break;
//			case 2:
//				DrvGPIOOutput(0, 19, 0);
//				DrvGPIOOutput(0, 12, 0);
//				DrvGPIOOutput(0, 18, 1);
//				break;
//			case 3:
//				DrvGPIOOutput(0, 19, 0);
//				DrvGPIOOutput(0, 12, 1);
//				DrvGPIOOutput(0, 18, 1);
//				break;
//			default:
//				DrvGPIOOutput(0, 19, 1);
//				DrvGPIOOutput(0, 12, 1);
//				DrvGPIOOutput(0, 18, 1);
//				break;
//		}

//		led_dpi_timer_count = 3000;
//		
//		led_dpi_flag = 0;
//	}
//}

#endif
