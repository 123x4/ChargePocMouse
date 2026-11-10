#ifndef CUSTOM_H		
#define CUSTOM_H

#include "gloab.h"
//#include "test.h"
#include "keydef.h"
//#include "device.h"
#include "led_effect.h"

#include "led_effect_group_code.h"

extern UINT8 Custom_HandleKey(UINT32 button_wheel,REPORT_POINTER * report_pointer);

#define CUSTOM_BUTTON_NUMBER 10

#define Profile_Name_Len 30

#define KEY_STATUS_UP 0 
#define KEY_STATUS_DOWN 1 

#define STD_KEY 0
#define SPEC_KEY 1

#define NO_RELATIONSHIP 0X00
#define MACRO_KEY 0X01
#define ONE_KEYBOARD_KEY 0X02
#define SHORT_KEY 0X03
#define ENABLE_KEY 0X04
#define MULTIMEDIA_KEY 0X05
#define HOT_KEY 0X06
#define MOUSE_KEY 0X07
#define SPECIAL_FUNCTION_KEY 0X08
#define Mouse_Add_Keyboard	0x09

#define MOUSE_KEY_REPORT 0x01
#define KEYBOARD_KEY_REPORT 0x02
#define MULTIMEDIA_KEY_REPORT 0x04

enum{
	ONCE_DPI = 0xC0,
	CYCLE_DPI = 0xC1,	
	LOCK_X = 0xC2,				
	LOCK_Y= 0xC3,			
	DPI_INCREASE = 0xC4,				
	DPI_REDUCE = 0xC5,		
	DYNAMIC_DPI= 0xC6,
	DRIVEN_CUSTOM = 0XC7,
	LED_MODE_SWITCH = 0XC8,
	CUSTOM_TEST_CHANNEL = 0XC9,
	SELECT_MODE = 0XCA,
	PRODUCTION_MODE_KEY = 0XCB,
	PRODUCTION_MODE_LED = 0XCC,
	CU_DPI_Default = 0XCD,
	CU_Zoon_Out = 0XCE,
	CU_Zoon_In = 0XCF,
};

enum{
	REPORT_RATE_125 = 8,
	REPORT_RATE_250 = 4,
	REPORT_RATE_500 = 2,
	REPORT_RATE_1000 = 0,
};

enum{
	DPI_GEAR_1 = 0,
	DPI_GEAR_2,
	DPI_GEAR_3,
	DPI_GEAR_4,
	DPI_GEAR_5,
	DPI_GEAR_6,
	
};

#define STORE_ONCE_SIGN	0XAA
#define MODE_NUM 2
#define MODE_NUM_S 4
#define SYNC_Dat_Len	14


#define  Profile_Name_ModefyIndex	12

#if 1
typedef union{
	struct{
		unsigned char id;
		unsigned char count;
	}macro;
	struct{
		unsigned char modify;
		unsigned char key;
	}shortcut;
	struct{
		unsigned char key_index;
		unsigned char reserved;
	}key;
	struct{
		unsigned char dpi_x;
		unsigned char dpi_y;
	}location;
	struct{
		unsigned char key1;
		unsigned char key2;
	}normalKey;
}CONTENT_T;
#endif

typedef struct{
	unsigned char id;
	CONTENT_T content; 
	CONTENT_T addContent; 
}REGIST_PARA;

typedef struct{
	REGIST_PARA usb_registration;

}KEY_REGIST_T;

typedef struct
{
	UINT16 dpi_x;
	UINT16 dpi_y;
}DPI_CLASS;


typedef struct{
	UINT16 nameID;
	UINT8 name[Profile_Name_Len];
	KEY_REGIST_T key_function[CUSTOM_BUTTON_NUMBER]; 
	DPI_CLASS dpi_class[5];
	UINT8 report_rate;
	UINT8 dpi_gear;
	UINT8 dpi_default;
	UINT8 dpi_max;
	UINT8 aloneXY[6];
	UINT8 pixThresh;
	UINT8 min_sq;
	UINT8 acceleration;
	UINT8 brightness;
	UINT8 enter_LefOff;
	LE_Group_Value LE_value;  
}GLOAB_MODE_TYPE;  //size  129

typedef enum
{
	LED_MODE_NONE = 0,
	LED_MODE_COLORFUL,
	LED_MODE_BREATH,
	LED_MODE_LIGHT_ON,
	LED_MODE_WAVE,
	LED_MODE_REACT,
	LED_MODE_NUM
}LED_MODE;

typedef struct{
	UINT8 sign;
	UINT8 currentMode;
	UINT8 FP_ID;
}GLOAB_SETTING_TYPE;

typedef struct{
	GLOAB_SETTING_TYPE setting;
	GLOAB_MODE_TYPE settingMode;
}GLOAB_SETTING_MODE_TYPE;

typedef struct{
	UINT16 layer_size;
	UINT8  layer_cnt;
}LAYER_Message;

extern GLOAB_SETTING_MODE_TYPE gloabSetting;

extern UINT8 Code_Compile_Timer[];

UINT8 Code_Compile_Timer_Len(void);


#define GLOAB_SETTING_SIZE	sizeof(GLOAB_SETTING_TYPE)
#define MODE_SETTING_SIZE	sizeof(GLOAB_MODE_TYPE)
#define MODE_SETTING_NAME_SIZE	30
#define Code_Compile_Timer_LEN	Code_Compile_Timer_Len()


void Custom_Init(void);
void Custom_Set_DPI(UINT8 gear,BOOL save,BOOL send,BOOL ledAction);
void Custom_Pointer_Init(void);
void Custom_GloabSetting_Init(void);
void Custom_Load_ModeSetting(void);
void Custom_ModeParamter_Init(UINT8 mode);

void Custom_Restore_Default(UINT8*buffer);
void Custom_RestoreAll_Default(void);
void Custom_Paramter_Init(void);
void Custom_DefineKey_Init(void);
void Custom_CodeCompileTimer_Read(UINT8 *buffer);
void Custom_Perform_Init(UINT8 mode);
void Custom_Profile(BOOL sendFlag);

void Custom_Select_Profile(void);
void Bootloadable_Load(void);
void Device_check_Profile(void);
//LAYER_Message Custom_Get_LayerMessage(void);
//void Custom_Set_LayerMessage(UINT8 layerCnt);
//void Custom_Store_LayerSize(UINT16 size);
//
//void Custom_Set_SyncData(UINT8 *fromBuffer);
//void Custom_Get_SyncData(UINT8 *toBuffer);

#endif

