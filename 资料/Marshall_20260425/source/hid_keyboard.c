/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdint.h>
#include <usb_device_config.h>
#include <usb.h>
#include <usb_device.h>
#include <usb_device_class.h>
#include <usb_device_hid.h>
#include <usb_device_ch9.h>
#include <usb_device_descriptor.h>
#include <composite.h>
#include <hid_keyboard.h>
#include <pin_mux.h>
#include <dma_config.h>
#include <hall_spi.h>
#include <Flash_iap.h>
#include <led.h>
#include <testapp_communication.h>
#include <driverapp_communication.h>
#include <macro.h>
#include <flash_zb25v.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TIME_200MS 		200000//125*8*200
#define TIME_5S 		40000//
/*******************************************************************************
 * FUNCTION
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_KeyboardBuffer[USB_HID_KEYBOARD_REPORT_LENGTH];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_BitmapBufferIn[HS_HID_Bitmap_INTERRUPT_IN_PACKET_SIZE];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_KeyboardBuffer1_DKS[USB_HID_KEYBOARD_REPORT_LENGTH];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_KeyboardBuffer2_DKS[USB_HID_KEYBOARD_REPORT_LENGTH];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_CustomBufferIn[USB_HID_Custom_REPORT_IN_LENGTH];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_CustomBufferOut[USB_HID_Custom_REPORT_OUT_LENGTH];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_ConsumerBufferIn[USB_HID_Consumer_REPORT_IN_LENGTH];
static usb_device_composite_struct_t *s_UsbDeviceComposite;
static usb_device_hid_keyboard_struct_t s_UsbDeviceHidKeyboard;

static unsigned char s_kb_complete =0;
unsigned char usb_init_last_command_done = 0;


USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_SETREPORT_BUFFER[USB_HID_SETREPORT_BUFFER_LENGTH];

unsigned short trigger_point_adjust_end_timer;
uint8_t Flag_FN_Status = 0;
uint8_t Flag_once = 0;
uint8_t Flag_FN_SPACE_Status = 0;
uint8_t Flag_FN_ESC_Status = 0;
uint8_t Flag_Need_ResetDefault = 0;
uint32_t Tick_FN_ESC_ResetDefault = 0;
//---------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t Segment_Last_SPI[TRANSFER_Group][TRANSFER_SIZE];
//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Current_key_state[TRANSFER_Group][TRANSFER_SIZE]= {0};
uint8_t Flag_KeyPressWithoutFN[TRANSFER_Group][TRANSFER_SIZE] = {0};
//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t LastCurrent_key_state[TRANSFER_Group][TRANSFER_SIZE]= {0};
//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Flag_Continue_TriggerMode_HasTrigger[TRANSFER_Group][TRANSFER_SIZE]= {0};
//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t KeyFunctionType[TRANSFER_Group][TRANSFER_SIZE]= {0};
uint8_t KeyCode_Buffer[TRANSFER_Group][TRANSFER_SIZE]= {0};
uint8_t LED_HIGHLIGHT_Buffer[TRANSFER_Group][TRANSFER_SIZE] = {0};
//----------------------------------------------------------------------------------------------------------------------------------------------------
/* 驱动通用 */
uint8_t Flag_DriverKeyStart[6][21];
//----------------------------------------------------------------------------------------------------------------------------------------------------
/*  一个DKS可以放四个按键，每个按键一个状态 */
uint16_t Flag_DKS_MODE[20][4]= {0};
uint8_t DKSKey_NeedBreak[20][4]= {0};
uint8_t DKSKey_State[20][4][4]= {0};
uint8_t Flag_DKSKey_HasDone[20][4]= {0};
/* TGL */
uint8_t  Flag_TGL_Press[20];
uint32_t TGLKey_CNT[20];
uint8_t  Flag_TGLKey_NeedBreak[20];
/* MT */
uint8_t  TGLKey[20];
uint8_t  Flag_MT_Press[20];
uint32_t MTKey_CNT[20];
uint8_t  Flag_MT_FunPress[20];
uint8_t  Flag_MTKey_NeedBreak[20];
//----------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Has_Key_Report_Interface0 = 0;
uint8_t Has_Key_Report_Interface3 = 0;
uint8_t Has_Key_Report_Interface1_DKS = 0;
uint8_t Has_Key_Report_Interface2_DKS = 0;
uint8_t Flag_FNMode_Init = 0;
uint8_t Flag_FNAndTAB_Press = 0;
uint8_t Tick_10ms_TestAPP = 0;
uint8_t Current_key_Press_cnt = 0;
uint8_t Buffer_Index = 3;
DrivetData_TriggerPoint   	g_DrivetData_TriggerPoint;
DrivetData_PBPoint   	  	g_DrivetData_PBPoint;
DrivetData_ContinuePoint   	g_DrivetData_ContinuePoint;
DrivetData_DKS   	 		g_DrivetData_DKS;
DrivetData_TGL   	 		g_DrivetData_TGL;
DrivetData_MT   	 		g_DrivetData_MT;
DrivetData_KeyMatrix		g_DrivetData_KeyMatrix;
//---------------------------------------------------------------------------------------------------------------------------------------------------
/* 注释表示hall的位置，{0,0}表示这个位置的hall对应矩阵是在哪行哪列 */
const Position HallLayout_Position_Table[6][21] =
{
//		0		 	1       	2       	3       	4       	5       	6       	7       	8       	9       	10       	11       	12       	13        14       15       16        17       18       19       20
//SPI0	UP	 	 	Right		Down		Left 		CtrlR		APP			FN			Alt_R		SPACE		Alt_L		Win_L	 	CtrlL   	NC			NC        NC	   NC	   	NC	      NC	   NC	    NC     	 NC
		{{4,12},	{5,10},		{5,9},  	{5,8},  	{5,7},  	{5,6},  	{5,5},  	{5,4},  	{5,3},  	{5,2},  	{5,1},  	{5,0},  	NULL_F,  	NULL_F,   NULL_F,  NULL_F,  NULL_F,   NULL_F,  NULL_F,  NULL_F,  NULL_F,},
//		0		 	1       	2       	3       	4       	5       	6       	7       	8       	9       	10       	11       	12       	13        14       15       16        17       18       19       20
//SPI1	ShiftR	 	/?	 		.>			,<	 		M	 		N		 	B	 		V	 		C	 		X	 		Z  	  		ShiftL      NC	    	NC	 	  NC	   NC	    NC        NC	   NC       NC       NC
		{{4,11},  	{4,10},  	{4,9},  	{4,8},  	{4,7},  	{4,6},  	{4,5},  	{4,4},  	{4,3},  	{4,2},  	{4,1},  	{4,0},  	NULL_F,  	NULL_F,   NULL_F,  NULL_F,  NULL_F,   NULL_F,  NULL_F,  NULL_F,  NULL_F,},
//		wodu		1       	2       	3       	4       	5       	6       	7       	8       	9       	10       	11       	12       	13        14       15       16        17       18       19       20
//SPI2	Backsp		=+			-_		 	0		 	9		 	8		 	7		 	6		 	5		 	4		 	3		  	2	   		1	    	`~	      ESC	   NC       NC	      NC	   NC	    NC       NC
		{{1,13},  	{1,12},  	{1,11},  	{1,10},  	{1,9},  	{1,8},  	{1,7},  	{1,6},  	{1,5},  	{1,4},  	{1,3},  	{1,2},  	{1,1},  	{0,0},    {1,0},  NULL_F,  NULL_F,   NULL_F,  NULL_F,  NULL_F,  NULL_F,},
//		0		 	1       	2       	3       	4       	5       	6       	7       	8       	9       	10       	11       	12       	13        14       15       16        17       18       19       20
//SPI3	PgUp	 	Home		Ins		 	F12		 	F11		 	F10		 	F9		 	F8		 	F7		 	F6		 	F5	  	  	F4	   		F3			F2		  F1	   NC	    NC	     NC	       NC	    NC	     NC
		{{1,16},  	{1,15},  	{1,14},  	{0,12},  	{0,11},  	{0,10},  	{0,9},  	{0,8},  	{0,7},  	{0,6},  	{0,5},  	{0,4},  	{0,3},  	{0,2},    {0,1},   NULL_F,  NULL_F,   NULL_F,  NULL_F,  NULL_F,  NULL_F,},
//		0		 	1       	2       	3       	4       	5       	6       	7       	8       	9       	10       	11       	12       	13        14       15       16        17       18       19       20
//SPI4	PgDn		END		 	DEL		 	K29		 	]}		 	[{		 	P		 	O		 	I		 	U		 	Y		  	T	  	 	R	   	 	E	  	  W	   	   Q        TAB	      NC	   NC	    NC       NC
		{{2,16},  	{2,15},  	{2,14},  	{2,13},  	{2,12},  	{2,11},  	{2,10},  	{2,9},  	{2,8},  	{2,7},  	{2,6},  	{2,5},  	{2,4},  	{2,3},    {2,2},   {2,1},   {2,0},   NULL_F,  NULL_F,  NULL_F,  NULL_F,},
//		0		 	1       	2       	3       	4       	5       	6       	7       	8       	9       	10       	11       	12       	13        14       15       16        17       18       19       20
//SPI6	ENTER		'"		 	;:		 	L		 	K		 	J		 	H		 	G		 	F		 	D		 	S		  	A	  	 	CAPS	   	NC	  	  NC	   	   NC        NC	      NC	   NC	    NC       NC
		{{3,12},  	{3,11},  	{3,10},  	{3,9},  	{3,8},  	{3,7},  	{3,6},  	{3,5},  	{3,4},  	{3,3},  	{3,2},  	{3,1},  	{3,0},  	NULL_F,    NULL_F,   NULL_F,   NULL_F,   NULL_F,  NULL_F,  NULL_F,  NULL_F,},
};
/* 这个矩阵用来存有特殊组合键,这个是默认的矩阵，如果驱动改了这个矩阵对应的按键功能，需要更新g_variableData.Driver_KeyMatrix_Table这个矩阵 */
const uint16_t KeyBoardLayout_Table[6][21] =
{
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R0
	//ESC			1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   		17	  		18		 	19         	20
	KEY_TILDE,		KEY_F1,		KEY_F2,    	KEY_F3,   	KEY_F4,    	KEY_F5,    	KEY_F6,    	KEY_F7,    	KEY_F8,    	KEY_F9,		KEY_F10,		KEY_F11,		KEY_F12,   		RESERVE,		RESERVE, 	RESERVE,  	RESERVE,		RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R1
	//0			 	1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   		17		  	18		 	19         	20
	KEY_ESC,  		KEY_1,		KEY_2,  	KEY_3,		KEY_4,		KEY_5,		KEY_6, 		KEY_7,  	KEY_8, 		KEY_9,  	KEY_0,			KEY_MINUS,		KEY_PLUS,		KEY_BACKSPACE,  KEY_INSERT, KEY_HOME,  	KEY_PAGEUP,		RESERVE,	RESERVE,	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R2
	//0			 	1			2		  	3			4  			5			6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   		17		  	18		 	19      	20
	KEY_TAB,    	KEY_Q,		KEY_W,    	KEY_E,   	KEY_R,    	KEY_T,    	KEY_Y,    	KEY_U,    	KEY_I,    	KEY_O,		KEY_P,			KEY_OBRACE,		KEY_CBRACE, 	KEY_BACKSLASH,	KEY_DELETE, KEY_END,  	KEY_PAGEDOWN,  	RESERVE,  	RESERVE,  	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R3
	//0			 	1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		   	 16		   		17		  	18		 	19         	20
	KEY_CAPS,    	KEY_A,		KEY_S,    	KEY_D,   	KEY_F,    	KEY_G,    	KEY_H,    	KEY_J,    	KEY_K,    	KEY_L,		KEY_COLON,		KEY_QUOTE,		KEY_ENTER,  	RESERVE,  		RESERVE, 	RESERVE, 	RESERVE,  		RESERVE,  	RESERVE,  	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R4
	//0			 	1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   		17		  	18		 	19         	20
	KEY_L_SHIFT, 	KEY_Z,		KEY_X,    	KEY_C,   	KEY_V,    	KEY_B,    	KEY_N,    	KEY_M,    	KEY_COMMA,  KEY_DOT,	KEY_QUESTION,	KEY_R_SHIFT,	KEY_UP, 		RESERVE,  		RESERVE,	RESERVE,  	RESERVE,  		RESERVE,  	RESERVE,	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R5
	//0			 	1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   		17		  	18		 	19         	20
	KEY_L_CTRL,  	KEY_L_WIN,	KEY_L_ALT,  KEY_SPACE,	KEY_R_ALT,	KEY_FN,		KEY_APP,	KEY_R_CTRL, KEY_LEFT,  	KEY_DOWN, 	KEY_RIGHT,  	RESERVE,		RESERVE,		RESERVE,		RESERVE,  	RESERVE,  	RESERVE,  		RESERVE,	RESERVE,	RESERVE,	RESERVE,

};
/* 这个矩阵用来对应纯码值，没有特殊功能键对应的码值 */
const uint16_t KeyBoardLayout_NormalCode[6][21] =
{
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R0
	//ESC			1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   		17	  		18		 	19         	20
	KEY_TILDE,		KEY_F1,		KEY_F2,    	KEY_F3,   	KEY_F4,    	KEY_F5,    	KEY_F6,    	KEY_F7,    	KEY_F8,    	KEY_F9,		KEY_F10,		KEY_F11,		KEY_F12,   		RESERVE,		RESERVE, 	RESERVE,  	RESERVE,		RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R1
	//0			 	1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   		17		  	18		 	19         	20
	KEY_ESC,  		KEY_1,		KEY_2,  	KEY_3,		KEY_4,		KEY_5,		KEY_6, 		KEY_7,  	KEY_8, 		KEY_9,  	KEY_0,			KEY_MINUS,		KEY_PLUS,		KEY_BACKSPACE,  KEY_INSERT, KEY_HOME,  	KEY_PAGEUP,		RESERVE,	RESERVE,	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R2
	//0			 	1			2		  	3			4  			5			6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   		17		  	18		 	19      	20
	KEY_TAB,    	KEY_Q,		KEY_W,    	KEY_E,   	KEY_R,    	KEY_T,    	KEY_Y,    	KEY_U,    	KEY_I,    	KEY_O,		KEY_P,			KEY_OBRACE,		KEY_CBRACE, 	KEY_BACKSLASH,	KEY_DELETE, KEY_END,  	KEY_PAGEDOWN,  	RESERVE,  	RESERVE,  	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R3
	//0			 	1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		   	 16		   		17		  	18		 	19         	20
	KEY_CAPS,    	KEY_A,		KEY_S,    	KEY_D,   	KEY_F,    	KEY_G,    	KEY_H,    	KEY_J,    	KEY_K,    	KEY_L,		KEY_COLON,		KEY_QUOTE,		KEY_ENTER,  	RESERVE,  		RESERVE, 	RESERVE, 	RESERVE,  		RESERVE,  	RESERVE,  	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R4
	//0			 	1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   		17		  	18		 	19         	20
	KEY_L_SHIFT, 	KEY_Z,		KEY_X,    	KEY_C,   	KEY_V,    	KEY_B,    	KEY_N,    	KEY_M,    	KEY_COMMA,  KEY_DOT,	KEY_QUESTION,	KEY_R_SHIFT,	KEY_UP, 		RESERVE,  		RESERVE,	RESERVE,  	RESERVE,  		RESERVE,  	RESERVE,	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R5
	//0			 	1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   		17		  	18		 	19         	20
	KEY_L_CTRL,  	KEY_L_WIN,	KEY_L_ALT,  KEY_SPACE,	KEY_R_ALT,	KEY_FN,		KEY_APP,	KEY_R_CTRL, KEY_LEFT,  	KEY_DOWN, 	KEY_RIGHT,  	RESERVE,		RESERVE,		RESERVE,		RESERVE,  	RESERVE,  	RESERVE,  		RESERVE,	RESERVE,	RESERVE,	RESERVE,

};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const uint16_t KeyBoardLayout_Consumer_Table[18] =
{
	KEY_Media,           KEY_PLAY_PAUSE,       KEY_Scan_NextTrack,    KEY_Scan_PreviousTrack,    KEY_Stop,       KEY_Mute,          KEY_VolumeUp,      KEY_VolumeDown,    KEY_AL_Email,
	KEY_AL_Calculator,   KEY_AL_MyComputer,    KEY_AC_Home,           KEY_AC_Search,             KEY_AC_Back,    KEY_AC_Forward,    KEY_AC_stop,       KEY_AC_Refresh,    KEY_AC_Favorites
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const uint16_t KeyBoardLayout_FN_Table[6][21] =
{
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R3
	//0			 1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   	17		  	18		 	19         	20
	KEY_L_SHIFT, KEY_Z,		KEY_X,    	KEY_C,   	KEY_V,    	KEY_B,    	KEY_N,    	KEY_M,    	KEY_COMMA,  KEY_DOT,	KEY_QUESTION,	KEY_R_SHIFT,	KEY_UP, 		KEY_PAGEDOWN,  	RESERVE,	RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R4
	//0			 1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   	17		  	18		 	19         	20
	KEY_L_CTRL,  KEY_L_WIN,	KEY_L_ALT,  KEY_SPACE,	KEY_R_ALT,	KEY_FN,		KEY_R_CTRL, KEY_LEFT,  	KEY_DOWN, 	KEY_RIGHT,  RESERVE,		RESERVE,		RESERVE,		RESERVE,  		RESERVE,  	RESERVE,  	RESERVE,	RESERVE,	RESERVE,	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R1
	//0			 1			2		  	3			4  			5			6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   	17		  	18		 	19      	20
	KEY_TAB,    KEY_Q,		KEY_W,    	KEY_E,   	KEY_R,    	KEY_T,    	KEY_Y,    	KEY_U,    	KEY_I,    	KEY_O,		KEY_P,			KEY_OBRACE,		KEY_CBRACE, 	KEY_BACKSLASH,  KEY_DELETE, RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R2
	//0			 1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		   	 16		   	17		  	18		 	19         	20
	KEY_CAPS,    KEY_A,		KEY_S,    	KEY_D,   	KEY_F,    	KEY_G,    	KEY_H,    	KEY_J,    	KEY_K,    	KEY_L,		KEY_COLON,		KEY_QUOTE,		KEY_ENTER,  	KEY_PAGEUP,  	RESERVE, 	RESERVE, 	RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R0
	//ESC		1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   	17	  		18		 	19         	20
	KEY_ESC,	KEY_1,		KEY_2,    	KEY_3,   	KEY_4,    	KEY_5,    	KEY_6,    	KEY_7,    	KEY_8,    	KEY_9,		KEY_0,			KEY_MINUS,		KEY_PLUS,   	KEY_BACKSPACE,	KEY_INSERT, RESERVE,  	RESERVE,	RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/* 这个矩阵用来对应纯码值，没有特殊功能键对应的码值 */
const uint16_t KeyBoardLayout_FNMODE[5][21] =
{
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R3
	//0			 1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   	17		  	18		 	19         	20
	KEY_L_SHIFT, KEY_Z,		KEY_X,    	KEY_C,   	KEY_V,    	KEY_B,    	KEY_N,    	KEY_M,    	KEY_COMMA,  KEY_DOT,	KEY_QUESTION,	KEY_R_SHIFT,	KEY_UP, 		KEY_PAGEDOWN,  	RESERVE,	RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R4
	//0			 1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   	17		  	18		 	19         	20
	KEY_L_CTRL,  KEY_L_WIN,	KEY_L_ALT,  KEY_SPACE,	KEY_R_ALT,	KEY_FN,		KEY_R_CTRL, KEY_LEFT,  	KEY_DOWN, 	KEY_RIGHT,  RESERVE,		RESERVE,		RESERVE,		RESERVE,  		RESERVE,  	RESERVE,  	RESERVE,	RESERVE,	RESERVE,	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R1
	//0			 1			2		  	3			4  			5			6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   	17		  	18		 	19      	20
	KEY_TAB,    KEY_Q,		KEY_W,    	KEY_E,   	KEY_R,    	KEY_T,    	KEY_Y,    	KEY_U,    	KEY_I,    	KEY_O,		KEY_P,			KEY_OBRACE,		KEY_CBRACE, 	KEY_BACKSLASH,  KEY_DELETE, RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R2
	//0			 1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		   	 16		   	17		  	18		 	19         	20
	KEY_CAPS,    KEY_A,		KEY_S,    	KEY_D,   	KEY_F,    	KEY_G,    	KEY_H,    	KEY_J,    	KEY_K,    	KEY_L,		KEY_COLON,		KEY_QUOTE,		KEY_ENTER,  	KEY_PAGEUP,  	RESERVE, 	RESERVE, 	RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//R0
	//ESC		1			2		  	3			4  			5		   	6		 	7			8			9			10       		11				12		   		13				14		   	15		    16		   	17	  		18		 	19         	20
	KEY_ESC,	KEY_1,		KEY_2,    	KEY_3,   	KEY_4,    	KEY_5,    	KEY_6,    	KEY_7,    	KEY_8,    	KEY_9,		KEY_0,			KEY_MINUS,		KEY_PLUS,   	KEY_BACKSPACE,	KEY_INSERT, RESERVE,  	RESERVE,	RESERVE,  	RESERVE,  	RESERVE,  	RESERVE,
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void Key_Process(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Keyindex);

void Key_Process_Special_Case(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Keyindex);

void Key_Normal_Press(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Position_Row, uint8_t Position_Col, uint8_t Keyindex);
void Key_Normal_Break(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Position_Row, uint8_t Position_Col, uint8_t Keyindex);

void Key_ActualTrigger_Press(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Position_Row, uint8_t Position_Col, uint8_t Keyindex);
void Key_ActualTrigger_Break(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Position_Row, uint8_t Position_Col, uint8_t Keyindex);

void Key_ContinueTrigger_Press(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Position_Row, uint8_t Position_Col, uint8_t Keyindex);

void HandleDKSKey_Press(uint8_t Keycode, uint8_t DKS_Index, uint8_t KeyIndex);

void MediaKey_Handle(uint8_t SPI_GROUP, uint8_t Keyindex,uint8_t Press_Break, uint8_t KeyCode);

void DKSKey_Handle(uint8_t SPI_GROUP, uint16_t Hall_Segment, uint8_t Keyindex, uint8_t Position_Row, uint8_t Position_Col);

void TGLKey_Handle(uint8_t SPI_GROUP, uint16_t Hall_Segment, uint8_t Keyindex, uint8_t Position_Row, uint8_t Position_Col);

void MTKey_Handle(uint8_t SPI_GROUP, uint16_t Hall_Segment, uint8_t Keyindex, uint8_t Position_Row, uint8_t Position_Col);

void MACROKey_Handle(uint8_t SPI_GROUP, uint16_t Hall_Segment, uint8_t Keyindex, uint8_t Position_Row, uint8_t Position_Col);

void FNNormal_Handle(uint8_t SPI_GROUP, uint8_t Keyindex,uint8_t Press_Break, uint8_t KeyCode);

void FNFunction_Handle(uint8_t SPI_GROUP, uint8_t Keyindex,uint8_t Press_Break, uint8_t KeyCode);

void FNtab_Handle(uint8_t SPI_GROUP, uint8_t Keyindex,uint8_t Press_Break, uint8_t KeyCode);

void FNFunction_Table(uint8_t Function_index);
/*******************************************************************************
 * Code
 ******************************************************************************/
//River code begin
unsigned char* USB_DeviceHidKeyboardGetReportBuff(void)
{
	return s_KeyboardBuffer;
}
unsigned char* USB_DeviceHidGenericGetReportBuffIn(void)
{
	return s_ConsumerBufferIn;
}
unsigned char* USB_DeviceHidCustomGetReportBuffIn(void)
{
	return s_CustomBufferIn;
}
unsigned char* USB_DeviceHidCustomGetReportBuffOut(void)
{
	return s_CustomBufferOut;
}
usb_status_t USB_DeviceHidKeyboardReport(void)
{
	unsigned int time_out=0;
	if (s_UsbDeviceComposite->attach)
	{
		//----------------------------------------------------------------------------------------------------------
		/* Interface 0 */
		if(Has_Key_Report_Interface0)
		{
				s_ConsumerBufferIn[0] = 4;
			s_kb_complete = 1;
				if(kStatus_USB_Success == USB_DeviceHidSend(s_UsbDeviceComposite->hidMouseHandle,USB_HID_MOUSE_ENDPOINT_IN,
						s_ConsumerBufferIn,USB_HID_Consumer_REPORT_IN_LENGTH))
				{
					 Has_Key_Report_Interface0 = 0;
			}
		}
		//----------------------------------------------------------------------------------------------------------
		/* Interface 1 */
		if(Has_Key_Report_Interface1_DKS)
		{
				s_KeyboardBuffer1_DKS[0] = 1;
			s_kb_complete = 1;
				if(kStatus_USB_Success == USB_DeviceHidSend(s_UsbDeviceComposite->hidKeyboardHandle,USB_HID_KEYBOARD_ENDPOINT_IN,
											//s_KeyboardBuffer1_DKS,USB_HID_KEYBOARD_REPORT_LENGTH))
						s_KeyboardBuffer1_DKS,USB_HID_KEYBOARD_REPORT_LENGTH))
				{
					FinishKeycodeSend=1;
					Has_Key_Report_Interface1_DKS = 0;
			}
		}
		//----------------------------------------------------------------------------------------------------------
		/* Interface 3 */
		if(Has_Key_Report_Interface2_DKS)
		{
				s_KeyboardBuffer2_DKS[0] = 2;
			s_kb_complete = 1;
				if(kStatus_USB_Success == USB_DeviceHidSend(s_UsbDeviceComposite->hidBitmapHandle,USB_HID_Bitmap_ENDPOINT_IN,
						s_KeyboardBuffer2_DKS,USB_HID_Bitmap_REPORT_IN_LENGTH))
				{
					FinishKeycodeSend=1;
					Has_Key_Report_Interface2_DKS = 0;
			}
		}
		//----------------------------------------------------------------------------------------------------------
		/* Interface 2 */
		if(Has_Key_Report_Interface3)
		{
			s_kb_complete = 1;
				if(kStatus_USB_Success == USB_DeviceHidSend(s_UsbDeviceComposite->hidCustomHandle,USB_HID_Custom_ENDPOINT_IN,
											  s_CustomBufferIn,USB_HID_Custom_REPORT_IN_LENGTH))
				{
					Has_Key_Report_Interface3 = 0;
			}
		}
		//----------------------------------------------------------------------------------------------------------
	}
	return kStatus_USB_Success;

}
#if 0
usb_status_t USB_DeviceHidKeyboardAction(void)
{
    static int x = 0U;
    usb_status_t error = kStatus_USB_Error;
    enum
    {
        DOWN,
        UP
    };
    static uint8_t dir = DOWN;

    s_UsbDeviceHidKeyboard.buffer[2] = 0x00U;
    switch (dir)
    {
        case DOWN:
            x++;
            if (x > 200U)
            {
                dir++;
                s_UsbDeviceHidKeyboard.buffer[4] = 0;//KEY_PAGEUP;
            }
            break;
        case UP:
            x--;
            if (x < 1U)
            {
                dir                              = DOWN;
                s_UsbDeviceHidKeyboard.buffer[4] = 0;//KEY_PAGEDOWN;
            }
            break;
        default:
            break;
    }
    s_UsbDeviceHidKeyboard.buffer[0] = 1;
    //hall_mcu_TX_disable();
    error= USB_DeviceHidSend(s_UsbDeviceComposite->hidKeyboardHandle, USB_HID_KEYBOARD_ENDPOINT_IN,
                             s_UsbDeviceHidKeyboard.buffer, USB_HID_KEYBOARD_REPORT_LENGTH);
    //hall_mcu_TX_enable();

    return error;
}
#endif
usb_status_t USB_DeviceHidBitmapCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    //hall_mcu_int0_disable();
    switch (event)
    {
        case kUSB_DeviceHidEventSendResponse:
            //if (s_UsbDeviceComposite->attach)
            {
            	//hall_mcu_TX_disable();
                //return USB_DeviceHidKeyboardAction();
            	if_can_send_data =1;
            	//hall_mcu_TX_enable();
            	error = kStatus_USB_Success;
            }
            break;
        case kUSB_DeviceHidEventRecvResponse:
        case kUSB_DeviceHidEventGetReport:
        case kUSB_DeviceHidEventSetReport:
        	error = kStatus_USB_Success;
        	break;
        case kUSB_DeviceHidEventRequestReportBuffer:
        	error = kStatus_USB_Success;
            break;
        case kUSB_DeviceHidEventGetIdle:
        case kUSB_DeviceHidEventGetProtocol:
        case kUSB_DeviceHidEventSetIdle:
        case kUSB_DeviceHidEventSetProtocol:
            error = kStatus_USB_Success;
            break;
        default:
            break;
    }
    //hall_mcu_int0_enable();
    return error;
}

usb_status_t USB_DeviceHidCustomCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    //hall_mcu_int0_disable();
    switch (event)
    {
        case kUSB_DeviceHidEventSendResponse:
            //if (s_UsbDeviceComposite->attach)
            {
            	if_can_send_data =1;
            	error = kStatus_USB_Success;
            }

            break;
       case kUSB_DeviceHidEventRecvResponse:
           if (s_UsbDeviceComposite->attach)
            {
        	   USB_DeviceHidCustomReportOutCallBack();
			   return USB_DeviceHidRecv(s_UsbDeviceComposite->hidCustomHandle,USB_HID_Custom_ENDPOINT_OUT,
					   s_CustomBufferOut,USB_HID_Custom_REPORT_OUT_LENGTH);
            }
             break;
        case kUSB_DeviceHidEventGetReport:
        case kUSB_DeviceHidEventSetReport:
        	error = kStatus_USB_Success;
        	break;
        case kUSB_DeviceHidEventRequestReportBuffer:
        	error = kStatus_USB_Success;
            break;
        case kUSB_DeviceHidEventGetIdle:
        case kUSB_DeviceHidEventGetProtocol:
        case kUSB_DeviceHidEventSetIdle:
        case kUSB_DeviceHidEventSetProtocol:
            error = kStatus_USB_Success;
            break;
        default:
            break;
    }
    return error;
}

/* 这是USB加载数据后，总线拿走数据后会回调这个callback */
usb_status_t USB_DeviceHidKeyboardCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;

    switch (event)
    {
        case kUSB_DeviceHidEventSendResponse:
            //if (s_UsbDeviceComposite->attach)
            {
            	error = kStatus_USB_Success;
            	//if(if_can_send_data ==0)
            	{
            		if_can_send_data =1;
            	}
            	//return USB_DeviceHidKeyboardAction();
            }
            break;

        case kUSB_DeviceHidEventRecvResponse:
             error = kStatus_USB_Success;
             break;
        case kUSB_DeviceHidEventGetReport:
        	//-----------------------------------------------------------------------------------------------------
        	/*
        	 * A complete SET REPORT will generate two callback,  the first callback is the event kUSB_DeviceHidEventRequestReportBuffer,
        	 * in this event, a buffer must be set to prepare for saving SET REPORT data,
        	 * then it  will generate a callback event kUSB_DeviceHidEventSetReport. Between the two callback, SET REPORT data has been loaded
        	 * into this buffer, so we can get the SET REPORT data in event kUSB_DeviceHidEventSetReport.
        	 * In event kUSB_DeviceHidEventRequestReportBuffer, we get the last data.
        	 */
        case kUSB_DeviceHidEventSetReport:
        	usb_init_last_command_done = 1;
        	error = kStatus_USB_Success;
        	 break;
        case kUSB_DeviceHidEventRequestReportBuffer:
        	(((usb_device_hid_report_struct_t *)param)->reportBuffer) = s_SETREPORT_BUFFER;
        	error = kStatus_USB_Success;
            break;
            //-----------------------------------------------------------------------------------------------------
        case kUSB_DeviceHidEventGetIdle:
        case kUSB_DeviceHidEventGetProtocol:
        case kUSB_DeviceHidEventSetIdle:
        case kUSB_DeviceHidEventSetProtocol:
            break;
        default:
            break;
    }

    return error;
}

