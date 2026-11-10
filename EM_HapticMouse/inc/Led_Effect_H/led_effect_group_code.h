#ifndef _LED_EFFECT_GROUP_CODE_H_
#define _LED_EFFECT_GROUP_CODE_H_

//#include "device.h"
#include "external_configuration.h"




#define Led_Effect_Group_Enable

#if 0
#define LE_Group_Num	7


#define LE_Group0_LedNum	6
#define LE_Group1_LedNum	6
#define LE_Group2_LedNum	3
#define LE_Group3_LedNum	8
#define LE_Group4_LedNum	1
#define LE_Group5_LedNum	1
#define LE_Group6_LedNum	1
#endif

#define LE_Group_Num	1


#define LE_Group0_LedNum	18
#define LE_Group1_LedNum	1
#define LE_Group2_LedNum	1
#define LE_Group3_LedNum	1
//#define LE_Group4_LedNum	3

enum
{
	LE_G_CODE_None			= 0,
	LE_G_CODE_Waves			= 1,
	LE_G_CODE_Breathe		= 2,
	LE_G_CODE_Static		= 3,
	LE_G_CODE_ColorCycle	= 4,
	LE_G_CODE_Reaction		= 5,
};




///for flash store and driver communicate 
typedef union
{
	struct  
	{
		UINT8 transparency;
		UINT8 stop;
		UINT8 speed;	
		UINT8 reserve[8];
	}colorCycle;
	struct  
	{
		UINT8 transparency;
		BOOL random;
		UINT8 speed;
		UINT16 customer_mask;
		RGB_STRU rgb0;
		RGB_STRU rgb1;
	}breathe;
	struct  
	{
		UINT8 transparency;
		UINT8 stop;
		UINT8 speed;
		UINT8 effectIndex;
		UINT8 direct;
		UINT8 reserve[6];
	}waves;
	struct  
	{
		UINT8 transparency;
		BOOL random;
		UINT8 speed;	
		RGB_STRU rgb0;
		RGB_STRU rgb1;
		UINT8 two;
		UINT8 reserve[1];
	}reaction;
	struct  
	{
		UINT8 transparency;
		BOOL random;
		RGB_STRU rgb;
		UINT8 reserve[6];
	}staticLed;
	
}LE_Group_Value_Uin; //size 11


typedef struct  
{
	UINT8 type;
	LE_Group_Value_Uin typeValue;
}LE_Group_Value;   //size 12

typedef struct
{	
	BOOL mul_interface;
	LE_Group_Value typeCode[LE_Group_Num];

}LE_Group_storeValue;  //size 1+(12x7)=85

#define LE_Group_RunValue_LEN	(sizeof(LE_Group_storeValue))

extern const UINT8 LE_Group_LedNum[LE_Group_Num];
extern const RGB_STRU blasoul_RGB;
extern UINT8 optical_scan_cycle;
///for FW running


typedef struct
{
	UINT8 type;
	UINT8 *runValue;
	UINT16 runValue_len;
}LED_Property_Stru;


//typedef struct
//{
//	Group_Property groupArray[LE_Group_Num];
//}LE_Group_Stru;

typedef struct
{
	const UINT8 * groupAdr;
	UINT8 ledNum;
}LE_Group_location;

typedef struct
{
	BOOL flag;
	UINT8 dat;
}Ramdom_Dat;

void Led_EffectCode_Group_Pointer_Init(void);
void Led_EffectCode_Group_Generate(void);
void Led_EffectCode_Group_Start(void);
void Led_EffectCode_Group_Scan(void);



//RGB_STRU Led_EffectCode_Random_Color(void);
//void Led_EffectCode_Update_Random_Color(RGB_STRU *rgb);
void Led_EffectCode_Update_Random_Color_Test(void);

void Led_EffectCode_Group_Stop(void);;

RGB_Bigger_STRU Led_Effect_Bigger_Transparency(UINT8 transparency,RGB_Bigger_STRU rgb);

void LedEffect_Group_Stop(void);
void LedEffect_Group_Start(void);

#endif
