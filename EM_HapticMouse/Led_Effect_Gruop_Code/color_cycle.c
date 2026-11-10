#include "color_cycle.h"
#include "led2.h"
#include "custom.h" 


UINT8 RGB_Value[50] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
					13, 16, 19, 22, 25, 28, 31, 34, 37, 40,
					45, 50, 55, 60, 65, 70, 75, 80, 85, 90,
					97, 104,111,118,125,132,139,146,153,160,
					170,180,190,200,210,220,230,240,250,255};
//{0, 2, 4, 6, 8, 10, 12, 14, 16, 18,
//					20, 23, 26, 29, 32, 35, 37, 40,
//					44, 48, 52, 56, 60,
//					66, 72, 78, 84, 90,
//					98, 106,114,122,130,
//					140,150,160,170,180,
//					195,210,225,240,255};
//{0, 3, 6, 9, 12, 16, 20, 24, 28,
//					32, 36, 40, 44, 48, 52, 56, 60,
//					64, 68, 73, 78, 83, 88, 93,
//					98, 103, 108, 114, 120, 126,
//					132, 138, 145, 152, 160,
//					170, 180, 190, 200,
//					210, 225, 240, 255};

ColorCycle_Stru colorCycle_dat;

//void ColorCycle_Generate(LE_Group_Value_Uin gV,LED_Property_Stru* gp)
//{
//	ColorCycle_Stru * cycle;

//	gp->type = LED_MODE_COLORFUL;
//	gp->runValue = LED_rundata_ram;
//	gp->runValue_len = ColorCycle_LEN;

//	cycle = (ColorCycle_Stru *)(gp->runValue);
//	
//	cycle->star = FALSE;


//}

void ColorCycle_Start(LE_Group_Value_Uin gV,LED_Property_Stru* gp)
{
	ColorCycle_Stru * cycle; 

	cycle = (ColorCycle_Stru *)(gp->runValue);

	cycle->star = TRUE;

	cycle->hsv.s = 253;
	cycle->hsv.v = 255;
	cycle->hsv.h = 0;

	cycle->frameCycle = 40 - gV.colorCycle.speed;

	cycle->Cycle = 0;

	cycle->lightness =  gV.colorCycle.transparency;
}

//void ColorCycle_Stop(void)
//{
//	colorCycle_dat.star = FALSE;
//}

void ColorCycle_Stop(LED_Property_Stru* gp)
{
	ColorCycle_Stru * cycle; 

	cycle = (ColorCycle_Stru *)(gp->runValue);

	cycle->star = FALSE;
}
//void ColorCycle_Scan(void)
//{
//	RGB_STRU rgb;

//	if ((!colorCycle_dat.star))
//	{
//		return;
//	}

//	if (colorCycle_dat.Cycle)colorCycle_dat.Cycle--;

//	if (colorCycle_dat.Cycle)
//	{
//		return;
//	}else
//	{
//		colorCycle_dat.Cycle = colorCycle_dat.frameCycle;
//	}



//	colorCycle_dat.hsv.h++;

//	//Tool_UsbSend_32BitData(colorCycle_dat.hsv.h);


//	rgb = ColorCycle_HsvToRgb1(colorCycle_dat.hsv);

//	//Tool_USB_ArrayData(&(rgb.red),3);


//	led_RGB_Stru.red = rgb.red;
//	led_RGB_Stru.green = rgb.green;
//	led_RGB_Stru.blue = rgb.blue;


//	led_pwmData_change = TRUE;
//}
RGB_STRU ColorCycle_HsvToRgb1(HsvColor hsv)
{
	RGB_STRU rgb;
	unsigned char region, remainder, p, q, t;

	if (hsv.s == 0)
	{
		rgb.red = hsv.v;
		rgb.green = hsv.v;
		rgb.blue = hsv.v;

		return rgb;
	}

	/* make region 0-5 */
	region = hsv.h / 50;
	/* find remainder part, make it from 0-255 */
	remainder = hsv.h - (region * 50); 

	//	colorCycle_dat.hsv.s = 255;
	//	colorCycle_dat.hsv.v = 175;
	p = (UINT16)(hsv.v * (255 - hsv.s)) >> 8;
	q = RGB_Value[49 - remainder];//(UINT16)(hsv.v * (255 - ((UINT16)(hsv.s * remainder) >> 8))) >> 8;
	t = RGB_Value[remainder];//(UINT16)(hsv.v * (255 - ((UINT16)(hsv.s * (255 - remainder)) >> 8))) >> 8;
	
	switch (region)
	{
	case 0://43
		rgb.red = hsv.v;
		rgb.green = t;
		rgb.blue = p;

		break;
	case 1:
		rgb.red = q;
		rgb.green = hsv.v;
		rgb.blue = p;

		break;
	case 2:
		rgb.red = p;
		rgb.green = hsv.v;
		rgb.blue = t;

		break;
	case 3:
		rgb.red = p;
		rgb.green = q;
		rgb.blue = hsv.v;

		break;
	case 4:
		rgb.red = t;
		rgb.green = p;
		rgb.blue = hsv.v;

		break;
	default://41
		rgb.red = hsv.v;
		rgb.green = p;
		rgb.blue = q;

		break;
	}

	return rgb;
}

//RGB_Event ColorCycle_Scan(LED_Property_Stru* gp)
//{
//	RGB_Event rgbEvent;

//	ColorCycle_Stru * colorCycle  = (ColorCycle_Stru *)(gp->runValue);; 

//	rgbEvent.had = FALSE;

//	if (!(colorCycle->star))
//	{
//		return rgbEvent;
//	}

//	if (colorCycle->Cycle)
//	{
//		(colorCycle->Cycle)--;
//		return rgbEvent;

//	}else
//	{
//		colorCycle->Cycle = colorCycle->frameCycle;
////		colorCycle->Cycle = 50;
//	}

////	(colorCycle->hsv).h += colorCycle->frameCycle;
//	(colorCycle->hsv).h ++;
//	if((colorCycle->hsv).h >= 300)(colorCycle->hsv).h = 0;

//	rgbEvent.rgb = ColorCycle_HsvToRgb1(colorCycle->hsv);

//	rgbEvent.rgb = Led_Effect_Transparency(rgbEvent.rgb, colorCycle->lightness);

//	rgbEvent.had = TRUE;

//	return rgbEvent;

//}



