usb_status_t USB_DeviceHidCustomSetConfigure(class_handle_t handle, uint8_t configure)
{
    if (USB_COMPOSITE_CONFIGURE_INDEX == configure)
    {
#if 0
    	USB_DeviceHidSend(s_UsbDeviceComposite->hidCustomHandle,
    			USB_HID_Custom_ENDPOINT_IN,
				s_CustomBufferIn,
				USB_HID_Custom_REPORT_IN_LENGTH);
#else
    	USB_DeviceHidRecv(s_UsbDeviceComposite->hidCustomHandle,
    			USB_HID_Custom_ENDPOINT_OUT,
				s_CustomBufferOut,
				USB_HID_Custom_REPORT_OUT_LENGTH);
        //return USB_DeviceHidKeyboardAction(); /* run the cursor movement code */
#endif
    	s_kb_complete=0;
        return kStatus_USB_Success;
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidKeyboardSetConfigure(class_handle_t handle, uint8_t configure)
{
    if (USB_COMPOSITE_CONFIGURE_INDEX == configure)
    {
    	s_KeyboardBuffer1_DKS[0] = 1;
    	USB_DeviceHidSend(s_UsbDeviceComposite->hidKeyboardHandle,
    												USB_HID_KEYBOARD_ENDPOINT_IN,
													s_KeyboardBuffer1_DKS,
													USB_HID_KEYBOARD_REPORT_LENGTH);
        //return USB_DeviceHidKeyboardAction(); /* run the cursor movement code */
    	s_kb_complete=0;
        return kStatus_USB_Success;
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidBitmapSetConfigure(class_handle_t handle, uint8_t configure)
{
    if (USB_COMPOSITE_CONFIGURE_INDEX == configure)
    {
        //return USB_DeviceHidKeyboardAction(); /* run the cursor movement code */
    	s_KeyboardBuffer2_DKS[0] = 2;
    	USB_DeviceHidSend(s_UsbDeviceComposite->hidBitmapHandle,
    											USB_HID_Bitmap_ENDPOINT_IN,
												s_KeyboardBuffer2_DKS,
												USB_HID_Bitmap_REPORT_IN_LENGTH);
    	s_kb_complete=0;
        return kStatus_USB_Success;
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidKeyboardSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting)
{
    if (USB_HID_KEYBOARD_INTERFACE_INDEX == interface)
    {
    	s_KeyboardBuffer1_DKS[0] = 1;
    	USB_DeviceHidSend(s_UsbDeviceComposite->hidKeyboardHandle,
    												USB_HID_KEYBOARD_ENDPOINT_IN,
													s_KeyboardBuffer1_DKS,
													USB_HID_KEYBOARD_REPORT_LENGTH);
        //return USB_DeviceHidKeyboardAction(); /* run the cursor movement code */
    	s_kb_complete=0;
    	 return kStatus_USB_Success;
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidBitmapSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting)
{
    if (USB_HID_Bitmap_INTERFACE_INDEX == interface)
    {
    	s_KeyboardBuffer2_DKS[0] = 2;
    	USB_DeviceHidSend(s_UsbDeviceComposite->hidBitmapHandle,
    											USB_HID_Bitmap_ENDPOINT_IN,
												s_KeyboardBuffer2_DKS,
												HS_HID_Custom_INTERRUPT_OUT_PACKET_SIZE);
    	s_kb_complete=0;
    	 return kStatus_USB_Success;
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidCustomSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting)
{
    if (USB_HID_Custom_INTERFACE_INDEX == interface)
    {
#if 0
    	USB_DeviceHidSend(s_UsbDeviceComposite->hidCustomHandle,
    			USB_HID_Custom_ENDPOINT_IN,
				s_CustomBufferIn,
				USB_HID_Custom_REPORT_IN_LENGTH);
#else
    	USB_DeviceHidRecv(s_UsbDeviceComposite->hidCustomHandle,
    			USB_HID_Custom_ENDPOINT_OUT,
				s_CustomBufferOut,
				USB_HID_Custom_REPORT_OUT_LENGTH);
        //return USB_DeviceHidKeyboardAction(); /* run the cursor movement code */
#endif
    	s_kb_complete=0;
    	 return kStatus_USB_Success;
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidKeyboardInit(usb_device_composite_struct_t *deviceComposite)
{
	 usb_init_last_command_done = 0;
    s_UsbDeviceComposite          = deviceComposite;
    s_UsbDeviceHidKeyboard.buffer = s_KeyboardBuffer;
    return kStatus_USB_Success;
}
usb_status_t USB_DeviceHidBitmapInit(usb_device_composite_struct_t *deviceComposite)
{
	 usb_init_last_command_done = 0;
    s_UsbDeviceComposite          = deviceComposite;
  //  s_UsbDeviceHidCustom.buffer = s_CustomBufferIn;


    return kStatus_USB_Success;
}
usb_status_t USB_DeviceHidCustomInit(usb_device_composite_struct_t *deviceComposite)
{
	usb_init_last_command_done = 0;
	s_UsbDeviceComposite          = deviceComposite;
    //s_UsbDeviceHidCustom.buffer = s_CustomBufferIn;
    return kStatus_USB_Success;
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : Hall_Rawdata_ConvertTo_Section
 * Description   : Convert hall raw data to component segment;
 *
 * END ****************************************************************************************************************/
#define Test_GudingBianhuaPanduan 1//用固定的值去判断按键变化
#define Test_FastBreak 1		//去掉快速抬起做的那个不抬起
uint8_t KeyData_Process(void)
{
	uint8_t ret = 1;
	uint16_t Hall_Segment = 0;
	//------------------------------------------------------------------------------------------
	Hall_Sample_Rawdata();
	/****************************************************************************************************/
#if 1
#if 1
	/* SPI 0 */
	//UP
	DMA_SPI_RX_1_Buffer_Store[0] = DMA_SPI_RX_1_Buffer[0]&0x3FFF;

#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[0] < 6600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[0] < (Baseline_Top_SPI0[0] - RawDate_Has_Change))
#endif
	{
		//----------------------------------------------------------------------------------------------
		/* CRC:380ns */
		if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(0))
		{
			//------------------------------------------------------------------------------------------
			DMA_SPI_RX_1_Buffer_BackUp[0] = DMA_SPI_RX_1_Buffer_Store[0];
			//-----------------------------------------------------------------------------------------
			//Segmented---1.3us
			Hall_Segment = Hall_Cal_Node_Data_SPI0(0);
			//------------------------------------------------------------------------------------------
			/* handle Key data */
			Key_Process(Hall_Segment, SPI0_GROUP, 0);
			//------------------------------------------------------------------------------------------
		}
		else
		{
			Flag_RawData_CRC_Check = 0x01;
		}
	}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][0])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 0);
	}
#endif
	//--------------------------------------------------------------------------------------------------
	//Right
	DMA_SPI_RX_1_Buffer_Store[1] = DMA_SPI_RX_1_Buffer[1]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[1] < 5600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[1] < (Baseline_Top_SPI0[1] - RawDate_Has_Change))
#endif
		{
			//----------------------------------------------------------------------------------------------
			/* CRC:380ns */
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(1))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_1_Buffer_BackUp[1] = DMA_SPI_RX_1_Buffer_Store[1];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI0(1);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI0_GROUP, 1);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				Flag_RawData_CRC_Check = 0x02;
			}
		}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][1])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 1);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//Down
	DMA_SPI_RX_1_Buffer_Store[2] = DMA_SPI_RX_1_Buffer[2]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[2] < 5600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[2] < (Baseline_Top_SPI0[2] - RawDate_Has_Change))
#endif
		{
			//----------------------------------------------------------------------------------------------
			/* CRC:380ns */
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(2))
			{
				DMA_SPI_RX_1_Buffer_BackUp[2] = DMA_SPI_RX_1_Buffer_Store[2];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI0(2);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI0_GROUP, 2);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				Flag_RawData_CRC_Check = 0x03;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][2])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 2);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//Left
	DMA_SPI_RX_1_Buffer_Store[3] = DMA_SPI_RX_1_Buffer[3]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[3] < 5600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[3] < (Baseline_Top_SPI0[3] - RawDate_Has_Change))
#endif
		{
			//----------------------------------------------------------------------------------------------
			/* CRC:380ns */
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(3))
			{
				DMA_SPI_RX_1_Buffer_BackUp[3] = DMA_SPI_RX_1_Buffer_Store[3];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI0(3);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI0_GROUP, 3);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				Flag_RawData_CRC_Check = 0x04;
			}
		}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][3])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 3);
	}
#endif
	}

	//--------------------------------------------------------------------------------------------------
	//Ctrlr
	DMA_SPI_RX_1_Buffer_Store[4] = DMA_SPI_RX_1_Buffer[4]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[4] < 5600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[4] < (Baseline_Top_SPI0[4] - RawDate_Has_Change))
#endif
		{
			//----------------------------------------------------------------------------------------------
			/* CRC:380ns */
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(4))
			{
				DMA_SPI_RX_1_Buffer_BackUp[4] = DMA_SPI_RX_1_Buffer_Store[4];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI0(4);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI0_GROUP, 4);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				Flag_RawData_CRC_Check = 0x05;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][4])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 4);
	}
#endif
	}

	//--------------------------------------------------------------------------------------------------
	//App
	DMA_SPI_RX_1_Buffer_Store[5] = DMA_SPI_RX_1_Buffer[5]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[5] < 5600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[5] < (Baseline_Top_SPI0[5] - RawDate_Has_Change))
#endif
		{
			//----------------------------------------------------------------------------------------------
			/* CRC:380ns */
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(5))
			{
				DMA_SPI_RX_1_Buffer_BackUp[5] = DMA_SPI_RX_1_Buffer_Store[5];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI0(5);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI0_GROUP, 5);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				Flag_RawData_CRC_Check = 0x06;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][5])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 5);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//FN
	DMA_SPI_RX_1_Buffer_Store[6] = DMA_SPI_RX_1_Buffer[6]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[6] < 5600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[6] < (Baseline_Top_SPI0[6] - RawDate_Has_Change))
#endif
		{
			//----------------------------------------------------------------------------------------------
			/* CRC:380ns */
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(6))
			{
				DMA_SPI_RX_1_Buffer_BackUp[6] = DMA_SPI_RX_1_Buffer_Store[6];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI0(6);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI0_GROUP, 6);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				Flag_RawData_CRC_Check = 0x07;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][6])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 6);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//Altr
	DMA_SPI_RX_1_Buffer_Store[7] = DMA_SPI_RX_1_Buffer[7]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[7] < 5600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[7] < (Baseline_Top_SPI0[7] - RawDate_Has_Change))
#endif
		{
			//----------------------------------------------------------------------------------------------
			/* CRC:380ns */
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(7))
			{
				DMA_SPI_RX_1_Buffer_BackUp[7] = DMA_SPI_RX_1_Buffer_Store[7];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI0(7);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI0_GROUP, 7);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				Flag_RawData_CRC_Check = 0x08;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][7])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 7);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//Space
	DMA_SPI_RX_1_Buffer_Store[8] = DMA_SPI_RX_1_Buffer[8]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[8] < 5600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[8] < (Baseline_Top_SPI0[8] - RawDate_Has_Change))
#endif
		{
			//----------------------------------------------------------------------------------------------
			/* CRC:380ns */
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(8))
			{
				DMA_SPI_RX_1_Buffer_BackUp[8] = DMA_SPI_RX_1_Buffer_Store[8];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI0(8);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI0_GROUP, 8);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				Flag_RawData_CRC_Check = 0x09;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][8])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 8);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//Altl
	DMA_SPI_RX_1_Buffer_Store[9] = DMA_SPI_RX_1_Buffer[9]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[9] < 5600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[9] < (Baseline_Top_SPI0[9] - RawDate_Has_Change))
#endif
		{
			//----------------------------------------------------------------------------------------------
			/* CRC:380ns */
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(9))
			{
				DMA_SPI_RX_1_Buffer_BackUp[9] = DMA_SPI_RX_1_Buffer_Store[9];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI0(9);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI0_GROUP, 9);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				Flag_RawData_CRC_Check = 0x0A;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][9])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 9);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//win
	DMA_SPI_RX_1_Buffer_Store[10] = DMA_SPI_RX_1_Buffer[10]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[10] < 5600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[10] < (Baseline_Top_SPI0[10] - RawDate_Has_Change))
#endif
		{
			//----------------------------------------------------------------------------------------------
			/* CRC:380ns */
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(10))
			{
				DMA_SPI_RX_1_Buffer_BackUp[10] = DMA_SPI_RX_1_Buffer_Store[10];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI0(10);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI0_GROUP, 10);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				Flag_RawData_CRC_Check = 0x0B;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][10])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 10);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//Ctrll
	DMA_SPI_RX_1_Buffer_Store[11] = DMA_SPI_RX_1_Buffer[11]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_1_Buffer_Store[11] < 5600)
#else
	if(DMA_SPI_RX_1_Buffer_Store[11] < (Baseline_Top_SPI0[11] - RawDate_Has_Change))
#endif
		{
			//----------------------------------------------------------------------------------------------
			/* CRC:380ns */
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(11))
			{
				DMA_SPI_RX_1_Buffer_BackUp[11] = DMA_SPI_RX_1_Buffer_Store[11];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI0(11);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI0_GROUP, 11);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				Flag_RawData_CRC_Check = 0x0C;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI0_GROUP][11])
	{
		Key_Process_Special_Case(0, SPI0_GROUP, 11);
	}
#endif
	}
#endif
	/**************************************************************************************************/
	/* SPI 1 */
	//--------------------------------------------------------------------------------------------------
	//Shiftr
	DMA_SPI_RX_2_Buffer_Store[0] = DMA_SPI_RX_2_Buffer[0]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[0] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[0] < (Baseline_Top_SPI1[0]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(0))
			{
				DMA_SPI_RX_2_Buffer_BackUp[0] = DMA_SPI_RX_2_Buffer_Store[0];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(0);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 0);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x11;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][0])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 0);
	}
#endif
	}

	//--------------------------------------------------------------------------------------------------
	///?
	DMA_SPI_RX_2_Buffer_Store[1] = DMA_SPI_RX_2_Buffer[1]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[1] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[1] < (Baseline_Top_SPI1[1]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(1))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_2_Buffer_BackUp[1] = DMA_SPI_RX_2_Buffer_Store[1];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(1);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 1);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x12;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][1])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 1);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//.>
	DMA_SPI_RX_2_Buffer_Store[2] = DMA_SPI_RX_2_Buffer[2]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[2] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[2] < (Baseline_Top_SPI1[2]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(2))
			{
				DMA_SPI_RX_2_Buffer_BackUp[2] = DMA_SPI_RX_2_Buffer_Store[2];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(2);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 2);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x13;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][2])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 2);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//,<
	DMA_SPI_RX_2_Buffer_Store[3] = DMA_SPI_RX_2_Buffer[3]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[3] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[3] < (Baseline_Top_SPI1[3]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(3))
			{
				DMA_SPI_RX_2_Buffer_BackUp[3] = DMA_SPI_RX_2_Buffer_Store[3];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(3);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 3);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x14;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][3])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 3);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//M
	DMA_SPI_RX_2_Buffer_Store[4] = DMA_SPI_RX_2_Buffer[4]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[4] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[4] < (Baseline_Top_SPI1[4]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(4))
			{
				DMA_SPI_RX_2_Buffer_BackUp[4] = DMA_SPI_RX_2_Buffer_Store[4];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(4);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 4);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x15;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][4])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 4);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//N
	DMA_SPI_RX_2_Buffer_Store[5] = DMA_SPI_RX_2_Buffer[5]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[5] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[5] < (Baseline_Top_SPI1[5]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(5))
			{
				DMA_SPI_RX_2_Buffer_BackUp[5] = DMA_SPI_RX_2_Buffer_Store[5];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(5);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 5);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x16;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][5])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 5);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//B
	DMA_SPI_RX_2_Buffer_Store[6] = DMA_SPI_RX_2_Buffer[6]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[6] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[6] < (Baseline_Top_SPI1[6]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(6))
			{
				DMA_SPI_RX_2_Buffer_BackUp[6] = DMA_SPI_RX_2_Buffer_Store[6];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(6);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 6);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x17;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][6])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 6);
	}
#endif
	}
	//--------------------------------------------------------------------------------------------------
	//V
	DMA_SPI_RX_2_Buffer_Store[7] = DMA_SPI_RX_2_Buffer[7]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[7] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[7] < (Baseline_Top_SPI1[7]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(7))
			{
				DMA_SPI_RX_2_Buffer_BackUp[7] = DMA_SPI_RX_2_Buffer_Store[7];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(7);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 7);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x18;
			}
			//---------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][7])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 7);
	}
#endif
	}
	//-------------------------------------------------------------------------------------------------
	//C
	DMA_SPI_RX_2_Buffer_Store[8] = DMA_SPI_RX_2_Buffer[8]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[8] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[8] < (Baseline_Top_SPI1[8]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(8))
			{
				DMA_SPI_RX_2_Buffer_BackUp[8] = DMA_SPI_RX_2_Buffer_Store[8];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(8);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 8);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x19;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][8])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 8);
	}
#endif
	}

	//--------------------------------------------------------------------------------------------------
	//X
	DMA_SPI_RX_2_Buffer_Store[9] = DMA_SPI_RX_2_Buffer[9]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[9] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[9] < (Baseline_Top_SPI1[9]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(9))
			{
				DMA_SPI_RX_2_Buffer_BackUp[9] = DMA_SPI_RX_2_Buffer_Store[9];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(9);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 9);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x1A;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][9])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 9);
	}
#endif
	}

	//--------------------------------------------------------------------------------------------------
	//Z
	DMA_SPI_RX_2_Buffer_Store[10] = DMA_SPI_RX_2_Buffer[10]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[10] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[10] < (Baseline_Top_SPI1[10]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(10))
			{
				DMA_SPI_RX_2_Buffer_BackUp[10] = DMA_SPI_RX_2_Buffer_Store[10];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(10);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 10);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x1A;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][10])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 10);
	}
#endif
	}

	//--------------------------------------------------------------------------------------------------
	//Shiftl
	DMA_SPI_RX_2_Buffer_Store[11] = DMA_SPI_RX_2_Buffer[11]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_2_Buffer_Store[11] < 5600)
#else
	if(DMA_SPI_RX_2_Buffer_Store[11] < (Baseline_Top_SPI1[11]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(11))
			{
				DMA_SPI_RX_2_Buffer_BackUp[11] = DMA_SPI_RX_2_Buffer_Store[11];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI1(11);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI1_GROUP, 11);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x1A;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI1_GROUP][11])
	{
		Key_Process_Special_Case(0, SPI1_GROUP, 11);
	}
#endif
	}

	/****************************************************************************************************/
	/* SPI 2 */
	//--------------------------------------------------------------------------------------------------
	//Backspace
	DMA_SPI_RX_3_Buffer_Store[0] = DMA_SPI_RX_3_Buffer[0]&0x3FFF;
	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[0] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[0] < (Baseline_Top_SPI2[0]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(0))
			{
				DMA_SPI_RX_3_Buffer_BackUp[0] = DMA_SPI_RX_3_Buffer_Store[0];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(0);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 0);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x31;
			}
			//----------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][0])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 0);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//=+
	DMA_SPI_RX_3_Buffer_Store[1] = DMA_SPI_RX_3_Buffer[1]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[1] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[1] < (Baseline_Top_SPI2[1]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(1))
			{
				DMA_SPI_RX_3_Buffer_BackUp[1] = DMA_SPI_RX_3_Buffer_Store[1];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(1);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 1);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x32;
			}
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][1])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 1);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//-_
	DMA_SPI_RX_3_Buffer_Store[2] = DMA_SPI_RX_3_Buffer[2]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[2] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[2] < (Baseline_Top_SPI2[2]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(2))
			{
				DMA_SPI_RX_3_Buffer_BackUp[2] = DMA_SPI_RX_3_Buffer_Store[2];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(2);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 2);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x32;
			}
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][2])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 2);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//0)
	DMA_SPI_RX_3_Buffer_Store[3] = DMA_SPI_RX_3_Buffer[3]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[3] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[3] < (Baseline_Top_SPI2[3]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(3))
			{
				DMA_SPI_RX_3_Buffer_BackUp[3] = DMA_SPI_RX_3_Buffer_Store[3];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(3);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 3);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x34;
			}
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][3])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 3);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//9(
	DMA_SPI_RX_3_Buffer_Store[4] = DMA_SPI_RX_3_Buffer[4]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[4] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[4] < (Baseline_Top_SPI2[4]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(4))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_3_Buffer_BackUp[4] = DMA_SPI_RX_3_Buffer_Store[4];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(4);
				//------------------------------------------------------------------------------------------
				Key_Process(Hall_Segment, SPI2_GROUP, 4);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x35;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][4])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 4);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//8*
	DMA_SPI_RX_3_Buffer_Store[5] = DMA_SPI_RX_3_Buffer[5]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[5] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[5] < (Baseline_Top_SPI2[5]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(5))
			{
				DMA_SPI_RX_3_Buffer_BackUp[5] = DMA_SPI_RX_3_Buffer_Store[5];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(5);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 5);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x36;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][5])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 5);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//7&
	DMA_SPI_RX_3_Buffer_Store[6] = DMA_SPI_RX_3_Buffer[6]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[6] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[6] < (Baseline_Top_SPI2[6]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(6))
			{
				DMA_SPI_RX_3_Buffer_BackUp[6] = DMA_SPI_RX_3_Buffer_Store[6];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(6);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 6);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x37;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][6])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 6);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//6^
	DMA_SPI_RX_3_Buffer_Store[7] = DMA_SPI_RX_3_Buffer[7]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[7] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[7] < (Baseline_Top_SPI2[7]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(7))
			{
				DMA_SPI_RX_3_Buffer_BackUp[7] = DMA_SPI_RX_3_Buffer_Store[7];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(7);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 7);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x38;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][7])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 7);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//5%
	DMA_SPI_RX_3_Buffer_Store[8] = DMA_SPI_RX_3_Buffer[8]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[8] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[8] < (Baseline_Top_SPI2[8]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(8))
			{
				DMA_SPI_RX_3_Buffer_BackUp[8] = DMA_SPI_RX_3_Buffer_Store[8];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(8);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 8);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x39;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][8])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 8);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//4$
	DMA_SPI_RX_3_Buffer_Store[9] = DMA_SPI_RX_3_Buffer[9]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[9] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[9] < (Baseline_Top_SPI2[9]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(9))
			{
				DMA_SPI_RX_3_Buffer_BackUp[9] = DMA_SPI_RX_3_Buffer_Store[9];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(9);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 9);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x3A;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][9])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 9);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//3#
	DMA_SPI_RX_3_Buffer_Store[10] = DMA_SPI_RX_3_Buffer[10]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[10] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[10] < (Baseline_Top_SPI2[10]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(10))
			{
				DMA_SPI_RX_3_Buffer_BackUp[10] = DMA_SPI_RX_3_Buffer_Store[10];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(10);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 10);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x3B;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][10])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 10);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//2@
	DMA_SPI_RX_3_Buffer_Store[11] = DMA_SPI_RX_3_Buffer[11]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[11] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[11] < (Baseline_Top_SPI2[11]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(11))
			{
				DMA_SPI_RX_3_Buffer_BackUp[11] = DMA_SPI_RX_3_Buffer_Store[11];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(11);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 11);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x3C;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][11])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 11);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//1!
	DMA_SPI_RX_3_Buffer_Store[12] = DMA_SPI_RX_3_Buffer[12]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[12] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[12] < (Baseline_Top_SPI2[12]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(12))
			{
				DMA_SPI_RX_3_Buffer_BackUp[12] = DMA_SPI_RX_3_Buffer_Store[12];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(12);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 12);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x3D;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][12])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 12);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//`~
	DMA_SPI_RX_3_Buffer_Store[13] = DMA_SPI_RX_3_Buffer[13]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[13] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[13] < (Baseline_Top_SPI2[13]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(13))
			{
				DMA_SPI_RX_3_Buffer_BackUp[13] = DMA_SPI_RX_3_Buffer_Store[13];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(13);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 13);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x3E;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][13])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 13);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//ESC
	DMA_SPI_RX_3_Buffer_Store[14] = DMA_SPI_RX_3_Buffer[14]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_3_Buffer_Store[14] < 5600)
