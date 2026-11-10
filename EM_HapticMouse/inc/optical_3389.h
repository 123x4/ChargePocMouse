#ifndef _OPTICAL_3389_H_
#define _OPTICAL_3389_H_

//#include "platform.h"
#include "device.h"
#include "gloab.h"
#include "spi.h"

   
#define OP_3389_Enable	
#ifdef OP_3389_Enable   






#define  OP_PixThresh_Init 0x0a
#define  OP_Min_Sq_Init 0X10

#define OP_3389_SROM_Ver	0X05

#define ENABLE_BURST_READ

/*Macros for optical sensor PAW3360 initialization */
#define Product_ID  	                    (0x00)  /* R 0x47 number with the product identifier */
    #define Product_ID_Value                    (0x47)

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


#define Config2                             (0x10)  /* RW - Config register 2 */
#define Rest_En                             (0x20)  /* bit5, 1-Enable to Rest Mode */ 
#define Rpt_Mod                             (0x04)  /* bit2, Select the X and Y CPI reporting mode, 0 - CPI X=Y; 1 - CPI Y= Config1,CPI X=Config5 */ 

#define SROM_ENABLE							(0x13)

#define Resolution_L                        (0x0E)  /* RW - Config register 1, resolution of X,Y/Y */
#define Resolution_H                        (0x0F)  /* RW - Config register 1, resolution of X,Y/Y */

#define Config5_L							(0X2E)
#define Config5_H							(0X2F)


#define Config5                             (0x2F)  /* RW - Config register 5, resolution of X, Rpt_Mod = 1 */
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
#define  LiftCuttoff_Cla1 0x4a
	// Calibration return bits in HEX
	#define CAL_INPROGRESS 0x81
	#define CAL_SUCCESS_1 0x82
	#define CAL_SUCCESS_2 0x83
	#define CAL_FAIL  0x84
	// Cal bits
	#define RUN_CAL_EN  0x80
	#define RUN_DISABLE  0x00                    
                                                    
#define Min_SQ_Run  0x2B
#define RawData_Threshold 0x2C

#define LiftCutoff_Cal3 0x41
#define LiftCutoff_Cal2 0x65
#define LiftCutoff_Cal1 0x4A

#define R_CPI_MIN	0x00	 //0	
#define R_CPI_MAX	0x0140	//16000

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

extern const unsigned char OP_3389_SROM_0X05[];

extern UINT8 pixThresh_back;
extern UINT8 min_sq_back;

extern OP_liftCut_Stru OP_liftCut_run; 
#define DPI_100   0x0002
#define DPI_200   0x0004
#define DPI_400		0x0008//0X08
#define DPI_800		0X0010
#define DPI_1000  0x0014
#define DPI_1200  0x0018
#define DPI_1600	0X0020
#define DPI_2000  0x0028
#define DPI_2400  0x0030
#define DPI_3000  0x003C
#define DPI_3200  0x0040
#define DPI_4000	0X0050
#define DPI_5000  0x0064
#define DPI_6400  0x0080
#define DPI_8000  0x00A0
#define DPI_9000  0x00B4
#define DPI_12000 0x00F0
#define DPI_16000	0X0140

//********************************************************************************************

//********************************************************************************************
// CPI value in decimal
#define CPI_CLICK_DURATION		200	   // 15/10/2014 
//#define UGS2_NON_MOUSE_VERSION

#define CPI_LO_0			0x10		  // 800CPI
#define CPI_LO_1			0x18		  // 1200CPI
#define CPI_LO_2			0x23		  // 1800CPI(default)
#define CPI_LO_3			0x45		  // 3500CPI
#define CPI_LO_4			0x62		  // 5000CPI
#define CPI_LO_5			0x9D		  // 8000CPI
#define CPI_LO_6			0xEB		  // 12000CPI
#define CPI_LO_7			0x3A		  // 16000CPI

#define CPI_HI_0			0x00		  // 800CPI
#define CPI_HI_1			0x00		  // 1200CPI
#define CPI_HI_2			0x00		  // 1800CPI(default)
#define CPI_HI_3			0x00		  // 3500CPI
#define CPI_HI_4			0x00		  // 5000CPI
#define CPI_HI_5			0x00		  // 8000CPI
#define CPI_HI_6			0x00		  // 12000CPI
#define CPI_HI_7			0x01		  // 16000CPI

enum{
	OP_H_Enter  = 0, 
	OP_H_Start = 1,
	OP_H_Stop = 2,
	OP_H_Quit = 3,
	OP_H_Determine = 4,
};



unsigned short OP_3389_SROM_0X05_GetSize(void);

#define RUN_ALL_SROM	

#define	Write_Sensor(Addr,Data)		SPI_Write(Addr,Data)
#define	Read_Sensor(Addr)			SPI_Read(Addr)

#define	MOTION_PIN_STATUS			Optical_MOT_Read()

void OP_3389_Init(void);
BOOL OP_3389_Read(W_TO_B_CONVERT *x,W_TO_B_CONVERT *y);
BOOL OP_3389_Scan(void);
BOOL OP_HeightRunning_Get(void);
void OP_3389_Packet(W_TO_B_CONVERT * x,W_TO_B_CONVERT *y,UINT8 *buffer);
void OP_3389_Check_ID(void);
void OP_3389_Set_DPI(UINT16 x,UINT16 y);
void OP_3389_Shutdown(void);



void OP_LiftCutOff(UINT8 type);

BOOL OP_LiftCutOff_Enter(void);
void Optical_LiftCutOff_Cal_Scan(void);
void Optical_LiftCutOff_Set(UINT8 pix_thresh,UINT8 min_sq);
void OP_HeightRunning_Clear(void);
void OP_HeightRunning_Set(void);
void OP_Set_LiftOff(void);

#endif  

#endif
