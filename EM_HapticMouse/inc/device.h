/*****************************************************************************
* File Name: device.h
*
* Version: 1.0
*
* Description:
* This file contains API which integrate all the modules and defines the device 
* functionality in various power modes
*
* Note:
*
* Owner: SKUV
*
* Related Document:
* 
*
* Hardware Dependency:
* 
*
* Code Tested With:
* 1. PSoC Creator 3.1 
* 2. ARM GCC 4.7.3
******************************************************************************
* Copyright (2015), Cypress Semiconductor Corporation.
******************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*****************************************************************************/



#ifndef _device_H_
#define _device_H_

#include "stdint.h"
#include <stdbool.h>
#include "Type.h"
#include "gloab.h"
//#include "usbhw.h"
//#include "../inc/optical.h"
//#include "../inc/scroll.h"
//#include "../inc/GPIO.h"
//#include "../inc/Timer.h"
//#include "../inc/ADC.h"
//#include "../inc/UART.h"
//#include "../inc/SPI.h"
//#include "../inc/I2C.h"
//#include "../inc/led_driver.h"
//#include "../inc/led.h"

//#include "Type.h"
//#include "usbcfg.h"

//#include "platform.h"
//#include "stdbool.h"
//#include "main.h"
//#include "gloab.h"
//#include "led_effect.h"
//#include "fingerprint.h"
//#include "test.h"
//#include "i2c_slave.h"


//#define DrvGPIORead(1,27) L_key

typedef enum
{
    No_Button =0,       
    Debouncing =1,        
} MS_Key_statet;

//#ifndef __MAIN_H__
//#define __MAIN_H__
////***********************************************************************
////*								INCLUDE									*
////***********************************************************************
//#include "../inc/GPIO.h"
//#include "../inc/Timer.h"
//#include "../inc/ADC.h"
//#include "../inc/UART.h"
//#include "../inc/SPI.h"
//#include "../inc/I2C.h"
//#include "../inc/led_driver.h"
//#include "../inc/led.h"

//#include "Type.h"
//#include "usbcfg.h"

#ifdef _MAIN_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif
//***********************************************************************
//*								DEFINE									*
//***********************************************************************
#define NRFR_REPORT_ID  0

#define NRFR_MOUSE_BUTTONS (NRFR_REPORT_ID + 1)

/**
Byte 2 in mouse packet, ID 1, is scroll wheel.
*/
#define NRFR_MOUSE_WHEEL (NRFR_MOUSE_BUTTONS + 1)

/**
Byte 3 in mouse packet, ID 1, is "pan".
*/
#define NRFR_MOUSE_PAN (NRFR_MOUSE_WHEEL + 1)
#define NRFR_ADVANCED_BUTTONS (NRFR_REPORT_ID + 1)
#define NRFR_ADVANCED_PAYLOAD_LENGTH (NRFR_ADVANCED_BUTTONS + 1)


#define SIM_MOUSE_SENSOR        0//1

#define ENABLE_UART             0

#define SUPPORT_MAC_BOOT_AND_DOS_MODE   1

#define SPIMODE                 0

#define DELAY_COUNT             (368*(MCU_OSC/48000000))            //100ns

//#define SW_DELAY                250                                 //250mS
//#define I2C_CHANGTIME           330
//#define LIGHT_MIN_PER           1                                   //% Unit
//#define LIGHT_MAX_PER           100                                 //% unit


//Keyboard define
//#define KEY_a   4
//#define KEY_b   5
//#define KEY_c   6
//#define KEY_d   7
//#define KEY_e   8
//#define KEY_f   9
//#define KEY_g   10
//#define KEY_h   11
//#define KEY_i   12
//#define KEY_j   13
//#define KEY_k   14
//#define KEY_l   15
//#define KEY_m   16
//#define KEY_n   17
//#define KEY_o   18
//#define KEY_p   19
//#define KEY_q   20
//#define KEY_r   21
//#define KEY_s   22
//#define KEY_t   23
//#define KEY_u   24
//#define KEY_v   25
//#define KEY_w   26
//#define KEY_x   27
//#define KEY_y   28
//#define KEY_z   29
//#define KEY_1   30
//#define KEY_2   31
//#define KEY_3   32
//#define KEY_4   33
//#define KEY_5   34
//#define KEY_6   35
//#define KEY_7   36
//#define KEY_8   37
//#define KEY_9   38
//#define KEY_0   39

