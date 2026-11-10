#include "led_effect.h"
#include "usb.h"
#include "custom.h"
#include "GPIO.h"
#include "pc_communicate.h"
#include "Production_line_test.h"
//#include "led_effect_code.h"
#include "led_effect_group_code.h"

BOOL ledStru_DataChang = FALSE;
extern Profile_Mode_Stru profile_mode;
extern uint16_t profile_timer;
LED_PROFILE_STRU led_profile_stru;
extern BOOL profile_keyflag;
extern uint8_t Bypass;
//GLOAB_SETTING_MODE_TYPE gloabSetting;

//const RGB_STRU sevenColor_board[SevenColorBorad_NUM] = 
//{
//	{255,0,0},		
//	{255,255,0},	
//	{0,255,0},	    
//	{0,255,255},	
//	{0,0,255},		
//	{255,0,255},	
//};


//BOOL Led_GradientData(UINT8 *fromData,UINT8 toData,UINT8 speed)
//{
//	UINT8 offSet,result = FALSE;

//	if (toData > *fromData)
//	{
//		offSet = ((toData - *fromData) > speed	)? speed : (toData - *fromData);
//		*fromData += offSet;

//	}else if (toData < *fromData)
//	{
//		offSet = ((*fromData - toData) > speed	)? speed : (*fromData - toData);
//		*fromData -= offSet;

//	}else
//	{
//		result = TRUE;
//	}

//	return result;
//}

//BOOL Led_RGB_GradientData(RGB_STRU *from,RGB_STRU to,UINT8 speed)
//{
//	BOOL result = FALSE;

////	if (
////		(Led_GradientData(&(from->red),to.red,speed)) & 
////		(Led_GradientData(&(from->green),to.green,speed)) &
////		(Led_GradientData(&(from->blue),to.blue,speed))
////		)
////	{
////		result = TRUE;
////	}
//    
//    return result;
//}


//void Led_Updata_StruData(void)
//{
//	UINT8 i;
//	//static UINT8 updateStru_cycle = 0;
//	//if (updateStru_cycle)updateStru_cycle--;
//	//if (!updateStru_cycle)
//	if (!(led_sendType & LED_MODIFY_PWM))
//	{
//		if (ledStru_DataChang)
//		{
//			ledStru_DataChang = FALSE;

//			for (i = 0;i < LED_NUMBER;i++)
//		{
//			led_stru[i].red.ledState = 1;
//			led_stru[i].green.ledState = 1;
//			led_stru[i].blue.ledState = 1;
//		}
////////		Led_Write_Page(LED_DRIVER_SLAVEADDR,POINT_TO_PAGE1);		
//		Led_Write_StructData(led_stru,led_stateRegData,LED_MODIFY_STATE,LED_ALL_SEND);
//			//updateStru_cycle = 20;//separate send consume 16ms'

//		Led_Write_StructData(led_stru,led_pwmRegData,LED_MODIFY_PWM,LED_SEPARATE_SEND);
//		}
//	}
//}

//void LightEffect_AssignedLedStru(UINT8 ledIndex,RGB_STRU dat)
//{
//	led_stru[ledIndex].red.pwmReg = dat.red;
//	led_stru[ledIndex].green.pwmReg = dat.green;
//	led_stru[ledIndex].blue.pwmReg = dat.blue;
//}

//void LightEffect_Chang(void)
//{
//	ledStru_DataChang = TRUE;
//	Bypass=3;
//}

//void Led_Structure_Test(UINT8 *buf)
//{
//	UINT8 ledIndex = buf[1];
//	UINT8 r = buf[2];
//	UINT8 g = buf[3];
//	UINT8 b = buf[4];

//	led_stru[ledIndex].red.pwmReg = r;
//	led_stru[ledIndex].green.pwmReg = g;
//	led_stru[ledIndex].blue.pwmReg = b;

//	Led_Write_StructData(led_stru,led_pwmRegData,LED_MODIFY_PWM,LED_ALL_SEND);
//}

//#if 0
//void LedEffect_Dpi_Start(UINT8 dpiGear)
//{
//	UINT8 led1 = 0,led2 = 0,led3 = 0;

//	led_dpi_stru.start = TRUE;
//	led_dpi_stru.timer = 4;

//	switch(dpiGear)
//	{
//	case DPI_GEAR_1:
//		led3 = 25;
//		break;
//	case DPI_GEAR_2:
//		led3 = 25;
//		led2 = 25;
//		break;
//	case DPI_GEAR_3:
//		led3 = 25;
//		led2 = 25;
//		led1 = 25;
//		break;
//	case DPI_GEAR_4:
//		led2 = 25;
//		led1 = 25;
//		break;
//	case DPI_GEAR_5:
//		led1 = 25;
//		break;
//	default:
//		break;
//	}

//	led_stru[DPI_LED2].red.pwmReg = led1;
//	led_stru[DPI_LED3].red.pwmReg = led2;
//	led_stru[DPI_LED4].red.pwmReg = led3;

