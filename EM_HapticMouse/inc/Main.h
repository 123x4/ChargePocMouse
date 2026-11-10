#ifndef __MAIN_H__
#define __MAIN_H__
////***********************************************************************
////*								INCLUDE									*
////***********************************************************************
//#include "../inc/device.h"
#include "device.h"

#endif
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

//#ifdef _MAIN_C_
//#define INTERFACE
//#else
//#define INTERFACE extern
//#endif
////***********************************************************************
////*								DEFINE									*
////***********************************************************************
//#define NRFR_REPORT_ID  0

//#define NRFR_MOUSE_BUTTONS (NRFR_REPORT_ID + 1)

///**
//Byte 2 in mouse packet, ID 1, is scroll wheel.
//*/
//#define NRFR_MOUSE_WHEEL (NRFR_MOUSE_BUTTONS + 1)

///**
//Byte 3 in mouse packet, ID 1, is "pan".
//*/
//#define NRFR_MOUSE_PAN (NRFR_MOUSE_WHEEL + 1)
//#define NRFR_ADVANCED_BUTTONS (NRFR_REPORT_ID + 1)
//#define NRFR_ADVANCED_PAYLOAD_LENGTH (NRFR_ADVANCED_BUTTONS + 1)


//#define SIM_MOUSE_SENSOR        0//1

//#define ENABLE_UART             1

//#define SUPPORT_MAC_BOOT_AND_DOS_MODE   1

//#define SPIMODE                 0

//#define DELAY_COUNT             (368*(MCU_OSC/48000000))            //100ns

//#define SW_DELAY                250                                 //250mS
//#define I2C_CHANGTIME           330
//#define LIGHT_MIN_PER           1                                   //% Unit
//#define LIGHT_MAX_PER           100                                 //% unit

////Keyboard define
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

//// USB define 
//#define EP1_IN BIT0
//#define EP1_OUT BIT1
//#define EP2_IN BIT2
//#define EP2_OUT BIT3
//#define EP3_IN BIT4
//#define EP3_OUT BIT5
//#define EP4_IN BIT6
//#define EP4_OUT BIT7



//typedef enum _STATIC_MACHINE_EVENT1_TYPE_
//{
//	SM_EVENT1_IDLE = 0, 	
//	SM_EVENT1_MOUSE_MONITOR,	
//	SM_EVENT1_KEYBOARD_MONITOR,	
//	SM_EVENT1_TYPE_NUMS
//}STATIC_MACHINE_EVENT1_TYPE;

//typedef enum _STATIC_MACHINE_EVENT2_TYPE_
//{
//	SM_EVENT2_IDLE = 0, 	
//	SM_EVENT2_BUTTON_HANDLER,
//	SM_EVENT2_TYPE_NUMS
//}STATIC_MACHINE_EVENT2_TYPE;

//typedef enum _STATIC_MACHINE_EVENT3_TYPE_
//{
//	SM_EVENT3_IDLE = 0, 	
//	SM_EVENT3_MACRO_HANDLER,
//	SM_EVENT3_TYPE_NUMS
//}STATIC_MACHINE_EVENT3_TYPE;

//typedef enum _STATIC_MACHINE_EVENT4_TYPE_		//Set request
//{
//	SM_EVENT4_IDLE = 0,
//	SM_EVENT4_GET_FIRMWAVE_VERSION =0x41,
//	SM_EVENT4_FIRMWARE_UPDATE = 0x51,	
//	SM_EVENT4_ERASE_FLASH = 0x52,
//	SM_EVENT4_WRITE_FLASH = 0x53

//}STATIC_MACHINE_EVENT4_TYPE;

//typedef enum _STATIC_MACHINE_EVENT5_TYPE_
//{
//	SM_EVENT5_IDLE = 0, 	
//	SM_EVENT5_GET_FIRMWAVE_VERSION =0x41,
//  SM_EVENT5_FIRMWARE_UPDATE = 0x51,	
//  SM_EVENT5_ERASE_FLASH = 0x52,
//  SM_EVENT5_WRITE_FLASH = 0x53
//}STATIC_MACHINE_EVENT5_TYPE;


//INTERFACE STATIC_MACHINE_EVENT1_TYPE g_StaticMachineEvent1Type;		//Sensor Key monitor
//INTERFACE STATIC_MACHINE_EVENT2_TYPE g_StaticMachineEvent2Type;		//Key parser
//INTERFACE STATIC_MACHINE_EVENT3_TYPE g_StaticMachineEvent3Type;		//Macro key
//#ifdef M_EVENT4_5
//INTERFACE STATIC_MACHINE_EVENT4_TYPE g_StaticMachineEvent4Type;		//Set report
//INTERFACE STATIC_MACHINE_EVENT5_TYPE g_StaticMachineEvent5Type;		//Get report
//#endif
//#ifdef M_EVENT6_7
//INTERFACE STATIC_MACHINE_EVENT6_TYPE g_StaticMachineEvent6Type;		//Set report
//INTERFACE STATIC_MACHINE_EVENT7_TYPE g_StaticMachineEvent7Type;		//Get report
//#endif