//#define KEY_NEXT_LINE   40 //Next Line
//#define KEY_ESC         41
//#define KEY_BACK_SPACE  42 //BackSpace
//#define KEY_TAB         43
//#define KEY_SPACE       44
//#define KEY_MINUS       45 // -
//#define KEY_EQU         46 // =
//#define KEY_L_S_BR      47 // [ Square Brackets
//#define KEY_R_S_BR      48 // ]
//#define KEY_VIRGULE     49 // \

//#define KEY_F1          58
//#define KEY_F2          59
//#define KEY_F3          60
//#define KEY_F4          61
//#define KEY_F5          62
//#define KEY_F6          63
//#define KEY_F7          64
//#define KEY_F8          65
//#define KEY_F9          66
//#define KEY_F10         67
//#define KEY_F11         68
//#define KEY_F12         69

//#define KEY_UP          82 
//#define KEY_DOWN        81 
//#define KEY_LEFT        80 
//#define KEY_RIGHT       79



// USB define 
#define EP1_IN BIT0
#define EP1_OUT BIT1
#define EP2_IN BIT2
#define EP2_OUT BIT3
#define EP3_IN BIT4
#define EP3_OUT BIT5
#define EP4_IN BIT6
#define EP4_OUT BIT7



#if 0
typedef enum _STATIC_MACHINE_EVENT1_TYPE_
{
	SM_EVENT1_IDLE = 0, 	
	SM_EVENT1_MOUSE_MONITOR,	
	SM_EVENT1_KEYBOARD_MONITOR,	
	SM_EVENT1_TYPE_NUMS
}STATIC_MACHINE_EVENT1_TYPE;

typedef enum _STATIC_MACHINE_EVENT2_TYPE_
{
	SM_EVENT2_IDLE = 0, 	
	SM_EVENT2_BUTTON_HANDLER,
	SM_EVENT2_TYPE_NUMS
}STATIC_MACHINE_EVENT2_TYPE;

typedef enum _STATIC_MACHINE_EVENT3_TYPE_
{
	SM_EVENT3_IDLE = 0, 	
	SM_EVENT3_MACRO_HANDLER,
	SM_EVENT3_TYPE_NUMS
}STATIC_MACHINE_EVENT3_TYPE;

//#if 0
typedef enum _STATIC_MACHINE_EVENT4_TYPE_		//Set request
{
	SM_EVENT4_IDLE = 0,
	SM_EVENT4_GET_FIRMWAVE_VERSION =0x41,
	SM_EVENT4_FIRMWARE_UPDATE = 0x51,	
	SM_EVENT4_ERASE_FLASH = 0x52,
	SM_EVENT4_WRITE_FLASH = 0x53

}STATIC_MACHINE_EVENT4_TYPE;

typedef enum _STATIC_MACHINE_EVENT5_TYPE_
{
	SM_EVENT5_IDLE = 0, 	
	SM_EVENT5_GET_FIRMWAVE_VERSION =0x41,
  SM_EVENT5_FIRMWARE_UPDATE = 0x51,	
  SM_EVENT5_ERASE_FLASH = 0x52,
  SM_EVENT5_WRITE_FLASH = 0x53
}STATIC_MACHINE_EVENT5_TYPE;

//#endif

INTERFACE STATIC_MACHINE_EVENT1_TYPE g_StaticMachineEvent1Type;		//Sensor Key monitor
INTERFACE STATIC_MACHINE_EVENT2_TYPE g_StaticMachineEvent2Type;		//Key parser
INTERFACE STATIC_MACHINE_EVENT3_TYPE g_StaticMachineEvent3Type;		//Macro key
//#if 0
#ifdef M_EVENT4_5
INTERFACE STATIC_MACHINE_EVENT4_TYPE g_StaticMachineEvent4Type;		//Set report
INTERFACE STATIC_MACHINE_EVENT5_TYPE g_StaticMachineEvent5Type;		//Get report
#endif
#ifdef M_EVENT6_7
INTERFACE STATIC_MACHINE_EVENT6_TYPE g_StaticMachineEvent6Type;		//Set report
INTERFACE STATIC_MACHINE_EVENT7_TYPE g_StaticMachineEvent7Type;		//Get report
#endif
#endif 

