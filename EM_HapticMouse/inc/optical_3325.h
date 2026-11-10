//--------------------------------------------------------------------------
//
// This module implements an interface to the Avago optical chip
// using an SPI port on the PSoC.
//
//--------------------------------------------------------------------------
//
// Copyright 2003-2006, Cypress Semiconductor Corporation.
//
// This software is owned by Cypress Semiconductor Corporation (Cypress)
// and is protected by and subject to worldwide patent protection (United
// States and foreign), United States copyright laws and international 
// treaty provisions. Cypress hereby grants to licensee a personal, 
// non-exclusive, non-transferable license to copy, use, modify, create 
// derivative works of, and compile the Cypress Source Code and derivative 
// works for the sole purpose of creating custom software in support of 
// licensee product to be used only in conjunction with a Cypress integrated 
// circuit as specified in the applicable agreement. Any reproduction, 
// modification, translation, compilation, or representation of this 
// software except as specified above is prohibited without the express 
// written permission of Cypress.
//
// Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND,EXPRESS OR IMPLIED, 
// WITH REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// Cypress reserves the right to make changes without further notice to the
// materials described herein. Cypress does not assume any liability arising
// out of the application or use of any product or circuit described herein.
// Cypress does not authorize its products for use as critical components in
// life-support systems where a malfunction or failure may reasonably be
// expected to result in significant injury to the user. The inclusion of
// Cypress� product in a life-support systems application implies that the
// manufacturer assumes all risk of such use and in doing so indemnifies
// Cypress against all charges.
//
// Use may be limited by and subject to the applicable Cypress software
// license agreement.
//
//--------------------------------------------------------------------------


#ifndef OPTICAL_3325_H
#define OPTICAL_3325_H

#define ENABLE_OPTICAL_SENSOR
//--------------------------------------
// Included files
//--------------------------------------

//------------------------------------------
#define REG_PRODUCT_ID                  0x00 // R
#define PRODUCT_ID                      0x4c //3327		0x43 //3325   //0x09//3050	//0x2A //3000
//------------------------------------------
#define REG_REVISION_ID                 0x01 // R
#define REVISION_ID                     0x01
//------------------------------------------
#define REG_MOTION                      0x02 // R
#define BURST_MOTION                    0x16 // R
#define MOTION_ST						0x80
//------------------------------------------
#define REG_DELTA_X_LO                  0x03 //R
#define REG_DELTA_X_HI                  0x04 //R
#define REG_DELTA_Y_LO                  0x05 //R
#define REG_DELTA_Y_HI                  0x06 //R
//------------------------------------------
#define REG_SQUAL						0x07 //R
#define REG_SHUT_HI						0X0C
#define REG_SHUT_LO						0X0B
//------------------------------------------
#define REG_PIX_SUM						0x08 //R
#define REG_PIX_MIN						0x0A //R
#define REG_PIX_MAX						0x09 //R
//------------------------------------------
#define REG_RESOLUTION					0x1B 
#define RESOLUTION_200					(0x04)
#define RESOLUTION_300					(0x06)
#define RESOLUTION_400					(0x09)
#define RESOLUTION_500					(0x0B)
#define RESOLUTION_600					(0x0e)
#define RESOLUTION_700					(0x10)
#define RESOLUTION_800					(0x12)
#define RESOLUTION_900					(0x15)
#define RESOLUTION_1000					(0x17)
#define RESOLUTION_1100					(0x19)
#define RESOLUTION_1200					(0x1B)
#define RESOLUTION_1300					(0x1D)
#define RESOLUTION_1400					(0x20)
#define RESOLUTION_1500					(0x22)
#define RESOLUTION_1600					(0x24)
#define RESOLUTION_1700					(0x27)
#define RESOLUTION_1800					(0x29)
#define RESOLUTION_1900					(0x2B)
#define RESOLUTION_2000					(0x2E)
#define RESOLUTION_2100					(0x30)
#define RESOLUTION_2200					(0x32)
#define RESOLUTION_2300					(0x34)
#define RESOLUTION_2400					(0x37)
#define RESOLUTION_2500					(0x39)
#define RESOLUTION_2600					(0x3B)
#define RESOLUTION_2700					(0x3E)
#define RESOLUTION_2800					(0x40)
#define RESOLUTION_2900					(0x42)
#define RESOLUTION_3000					(0x45)
#define RESOLUTION_3100					(0x48)
#define RESOLUTION_3200					(0x4a)
#define RESOLUTION_3300					(0x4d)
#define RESOLUTION_3400					(0x4E)
#define RESOLUTION_3500					(0x51)
#define RESOLUTION_3600					(0x54)
#define RESOLUTION_3700					(0x55)
#define RESOLUTION_3800					(0x58)
#define RESOLUTION_3900					(0x5b)
#define RESOLUTION_4000					(0x5d)
#define RESOLUTION_4100					(0x5f)
#define RESOLUTION_4200					(0x62)
#define RESOLUTION_4300					(0x64)
#define RESOLUTION_4400					(0x66)
#define RESOLUTION_4500					(0x69)
#define RESOLUTION_4600					(0x6b)
#define RESOLUTION_4700					(0x6d)
#define RESOLUTION_4800					(0x70)
#define RESOLUTION_4900					(0x72)
#define RESOLUTION_5000					(0x74)
#define RESOLUTION_5100					(0x77)
#define RESOLUTION_5200					(0x79)
#define RESOLUTION_5300					(0x7b)
#define RESOLUTION_5400					(0x7e)
#define RESOLUTION_5500					(0x80)
#define RESOLUTION_5600					(0x82)
#define RESOLUTION_5700					(0x85)
#define RESOLUTION_5800					(0x87)
#define RESOLUTION_5900					(0x89)
#define RESOLUTION_6000					(0x8c)
#define RESOLUTION_6100					(0x8e)
#define RESOLUTION_6200					(0x91)
//------------------------------------------
#define REG_DELTA_XY_HIGH				0x0c
#define X_HIGH_MASK                     0xf0
#define Y_HIGH_MASK						0x0f
//------------------------------------------
#define REG_POWER_UP_RESET				0x3a 
#define POWER_UP_VALUE					0x5a
#define RESET_VALUE						0x5b
#define REG_SHUTDOWN					0x3b
#define SHUTDOWN_VALUE					0xb6
//------------------------------------------
#define REG_BURST_MOTION_READ			0x16

