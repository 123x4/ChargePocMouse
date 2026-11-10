#ifndef _GROUP_BREATHE_
#define _GROUP_BREATHE_

#include "led_effect_group_code.h"
#include "external_configuration.h"

#define Breathe_MAX_Index 2

typedef struct
{
	BOOL start;
	UINT8 frameCnt;
	UINT8 frameCycle;
	UINT8 frameCount;
	UINT8 ledColor_index;
	UINT16 customer_mask;
	BOOL changedColor;
	RGB_STRU colorBoard[Breathe_MAX_Index];
	RGB_STRU rgbBack;
	BOOL random;
	UINT8 transparency;
}LE_G_Breathe_Stru;

#define LE_G_Breathe_LEN	(sizeof(LE_G_Breathe_Stru))

void Breathe_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp);
void Breathe_Group_Start(LE_Group_Value_Uin gV,Group_Property* gp);
void Breathe_Group_Stop(Group_Property* gp);
RGB_Event Breathe_Group_Scan(Group_Property* gp);

#endif