//typedef enum _GIndex_
//{
//    GIdx0 = 0,
//    GIdx1,
//    GIdx2,
//    GIdx3,
//    GIdx4,
//    GIdx5,
//    GIdx6,
//    GIdx7,
//    GIdx8,
//    GIdxLen =  GIdx8,
//}GIndex;

//typedef enum _PROFILEIDX_
//{
//	PROFILE_DEFAULT=0,
//	PROFILE_1,
//	PROFILE_2,
//	PROFILE_3,
//	PROFILE_ALL,
//	LED_EFFECT_ALWAYS=0,
//	LED_EFFECT_BREATH,
//	LED_EFFECT_COLOR_CYCLE,
//	LED_EFFECT_RANDON,
//	LED_REGION_LOGO=0,
//	LED_REGION_WHEEL,
//	LED_REGION_SIDE,
//	LED_REGION_ALL
//}PROFILEIDX;

//#define MAIN_FLAG_MOUSE_QIE_IN_REPORT BIT1
//#define MAIN_FLAG_MOUSE_SENSOR_IN_REPORT BIT2
//#define MAIN_FLAG_MOUSE_SENSOR_FRAME_CAPTURE_EVENT BIT3

////2016_0411_Hsiang
//#define MAIN_FLAG_MOUSE_REPORT_READY	BIT0	
//#define MAIN_FLAG_MOUSE_BUTTON_EVENT	BIT1	
//#define MAIN_FLAG_MOUSE_QIE_EVENT		BIT2
//#define MAIN_FLAG_MOUSE_EVENT			BIT3


///* HID Demo Variables */
//#define MOUSE_IN_REPORT_SIZE 6
//#define MEDIA_IN_REPORT_SIZE 8
//#define SYSTEM_IN_REPORT_SIZE	1
//#define KEYBOARD_IN_REPORT_SIZE 2//9					//EP2
//#define SHORTCUT_IN_REPORT_SIZE 8

//#define CONSUMER_IN_REPORT_SIZE 3 //EP2			
//#define CUSTOMER_IN_REPORT_SIZE 64 //EP3		
//#define CUSTOMER_OUT_REPORT_SIZE 64 //EP3  

//#define FWUPDATE_IN_REPORT_SIZE 64  //EP4
//#define FWUPDATE_OUT_REPORT_SIZE 64 //EP4
//#define MACRO_PRESS_ANY			0
//#define MACRO_RELEASE_KEY		1


//#define MAX_PROFILE	3
//#define MAX_MARCO	0xFFFF
//#define EEPROM_SIZE 0x1000
//#define EERPOM_CHECK_ADDR 4030



//#define KEY_EFFECT_CHOOSE 0x28


////#define P503 1
//	
//typedef __packed struct _LPC11Uxx_MAIN_SETTING_
//{	
//	U8 MouseInReport[MOUSE_IN_REPORT_SIZE];
//	U8 MediaInReport[MEDIA_IN_REPORT_SIZE];
//	U8 SystemInReport[SYSTEM_IN_REPORT_SIZE];
//	U8 KeyboardInReport[KEYBOARD_IN_REPORT_SIZE];
//	U8 ShortcutInReport[SHORTCUT_IN_REPORT_SIZE];
//	U8 ConsumerInReport[CONSUMER_IN_REPORT_SIZE];

//	U8 FeatureInReport[16];	
//	//U8 FeatureInReport[MOUSE_IN_REPORT_SIZE];
//	//U8 FeatureInReport[USB_PACKET_SIZE];
//	U8 InReport[CUSTOMER_IN_REPORT_SIZE];
//	U8 OutReport[CUSTOMER_OUT_REPORT_SIZE];
//  U8 FirmwareUpdateInReport[FWUPDATE_IN_REPORT_SIZE];
//  U8 FirmwareUpdateOutReport[FWUPDATE_OUT_REPORT_SIZE];
//	//U8 InReportSize;
//	U16 EPInOutReq;
//	//U8  EP0_SetReq;
//	//U8  EP0_GetReq;
//	U8 InReportSize;						//2016_0411_Hsiang
//	U16 MouseEvent;						//2016_0411_Hsiang
//	U16 Total_Size;
//	U16 ADR_Offset;
//	U8 SensorReset;
//	U8 MacroStopMode;
//   	 U8 OS_ModeType;
//}LPC11Uxx_MAIN_SETTING;

//INTERFACE LPC11Uxx_MAIN_SETTING g_Main_Setting;

//typedef struct _SENSOR_DATA_
//{
//	U8	X1;
//	U8	X2;
//	U8	Y1;
//	U8	Y2;
//	U8	Changed;
//}SENSOR_DATA;

//typedef struct _REPROT_PROCESS_
//{
//	U8	Mouse_Report_In;
//	U8	Media_Report_In;
//	U8	System_Report_In;
//	U8	Keyboard_Report_In;
//	U8	Macro_Keyboard_Report_In;
//	U8 	Shortcut_Report_In;
//	U8	Consumer_Report_In;

//	U8	Feature_Report_In;
//  U8  FirmwareUpdate_Report_In;
//}REPORT_PROCESS;

