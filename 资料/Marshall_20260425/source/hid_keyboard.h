/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_HID_KEYBOARD_H__
#define __USB_DEVICE_HID_KEYBOARD_H__

#include "dma_config.h"
#include "composite.h"
/*
 *
 * Define
 *
 */
#define RESERVE  							0xFFFF
#define VGN_lOGO  							0xFE
#define TriggerPoint_MAX  					195
/*Key Code*/
#define KEY_ERRORROLLOVER 					0x0100U
#define KEY_POSTFAIL 						0x0200U
#define KEY_ERRORUNDEFINED 					0x0300U
#define KEY_A 								0x0400U
#define KEY_B 								0x0500U
#define KEY_C 								0x0600U
#define KEY_D 								0x0700U
#define KEY_E 								0x0800U
#define KEY_F 								0x0900U
#define KEY_G 								0x0A00U
#define KEY_H 								0x0B00U
#define KEY_I 								0x0C00U
#define KEY_J 								0x0D00U
#define KEY_K 								0x0E00U
#define KEY_L 								0x0F00U
#define KEY_M 								0x1000U
#define KEY_N 								0x1100U
#define KEY_O 								0x1200U
#define KEY_P 								0x1300U
#define KEY_Q 								0x1400U
#define KEY_R 								0x1500U
#define KEY_S 								0x1600U
#define KEY_T 								0x1700U
#define KEY_U 								0x1800U
#define KEY_V 								0x1900U
#define KEY_W 								0x1A00U
#define KEY_X 								0x1B00U
#define KEY_Y 								0x1C00U
#define KEY_Z 								0x1D00U
#define KEY_1 								0x1E00U
#define KEY_2 								0x1F00U
#define KEY_3 								0x2000U
#define KEY_4 								0x2100U
#define KEY_5 								0x2200U
#define KEY_6 								0x2300U
#define KEY_7 								0x2400U
#define KEY_8 								0x2500U
#define KEY_9 								0x2600U
#define KEY_0 								0x2700U
#define KEY_ENTER 							0x2800U
#define KEY_ESC 							0x2900U
#define KEY_BACKSPACE 						0x2A00U
#define KEY_TAB 							0x2B00U
#define KEY_SPACE							0x2C00U
#define KEY_MINUS 							0x2D00U				//[-_]
#define KEY_PLUS 							0x2E00U				//[=+]
#define KEY_OBRACE 							0x2F00U				//[[{]
#define KEY_CBRACE 							0x3000U				//[]}]
#define KEY_BACKSLASH						0x3100U				//斜杠
#define KEY_NONUS_NUMBER_SIGN_TILDE 		0x3200U   			//k42
#define KEY_COLON 							0x3300U				//[;:]
#define KEY_QUOTE 							0x3400U				//['"]
#define KEY_TILDE 							0x3500U				//[`~]
#define KEY_COMMA 							0x3600U				//[,<]
#define KEY_DOT 							0x3700U				//[.>]
#define KEY_QUESTION		 				0x3800U				//[/?]
#define KEY_CAPS 							0x3900U
#define KEY_F1 								0x3A00U
#define KEY_F2 								0x3B00U
#define KEY_F3 								0x3C00U
#define KEY_F4 								0x3D00U
#define KEY_F5 								0x3E00U
#define KEY_F6 								0x3F00U
#define KEY_F7 								0x4000U
#define KEY_F8 								0x4100U
#define KEY_F9 								0x4200U
#define KEY_F10 							0x4300U
#define KEY_F11 							0x4400U
#define KEY_F12 							0x4500U
#define KEY_PRINT							0x4600U
#define KEY_SCROLL_LOCK 					0x4700U
#define KEY_PAUSE 							0x4800U
#define KEY_INSERT 							0x4900U
#define KEY_HOME 							0x4A00U
#define KEY_PAGEUP 							0x4B00U
#define KEY_DELETE 							0x4C00U
#define KEY_END 							0x4D00U
#define KEY_PAGEDOWN 						0x4E00U
#define KEY_RIGHT 							0x4F00U
#define KEY_LEFT 							0x5000U
#define KEY_DOWN 							0x5100U
#define KEY_UP								0x5200U
#define KEY_KEYPAD_NUM_LOCK_AND_CLEAR 		0x5300U
#define KEY_KEYPAD_SLASH 					0x5400U
#define KEY_KEYPAD_MUL 						0x5500U
#define KEY_KEYPAD_MINUS 					0x5600U
#define KEY_KEYPAD_PLUS 					0x5700U
#define KEY_KEYPAD_ENTER 					0x5800U
#define KEY_KEYPAD_1	 					0x5900U
#define KEY_KEYPAD_2						0x5A00U
#define KEY_KEYPAD_3						0x5B00U
#define KEY_KEYPAD_4						0x5C00U
#define KEY_KEYPAD_5 						0x5D00U
#define KEY_KEYPAD_6 						0x5E00U
#define KEY_KEYPAD_7 						0x5F00U
#define KEY_KEYPAD_8 						0x6000U
#define KEY_KEYPAD_9						0x6100U
#define KEY_KEYPAD_0						0x6200U
#define KEY_KEYPAD_DOT						0x6300U
#define KEY_SLASH 							0x6400U      			//k45
#define KEY_APP								0x6500U
#define KEY_POWER 							0x6600U
#define KEY_KEYPAD_EQUAL 					0x6700U
#define KEY_F13 							0x6800U
#define KEY_F14 							0x6900U
#define KEY_F15 							0x6A00U
#define KEY_F16 							0x6B00U
#define KEY_F17 							0x6C00U
#define KEY_F18 							0x6D00U
#define KEY_F19 							0x6E00U
#define KEY_F20 							0x6F00U
#define KEY_F21 							0x7000U
#define KEY_F22 							0x7100U
#define KEY_F23 							0x7200U
#define KEY_F24 							0x7300U
#define KEY_EXECUTE 						0x7400U
#define KEY_HELP 							0x7500U
#define KEY_MENU 							0x7600U
#define KEY_SELECT 							0x7700U
#define KEY_STOP 							0x7800U
#define KEY_AGAIN 							0x7900U
#define KEY_UNDO 							0x7A00U
#define KEY_CUT			 					0x7B00U
#define KEY_COPY 							0x7C00U
#define KEY_PASTE 							0x7D00U
#define KEY_FIND 							0x7E00U
#define KEY_MUTE 							0x7F00U
#define KEY_VOLUME_UP 						0x8000U
#define KEY_VOLUME_DOWN 					0x8100U
#define KEY_LOCKING_CAPS_LOCK 				0x8200U
#define KEY_LOCKING_NUM_LOCK 				0x8300U
#define KEY_LOCKING_SCROLL_LOCK 			0x8400U
#define KEY_KEYPAD_COMMA 					0x8500U
#define KEY_KEYPAD_EQUAL_SIGN 				0x8600U
#define KEY_INTERNATIONAL1 					0x8700U   //k56
#define KEY_INTERNATIONAL2 					0x8800U
#define KEY_INTERNATIONAL3 					0x8900U
#define KEY_INTERNATIONAL4 					0x8A00U
#define KEY_INTERNATIONAL5 					0x8B00U
#define KEY_INTERNATIONAL6 					0x8C00U
#define KEY_INTERNATIONAL7 					0x8D00U
#define KEY_INTERNATIONAL8 					0x8E00U
#define KEY_INTERNATIONAL9 					0x8F00U
#define KEY_LANG1 							0x9000U
#define KEY_LANG2 							0x9100U
#define KEY_LANG3 							0x9200U
#define KEY_LANG4 							0x9300U
#define KEY_LANG5 							0x9400U
#define KEY_LANG6 							0x9500U
#define KEY_LANG7 							0x9600U
#define KEY_LANG8 							0x9700U
#define KEY_LANG9 							0x9800U
#define KEY_ALTERNATE_ERASE 				0x9900U
#define KEY_SYSREQ 							0x9A00U
#define KEY_CANCEL 							0x9B00U
#define KEY_CLEAR 							0x9C00U
#define KEY_PRIOR 							0x9D00U
#define KEY_RETURN 							0x9E00U
#define KEY_SEPARATOR 						0x9F00U
#define KEY_OUT 							0xA000U
#define KEY_OPER 							0xA100U
#define KEY_CLEAR_AGAIN 					0xA200U
#define KEY_CRSEL 							0xA300U
#define KEY_EXSEL 							0xA400U
#define KEY_KEYPAD_00 						0xB000U
#define KEY_KEYPAD_000 						0xB100U
#define KEY_THOUSANDS_SEPARATOR 			0xB200U
#define KEY_DECIMAL_SEPARATOR 				0xB300U
#define KEY_CURRENCY_UNIT 					0xB400U
#define KEY_CURRENCY_SUB_UNIT 				0xB500U
#define KEY_KEYPAD_OPARENTHESIS 			0xB600U
#define KEY_KEYPAD_CPARENTHESIS 			0xB700U
#define KEY_KEYPAD_OBRACE 					0xB800U
#define KEY_KEYPAD_CBRACE 					0xB900U
#define KEY_KEYPAD_TAB 						0xBA00U
#define KEY_KEYPAD_BACKSPACE 				0xBB00U
#define KEY_KEYPAD_A 						0xBC00U
#define KEY_KEYPAD_B 						0xBD00U
#define KEY_KEYPAD_C 						0xBE00U
#define KEY_KEYPAD_D 						0xBF00U
#define KEY_KEYPAD_E 						0xC000U
#define KEY_KEYPAD_F 						0xC100U
#define KEY_KEYPAD_XOR 						0xC200U
#define KEY_KEYPAD_CARET 					0xC300U
#define KEY_KEYPAD_PERCENT 					0xC400U
#define KEY_KEYPAD_LESS 					0xC500U
#define KEY_KEYPAD_GREATER 					0xC600U
#define KEY_KEYPAD_AMPERSAND 				0xC700U
#define KEY_KEYPAD_LOGICAL_AND 				0xC800U
#define KEY_KEYPAD_VERTICAL_BAR 			0xC900U
#define KEY_KEYPAD_LOGIACL_OR 				0xCA00U
#define KEY_KEYPAD_COLON 					0xCB00U
#define KEY_KEYPAD_NUMBER_SIGN 				0xCC00U
#define KEY_KEYPAD_SPACE 					0xCD00U
#define KEY_KEYPAD_AT 						0xCE00U
#define KEY_KEYPAD_EXCLAMATION_MARK	 		0xCF00U
#define KEY_KEYPAD_MEMORY_STORE 			0xD000U
#define KEY_KEYPAD_MEMORY_RECALL 			0xD100U
#define KEY_KEYPAD_MEMORY_CLEAR 			0xD200U
#define KEY_KEYPAD_MEMORY_ADD 				0xD300U
#define KEY_KEYPAD_MEMORY_SUBTRACT 			0xD400U
#define KEY_KEYPAD_MEMORY_MULTIPLY 			0xD500U
#define KEY_KEYPAD_MEMORY_DIVIDE 			0xD600U
#define KEY_KEYPAD_PLUSMINUS 				0xD700U
#define KEY_KEYPAD_CLEAR 					0xD800U
#define KEY_KEYPAD_CLEAR_ENTRY 				0xD900U
#define KEY_KEYPAD_BINARY 					0xDA00U
#define KEY_KEYPAD_OCTAL 					0xDB00U
#define KEY_KEYPAD_DECIMAL 					0xDC00U
#define KEY_KEYPAD_HEXADECIMAL 				0xDD00U

