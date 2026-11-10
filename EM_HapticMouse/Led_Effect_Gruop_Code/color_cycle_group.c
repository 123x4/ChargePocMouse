#include "color_cycle_group.h"
#include "led_effect.h"





void ColorCycle_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp)
{
	MALLOC_STRU mallocValue = {NULL,FALSE};
	LE_G_ColorCycle_Stru * cycle;

	mallocValue = RamMalloc_Assign(LE_G_ColorCycle_LEN);

	if (!mallocValue.hadSpace)
	{
		return;
	}

	gp->type = LE_G_CODE_ColorCycle;
	gp->runValue = mallocValue.address;
	gp->runValue_len = LE_G_ColorCycle_LEN;

	cycle = (LE_G_ColorCycle_Stru *)(gp->runValue);
	
	cycle->start = FALSE;


}

void ColorCycle_Group_Start(LE_Group_Value_Uin gV,Group_Property* gp)
{
	LE_G_ColorCycle_Stru * cycle; 

	cycle = (LE_G_ColorCycle_Stru *)(gp->runValue);

	cycle->start = TRUE;

	cycle->hsv.s = 255;
	cycle->hsv.v = 255;//175;
	cycle->hsv.h = 0;

	cycle->frameCycle = gV.colorCycle.speed;

	cycle->Cycle = 0;

	cycle->transparency =  gV.colorCycle.transparency;
}

void ColorCycle_Group_Stop(Group_Property* gp)
{
	LE_G_ColorCycle_Stru * cycle; 

	cycle = (LE_G_ColorCycle_Stru *)(gp->runValue);

	cycle->start = FALSE;
}


RGB_STRU ColorCycle_HsvToRgb(LE_G_HsvColor hsv)
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
	region = hsv.h / 43;
	/* find remainder part, make it from 0-255 */
	remainder = (hsv.h - (region * 43)) * 6; 


	p = (UINT16)(hsv.v * (255 - hsv.s)) >> 8;
	q = (UINT16)(hsv.v * (255 - ((UINT16)(hsv.s * remainder) >> 8))) >> 8;
	t = (UINT16)(hsv.v * (255 - ((UINT16)(hsv.s * (255 - remainder)) >> 8))) >> 8;

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

RGB_Event ColorCycle_Group_Scan(Group_Property* gp)
{
	RGB_Event rgbEvent;

	LE_G_ColorCycle_Stru * colorCycle  = (LE_G_ColorCycle_Stru *)(gp->runValue);; 

	rgbEvent.had = FALSE;

	if (!(colorCycle->start))
	{
		return rgbEvent;
	}

	if (colorCycle->Cycle)
	{
		(colorCycle->Cycle)--;
		return rgbEvent;

	}else
	{
		//colorCycle->Cycle = colorCycle->frameCycle;
		colorCycle->Cycle = 50;
	}

	(colorCycle->hsv).h += colorCycle->frameCycle;

	rgbEvent.rgb = ColorCycle_HsvToRgb(colorCycle->hsv);

	rgbEvent.rgb = Led_Effect_Transparency(colorCycle->transparency,rgbEvent.rgb);

	rgbEvent.had = TRUE;

	return rgbEvent;

}
















