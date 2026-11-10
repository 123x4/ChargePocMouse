#ifndef _REACTION_
#define _REACTION_

#include "led2.h"
#include "led_effect_group_code.h"

typedef struct
{
	BOOL flag;
	BOOL Trg;
	BOOL keypress;
	UINT8 lightness;
	UINT8 frameCnt;
	UINT8 frameCycle;
	UINT8 frameCount;
	UINT8 ledColor_index;
	UINT8 phase;
	UINT16 customer_mask;
	RGB_STRU led_RGB[2];
	BOOL twoColor; 
	BOOL ramdom;
}LED_REACT_STRU;

#define Reaction_LEN	(sizeof(LED_REACT_STRU))

//void Led_React_Stop(void);
//void Led_React_Start(BOOL KeyPress);
//RETURN_STRUCT Led_React_Algorithm();
//void Led_React_Scan(void);
//void Reaction_Generate(LE_Group_Value_Uin gV,LED_Property_Stru* gp);
void Reaction_Start(LE_Group_Value_Uin gV,LED_Property_Stru* gp);
//RGB_Event Reaction_Scan(LED_Property_Stru* gp);
RGB_Event Reaction_Algorithm(LED_Property_Stru* gp);
void Reaction_Stop(LED_Property_Stru* gp);

#endif
