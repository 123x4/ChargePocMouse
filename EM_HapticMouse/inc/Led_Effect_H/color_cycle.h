#ifndef _COLOR_CYCLE_
#define _COLOR_CYCLE_

#include "gloab.h"
#include "led_effect_group_code.h"

typedef struct
{
	UINT16 h;
	UINT8 s;
	UINT8 v;
} HsvColor;

typedef struct
{
	BOOL star;
	HsvColor hsv;
	UINT8 lightness;
	UINT8 frameCycle;
	UINT8 Cycle;
	UINT8 startframe;
}ColorCycle_Stru;

#define ColorCycle_LEN	(sizeof(ColorCycle_Stru))

void ColorCycle_Start(LE_Group_Value_Uin gV,LED_Property_Stru* gp);
//void ColorCycle_Stop(void);
//void ColorCycle_Scan(void);
//void ColorCycle_Generate(LE_Group_Value_Uin gV,LED_Property_Stru* gp);
//RGB_Event ColorCycle_Scan(LED_Property_Stru* gp);
void ColorCycle_Stop(LED_Property_Stru* gp);

#endif