typedef enum _GIndex_
{
    GIdx0 = 0,
    GIdx1,
    GIdx2,
    GIdx3,
    GIdx4,
    GIdx5,
    GIdx6,
    GIdx7,
    GIdx8,
    GIdxLen =  GIdx8,
}GIndex;
#if 0
typedef enum _PROFILEIDX_
{
	PROFILE_DEFAULT=0,
	PROFILE_1,
	PROFILE_2,
	PROFILE_3,
	PROFILE_ALL,
	LED_EFFECT_ALWAYS=0,
	LED_EFFECT_BREATH,
	LED_EFFECT_COLOR_CYCLE,
	LED_EFFECT_RANDON,
	LED_REGION_LOGO=0,
	LED_REGION_WHEEL,
	LED_REGION_SIDE,
	LED_REGION_ALL
}PROFILEIDX;


#define MAIN_FLAG_MOUSE_QIE_IN_REPORT BIT1
#define MAIN_FLAG_MOUSE_SENSOR_IN_REPORT BIT2
#define MAIN_FLAG_MOUSE_SENSOR_FRAME_CAPTURE_EVENT BIT3

//2016_0411_Hsiang
#define MAIN_FLAG_MOUSE_REPORT_READY	BIT0	
#define MAIN_FLAG_MOUSE_BUTTON_EVENT	BIT1	
#define MAIN_FLAG_MOUSE_QIE_EVENT		BIT2
#define MAIN_FLAG_MOUSE_EVENT			BIT3
#endif

/* HID Demo Variables */
#define MOUSE_IN_REPORT_SIZE 8 //6
#define MEDIA_IN_REPORT_SIZE 3   //8
#define SYSTEM_IN_REPORT_SIZE	1
#define KEYBOARD_IN_REPORT_SIZE 8//9		//2			//EP2
//#define SHORTCUT_IN_REPORT_SIZE 8

//#define CONSUMER_IN_REPORT_SIZE 3 //EP2			
#define CUSTOMER_IN_REPORT_SIZE 64 //EP3		
#define CUSTOMER_OUT_REPORT_SIZE 64 //EP3  

#define FWUPDATE_IN_REPORT_SIZE 64  //EP4
#define FWUPDATE_OUT_REPORT_SIZE 64 //EP4
#define MACRO_PRESS_ANY			0
#define MACRO_RELEASE_KEY		1



#define MAX_PROFILE	3
#define MAX_MARCO	0xFFFF
#define EEPROM_SIZE 0x1000
#define EERPOM_CHECK_ADDR 4030



#define KEY_EFFECT_CHOOSE 0x28


//#define P503 1
	
typedef __packed struct _LPC11Uxx_MAIN_SETTING_
{	
	U8 MouseInReport[MOUSE_IN_REPORT_SIZE];
//	U8 MediaInReport[MEDIA_IN_REPORT_SIZE];
//	U8 SystemInReport[SYSTEM_IN_REPORT_SIZE];
	U8 KeyboardInReport[KEYBOARD_IN_REPORT_SIZE];
//	U8 ShortcutInReport[SHORTCUT_IN_REPORT_SIZE];
//	U8 ConsumerInReport[CONSUMER_IN_REPORT_SIZE];

//	U8 FeatureInReport[16];	
	//U8 FeatureInReport[MOUSE_IN_REPORT_SIZE];
	//U8 FeatureInReport[USB_PACKET_SIZE];
	U8 InReport[CUSTOMER_IN_REPORT_SIZE];
//	U8 OutReport[CUSTOMER_OUT_REPORT_SIZE];
//  U8 FirmwareUpdateInReport[FWUPDATE_IN_REPORT_SIZE];
//  U8 FirmwareUpdateOutReport[FWUPDATE_OUT_REPORT_SIZE];
	//U8 InReportSize;
	U16 EPInOutReq;
	//U8  EP0_SetReq;
	//U8  EP0_GetReq;
	
//	U8 InReportSize;						//2016_0411_Hsiang
//	U16 MouseEvent;						//2016_0411_Hsiang
//	U16 Total_Size;
//	U16 ADR_Offset;
//	U8 SensorReset;
//	U8 MacroStopMode;
//   	 U8 OS_ModeType;
}LPC11Uxx_MAIN_SETTING;

INTERFACE LPC11Uxx_MAIN_SETTING g_Main_Setting;

#if 0
typedef struct _SENSOR_DATA_
{
	U8	X1;
	U8	X2;
	U8	Y1;
	U8	Y2;
	U8	Changed;
}SENSOR_DATA;

