#include "breathe.h"
#include "custom.h" 

UINT8 RGB_Breath_Value[64] = {0, 1, 2, 3, 4, 5, 6, 7, 8,
					10, 12, 14, 16, 18, 20, 22, 24, 26,
					29, 32, 35, 38, 41, 44, 47, 50, 53,
					57, 61, 65, 69, 73, 77, 81, 85, 89,
					94, 99,	104,109,114,119,124,129,134,
					140,146,152,158,164,170,176,182,188,
					195,202,209,216,223,230,237,244,251,255};

LED_BREATH_CYCLE_STRU led_breath;
UINT8 pwm_t;
//void Led_Breath_Start(LE_Group_Value_Uin led_setting)
//{
//	led_breath.breath_start = TRUE;
//	led_breath.Breath_RGB[0].red = led_setting.breathe.rgb0.red;
//	led_breath.Breath_RGB[0].green = led_setting.breathe.rgb0.green;
//	led_breath.Breath_RGB[0].blue = led_setting.breathe.rgb0.blue;
//	led_breath.frameCount = 0;
//	led_breath.frameCycle = led_setting.breathe.speed;
//}

UINT8  Led_Cutomer_ColorIndex_Changed(LED_BREATH_CYCLE_STRU * breathe)
{
	UINT16 mask;
	UINT8 ii;
	UINT8 ledColorIndex = 0;
	UINT8 cnt = 30;

	ii = breathe->ledColor_index;

	while (1)
	{			
		mask = (0x0001 << ii);

		if (mask & breathe->customer_mask)
		{
			ledColorIndex = ii;
			breathe->ledColor_index = ii;

			break;
		}	

		ii++;

		if (ii >= Breathe_MAX_Index)
		{
			ii = 0;
		}

		///for died
		if (cnt)cnt--;
		if (!cnt)
		{
			break;
		}
	}

	breathe->ledColor_index++;

	if (breathe->ledColor_index >= Breathe_MAX_Index)
	{
		breathe->ledColor_index = 0;
	}

	return ledColorIndex;
}

//void Breathe_Generate(LE_Group_Value_Uin gV,LED_Property_Stru* gp)
//{
//	LED_BREATH_CYCLE_STRU * breathe;

//	gp->type = LED_MODE_BREATH;
//	gp->runValue = LED_rundata_ram;
//	gp->runValue_len = LE_G_Breathe_LEN;

//	breathe = (LED_BREATH_CYCLE_STRU *)(gp->runValue);

//	breathe->breath_start = FALSE;
//}

//void Breathe_Start(LE_Group_Value_Uin gV,LED_Property_Stru* gp)
//{
//	LED_BREATH_CYCLE_STRU * breathe; 
//    
//	UINT8 customerIndex;

//	breathe = (LED_BREATH_CYCLE_STRU *)(gp->runValue);

//	breathe->breath_start = TRUE;
//	breathe->lightness = gV.breathe.transparency;

//	breathe->ledColor_index = 0;
//	breathe->customer_mask = gV.breathe.customer_mask;

//	if (gV.breathe.random)
//	{
//		Led_EffectCode_Update_Random_Color(&(breathe->Breath_RGB[0]));
//		Led_EffectCode_Update_Random_Color(&(breathe->Breath_RGB[1]));

//		breathe->random = TRUE;
//	}else
//	{
//		breathe->Breath_RGB[0] = (gV.breathe.rgb0);
//		breathe->Breath_RGB[1] = (gV.breathe.rgb1);

//		breathe->random = FALSE;
//	}

//	customerIndex = Led_Cutomer_ColorIndex_Changed(breathe);

//	breathe->breath_rgb = breathe->Breath_RGB[customerIndex];

//	breathe->frameCount = 128;
//	breathe->frameCycle = gV.breathe.speed;
//	breathe->frameCnt = 0;

//	breathe->changedColor = TRUE;	

////	breathe->transparency =  gV.breathe.transparency;
//}

