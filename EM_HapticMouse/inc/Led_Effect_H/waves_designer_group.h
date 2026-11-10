#ifndef _GROUP_WAVES_DESIGNER_H_
#define _GROUP_WAVES_DESIGNER_H_



#include "led_effect_group_code.h"
#include "external_configuration.h"

typedef struct
{
	RGB_STRU RGB;
	UINT16 timer;
}Waves_TimerLine;

typedef struct
{
	UINT8 index;
	RGB_STRU RGB;
	UINT16 timer;
}Waves_Run_Init;

typedef struct
{
	UINT8 ledIndex;
	RGB_Bigger_STRU rgb;
	UINT16 timer;
	UINT8 timerLine_index;
}Waves_Designer_Led_Stru;

typedef struct
{
	BOOL start;
	UINT8 cycle;
	UINT8 speedFrame;
	UINT8 ledNum;
	UINT8 lineNum;
	BOOL dirct;
	UINT8 transparency;
	Waves_TimerLine *timerLine;
	RGB_Bigger_STRU *speed;
	Waves_Designer_Led_Stru *runDat;
}Waves_Designer_Stru;

typedef struct
{
	BOOL had;
	Waves_Designer_Led_Stru * runDat;
}Waves_Designer_Ret_Stru;

#define LIGHT_EFFECT_SCAN_SPEED	30

#define DesignerWaves_Speed_LEN (sizeof(RGB_Bigger_STRU))
#define DesignerWaves_led_LEN	(sizeof(Waves_Designer_Led_Stru))
#define DesignerWaves_LEN	(sizeof(Waves_Designer_Stru))
#define DesignerWaves_TimerLine_LEN	(sizeof(Waves_TimerLine))


#define DW_Effect_Num	  1

#define DW_UseLed_Num	20//26

#define DesignerWave_TimerLine_0_Num	8//7
#define DesignerWave_TimerLine_1_Num	5//4//3
#define DesignerWave_TimerLine_2_Num	7//6//5
#define DesignerWave_TimerLine_3_Num	5//4//3



void Waves_Designer_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp,UINT8 groupNum);
Waves_Designer_Ret_Stru  Waves_Designer_Group_Scan(Group_Property* gp);
void Waves_Designer_Group_Stop(Group_Property* gp);




#endif