typedef struct _REPROT_PROCESS_
{
	U8	Mouse_Report_In;
	U8	Media_Report_In;
	U8	System_Report_In;
	U8	Keyboard_Report_In;
	U8	Macro_Keyboard_Report_In;
	U8 	Shortcut_Report_In;
	U8	Consumer_Report_In;
	U8	Feature_Report_In;
  U8  FirmwareUpdate_Report_In;
}REPORT_PROCESS;

INTERFACE REPORT_PROCESS	g_Repot_Process;

typedef struct _KEY_MATRIX_ATTR_
{
	U16 KeyDelay;
	U8	fKeyDelay;
	U8 KeyAction;
	U8 KeyTableNum;
	U8 KeyLoop;
	U8 DecKeyLoop;
}KEY_MATRIX_ATTR;

typedef struct _SET_MATRIX_A_
{
	U8	Key_Group;
	U8	Action;
	U8	M_Data_Byte;
}SET_MATRIX_A;

typedef struct _SET_TEMP_INSTANT_
{
	U8 u8InstantProfile;
	U8 u8InstantKyeNum;
	U8 u8InstantKyeEnable;
}SET_TEMP_INSTANT;




INTERFACE SET_MATRIX_A	g_Set_Matrix;
INTERFACE SENSOR_DATA	g_Sensor_Data;

INTERFACE KEY_MATRIX_ATTR	g_Key_Matrix_Attr;
INTERFACE SET_TEMP_INSTANT  g_Instant_Key;


/* HID Demo Functions */
INTERFACE void nsAPI_Main_FirmwareUpdateInReportHandler(void);
INTERFACE void nsAPI_Main_GetInReport(void);
INTERFACE void nsAPI_Main_SetOutReport(void); 
INTERFACE void nsAPI_Main_MouseInReportHandler(void);
INTERFACE void nsAPI_Main_MediaInReportHandler(void);
INTERFACE void nsAPI_Main_SystemInReportHandler(void);
INTERFACE void nsAPI_Main_KeyboardInReportHandler(void);
INTERFACE void nsAPI_Main_ShortcutInReportHandler(void);
INTERFACE void nsAPI_Main_MouseButtonEventHandler(void);
INTERFACE void nsAPI_Main_KeyboardButtonEventHandler(void);
INTERFACE void nsAPI_Main_GetInMouseQIEReport(S8 s8Temp);
INTERFACE void nsAPI_Main_Sensor_InReport(U8 * u8pTemp);
INTERFACE void nsAPI_Main_ConsumerInReportHandler(void);
INTERFACE void nsAPI_Main_Clr_Mouse_Event(void);
INTERFACE void nsAPI_Main_Clr_EP1Event(void);
INTERFACE void nsAPI_Main_Clr_KeyboardEvent(void);
INTERFACE void nsAPI_Main_Clr_ConsumerEvent(void);
INTERFACE void nsAPI_Main_Clr_EP3Event(void);
INTERFACE void nsAPI_Main_Clr_EP4Event(void);


INTERFACE void Mapp_Main_Init(void);
INTERFACE void MacroKey_Handler(void);
INTERFACE void MarcoKey_Release_Key(void);
INTERFACE void Mapp_StaticMachineEvent1(void);
INTERFACE void Mapp_StaticMachineEvent2(void);
INTERFACE void Mapp_StaticMachineEvent3(void);
INTERFACE void Mapp_StaticMachineEvent4(void);
INTERFACE void Mapp_StaticMachineEvent5(void);
INTERFACE void Mapp_StaticMachineEvent6(void);
INTERFACE void Mapp_StaticMachineEvent7(void);



extern U16 BUTTON_MAP_DEFAULT[];
extern U16 BUTTON_MAP_NORMAL[];
extern U8 USB_BUFFER[];

//***********************************************************************
//*								REGISTER								*
//***********************************************************************
extern unsigned int ACK ;
//SW
extern unsigned int SW1_Count ;
extern unsigned int SW2_Count ;
extern unsigned int SW3_Count ;
extern unsigned int SW4_Count ;
extern unsigned int SW5_Count ;
extern unsigned int SW_Status ;
//LED_Control
extern unsigned int VR_Data ;
extern unsigned int Light_Data ;
extern unsigned int LED_Mode ;
extern unsigned int Light_Per ;
extern unsigned int Light_Time ;
//I2C_Mode
extern unsigned char I2C_GetMode ;
extern unsigned char I2C_GetStatus ;
extern unsigned int I2C_Count ;
// 7 SEG Data
extern unsigned char Show_Status ;
extern unsigned int Show_Data ;
extern unsigned char Show8_Data ;
//
extern unsigned char GSOK ;
extern unsigned int MainI ;
extern unsigned char RXData ;