//Luohao
#define   KEY_L_CTRL       					0xE008//0x01
#define   KEY_L_SHIFT					    0xE108//0x02
#define   KEY_L_ALT        					0xE208//0x04
#define   KEY_L_WIN        					0xE308//0x08
#define   KEY_R_CTRL      	 				0xE408//0x10
#define   KEY_R_SHIFT      					0xE508//0x20
#define   KEY_R_ALT        					0xE608//0x40
#define   KEY_R_WIN        					0xE708//0x80
//Luohao
#define   KEY_L_CTRL_B       				0xE0
#define   KEY_L_SHIFT_B     				0xE1
#define   KEY_L_ALT_B       				0xE2
#define   KEY_L_WIN_B        				0xE3
#define   KEY_R_CTRL_B       				0xE4
#define   KEY_R_SHIFT_B      				0xE5
#define   KEY_R_ALT_B        				0xE6
#define   KEY_R_WIN_B        				0xE7
//-----------------------------------------------------
///river define
#define KEY_FN 								0xFCEE
#define KEY_FN_SPACE 						0xFDEE
#define KEY_RES 							0x00
//-----------------------------------------------------
#define KeyType_Normal_ID 					0x00
/* 多媒体功能 */
#define KeyType_Media_ID 					0x06
/* 系统按键 */
#define KeyType_System_ID 					0x08
/* 功能按键 */
#define KeyType_Function_ID 				0x0C
#define KeyType_FNtab_ID 					0x0D
#define KeyType_Function_1_ID 				0xEE
/* DKS */
#define KeyType_DKS_ID 						0x0E
/* TGL */
#define KeyType_TGL_ID 						0x10
/* MT */
#define KeyType_MT_ID 						0x0F
/* MACRO */
#define KeyType_MACRO_ID 					0x01
/* 组合发正常码值的组合键 */
#define KeyType_FNNormal_ID 				0xFA

