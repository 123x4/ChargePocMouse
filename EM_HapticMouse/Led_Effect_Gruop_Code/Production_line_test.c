#include "custom.h"
#include "Production_line_test.h"
#include "led1.h"
#include "usb.h"
#include "button.h"
#include "pc_communicate.h"
#include "led_effect.h"
#include "led_effect_group_code.h"
#include "led2.h"
#include "Timer.h"
#include "gpio.h"

const RGB_STRU productionLine_ledTest[5] = 
{
	{255,0,0},
	{0,255,0},
	{0,0,255},
	{255,255,255},
	{0,0,0}
};

BOOL productLine_start = FALSE;

void ProductionLine_Start_Set(void)
{
	productLine_start = TRUE;
}

void ProductionLine_Start_Clear(void)
{
	productLine_start = FALSE;
}

BOOL ProductionLine_Start_Get(void)
{
	return productLine_start;
}

//void ProductLine_Test(void)
//{
//	//UINT8 cnt = 20,i,ii;
//	UINT8 cnt = 20,ii;
//	BOOL led_trg = FALSE;
//	UINT8 ledIndex;
//	while ((Button_GetState() & (VOLUME_UP_BUTTON_REPORT_BIT_MASK | DPI_CYCLE_BUTTON_REPORT_BIT_MASK))
//		== (VOLUME_UP_BUTTON_REPORT_BIT_MASK | DPI_CYCLE_BUTTON_REPORT_BIT_MASK))
//	{
////		WATCH_DOG_CLEAR;
//		TimerDelayMsec(100);

//		if (cnt)cnt--;
//		if (!cnt) break;
//	}

//	if (!cnt)
//	{
//		Led_EffectCode_Stop();

//		for (ii = 0;ii < CUSTOM_BUTTON_NUMBER;ii++)
//		{
//			gloabSetting.settingMode.key_function[ii].usb_registration.id = 0xcB;//l
//		}

//		gloabSetting.settingMode.key_function[0].usb_registration.id = 0xB0;//l
//		gloabSetting.settingMode.key_function[1].usb_registration.id = 0xCc;//l
//		gloabSetting.settingMode.key_function[9].usb_registration.id = CYCLE_DPI;//Profile

//		ProductionLine_Start_Set();

//		while (((Button_GetState() & (VOLUME_UP_BUTTON_REPORT_BIT_MASK | DPI_CYCLE_BUTTON_REPORT_BIT_MASK))
//			== (VOLUME_UP_BUTTON_REPORT_BIT_MASK | DPI_CYCLE_BUTTON_REPORT_BIT_MASK)))
//		{
//			if (!led_trg)
//			{
//				ledIndex = 4;
//			}else
//			{
//				ledIndex = 3;
//			}

//			led_trg = !led_trg;
//			
//			PWM_set_duty(productionLine_ledTest[ledIndex], LED_MODE_LIGHT_ON);
//			
//			TimerDelayMsec(200);
//		}
//		
//		PWM_set_duty(productionLine_ledTest[3], LED_MODE_LIGHT_ON);
//	}	
//}


//void ProductionLine_Deal(UINT8 testMode,UINT8 keyIndex,UINT8 status)
//{
//	static UINT8 ledMode_index = 0XFF;
////	UINT8 i=0;
////	RGB_STRU rgb = {25,0,0};
//	receive_buff[0] = PRODUCTION_LINE_TEST_MODE;
//	receive_buff[1] = keyIndex;
//	receive_buff[2] = status;
//	if (testMode == PRODUCTION_MODE_LED && status)
//	{
//		ledMode_index++;
//		if (ledMode_index > 3)
//		{
//			ledMode_index = 0;
//		}
//		
//		switch(ledMode_index)
//		{
//			case 0:
//				DrvGPIOOutput(0, 19, 0);
//				DrvGPIOOutput(0, 12, 1);
//				DrvGPIOOutput(0, 18, 1);
//				break;
//			case 1:
//				DrvGPIOOutput(0, 19, 1);
//				DrvGPIOOutput(0, 12, 0);
//				DrvGPIOOutput(0, 18, 1);
//				break;
//			case 2:
//				DrvGPIOOutput(0, 19, 1);
//				DrvGPIOOutput(0, 12, 1);
//				DrvGPIOOutput(0, 18, 0);
//				break;
//			case 3:
//				DrvGPIOOutput(0, 19, 0);
//				DrvGPIOOutput(0, 12, 0);
//				DrvGPIOOutput(0, 18, 0);
//				break;
//			default:
//				DrvGPIOOutput(0, 19, 0);
//				DrvGPIOOutput(0, 12, 0);
//				DrvGPIOOutput(0, 18, 0);
//				break;
//		}
//		
//		PWM_set_duty(productionLine_ledTest[ledMode_index], LED_MODE_LIGHT_ON);
//	}

//	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 	
//}