extern unsigned char DelayFg ;
extern unsigned int TimeDelay ;
extern unsigned char AdamWang ; 
//***********************************************************************
//*								DECLARE									*
//***********************************************************************
//extern void DrvDelay(unsigned long DelayData) ;
//extern void DrvDelay1mS(unsigned int DelayData) ;
//extern void Delayus(uint16_t u16DelayTime);
void Main_Send_Mouse(U8 KeyCode);
void Main_Release_Mouse(U8 KeyCode);
#endif
#undef INTERFACE
//***********************************************************************
//*								END										*
//***********************************************************************

//#endif //__MAIN_H__


//uint8_t buttons_scan();
void Device_Init(void);
void Device_Active(void);
void Device_Scan(void);
void Device_Pack(void);
void GPIO_Check_Key(void);

void nsAPI_Sensor_EventMointor(void);
void nsAPI_GPIO_ButtonEventMointor(void);
void Device_Button_Scan(void);
void Device_Wheel_Scan(void);
void Device_Report(void);
void DrvDelay(unsigned long DelayData) ;
void DrvDelay1mS(unsigned int DelayData) ;
void Delayus(uint16_t u16DelayTime);
void nrfr_mouse_buttons_read(uint8_t* mouse_buttons, uint8_t* mouse_pan, uint8_t* advanced_buttons,bool* cpi);
/* Bit position macros */
/*
#define BIT_0_POSITION                           (0u)
#define BIT_1_POSITION                           (1u)
#define BIT_2_POSITION                           (2u)
#define BIT_3_POSITION                           (3u)
#define BIT_4_POSITION                           (4u)
#define BIT_5_POSITION                           (5u)
#define BIT_6_POSITION                           (6u)
#define BIT_7_POSITION                           (7u)
#define BIT_8_POSITION                           (8u)
#define BIT_9_POSITION                           (9u)
#define BIT_10_POSITION                         (10u)
#define BIT_11_POSITION                         (11u)
#define BIT_12_POSITION                         (12u)
#define BIT_13_POSITION                         (13u)
#define BIT_14_POSITION                         (14u)
#define BIT_15_POSITION                         (15u)
#define BIT_16_POSITION                         (16u)
#define BIT_17_POSITION                         (17u)
#define BIT_18_POSITION                         (18u)
#define BIT_19_POSITION                         (19u)
#define BIT_20_POSITION                         (20u)
#define BIT_21_POSITION                         (21u)
#define BIT_22_POSITION                         (22u)
#define BIT_23_POSITION                         (23u)
#define BIT_24_POSITION                         (24u)
#define BIT_25_POSITION                         (25u)
#define BIT_26_POSITION                         (26u)
#define BIT_27_POSITION                         (27u)
#define BIT_28_POSITION                         (28u)
#define BIT_29_POSITION                         (29u)
#define BIT_30_POSITION                         (30u)
#define BIT_31_POSITION                         (31u)
*/
/* Bit mask macros */
/*
#define BIT_0_MASK                              ((uint32)1<<BIT_0_POSITION)
#define BIT_1_MASK                              ((uint32)1<<BIT_1_POSITION)
#define BIT_2_MASK                              ((uint32)1<<BIT_2_POSITION)
#define BIT_3_MASK                              ((uint32)1<<BIT_3_POSITION)
#define BIT_4_MASK                              ((uint32)1<<BIT_4_POSITION)
#define BIT_5_MASK                              ((uint32)1<<BIT_5_POSITION)
#define BIT_6_MASK                              ((uint32)1<<BIT_6_POSITION)
#define BIT_7_MASK                              ((uint32)1<<BIT_7_POSITION)
#define BIT_8_MASK                              ((uint32)1<<BIT_8_POSITION)
#define BIT_9_MASK                              ((uint32)1<<BIT_9_POSITION)
#define BIT_10_MASK                             ((uint32)1<<BIT_10_POSITION)
#define BIT_11_MASK                             ((uint32)1<<BIT_11_POSITION)
#define BIT_12_MASK                             ((uint32)1<<BIT_12_POSITION)
#define BIT_13_MASK                             ((uint32)1<<BIT_13_POSITION)
#define BIT_14_MASK                             ((uint32)1<<BIT_14_POSITION)
#define BIT_15_MASK                             ((uint32)1<<BIT_15_POSITION)
#define BIT_16_MASK                             ((uint32)1<<BIT_16_POSITION)
#define BIT_17_MASK                             ((uint32)1<<BIT_17_POSITION)
#define BIT_18_MASK                             ((uint32)1<<BIT_18_POSITION)
#define BIT_19_MASK                             ((uint32)1<<BIT_19_POSITION)
#define BIT_20_MASK                             ((uint32)1<<BIT_20_POSITION)
#define BIT_21_MASK                             ((uint32)1<<BIT_21_POSITION)
#define BIT_22_MASK                             ((uint32)1<<BIT_22_POSITION)
#define BIT_23_MASK                             ((uint32)1<<BIT_23_POSITION)
#define BIT_24_MASK                             ((uint32)1<<BIT_24_POSITION)
#define BIT_25_MASK                             ((uint32)1<<BIT_25_POSITION)
#define BIT_26_MASK                             ((uint32)1<<BIT_26_POSITION)
#define BIT_27_MASK                             ((uint32)1<<BIT_27_POSITION)
#define BIT_28_MASK                             ((uint32)1<<BIT_28_POSITION)
#define BIT_29_MASK                             ((uint32)1<<BIT_29_POSITION)
#define BIT_30_MASK                             ((uint32)1<<BIT_30_POSITION)
#define BIT_31_MASK                             ((uint32)1<<BIT_31_POSITION)
*/
#define MOUSE_ID 0X01
#define MOUSE_MULTIMEDIA_ID 0X05