//	Led_Write_StructData(led_stru,led_pwmRegData,LED_MODIFY_PWM,LED_ALL_SEND);
//}
//#endif

//void LedEffect_Profile_Start(UINT8 dpiGear)
//{
//		led_profile_stru.start = TRUE;
//	  led_profile_stru.timer = 3;
//	switch(dpiGear)
//	{
//	case DPI_GEAR_1:
////		led3 = 25;
//	led_stru[PROFILE_LED].red.pwmReg = 0;
//	led_stru[PROFILE_LED].green.pwmReg = 0;
//	led_stru[PROFILE_LED].blue.pwmReg = 150;
//		break;
//	case DPI_GEAR_2:
//	led_stru[PROFILE_LED].red.pwmReg = 100;
//	led_stru[PROFILE_LED].green.pwmReg = 100;
//	led_stru[PROFILE_LED].blue.pwmReg = 100;
////		led3 = 25;
////		led2 = 25;
//		break;
//	case DPI_GEAR_3:
//	led_stru[PROFILE_LED].red.pwmReg = 0;
//	led_stru[PROFILE_LED].green.pwmReg = 76;  //176
//	led_stru[PROFILE_LED].blue.pwmReg = 100;   //240
////		led3 = 25;
////		led2 = 25;
////		led1 = 25;
//		break;
//	case DPI_GEAR_4:
//	led_stru[PROFILE_LED].red.pwmReg = 255;
//	led_stru[PROFILE_LED].green.pwmReg = 0;
//	led_stru[PROFILE_LED].blue.pwmReg = 0;
////		led2 = 25;
////		led1 = 25;
//		break;
//	case DPI_GEAR_5:
//	led_stru[PROFILE_LED].red.pwmReg = 0;
//	led_stru[PROFILE_LED].green.pwmReg = 255;
//	led_stru[PROFILE_LED].blue.pwmReg = 0;
////		led1 = 25;
//		break;
//		case DPI_GEAR_6:
//	led_stru[PROFILE_LED].red.pwmReg = 255;
//	led_stru[PROFILE_LED].green.pwmReg = 255;
//	led_stru[PROFILE_LED].blue.pwmReg = 0;
////		led1 = 25;
//		break;
//	
//	default:
//		break;
//	}
//	
//	Led_Write_StructData(led_stru,led_pwmRegData,LED_MODIFY_PWM,LED_ALL_SEND);
//}

//#if 0
//void LedEffect_Profile_Stop(void)
//{

//	led_stru[PROFILE_LED].red.pwmReg = 0;
//	led_stru[PROFILE_LED].green.pwmReg = 0;
//	led_stru[PROFILE_LED].blue.pwmReg = 0;

//	Led_Write_StructData(led_stru,led_pwmRegData,LED_MODIFY_PWM,LED_ALL_SEND);
//}
//#endif

//void LedEffect_profile_Scan(void)
//{
//	static UINT16 profileScanTimer = 0;

//	if (led_profile_stru.start)
//	{
//		profileScanTimer++;
//		if (profileScanTimer > 1000)
//		{
//			profileScanTimer = 0;
//			if (led_profile_stru.timer)led_profile_stru.timer--;
//			if (!led_profile_stru.timer)	
//			{
//				led_stru[PROFILE_LED].red.pwmReg = 0;
//				led_stru[PROFILE_LED].green.pwmReg = 0;
//				led_stru[PROFILE_LED].blue.pwmReg = 0;
////				led_stru[PROFILE_LED].red.ledState = 0;
////				led_stru[PROFILE_LED].red.blinkCtrl =0;
////				Led_Write_StructData(led_stru,led_blinkRegData,LED_MODIFY_BLINK,LED_ALL_SEND);
////				Led_Write_StructData(led_stru,led_stateRegData,LED_MODIFY_STATE,LED_ALL_SEND);
//				Led_Write_StructData(led_stru,led_pwmRegData,LED_MODIFY_PWM,LED_ALL_SEND);

//				led_profile_stru.start = FALSE;
//			}
//		}
//	}
//}

//void Select_Assigned_Dat(RGB_STRU dat ,BOOL excludeDPI)
//{
//	UINT8 ii;
//	for (ii = 0;ii < LED_NUMBER;ii++)
//	{
//		if (excludeDPI)
//		{
//			if ((ii == DPI_LED2) || (ii == DPI_LED3)||(ii == DPI_LED4))
//			{
//				continue;
//			}
//		}

//		LightEffect_AssignedLedStru(ii,dat);
//	}
//}

//void LedEffect_Put_StaticColor(RGB_STRU dat,BOOL excludeDPI,UINT8 sendType)
//{
////	Select_Assigned_Dat(dat,excludeDPI);

//	Led_Write_StructData(led_stru,led_pwmRegData,LED_MODIFY_PWM,sendType);
//}


