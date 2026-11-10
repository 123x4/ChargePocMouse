/*****************************************************************************
* File Name: optical.h
*
* Version: 1.0
*
* Description:
* This file contains definitions for custom 2 wire communicaiton implementation for optical sensor
*
* Note:
*
*
* Related Document:
* 
*
* Hardware Dependency:
* 
*
* Code Tested With:
* 1. PSoC Creator 3.3 
* 2. ARM GCC 4.9.3
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
#ifndef _OPTICAL_H_
#define _OPTICAL_H_

//#include "platform.h"
//#include "device.h"
//#include "gloab.h"
//#include "spi_hardware.h"
//#include "../inc/device.h"
//  #include "pc_communicate.h"  

//* 1 Lift cut off calibration SROM: SROM 0x81 or above (4KB).
//* 2 Tracking SROM: SROM 0x03 or above (4KB
#define TRACKING_SROM_VER	0X05
#define LiftCutOff_SROM_VER	0X81


#define  OP_PixThresh_Init 0x0a
#define  OP_Min_Sq_Init 0X10


#define ENABLE_BURST_READ

/*Macros for optical sensor PAW3360 initialization */
#define Product_ID  	                    (0x00)  /* R 0x42 number with the product identifier */
    #define Product_ID_Value                    (0x47)
	#define PRODUCT_ID                          (0x47) 
#define Revision_ID 	                    (0x01)  /* R IC Revision */					 	
#define Motion 	                            (0x20)  /* RW - Bit field */
    #define MOTION_DETECT                       (0x80)  /* 1-have motion */
  
#define Delta_XL 		                    (0x03)  /* R - delta X lower 8bit */
#define Delta_XH 		                    (0x04)  /* R - delta X upper 8bit */
#define Delta_YL 		                    (0x05)  /* R - delta Y lower 8bit */
#define Delta_YH 		                    (0x06)  /* R - delta Y upper 8bit */
#define SQUAL                               (0x07)  /* R - Surface quality register */
#define Pixel_Sum                           (0x08)  /* R - Used to find the average pixel value. */
#define Maximum_Pixel                       (0x09)  /* R - Maximum Pixel value in current frame. */
#define Minimum_Pixel                       (0x0A)  /* R - Minimum Pixel value in current frame */
#define Shutter_Lower                       (0x0B)  /* R - Lower 8 bit.The shutter is adjusted to keep the average pixel values within normal operating ranges */
#define Shutter_Upper                       (0x0C)  /* R - Upper 8 bit */
#define Control                             (0x0D)  /* RW - This register defines programmable invert able of XY register scheme */
#define SROM_ENABLE							            (0x13)
#define Resolution_L                        (0x0E)
#define Resolution_H                        (0x0F)
//#define Config1                             (0x0F)  /* RW - Config register 1, resolution of X,Y/Y */
#define Config2                             (0x10)  /* RW - Config register 2 */
#define Rest_En                             (0x20)  /* bit5, 1-Enable to Rest Mode */ 
#define Rpt_Mod                             (0x04)  /* bit2, Select the X and Y CPI reporting mode, 0 - CPI X=Y; 1 - CPI Y= Config1,CPI X=Config5 */ 
#define Config5_L                           (0x2E) 
#define Config5_H                           (0x2F)  /* RW - Config register 5, resolution of X, Rpt_Mod = 1 */
#define Power_Up_Reset                      (0x3A)  /* W - Command register to softreset sensor */  
#define Shutdown                            (0x3B)  /* W - Command register to shutdown sensor */    
#define Motion_Burst                        (0x50)  /* R start address of the motion burst read mode */
#define SROM_Load_Burst						(0x62)
#define SROM_ID  							(0x2A)  
    
/* Macro of Motion Burst data Index */ 
#define Burst_Motion_Index                  (0x00)    
#define Burst_Observation_Index             (0x01)    
#define Burst_Delta_X_L_Index               (0x02)    
#define Burst_Delta_X_H_Index               (0x03)    
#define Burst_Delta_Y_L_Index               (0x04)    
#define Burst_Delta_Y_H_Index               (0x05)    
#define Burst_SQUAL_Index                   (0x06)    
#define Burst_Pixel_Sum_Index               (0x07)    
#define Burst_Maximum_Pixel_Index           (0x08)    
#define Burst_Minimum_Pixel_Index           (0x09)    
#define Burst_Shutter_Upper_Index           (0x0a)    
#define Burst_Shutter_Lower_Index           (0x0b)    


/*Macros for optical sensor PAW3360 Lift Cut Off */
#define  LiftCuttoff_Tune1 0x4a
	// Calibration return bits in HEX
	#define CAL_INPROGRESS 0x81
	#define CAL_SUCCESS_1 0x82
	#define CAL_SUCCESS_2 0x83
	#define CAL_FAIL  0x84
	// Cal bits
	#define RUN_CAL_EN  0x80
	#define RUN_DISABLE  0x00                    
                                                    
#define MIN_SQ_RUN_ADDR  0x2B
#define PIXEL_THRESHOLD_ADDR 0x2C

#define LCO_TUNE2_ADDR 0x65
#define LCO_TUNE3_ADDR 0x41

typedef union
{
	struct{
		unsigned char hi;
		unsigned char lo;
	}b;
	int w;
}W_TO_B_CONVERT;

typedef struct
{
	BOOL start;
	BOOL identify_desktop;
	BOOL running;

}OP_liftCut_Stru;

extern const unsigned char tracking_SROM[];
extern const unsigned char liftCutOff_calibration_SROM[];


#define DPI_800		0X07
//#define DPI_1500	0X0E
//#define DPI_3200	0X1F
#define DPI_1600	0X0F
#define DPI_4000	0X27
#define DPI_6400	0X3F
#define DPI_12000	0X77

enum{
	OP_H_Enter  = 0, 
	OP_H_Start = 1,
	OP_H_Stop = 2,
	OP_H_Quit = 3,
	OP_H_Determine = 4,
};


void Optical_Init(void);

void Optical_Stop(void);

BOOL Optical_Read(W_TO_B_CONVERT *x,W_TO_B_CONVERT *y);

BOOL Optical_Scan(void);

void Optical_Packet(W_TO_B_CONVERT * x,W_TO_B_CONVERT *y,UINT8 *buffer);

void Optical_Check_ID(void);

BOOL OP_LiftCutOff_Enter(void);
void Optical_LiftCutOff_Cal_Scan(void);
void Optical_LiftCutOff_Set(UINT8 pix_thresh,UINT8 min_sq);


unsigned short Get_SROM_0X04_Size(void);
unsigned short Get_SROM_0X81_Size(void);

void Optical_Set_DPI(UINT8 x,UINT8 y);

void Op_read(void);

void OP_Set_LiftOff(void);
extern uint16_t Get_SROM_0X04_Size(void);

#define	Write_Sensor(Addr,Data)		SPI_Write(Addr,Data)
#define	Read_Sensor(Addr)			SPI_Read(Addr)

#define	MOTION_PIN_STATUS			Optical_MOT_Read()

void OP_LiftCutOff(UINT8 type);

extern UINT8 pixThresh_back;
extern UINT8 min_sq_back;
extern UINT8 receive_buff[TRANSFER_LENGTH];
extern OP_liftCut_Stru OP_liftCut_run; 
void TimerDelayMsec(uint32_t t_count);
BOOL OP_HeightRunning_Get(void);
void OP_HeightRunning_Set(void);
void OP_HeightRunning_Clear(void);
void Sensor_Shutdown(void);

#endif  



