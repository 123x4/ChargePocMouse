#ifndef _GROUP_STATIC_
#define _GROUP_STATIC_


#include "led_effect_group_code.h"
#include "external_configuration.h"


typedef struct
{
	BOOL start;
	RGB_STRU rgb;
	UINT8 transparency;
}LE_G_Static_Led_Stru;

#define LE_G_Static_LEN	(sizeof(LE_G_Static_Led_Stru))

void Static_Group_Start(LE_Group_Value_Uin gV,Group_Property* gp);
void Static_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp);
RGB_Event Static_Group_Scan(Group_Property* gp);
void Static_Group_Stop(Group_Property* gp);

#endif