#else
	if(DMA_SPI_RX_3_Buffer_Store[14] < (Baseline_Top_SPI2[14]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI2(14))
			{
				DMA_SPI_RX_3_Buffer_BackUp[14] = DMA_SPI_RX_3_Buffer_Store[14];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI2(14);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI2_GROUP, 14);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x3F;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI2_GROUP][14])
	{
		Key_Process_Special_Case(0, SPI2_GROUP, 14);
	}
#endif
	}

	/****************************************************************************************************/
	/* SPI 3 */
	//------------------------------------------------------------------------------------------
	//Pgdown
	DMA_SPI_RX_4_Buffer_Store[0] = DMA_SPI_RX_4_Buffer[0]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[0] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[0] < (Baseline_Top_SPI3[0]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(0))
			{
				DMA_SPI_RX_4_Buffer_BackUp[0] = DMA_SPI_RX_4_Buffer_Store[0];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(0);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 0);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x41;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][0])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 0);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//END
	DMA_SPI_RX_4_Buffer_Store[1] = DMA_SPI_RX_4_Buffer[1]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[1] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[1] < (Baseline_Top_SPI3[1]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(1))
			{
				DMA_SPI_RX_4_Buffer_BackUp[1] = DMA_SPI_RX_4_Buffer_Store[1];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(1);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 1);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x42;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][1])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 1);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//DEL
	DMA_SPI_RX_4_Buffer_Store[2] = DMA_SPI_RX_4_Buffer[2]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[2] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[2] < (Baseline_Top_SPI3[2]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(2))
			{
				DMA_SPI_RX_4_Buffer_BackUp[2] = DMA_SPI_RX_4_Buffer_Store[2];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(2);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 2);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x43;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][2])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 2);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//K29
	DMA_SPI_RX_4_Buffer_Store[3] = DMA_SPI_RX_4_Buffer[3]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[3] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[3] < (Baseline_Top_SPI3[3]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(3))
			{
				DMA_SPI_RX_4_Buffer_BackUp[3] = DMA_SPI_RX_4_Buffer_Store[3];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(3);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 3);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x44;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][3])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 3);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//]}
	DMA_SPI_RX_4_Buffer_Store[4] = DMA_SPI_RX_4_Buffer[4]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[4] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[4] < (Baseline_Top_SPI3[4]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(4))
			{
				DMA_SPI_RX_4_Buffer_BackUp[4] = DMA_SPI_RX_4_Buffer_Store[4];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(4);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 4);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x45;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][4])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 4);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//{[
	DMA_SPI_RX_4_Buffer_Store[5] = DMA_SPI_RX_4_Buffer[5]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[5] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[5] < (Baseline_Top_SPI3[5]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(5))
			{
				DMA_SPI_RX_4_Buffer_BackUp[5] = DMA_SPI_RX_4_Buffer_Store[5];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(5);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 5);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x46;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][5])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 5);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//P
	DMA_SPI_RX_4_Buffer_Store[6] = DMA_SPI_RX_4_Buffer[6]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[6] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[6] < (Baseline_Top_SPI3[6]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(6))
			{
				DMA_SPI_RX_4_Buffer_BackUp[6] = DMA_SPI_RX_4_Buffer_Store[6];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(6);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 6);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x47;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][6])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 6);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//O
	DMA_SPI_RX_4_Buffer_Store[7] = DMA_SPI_RX_4_Buffer[7]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[7] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[7] < (Baseline_Top_SPI3[7]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(7))
			{
				DMA_SPI_RX_4_Buffer_BackUp[7] = DMA_SPI_RX_4_Buffer_Store[7];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(7);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 7);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x48;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][7])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 7);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//I
	DMA_SPI_RX_4_Buffer_Store[8] = DMA_SPI_RX_4_Buffer[8]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[8] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[8] < (Baseline_Top_SPI3[8]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(8))
			{
				DMA_SPI_RX_4_Buffer_BackUp[8] = DMA_SPI_RX_4_Buffer_Store[8];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(8);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 8);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x49;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][8])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 8);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//U
	DMA_SPI_RX_4_Buffer_Store[9] = DMA_SPI_RX_4_Buffer[9]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[9] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[9] < (Baseline_Top_SPI3[9]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(9))
			{
				DMA_SPI_RX_4_Buffer_BackUp[9] = DMA_SPI_RX_4_Buffer_Store[9];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(9);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 9);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x4A;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][9])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 9);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//T
	DMA_SPI_RX_4_Buffer_Store[10] = DMA_SPI_RX_4_Buffer[10]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[10] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[10] < (Baseline_Top_SPI3[10]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(10))
			{
				DMA_SPI_RX_4_Buffer_BackUp[10] = DMA_SPI_RX_4_Buffer_Store[10];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(10);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 10);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x4B;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][10])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 10);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//R
	DMA_SPI_RX_4_Buffer_Store[11] = DMA_SPI_RX_4_Buffer[11]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[11] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[11] < (Baseline_Top_SPI3[11]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(11))
			{
				DMA_SPI_RX_4_Buffer_BackUp[11] = DMA_SPI_RX_4_Buffer_Store[11];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(11);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 11);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x4C;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][11])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 11);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//E
	DMA_SPI_RX_4_Buffer_Store[12] = DMA_SPI_RX_4_Buffer[12]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[12] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[12] < (Baseline_Top_SPI3[12]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(12))
			{
				DMA_SPI_RX_4_Buffer_BackUp[12] = DMA_SPI_RX_4_Buffer_Store[12];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(12);
				//------------------------------------------------------------------------------------------
				Key_Process(Hall_Segment, SPI3_GROUP, 12);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x4D;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][12])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 12);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//W
	DMA_SPI_RX_4_Buffer_Store[13] = DMA_SPI_RX_4_Buffer[13]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[13] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[13] < (Baseline_Top_SPI3[13]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(13))
			{
				DMA_SPI_RX_4_Buffer_BackUp[13] = DMA_SPI_RX_4_Buffer_Store[13];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(13);
				//------------------------------------------------------------------------------------------
				//usb_echo("DMA_SPI_RX_4_Buffer_Store[13]:%d\n",DMA_SPI_RX_4_Buffer_Store[13]);
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 13);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x4E;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][13])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 13);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//Q
	DMA_SPI_RX_4_Buffer_Store[14] = DMA_SPI_RX_4_Buffer[14]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[14] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[14] < (Baseline_Top_SPI3[14]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(14))
			{
				DMA_SPI_RX_4_Buffer_BackUp[14] = DMA_SPI_RX_4_Buffer_Store[14];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(14);
				//------------------------------------------------------------------------------------------
				//usb_echo("DMA_SPI_RX_4_Buffer_Store[13]:%d\n",DMA_SPI_RX_4_Buffer_Store[13]);
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 14);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x4E;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][14])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 14);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//TAB
	DMA_SPI_RX_4_Buffer_Store[15] = DMA_SPI_RX_4_Buffer[15]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_4_Buffer_Store[15] < 5600)
#else
		if(DMA_SPI_RX_4_Buffer_Store[15] < (Baseline_Top_SPI3[15]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(15))
			{
				DMA_SPI_RX_4_Buffer_BackUp[15] = DMA_SPI_RX_4_Buffer_Store[15];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI3(15);
				//------------------------------------------------------------------------------------------
				//usb_echo("DMA_SPI_RX_4_Buffer_Store[13]:%d\n",DMA_SPI_RX_4_Buffer_Store[13]);
				/* handle Key data */
				Key_Process(Hall_Segment, SPI3_GROUP, 15);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x4E;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI3_GROUP][15])
	{
		Key_Process_Special_Case(0, SPI3_GROUP, 15);
	}
#endif
	}

#if 1
	/****************************************************************************************************/
	/* SPI 4 */
	//------------------------------------------------------------------------------------------
	//ESC
	DMA_SPI_RX_5_Buffer_Store[0] = DMA_SPI_RX_5_Buffer[0]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[0] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[0] < (Baseline_Top_SPI4[0]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(0))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_5_Buffer_BackUp[0] = DMA_SPI_RX_5_Buffer_Store[0];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(0);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 0);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x71;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][0])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 0);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//1
	DMA_SPI_RX_5_Buffer_Store[1] = DMA_SPI_RX_5_Buffer[1]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[1] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[1] < (Baseline_Top_SPI4[1]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(1))
			{
				DMA_SPI_RX_5_Buffer_BackUp[1] = DMA_SPI_RX_5_Buffer_Store[1];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(1);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 1);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x72;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][1])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 1);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//2
	DMA_SPI_RX_5_Buffer_Store[2] = DMA_SPI_RX_5_Buffer[2]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[2] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[2] < (Baseline_Top_SPI4[2]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(2))
			{
				DMA_SPI_RX_5_Buffer_BackUp[2] = DMA_SPI_RX_5_Buffer_Store[2];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(2);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 2);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x73;
			}
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][2])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 2);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//3
	DMA_SPI_RX_5_Buffer_Store[3] = DMA_SPI_RX_5_Buffer[3]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[3] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[3] < (Baseline_Top_SPI4[3]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(3))
			{
				DMA_SPI_RX_5_Buffer_BackUp[3] = DMA_SPI_RX_5_Buffer_Store[3];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(3);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 3);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x74;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][3])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 3);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//4
	DMA_SPI_RX_5_Buffer_Store[4] = DMA_SPI_RX_5_Buffer[4]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[4] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[4] < (Baseline_Top_SPI4[4]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(4))
			{
				DMA_SPI_RX_5_Buffer_BackUp[4] = DMA_SPI_RX_5_Buffer_Store[4];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(4);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 4);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x75;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][4])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 4);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//5
	DMA_SPI_RX_5_Buffer_Store[5] = DMA_SPI_RX_5_Buffer[5]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[5] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[5] < (Baseline_Top_SPI4[5]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(5))
			{
				DMA_SPI_RX_5_Buffer_BackUp[5] = DMA_SPI_RX_5_Buffer_Store[5];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(5);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 5);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x76;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][5])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 5);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//6
	DMA_SPI_RX_5_Buffer_Store[6] = DMA_SPI_RX_5_Buffer[6]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[6] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[6] < (Baseline_Top_SPI4[6]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(6))
			{
				DMA_SPI_RX_5_Buffer_BackUp[6] = DMA_SPI_RX_5_Buffer_Store[6];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(6);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 6);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x77;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][6])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 6);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//7
	DMA_SPI_RX_5_Buffer_Store[7] = DMA_SPI_RX_5_Buffer[7]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[7] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[7] < (Baseline_Top_SPI4[7]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(7))
			{
				DMA_SPI_RX_5_Buffer_BackUp[7] = DMA_SPI_RX_5_Buffer_Store[7];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(7);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 7);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x78;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][7])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 7);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//8
	DMA_SPI_RX_5_Buffer_Store[8] = DMA_SPI_RX_5_Buffer[8]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[8] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[8] < (Baseline_Top_SPI4[8]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(8))
			{
				DMA_SPI_RX_5_Buffer_BackUp[8] = DMA_SPI_RX_5_Buffer_Store[8];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(8);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 8);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x79;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][8])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 8);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//9
	DMA_SPI_RX_5_Buffer_Store[9] = DMA_SPI_RX_5_Buffer[9]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[9] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[9] < (Baseline_Top_SPI4[9]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(9))
			{
				DMA_SPI_RX_5_Buffer_BackUp[9] = DMA_SPI_RX_5_Buffer_Store[9];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(9);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 9);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7A;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][9])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 9);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//0
	DMA_SPI_RX_5_Buffer_Store[10] = DMA_SPI_RX_5_Buffer[10]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[10] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[10] < (Baseline_Top_SPI4[10]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(10))
			{
				DMA_SPI_RX_5_Buffer_BackUp[10] = DMA_SPI_RX_5_Buffer_Store[10];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(10);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 10);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7B;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][10])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 10);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//-_
	DMA_SPI_RX_5_Buffer_Store[11] = DMA_SPI_RX_5_Buffer[11]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[11] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[11] < (Baseline_Top_SPI4[11]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(11))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_5_Buffer_BackUp[11] = DMA_SPI_RX_5_Buffer_Store[11];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(11);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 11);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7C;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][11])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 11);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//=+
	DMA_SPI_RX_5_Buffer_Store[12] = DMA_SPI_RX_5_Buffer[12]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[12] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[12] < (Baseline_Top_SPI4[12]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(12))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_5_Buffer_BackUp[12] = DMA_SPI_RX_5_Buffer_Store[12];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(12);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 12);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7D;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][12])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 12);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//BACKSP
	DMA_SPI_RX_5_Buffer_Store[13] = DMA_SPI_RX_5_Buffer[13]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[13] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[13] < (Baseline_Top_SPI4[13]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(13))
			{
				DMA_SPI_RX_5_Buffer_BackUp[13] = DMA_SPI_RX_5_Buffer_Store[13];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(13);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 13);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7E;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][13])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 13);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//Ins
	DMA_SPI_RX_5_Buffer_Store[14] = DMA_SPI_RX_5_Buffer[14]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[14] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[14] < (Baseline_Top_SPI4[14]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(14))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_5_Buffer_BackUp[14] = DMA_SPI_RX_5_Buffer_Store[14];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(14);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 14);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7F;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][14])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 14);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//Ins
	DMA_SPI_RX_5_Buffer_Store[15] = DMA_SPI_RX_5_Buffer[15]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[15] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[15] < (Baseline_Top_SPI4[15]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(15))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_5_Buffer_BackUp[15] = DMA_SPI_RX_5_Buffer_Store[15];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(15);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 15);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7F;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][15])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 15);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//Ins
	DMA_SPI_RX_5_Buffer_Store[16] = DMA_SPI_RX_5_Buffer[16]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_5_Buffer_Store[16] < 5600)
#else
		if(DMA_SPI_RX_5_Buffer_Store[16] < (Baseline_Top_SPI4[16]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(16))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_5_Buffer_BackUp[16] = DMA_SPI_RX_5_Buffer_Store[16];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI4(16);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI4_GROUP, 16);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7F;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI4_GROUP][16])
	{
		Key_Process_Special_Case(0, SPI4_GROUP, 16);
	}
#endif
	}

#endif
#if 1
	/****************************************************************************************************/
	/* SPI 6 */
	//------------------------------------------------------------------------------------------
	//ESC
	DMA_SPI_RX_6_Buffer_Store[0] = DMA_SPI_RX_6_Buffer[0]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[0] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[0] < (Baseline_Top_SPI6[0]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(0))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_6_Buffer_BackUp[0] = DMA_SPI_RX_6_Buffer_Store[0];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(0);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 0);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x71;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][0])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 0);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//1
	DMA_SPI_RX_6_Buffer_Store[1] = DMA_SPI_RX_6_Buffer[1]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[1] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[1] < (Baseline_Top_SPI6[1]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(1))
			{
				DMA_SPI_RX_6_Buffer_BackUp[1] = DMA_SPI_RX_6_Buffer_Store[1];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(1);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 1);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x72;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][1])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 1);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//2
	DMA_SPI_RX_6_Buffer_Store[2] = DMA_SPI_RX_6_Buffer[2]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[2] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[2] < (Baseline_Top_SPI6[2]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(2))
			{
				DMA_SPI_RX_6_Buffer_BackUp[2] = DMA_SPI_RX_6_Buffer_Store[2];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(2);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 2);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x73;
			}
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][2])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 2);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//3
	DMA_SPI_RX_6_Buffer_Store[3] = DMA_SPI_RX_6_Buffer[3]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[3] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[3] < (Baseline_Top_SPI6[3]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(3))
			{
				DMA_SPI_RX_6_Buffer_BackUp[3] = DMA_SPI_RX_6_Buffer_Store[3];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(3);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 3);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x74;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][3])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 3);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//4
	DMA_SPI_RX_6_Buffer_Store[4] = DMA_SPI_RX_6_Buffer[4]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[4] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[4] < (Baseline_Top_SPI6[4]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(4))
			{
				DMA_SPI_RX_6_Buffer_BackUp[4] = DMA_SPI_RX_6_Buffer_Store[4];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(4);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 4);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x75;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][4])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 4);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//5
	DMA_SPI_RX_6_Buffer_Store[5] = DMA_SPI_RX_6_Buffer[5]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[5] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[5] < (Baseline_Top_SPI6[5]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(5))
			{
				DMA_SPI_RX_6_Buffer_BackUp[5] = DMA_SPI_RX_6_Buffer_Store[5];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(5);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 5);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x76;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][5])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 5);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//6
	DMA_SPI_RX_6_Buffer_Store[6] = DMA_SPI_RX_6_Buffer[6]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[6] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[6] < (Baseline_Top_SPI6[6]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(6))
			{
				DMA_SPI_RX_6_Buffer_BackUp[6] = DMA_SPI_RX_6_Buffer_Store[6];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(6);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 6);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x77;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][6])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 6);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//7
	DMA_SPI_RX_6_Buffer_Store[7] = DMA_SPI_RX_6_Buffer[7]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[7] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[7] < (Baseline_Top_SPI6[7]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(7))
			{
				DMA_SPI_RX_6_Buffer_BackUp[7] = DMA_SPI_RX_6_Buffer_Store[7];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(7);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 7);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x78;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][7])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 7);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//8
	DMA_SPI_RX_6_Buffer_Store[8] = DMA_SPI_RX_6_Buffer[8]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[8] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[8] < (Baseline_Top_SPI6[8]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(8))
			{
				DMA_SPI_RX_6_Buffer_BackUp[8] = DMA_SPI_RX_6_Buffer_Store[8];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(8);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 8);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x79;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][8])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 8);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//9
	DMA_SPI_RX_6_Buffer_Store[9] = DMA_SPI_RX_6_Buffer[9]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[9] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[9] < (Baseline_Top_SPI6[9]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(9))
			{
				DMA_SPI_RX_6_Buffer_BackUp[9] = DMA_SPI_RX_6_Buffer_Store[9];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(9);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 9);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7A;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][9])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 9);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//0
	DMA_SPI_RX_6_Buffer_Store[10] = DMA_SPI_RX_6_Buffer[10]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[10] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[10] < (Baseline_Top_SPI6[10]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(10))
			{
				DMA_SPI_RX_6_Buffer_BackUp[10] = DMA_SPI_RX_6_Buffer_Store[10];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(10);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 10);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7B;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][10])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 10);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//-_
	DMA_SPI_RX_6_Buffer_Store[11] = DMA_SPI_RX_6_Buffer[11]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[11] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[11] < (Baseline_Top_SPI6[11]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(11))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_6_Buffer_BackUp[11] = DMA_SPI_RX_6_Buffer_Store[11];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(11);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 11);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7C;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][11])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 11);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//=+
	DMA_SPI_RX_6_Buffer_Store[12] = DMA_SPI_RX_6_Buffer[12]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[12] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[12] < (Baseline_Top_SPI6[12]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(12))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_6_Buffer_BackUp[12] = DMA_SPI_RX_6_Buffer_Store[12];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(12);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 12);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7D;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][12])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 12);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//BACKSP
	DMA_SPI_RX_6_Buffer_Store[13] = DMA_SPI_RX_6_Buffer[13]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[13] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[13] < (Baseline_Top_SPI6[13]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(13))
			{
				DMA_SPI_RX_6_Buffer_BackUp[13] = DMA_SPI_RX_6_Buffer_Store[13];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(13);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 13);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7E;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][13])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 13);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//Ins
	DMA_SPI_RX_6_Buffer_Store[14] = DMA_SPI_RX_6_Buffer[14]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[14] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[14] < (Baseline_Top_SPI6[14]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(14))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_6_Buffer_BackUp[14] = DMA_SPI_RX_6_Buffer_Store[14];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(14);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 14);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7F;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][14])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 14);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//Ins
	DMA_SPI_RX_6_Buffer_Store[15] = DMA_SPI_RX_6_Buffer[15]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[15] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[15] < (Baseline_Top_SPI6[15]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(15))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_6_Buffer_BackUp[15] = DMA_SPI_RX_6_Buffer_Store[15];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(15);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 15);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7F;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][15])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 15);
	}
#endif
	}

	//------------------------------------------------------------------------------------------
	//Ins
	DMA_SPI_RX_6_Buffer_Store[16] = DMA_SPI_RX_6_Buffer[16]&0x3FFF;

	{
#if Test_GudingBianhuaPanduan
	if(DMA_SPI_RX_6_Buffer_Store[16] < 5600)
#else
		if(DMA_SPI_RX_6_Buffer_Store[16] < (Baseline_Top_SPI6[16]-RawDate_Has_Change))
#endif
		{
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(16))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_6_Buffer_BackUp[16] = DMA_SPI_RX_6_Buffer_Store[16];
				//------------------------------------------------------------------------------------------
				//Segmented
				Hall_Segment = Hall_Cal_Node_Data_SPI6(16);
				//------------------------------------------------------------------------------------------
				/* handle Key data */
				Key_Process(Hall_Segment, SPI6_GROUP, 16);
			}
			else
			{
				Flag_RawData_CRC_Check = 0x7F;
			}
			//------------------------------------------------------------------------------------------
		}
#if Test_FastBreak
	else if(Current_key_state[SPI6_GROUP][16])
	{
		Key_Process_Special_Case(0, SPI6_GROUP, 16);
	}
#endif
	}

#endif
#endif
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	//TEST_PIN_SET_LOW();
	return ret;
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : TestMode_data_Handle
 * Description   : use this function to handle hall sensor data in test mode.
 *
 * END ****************************************************************************************************************/
uint8_t TestMode_data_Handle(void)
{
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t ret = 1;
	if(TestAPP_ReportData_Type == TestAPP_Byte10_Checkself)
	{
		HALLChain_Check_FREQ_DETECH_Doubling();
	}
	else
	{
		//------------------------------------------------------------------------------------------
		//DMA_StartTransfer_Once_TESTAPP();
		DMA_StartTransfer_OnceFor_RawData();
		DMA_StartTransfer_OnceFor_RawData();
		DMA_StartTransfer_OnceFor_RawData();
		/****************************************************************************************************/
		/* SPI 0 */
		//ESC
		for(i = 0; i < TRANSFER_SIZE; i++)
		{
			if(i < SPI0_TRANSFER_SIZE)
			{
				/* 测试模式不用卡是否有变化才做处理 */
				//------------------------------------------------------------------------------------------
				if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(i))
				{
					//------------------------------------------------------------------------------------------
					DMA_SPI_RX_1_Buffer_BackUp[i] = DMA_SPI_RX_1_Buffer[i]&0x3FFF;
					DMA_SPI_RX_1_Buffer_Store[i] = DMA_SPI_RX_1_Buffer[i]&0x3FFF;
					if(DMA_SPI_RX_1_Buffer_BackUp[i] == 255)
					{
						usb_echo("DMA_SPI_RX_1_Buffer_BackUp[i] = %d.\r\n",DMA_SPI_RX_1_Buffer_BackUp[i]);
					}
					//------------------------------------------------------------------------------------------
					//Segmented
					Hall_Cal_Node_Data_SPI0(i);
					//------------------------------------------------------------------------------------------
				}
			}
			//------------------------------------------------------------------------------------------
			if(i < SPI1_TRANSFER_SIZE)
			{
				if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(i))
				{
					//------------------------------------------------------------------------------------------
					DMA_SPI_RX_2_Buffer_BackUp[i] = DMA_SPI_RX_2_Buffer[i]&0x3FFF;
					DMA_SPI_RX_2_Buffer_Store[i] = DMA_SPI_RX_2_Buffer[i]&0x3FFF;
					//------------------------------------------------------------------------------------------
					//Segmented
					Hall_Cal_Node_Data_SPI1(i);
					//------------------------------------------------------------------------------------------
				}
			}
			//------------------------------------------------------------------------------------------
			if(i < SPI3_TRANSFER_SIZE)
			{
				if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(i))
				{
					//------------------------------------------------------------------------------------------
					DMA_SPI_RX_3_Buffer_BackUp[i] = DMA_SPI_RX_3_Buffer[i]&0x3FFF;
					DMA_SPI_RX_3_Buffer_Store[i] = DMA_SPI_RX_3_Buffer[i]&0x3FFF;
					//------------------------------------------------------------------------------------------
					//Segmented
					Hall_Cal_Node_Data_SPI3(i);
					//------------------------------------------------------------------------------------------
				}
			}
			//------------------------------------------------------------------------------------------
			if(i < SPI4_TRANSFER_SIZE)
			{
				//------------------------------------------------------------------------------------------
				/* CRC:380ns */
				if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(i))
				{
					//------------------------------------------------------------------------------------------
					DMA_SPI_RX_4_Buffer_BackUp[i] = DMA_SPI_RX_4_Buffer[i]&0x3FFF;
					DMA_SPI_RX_4_Buffer_Store[i] = DMA_SPI_RX_4_Buffer[i]&0x3FFF;
					//------------------------------------------------------------------------------------------
					//Segmented
					Hall_Cal_Node_Data_SPI4(i);
					//------------------------------------------------------------------------------------------
				}
			}
			if(i < SPI7_TRANSFER_SIZE)
			{
				if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(i))
				{
					//------------------------------------------------------------------------------------------
					DMA_SPI_RX_5_Buffer_BackUp[i] = DMA_SPI_RX_5_Buffer[i]&0x3FFF;
					DMA_SPI_RX_5_Buffer_Store[i] = DMA_SPI_RX_5_Buffer[i]&0x3FFF;
					//------------------------------------------------------------------------------------------
					//Segmented
					Hall_Cal_Node_Data_SPI7(i);
					//------------------------------------------------------------------------------------------
				}
			}
			//------------------------------------------------------------------------------------------
		}
		return ret;
		//------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : TestMode_Process
 * Description   : handle test mode data in this function;
 *
 * END ****************************************************************************************************************/