#define KEY_Media							0x0183
#define KEY_PLAY_PAUSE 						0x00CD
#define KEY_Scan_NextTrack 					0x00B5
#define KEY_Scan_PreviousTrack				0x00B6
#define KEY_Stop							0x00B7
#define KEY_Mute							0x00E2
#define KEY_VolumeUp						0x00E9
#define KEY_VolumeDown						0x00EA
#define KEY_AL_Email						0x018A
#define KEY_AL_Calculator					0x0192
#define KEY_AL_MyComputer					0x0194
#define KEY_AC_Home							0x0223		// WWW Home
#define KEY_AC_Search						0x0221 		// WWW Search
#define KEY_AC_Back							0x0224 		// WWW Back
#define KEY_AC_Forward						0x0225 		// WWW Forward
#define KEY_AC_stop							0x00AE 		// WWW stop
#define KEY_AC_Refresh						0x0227 		// WWW Refresh
#define KEY_AC_Favorites					0x022A 		// WWW Favorites
//-----------------------------------------------------
#define KEY_LOGO1       					0xf0
#define KEY_LOGO2       					0xf1
//-----------------------------------------------------
#define KEY_WINLOCK     					0xf2
#define KEY_BRIGHTNESS  					0xf3
#define KEY_PROFILE     					0xf4
//-----------------------------------------------------
#define RES									0xff
//--------------------------------------------------------------------
#define KEY_FN_F1          					0x0006
#define KEY_FN_F2          					0x0106
#define KEY_FN_F3          					0x0206
#define KEY_FN_F4          					0x0306
#define KEY_FN_F5          					0x0406
#define KEY_FN_F6          					0x0506
#define KEY_FN_F7          					0x0606
#define KEY_FN_F8          					0x0706
#define KEY_FN_F9          					0x0806
#define KEY_FN_F10          				0x0906
#define KEY_FN_F11          				0x1006
#define KEY_FN_F12          				0x1106