//typedef struct _KEY_MATRIX_ATTR_
//{
//	U16 KeyDelay;
//	U8	fKeyDelay;
//	U8 KeyAction;
//	U8 KeyTableNum;
//	U8 KeyLoop;
//	U8 DecKeyLoop;
//}KEY_MATRIX_ATTR;

//typedef struct _SET_MATRIX_A_
//{
//	U8	Key_Group;
//	U8	Action;
//	U8	M_Data_Byte;
//}SET_MATRIX_A;

//typedef struct _SET_TEMP_INSTANT_
//{
//	U8 u8InstantProfile;
//	U8 u8InstantKyeNum;
//	U8 u8InstantKyeEnable;
//}SET_TEMP_INSTANT;




//INTERFACE SET_MATRIX_A	g_Set_Matrix;
//INTERFACE SENSOR_DATA	g_Sensor_Data;
//INTERFACE REPORT_PROCESS	g_Repot_Process;
//INTERFACE KEY_MATRIX_ATTR	g_Key_Matrix_Attr;
//INTERFACE SET_TEMP_INSTANT  g_Instant_Key;


///* HID Demo Functions */
//INTERFACE void nsAPI_Main_FirmwareUpdateInReportHandler(void);
//INTERFACE void nsAPI_Main_GetInReport(void);
//INTERFACE void nsAPI_Main_SetOutReport(void); 
//INTERFACE void nsAPI_Main_MouseInReportHandler(void);
//INTERFACE void nsAPI_Main_MediaInReportHandler(void);
//INTERFACE void nsAPI_Main_SystemInReportHandler(void);
//INTERFACE void nsAPI_Main_KeyboardInReportHandler(void);
//INTERFACE void nsAPI_Main_ShortcutInReportHandler(void);
//INTERFACE void nsAPI_Main_MouseButtonEventHandler(void);
//INTERFACE void nsAPI_Main_KeyboardButtonEventHandler(void);
//INTERFACE void nsAPI_Main_GetInMouseQIEReport(S8 s8Temp);
//INTERFACE void nsAPI_Main_Sensor_InReport(U8 * u8pTemp);
//INTERFACE void nsAPI_Main_ConsumerInReportHandler(void);
//INTERFACE void nsAPI_Main_Clr_Mouse_Event(void);
//INTERFACE void nsAPI_Main_Clr_EP1Event(void);
//INTERFACE void nsAPI_Main_Clr_KeyboardEvent(void);
//INTERFACE void nsAPI_Main_Clr_ConsumerEvent(void);
//INTERFACE void nsAPI_Main_Clr_EP3Event(void);
//INTERFACE void nsAPI_Main_Clr_EP4Event(void);


//INTERFACE void Mapp_Main_Init(void);
//INTERFACE void MacroKey_Handler(void);
//INTERFACE void MarcoKey_Release_Key(void);
//INTERFACE void Mapp_StaticMachineEvent1(void);
//INTERFACE void Mapp_StaticMachineEvent2(void);
//INTERFACE void Mapp_StaticMachineEvent3(void);
//INTERFACE void Mapp_StaticMachineEvent4(void);
//INTERFACE void Mapp_StaticMachineEvent5(void);
//INTERFACE void Mapp_StaticMachineEvent6(void);
//INTERFACE void Mapp_StaticMachineEvent7(void);



//extern U16 BUTTON_MAP_DEFAULT[];
//extern U16 BUTTON_MAP_NORMAL[];
//extern U8 USB_BUFFER[];

////***********************************************************************
////*								REGISTER								*
////***********************************************************************
//extern unsigned int ACK ;
////SW
//extern unsigned int SW1_Count ;
//extern unsigned int SW2_Count ;
//extern unsigned int SW3_Count ;
//extern unsigned int SW4_Count ;
//extern unsigned int SW5_Count ;
//extern unsigned int SW_Status ;
////LED_Control
//extern unsigned int VR_Data ;
//extern unsigned int Light_Data ;
//extern unsigned int LED_Mode ;
//extern unsigned int Light_Per ;
//extern unsigned int Light_Time ;
////I2C_Mode
//extern unsigned char I2C_GetMode ;
//extern unsigned char I2C_GetStatus ;
//extern unsigned int I2C_Count ;
//// 7 SEG Data
//extern unsigned char Show_Status ;
//extern unsigned int Show_Data ;
//extern unsigned char Show8_Data ;
////
//extern unsigned char GSOK ;
//extern unsigned int MainI ;
//extern unsigned char RXData ;

//extern unsigned char DelayFg ;
//extern unsigned int TimeDelay ;
//extern unsigned char AdamWang ; 
////***********************************************************************
////*								DECLARE									*
////***********************************************************************
//extern void DrvDelay(unsigned long DelayData) ;
//extern void DrvDelay1mS(unsigned int DelayData) ;
//extern void Delayus(uint16_t u16DelayTime);
//void Main_Send_Mouse(U8 KeyCode);
//void Main_Release_Mouse(U8 KeyCode);
//#undef INTERFACE
////***********************************************************************
////*								END										*
////***********************************************************************

//#endif //__MAIN_H__