void TestMode_Process(void)
{
	if(Sample_Hall_Once)
	{
		if(++Tick_10ms > 80)
		{
			Tick_10ms = 0;
			Sample_Hall_Once = 0;
			Tick_10ms_TestAPP = 0;
			TestMode_data_Handle();
		}
	}
	else
	{
		/* 测试模式下发送的数据慢点，避免测试软件占用太多内存导致电脑卡顿 */
		if(++Tick_10ms_TestAPP > 80)
		{
			Tick_10ms_TestAPP = 0;
			TestAPP_COMMUNICATION();
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Key_AllRelease
 * Description  	: Release all key.
 *
 * END ****************************************************************************************************************/
void Key_AllRelease(void)
{
	Clear_buffer(s_KeyboardBuffer1_DKS,64);
	Clear_buffer(s_KeyboardBuffer2_DKS,64);
	Has_Key_Report_Interface1_DKS = 1;
	Has_Key_Report_Interface2_DKS = 1;
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Key_Handle_Normal_SPI0
 * Description  	: 按键处理.
 *
 * END ****************************************************************************************************************/
void Key_Process(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Keyindex)
{
	uint8_t Position_Row = 0;
	uint8_t Position_Col = 0;
	//判断是正常的键还是有别的功能的键
	//---------------------------------------------------------------------------------------------------------------------------
	Position_Row = HallLayout_Position_Table[SPI_GROUP][Keyindex].row;
	Position_Col = HallLayout_Position_Table[SPI_GROUP][Keyindex].col;
	//---------------------------------------------------------------------------------------------------------------------------
	KeyFunctionType[SPI_GROUP][Keyindex] = g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col];
//	usb_echo("Hall_Segment[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,Hall_Segment);
//	usb_echo("Position_Row:%d\n",Position_Row);
//	usb_echo("Position_Col:%d\n",Position_Col);
//	usb_echo("Segment_Last_SPI:%d\n",Segment_Last_SPI[SPI_GROUP][Keyindex]);
	//---------------------------------------------------------------------------------------------------------------------------
	/* 普通按键：把其他的功能都放在另一个else里面，保证普通按键的操作能达到8K*/
	if((KeyFunctionType[SPI_GROUP][Keyindex] != KeyType_DKS_ID)
		&& (KeyFunctionType[SPI_GROUP][Keyindex] != KeyType_TGL_ID)
		&& (KeyFunctionType[SPI_GROUP][Keyindex] != KeyType_MT_ID)
		&& (KeyFunctionType[SPI_GROUP][Keyindex] != KeyType_MACRO_ID))
	{
		/************************************************************************************************************************/
		/* 按下 */
		if(Hall_Segment > Segment_Last_SPI[SPI_GROUP][Keyindex])
		{
			/* 没有按键按下的时候等按键按到触发值后才按下，按键按下后就每次分段值变化就来更新分段值 */
			if(!Current_key_state[SPI_GROUP][Keyindex])
			{
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					Key_ActualTrigger_Press(Hall_Segment, SPI_GROUP, Position_Row, Position_Col, Keyindex);
				}
				else if(g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					Key_ContinueTrigger_Press(Hall_Segment, SPI_GROUP, Position_Row, Position_Col, Keyindex);
				}
				else
				{
					Key_Normal_Press(Hall_Segment, SPI_GROUP, Position_Row, Position_Col, Keyindex);
				}
			}
			else
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
			}
		}
		//Break
		else if(Hall_Segment < Segment_Last_SPI[SPI_GROUP][Keyindex])
		{
			/* 有按键按下后,Segment_Last道和Current_key_state更新，抬起的时候抬超过触发值后才更新Segment_Last，没按下的时候，Segment_Last都是之前的 */
			if(Current_key_state[SPI_GROUP][Keyindex])
			{
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					Key_ActualTrigger_Break(Hall_Segment, SPI_GROUP, Position_Row, Position_Col, Keyindex);
				}
				else
				{
					Key_ActualTrigger_Break(Hall_Segment, SPI_GROUP, Position_Row, Position_Col, Keyindex);
				}
			}
			else
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
			}
		}
		/************************************************************************************************************************/
	}
	/* 其他功能键 */
	else
	{
		/************************************************************************************************************************/

		//---------------------------------------------------------------------------------------------------------------------------
		/* DKS */
		if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_DKS_ID)
		{
			DKSKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row, Position_Col);
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* TGL */
		else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_TGL_ID)
		{
			TGLKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row, Position_Col);
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* MT */
		else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_MT_ID)
		{
			MTKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row, Position_Col);
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* macro */
		else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_MACRO_ID)
		{
			MACROKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row, Position_Col);
		}
		/************************************************************************************************************************/
	}
}
uint8_t Position_Row_TEST = 0;
uint8_t Position_Col_TEST = 0;
void Key_Process_Special_Case(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Keyindex)
{
//	uint8_t Position_Row = 0;
//	uint8_t Position_Col = 0;
	//判断是正常的键还是有别的功能的键
	//if(Current_key_state[SPI_GROUP][Keyindex])
	{
		//---------------------------------------------------------------------------------------------------------------------------
		Position_Row_TEST = HallLayout_Position_Table[SPI_GROUP][Keyindex].row;
		Position_Col_TEST = HallLayout_Position_Table[SPI_GROUP][Keyindex].col;
		//---------------------------------------------------------------------------------------------------------------------------
		KeyFunctionType[SPI_GROUP][Keyindex] = g_variableData.Driver_KeyMatrix_Table[Position_Row_TEST][Position_Col_TEST];
		//---------------------------------------------------------------------------------------------------------------------------
		/* 普通按键：把其他的功能都放在另一个else里面，保证普通按键的操作能达到8K*/
		if((KeyFunctionType[SPI_GROUP][Keyindex] != KeyType_DKS_ID)
			&& (KeyFunctionType[SPI_GROUP][Keyindex] != KeyType_TGL_ID)
			&& (KeyFunctionType[SPI_GROUP][Keyindex] != KeyType_MT_ID)
			&& (KeyFunctionType[SPI_GROUP][Keyindex] != KeyType_MACRO_ID))
		{
			/************************************************************************************************************************/
			//Break
			if(Hall_Segment < Segment_Last_SPI[SPI_GROUP][Keyindex])
			{
				/* 有按键按下后,Segment_Last道和Current_key_state更新，抬起的时候抬超过触发值后才更新Segment_Last，没按下的时候，Segment_Last都是之前的 */
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					Key_ActualTrigger_Break(Hall_Segment, SPI_GROUP, Position_Row_TEST, Position_Col_TEST, Keyindex);
				}
				else
				{
					Key_Normal_Break(Hall_Segment, SPI_GROUP, Position_Row_TEST, Position_Col_TEST, Keyindex);
				}
			}
			/************************************************************************************************************************/
		}
		/* 其他功能键 */
		else
		{
			/************************************************************************************************************************/

			//---------------------------------------------------------------------------------------------------------------------------
			/* DKS */
			if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_DKS_ID)
			{
				DKSKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row_TEST, Position_Col_TEST);
			}
			//---------------------------------------------------------------------------------------------------------------------------
			/* TGL */
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_TGL_ID)
			{
				TGLKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row_TEST, Position_Col_TEST);
			}
			//---------------------------------------------------------------------------------------------------------------------------
			/* MT */
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_MT_ID)
			{
				MTKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row_TEST, Position_Col_TEST);
			}
			//---------------------------------------------------------------------------------------------------------------------------
			/* macro */
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_MACRO_ID)
			{
				MACROKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row_TEST, Position_Col_TEST);
			}
			/************************************************************************************************************************/
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: SPI0_Key_Handle_Press
 * Description  	: 按键处理.
 *
 * END ****************************************************************************************************************/
void Key_Normal_Press(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Position_Row, uint8_t Position_Col, uint8_t Keyindex)
{
	/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
	if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex] == TriggerPoint_MAX)
	{
		if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
		{
			Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
			//---------------------------------------------------------------------------------------------------------------------------
			if(Flag_FN_Status || Flag_FN_SPACE_Status)
			{
				KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
				KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
				Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_PRESS;
			}
			else
			{
				KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col];
				KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
				Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_NOTPRESS;
			}
			//usb_echo("KeyFunctionType[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyFunctionType[SPI_GROUP][Keyindex]);
			//usb_echo("KeyCode_Buffer[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyCode_Buffer[SPI_GROUP][Keyindex]);
			//---------------------------------------------------------------------------------------------------------------------------
			if(KeyFunctionType[SPI_GROUP][Keyindex] == 0)
			{
				FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Media_ID)
			{
				MediaKey_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNNormal_ID)
			{
				FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_System_ID)
			{
				FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_ID) || (KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_1_ID))
			{
				FNFunction_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			//---------------------------------------------------------------------------------------------------------------------------
		}
	}
	else
	{	/* 行程设置不到40的情况 */
		//---------------------------------------------------------------------------------------------------------------------------
		/* 普通按鍵功能 */
		if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Press))
		{
			Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
			//---------------------------------------------------------------------------------------------------------------------------
			Position_Row = HallLayout_Position_Table[SPI_GROUP][Keyindex].row;
			Position_Col = HallLayout_Position_Table[SPI_GROUP][Keyindex].col;
			//---------------------------------------------------------------------------------------------------------------------------
			if(Flag_FN_Status || Flag_FN_SPACE_Status)
			{
				KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
				KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
				Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_PRESS;
			}
			else
			{
				KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col];
				KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
				Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_NOTPRESS;
			}
			//usb_echo("KeyFunctionType[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyFunctionType[SPI_GROUP][Keyindex]);
			//usb_echo("KeyCode_Buffer[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyCode_Buffer[SPI_GROUP][Keyindex]);
			//---------------------------------------------------------------------------------------------------------------------------
			if(KeyFunctionType[SPI_GROUP][Keyindex] == 0)
			{
				FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Media_ID)
			{
				MediaKey_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNNormal_ID)
			{
				FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_System_ID)
			{
				FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_ID) || (KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_1_ID))
			{
				FNFunction_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			//---------------------------------------------------------------------------------------------------------------------------
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: SPI0_Key_Handle_Break
 * Description  	: 按键处理.
 *
 * END ****************************************************************************************************************/
void Key_Normal_Break(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Position_Row, uint8_t Position_Col, uint8_t Keyindex)
{
	{
		if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Break))
		{
			Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
			//-----------------------------------------------------------------------------------------------------------------------------
			if(DriverAPP_Nkey_Rollover_Switch)
			{
				if(Current_key_Press_cnt>0)
				{
					//---------------------------------------------------------------------------------------------------------------------------
					Current_key_Press_cnt--;
					//---------------------------------------------------------------------------------------------------------------------------
				}
				HandleDKSKey_Break((g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col]>>8), 1, Keyindex);
				Current_key_state[SPI_GROUP][Keyindex] = 0;
			}
			else
			{
				if(Current_key_Press_cnt>0)
				{
					//---------------------------------------------------------------------------------------------------------------------------
					Current_key_Press_cnt--;
					//---------------------------------------------------------------------------------------------------------------------------
				}
				HandleDKSKey_Break((g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col]>>8), 1, Keyindex);
				Current_key_state[SPI_GROUP][Keyindex] = 0;
			}
			//-----------------------------------------------------------------------------------------------------------------------------
		}
	}

}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: SPI0_Key_ActualTrigger_Press
 * Description  	: 按键处理.
 *
 * END ****************************************************************************************************************/
void Key_ActualTrigger_Press(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Position_Row, uint8_t Position_Col, uint8_t Keyindex)
{
	if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex] >= TriggerPoint_MAX)
	{
		/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
		if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
		{
			if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				if(Flag_FN_Status || Flag_FN_SPACE_Status)
				{
					KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
					KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
					Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_PRESS;
				}
				else
				{
					KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col];
					KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
					Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_NOTPRESS;
				}
				//---------------------------------------------------------------------------------------------------------------------------
				if(KeyFunctionType[SPI_GROUP][Keyindex] == 0)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Media_ID)
				{
					MediaKey_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNNormal_ID)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_System_ID)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_ID) || (KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_1_ID))
				{
					FNFunction_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNtab_ID))
				{
					FNtab_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				//---------------------------------------------------------------------------------------------------------------------------
			}
		}
	}
	else
	{
		/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
		if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
		{
			if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Press))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				if(Flag_FN_Status || Flag_FN_SPACE_Status)
				{
					KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
					KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
					Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_PRESS;
				}
				else
				{
					KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col];
					KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
					Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_NOTPRESS;
				}
//				usb_echo("KeyFunctionType[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyFunctionType[SPI_GROUP][Keyindex]);
//				usb_echo("KeyCode_Buffer[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyCode_Buffer[SPI_GROUP][Keyindex]);
				//---------------------------------------------------------------------------------------------------------------------------
				if(KeyFunctionType[SPI_GROUP][Keyindex] == 0)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Media_ID)
				{
					MediaKey_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNNormal_ID)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_System_ID)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_ID) || (KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_1_ID))
				{
					FNFunction_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNtab_ID))
				{
					FNtab_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				//---------------------------------------------------------------------------------------------------------------------------
			}
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Key_ContinueTrigger_Press
 * Description  	: 按键处理.
 *
 * END ****************************************************************************************************************/