#define KEY_FN_VOLINC          				0x0606
#define KEY_FN_VOLDEC          				0x0706
//--------------------------------------------------------------------
#define KEY_FN_CALLMUTE          			0x010A
//--------------------------------------------------------------------
#define KEY_FN_S          					0x010C
#define KEY_FN_D          					0x040C
#define KEY_FN_A          					0x050C
#define KEY_FN_E          					0x060C
#define KEY_FN_Q          					0x070C
#define KEY_FN_W          					0x090C
#define KEY_FN_ESC          				0x130C
#define KEY_FN_SHIFT_R          			0x140C
//--------------------------------------------------------------------
#define KEY_FN_1          					0x00FAU
#define KEY_FN_2          					0x01FAU
#define KEY_FN_3          					0x02FAU
#define KEY_FN_4          					0x03FAU
#define KEY_FN_5          					0x04FAU
#define KEY_FN_6          					0x05FAU
#define KEY_FN_7          					0x06FAU
#define KEY_FN_8          					0x07FAU
#define KEY_FN_9          					0x08FAU
#define KEY_FN_0          					0x09FAU
#define KEY_FN_MINUS 						0x0AFAU				//[-_]
#define KEY_FN_PLUS 						0x0BFAU				//[=+]
#define KEY_FN_Y 							0x0CFAU
#define KEY_FN_U 							0x0DFAU
#define KEY_FN_I 							0x0EFAU
#define KEY_FN_O 							0x0FFAU
#define KEY_FN_P 							0x10FAU
#define KEY_FN_H 							0x11FAU
#define KEY_FN_J 							0x12FAU
#define KEY_FN_K 							0x13FAU
#define KEY_FN_L 							0x14FAU
#define KEY_FN_COLON 						0x15FAU				//[;:]
#define KEY_FN_N 							0x16FAU
#define KEY_FN_M 							0x17FAU
#define KEY_FN_COMMA 						0x18FAU				//[,<]
#define KEY_FN_DOT 							0x19FAU				//[.>]
#define KEY_FN_QUESTION		 				0x1AFAU				//[/?]
//--------------------------------------------------------------------
#define KEY_FN_TAB 							0x010DU
//--------------------------------------------------------------------
//	Digital	( 10 )
//--------------------------------------------------------------------
//Row0
#define KEY_1_INDEX          				0
#define KEY_2_INDEX          				1
#define KEY_3_INDEX          				2
#define KEY_4_INDEX          				3
#define KEY_5_INDEX          				4
#define KEY_6_INDEX          				5
#define KEY_7_INDEX          				6
#define KEY_8_INDEX   	      				7
#define KEY_9_INDEX          				8
#define KEY_0_INDEX          				9
#define KEY_MINUS_INDEX    					10   	//-_
#define KEY_PLUS_INDEX	   					11   	//=+
#define KEY_BS_INDEX		 	   			12
#define KEY_TILDE_INDEX       	  			13   	//`~
#define KEY_UNUSE1_INDEX					14 		//reserve---can add a key in here.
//--------------------------------------------------------------------
//Row1
#define KEY_Q_INDEX        	    			15
#define KEY_W_INDEX        	    			16
#define KEY_E_INDEX    	        			17
#define KEY_R_INDEX       	    			18
#define KEY_T_INDEX        	    			19
#define KEY_Y_INDEX        	    			20
#define KEY_U_INDEX        	    			21
#define KEY_I_INDEX    	        			22
#define KEY_O_INDEX     	       			23
#define KEY_P_INDEX        	    			24
#define KEY_BRKTS_L_INDEX    				25   	//[{
#define KEY_BRKTS_R_INDEX	    			26   	//]}
#define KEY_SLASH_INDEX         			27		//\|
#define KEY_DELETE_INDEX   					28
#define KEY_UNUSE2_INDEX					29 		//reserve---can add a key in here.
//--------------------------------------------------------------------
//Row2
#define KEY_CAPSLOCK_INDEX 	    			30
#define KEY_A_INDEX   	    	     		31
#define KEY_S_INDEX        	    			32
#define KEY_D_INDEX    	       	 			33
#define KEY_F_INDEX    	       		 		34
#define KEY_G_INDEX    	        			35
#define KEY_H_INDEX    	        			36
#define KEY_J_INDEX    	        			37
#define KEY_K_INDEX    	        			38
#define KEY_L_INDEX    	        			39
#define KEY_SEMICOLON_INDEX     			40   	//;:
#define KEY_QUOTATION_INDEX     			41   	//'"
#define KEY_ENTER_INDEX        				42
#define KEY_UNUSE3_INDEX					43 		//reserve---can add a key in here.
#define KEY_UNUSE4_INDEX					44 		//reserve---can add a key in here.
//--------------------------------------------------------------------
//Row3
#define KEY_L_SHIFT_INDEX     	 			45
#define KEY_Z_INDEX        	    			46
#define KEY_X_INDEX        	    			47
#define KEY_C_INDEX    	     	  	 		48
#define KEY_V_INDEX        	    			49
#define KEY_B_INDEX    	       		 		50
#define KEY_N_INDEX    	        			51
#define KEY_M_INDEX    	        			52
#define KEY_COMMA_INDEX        	 			53   	//,<
#define KEY_PERIOD_INDEX        			54   	//.>
#define KEY_DUNHAO_INDEX		 			55   	///?
#define KEY_R_SHIFT_INDEX    	 			56
#define KEY_UPARROW_INDEX  					57
#define KEY_UNUSE5_INDEX					58 		//reserve---can add a key in here.
#define KEY_UNUSE6_INDEX					59 		//reserve---can add a key in here.
//--------------------------------------------------------------------
//Row4
#define KEY_ESC_INDEX		   	    		60
#define KEY_TAB_INDEX      	    			61
#define KEY_L_CTRL_INDEX       				62
#define KEY_L_GUI_INDEX       	 			63
#define KEY_L_ALT_INDEX       	 			64
#define KEY_SPACE_INDEX	     				65
#define KEY_R_ALT_INDEX       	 			66
#define KEY_FN_INDEX						67
#define KEY_R_CTRL_INDEX      	 			68
#define KEY_LEFTARR_INDEX  					69
#define KEY_DOWNARR_INDEX  					70
#define KEY_RARR_INDEX 						71
#define KEY_PAGEDOWN_INDEX 					72
#define KEY_PAGEUP_INDEX					73
#define KEY_UNUSE7_INDEX					74 		//reserve---can add a key in here.
//--------------------------------------------------------------------
#define DKS_1_State							1 		//DKS七种状态
#define DKS_2_State							2 		//DKS七种状态
#define DKS_3_State							3 		//DKS七种状态
#define DKS_4_State							4 		//DKS七种状态
#define DKS_5_State							5 		//DKS七种状态
#define DKS_6_State							6 		//DKS七种状态
#define DKS_7_State							7 		///DKS七种状态
/* Trigger adjust value, In order to avoid jumping back and forth at the critical time, the actual trigger point should be more than the set trigger point. */
#define Hall_Trigger_Adjust_Value_Press			5
#define Hall_Trigger_Adjust_Value_Break			12
//--------------------------------------------------------------------
#define key_pos_table_ana   key_pos_table

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct _usb_device_hid_keyboard_struct
{
    uint8_t *buffer;
    uint8_t idleRate;
} usb_device_hid_keyboard_struct_t;