void Breathe_Stop(LED_Property_Stru* gp)
{
	LED_BREATH_CYCLE_STRU * breathe; 

	breathe = (LED_BREATH_CYCLE_STRU *)(gp->runValue);

	breathe->breath_start = FALSE;
}

//void Led_Breath_Stop(void)
//{
//	led_breath.breath_start = FALSE;
//}

//RETURN_STRUCT Led_BreatheAlgorithm1(void)
//{
//	UINT8 ratio = 0;
//	UINT32 redTmp = 0,greenTmp = 0,blueTmp = 0;
//	RETURN_STRUCT result = {TRUE,TRUE};

//	if (led_breath.frameCnt)led_breath.frameCnt--;
//	if (!(led_breath.frameCnt))
//	{
//		led_breath.frameCnt = led_breath.frameCycle;
//		led_breath.frameCount++;
//		if (led_breath.frameCount > 187)
//		{
//			led_breath.frameCount = 0;
//		}
//	}else
//	{
//		result.hadData = FALSE;
//		return result;
//	}
//	if (led_breath.frameCount < 64)
//	{
//		ratio = 255 - 4* (led_breath.frameCount);
//	
//	}else  if ((led_breath.frameCount > 63) & (led_breath.frameCount < 94) )
//	{
//		ratio = 0;
//	}else if ((led_breath.frameCount > 93) & (led_breath.frameCount < 158))
//	{
//		ratio = 255 - ( 255 - 4*(led_breath.frameCount-94));

//	}else if (led_breath.frameCount > 157)
//	{
//		ratio = 255;
//	}

//	redTmp = (UINT32)(led_breath.Breath_RGB[0].red * ratio);
//	greenTmp =(UINT32)(led_breath.Breath_RGB[0].green * ratio);
//	blueTmp = (UINT32)(led_breath.Breath_RGB[0].blue * ratio);

//	redTmp >>= 8;
//	greenTmp >>= 8;
//	blueTmp >>= 8;


//	led_RGB_Stru.red = (UINT8)redTmp;
//	led_RGB_Stru.green = (UINT8)greenTmp;
//	led_RGB_Stru.blue = (UINT8)blueTmp;

//	return result;
//}

//void Led_Breath_Cycle(void)
//{
//	RETURN_STRUCT resultData = {FALSE,FALSE};


//	if (led_breath.breath_start == FALSE)
//	{
//		return;
//	}
//	
//	resultData = Led_BreatheAlgorithm1();

//	if (resultData.hadData)
//	{
//		led_pwmData_change = TRUE;
//	}
//	//}	
//}

//RGB_Event Led_BreatheAlgorithm(LED_Property_Stru* gp)
//{
//	UINT8 ratio = 0,customerIndex;
//	UINT32 redTmp = 0,greenTmp = 0,blueTmp = 0;
//	RGB_Event ret;
//	LED_BREATH_CYCLE_STRU * breathe = (LED_BREATH_CYCLE_STRU *)(gp->runValue); 

//	ret.had = TRUE;

//	if (breathe->frameCnt)breathe->frameCnt--;
//	if (!(breathe->frameCnt))
//	{
////		breathe->frameCnt = 50 - breathe->frameCycle * 2;
//		if(breathe->frameCount < 255)breathe->frameCount++;
//		else breathe->frameCount = 0;
//		pwm_t = breathe->frameCount;
////		breathe->frameCount += breathe->frameCycle;
//		if (!breathe->frameCount)
//		{
//			breathe->changedColor = TRUE;
//			breathe->frameCount = 0;
//		}
//	}else
//	{
//		ret.had = FALSE;
//		return ret;
//	}