#define			NO_EVENT		0x00
#define    KEYBOARD_EVENT		0x01
#define    BUTTON_EVENT			0x02
#define    SCROLL_EVENT			0x04
#define   OPTICAL_EVENT			0x08 
#define   LR_WHEEL_EVENT		0x10   
#define   DOUBLE_CLICK_EVENT    0x20   
#define   MACRO_DEAL_EVENT		0x40   
#define   Zoom_Out_In_Event		0x80   


enum
{
	Zoom_Phase_3	= 3,
	Zoom_Phase_2	= 2,
	Zoom_Phase_1	= 1,
	Zoom_Phase_0	= 0,
};

typedef struct
{
	BOOL press;
	UINT8 firstCnt;
	UINT8 data;
}LR_WHEEL_STRU;

typedef struct
{
	BOOL press;
	UINT8 clickCnt;
	UINT8 data;
}DOUBLE_CLICK_STRU;

typedef struct
{
	BOOL start;
	BOOL first;
	UINT8 phase;
	UINT8 type;
	UINT16 frame;
	UINT16 cycle;
}Zoom_Out_In_STRU;




extern UINT8 deviceEvent;
extern LR_WHEEL_STRU device_lrWheel;
extern UINT32 wheel_button_status;
extern UINT16 de_determineUsbAct;

extern UINT16 LED_off_cycle;
extern UINT8 LED_off_cnt;
extern UINT16 timer_1s;

/* Macro for reading and raising the event from the modules */
#define DEVICE_POST_EVENT(evt)                 { deviceEvent |= evt; }
#define DEVICE_GET_EVENT(evt)                  ( deviceEvent & evt)
#define DEVICE_CLEAR_EVENT(evt)                ( deviceEvent &= ~evt)

#define Device_Determine_Time	  800                             
//                                            

//void Device_Init(void);

//void Device_Active(void);

//void Device_Scan(void);

void Device_LrWheel_Start(UINT8 data);
void Device_LrWheel_Scan(void);
UINT8 Device_LrWheel_GetData(void);
void Device_LrWheel_Stop(void);

void Device_DoubleClick_Start(void);
void Device_DoubleClick_Scan(void);
void Device_DoubleClick_GetData(UINT8 *data);

void Device_Set_ReportRate(UINT8 reportRate);
void Device_Set_LEDOff(UINT8 led_off);

void nsAPI_SSP_Delay1us(U16 u16DelayTime);
void Device_Zoom_OutIn_Start(UINT8 type);
void Device_Zoom_OutIn_Stop(void);
void Device_Zoom_OutIn_Scan(void);
UINT8 Device_Zoom_OutIn_GetData(UINT8 *mouseBuf,UINT8 *keyboardBuf);

BOOL Device_Check_UsbActive(void);

#endif /* _device_H_ */