enum {
	SPI0_GROUP = 0,
	SPI1_GROUP = 1,
	SPI2_GROUP = 2,
	SPI3_GROUP = 3,
	SPI4_GROUP = 4,
	SPI6_GROUP = 5,
};

enum {
	FNKey_NOTPRESS = 1,
	FNKey_PRESS = 2,
};

enum {
	DKS_END 	= 0,
	DKS_Press 	= 1,
	DKS_Break 	= 2,
};
enum {
	HIDData_Comsumer 		= 1,
	HIDData_Mouse 			= 2,
	HIDData_Keyboard 		= 3,
};
typedef struct
{
	/* 以256个byte为一组 */
	//-------------------------------------------------------------------------------------------------------------------
	uint8_t ActualTriggerPoint_OnOff;
	/* 5*21 = 105 byte */
	uint8_t ActualTriggerPoint_Mode[TRANSFER_Group][TRANSFER_SIZE];
	//-------------------------------------------------------------------------------------------------------------------
	/* 5*21 = 105 byte */
	uint8_t ActualTriggerPoint[TRANSFER_Group][TRANSFER_SIZE];
	//-------------------------------------------------------------------------------------------------------------------
	/* 加参数的时候要把这个保留的数组也同步减少，加多少这里就减多少 */
	/* 256-105-105 = 46 */
	uint8_t reserve[46];
}DrivetData_TriggerPoint;

