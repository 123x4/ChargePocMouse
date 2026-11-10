#ifndef _GROUP_COLOR_CYCLE_
#define _GROUP_COLOR_CYCLE_

#include "led_effect_group_code.h"
#include "external_configuration.h"


typedef struct
{
	UINT8 h;
	UINT8 s;
	UINT8 v;
} LE_G_HsvColor;

typedef struct
{
	BOOL start;
	LE_G_HsvColor hsv;
	UINT8 frameCycle;
	UINT8 Cycle;
	UINT8 transparency;
}LE_G_ColorCycle_Stru;

#define LE_G_ColorCycle_LEN	(sizeof(LE_G_ColorCycle_Stru))

void ColorCycle_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp);
void ColorCycle_Group_Start(LE_Group_Value_Uin gV,Group_Property* gp);
void ColorCycle_Group_Stop(Group_Property* gp);
RGB_Event ColorCycle_Group_Scan(Group_Property* gp);



#endif