void Key_ContinueTrigger_Press(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Position_Row, uint8_t Position_Col, uint8_t Keyindex)
{
	if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex] >= TriggerPoint_MAX)
	{
		/* 连续快速触发的时候如果已经触发了按键，那所有的行程都能做快速触发 */
		if(Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] ==0)
		{
			/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
			if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
			{
				if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
				{
					Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
					//---------------------------------------------------------------------------------------------------------------------------
					Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
					//---------------------------------------------------------------------------------------------------------------------------
					if(Flag_FN_Status || Flag_FN_SPACE_Status)
					{
						KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
						KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
						Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_PRESS;
					}
					else
					{
						KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col];
						KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
						Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_NOTPRESS;
					}
					//usb_echo("KeyFunctionType[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyFunctionType[SPI_GROUP][Keyindex]);
					//usb_echo("KeyCode_Buffer[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyCode_Buffer[SPI_GROUP][Keyindex]);
					//---------------------------------------------------------------------------------------------------------------------------
					if(KeyFunctionType[SPI_GROUP][Keyindex] == 0)
					{
						FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Media_ID)
					{
						MediaKey_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNNormal_ID)
					{
						FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_System_ID)
					{
						FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_ID) || (KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_1_ID))
					{
						FNFunction_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNtab_ID))
					{
						FNtab_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					//---------------------------------------------------------------------------------------------------------------------------
				}
			}
		}
		/* 连续快速触发的时候如果已经触发了按键，那所有的行程都能做快速触发 */
		else
		{
			if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Press))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
				//---------------------------------------------------------------------------------------------------------------------------
				if(Flag_FN_Status || Flag_FN_SPACE_Status)
				{
					KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
					KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
					Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_PRESS;
				}
				else
				{
					KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col];
					KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
					Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_NOTPRESS;
				}
				//usb_echo("KeyFunctionType[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyFunctionType[SPI_GROUP][Keyindex]);
				//usb_echo("KeyCode_Buffer[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyCode_Buffer[SPI_GROUP][Keyindex]);
				//---------------------------------------------------------------------------------------------------------------------------
				if(KeyFunctionType[SPI_GROUP][Keyindex] == 0)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Media_ID)
				{
					MediaKey_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNNormal_ID)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_System_ID)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_ID) || (KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_1_ID))
				{
					FNFunction_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNtab_ID))
				{
					FNtab_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				//---------------------------------------------------------------------------------------------------------------------------
			}
		}
	}
	else
	{
		/* 连续快速触发的时候如果已经触发了按键，那所有的行程都能做快速触发 */
		if(Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] ==0)
		{
			/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
			if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
			{
				if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Press))
				{
					Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
					//---------------------------------------------------------------------------------------------------------------------------
					Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
					//---------------------------------------------------------------------------------------------------------------------------
					if(Flag_FN_Status || Flag_FN_SPACE_Status)
					{
						KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
						KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
						Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_PRESS;
					}
					else
					{
						KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col];
						KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
						Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_NOTPRESS;
					}
					//usb_echo("KeyFunctionType[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyFunctionType[SPI_GROUP][Keyindex]);
					//usb_echo("KeyCode_Buffer[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyCode_Buffer[SPI_GROUP][Keyindex]);
					//---------------------------------------------------------------------------------------------------------------------------
					if(KeyFunctionType[SPI_GROUP][Keyindex] == 0)
					{
						FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Media_ID)
					{
						MediaKey_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNNormal_ID)
					{
						FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_System_ID)
					{
						FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_ID) || (KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_1_ID))
					{
						FNFunction_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNtab_ID))
					{
						FNtab_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
					}
					//---------------------------------------------------------------------------------------------------------------------------
				}
			}
		}
		/* 连续快速触发的时候如果已经触发了按键，那所有的行程都能做快速触发 */
		else
		{
			if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Press))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
				//---------------------------------------------------------------------------------------------------------------------------
				if(Flag_FN_Status || Flag_FN_SPACE_Status)
				{
					KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
					KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
					Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_PRESS;
				}
				else
				{
					KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col];
					KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
					Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = FNKey_NOTPRESS;
				}
				//usb_echo("KeyFunctionType[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyFunctionType[SPI_GROUP][Keyindex]);
				//usb_echo("KeyCode_Buffer[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyCode_Buffer[SPI_GROUP][Keyindex]);
				//---------------------------------------------------------------------------------------------------------------------------
				if(KeyFunctionType[SPI_GROUP][Keyindex] == 0)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Media_ID)
				{
					MediaKey_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNNormal_ID)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_System_ID)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_ID) || (KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_1_ID))
				{
					FNFunction_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNtab_ID))
				{
					FNtab_Handle(SPI_GROUP, Keyindex, 1, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				//---------------------------------------------------------------------------------------------------------------------------
			}
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Key_ActualTrigger_Break
 * Description  	: 按键处理.
 *
 * END ****************************************************************************************************************/
void Key_ActualTrigger_Break(uint16_t Hall_Segment, uint8_t SPI_GROUP, uint8_t Position_Row, uint8_t Position_Col, uint8_t Keyindex)
{
	//usb_echo("BREAK\n");
	/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
	{
		/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
		if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
		{
			if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Break))
			{
				//-----------------------------------------------------------------------------------------------------------------------------
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				if(Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] == FNKey_NOTPRESS)
				{
					KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col];
					KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
					Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = 0;
				}
				else if(Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] == FNKey_PRESS)
				{
					KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
					KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
				}
				else
				{
					if(Flag_FN_Status || Flag_FN_SPACE_Status)
					{
						KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
						KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
					}
				}
				//usb_echo("KeyFunctionType[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyFunctionType[SPI_GROUP][Keyindex]);
				//usb_echo("KeyCode_Buffer[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyCode_Buffer[SPI_GROUP][Keyindex]);
				//---------------------------------------------------------------------------------------------------------------------------
				if(KeyFunctionType[SPI_GROUP][Keyindex] == 0)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 0, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Media_ID)
				{
					MediaKey_Handle(SPI_GROUP, Keyindex, 0, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNNormal_ID)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 0, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_System_ID)
				{
					FNNormal_Handle(SPI_GROUP, Keyindex, 0, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_ID) || (KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_1_ID))
				{
					FNFunction_Handle(SPI_GROUP, Keyindex, 0, KeyCode_Buffer[SPI_GROUP][Keyindex]);
				}
				//---------------------------------------------------------------------------------------------------------------------------
			}
		}
		/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发,如果太高超过了触发点，那就直接抬起 */
		else if((Hall_Segment <= Hall_Trigger_Adjust_Value_Break) || (Hall_Segment == 0))
		{
			//-----------------------------------------------------------------------------------------------------------------------------
			Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
			//---------------------------------------------------------------------------------------------------------------------------
			if(Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] == FNKey_NOTPRESS)
			{
				KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col];
				KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
				Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] = 0;
			}
			else if(Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex] == FNKey_PRESS)
			{
				KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
				KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
			}
			else
			{
				if(Flag_FN_Status || Flag_FN_SPACE_Status)
				{
					KeyFunctionType[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col];
					KeyCode_Buffer[SPI_GROUP][Keyindex] = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[Position_Row][Position_Col] >> 8;
				}
			}
			//usb_echo("KeyFunctionType[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyFunctionType[SPI_GROUP][Keyindex]);
			//usb_echo("KeyCode_Buffer[%d][%d]:0x%x\n",SPI_GROUP,Keyindex,KeyCode_Buffer[SPI_GROUP][Keyindex]);
			//---------------------------------------------------------------------------------------------------------------------------
			if(KeyFunctionType[SPI_GROUP][Keyindex] == 0)
			{
				FNNormal_Handle(SPI_GROUP, Keyindex, 0, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Media_ID)
			{
				MediaKey_Handle(SPI_GROUP, Keyindex, 0, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_FNNormal_ID)
			{
				FNNormal_Handle(SPI_GROUP, Keyindex, 0, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_System_ID)
			{
				FNNormal_Handle(SPI_GROUP, Keyindex, 0, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			else if((KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_ID) || (KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_Function_1_ID))
			{
				FNFunction_Handle(SPI_GROUP, Keyindex, 0, KeyCode_Buffer[SPI_GROUP][Keyindex]);
			}
			//---------------------------------------------------------------------------------------------------------------------------
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: MediaKey_Handle
 * Description  	: 媒体键处理.
 *
 * END ****************************************************************************************************************/
void MediaKey_Handle(uint8_t SPI_GROUP, uint8_t Keyindex,uint8_t Press_Break, uint8_t KeyCode)
{
	/* 按下 */
	if(Press_Break)
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 这里相当于固定用这个位置来发多媒体键，这样的话按第二个多媒体按键的时候第一个就被替换掉了,目前看11U35磁轴也一样，暂时保存这个做法，后续如果不行再做按键抬起的处理 */
		s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyCode];
		s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyCode]>>8;
		Has_Key_Report_Interface0  = 1;
		Current_key_state[SPI_GROUP][Keyindex] = 1;
		//---------------------------------------------------------------------------------------------------------------------------
		Current_key_state[SPI_GROUP][Keyindex] = 1;
		//---------------------------------------------------------------------------------------------------------------------------
	}
	/* 抬起 */
	else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 做六键无冲 */
		if(DriverAPP_Nkey_Rollover_Switch)
		{
			if(Current_key_Press_cnt>0)
			{
				//---------------------------------------------------------------------------------------------------------------------------
				s_ConsumerBufferIn[1] = 0;
				s_ConsumerBufferIn[2] = 0;
				Has_Key_Report_Interface0 = 1;
				//---------------------------------------------------------------------------------------------------------------------------
				Current_key_state[SPI_GROUP][Keyindex] = 0;
				//---------------------------------------------------------------------------------------------------------------------------
			}
		}
		/* 全键无冲 */
		else
		{
			//---------------------------------------------------------------------------------------------------------------------------
			s_ConsumerBufferIn[1] = 0;
			s_ConsumerBufferIn[2] = 0;
			Has_Key_Report_Interface0 = 1;
			//---------------------------------------------------------------------------------------------------------------------------
			Current_key_state[SPI_GROUP][Keyindex] = 0;
			//---------------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: SPI0_FNNormal_Handle
 * Description  	: FN+普通按键实现普通按键的功能.
 *
 * END ****************************************************************************************************************/
void FNNormal_Handle(uint8_t SPI_GROUP, uint8_t Keyindex, uint8_t Press_Break, uint8_t KeyCode)
{
	/* 按下 */
	if(Press_Break)
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 做六键无冲 */
		if(DriverAPP_Nkey_Rollover_Switch)
		{
			if(Current_key_Press_cnt<6)
			{
				//---------------------------------------------------------------------------------------------------------------------------
				HandleDKSKey_Press(KeyCode, 1, Keyindex);
				Current_key_state[SPI_GROUP][Keyindex] = 1;
				Current_key_Press_cnt++;
				//---------------------------------------------------------------------------------------------------------------------------
			}
		}
		/* 全键无冲 */
		else
		{
			//---------------------------------------------------------------------------------------------------------------------------
			HandleDKSKey_Press(KeyCode, 1, Keyindex);
			Current_key_state[SPI_GROUP][Keyindex] = 1;
			Current_key_Press_cnt++;
			//---------------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
	/* 抬起 */
	else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 做六键无冲 */
		if(DriverAPP_Nkey_Rollover_Switch)
		{
			if(Current_key_Press_cnt>0)
			{
				//---------------------------------------------------------------------------------------------------------------------------
				HandleDKSKey_Break(KeyCode, 1, Keyindex);
				Current_key_state[SPI_GROUP][Keyindex] = 0;
				Current_key_Press_cnt--;
				//---------------------------------------------------------------------------------------------------------------------------
			}
		}
		/* 全键无冲 */
		else
		{
			//---------------------------------------------------------------------------------------------------------------------------
			HandleDKSKey_Break(KeyCode, 1, Keyindex);
			Current_key_state[SPI_GROUP][Keyindex] = 0;
			Current_key_Press_cnt--;
			//---------------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: FNFunction_Handle
 * Description  	: FN+普通按键实现灯光切换等功能.
 *
 * END ****************************************************************************************************************/
void FNFunction_Handle(uint8_t SPI_GROUP, uint8_t Keyindex,uint8_t Press_Break, uint8_t KeyCode)
{
	/* 按下 */
	if(Press_Break)
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 做六键无冲 */
		if(DriverAPP_Nkey_Rollover_Switch)
		{
			if(Current_key_Press_cnt<6)
			{
				//FN键
				if(KeyCode == 0xFC)
				{
					if(!Flag_FN_Status)
					{
						Flag_FN_Status = 1;
					}
					if(Flag_FN_SPACE_Status)
					{
						Flag_FN_SPACE_Status = 0;
						Flag_LedMode_ModeInit = 1;
						//------------------------------------------------------------------------------------------
						Flag_LedMode_RGBUpdate |= (LED_Update_Clear_Highlight);
						//------------------------------------------------------------------------------------------
					}
				}
				else
				{
					//---------------------------------------------------------------------------------------------------------------------------
					FNFunction_Table(KeyCode);
					//---------------------------------------------------------------------------------------------------------------------------
				}
				//---------------------------------------------------------------------------------------------------------------------------
				Current_key_state[SPI_GROUP][Keyindex] = 1;
				Current_key_Press_cnt++;
				//---------------------------------------------------------------------------------------------------------------------------
			}
		}
		else
		{
			//FN键
			if(KeyCode == 0xFC)
			{
				if(!Flag_FN_Status)
				{
					Flag_FN_Status = 1;
				}
				if(Flag_FN_SPACE_Status)
				{
					Flag_FN_SPACE_Status = 0;
					Flag_LedMode_ModeInit = 1;
					//------------------------------------------------------------------------------------------
					Flag_LedMode_RGBUpdate |= (LED_Update_Clear_Highlight);
					//------------------------------------------------------------------------------------------
				}
			}
			else
			{
				//---------------------------------------------------------------------------------------------------------------------------
				FNFunction_Table(KeyCode);
				//---------------------------------------------------------------------------------------------------------------------------
			}
			//---------------------------------------------------------------------------------------------------------------------------
			Current_key_state[SPI_GROUP][Keyindex] = 1;
			Current_key_Press_cnt++;
			//---------------------------------------------------------------------------------------------------------------------------
		}
	}
	/* 抬起 */
	else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 做六键无冲 */
		if(DriverAPP_Nkey_Rollover_Switch)
		{
			if(Current_key_Press_cnt>0)
			{
				//---------------------------------------------------------------------------------------------------------------------------
				//FN键
				if(KeyCode == 0xFC)
				{
					if(Flag_FN_Status)
					{
						Flag_FN_Status = 0;
					}
					if(Flag_FN_Status == 0)
					{
						Flag_FN_ESC_Status = 0;
						Tick_FN_ESC_ResetDefault = 0;
					}
				}
				else
				{
					//---------------------------------------------------------------------------------------------------------------------------
					if(Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex])
					{
						HandleDKSKey_Break(KeyCode, 1, Keyindex);
					}
					//---------------------------------------------------------------------------------------------------------------------------
				}
				//---------------------------------------------------------------------------------------------------------------------------
				Current_key_state[SPI_GROUP][Keyindex] = 0;
				Current_key_Press_cnt--;
				//---------------------------------------------------------------------------------------------------------------------------
			}
		}
		/* 全键无冲 */
		else
		{
			//---------------------------------------------------------------------------------------------------------------------------
			//FN键
			if(KeyCode == 0xFC)
			{
				if(Flag_FN_Status)
				{
					Flag_FN_Status = 0;
				}
				if(Flag_FN_Status == 0)
				{
					Flag_FN_ESC_Status = 0;
					Tick_FN_ESC_ResetDefault = 0;
				}
			}
			else
			{
				//---------------------------------------------------------------------------------------------------------------------------
				if(Flag_KeyPressWithoutFN[SPI_GROUP][Keyindex])
				{
					HandleDKSKey_Break(KeyCode, 1, Keyindex);
				}
				//---------------------------------------------------------------------------------------------------------------------------
			}
			//---------------------------------------------------------------------------------------------------------------------------
			Current_key_state[SPI_GROUP][Keyindex] = 0;
			Current_key_Press_cnt--;
			//---------------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: FNFunction_Handle
 * Description  	: FN+普通按键实现灯光切换等功能.
 *
 * END ****************************************************************************************************************/
void FNtab_Handle(uint8_t SPI_GROUP, uint8_t Keyindex,uint8_t Press_Break, uint8_t KeyCode)
{
	/* 按下 */
	if(Press_Break)
	{
		Flag_FNMode_Init = 1;
		Flag_FNAndTAB_Press = 1;
		device_mode = MODE_FNTAB;
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: SPI0_DKSKey_Press
 * Description  	: 按键处理.
 *
 * END ****************************************************************************************************************/
void DKSKey_Handle(uint8_t SPI_GROUP, uint16_t Hall_Segment, uint8_t Keyindex, uint8_t Position_Row, uint8_t Position_Col)
{
	uint8_t KeyFunctionIndex = 0;
	uint8_t KeyFunction_DKS_ID = 0;
	uint8_t KeyFunction_DKS_Status_Low = 0;
	uint8_t KeyFunction_DKS_Status_High = 0;
	uint8_t i = 0;
	uint8_t j = 0;
	/************************************************************************************************************************/
	/* DKS */
	/* 按下 */
	//usb_echo("Hall_Segment:%x.\n",Hall_Segment);
	if(Hall_Segment > Segment_Last_SPI[SPI_GROUP][Keyindex])
	{
		Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
		/* 按下的时候先到36再到触发点 */
		if((g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]) > 36)
		{
			/* 处理按下到36这一段 */
			if(((Hall_Segment) >= (36)) && ((Hall_Segment) < (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex])))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				/* 不考虑全键无冲/不考虑FN键是否按 */
				//---------------------------------------------------------------------------------------------------------------------------
				KeyFunction_DKS_ID = (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00)>>8;
				//---------------------------------------------------------------------------------------------------------------------------
				/* 依次查四个状态的按键 */
				for(i=0;i<4;i++)
				{
					//---------------------------------------------------------------------------------------------------------------------------
					/* 如果是一个DKS键，首先把键的状态拿出来 */
					Flag_DKS_MODE[KeyFunction_DKS_ID][i]= (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+1] | (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+2]<<8));
					//---------------------------------------------------------------------------------------------------------------------------
					/* 看四个按键有没有都设置有按键 */
					if(Flag_DKS_MODE[KeyFunction_DKS_ID][i] != 0)
					{
						/* 一个新的DKS开始 */
						if(Flag_DKSKey_HasDone[KeyFunction_DKS_ID][i] == 0)
						{
							/* 看状态3(36按下)有没有放按键 */
							if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0008) == 0x0008)
							{
								/* 每一个按键和每一个按键分别四个触发点的状态 */
								if(DKSKey_State[KeyFunction_DKS_ID][i][0] == 0)
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
											s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
											s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
											Has_Key_Report_Interface0  = 1;
											/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
											DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
											DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
											DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
								else
								{
									if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												//想一下媒体键要不要发抬起
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
												DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
												Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
												DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
												Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
								}
							}
							//---------------------------------------------------------------------------------------------------------------------------
							/* 00 03 表示36按下，按到触发点抬起 */
							else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00FF) == 0x0003)
							{
								/* 长按的状态按下后就不再进来了，等下一个触发点再抬起*/
								if(DKSKey_State[KeyFunction_DKS_ID][i] == 0)
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//---------------------------------------------------------------------------------------------------------------------------
											KeyFunctionIndex = g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col]>>8;
											s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
											s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
											Has_Key_Report_Interface0  = 1;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 记录DKS键按没按下，按下在抬起那里补抬起 */
											DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											/* 记录DKS键按没按下 */
											DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											/* 记录DKS键按没按下 */
											DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
							//-----------------------------------------------------------------------------------------------------------------------------------
							/* 00 10  表示36按下，抬起到36才抬起 */
							else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0010) == 0x0010)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							//-----------------------------------------------------------------------------------------------------------------------------------
							/* 00 F0  表示36按下，抬起到触发点才抬起 */
							else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00F0) == 0x00F0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							//-----------------------------------------------------------------------------------------------------------------------------------
						}
					}
				}
			}
			/* 处理触发点这一段 */
			else if((Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				/* 不考虑全键无冲/不考虑FN键是否按 */
				//---------------------------------------------------------------------------------------------------------------------------
				KeyFunction_DKS_ID = (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00)>>8;
				//---------------------------------------------------------------------------------------------------------------------------
				for(i=0;i<4;i++)
				{
					//---------------------------------------------------------------------------------------------------------------------------
					/* 如果是一个DKS键，首先把键的状态拿出来 */
					Flag_DKS_MODE[KeyFunction_DKS_ID][i] = (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+1] | (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+2]<<8));
					//---------------------------------------------------------------------------------------------------------------------------
					/* 看四个按键有没有都设置有按键 */
					if(Flag_DKS_MODE[KeyFunction_DKS_ID][i] != 0)
					{
						/* 做完这个状态就不做了，等按键完全抬起后再重新按下的时候再做 */
						if(Flag_DKSKey_HasDone[KeyFunction_DKS_ID][i] == 0)
						{
							//-----------------------------------------------------------------------------------------------------------------------------------
							/* 这些状态的先后顺序是有关系的，因为他们有包含的关系，比如0x00FF和0x0001,满足0x00FF一定满足0x0001，所以先看0x00FF*/
							/* 00 FF  表示触发点按下，抬起到触发点才抬起 */
							if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00FF) == 0x00FF)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							//---------------------------------------------------------------------------------------------------------------------------
							/* 00 1F  表示触发点按下，抬起到36才抬起 */
							else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x001F) == 0x001F)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							//---------------------------------------------------------------------------------------------------------------------------
							/* 00 03 表示36按下，按到触发点抬起,所以触发点这里发抬起 */
							else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0003) == 0x0003)
							{
								if(Flag_DKSKey_HasDone[KeyFunction_DKS_ID][i] == 0)
								{
									/* 没发码就发码---这里要判断一下前面的36有没有按下，有可能太快了一下子分段值就直接越过36那一段了，没有的话，得先按下，然后再补抬起 */
									if(DKSKey_State[KeyFunction_DKS_ID][i][0] == 0)
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
												Has_Key_Report_Interface0  = 1;
												/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
												DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
												DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
												Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
												/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
												DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
												DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
												Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
												/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
												DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
												DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
												Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
									/* 发了码就补抬起 */
									else
									{

										if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 不考虑全键无冲/不考虑FN键是否按 */
											//---------------------------------------------------------------------------------------------------------------------------
											switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
											{
												//---------------------------------------------------------------------------------------------------------------------------
												/* 媒体键 */
												case KeyType_Media_ID:
													//想一下媒体键要不要发抬起
													break;
												//---------------------------------------------------------------------------------------------------------------------------
												/* 功能键、模式切换等 */
												case KeyType_Function_ID:
													break;
												//---------------------------------------------------------------------------------------------------------------------------
												/* Ctrl等、普通按键 */
												case KeyType_System_ID:
													HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
													DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
													Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
													break;
												case KeyType_Normal_ID:
													HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
													DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
													Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
													break;
												//---------------------------------------------------------------------------------------------------------------------------
												default:
													break;
											}
										}
									}
								}
							}
							//---------------------------------------------------------------------------------------------------------------------------
							/* 看状态1(40按下)有没有放按键 */
							else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0001) == 0x0001)
							{
								/* 没发码就发码 */
								if(DKSKey_State[KeyFunction_DKS_ID][i][1] == 0)
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
											s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
											s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
											Has_Key_Report_Interface0  = 1;
											/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
											DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
											DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
											DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
								/* 发了码就补抬起 */
								else
								{
									if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												//想一下媒体键要不要发抬起
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
												DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
												Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
												DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
												Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
								}
							}
							//-----------------------------------------------------------------------------------------------------------------------------------
						}
					}
				}
			}
		}
		/* 先按到触发点，再按到36 */
		else
		{
			if(((Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex])) && ((Hall_Segment) < (36)))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				/* 不考虑全键无冲/不考虑FN键是否按 */
				//---------------------------------------------------------------------------------------------------------------------------
				KeyFunction_DKS_ID = (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00)>>8;
				//---------------------------------------------------------------------------------------------------------------------------
				for(i=0;i<4;i++)
				{
					//---------------------------------------------------------------------------------------------------------------------------
					/* 如果是一个DKS键，首先把键的状态拿出来 */
					Flag_DKS_MODE[KeyFunction_DKS_ID][i]= (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+1] | (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+2]<<8));
					//usb_echo("Flag_DKS_MODE[%d][%d]:%x.\n",KeyFunction_DKS_ID,i,Flag_DKS_MODE[KeyFunction_DKS_ID][i]);
					//---------------------------------------------------------------------------------------------------------------------------
					/* 做完这个状态就不做了，等按键完全抬起后再重新按下的时候再做 */
					if(Flag_DKSKey_HasDone[KeyFunction_DKS_ID][i] == 0)
					{
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 FF  表示触发点按下，抬起到触发点才抬起 */
						if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00FF) == 0x00FF)
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 媒体键 */
								case KeyType_Media_ID:
									KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
									s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
									s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
									Has_Key_Report_Interface0  = 1;
									/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
									DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 功能键、模式切换等 */
								case KeyType_Function_ID:
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* Ctrl等、普通按键 */
								case KeyType_System_ID:
									HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
									/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
									DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									break;
								case KeyType_Normal_ID:
									HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
									/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
									DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								default:
									break;
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 1F  表示触发点按下，抬起到36才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x001F) == 0x001F)
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 媒体键 */
								case KeyType_Media_ID:
									KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
									s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
									s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
									Has_Key_Report_Interface0  = 1;
									/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
									DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 功能键、模式切换等 */
								case KeyType_Function_ID:
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* Ctrl等、普通按键 */
								case KeyType_System_ID:
									HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
									/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
									DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									break;
								case KeyType_Normal_ID:
									HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
									/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
									DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								default:
									break;
							}
						}
						//---------------------------------------------------------------------------------------------------------------------------
						/* 00 03 表示触发点按下，按到36抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0003) == 0x0003)
						{
							/* 没发码就发码---这里要判断一下前面的36有没有按下，有可能太快了一下子分段值就直接越过36那一段了，没有的话，得先按下，然后再补抬起 */
							if(DKSKey_State[KeyFunction_DKS_ID][i][0] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
						/* 看状态1(触发点按下)有没有放按键 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0001) == 0x0001)
						{
							/* 没发码就发码 */
							if(DKSKey_State[KeyFunction_DKS_ID][i][0] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 发了码就补抬起 */
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
								{
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
					}
				}
			}
			else if((Hall_Segment) >= (36))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				/* 不考虑全键无冲/不考虑FN键是否按 */
				//---------------------------------------------------------------------------------------------------------------------------
				KeyFunction_DKS_ID = (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00)>>8;
				//---------------------------------------------------------------------------------------------------------------------------
				for(i=0;i<4;i++)
				{
					//---------------------------------------------------------------------------------------------------------------------------
					/* 如果是一个DKS键，首先把键的状态拿出来 */
					Flag_DKS_MODE[KeyFunction_DKS_ID][i]= (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+1] | (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+2]<<8));
					//---------------------------------------------------------------------------------------------------------------------------
					/* 这个DKS键的这个状态有没有做 */
					if(Flag_DKSKey_HasDone[KeyFunction_DKS_ID][i] == 0)
					{
						/* 看状态3(36按下)有没有放按键 */
						if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0008) == 0x0008)
						{
							if(DKSKey_State[KeyFunction_DKS_ID][i][1] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
								{
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
						//---------------------------------------------------------------------------------------------------------------------------
						/* 00 03 表示触发点按下，按到36抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00FF) == 0x0003)
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							/* 防止太快按的时候分段直接大于36了,那之前触发点到36这一段就没有按按键 */
							if(DKSKey_State[KeyFunction_DKS_ID][i][0] == 0)
							{
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								//if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
								{
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 F0  表示36按下，抬起到触发点才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00F0) == 0x00F0)
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if(DKSKey_State[KeyFunction_DKS_ID][i][1] == 0)
							{
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 10  表示36按下，抬起到36才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0010) == 0x0010)
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if(DKSKey_State[KeyFunction_DKS_ID][i][1] == 0)
							{
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
					}
				}
			}
		}
	}
	/* 抬起 */
	else if(Hall_Segment < Segment_Last_SPI[SPI_GROUP][Keyindex])
	{
		Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
		/* 抬起的时候到36再到触发点 */
		if((g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]) < 36)
		{
			/* 处理抬起到36这一段 */
			if(((Hall_Segment) <= (36)) && ((Hall_Segment) > (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex])))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				/* 不考虑全键无冲/不考虑FN键是否按 */
				//---------------------------------------------------------------------------------------------------------------------------
				KeyFunction_DKS_ID = (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00)>>8;
				//---------------------------------------------------------------------------------------------------------------------------
				for(i=0;i<4;i++)
				{
					//---------------------------------------------------------------------------------------------------------------------------
					/* 如果是一个DKS键，首先把键的状态拿出来 */
					Flag_DKS_MODE[KeyFunction_DKS_ID][i]= (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+1] | (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+2]<<8));
					//---------------------------------------------------------------------------------------------------------------------------
					/* 这个DKS键的这个状态有没有做 */
					if(Flag_DKSKey_HasDone[KeyFunction_DKS_ID][i] == 0)
					{
						//---------------------------------------------------------------------------------------------------------------------------
						/* 00 1F 表示触发点按下，抬起到36才抬起 */
						if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00FF) == 0x001F)
						{
							if(DKSKey_State[KeyFunction_DKS_ID][i][0] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 10  表示36按下，抬起到36才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0010) == 0x0010)
						{
							if(DKSKey_State[KeyFunction_DKS_ID][i][1] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 C0  表示抬起到36发按下，抬起到触发点才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00C0) == 0x00C0)
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 媒体键 */
								case KeyType_Media_ID:
									KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
									s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
									s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
									Has_Key_Report_Interface0  = 1;
									/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
									DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 功能键、模式切换等 */
								case KeyType_Function_ID:
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* Ctrl等、普通按键 */
								case KeyType_System_ID:
									HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
									/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
									DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									break;
								case KeyType_Normal_ID:
									HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
									/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
									DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								default:
									break;
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 看状态5(抬起36)有没有放按键 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0040) == 0x0040)
						{
							if(DKSKey_State[KeyFunction_DKS_ID][i][2] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
								{
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
						//---------------------------------------------------------------------------------------------------------------------------
					}
				}
			}
			/* 处理触发点这一段 */
			else if((Hall_Segment) <= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				/* 不考虑全键无冲/不考虑FN键是否按 */
				//---------------------------------------------------------------------------------------------------------------------------
				KeyFunction_DKS_ID = (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00)>>8;
				//---------------------------------------------------------------------------------------------------------------------------
				for(i=0;i<4;i++)
				{
					//---------------------------------------------------------------------------------------------------------------------------
					/* 如果是一个DKS键，首先把键的状态拿出来 */
					Flag_DKS_MODE[KeyFunction_DKS_ID][i]= (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+1] | (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+2]<<8));
					//---------------------------------------------------------------------------------------------------------------------------
					/* 做完这个状态就不做了，等按键完全抬起后再重新按下的时候再做 */
					if(Flag_DKSKey_HasDone[KeyFunction_DKS_ID][i] == 0)
					{
						/* 02 00 状态7选中	对应抬起到触发点发按键 */
						if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0200) == 0x0200)
						{
							/* 没发码就发码 */
							if(DKSKey_State[KeyFunction_DKS_ID][i][3] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 发了码就补抬起 */
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
								{
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
						//---------------------------------------------------------------------------------------------------------------------------
						/* 00 FF  表示触发点按下，抬起到触发点才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00FF) == 0x00FF)
						{
							/* 没发码就发码 */
							if(DKSKey_State[KeyFunction_DKS_ID][i][0] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 发了码就补抬起 */
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
								{
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 F0  表示36按下，抬起到触发点才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00FF) == 0x00FF)
						{
							/* 没发码就发码 */
							if(DKSKey_State[KeyFunction_DKS_ID][i][1] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 发了码就补抬起 */
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
								{
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 C0  表示抬起到36按下，抬起到触发点才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00C0) == 0x00C0)
						{
							/* 没发码就发码 */
							if(DKSKey_State[KeyFunction_DKS_ID][i][2] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 发了码就补抬起 */
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
								{
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
					}
				}
			}
		}
		/* 先抬到触发点再到36 */
		else
		{
			/* 处理抬起到触发点这一段 */
			if(((Hall_Segment) <= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex])) && ((Hall_Segment) > (36)))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				/* 不考虑全键无冲/不考虑FN键是否按 */
				//---------------------------------------------------------------------------------------------------------------------------
				KeyFunction_DKS_ID = (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00)>>8;
				//---------------------------------------------------------------------------------------------------------------------------
				for(i=0;i<4;i++)
				{
					//---------------------------------------------------------------------------------------------------------------------------
					/* 如果是一个DKS键，首先把键的状态拿出来 */
					Flag_DKS_MODE[KeyFunction_DKS_ID][i]= (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+1] | (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+2]<<8));
					//---------------------------------------------------------------------------------------------------------------------------
					/* 做完这个状态就不做了，等按键完全抬起后再重新按下的时候再做 */
					if(Flag_DKSKey_HasDone[KeyFunction_DKS_ID][i] == 0)
					{
						/* 02 00 状态7选中	对应抬起到触发点发按键 */
						if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0200) == 0x0200)
						{
							/* 没发码就发码 */
							if(DKSKey_State[KeyFunction_DKS_ID][i][2] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][2] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 发了码就补抬起 */
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
								{
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
						//---------------------------------------------------------------------------------------------------------------------------
						/* 00 FF  表示触发点按下，抬起到触发点才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00FF) == 0x00FF)
						{
							/* 没发码就发码 */
							if(DKSKey_State[KeyFunction_DKS_ID][i][1] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 发了码就补抬起 */
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 F0  表示36按下，抬起到触发点才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00F0) == 0x00F0)
						{
							/* 没发码就发码 */
							if(DKSKey_State[KeyFunction_DKS_ID][i][0] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][0] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 发了码就补抬起 */
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 C0 表示抬起到36按下，按到触发点抬起,所以触发点这里发抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00C0) == 0x00C0)
						{
							/* 没发码就发码---这里要判断一下前面的36有没有按下，有可能太快了一下子分段值就直接越过36那一段了，没有的话，得先按下，然后再补抬起 */
							if(DKSKey_State[KeyFunction_DKS_ID][i][3] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 发了码就补抬起 */
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
					}
				}
			}
			else if((Hall_Segment) <= (36))
			{
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
				//---------------------------------------------------------------------------------------------------------------------------
				/* 不考虑全键无冲/不考虑FN键是否按 */
				//---------------------------------------------------------------------------------------------------------------------------
				KeyFunction_DKS_ID = (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00)>>8;
				//---------------------------------------------------------------------------------------------------------------------------
				for(i=0;i<4;i++)
				{
					//---------------------------------------------------------------------------------------------------------------------------
					/* 如果是一个DKS键，首先把键的状态拿出来 */
					Flag_DKS_MODE[KeyFunction_DKS_ID][i]= (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+1] | (g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+2]<<8));
					//---------------------------------------------------------------------------------------------------------------------------
					/* 这个DKS键的这个状态有没有做 */
					if(Flag_DKSKey_HasDone[KeyFunction_DKS_ID][i] == 0)
					{
						//---------------------------------------------------------------------------------------------------------------------------
						/* 00 1F 表示触发点按下，抬起到36才抬起 */
						if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x001F) == 0x001F)
						{
							if(DKSKey_State[KeyFunction_DKS_ID][i][1] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 10  表示36按下，抬起到36才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0010) == 0x0010)
						{
							if(DKSKey_State[KeyFunction_DKS_ID][i][0] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][1] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 00 C0  表示抬起到36发按下，抬起到触发点才抬起 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x00C0) == 0x00C0)
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if(DKSKey_State[KeyFunction_DKS_ID][i][3] == 0)
							{
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
						//-----------------------------------------------------------------------------------------------------------------------------------
						/* 看状态5(抬起36)有没有放按键 */
						else if((Flag_DKS_MODE[KeyFunction_DKS_ID][i] & 0x0040) == 0x0040)
						{
							if(DKSKey_State[KeyFunction_DKS_ID][i][3] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										KeyFunctionIndex = g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3];
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[KeyFunctionIndex]>>8;
										Has_Key_Report_Interface0  = 1;
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
										/* 记录DKS键的触发点有没有按下，按下在抬起那里补抬起 */
										DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 1;
										DKSKey_State[KeyFunction_DKS_ID][i][3] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
								{
									switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
											DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	/* 分段值相等的情况，用来做各种抬起 */
	else
	{
		Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
		//---------------------------------------------------------------------------------------------------------------------------
		/* 不考虑全键无冲/不考虑FN键是否按 */
		//---------------------------------------------------------------------------------------------------------------------------
		KeyFunction_DKS_ID = (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00)>>8;
		//---------------------------------------------------------------------------------------------------------------------------
		if(Hall_Segment)
		{
			for(i=0;i<4;i++)
			{
				//---------------------------------------------------------------------------------------------------------------------------
				if(DKSKey_NeedBreak[KeyFunction_DKS_ID][i])
				{
					if(Flag_DKS_MODE[KeyFunction_DKS_ID][i])
					{
						//---------------------------------------------------------------------------------------------------------------------------
						/* 不考虑全键无冲/不考虑FN键是否按 */
						//---------------------------------------------------------------------------------------------------------------------------
						switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 媒体键 */
							case KeyType_Media_ID:
								//想一下媒体键要不要发抬起
								break;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 功能键、模式切换等 */
							case KeyType_Function_ID:
								break;
							//---------------------------------------------------------------------------------------------------------------------------
							/* Ctrl等、普通按键 */
							case KeyType_System_ID:
								HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
								DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
								break;
							case KeyType_Normal_ID:
								HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
								DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
								break;
							//---------------------------------------------------------------------------------------------------------------------------
							default:
								break;
						}
					}
				}
			}
		}
		/* 抬起到0是补抬起和清参数准备从新开始 */
		else
		{
			for(i=0;i<4;i++)
			{
				for(j=0;j<4;j++)
				{
					//---------------------------------------------------------------------------------------------------------------------------
					if(DKSKey_State[KeyFunction_DKS_ID][i][j])
					{
						if(Flag_DKS_MODE[KeyFunction_DKS_ID][i])
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							switch(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+4])
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 媒体键 */
								case KeyType_Media_ID:
									//想一下媒体键要不要发抬起
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 功能键、模式切换等 */
								case KeyType_Function_ID:
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* Ctrl等、普通按键 */
								case KeyType_System_ID:
									HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
									DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
									break;
								case KeyType_Normal_ID:
									HandleDKSKey_Break(g_DrivetData_DKS.DKS_Buffer[KeyFunction_DKS_ID*17+i*4+3], KeyFunction_DKS_ID, i);
									DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								default:
									break;
							}
						}
					}
				}
				DKSKey_NeedBreak[KeyFunction_DKS_ID][i] = 0;
				DKSKey_State[KeyFunction_DKS_ID][i][0] = 0;
				DKSKey_State[KeyFunction_DKS_ID][i][1] = 0;
				DKSKey_State[KeyFunction_DKS_ID][i][2] = 0;
				DKSKey_State[KeyFunction_DKS_ID][i][3] = 0;
				Flag_DKSKey_HasDone[KeyFunction_DKS_ID][i] = 0;
				Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
			}
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TGLKey_Handle
 * Description  	: tgl处理.
 *
 * END ****************************************************************************************************************/
void TGLKey_Handle(uint8_t SPI_GROUP, uint16_t Hall_Segment, uint8_t Keyindex, uint8_t Position_Row, uint8_t Position_Col)
{
	uint8_t KeyFunctionType = 0;
	uint8_t KeyFunctionIndex = 0;
	uint8_t TGLKey_id = 0;
	/* 按下 */
	if(Hall_Segment > Segment_Last_SPI[SPI_GROUP][Keyindex])
	{
		if(!Current_key_state[SPI_GROUP][Keyindex])
		{
			/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex] == TriggerPoint_MAX)
			{
				/* 快速触发 */
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
					if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							/* 短按第一次是按下，第二次是抬起 */
							if(Flag_TGLKey_NeedBreak[TGLKey_id] == 0)
							{
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]]>>8;
										Has_Key_Report_Interface0  = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										/* 切灯效 */
										if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x01)
										{
											LedMode_Change();
											Flag_LedMode_ModeInit = 1;
										}
										/* 亮度加 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x04)
										{
											LedVolume_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* 亮度减 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x05)
										{
											LedVolume_Dec();
											Flag_LedMode_ModeInit = 1;
										}
										/* 速度加 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x06)
										{
											LedSpeed_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* 速度减 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x07)
										{
											LedSpeed_Dec();
											Flag_LedMode_ModeInit = 1;
										}
										/* 颜色切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x09)
										{
											LedColorIndex_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* mac切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x0F)
										{
											g_variableData.DriverAPP_WinMacMode = 1;
											DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
										}
										/* win切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x10)
										{
											g_variableData.DriverAPP_WinMacMode = 0;
											DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
										}
										/* win lock */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x11)
										{
											g_variableData.DriverAPP_WinLock_Switch = 1;
											DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
										}
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 短按第一次是按下，第二次是抬起 */
							else
							{
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										s_ConsumerBufferIn[1] = 0;
										s_ConsumerBufferIn[2] = 0;
										Has_Key_Report_Interface0 = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										/* 短按第一次是按下，第二次是抬起 */
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
					}
				}
				/* 快速连续触发 */
				else if(g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 连续快速触发的时候如果没有触发，要等到到触发值以后再做rapid trigger */
					if(Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] ==0)
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
							{
								Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
								//---------------------------------------------------------------------------------------------------------------------------
								Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
								//---------------------------------------------------------------------------------------------------------------------------
								TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								/* 短按第一次是按下，第二次是抬起 */
								if(Flag_TGLKey_NeedBreak[TGLKey_id] == 0)
								{
									switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]];
											s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]]>>8;
											Has_Key_Report_Interface0  = 1;
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											/* 切灯效 */
											if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x01)
											{
												LedMode_Change();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度加 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x04)
											{
												LedVolume_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度减 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x05)
											{
												LedVolume_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度加 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x06)
											{
												LedSpeed_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度减 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x07)
											{
												LedSpeed_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 颜色切换 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x09)
											{
												LedColorIndex_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* mac切换 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x0F)
											{
												g_variableData.DriverAPP_WinMacMode = 1;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win切换 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x10)
											{
												g_variableData.DriverAPP_WinMacMode = 0;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win lock */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x11)
											{
												g_variableData.DriverAPP_WinLock_Switch = 1;
												DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
											}
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
								/* 短按第一次是按下，第二次是抬起 */
								else
								{
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											s_ConsumerBufferIn[1] = 0;
											s_ConsumerBufferIn[2] = 0;
											Has_Key_Report_Interface0 = 1;
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											/* 短按第一次是按下，第二次是抬起 */
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
					}
					/* 连续快速触发的时候已经触发过了，那就所有的行程都做rapid trigger */
					else
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
							//---------------------------------------------------------------------------------------------------------------------------
							TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							/* 短按第一次是按下，第二次是抬起 */
							if(Flag_TGLKey_NeedBreak[TGLKey_id] == 0)
							{
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]]>>8;
										Has_Key_Report_Interface0  = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										/* 切灯效 */
										if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x01)
										{
											LedMode_Change();
											Flag_LedMode_ModeInit = 1;
										}
										/* 亮度加 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x04)
										{
											LedVolume_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* 亮度减 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x05)
										{
											LedVolume_Dec();
											Flag_LedMode_ModeInit = 1;
										}
										/* 速度加 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x06)
										{
											LedSpeed_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* 速度减 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x07)
										{
											LedSpeed_Dec();
											Flag_LedMode_ModeInit = 1;
										}
										/* 颜色切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x09)
										{
											LedColorIndex_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* mac切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x0F)
										{
											g_variableData.DriverAPP_WinMacMode = 1;
											DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
										}
										/* win切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x10)
										{
											g_variableData.DriverAPP_WinMacMode = 0;
											DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
										}
										/* win lock */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x11)
										{
											g_variableData.DriverAPP_WinLock_Switch = 1;
											DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
										}
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 短按第一次是按下，第二次是抬起 */
							else
							{
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										s_ConsumerBufferIn[1] = 0;
										s_ConsumerBufferIn[2] = 0;
										Has_Key_Report_Interface0 = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										/* 短按第一次是按下，第二次是抬起 */
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
					}
				}
				/* 普通模式 */
				else
				{
					if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//---------------------------------------------------------------------------------------------------------------------------
						TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						//---------------------------------------------------------------------------------------------------------------------------
						/* 不考虑全键无冲/不考虑FN键是否按 */
						//---------------------------------------------------------------------------------------------------------------------------
						/* 短按第一次是按下，第二次是抬起 */
						if(Flag_TGLKey_NeedBreak[TGLKey_id] == 0)
						{
							switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 媒体键 */
								case KeyType_Media_ID:
									s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]];
									s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]]>>8;
									Has_Key_Report_Interface0  = 1;
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 功能键、模式切换等 */
								case KeyType_Function_ID:
									/* 切灯效 */
									if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x01)
									{
										LedMode_Change();
										Flag_LedMode_ModeInit = 1;
									}
									/* 亮度加 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x04)
									{
										LedVolume_Inc();
										Flag_LedMode_ModeInit = 1;
									}
									/* 亮度减 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x05)
									{
										LedVolume_Dec();
										Flag_LedMode_ModeInit = 1;
									}
									/* 速度加 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x06)
									{
										LedSpeed_Inc();
										Flag_LedMode_ModeInit = 1;
									}
									/* 速度减 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x07)
									{
										LedSpeed_Dec();
										Flag_LedMode_ModeInit = 1;
									}
									/* 颜色切换 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x09)
									{
										LedColorIndex_Inc();
										Flag_LedMode_ModeInit = 1;
									}
									/* mac切换 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x0F)
									{
										g_variableData.DriverAPP_WinMacMode = 1;
										DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
									}
									/* win切换 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x10)
									{
										g_variableData.DriverAPP_WinMacMode = 0;
										DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
									}
									/* win lock */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x11)
									{
										g_variableData.DriverAPP_WinLock_Switch = 1;
										DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
									}
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* Ctrl等、普通按键 */
								case KeyType_System_ID:
									HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
									break;
								case KeyType_Normal_ID:
									HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								default:
									break;
							}
						}
						/* 短按第一次是按下，第二次是抬起 */
						else
						{
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 媒体键 */
								case KeyType_Media_ID:
									//想一下媒体键要不要发抬起
									s_ConsumerBufferIn[1] = 0;
									s_ConsumerBufferIn[2] = 0;
									Has_Key_Report_Interface0 = 1;
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									/* 短按第一次是按下，第二次是抬起 */
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 功能键、模式切换等 */
								case KeyType_Function_ID:
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* Ctrl等、普通按键 */
								case KeyType_System_ID:
									HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
									/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									break;
								case KeyType_Normal_ID:
									HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
									/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								default:
									break;
							}
						}
					}
				}
			}
			/* 触发值不是40的情况 */
			else
			{
				/* 快速触发 */
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
					if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							/* 短按第一次是按下，第二次是抬起 */
							if(Flag_TGLKey_NeedBreak[TGLKey_id] == 0)
							{
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]]>>8;
										Has_Key_Report_Interface0  = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										/* 切灯效 */
										if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x01)
										{
											LedMode_Change();
											Flag_LedMode_ModeInit = 1;
										}
										/* 亮度加 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x04)
										{
											LedVolume_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* 亮度减 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x05)
										{
											LedVolume_Dec();
											Flag_LedMode_ModeInit = 1;
										}
										/* 速度加 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x06)
										{
											LedSpeed_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* 速度减 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x07)
										{
											LedSpeed_Dec();
											Flag_LedMode_ModeInit = 1;
										}
										/* 颜色切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x09)
										{
											LedColorIndex_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* mac切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x0F)
										{
											g_variableData.DriverAPP_WinMacMode = 1;
											DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
										}
										/* win切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x10)
										{
											g_variableData.DriverAPP_WinMacMode = 0;
											DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
										}
										/* win lock */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x11)
										{
											g_variableData.DriverAPP_WinLock_Switch = 1;
											DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
										}
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 短按第一次是按下，第二次是抬起 */
							else
							{
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										s_ConsumerBufferIn[1] = 0;
										s_ConsumerBufferIn[2] = 0;
										Has_Key_Report_Interface0 = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										/* 短按第一次是按下，第二次是抬起 */
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
					}
				}
				/* 快速连续触发 */
				else if(g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 连续快速触发的时候如果已经触发了按键，那所有的行程都能做快速触发 */
					if(Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] ==0)
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
							{
								Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
								//---------------------------------------------------------------------------------------------------------------------------
								Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
								//---------------------------------------------------------------------------------------------------------------------------
								TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								/* 短按第一次是按下，第二次是抬起 */
								if(Flag_TGLKey_NeedBreak[TGLKey_id] == 0)
								{
									switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]];
											s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]]>>8;
											Has_Key_Report_Interface0  = 1;
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											/* 切灯效 */
											if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x01)
											{
												LedMode_Change();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度加 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x04)
											{
												LedVolume_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度减 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x05)
											{
												LedVolume_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度加 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x06)
											{
												LedSpeed_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度减 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x07)
											{
												LedSpeed_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 颜色切换 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x09)
											{
												LedColorIndex_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* mac切换 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x0F)
											{
												g_variableData.DriverAPP_WinMacMode = 1;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win切换 */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x10)
											{
												g_variableData.DriverAPP_WinMacMode = 0;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win lock */
											else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x11)
											{
												g_variableData.DriverAPP_WinLock_Switch = 1;
												DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
											}
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
								/* 短按第一次是按下，第二次是抬起 */
								else
								{
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											s_ConsumerBufferIn[1] = 0;
											s_ConsumerBufferIn[2] = 0;
											Has_Key_Report_Interface0 = 1;
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											/* 短按第一次是按下，第二次是抬起 */
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
											Current_key_state[SPI_GROUP][Keyindex] = 1;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 1;
											TGLKey_CNT[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
					}
					/* 连续快速触发的时候已经触发过了，那就所有的行程都做rapid trigger */
					else
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
							//---------------------------------------------------------------------------------------------------------------------------
							TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							/* 短按第一次是按下，第二次是抬起 */
							if(Flag_TGLKey_NeedBreak[TGLKey_id] == 0)
							{
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]];
										s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]]>>8;
										Has_Key_Report_Interface0  = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										/* 切灯效 */
										if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x01)
										{
											LedMode_Change();
											Flag_LedMode_ModeInit = 1;
										}
										/* 亮度加 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x04)
										{
											LedVolume_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* 亮度减 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x05)
										{
											LedVolume_Dec();
											Flag_LedMode_ModeInit = 1;
										}
										/* 速度加 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x06)
										{
											LedSpeed_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* 速度减 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x07)
										{
											LedSpeed_Dec();
											Flag_LedMode_ModeInit = 1;
										}
										/* 颜色切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x09)
										{
											LedColorIndex_Inc();
											Flag_LedMode_ModeInit = 1;
										}
										/* mac切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x0F)
										{
											g_variableData.DriverAPP_WinMacMode = 1;
											DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
										}
										/* win切换 */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x10)
										{
											g_variableData.DriverAPP_WinMacMode = 0;
											DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
										}
										/* win lock */
										else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x11)
										{
											g_variableData.DriverAPP_WinLock_Switch = 1;
											DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
										}
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							/* 短按第一次是按下，第二次是抬起 */
							else
							{
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										s_ConsumerBufferIn[1] = 0;
										s_ConsumerBufferIn[2] = 0;
										Has_Key_Report_Interface0 = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										/* 短按第一次是按下，第二次是抬起 */
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
										Current_key_state[SPI_GROUP][Keyindex] = 1;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 1;
										TGLKey_CNT[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
						}
					}
				}
				/* 普通模式 */
				else
				{
					if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//---------------------------------------------------------------------------------------------------------------------------
						TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						//---------------------------------------------------------------------------------------------------------------------------
						/* 不考虑全键无冲/不考虑FN键是否按 */
						//---------------------------------------------------------------------------------------------------------------------------
						/* 短按第一次是按下，第二次是抬起 */
						if(Flag_TGLKey_NeedBreak[TGLKey_id] == 0)
						{
							switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 媒体键 */
								case KeyType_Media_ID:
									s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]];
									s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1]]>>8;
									Has_Key_Report_Interface0  = 1;
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 功能键、模式切换等 */
								case KeyType_Function_ID:
									/* 切灯效 */
									if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x01)
									{
										LedMode_Change();
										Flag_LedMode_ModeInit = 1;
									}
									/* 亮度加 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x04)
									{
										LedVolume_Inc();
										Flag_LedMode_ModeInit = 1;
									}
									/* 亮度减 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x05)
									{
										LedVolume_Dec();
										Flag_LedMode_ModeInit = 1;
									}
									/* 速度加 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x06)
									{
										LedSpeed_Inc();
										Flag_LedMode_ModeInit = 1;
									}
									/* 速度减 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x07)
									{
										LedSpeed_Dec();
										Flag_LedMode_ModeInit = 1;
									}
									/* 颜色切换 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x09)
									{
										LedColorIndex_Inc();
										Flag_LedMode_ModeInit = 1;
									}
									/* mac切换 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x0F)
									{
										g_variableData.DriverAPP_WinMacMode = 1;
										DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
									}
									/* win切换 */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x10)
									{
										g_variableData.DriverAPP_WinMacMode = 0;
										DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
									}
									/* win lock */
									else if(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1] == 0x11)
									{
										g_variableData.DriverAPP_WinLock_Switch = 1;
										DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
									}
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* Ctrl等、普通按键 */
								case KeyType_System_ID:
									HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
									break;
								case KeyType_Normal_ID:
									HandleDKSKey_Press(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								default:
									break;
							}
						}
						/* 短按第一次是按下，第二次是抬起 */
						else
						{
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
							{
								//---------------------------------------------------------------------------------------------------------------------------
								/* 媒体键 */
								case KeyType_Media_ID:
									//想一下媒体键要不要发抬起
									s_ConsumerBufferIn[1] = 0;
									s_ConsumerBufferIn[2] = 0;
									Has_Key_Report_Interface0 = 1;
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									/* 短按第一次是按下，第二次是抬起 */
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 功能键、模式切换等 */
								case KeyType_Function_ID:
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								/* Ctrl等、普通按键 */
								case KeyType_System_ID:
									HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
									/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									break;
								case KeyType_Normal_ID:
									HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
									/* 短按第一次是按下，第二次是抬起,按下的时候发抬起的数据的时候要按照正常按下的流程走 */
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									Flag_TGL_Press[TGLKey_id] = 1;
									TGLKey_CNT[TGLKey_id] = 0;
									break;
								//---------------------------------------------------------------------------------------------------------------------------
								default:
									break;
							}
						}
					}
				}
			}
		}
		else
		{
			Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
		}
	}
	/* 抬起 */
	else if(Hall_Segment < Segment_Last_SPI[SPI_GROUP][Keyindex])
	{
		if(Current_key_state[SPI_GROUP][Keyindex])
		{
			/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex] == TriggerPoint_MAX)
			{
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					if(Hall_Segment != 0)
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
							{
								TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
								if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex]))
								{
									Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
									//-----------------------------------------------------------------------------------------------------------------------------
									TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									if(Flag_TGL_Press[TGLKey_id])
									{
										if(TGLKey_CNT[TGLKey_id] > TIME_200MS)
										{
											/* 不考虑全键无冲/不考虑FN键是否按 */
											//---------------------------------------------------------------------------------------------------------------------------
											switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
											{
												//---------------------------------------------------------------------------------------------------------------------------
												/* 媒体键 */
												case KeyType_Media_ID:
													//想一下媒体键要不要发抬起
													s_ConsumerBufferIn[1] = 0;
													s_ConsumerBufferIn[2] = 0;
													Has_Key_Report_Interface0 = 1;
													Current_key_state[SPI_GROUP][Keyindex] = 0;
													Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
													Flag_TGL_Press[TGLKey_id] = 0;
													TGLKey_CNT[TGLKey_id] = 0;
													Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
													break;
												//---------------------------------------------------------------------------------------------------------------------------
												/* 功能键、模式切换等 */
												case KeyType_Function_ID:
													Current_key_state[SPI_GROUP][Keyindex] = 0;
													Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
													Flag_TGL_Press[TGLKey_id] = 0;
													TGLKey_CNT[TGLKey_id] = 0;
													Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
													break;
												//---------------------------------------------------------------------------------------------------------------------------
												/* Ctrl等、普通按键 */
												case KeyType_System_ID:
													HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
													Current_key_state[SPI_GROUP][Keyindex] = 0;
													Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
													Flag_TGL_Press[TGLKey_id] = 0;
													TGLKey_CNT[TGLKey_id] = 0;
													Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
													break;
												case KeyType_Normal_ID:
													HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
													Current_key_state[SPI_GROUP][Keyindex] = 0;
													Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
													Flag_TGL_Press[TGLKey_id] = 0;
													TGLKey_CNT[TGLKey_id] = 0;
													Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
													break;
												//---------------------------------------------------------------------------------------------------------------------------
												default:
													break;
											}
										}
										else
										{
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 短按的时候这里要这准备第二次按下的时候做抬起 */
											Flag_TGL_Press[TGLKey_id] = 0;
											//---------------------------------------------------------------------------------------------------------------------------
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = ~Flag_TGLKey_NeedBreak[TGLKey_id];
											usb_echo("RGB:0x%x\n",Flag_TGLKey_NeedBreak[TGLKey_id]);
										}
									}
								}
							}
						}
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发,如果抬起太快直接超过了触发点，那就直接抬起 */
						else
						{
							//-----------------------------------------------------------------------------------------------------------------------------
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//-----------------------------------------------------------------------------------------------------------------------------
							TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if(Flag_TGL_Press[TGLKey_id])
							{
								if(TGLKey_CNT[TGLKey_id] > TIME_200MS)
								{
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											s_ConsumerBufferIn[1] = 0;
											s_ConsumerBufferIn[2] = 0;
											Has_Key_Report_Interface0 = 1;
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
								else
								{
									Current_key_state[SPI_GROUP][Keyindex] = 0;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 短按的时候这里要这准备第二次按下的时候做抬起 */
									Flag_TGL_Press[TGLKey_id] = 0;
									//---------------------------------------------------------------------------------------------------------------------------
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = ~Flag_TGLKey_NeedBreak[TGLKey_id];
								}
							}
						}
					}
					/* Hall_Segment = 0 */
					else
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//-----------------------------------------------------------------------------------------------------------------------------
						TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						//---------------------------------------------------------------------------------------------------------------------------
						/* 不考虑全键无冲/不考虑FN键是否按 */
						//---------------------------------------------------------------------------------------------------------------------------
						if(Flag_TGL_Press[TGLKey_id])
						{
							if(TGLKey_CNT[TGLKey_id] > TIME_200MS)
							{
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										s_ConsumerBufferIn[1] = 0;
										s_ConsumerBufferIn[2] = 0;
										Has_Key_Report_Interface0 = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								Current_key_state[SPI_GROUP][Keyindex] = 0;
								Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 短按的时候这里要这准备第二次按下的时候做抬起 */
								Flag_TGL_Press[TGLKey_id] = 0;
								//---------------------------------------------------------------------------------------------------------------------------
								TGLKey_CNT[TGLKey_id] = 0;
								Flag_TGLKey_NeedBreak[TGLKey_id] = ~Flag_TGLKey_NeedBreak[TGLKey_id];
							}
						}
					}
				}
				/* 不是快速触发的模式 */
				else
				{
					if(Hall_Segment != 0)
					{
						TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex])
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//-----------------------------------------------------------------------------------------------------------------------------
							TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if(Flag_TGL_Press[TGLKey_id])
							{
								if(TGLKey_CNT[TGLKey_id] > TIME_200MS)
								{
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											s_ConsumerBufferIn[1] = 0;
											s_ConsumerBufferIn[2] = 0;
											Has_Key_Report_Interface0 = 1;
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
								else
								{
									Current_key_state[SPI_GROUP][Keyindex] = 0;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 短按的时候这里要这准备第二次按下的时候做抬起 */
									Flag_TGL_Press[TGLKey_id] = 0;
									//---------------------------------------------------------------------------------------------------------------------------
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = ~Flag_TGLKey_NeedBreak[TGLKey_id];
								}
							}
						}
					}
					/* Hall_Segment = 0 */
					else
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//-----------------------------------------------------------------------------------------------------------------------------
						TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						//---------------------------------------------------------------------------------------------------------------------------
						/* 不考虑全键无冲/不考虑FN键是否按 */
						//---------------------------------------------------------------------------------------------------------------------------
						if(Flag_TGL_Press[TGLKey_id])
						{
							if(TGLKey_CNT[TGLKey_id] > TIME_200MS)
							{
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										s_ConsumerBufferIn[1] = 0;
										s_ConsumerBufferIn[2] = 0;
										Has_Key_Report_Interface0 = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								Current_key_state[SPI_GROUP][Keyindex] = 0;
								Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 短按的时候这里要这准备第二次按下的时候做抬起 */
								Flag_TGL_Press[TGLKey_id] = 0;
								//---------------------------------------------------------------------------------------------------------------------------
								TGLKey_CNT[TGLKey_id] = 0;
								Flag_TGLKey_NeedBreak[TGLKey_id] = ~Flag_TGLKey_NeedBreak[TGLKey_id];
							}
						}
					}
				}
			}
			/* 触发点没设置成40 */
			else
			{
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					if(Hall_Segment != 0)
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
							{
								TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
								if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Break))
								{
									Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
									//-----------------------------------------------------------------------------------------------------------------------------
									TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									if(Flag_TGL_Press[TGLKey_id])
									{
										if(TGLKey_CNT[TGLKey_id] > TIME_200MS)
										{
											/* 不考虑全键无冲/不考虑FN键是否按 */
											//---------------------------------------------------------------------------------------------------------------------------
											switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
											{
												//---------------------------------------------------------------------------------------------------------------------------
												/* 媒体键 */
												case KeyType_Media_ID:
													//想一下媒体键要不要发抬起
													s_ConsumerBufferIn[1] = 0;
													s_ConsumerBufferIn[2] = 0;
													Has_Key_Report_Interface0 = 1;
													Current_key_state[SPI_GROUP][Keyindex] = 0;
													Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
													Flag_TGL_Press[TGLKey_id] = 0;
													TGLKey_CNT[TGLKey_id] = 0;
													Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
													break;
												//---------------------------------------------------------------------------------------------------------------------------
												/* 功能键、模式切换等 */
												case KeyType_Function_ID:
													Current_key_state[SPI_GROUP][Keyindex] = 0;
													Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
													Flag_TGL_Press[TGLKey_id] = 0;
													TGLKey_CNT[TGLKey_id] = 0;
													Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
													break;
												//---------------------------------------------------------------------------------------------------------------------------
												/* Ctrl等、普通按键 */
												case KeyType_System_ID:
													HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
													Current_key_state[SPI_GROUP][Keyindex] = 0;
													Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
													Flag_TGL_Press[TGLKey_id] = 0;
													TGLKey_CNT[TGLKey_id] = 0;
													Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
													break;
												case KeyType_Normal_ID:
													HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
													Current_key_state[SPI_GROUP][Keyindex] = 0;
													Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
													Flag_TGL_Press[TGLKey_id] = 0;
													TGLKey_CNT[TGLKey_id] = 0;
													Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
													break;
												//---------------------------------------------------------------------------------------------------------------------------
												default:
													break;
											}
										}
										else
										{
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 短按的时候这里要这准备第二次按下的时候做抬起 */
											Flag_TGL_Press[TGLKey_id] = 0;
											//---------------------------------------------------------------------------------------------------------------------------
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = ~Flag_TGLKey_NeedBreak[TGLKey_id];
											usb_echo("RGB:0x%x\n",Flag_TGLKey_NeedBreak[TGLKey_id]);
										}
									}
								}
							}
						}
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发,如果抬起太快直接超过了触发点，那就直接抬起 */
						else
						{
							//-----------------------------------------------------------------------------------------------------------------------------
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//-----------------------------------------------------------------------------------------------------------------------------
							TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if(Flag_TGL_Press[TGLKey_id])
							{
								if(TGLKey_CNT[TGLKey_id] > TIME_200MS)
								{
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											s_ConsumerBufferIn[1] = 0;
											s_ConsumerBufferIn[2] = 0;
											Has_Key_Report_Interface0 = 1;
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
								else
								{
									Current_key_state[SPI_GROUP][Keyindex] = 0;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 短按的时候这里要这准备第二次按下的时候做抬起 */
									Flag_TGL_Press[TGLKey_id] = 0;
									//---------------------------------------------------------------------------------------------------------------------------
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = ~Flag_TGLKey_NeedBreak[TGLKey_id];
								}
							}
						}
					}
					/* Hall_Segment = 0 */
					else
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//-----------------------------------------------------------------------------------------------------------------------------
						TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						//---------------------------------------------------------------------------------------------------------------------------
						/* 不考虑全键无冲/不考虑FN键是否按 */
						//---------------------------------------------------------------------------------------------------------------------------
						if(Flag_TGL_Press[TGLKey_id])
						{
							if(TGLKey_CNT[TGLKey_id] > TIME_200MS)
							{
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										s_ConsumerBufferIn[1] = 0;
										s_ConsumerBufferIn[2] = 0;
										Has_Key_Report_Interface0 = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								Current_key_state[SPI_GROUP][Keyindex] = 0;
								Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 短按的时候这里要这准备第二次按下的时候做抬起 */
								Flag_TGL_Press[TGLKey_id] = 0;
								//---------------------------------------------------------------------------------------------------------------------------
								TGLKey_CNT[TGLKey_id] = 0;
								Flag_TGLKey_NeedBreak[TGLKey_id] = ~Flag_TGLKey_NeedBreak[TGLKey_id];
							}
						}
					}
				}
				/* 不是快速触发的模式 */
				else
				{
					if(Hall_Segment != 0)
					{
						TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex])
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//-----------------------------------------------------------------------------------------------------------------------------
							TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if(Flag_TGL_Press[TGLKey_id])
							{
								if(TGLKey_CNT[TGLKey_id] > TIME_200MS)
								{
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											//想一下媒体键要不要发抬起
											s_ConsumerBufferIn[1] = 0;
											s_ConsumerBufferIn[2] = 0;
											Has_Key_Report_Interface0 = 1;
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
											Current_key_state[SPI_GROUP][Keyindex] = 0;
											Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
											Flag_TGL_Press[TGLKey_id] = 0;
											TGLKey_CNT[TGLKey_id] = 0;
											Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
								else
								{
									Current_key_state[SPI_GROUP][Keyindex] = 0;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 短按的时候这里要这准备第二次按下的时候做抬起 */
									Flag_TGL_Press[TGLKey_id] = 0;
									//---------------------------------------------------------------------------------------------------------------------------
									TGLKey_CNT[TGLKey_id] = 0;
									Flag_TGLKey_NeedBreak[TGLKey_id] = ~Flag_TGLKey_NeedBreak[TGLKey_id];
								}
							}
						}
					}
					/* Hall_Segment = 0 */
					else
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//-----------------------------------------------------------------------------------------------------------------------------
						TGLKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						//---------------------------------------------------------------------------------------------------------------------------
						/* 不考虑全键无冲/不考虑FN键是否按 */
						//---------------------------------------------------------------------------------------------------------------------------
						if(Flag_TGL_Press[TGLKey_id])
						{
							if(TGLKey_CNT[TGLKey_id] > TIME_200MS)
							{
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								switch(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][2])
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 媒体键 */
									case KeyType_Media_ID:
										//想一下媒体键要不要发抬起
										s_ConsumerBufferIn[1] = 0;
										s_ConsumerBufferIn[2] = 0;
										Has_Key_Report_Interface0 = 1;
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* 功能键、模式切换等 */
									case KeyType_Function_ID:
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									/* Ctrl等、普通按键 */
									case KeyType_System_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									case KeyType_Normal_ID:
										HandleDKSKey_Break(g_DrivetData_TGL.TGL_Buffer[TGLKey_id][1], TGLKey_id, 0);
										Current_key_state[SPI_GROUP][Keyindex] = 0;
										Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
										Flag_TGL_Press[TGLKey_id] = 0;
										TGLKey_CNT[TGLKey_id] = 0;
										Flag_TGLKey_NeedBreak[TGLKey_id] = 0;
										break;
									//---------------------------------------------------------------------------------------------------------------------------
									default:
										break;
								}
							}
							else
							{
								Current_key_state[SPI_GROUP][Keyindex] = 0;
								Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 短按的时候这里要这准备第二次按下的时候做抬起 */
								Flag_TGL_Press[TGLKey_id] = 0;
								//---------------------------------------------------------------------------------------------------------------------------
								TGLKey_CNT[TGLKey_id] = 0;
								Flag_TGLKey_NeedBreak[TGLKey_id] = ~Flag_TGLKey_NeedBreak[TGLKey_id];
							}
						}
					}
				}
			}
		}
		else
		{
			Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: MTKey_Handle
 * Description  	: MT处理.
 *
 * END ****************************************************************************************************************/
void MTKey_Handle(uint8_t SPI_GROUP, uint16_t Hall_Segment, uint8_t Keyindex, uint8_t Position_Row, uint8_t Position_Col)
{
	uint8_t KeyFunctionType = 0;
	uint8_t KeyFunctionIndex = 0;
	uint8_t MTKey_id = 0;
	uint32_t MTTime = 0;
	//---------------------------------------------------------------------------------------------------------------------------
	MTKey_id = (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
	//---------------------------------------------------------------------------------------------------------------------------
	/* 按下 */
	if(Hall_Segment > Segment_Last_SPI[SPI_GROUP][Keyindex])
	{
		if(!Current_key_state[SPI_GROUP][Keyindex])
		{
			/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex] == TriggerPoint_MAX)
			{
				/* 快速触发 */
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
					if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if(Flag_MT_Press[MTKey_id] == 0)
							{
								Current_key_state[SPI_GROUP][Keyindex] = 1;
								Flag_MT_Press[MTKey_id] = 1;
								MTKey_CNT[MTKey_id] = 0;
								Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
							}
						}
					}
				}
				/* 快速连续触发 */
				else if(g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 连续快速触发的时候如果已经触发了按键，那所有的行程都能做快速触发 */
					if(Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] ==0)
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
							{
								Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
								//---------------------------------------------------------------------------------------------------------------------------
								Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if(Flag_MT_Press[MTKey_id] == 0)
								{
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_MT_Press[MTKey_id] = 1;
									MTKey_CNT[MTKey_id] = 0;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
								}
							}
						}
					}
					else
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if(Flag_MT_Press[MTKey_id] == 0)
							{
								Current_key_state[SPI_GROUP][Keyindex] = 1;
								Flag_MT_Press[MTKey_id] = 1;
								MTKey_CNT[MTKey_id] = 0;
								Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
							}
						}
					}
				}
				else
				{
					if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//---------------------------------------------------------------------------------------------------------------------------
						/* 不考虑全键无冲/不考虑FN键是否按 */
						//---------------------------------------------------------------------------------------------------------------------------
						if(Flag_MT_Press[MTKey_id] == 0)
						{
							Current_key_state[SPI_GROUP][Keyindex] = 1;
							Flag_MT_Press[MTKey_id] = 1;
							MTKey_CNT[MTKey_id] = 0;
							Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
						}
					}
				}
			}
			/* 触发值不是40的情况 */
			else
			{
				/* 快速触发 */
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
					if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if(Flag_MT_Press[MTKey_id] == 0)
							{
								Current_key_state[SPI_GROUP][Keyindex] = 1;
								Flag_MT_Press[MTKey_id] = 1;
								MTKey_CNT[MTKey_id] = 0;
								Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
							}
						}
					}
				}
				/* 快速连续触发 */
				else if(g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 连续快速触发的时候如果已经触发了按键，那所有的行程都能做快速触发 */
					if(Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] ==0)
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
							{
								Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
								//---------------------------------------------------------------------------------------------------------------------------
								Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if(Flag_MT_Press[MTKey_id] == 0)
								{
									Current_key_state[SPI_GROUP][Keyindex] = 1;
									Flag_MT_Press[MTKey_id] = 1;
									MTKey_CNT[MTKey_id] = 0;
									Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
								}
							}
						}
					}
					else
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if(Flag_MT_Press[MTKey_id] == 0)
							{
								Current_key_state[SPI_GROUP][Keyindex] = 1;
								Flag_MT_Press[MTKey_id] = 1;
								MTKey_CNT[MTKey_id] = 0;
								Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
							}
						}
					}
				}
				else
				{
					if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//---------------------------------------------------------------------------------------------------------------------------
						/* 不考虑全键无冲/不考虑FN键是否按 */
						//---------------------------------------------------------------------------------------------------------------------------
						if(Flag_MT_Press[MTKey_id] == 0)
						{
							usb_echo("MT begin.\r\n");
							Current_key_state[SPI_GROUP][Keyindex] = 1;
							Flag_MT_Press[MTKey_id] = 1;
							Flag_MT_FunPress[MTKey_id] = 0;
							MTKey_CNT[MTKey_id] = 0;
							Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 1;
						}
					}
				}
			}
		}
		else
		{
			Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
		}
	}
	/* 抬起或者保持不动 */
	else
	{
		//usb_echo("break or hold\n");
		/* 没有做过MT按键，开始去做MT按键 */
		if(Flag_MTKey_NeedBreak[MTKey_id] == 0)
		{
			if(Flag_MT_Press[MTKey_id])
			{
				/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
				if(g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex] == TriggerPoint_MAX)
				{
					if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 表示抬起了 */
							if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex]))
							{
								Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
								/* 还没有做MT的按键 */
								if(Flag_MT_FunPress[MTKey_id] == 0)
								{
									//---------------------------------------------------------------------------------------------------------------------------
									MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
									/* 时间到代表是按住 */
									if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
									/* 单击 */
									else
									{
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][6])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
								}
								/* 做了MT按键后抬起的时候补抬起 */
								else
								{
									Flag_MTKey_NeedBreak[MTKey_id] = 1;
								}
							}
							/* 表示没有抬起，在持续按下 */
							else
							{
								/* 还没有做MT的按键,e */
								if(Flag_MT_FunPress[MTKey_id] == 0)
								{
									//---------------------------------------------------------------------------------------------------------------------------
									MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
									/* 时间到代表是按住，这里只做长按的按键，不做单击的按键 */
									if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
								}
							}
						}
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发,如果抬起太快直接超过了触发点，那就直接抬起 */
						else
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 表示抬起了 */
							if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex]))
							{
								/* 还没有做MT的按键 */
								if(Flag_MT_FunPress[MTKey_id] == 0)
								{
									//---------------------------------------------------------------------------------------------------------------------------
									MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
									/* 时间到代表是按住 */
									if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
									{
										Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
									else
									{
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][6])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
								}
								/* 做了MT按键后抬起的时候补抬起 */
								else
								{
									Flag_MTKey_NeedBreak[MTKey_id] = 1;
								}
							}
							/* 表示没有抬起，在持续按下 */
							else
							{
								if(Flag_MT_FunPress[MTKey_id] == 0)
								{
									//---------------------------------------------------------------------------------------------------------------------------
									MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
									/* 时间到代表是按住 */
									if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
								}
							}
						}
					}
					else
					{
						//---------------------------------------------------------------------------------------------------------------------------
						/* 表示抬起了 */
						if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							/* 还没有做MT的按键 */
							if(Flag_MT_FunPress[MTKey_id] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
								/* 时间到代表是按住 */
								if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
											s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
											Has_Key_Report_Interface0  = 1;
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											/* 切灯效 */
											if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
											{
												LedMode_Change();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
											{
												LedVolume_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
											{
												LedVolume_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
											{
												LedSpeed_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
											{
												LedSpeed_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 颜色切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
											{
												LedColorIndex_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* mac切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
											{
												g_variableData.DriverAPP_WinMacMode = 1;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
											{
												g_variableData.DriverAPP_WinMacMode = 0;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win lock */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
											{
												g_variableData.DriverAPP_WinLock_Switch = 1;
												DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
											}
											Flag_MT_FunPress[MTKey_id] = 1;
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
								else
								{
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_MT.MT_Buffer[MTKey_id][6])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]];
											s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]]>>8;
											Has_Key_Report_Interface0  = 1;
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											/* 切灯效 */
											if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
											{
												LedMode_Change();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
											{
												LedVolume_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
											{
												LedVolume_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
											{
												LedSpeed_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
											{
												LedSpeed_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 颜色切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
											{
												LedColorIndex_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* mac切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
											{
												g_variableData.DriverAPP_WinMacMode = 1;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
											{
												g_variableData.DriverAPP_WinMacMode = 0;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win lock */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
											{
												g_variableData.DriverAPP_WinLock_Switch = 1;
												DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
											}
											Flag_MT_FunPress[MTKey_id] = 1;
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
							/* 做了MT按键后抬起的时候补抬起 */
							else
							{
								Flag_MTKey_NeedBreak[MTKey_id] = 1;
							}
						}
						else
						{
							/* 表示没有抬起，在持续按下 */
							if(Flag_MT_FunPress[MTKey_id] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
								/* 时间到代表是按住 */
								if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
											s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
											Has_Key_Report_Interface0  = 1;
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键没有抬起，无需补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											/* 切灯效 */
											if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
											{
												LedMode_Change();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
											{
												LedVolume_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
											{
												LedVolume_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
											{
												LedSpeed_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
											{
												LedSpeed_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 颜色切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
											{
												LedColorIndex_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* mac切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
											{
												g_variableData.DriverAPP_WinMacMode = 1;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
											{
												g_variableData.DriverAPP_WinMacMode = 0;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win lock */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
											{
												g_variableData.DriverAPP_WinLock_Switch = 1;
												DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
											}
											Flag_MT_FunPress[MTKey_id] = 1;
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键没有抬起，无需补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 0;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键没有抬起，无需补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
					}
				}
				/* 触发点没设置成40 */
				else
				{
					if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 表示抬起了 */
							if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Break))
							{
								Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
								/* 还没有做MT的按键 */
								if(Flag_MT_FunPress[MTKey_id] == 0)
								{
									//---------------------------------------------------------------------------------------------------------------------------
									MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
									/* 时间到代表是按住 */
									if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
									/* 单击 */
									else
									{
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][6])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
								}
								/* 做了MT按键后抬起的时候补抬起 */
								else
								{
									Flag_MTKey_NeedBreak[MTKey_id] = 1;
								}
							}
							else
							{
								/* 表示没有抬起，在持续按下 */
								if(Flag_MT_FunPress[MTKey_id] == 0)
									{
									//---------------------------------------------------------------------------------------------------------------------------
									MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
									/* 时间到代表是按住 */
									if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
									}
							}
						}
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发,如果抬起太快直接超过了触发点，那就直接抬起 */
						else
						{
							//---------------------------------------------------------------------------------------------------------------------------
							/* 表示抬起了 */
							if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Break))
							{
								Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
								/* 还没有做MT的按键 */
								if(Flag_MT_FunPress[MTKey_id] == 0)
								{
									//---------------------------------------------------------------------------------------------------------------------------
									MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
									/* 时间到代表是按住 */
									if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
									else
									{
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][6])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键已经抬起，需要补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
								}
								/* 做了MT按键后抬起的时候补抬起 */
								else
								{
									Flag_MTKey_NeedBreak[MTKey_id] = 1;
								}
							}
							else
							{
								/* 表示没有抬起，在持续按下 */
								if(Flag_MT_FunPress[MTKey_id] == 0)
								{
									//---------------------------------------------------------------------------------------------------------------------------
									MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
									/* 时间到代表是按住 */
									if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 不考虑全键无冲/不考虑FN键是否按 */
										//---------------------------------------------------------------------------------------------------------------------------
										switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
										{
											//---------------------------------------------------------------------------------------------------------------------------
											/* 媒体键 */
											case KeyType_Media_ID:
												s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
												s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
												Has_Key_Report_Interface0  = 1;
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* 功能键、模式切换等 */
											case KeyType_Function_ID:
												/* 切灯效 */
												if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
												{
													LedMode_Change();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
												{
													LedVolume_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 亮度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
												{
													LedVolume_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度加 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
												{
													LedSpeed_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* 速度减 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
												{
													LedSpeed_Dec();
													Flag_LedMode_ModeInit = 1;
												}
												/* 颜色切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
												{
													LedColorIndex_Inc();
													Flag_LedMode_ModeInit = 1;
												}
												/* mac切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
												{
													g_variableData.DriverAPP_WinMacMode = 1;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win切换 */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
												{
													g_variableData.DriverAPP_WinMacMode = 0;
													DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
												}
												/* win lock */
												else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
												{
													g_variableData.DriverAPP_WinLock_Switch = 1;
													DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
												}
												Flag_MT_FunPress[MTKey_id] = 1;
												Flag_MTKey_NeedBreak[MTKey_id] = 1;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											/* Ctrl等、普通按键 */
											case KeyType_System_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											case KeyType_Normal_ID:
												HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
												Flag_MT_FunPress[MTKey_id] = 1;
												/* 此时按键没有抬起，无需补抬起 */
												Flag_MTKey_NeedBreak[MTKey_id] = 0;
												break;
											//---------------------------------------------------------------------------------------------------------------------------
											default:
												break;
										}
									}
								}
							}
						}
					}
					else
					{
						//---------------------------------------------------------------------------------------------------------------------------
						/* 表示抬起了 */
						if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Break))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							/* 还没有做MT的按键 */
							if(Flag_MT_FunPress[MTKey_id] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
								/* 时间到代表是按住 */
								if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
											s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
											Has_Key_Report_Interface0  = 1;
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											/* 切灯效 */
											if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
											{
												LedMode_Change();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
											{
												LedVolume_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
											{
												LedVolume_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
											{
												LedSpeed_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
											{
												LedSpeed_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 颜色切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
											{
												LedColorIndex_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* mac切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
											{
												g_variableData.DriverAPP_WinMacMode = 1;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
											{
												g_variableData.DriverAPP_WinMacMode = 0;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win lock */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
											{
												g_variableData.DriverAPP_WinLock_Switch = 1;
												DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
											}
											Flag_MT_FunPress[MTKey_id] = 1;
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
								else
								{
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_MT.MT_Buffer[MTKey_id][6])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]];
											s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][5]]>>8;
											Has_Key_Report_Interface0  = 1;
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											/* 切灯效 */
											if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
											{
												LedMode_Change();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
											{
												LedVolume_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
											{
												LedVolume_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
											{
												LedSpeed_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
											{
												LedSpeed_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 颜色切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
											{
												LedColorIndex_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* mac切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
											{
												g_variableData.DriverAPP_WinMacMode = 1;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
											{
												g_variableData.DriverAPP_WinMacMode = 0;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win lock */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
											{
												g_variableData.DriverAPP_WinLock_Switch = 1;
												DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
											}
											Flag_MT_FunPress[MTKey_id] = 1;
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键已经抬起，需要补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
							/* 做了MT按键后抬起的时候补抬起 */
							else
							{
								Flag_MTKey_NeedBreak[MTKey_id] = 1;
							}
						}
						else
						{
							/* 表示没有抬起，在持续按下 */
							if(Flag_MT_FunPress[MTKey_id] == 0)
							{
								//---------------------------------------------------------------------------------------------------------------------------
								MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
								/* 时间到代表是按住 */
								if((MTKey_CNT[MTKey_id] >= MTTime) && (Flag_MT_Press[MTKey_id]))
								{
									//---------------------------------------------------------------------------------------------------------------------------
									/* 不考虑全键无冲/不考虑FN键是否按 */
									//---------------------------------------------------------------------------------------------------------------------------
									switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
									{
										//---------------------------------------------------------------------------------------------------------------------------
										/* 媒体键 */
										case KeyType_Media_ID:
											s_ConsumerBufferIn[1] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]];
											s_ConsumerBufferIn[2] = KeyBoardLayout_Consumer_Table[g_DrivetData_MT.MT_Buffer[MTKey_id][3]]>>8;
											Has_Key_Report_Interface0  = 1;
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键没有抬起，无需补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* 功能键、模式切换等 */
										case KeyType_Function_ID:
											/* 切灯效 */
											if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x01)
											{
												LedMode_Change();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x04)
											{
												LedVolume_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 亮度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x05)
											{
												LedVolume_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度加 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x06)
											{
												LedSpeed_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* 速度减 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x07)
											{
												LedSpeed_Dec();
												Flag_LedMode_ModeInit = 1;
											}
											/* 颜色切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x09)
											{
												LedColorIndex_Inc();
												Flag_LedMode_ModeInit = 1;
											}
											/* mac切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x0F)
											{
												g_variableData.DriverAPP_WinMacMode = 1;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win切换 */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x10)
											{
												g_variableData.DriverAPP_WinMacMode = 0;
												DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
											}
											/* win lock */
											else if(g_DrivetData_MT.MT_Buffer[MTKey_id][3] == 0x11)
											{
												g_variableData.DriverAPP_WinLock_Switch = 1;
												DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
											}
											Flag_MT_FunPress[MTKey_id] = 1;
											Flag_MTKey_NeedBreak[MTKey_id] = 1;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										/* Ctrl等、普通按键 */
										case KeyType_System_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键没有抬起，无需补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 0;
											break;
										case KeyType_Normal_ID:
											HandleDKSKey_Press(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
											Flag_MT_FunPress[MTKey_id] = 1;
											/* 此时按键没有抬起，无需补抬起 */
											Flag_MTKey_NeedBreak[MTKey_id] = 0;
											break;
										//---------------------------------------------------------------------------------------------------------------------------
										default:
											break;
									}
								}
							}
						}
					}
				}
			}
			else
			{
				/* 操作都做完了要及时同步分段值 */
				Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
			}
		}
		/* 补抬起 */
		else
		{
			Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
			//-----------------------------------------------------------------------------------------------------------------------------
			MTTime = ((g_DrivetData_MT.MT_Buffer[MTKey_id][1] | (g_DrivetData_MT.MT_Buffer[MTKey_id][2] << 8))*1000);
			/* 时间到代表是按住 */
			if(MTKey_CNT[MTKey_id] >= MTTime)
			{
				/* 不考虑全键无冲/不考虑FN键是否按 */
				//---------------------------------------------------------------------------------------------------------------------------
				switch(g_DrivetData_MT.MT_Buffer[MTKey_id][4])
				{
					//---------------------------------------------------------------------------------------------------------------------------
					/* 媒体键 */
					case KeyType_Media_ID:
						s_ConsumerBufferIn[1] = 0;
						s_ConsumerBufferIn[2] = 0;
						Has_Key_Report_Interface0  = 1;
						Flag_MT_Press[MTKey_id] = 0;
						Flag_MT_FunPress[MTKey_id] = 0;
						Flag_MTKey_NeedBreak[MTKey_id] = 0;
						MTKey_CNT[MTKey_id] = 0;
						Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
						break;
					//---------------------------------------------------------------------------------------------------------------------------
					/* 功能键、模式切换等 */
					case KeyType_Function_ID:
						Flag_MT_Press[MTKey_id] = 0;
						Flag_MT_FunPress[MTKey_id] = 0;
						Flag_MTKey_NeedBreak[MTKey_id] = 0;
						MTKey_CNT[MTKey_id] = 0;
						Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
						break;
					//---------------------------------------------------------------------------------------------------------------------------
					/* Ctrl等、普通按键 */
					case KeyType_System_ID:
						HandleDKSKey_Break(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
						Flag_MT_Press[MTKey_id] = 0;
						Flag_MTKey_NeedBreak[MTKey_id] = 0;
						Flag_MT_FunPress[MTKey_id] = 0;
						MTKey_CNT[MTKey_id] = 0;
						Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
						break;
					case KeyType_Normal_ID:
						HandleDKSKey_Break(g_DrivetData_MT.MT_Buffer[MTKey_id][3], MTKey_id, 0);
						Flag_MT_Press[MTKey_id] = 0;
						Flag_MTKey_NeedBreak[MTKey_id] = 0;
						Flag_MT_FunPress[MTKey_id] = 0;
						MTKey_CNT[MTKey_id] = 0;
						Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
						break;
					//---------------------------------------------------------------------------------------------------------------------------
					default:
						break;
				}
			}
			/* 时间没到代表是单击 */
			else if(MTKey_CNT[MTKey_id] < MTTime)
			{
				/* 不考虑全键无冲/不考虑FN键是否按 */
				//---------------------------------------------------------------------------------------------------------------------------
				switch(g_DrivetData_MT.MT_Buffer[MTKey_id][6])
				{
					//---------------------------------------------------------------------------------------------------------------------------
					/* 媒体键 */
					case KeyType_Media_ID:
						s_ConsumerBufferIn[1] = 0;
						s_ConsumerBufferIn[2] = 0;
						Has_Key_Report_Interface0  = 1;
						Flag_MT_Press[MTKey_id] = 0;
						Flag_MT_FunPress[MTKey_id] = 0;
						Flag_MTKey_NeedBreak[MTKey_id] = 0;
						MTKey_CNT[MTKey_id] = 0;
						Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
						break;
					//---------------------------------------------------------------------------------------------------------------------------
					/* 功能键、模式切换等 */
					case KeyType_Function_ID:
						Flag_MT_Press[MTKey_id] = 0;
						Flag_MT_FunPress[MTKey_id] = 0;
						Flag_MTKey_NeedBreak[MTKey_id] = 0;
						MTKey_CNT[MTKey_id] = 0;
						Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
						break;
					//---------------------------------------------------------------------------------------------------------------------------
					/* Ctrl等、普通按键 */
					case KeyType_System_ID:
						HandleDKSKey_Break(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
						Flag_MT_Press[MTKey_id] = 0;
						Flag_MT_FunPress[MTKey_id] = 0;
						Flag_MTKey_NeedBreak[MTKey_id] = 0;
						MTKey_CNT[MTKey_id] = 0;
						Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
						break;
					case KeyType_Normal_ID:
						HandleDKSKey_Break(g_DrivetData_MT.MT_Buffer[MTKey_id][5], MTKey_id, 0);
						Flag_MT_Press[MTKey_id] = 0;
						Flag_MT_FunPress[MTKey_id] = 0;
						Flag_MTKey_NeedBreak[MTKey_id] = 0;
						MTKey_CNT[MTKey_id] = 0;
						Flag_DriverKeyStart[SPI_GROUP][Keyindex] = 0;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
						break;
					//---------------------------------------------------------------------------------------------------------------------------
					default:
						break;
				}
			}
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: MACROKey_Handle
 * Description  	: 宏处理.
 *
 * END ****************************************************************************************************************/
void MACROKey_Handle(uint8_t SPI_GROUP, uint16_t Hall_Segment, uint8_t Keyindex, uint8_t Position_Row, uint8_t Position_Col)
{
	uint8_t KeyFunctionType = 0;
	uint8_t KeyFunctionIndex = 0;
	uint8_t MACROKey_id = 0;
	/* 按下 */
	if(Hall_Segment > Segment_Last_SPI[SPI_GROUP][Keyindex])
	{
		if(!Current_key_state[SPI_GROUP][Keyindex])
		{
			/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex] == TriggerPoint_MAX)
			{
				/* 快速触发 */
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
					if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							Macro_ID_Check = MACROKey_id;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if((MACROKey_State != 0x03) && (Macro_Press_again == 0))			//循环至此按键按下
							{
								u1.fReadROMdata=1;					//在ProcessMacroKey時讀取ROM data,存在FlashBuff.
								ReadMacroCount=0;					//每个macro五包，从第一包开始
								KeyCount=0;							//从第一个byte开始
								u1.fIntoMacro=1;					//這3個flag要都為1,在主流程才能進入macro key check.
								u1.fKeyDelay=1;
								u1.Macro_Pres=1;
								u1.Macro_Break=0;
								FinishKeycodeSend=1;
								fKeyDelay=1;
								Current_key_state[SPI_GROUP][Keyindex] = 1;
							}
							if(Macro_Press_again)
							{
								Macro_Press_again = 2;
							}
							else
							{
								Macro_Press_again = 1;
							}
							//---------------------------------------------------------------------------------------------------------------------------
						}
					}
				}
				/* 快速连续触发 */
				else if(g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 连续快速触发的时候如果没有触发，要等到到触发值以后再做rapid trigger */
					if(Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] ==0)
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
							{
								Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
								//---------------------------------------------------------------------------------------------------------------------------
								Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
								//---------------------------------------------------------------------------------------------------------------------------
								MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
								Macro_ID_Check = MACROKey_id;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if((MACROKey_State != 0x03) && (Macro_Press_again == 0))			//循环至此按键按下
								{
									u1.fReadROMdata=1;					//在ProcessMacroKey時讀取ROM data,存在FlashBuff.
									ReadMacroCount=0;					//每个macro五包，从第一包开始
									KeyCount=0;							//从第一个byte开始
									u1.fIntoMacro=1;					//這3個flag要都為1,在主流程才能進入macro key check.
									u1.fKeyDelay=1;
									u1.Macro_Pres=1;
									u1.Macro_Break=0;
									FinishKeycodeSend=1;
									fKeyDelay=1;
									Current_key_state[SPI_GROUP][Keyindex] = 1;
								}
								if(Macro_Press_again)
								{
									Macro_Press_again = 2;
								}
								else
								{
									Macro_Press_again = 1;
								}
								//---------------------------------------------------------------------------------------------------------------------------
							}
						}
					}
					/* 连续快速触发的时候已经触发过了，那就所有的行程都做rapid trigger */
					else
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
							//---------------------------------------------------------------------------------------------------------------------------
							MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							Macro_ID_Check = MACROKey_id;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if((MACROKey_State != 0x03) && (Macro_Press_again == 0))			//循环至此按键按下
							{
								u1.fReadROMdata=1;					//在ProcessMacroKey時讀取ROM data,存在FlashBuff.
								ReadMacroCount=0;					//每个macro五包，从第一包开始
								KeyCount=0;							//从第一个byte开始
								u1.fIntoMacro=1;					//這3個flag要都為1,在主流程才能進入macro key check.
								u1.fKeyDelay=1;
								u1.Macro_Pres=1;
								u1.Macro_Break=0;
								FinishKeycodeSend=1;
								fKeyDelay=1;
								Current_key_state[SPI_GROUP][Keyindex] = 1;
							}
							if(Macro_Press_again)
							{
								Macro_Press_again = 2;
							}
							else
							{
								Macro_Press_again = 1;
							}
							//---------------------------------------------------------------------------------------------------------------------------
						}
					}
				}
				/* 普通模式 */
				else
				{
					if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//---------------------------------------------------------------------------------------------------------------------------
						MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						Macro_ID_Check = MACROKey_id;
						//---------------------------------------------------------------------------------------------------------------------------
						/* 不考虑全键无冲/不考虑FN键是否按 */
						//---------------------------------------------------------------------------------------------------------------------------
						if((MACROKey_State != 0x03) && (Macro_Press_again == 0))			//循环至此按键按下
						{
							u1.fReadROMdata=1;					//在ProcessMacroKey時讀取ROM data,存在FlashBuff.
							ReadMacroCount=0;					//每个macro五包，从第一包开始
							KeyCount=0;							//从第一个byte开始
							u1.fIntoMacro=1;					//這3個flag要都為1,在主流程才能進入macro key check.
							u1.fKeyDelay=1;
							u1.Macro_Pres=1;
							u1.Macro_Break=0;
							FinishKeycodeSend=1;
							fKeyDelay=1;
							Current_key_state[SPI_GROUP][Keyindex] = 1;
						}
						if(Macro_Press_again)
						{
							Macro_Press_again = 2;
						}
						else
						{
							Macro_Press_again = 1;
						}
						//---------------------------------------------------------------------------------------------------------------------------
					}
				}
			}
			/* 触发值不是40的情况 */
			else
			{
				/* 快速触发 */
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
					if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							Macro_ID_Check = MACROKey_id;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if((MACROKey_State != 0x03) && (Macro_Press_again == 0))			//循环至此按键按下
							{
								u1.fReadROMdata=1;					//在ProcessMacroKey時讀取ROM data,存在FlashBuff.
								ReadMacroCount=0;					//每个macro五包，从第一包开始
								KeyCount=0;							//从第一个byte开始
								u1.fIntoMacro=1;					//這3個flag要都為1,在主流程才能進入macro key check.
								u1.fKeyDelay=1;
								u1.Macro_Pres=1;
								u1.Macro_Break=0;
								FinishKeycodeSend=1;
								fKeyDelay=1;
								Current_key_state[SPI_GROUP][Keyindex] = 1;
							}
							if(Macro_Press_again)
							{
								Macro_Press_again = 2;
							}
							else
							{
								Macro_Press_again = 1;
							}
							//---------------------------------------------------------------------------------------------------------------------------
						}
					}
				}
				/* 快速连续触发 */
				else if(g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					/* 连续快速触发的时候如果已经触发了按键，那所有的行程都能做快速触发 */
					if(Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] ==0)
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
							{
								Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
								//---------------------------------------------------------------------------------------------------------------------------
								Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
								//---------------------------------------------------------------------------------------------------------------------------
								MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
								Macro_ID_Check = MACROKey_id;
								//---------------------------------------------------------------------------------------------------------------------------
								/* 不考虑全键无冲/不考虑FN键是否按 */
								//---------------------------------------------------------------------------------------------------------------------------
								if((MACROKey_State != 0x03) && (Macro_Press_again == 0))			//循环至此按键按下
								{
									u1.fReadROMdata=1;					//在ProcessMacroKey時讀取ROM data,存在FlashBuff.
									ReadMacroCount=0;					//每个macro五包，从第一包开始
									KeyCount=0;							//从第一个byte开始
									u1.fIntoMacro=1;					//這3個flag要都為1,在主流程才能進入macro key check.
									u1.fKeyDelay=1;
									u1.Macro_Pres=1;
									u1.Macro_Break=0;
									FinishKeycodeSend=1;
									fKeyDelay=1;
									Current_key_state[SPI_GROUP][Keyindex] = 1;
								}
								if(Macro_Press_again)
								{
									Macro_Press_again = 2;
								}
								else
								{
									Macro_Press_again = 1;
								}
								//---------------------------------------------------------------------------------------------------------------------------
							}
						}
					}
					/* 连续快速触发的时候已经触发过了，那就所有的行程都做rapid trigger */
					else
					{
						if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//---------------------------------------------------------------------------------------------------------------------------
							Flag_Continue_TriggerMode_HasTrigger[SPI_GROUP][Keyindex] = 1;
							//---------------------------------------------------------------------------------------------------------------------------
							MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							Macro_ID_Check = MACROKey_id;
							//---------------------------------------------------------------------------------------------------------------------------
							/* 不考虑全键无冲/不考虑FN键是否按 */
							//---------------------------------------------------------------------------------------------------------------------------
							if((MACROKey_State != 0x03) && (Macro_Press_again == 0))			//循环至此按键按下
							{
								u1.fReadROMdata=1;					//在ProcessMacroKey時讀取ROM data,存在FlashBuff.
								ReadMacroCount=0;					//每个macro五包，从第一包开始
								KeyCount=0;							//从第一个byte开始
								u1.fIntoMacro=1;					//這3個flag要都為1,在主流程才能進入macro key check.
								u1.fKeyDelay=1;
								u1.Macro_Pres=1;
								u1.Macro_Break=0;
								FinishKeycodeSend=1;
								fKeyDelay=1;
								Current_key_state[SPI_GROUP][Keyindex] = 1;
							}
							if(Macro_Press_again)
							{
								Macro_Press_again = 2;
							}
							else
							{
								Macro_Press_again = 1;
							}
							//---------------------------------------------------------------------------------------------------------------------------
						}
					}
				}
				/* 普通模式 */
				else
				{
					if((Hall_Segment - Segment_Last_SPI[SPI_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI_GROUP][Keyindex]))
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//---------------------------------------------------------------------------------------------------------------------------
						MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						Macro_ID_Check = MACROKey_id;
						//---------------------------------------------------------------------------------------------------------------------------
						/* 不考虑全键无冲/不考虑FN键是否按 */
						//---------------------------------------------------------------------------------------------------------------------------
						if((MACROKey_State != 0x03) && (Macro_Press_again == 0))			//循环至此按键按下
						{
							u1.fReadROMdata=1;					//在ProcessMacroKey時讀取ROM data,存在FlashBuff.
							ReadMacroCount=0;					//每个macro五包，从第一包开始
							KeyCount=0;							//从第一个byte开始
							u1.fIntoMacro=1;					//這3個flag要都為1,在主流程才能進入macro key check.
							u1.fKeyDelay=1;
							u1.Macro_Pres=1;
							u1.Macro_Break=0;
							FinishKeycodeSend=1;
							fKeyDelay=1;
							Current_key_state[SPI_GROUP][Keyindex] = 1;
						}
						if(Macro_Press_again)
						{
							Macro_Press_again = 2;
						}
						else
						{
							Macro_Press_again = 1;
						}
						//---------------------------------------------------------------------------------------------------------------------------
					}
				}
			}
		}
		else
		{
			Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
		}
	}
	/* 抬起 */
	else if(Hall_Segment < Segment_Last_SPI[SPI_GROUP][Keyindex])
	{
		if(Current_key_state[SPI_GROUP][Keyindex])
		{
			/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex] == TriggerPoint_MAX)
			{
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					if(Hall_Segment != 0)
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
							{
								if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex]))
								{
									Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
									//-----------------------------------------------------------------------------------------------------------------------------
									MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
									//---------------------------------------------------------------------------------------------------------------------------
									u1.Macro_Pres=0;
									u1.Macro_Break=1;
									Current_key_state[SPI_GROUP][Keyindex] = 0;
								}
							}
						}
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发,如果抬起太快直接超过了触发点，那就直接抬起 */
						else
						{
							//-----------------------------------------------------------------------------------------------------------------------------
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//-----------------------------------------------------------------------------------------------------------------------------
							MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							u1.Macro_Pres=0;
							u1.Macro_Break=1;
							Current_key_state[SPI_GROUP][Keyindex] = 0;
						}
					}
					/* Hall_Segment = 0 */
					else
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//-----------------------------------------------------------------------------------------------------------------------------
						MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						//---------------------------------------------------------------------------------------------------------------------------
						u1.Macro_Pres=0;
						u1.Macro_Break=1;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
					}
				}
				/* 不是快速触发的模式 */
				else
				{
					if(Hall_Segment != 0)
					{
						if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex])
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//-----------------------------------------------------------------------------------------------------------------------------
							MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							u1.Macro_Pres=0;
							u1.Macro_Break=1;
							Current_key_state[SPI_GROUP][Keyindex] = 0;
						}
					}
					/* Hall_Segment = 0 */
					else
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//-----------------------------------------------------------------------------------------------------------------------------
						MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						//---------------------------------------------------------------------------------------------------------------------------
						u1.Macro_Pres=0;
						u1.Macro_Break=1;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
					}
				}
			}
			/* 触发点没设置成40 */
			else
			{
				if(g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI_GROUP][Keyindex])
				{
					if(Hall_Segment != 0)
					{
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发 */
						if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
						{
							if((Hall_Segment >= (g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI_GROUP][Keyindex])))
							{
								if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex] + Hall_Trigger_Adjust_Value_Break))
								{
									Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
									//-----------------------------------------------------------------------------------------------------------------------------
									MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
									//---------------------------------------------------------------------------------------------------------------------------
									u1.Macro_Pres=0;
									u1.Macro_Break=1;
									Current_key_state[SPI_GROUP][Keyindex] = 0;
								}
							}
						}
						/* 快速触发的时候要先大于触发点，在触发点后面的行程做快速触发,如果抬起太快直接超过了触发点，那就直接抬起 */
						else
						{
							//-----------------------------------------------------------------------------------------------------------------------------
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//-----------------------------------------------------------------------------------------------------------------------------
							MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							u1.Macro_Pres=0;
							u1.Macro_Break=1;
							Current_key_state[SPI_GROUP][Keyindex] = 0;
						}
					}
					/* Hall_Segment = 0 */
					else
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//-----------------------------------------------------------------------------------------------------------------------------
						MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						//---------------------------------------------------------------------------------------------------------------------------
						u1.Macro_Pres=0;
						u1.Macro_Break=1;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
					}
				}
				/* 不是快速触发的模式 */
				else
				{
					if(Hall_Segment != 0)
					{
						if((Segment_Last_SPI[SPI_GROUP][Keyindex] - Hall_Segment) >= g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI_GROUP][Keyindex])
						{
							Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
							//-----------------------------------------------------------------------------------------------------------------------------
							MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
							//---------------------------------------------------------------------------------------------------------------------------
							u1.Macro_Pres=0;
							u1.Macro_Break=1;
							Current_key_state[SPI_GROUP][Keyindex] = 0;
						}
					}
					/* Hall_Segment = 0 */
					else
					{
						Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
						//-----------------------------------------------------------------------------------------------------------------------------
						MACROKey_id =  (g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col] & 0xFF00) >> 8;
						//---------------------------------------------------------------------------------------------------------------------------
						u1.Macro_Pres=0;
						u1.Macro_Break=1;
						Current_key_state[SPI_GROUP][Keyindex] = 0;
					}
				}
			}
		}
		else
		{
			Segment_Last_SPI[SPI_GROUP][Keyindex] = Hall_Segment;
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: DriverFunction_Process
 * Description  	: 驱动按键处理.
 *
 * END ****************************************************************************************************************/
void DriverFunction_Process(uint8_t SPI_GROUP, uint8_t Hall_Segment, uint8_t Keyindex)
{
	uint8_t Position_Row = 0;
	uint8_t Position_Col = 0;

	//判断是正常的键还是有别的功能的键
	//---------------------------------------------------------------------------------------------------------------------------
	Position_Row = HallLayout_Position_Table[SPI_GROUP][Keyindex].row;
	Position_Col = HallLayout_Position_Table[SPI_GROUP][Keyindex].col;
	//---------------------------------------------------------------------------------------------------------------------------
	KeyFunctionType[SPI_GROUP][Keyindex] = g_variableData.Driver_KeyMatrix_Table[Position_Row][Position_Col];
	//---------------------------------------------------------------------------------------------------------------------------
	/* DKS */
	if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_DKS_ID)
	{
		DKSKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row, Position_Col);
	}
	//---------------------------------------------------------------------------------------------------------------------------
	/* TGL */
	else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_TGL_ID)
	{
		TGLKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row, Position_Col);
	}
	//---------------------------------------------------------------------------------------------------------------------------
	/* MT */
	else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_MT_ID)
	{
		MTKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row, Position_Col);
	}
	//---------------------------------------------------------------------------------------------------------------------------
	/* macro */
	else if(KeyFunctionType[SPI_GROUP][Keyindex] == KeyType_MACRO_ID)
	{
		MACROKey_Handle(SPI_GROUP, Hall_Segment, Keyindex, Position_Row, Position_Col);
	}
	/************************************************************************************************************************/
}

