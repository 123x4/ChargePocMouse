#include "static.h"
#include "led2.h"
#include "custom.h"

RGB_STRU staticRgb;
BOOL staticRgb_flag;

//void Led_Start_LightsOn(RGB_STRU rgb)
//{
//	staticRgb = rgb;

//	staticRgb_flag = TRUE;
//}

//void Static_Generate(LE_Group_Value_Uin gV, LED_Property_Stru* gp)
//{
//	Static_Stru * staticLed;

//	gp->type = LED_MODE_LIGHT_ON;
//	gp->runValue = LED_rundata_ram;
//	gp->runValue_len = Static_LEN;

//	staticLed = (Static_Stru *)(gp->runValue);

//	staticLed->staticRgb_flag = FALSE;

//}

//void Static_Start(LE_Group_Value_Uin gV,LED_Property_Stru* gp)
//{
//	Static_Stru * staticLed; 

//	staticLed = (Static_Stru *)(gp->runValue);

//	staticLed->staticRgb_flag = TRUE;
//	staticLed->lightness = gV.staticLed.transparency;

//	if (gV.staticLed.random)
//	{
//		Led_EffectCode_Update_Random_Color(&(staticLed->staticRgb));
//	}else
//	{
//		staticLed->staticRgb = gV.staticLed.rgb;
//	}

////	staticLed->transparency =  gV.staticLed.transparency;
//}

void Static_Stop(LED_Property_Stru* gp)
{
	Static_Stru * staticLed; 

	staticLed = (Static_Stru *)(gp->runValue);

	staticLed->staticRgb_flag = FALSE;
}

//void Led_Start_LightsOff(void)
//{
//	RGB_STRU rgb = {0,0,0};

//	staticRgb = rgb;

//	staticRgb_flag = TRUE;
//}


//void Led_Static_Scan(void)
//{

//	if ((staticRgb_flag))
//	{
//		staticRgb_flag = FALSE;

//		led_RGB_Stru.red = staticRgb.red;
//		led_RGB_Stru.green = staticRgb.green;
//		led_RGB_Stru.blue = staticRgb.blue;

//		led_pwmData_change = TRUE;
//	}
//}


//void Led_Static_Stop(void)
//{
//	staticRgb_flag = FALSE;
//}



//RGB_Event Static_Scan(LED_Property_Stru* gp)
//{
//	RGB_Event rec = {FALSE,FALSE,{0,0,0}};

//	Static_Stru * staticLed  = (Static_Stru *)(gp->runValue);;

//	if (staticLed->staticRgb_flag)
//	{
//		staticLed->staticRgb_flag = FALSE;

//		rec.had = TRUE;

//		rec.rgb = staticLed->staticRgb;

//		rec.rgb = Led_Effect_Transparency(rec.rgb, staticLed->lightness);
//	}

//	return rec;
//}

