#ifndef _GROUP_WAVES_H_
#define _GROUP_WAVES_H_

#include "led_effect_group_code.h"
#include "external_configuration.h"

#ifdef LE_Group_Waves_enable

#define Waves_Max_LedNum  8

typedef struct
{
	UINT8 ledIndex;
	RGB_STRU rgb;
	UINT8 colorSign;
}LE_G_Waves_Led_Stru;


typedef struct
{
	BOOL start;
	UINT8 speed;
	UINT8 cycleCnt;
	UINT8 colorMax;
	BOOL dircet;
	RGB_STRU *board;
	LE_G_Waves_Led_Stru runDat[Waves_Max_LedNum];
}LE_G_Waves_Stru;

typedef struct
{
	BOOL had;
	LE_G_Waves_Led_Stru* runDat;
}Waves_Ret_Stru;

#define LE_G_Waves_LEN	(sizeof(LE_G_Waves_Stru))

#define LE_G_Waves_Board_0 6
#define LE_G_Waves_Board_1 6
#define LE_G_Waves_Board_2 2

void Waves_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp);
void Waves_Group_Start(LE_Group_Value_Uin gV,Group_Property* gp,UINT8 groupNum);
void Waves_Group_Stop(Group_Property* gp);
Waves_Ret_Stru Waves_Group_Scan(Group_Property* gp,UINT8 groupNum);

#endif

#endif