/******************************************************************************
* 处理DKS按键
******************************************************************************/
void HandleDKSKey_Break(uint8_t Keycode, uint8_t DKS_Index, uint8_t KeyIndex)
{
	uint8_t i,j;
	/* 如果这个按键确定是按下了，才会发抬起，没按下的话要发按下,因为可能没有检测到36直接检测到了40 */
	//-------------------------------------- Release --------------------------------------
	for(i=3; i<Buffer_Index; i++)
	{
		/* 第一个Buffer */
		if(i<64)
		{
			if(s_KeyboardBuffer1_DKS[i]==Keycode)
			{
				Buffer_Index--;
				for(j=i; j<Buffer_Index; j++)
				{
					s_KeyboardBuffer1_DKS[j]=s_KeyboardBuffer1_DKS[j+1];
				}
				s_KeyboardBuffer1_DKS[Buffer_Index]=0;
				Has_Key_Report_Interface1_DKS = KeyType_DKS_ID;
				break;
			}
		}
		else
		{/* 第二个Buffer */
			if(s_KeyboardBuffer2_DKS[i-64+3]==Keycode)
			{
				Buffer_Index--;
				for(j=i; j<Buffer_Index; j++)
				{
					s_KeyboardBuffer2_DKS[j-64+3]=s_KeyboardBuffer2_DKS[j+1-64+3];
				}
				s_KeyboardBuffer2_DKS[Buffer_Index+1-64+3]=0;
				Has_Key_Report_Interface2_DKS = KeyType_DKS_ID;
				break;
			}
		}
	}
}
/******************************************************************************
* 处理DKS按键
******************************************************************************/
void HandleDKSKey_Press(uint8_t Keycode, uint8_t DKS_Index, uint8_t KeyIndex)
{
	uint8_t i,j;
	//-------------------------------------- Press --------------------------------------
	//-------------------------------------- 1.02us --------------------------------------
    {
		//usb_echo("Keycode = 0x%x.\r\n",Keycode);
    	for(i=3; i<Buffer_Index; i++)
    	{
    		if(i<64)
    		{
				if(s_KeyboardBuffer1_DKS[i]==Keycode)
				{
					return;
				}
    		}
    		else
    		{
    			if(s_KeyboardBuffer2_DKS[i-64+3]==Keycode)
				{
					return;
				}
    		}

    	}
    	//usb_echo("i = %d.\r\n",i);
    	if(Buffer_Index < 64)
    	{
    		s_KeyboardBuffer1_DKS[Buffer_Index]=Keycode;
			//---------------------------------------------------------------------------------------------------------------------------
			/* 锁win */
			if((DriverAPP_WinLock_Switch) && (s_KeyboardBuffer1_DKS[Buffer_Index] == KEY_L_WIN_B))
			{
				s_KeyboardBuffer1_DKS[Buffer_Index] = 0;
				Has_Key_Report_Interface1_DKS = 0;
			}
			else
			{
				Buffer_Index++;
				Has_Key_Report_Interface1_DKS = KeyType_DKS_ID;
			}
			//---------------------------------------------------------------------------------------------------------------------------
    	}
    	else
    	{
    		s_KeyboardBuffer2_DKS[Buffer_Index-64+3]=Keycode;
			//---------------------------------------------------------------------------------------------------------------------------
			/* 锁win */
			if((DriverAPP_WinLock_Switch) && (s_KeyboardBuffer2_DKS[Buffer_Index-64+3] == KEY_L_WIN_B))
			{
				s_KeyboardBuffer2_DKS[Buffer_Index-64+3] = 0;
				Has_Key_Report_Interface2_DKS = 0;
			}
			else
			{
				Buffer_Index++;
				Has_Key_Report_Interface2_DKS = KeyType_DKS_ID;
			}
			//---------------------------------------------------------------------------------------------------------------------------
    	}
    	//usb_echo("s_KeyboardBuffer1_DKS = 0x%x.\r\n",s_KeyboardBuffer1_DKS[Buffer_Index-1]);
   	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : uint8_TGL_TIMECNT_Process
 * Description   : 计时
 *
 * END ****************************************************************************************************************/
void TGL_TIMECNT_Process(void)
{
	uint8_t i;
	for(i = 0; i<20; i++)
	{
		if(Flag_TGL_Press[i])
		{
			if(TGLKey_CNT[i]<=TIME_200MS)
			{
				TGLKey_CNT[i] += 125;
			}
		}
		if(Flag_MT_Press[i])
		{
			if(MTKey_CNT[i]<=((g_DrivetData_MT.MT_Buffer[i][1] | (g_DrivetData_MT.MT_Buffer[i][2] << 8))*1000))
			{
				MTKey_CNT[i] += 125;
			}
		}
	}
	if(u1.fTime1ms)	//10ms
   	{
		Time_1ms++;
   		if(Time_1ms>=8)
   		{
   			Time_1ms=0;
	   		if(KeyDelay && FinishKeycodeSend)
	   		{
	   			KeyDelay--;
	   			if(!KeyDelay )
	   			{
					fKeyDelay=1;
					u1.fTime1ms=0;
	   			}
	   		}
   		}
   	}
	if(Flag_FN_ESC_Status)
	{
		if(++Tick_FN_ESC_ResetDefault > TIME_5S)
		{
			Flag_FN_ESC_Status = 0;
			Tick_FN_ESC_ResetDefault = 0;
			Flag_Need_ResetDefault = 1;
		}
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : Keyboard_Variable_Init
 * Description   : 键盘参数初始化
 *
 * END ****************************************************************************************************************/
void Keyboard_Variable_Init(void)
{
	uint8_t i;
	uint8_t j;
	//---------------------------------------------------------------------------------------------------------------------------
	Tick_SETGain_Need_Wait = 0;
	//---------------------------------------------------------------------------------------------------------------------------
	for(i = 0; i < TRANSFER_SIZE; i++)
	{
		Current_key_state[SPI0_GROUP][i] = 0;
		Current_key_state[SPI1_GROUP][i] = 0;
		Current_key_state[SPI2_GROUP][i] = 0;
		Current_key_state[SPI3_GROUP][i] = 0;
		Current_key_state[SPI4_GROUP][i] = 0;
		Current_key_state[SPI6_GROUP][i] = 0;
	}
	//---------------------------------------------------------------------------------------------------------------------------
    Flag_FN_Status = 0;
	Flag_once = 0;
	Flag_FN_SPACE_Status = 0;
	Flag_FN_ESC_Status = 0;
	Flag_Need_ResetDefault = 0;
	Tick_FN_ESC_ResetDefault = 0;
	//---------------------------------------------------------------------------------------------------------------------------
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : KeyScan_Suspend
 * Description   : 睡眠的时候定时唤醒用这个函数去读raw data并判断有无按键按下;
 *
 * END ****************************************************************************************************************/
uint8_t KeyScan_Suspend(void)
{
	uint8_t i = 0;
	uint8_t ret = 0;
	/****************************************************************************************************/
	DMA_StartTransfer_Suspend();
	/****************************************************************************************************/
	for(i = 0; i<TRANSFER_SIZE; i++)
	{
		//---------------------------------------------------------------------------------------------------------------------------
		if(i<SPI0_TRANSFER_SIZE)
		{
			DMA_SPI_RX_1_Buffer_Store[i] = DMA_SPI_RX_1_Buffer[i] & 0x3FFF;
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(i))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_1_Buffer_BackUp[i] = DMA_SPI_RX_1_Buffer_Store[i];
				//-----------------------------------------------------------------------------------------
				if(DMA_SPI_RX_1_Buffer_Store[i] < 5600)
				{
					ret = 1;
				}
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------
		if(i<SPI1_TRANSFER_SIZE)
		{
			DMA_SPI_RX_2_Buffer_Store[i] = DMA_SPI_RX_2_Buffer[i] & 0x3FFF;
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(i))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_2_Buffer_BackUp[i] = DMA_SPI_RX_2_Buffer_Store[i];
				//-----------------------------------------------------------------------------------------
				if(DMA_SPI_RX_2_Buffer_Store[i] < 5600)
				{
					ret = 1;
				}
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------
		if(i<SPI3_TRANSFER_SIZE)
		{
			DMA_SPI_RX_3_Buffer_Store[i] = DMA_SPI_RX_3_Buffer[i] & 0x3FFF;
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(i))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_3_Buffer_BackUp[i] = DMA_SPI_RX_3_Buffer_Store[i];
				//-----------------------------------------------------------------------------------------
				if(DMA_SPI_RX_3_Buffer_Store[i] < 5600)
				{
					ret = 1;
				}
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------
		if(i<SPI4_TRANSFER_SIZE)
		{
			DMA_SPI_RX_4_Buffer_Store[i] = DMA_SPI_RX_4_Buffer[i] & 0x3FFF;
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(i))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_4_Buffer_BackUp[i] = DMA_SPI_RX_4_Buffer_Store[i];
				//-----------------------------------------------------------------------------------------
				if(DMA_SPI_RX_4_Buffer_Store[i] < 5600)
				{
					ret = 1;
				}
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------
		if(i<SPI7_TRANSFER_SIZE)
		{
			DMA_SPI_RX_5_Buffer_Store[i] = DMA_SPI_RX_5_Buffer[i] & 0x3FFF;
			if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(i))
			{
				//------------------------------------------------------------------------------------------
				DMA_SPI_RX_5_Buffer_BackUp[i] = DMA_SPI_RX_5_Buffer_Store[i];
				//-----------------------------------------------------------------------------------------
				if(DMA_SPI_RX_5_Buffer_Store[i] < 5600)
				{
					ret = 1;
				}
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
	/****************************************************************************************************/
	return ret;
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: FNFunction_Table
 * Description  	: FN+普通按键实现灯光切换等功能.
 *
 * END ****************************************************************************************************************/
void FNFunction_Table(uint8_t Function_index)
{
	switch(Function_index)
	{
		//---------------------------------------------------------------------------------------------------------------------------
		//灯光模式切换
		case 0x01:
			LedMode_Change();
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//灯光模式向前切换
		case 0x02:
			LedMode_Forward_Change();
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//灯光模式向后切换
		case 0x03:
			LedMode_Backward_Change();
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//灯光亮度+
		case 0x04:
			LedVolume_Inc();
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//灯光亮度-
		case 0x05:
			LedVolume_Dec();
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//灯光速度+
		case 0x06:
			LedSpeed_Inc();
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//灯光速度-
		case 0x07:
			LedSpeed_Dec();
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//灯效方向切换
		case 0x08:

			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//灯效颜色切换
		case 0x09:
			LedColorIndex_Inc();
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//背光开关
		case 0x0A:

			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//蓝牙1
		case 0x0B:

			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//蓝牙2
		case 0x0C:

			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//蓝牙3
		case 0x0D:

			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//2.4G
		case 0x0E:

			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//Mac模式
		case 0x0F:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			/* out */
			//Windows和Mac系统切换
			if(DriverAPP_WinMacMode != 1)
			{
				DriverAPP_WinMacMode = 1;
				g_variableData.DriverAPP_WinMacMode = DriverAPP_WinMacMode;
				//------------------------------------------------------------------------------------------
				/* Win_L变成Alt_L */
				g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][1] = KeyBoardLayout_NormalCode[4][2];
				/* Alt_L变成Win_L */
				g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][2] = KeyBoardLayout_NormalCode[4][1];
				//------------------------------------------------------------------------------------------
				Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_KeyMatrix;
				Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
				Tick_UpdateMCUFlash = 0;
				//-----------------------------------------------------------------------------------------------------------------------------------------
			}
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//Win模式
		case 0x10:
			if(DriverAPP_WinMacMode != 0)
			{
				DriverAPP_WinMacMode = 0;
				g_variableData.DriverAPP_WinMacMode = DriverAPP_WinMacMode;
				//------------------------------------------------------------------------------------------
				/* Win_L恢复成Win_L */
				g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][1] = KeyBoardLayout_NormalCode[4][1];
				/* Alt_L恢复成Alt_L */
				g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][2] = KeyBoardLayout_NormalCode[4][2];
				//------------------------------------------------------------------------------------------
				/* 保存flash */
				//Tick_UpdateMCUFlash = 0;
				//Flag_NeedUpdate_VariableFlash = 1;
				//------------------------------------------------------------------------------------------
				Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_KeyMatrix;
				Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
				Tick_UpdateMCUFlash = 0;
				//-----------------------------------------------------------------------------------------------------------------------------------------
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//锁Win模式
		case 0x11:
			if(DriverAPP_WinLock_Switch)
			{
				DriverAPP_WinLock_Switch = 0;
				g_variableData.DriverAPP_WinLock_Switch = DriverAPP_WinLock_Switch;
				//-----------------------------------------------------------------------------------------------------------------------------------------
				Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
				Tick_UpdateMCUFlash = 0;
				//-----------------------------------------------------------------------------------------------------------------------------------------
			}
			else
			{
				//-----------------------------------------------------------------------------------------------------------------------------------------
				/* out */
				DriverAPP_WinLock_Switch = 1;
				g_variableData.DriverAPP_WinLock_Switch = DriverAPP_WinLock_Switch;
				//-----------------------------------------------------------------------------------------------------------------------------------------
				Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
				Tick_UpdateMCUFlash = 0;
				//-----------------------------------------------------------------------------------------------------------------------------------------
			}
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//重置当前Profile
		case 0x12:

			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//恢复出厂设置
		case 0x13:
			if(Flag_FN_ESC_Status == 0)
			{
				Flag_FN_ESC_Status = 1;
				Tick_FN_ESC_ResetDefault = 0;
			}
			else
			{
				Flag_FN_ESC_Status = 0;
				Tick_FN_ESC_ResetDefault = 0;
			}
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//循环切换配置文件
		case 0x14:

			break;
		//---------------------------------------------------------------------------------------------------------------------------
		//FN+space
		case 0xFD:
			if(!Flag_FN_SPACE_Status)
			{
				Flag_FN_SPACE_Status = 1;
				Flag_LedMode_ModeInit = 1;
			}
			break;
		//---------------------------------------------------------------------------------------------------------------------------
		default:
			break;
		//---------------------------------------------------------------------------------------------------------------------------
	}

}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: FNFunction_Table
 * Description  	: FN+普通按键实现灯光切换等功能.
 *
 * END ****************************************************************************************************************/
void Reset_DefultSetting(void)
{
	uint8_t i = 0;
	uint8_t j = 0;
	if(Flag_Need_ResetDefault)
	{
		Hall_PWMOff_Suspend();
		//-----------------------------------------------------------------------------------------------------------------------------------------
		/* 参数恢复默认 */
		Variable_Reset();
		//-----------------------------------------------------------------------------------------------------------------------------------------
		/* 补抬起 */
		for(i = 0; i < TRANSFER_Group; i++)
		{
			for(j = 0; j < TRANSFER_SIZE; j++)
			{
				LastCurrent_key_state[i][j] = Current_key_state[i][j];
			}
		}

		for(i = 0; i < USB_HID_KEYBOARD_REPORT_LENGTH; i++)
		{
			s_KeyboardBuffer1_DKS[i]=0;
			s_KeyboardBuffer2_DKS[i] = 0;
		}
		Has_Key_Report_Interface1_DKS = 1;
		Has_Key_Report_Interface2_DKS = 1;
		//-----------------------------------------------------------------------------------------------------------------------------------------

	}

}