typedef struct
{
	/* 以256个byte为一组 */
	//-------------------------------------------------------------------------------------------------------------------
	/* 5*21 = 105 byte */
	uint8_t Segment_SET_Trigger_Press[TRANSFER_Group][TRANSFER_SIZE];
	//-------------------------------------------------------------------------------------------------------------------
	/* 5*21 = 105 byte */
	uint8_t Segment_SET_Trigger_Break[TRANSFER_Group][TRANSFER_SIZE];
	//-------------------------------------------------------------------------------------------------------------------
	/* 加参数的时候要把这个保留的数组也同步减少，加多少这里就减多少 */
	/* 256-105-105 = 46 */
	uint8_t reserve[46];
}DrivetData_PBPoint;

typedef struct
{
	/* 以256个byte为一组 */
	//-------------------------------------------------------------------------------------------------------------------
	/* 2 byte */
	uint8_t Trigger_Mode;
	uint8_t ContinueTriggerPoint_OnOff;
	//-------------------------------------------------------------------------------------------------------------------
	/* 5*21 = 105 byte */
	uint8_t ContinueTriggerPoint_Mode[TRANSFER_Group][TRANSFER_SIZE];
	//-------------------------------------------------------------------------------------------------------------------
	/* 5*21 = 105 byte */
	uint8_t reserve1[TRANSFER_Group][TRANSFER_SIZE];
	//-------------------------------------------------------------------------------------------------------------------
	/* 加参数的时候要把这个保留的数组也同步减少，加多少这里就减多少 */
	/* 256-105-105-2 = 44 */
	uint8_t reserve2[44];
}DrivetData_ContinuePoint;

typedef struct
{
	/* 以256个byte为一组 */
	//-------------------------------------------------------------------------------------------------------------------
	/* 1 byte */
	uint8_t DKS_OnOff;
	//-------------------------------------------------------------------------------------------------------------------
	/* 17*20 = 340 byte */
	/* 格式为：index+dks里面第一个按键的状态和码值+第二个按键的状态和码值
	 * byte 0 		1 			2 				3			4 			5 			6 				7 			8 			9 				10				11			12			13			14				15			16
	 *      index	status_low	status_high		码值High		码值Low		status_low	status_high		码值High		码值Low		status_low		status_high		码值High		码值Low		status_low	status_high		码值High		码值Low	*/
	uint8_t DKS_Buffer[340];
	//-------------------------------------------------------------------------------------------------------------------
}DrivetData_DKS;

