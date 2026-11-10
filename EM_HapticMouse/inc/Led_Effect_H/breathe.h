#ifndef _BREATHE_
#define _BREATHE_

#include "led2.h"
#include "led_effect_group_code.h"

#define Breathe_MAX_Index 2

typedef struct
{
	BOOL breath_start;
	UINT8 lightness;
	UINT8 frameCnt;
	UINT8 frameCycle;
	UINT8 frameCount;
	UINT8 gradientCycle;
	UINT8 ledColor_index;
	UINT16 customer_mask;
	BOOL changedColor;
	RGB_STRU Breath_RGB[Breathe_MAX_Index];
	RGB_STRU breath_rgb;
	BOOL random;
}LED_BREATH_CYCLE_STRU;

#define LE_G_Breathe_LEN	(sizeof(LED_BREATH_CYCLE_STRU))

void Led_Breath_Cycle(void);
RETURN_STRUCT Led_BreatheAlgorithm1(void);
void Led_Breath_Stop(void);
void Led_Breath_Start(LE_Group_Value_Uin led_setting);
//void Breathe_Generate(LE_Group_Value_Uin gV,LED_Property_Stru* gp);
//void Breathe_Start(LE_Group_Value_Uin gV,LED_Property_Stru* gp);
UINT8  Led_Cutomer_ColorIndex_Changed(LED_BREATH_CYCLE_STRU * breathe);
//RGB_Event Breathe_Scan(LED_Property_Stru* gp);
void Breathe_Stop(LED_Property_Stru* gp);
extern UINT8 pwm_t;
	
#endif