//	if (breathe->frameCount < 42)
//	{
//		ratio = breathe->frameCount;//RGB_Breath_Value[63 - breathe->frameCount];
//		breathe->frameCnt = 22;// + breathe->frameCount / 2;
//	}
//	else if ((breathe->frameCount > 41) && (breathe->frameCount < 86))
//	{
//		ratio = (breathe->frameCount - 41) * 2 + 41;//RGB_Breath_Value[63 - breathe->frameCount];
//		breathe->frameCnt = 22;// + breathe->frameCount / 2;	
//	}
//	else if ((breathe->frameCount > 85) && (breathe->frameCount < 128))
//	{
//		ratio = (breathe->frameCount - 85) * 3 + 129;//RGB_Breath_Value[63 - breathe->frameCount];
//		breathe->frameCnt = 22;// + breathe->frameCount / 2;	
//	}
//	else if ((breathe->frameCount > 127) && (breathe->frameCount < 170))
//	{
//		ratio = (170 - breathe->frameCount) * 3 + 129;//RGB_Breath_Value[63 - breathe->frameCount];
//		breathe->frameCnt = 22;// + breathe->frameCount / 2;	
//	}
//	else if ((breathe->frameCount > 169) && (breathe->frameCount < 214))
//	{
//		ratio = (214 - breathe->frameCount) * 2 + 41;//RGB_Breath_Value[63 - breathe->frameCount];
//		breathe->frameCnt = 22;// + breathe->frameCount / 2;	
//	}
//	else if (breathe->frameCount > 213)
//	{
//		ratio = 255 - breathe->frameCount;//RGB_Breath_Value[63 - breathe->frameCount];
//		breathe->frameCnt = 22;// + breathe->frameCount / 2;	
//	}
////	else  if ((breathe->frameCount > 63) & (breathe->frameCount < 68) )
////	{
////		ratio = 0;

////		if (breathe->changedColor == TRUE)
////		{
////			breathe->changedColor = FALSE;

////			customerIndex = Led_Cutomer_ColorIndex_Changed(breathe);

////			if (breathe->random)
////			{
////				Led_EffectCode_Update_Random_Color(&(breathe->Breath_RGB[0]));
////				Led_EffectCode_Update_Random_Color(&(breathe->Breath_RGB[1]));

////				//Tool_USB_ArrayData((UINT8*)(breathe->colorBoard),6);
////			}

////			breathe->breath_rgb = breathe->Breath_RGB[customerIndex];

////		}

////	}
////	else if ((breathe->frameCount > 63) && (breathe->frameCount < 128))
////	{
////		ratio = RGB_Breath_Value[breathe->frameCount - 64];
////		breathe->frameCnt = 35;// + (127 - breathe->frameCount) / 2;
////	}
////		ratio = 255;

//	if(breathe->frameCount == 0xff)
//	{
//		breathe->frameCnt = 200;
//		if (breathe->changedColor == TRUE)
//		{
//			breathe->changedColor = FALSE;

//			customerIndex = Led_Cutomer_ColorIndex_Changed(breathe);

//			if (breathe->random)
//			{
//				Led_EffectCode_Update_Random_Color(&(breathe->Breath_RGB[0]));
//				Led_EffectCode_Update_Random_Color(&(breathe->Breath_RGB[1]));

//				//Tool_USB_ArrayData((UINT8*)(breathe->colorBoard),6);
//			}

//			breathe->breath_rgb = breathe->Breath_RGB[customerIndex];

//		}
//	}

//	redTmp = (UINT32)(breathe->breath_rgb.red * ratio);
//	greenTmp =(UINT32)(breathe->breath_rgb.green * ratio);
//	blueTmp = (UINT32)(breathe->breath_rgb.blue * ratio);

//	redTmp >>= 8;
//	greenTmp >>= 8;
//	blueTmp >>= 8;

//	
//	ret.rgb.red = redTmp;
//	ret.rgb.green = greenTmp;
//	ret.rgb.blue = blueTmp;



//	return ret;
//}

//RGB_Event Breathe_Scan(LED_Property_Stru* gp)
//{
//	RGB_Event rec = {FALSE,FALSE,{0,0,0}};

//	LED_BREATH_CYCLE_STRU * breathe  = (LED_BREATH_CYCLE_STRU *)(gp->runValue);;

//	if (breathe->breath_start)
//	{	
//		rec = Led_BreatheAlgorithm(gp);

//		rec.rgb = Led_Effect_Transparency(rec.rgb, breathe->lightness);
//	}

//	return rec;
//}