//-------------------------------------------
#define AUTO_LED_CTRL					0x43
#define LED_HI_15mA						0x00
#define LED_HI_20mA						(0x01<<2)
#define LED_HI_30mA						(0x02<<2)//default
#define LED_HI_36mA						(0x03<<2)
#define LED_LO_15mA						(0x00)//default
#define LED_LO_20mA                     (0x01<<1)
#define A_LED_DISABLE				    0x01
#define A_LED_ENABLE					0x00
//-------------------------------------------
#define MAN_LED_CTRL					0x40
#define LED_MAN_NORMAL					0x00
#define LED_MAN_OFF						0x08
#define LED_MAN_ON						0x04
#define LED_MAN_20mA					0x00
#define LED_MAN_15mA					0x01
#define LED_MAN_36mA					0x02
#define LED_MAN_30mA					0x03	
//-------------------------------------------
#define NAV_CTRL2						0x22
#define F_AWAKE_ENABLE					0X00
#define F_AWAKE_DISABLE					0X80
//#define MOTSWK_LOW_PULSE_WAKEUP
#define DPI_SETTING_1	0
#define DPI_SETTING_2	1
#define DPI_SETTING_3	2
#define DPI_SETTING_4	3
#define DPI_SETTING_1000 DPI_SETTING_1	
#define DPI_SETTING_1500 DPI_SETTING_2	
#define DPI_SETTING_2000 DPI_SETTING_4	

#define O_NCS1_MASK    0x80
#define MOUSE_OPTICAL_nCS_MASK  	O_NCS1_MASK
#define MOUSE_OPTICAL_nCS_PORT  	O_NCS1_Data_ADDR
#define MOUSE_OPTICAL_nCS_SHDW		O_NCS1_DataShadow
#define MOUSE_OPTICAL_MOTION_MASK 	O_MOTION_1_MASK
#define MOUSE_OPTICAL_PORT			O_MOTION_1_Data_ADDR
						
#define GPIO_ISR_OPTICAL_IE_PORT     O_MOTION_1_IntEn_ADDR
#define GPIO_ISR_OPTICAL_INT         O_MOTION_1_MASK 
#define	MOTION_PIN_STATUS			Optical_MOT_Read()
#define Burst_Motion_Index                  (0x00)    
//--------------------------------------
// API Definitions and Types
//--------------------------------------
typedef struct
{
	unsigned char x_lo;
	unsigned char y_lo;
	unsigned char x_hi;
	unsigned char y_hi;
}OPTICAL_REPORT;
	
typedef struct
{
	float x_lo;
	float y_lo;
	float x_hi;
	float y_hi;
}OPTICAL_DECIMAL_REPORT;

typedef struct
{
	float x_lo;
	float y_lo;
	float x_hi;
	float y_hi;
}OPTICAL_FLOAT_REPORT;

typedef union
{
	struct{
		unsigned char hi;
		unsigned char lo;
	}b;
	int w;
}W_n_B_CONVERT;

#define optical_spi_put( addr, data )  SPI_Write((addr ), (data))//SPI_Write( (addr | 0x80), (data) )
#define optical_spi_get( addr )        SPI_Read (addr)

extern OPTICAL_REPORT optical_report;
extern unsigned char optical_initilized;
extern unsigned char optical_double_flag;
extern const unsigned char dpi_value_array[79];

//--------------------------------------
// API Function Declarations
//--------------------------------------
extern void optical_init(void);
//extern unsigned char optical_get_report(OPTICAL_REPORT* optical_report);
extern unsigned char optical_get_report( W_n_B_CONVERT *x, W_n_B_CONVERT *y);
extern void optical_power_rest(void);
extern void optical_power_down(void);
extern void OP_3325_Packet(W_n_B_CONVERT * x,W_n_B_CONVERT *y,UINT8 *buffer);
//extern void optical_set_dpi(void);
extern void optical_set_dpi(UINT8 optical_configration);
#endif   // OPTICAL_H



