#ifndef _LED_EFFECT_H_
#define _LED_EFFECT_H_


#include "gloab.h"
#include "led1.h"


#define LE_PutOff_Sec	60

enum
{
   DPI_LED2 = 2,
   DPI_LED3 = 3,
   DPI_LED4 = 4,
};
enum
{
 PROFILE_LED = 0,
};

typedef enum
{
	COLOR_NONE = 0,
	RED,
	ORANGE,
	YELLOW,
	GREEN,
	BLUE,
	DING,
	ZI,
	WHITE,
	SEVEN_COLOR //9
}COLOR_TYPE;



typedef enum
{
	SINGLE_COLOR_BREATH = 0,
	COLORFUL_CYCLE,
}LED_EFFECT_MODE;

typedef enum
{
	SINGLE_COLOR_CHANGE = 0,
	MANY_COLOR_CHANGE,

}LED_COLOR_NUM_CHANGE;

enum
{
	PHASE_NONE = 0,
	PHASE_ONE,
	PHASE_TWO,
	PHASE_THREE,
};

enum
{
	LE_PHASE_NONE = 0,
	LE_PHASE_Cal_Process,
	LE_PHASE_Dark,
	LE_PHASE_Bright,
};

#define SevenColorBorad_NUM	6

#define LE_EnterOff_Gradation	8

typedef struct  
{
	BOOL complete;
	BOOL hadData;
}RETURN_STRUCT;

typedef struct  
{
	BOOL start;
	UINT8 timer;
}LED_PROFILE_STRU;

typedef struct
{
	BOOL flag;
	UINT8 count;
	UINT8 backCount;
	BOOL put_off;
	UINT8 sec;
	UINT8 effect_pahse;
	UINT8 effect_bright;
	UINT8 effect_backBright;
}LE_EnterOff_Stru;


//extern LE_EnterOff_Stru LE_enterOff;

//extern BOOL ledStru_DataChang;

//extern const RGB_STRU sevenColor_board[SevenColorBorad_NUM] ;




//void Led_Updata_StruData(void);


//void Led_Structure_Test(UINT8 *buf);


//void Led_Red(void);

//void LightEffect_Chang(void);
//void LightEffect_AssignedLedStru(UINT8 ledIndex,RGB_STRU dat);

//BOOL Led_GradientData(UINT8 *fromData,UINT8 toData,UINT8 speed);
//BOOL Led_RGB_GradientData(RGB_STRU *from,RGB_STRU to,UINT8 speed);

//BOOL Led_Bigger_RGB_GradientData(RGB_Bigger_STRU *from,RGB_STRU to,RGB_Bigger_STRU speed);

//void LedEffect_Dpi_Start(UINT8 dpiGear);
//void LedEffect_Dpi_Scan(void);

//void LedEffect_Put_StaticColor(RGB_STRU dat,BOOL excludeDPI,UINT8 sendType);
//void Select_Assigned_Dat(RGB_STRU dat ,BOOL excludeDPI);

//void LedEffect_Start(void);
//void LedEffect_Profile_Start(UINT8 dpiGear);
//void LedEffect_Profile_Stop(void);
//void Led_Scan(void);


//void Led_Enter_LefOff_Set(UINT8 ledOff);
//void Led_Enter_LefOff_ClearCount(UINT8 gcc);
//void Led_Enter_LefOff_Scan(void);

#endif 
