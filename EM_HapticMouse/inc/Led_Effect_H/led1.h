#ifndef LED1_H		
#define LED1_H

#include "device.h"
#include "LedControl.h"
//#include "test.h"


typedef struct
{
	BOOL ledState;
	BOOL blinkCtrl;
	UINT8 pwmReg;

}LED_STATUS_STRU;

typedef struct
{
	LED_STATUS_STRU red;
	LED_STATUS_STRU green;
	LED_STATUS_STRU blue;

}LED_RGB_STRU;


typedef struct
{
	UINT8 regGroupCnt;
	UINT8 bitLoation;
}LED_REG_STRU;

typedef struct
{
	UINT8 firstAdder;
	UINT8 count;
	UINT8 mask;
}LED_PWM_DATA_STRU;

typedef struct
{
	LED_REG_STRU red;
	LED_REG_STRU green;
	LED_REG_STRU blue;
}LED_RGB_REG_STRU;

typedef struct
{
	UINT8 red;
	UINT8 green;
	UINT8 blue;        
}RGB_STRU;

typedef struct
{
	UINT16 red;
	UINT16 green;
	UINT16 blue;        
}RGB_Bigger_STRU;


typedef struct
{
	BOOL had;
	BOOL complete;
	RGB_STRU rgb;
}RGB_Event;

typedef struct
{
	BOOL had;
	BOOL complete;
	RGB_STRU *rgb;
}String_RGB_Event;

#define  LED_NUMBER 21//32

#define  LED_GROUP_NUM	4//8

#define LED_SEPARATE_SEND 1
#define LED_ALL_SEND 0

#define LED_MODIFY_STATE 0X01
#define LED_MODIFY_BLINK 0X02
#define LED_MODIFY_PWM	 0X04

#define LED_COLOR_RED 0
#define LED_COLOR_GREEN 1
#define LED_COLOR_BLUE 2

#define LED_LINE_NUM	18

#define LED_SPERATE_WRITE_TEM_CNT 15




///MATRIX ALL BYTE
#define LED_PWM_ALL_DATALINE	(8 * 18)
#define LED_BLINK_ALL_DATALINE	(18)
#define LED_STATE_ALL_DATALINE	(18)


//#define LED_GROUP0_NUM 3
//#define LED_GROUP1_NUM 1
//#define LED_GROUP2_NUM 3
//#define LED_GROUP3_NUM 1
//#define LED_GROUP4_NUM 1
//#define LED_GROUP5_NUM 7
//#define LED_GROUP6_NUM 6
//#define LED_GROUP7_NUM 6

//#define LED_MAX_GROUP_NUM 7

//typedef enum
//{
//	LED_GROUP_0 = 0,
//	LED_GROUP_1,
//	LED_GROUP_2,
//	LED_GROUP_3,
//	LED_GROUP_4,
//	LED_GROUP_5,
//	LED_GROUP_6,
//	LED_GROUP_7,
//	MAX_LED_GROUP,
//}LED_GROUP;

//extern UINT8 led_sendType;

//extern UINT8 *ledGroup[LED_GROUP_NUM];

//extern LED_RGB_STRU led_stru[LED_NUMBER];

//extern UINT8 led_pwmRegData[LED_PWM_ALL_DATALINE];
//extern UINT8 led_blinkRegData[LED_BLINK_ALL_DATALINE];
//extern UINT8 led_stateRegData[LED_STATE_ALL_DATALINE];
//extern const UINT8 ledGroup_index[5][8];

//void Led_Init(void);
//void Led_Test(void);
//void Led_I2C_SeparateSend_StructData(void);
//void Led_Write_StructData(LED_RGB_STRU *stru,UINT8 * regData,UINT8 dataType,UINT8 sendType);
//void Led_Check_EFT(void);
//void Led_Lightness_Init(void);
//void Led_ShutDown(void);
//void Led_PowerUp(void);
//void Led_Lightness_Adjust(UINT8 class);
//void Led_Reg_ClearDefault(UINT8 slaverAdr);
//void Led_Write_GCC(UINT8 slaveAdr,UINT8 dat);
//void Led_Adjust_Brightness(UINT8 gcc);


#endif
