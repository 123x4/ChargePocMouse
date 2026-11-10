#ifndef __Timer_H__
#define __Timer_H__
#include "gloab.h"
#include "led1.h"
#include "custom.h"
//***********************************************************************
//*                             INCLUDE                                 *
//***********************************************************************

//***********************************************************************
//*                             DEFINE                                  *
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 CT16B0                          ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define TIME_REAL_HZ            //((MCU_OSC/(COUNT+1))/(PR+1))
#define TIME_CT16B0_COUNT       999									//0 To 999 = 1000
#define TIME_CT16B0_PR          47									//0 To 47 = 48
#define TIME_CT16B1_COUNT       9	                                //0 To 9 = 10
#define TIME_CT16B1_PR          47                                  //0 To 47 = 48
#define TIME_CT32B0_COUNT       199999								//0 To 199999 = 200000
#define TIME_CT32B0_PR          0									//0 To 0 = 1
#define TIME_CT32B1_COUNT       199999								//0 To 199999 = 200000
#define TIME_CT32B1_PR          0									//0 To 0 = 1
//***********************************************************************
//*                             REGISTER                                *
//***********************************************************************
extern unsigned char TimerCount ;
extern UINT8 optical_scan_cycle;
//***********************************************************************
//*                             DECLARE                                 *
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Init                            ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern void InitCT16B0(unsigned int PR ,unsigned int Time) ;
extern void InitCT16B1(unsigned int PR ,unsigned int Time) ;
void CT16B1_DelayUs(unsigned short us);
extern void InitCT32B0(unsigned int PR ,unsigned int Time) ;
extern void InitCT32B1(unsigned int PR ,unsigned int Time) ;

void PWM_Config(uint32_t pwm_frc );
void PWM_set_duty( RGB_STRU rgb, UINT8 led_mode);
void PWM_Enable(void);
//void PWM_Disable(void);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Drv                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//***********************************************************************
//*                             END                                     *
//***********************************************************************
#endif
