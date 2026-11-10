#ifndef _STATIC_
#define _STATIC_
#include "led2.h"
#include "led_effect_group_code.h"

typedef struct
{
	BOOL staticRgb_flag;
	RGB_STRU staticRgb;
	UINT8 lightness;
}Static_Stru;

#define Static_LEN	(sizeof(Static_Stru))
	
//void Led_Start_LightsOn(RGB_STRU rgb);
//void Led_Start_LightsOff(void);
//void Led_Static_Scan(void);
//void Led_Static_Stop(void);
//void Static_Generate(LE_Group_Value_Uin gV,LED_Property_Stru* gp);
//void Static_Start(LE_Group_Value_Uin gV,LED_Property_Stru* gp);
//RGB_Event Static_Scan(LED_Property_Stru* gp);
void Static_Stop(LED_Property_Stru* gp);
#endif