//void LedEffect_Start(void)
//{
//	if(!ProductionLine_Start_Get())
//	{
////		Led_EffectCode_Group_Assign(gloabSetting.settingMode.LE_value);

////		Led_EffectCode_Group_GenetateStart();
//	}
//}


void Led_Scan(void)
{
//	LedEffect_Start();
//	Led_EffectCode_Group_GenetateStart();
//	DrvGPIOOutput(0,23,0);
//	if (optical_scan_cycle)
	{
//	Led_EffectCode_Group_Scan();
	}
//  DrvGPIOOutput(0,23,1);
	if(Bypass==1)
	{
//	Led_Updata_StruData();
  }
	if(Bypass==0)
	{
	Led_I2C_SeparateSend_StructData();
  }
	if(Bypass)
	{
	  Bypass--;
	}
	Led_Check_EFT();
	
//	LedEffect_profile_Scan();
//	if((!profile_timer)&&(profile_keyflag))
// {
////	profile_timer=3000; 
//  profile_keyflag= FALSE;
//	LedEffect_Profile_Stop();
// }
//	LedEffect_Dpi_Scan();
}


LE_EnterOff_Stru LE_enterOff;


void Led_Enter_LefOff_Set(UINT8 ledOff)
{
	BOOL flag;

	if (ledOff & 0x80)
	{
		flag = TRUE;
	}else
	{
		flag = FALSE;
	}

	ledOff = ledOff & 0x7F;

	if (flag)
	{
		LE_enterOff.flag = FALSE;

		Led_Adjust_Brightness(gloabSetting.settingMode.brightness);

	}else
	{
		LE_enterOff.flag = TRUE;

		LE_enterOff.count = ledOff;

		LE_enterOff.backCount = ledOff;

		LE_enterOff.sec = LE_PutOff_Sec;

		LE_enterOff.put_off = FALSE;

		LE_enterOff.effect_pahse = LE_PHASE_Cal_Process;

		LE_enterOff.effect_bright = gloabSetting.settingMode.brightness;

		LE_enterOff.effect_backBright = LE_enterOff.effect_bright;
	}
}

void Led_Enter_LefOff_ClearCount(UINT8 gcc)
{
	if (!LE_enterOff.flag)
	{
		return;
	}

	LE_enterOff.count = LE_enterOff.backCount;

	if (LE_enterOff.put_off)
	{
		LE_enterOff.put_off = FALSE;

		LE_enterOff.sec = LE_PutOff_Sec;

		LE_enterOff.effect_pahse = LE_PHASE_Bright;

		LE_enterOff.effect_backBright = gcc;
	}	
}

void Led_Enter_LefOff_Scan(void)
{
	static UINT16 ds_cycle = 1000;
	static UINT8 LE_offCycle = 0;

	if (!LE_enterOff.flag)
	{
		return;
	}

	if(LE_enterOff.effect_pahse == LE_PHASE_NONE)
	{
		return;
	}

	if (LE_enterOff.effect_pahse == LE_PHASE_Bright)
	{
		if (LE_offCycle)
		{
			LE_offCycle--;
		}else 
		{
			LE_offCycle = LE_EnterOff_Gradation;

			if (LE_enterOff.effect_bright < LE_enterOff.effect_backBright)
			{
				LE_enterOff.effect_bright++;

				Led_Adjust_Brightness(LE_enterOff.effect_bright);

			}else
			{
				LE_enterOff.effect_pahse = LE_PHASE_Cal_Process;
			}		
		}	
	}else if (LE_enterOff.effect_pahse == LE_PHASE_Cal_Process)
	{
		///1 sec 
		if (ds_cycle)
		{
			ds_cycle--;
		}
		if (!ds_cycle)
		{
			ds_cycle = 1000;
			if (LE_enterOff.sec)LE_enterOff.sec--;
			///1 min
			if (!LE_enterOff.sec)
			{
				LE_enterOff.sec = LE_PutOff_Sec;//1 min

				if (LE_enterOff.count)LE_enterOff.count--;
				if (!LE_enterOff.count)
				{
					LE_enterOff.put_off = TRUE;

					LE_enterOff.effect_pahse = LE_PHASE_Dark;

					LE_enterOff.effect_bright = LE_enterOff.effect_backBright;
				}
			}
		}

	}else if (LE_enterOff.effect_pahse == LE_PHASE_Dark)
	{
		if (LE_offCycle)
		{
			LE_offCycle--;
		}else 
		{
			LE_offCycle = LE_EnterOff_Gradation;

			if (LE_enterOff.effect_bright)
			{
				LE_enterOff.effect_bright--;

				Led_Adjust_Brightness(LE_enterOff.effect_bright);	

			}

			if (!LE_enterOff.effect_bright)
			{
				LE_enterOff.effect_pahse = LE_PHASE_NONE;

			}
		}	
	}
}