typedef struct
{
	/* 以256个byte为一组 */
	//-------------------------------------------------------------------------------------------------------------------
	/* 1 byte */
	uint8_t TGL_OnOff;
	//-------------------------------------------------------------------------------------------------------------------
	/* 20*3 = 60 byte */
	/* 格式为：index+TGL码值（两个byte）*/
	uint8_t TGL_Buffer[20][3];
	//-------------------------------------------------------------------------------------------------------------------
	/* 加参数的时候要把这个保留的数组也同步减少，加多少这里就减多少 */
	/* 256-1-60 = 195 */
	uint8_t reserve[195];
}DrivetData_TGL;

typedef struct
{
	/* 以256个byte为一组 */
	//-------------------------------------------------------------------------------------------------------------------
	/* 1 byte */
	uint8_t MT_OnOff;
	//-------------------------------------------------------------------------------------------------------------------
	/* 20*7 = 140 byte */
	/* 格式为：index+TGL码值（两个byte）*/
	uint8_t MT_Buffer[20][7];
	//-------------------------------------------------------------------------------------------------------------------
	/* 加参数的时候要把这个保留的数组也同步减少，加多少这里就减多少 */
	/* 256-1-140 = 67 */
	uint8_t reserve[67];
	//-------------------------------------------------------------------------------------------------------------------
}DrivetData_MT;
//-------------------------------------------------------------------------------------------------------------------
typedef struct
{
	/* 以256个byte为一组 */
	//-------------------------------------------------------------------------------------------------------------------
	uint16_t KeyMatrix_Buffer[6][21];
	uint16_t FN_KeyMatrix_Buffer[6][21];
	//-------------------------------------------------------------------------------------------------------------------
}DrivetData_KeyMatrix;
//-------------------------------------------------------------------------------------------------------------------

extern DrivetData_TriggerPoint   	g_DrivetData_TriggerPoint;
extern DrivetData_PBPoint   	 	g_DrivetData_PBPoint;
extern DrivetData_ContinuePoint   	g_DrivetData_ContinuePoint;
extern DrivetData_DKS   	 		g_DrivetData_DKS;
extern DrivetData_TGL   	 		g_DrivetData_TGL;
extern DrivetData_MT   	 			g_DrivetData_MT;
extern DrivetData_KeyMatrix			g_DrivetData_KeyMatrix;

extern unsigned short trigger_point_adjust_end_timer;

extern unsigned char usb_init_last_command_done ;
//------------------------------------------------------------------------------------------------------------------------------------------------------------
extern const uint16_t Hall_Map_KeyMatrix_Table[Group][Key_Num];
extern const uint16_t KeyBoardLayout_Table[6][21];
extern const uint16_t KeyBoardLayout_NormalCode[6][21];
extern const uint16_t KeyBoardLayout_Consumer_Table[18];
extern const uint16_t KeyBoardLayout_FN_Table[6][21];
extern const uint16_t KeyBoardLayout_FNMODE[5][21];
//------------------------------------------------------------------------------------------------------------------------------------------------------------
extern uint8_t Flag_FN_SPACE_Status;
extern uint8_t Flag_FN_ESC_Status;
extern uint8_t Flag_Need_ResetDefault;
extern uint32_t Tick_FN_ESC_ResetDefault;
//------------------------------------------------------------------------------------------------------------------------------------------------------------
extern uint16_t Segment_Last_SPI[TRANSFER_Group][TRANSFER_SIZE];
//---------------------------------------------------------------------------------------------------------------------------------------
extern uint8_t Current_key_state[TRANSFER_Group][TRANSFER_SIZE];
//-------------------------------------------------------------------------------------------------------------------
extern uint8_t LastCurrent_key_state[TRANSFER_Group][TRANSFER_SIZE];
extern uint8_t LED_HIGHLIGHT_Buffer[TRANSFER_Group][TRANSFER_SIZE];
//-------------------------------------------------------------------------------------------------------------------
extern uint8_t Has_Key_Report_Interface0;
extern uint8_t Has_Key_Report_Interface3;
extern uint8_t Has_Key_Report_Interface1_DKS;
extern uint8_t Has_Key_Report_Interface2_DKS;
extern uint8_t Flag_FNMode_Init;
extern uint8_t Flag_FNAndTAB_Press;
//-------------------------------------------------------------------------------------------------------------------
extern USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_KeyboardBuffer[USB_HID_KEYBOARD_REPORT_LENGTH];
extern USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_BitmapBufferIn[HS_HID_Bitmap_INTERRUPT_IN_PACKET_SIZE];

extern USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_KeyboardBuffer1_DKS[USB_HID_KEYBOARD_REPORT_LENGTH];
extern USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_KeyboardBuffer2_DKS[USB_HID_KEYBOARD_REPORT_LENGTH];

extern USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_CustomBufferIn[USB_HID_Custom_REPORT_IN_LENGTH];
extern USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_CustomBufferOut[USB_HID_Custom_REPORT_OUT_LENGTH];
extern USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_ConsumerBufferIn[USB_HID_Consumer_REPORT_IN_LENGTH];
extern USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t s_SETREPORT_BUFFER[USB_HID_SETREPORT_BUFFER_LENGTH];
//-------------------------------------------------------------------------------------------------------------------


/*******************************************************************************
 * API
 ******************************************************************************/

extern usb_status_t USB_DeviceHidKeyboardInit(usb_device_composite_struct_t *deviceComposite);
extern usb_status_t USB_DeviceHidBitmapInit(usb_device_composite_struct_t *deviceComposite);
extern usb_status_t USB_DeviceHidCustomInit(usb_device_composite_struct_t *deviceComposite);
extern usb_status_t USB_DeviceHidKeyboardCallback(class_handle_t handle, uint32_t event, void *param);
extern usb_status_t USB_DeviceHidBitmapCallback(class_handle_t handle, uint32_t event, void *param);
extern usb_status_t USB_DeviceHidCustomCallback(class_handle_t handle, uint32_t event, void *param);
extern usb_status_t USB_DeviceHidKeyboardSetConfigure(class_handle_t handle, uint8_t configure);
extern usb_status_t USB_DeviceHidBitmapSetConfigure(class_handle_t handle, uint8_t configure);
extern usb_status_t USB_DeviceHidCustomSetConfigure(class_handle_t handle, uint8_t configure);
extern usb_status_t USB_DeviceHidKeyboardSetInterface(class_handle_t handle,
                                                      uint8_t interface,
                                                      uint8_t alternateSetting);
extern usb_status_t USB_DeviceHidBitmapSetInterface(class_handle_t handle,
                                                      uint8_t interface,
                                                      uint8_t alternateSetting);
extern usb_status_t USB_DeviceHidCustomSetInterface(class_handle_t handle,
                                                      uint8_t interface,
                                                      uint8_t alternateSetting);

extern unsigned char usb_init_last_command_done ;
//extern usb_status_t USB_DeviceHidKeyboardAction(void);
extern usb_status_t USB_DeviceHidKeyboardReport(void);

extern unsigned char* USB_DeviceHidKeyboardGetReportBuff(void);

extern unsigned char* USB_DeviceHidGenericGetReportBuffIn(void);
extern unsigned char* USB_DeviceHidGenericGetReportBuffOut(void);

extern unsigned char* USB_DeviceHidCustomGetReportBuffIn(void);
extern unsigned char* USB_DeviceHidCustomGetReportBuffOut(void);

extern usb_status_t USB_DeviceHidKeyboardReport(void);

extern uint8_t KeyData_Process(void);
//------------------------------------------------------------------------------------------
extern void Key_AllRelease(void);
extern uint8_t TestMode_data_Handle(void);
extern void TestMode_Process(void);
extern void TGL_TIMECNT_Process(void);
extern void Keyboard_Variable_Init(void);
extern uint8_t KeyScan_Suspend(void);
extern void DriverFunction_Process(uint8_t SPI_GROUP, uint8_t Hall_Segment, uint8_t Keyindex);
extern void HandleDKSKey_Press(uint8_t Keycode, uint8_t DKS_Index, uint8_t KeyIndex);
extern void HandleDKSKey_Break(uint8_t Keycode, uint8_t DKS_Index, uint8_t KeyIndex);
extern void Reset_DefultSetting(void);
//------------------------------------------------------------------------------------------

#endif /* __USB_DEVICE_HID_KEYBOARD_H__ */
