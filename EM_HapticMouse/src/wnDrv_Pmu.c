/*----------------------------------------------------------------------------
*      Name : wnDrv_Pmu.c
*      Note(s):
*----------------------------------------------------------------------------
* PLEASE READ THIS AS IS CLAUSE CAREFULLY. By accessing and using the Source Code, you accept this AS IS CLAUSE in 
* its entirety and agree to only use the Source Code in accordance with the *following terms and conditions. 
* If you do not wish to be bound by these terms and conditions, do not access or use the Source Code.

* The Source Code is provided to you "AS IS". YOU ASSUME TOTAL RESPONSIBILITY AND RISK FOR YOUR USE OF IT INCLUDING 
* THE RISK OF ANY DEFECTS OR INACCURACIES THEREIN. WPI DOES NOT *MAKE, AND EXPRESSLY DISCLAIMS, ANY EXPRESS OR IMPLIED
* WARRANTIES OF ANY KIND WHATSOEVER, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY OR 
* FITNESS FOR A PARTICULAR PURPOSE, WARRANTIES OF TITLE OR NON-INFRINGEMENT OF ANY IPR OR TRADEMARK RIGHTS, 
* ANY WARRANTIES ARISING BY USAGE OF TRADE, COURSE OF DEALING *OR COURSE OF PERFORMANCE, 
* OR ANY WARRANTY THAT THE SOURCE CODE IS "ERROR FREE" OR WILL MEET YOUR REQUIREMENTS.

* IN NO EVENT SHALL WPI BE LIABLE FOR (a) ANY INCIDENTAL, CONSEQUENTIAL, OR INDIRECT DAMAGES (INCLUDING, 
* WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS *INTERRUPTION, LOSS OF PROGRAMS OR INFORMATION,
* AND THE LIKE) ARISING OUT OF THE USE OF OR INABILITY TO USE THE SOURCE CODE , EVEN IF WPI OR ANY OF ITS AUTHORIZED
* REPRESENTATIVES HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, (b) ANY CLAIM ATTRIBUTABLE TO ERRORS,
* OMISSIONS, OR OTHER INACCURACIES IN THE SOURCE CODE, OR (c) ANY *CLAIM BY ANY THIRD PARTY. *
* Copyright (c) 2013 .  World Peace Industrial Co., LTD. All rights reserved.
*-----------------------------------------------------------------------------
*/

#ifndef __WNDRV_PMU_C__
#define __WNDRV_PMU_C__

#include "GPIO.h"
#include "../inc/LPC11Uxx.h"
#include "wnDrv_Pmu.h"
#include "my_board.h"

/*****************************************************************************
** Function name:		wnDrv_Pmu_Init
**
** Descriptions:		Initialize PMU and setup wakeup source.
**						For Sleep and deepsleep, any of the I/O pins can be
**						used as the wakeup source.
**						For Deep Powerdown, only pin P1.4 can be used as
**						wakeup source from deep powerdown.
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
#define USB_WAKEUP BIT19
#define GPIOINT0 BIT20
#define GPIOINT1 BIT21

void wnDrv_Pmu_Init(void)
{
    U32 ENA[2] = {0x00000000,0x00000000};
    U32 POL[2] = {0x00000000,0x00000000};

    /* Enable all clocks, even those turned off at power up. */
#if 0
    LPC_SYSCON->PDRUNCFG &=~(ADC_PD|WDTOSC_PD);
#else  
    {    
        LPC_SYSCON->PDRUNCFG &= ~SYS_PLL_PD;
        LPC_SYSCON->PDRUNCFG |= (IRC_OUT_PD|IRC_PD|ADC_PD|BOD_PD|WDT_OSC_PD);
    }
#endif      

    #if (defined(HAPTIC_V2) || defined (HAPTIC_V3) || defined (HAPTIC_V4) \
        || defined (IRPT_V4) || defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))
    ENA[GPIO_PORT0] = (MIDDLE_BUTTON_MASK | K5_BUTTON_MASK | DPI_CYCLE_BUTTON_MASK | K4_BUTTON_MASK); 
    #elif (defined(P723_POC))
    ENA[GPIO_PORT0] = (MIDDLE_BUTTON_MASK | K5_BUTTON_MASK | DPI_CYCLE_BUTTON_MASK ); 
    ENA[GPIO_PORT1] =(K4_BUTTON_MASK) ;
    #elif (defined(GEN_2_H_V1))
    ENA[GPIO_PORT0] = (MIDDLE_BUTTON_MASK | K4_BUTTON_MASK | DPI_CYCLE_BUTTON_MASK ); 
    ENA[GPIO_PORT1] =(K5_BUTTON_MASK) ;
    #else
    #error "Hardware information is undefined."
    #endif
    
      
//		ENA[GPIO_PORT0] = (LEFT_BUTTON_MASK | RIGHT_BUTTON_MASK | MIDDLE_BUTTON_MASK |K4_BUTTON_MASK |\
//                       K5_BUTTON_MASK|VOLUME_UP_BUTTON_MASK | VOLUME_DOWN_BUTTON_MASK |DPI_CYCLE_BUTTON_MASK );  
//  ENA[GPIO_PORT1] =(RIGHT_BUTTON_MASK |PROFILE_BUTTON_MASK | K5_BUTTON_MASK) ;//BUTTON_PORT1;//(__SBF(GPIO_KEY_C)|__SBF(GPIO_KEY_R)|__SBF(GPIO_WHEEL_ENC_A)|__SBF(GPIO_WHEEL_ENC_B));  
    
    /*    
    0 = The Pin Is Active LOW. If The Level On This Pin Is LOW, The
    Pin Contributes To The Group Interrupt.
    
    1 = The Pin Is Active HIGH. If The Level On This Pin Is HIGH, The
    Pin Contributes To The Group Interrupt.
    */
    
    /* GINT0_INT sources OR together */        
    InitGPIOGroupedInterrupt(GPIO_PORT0,&ENA[0],FALSE,0,&POL[0]);   
    /* GINT0_INT as wakeup source */
    LPC_SYSCON->STARTERP1 = (GPIOINT0|USB_WAKEUP); 

		
}

/*****************************************************************************
** Function name:		wnDrv_Pmu_Sleep
**
** Descriptions:		Put some of the peripheral in sleep mode.
**
** parameters:			SleepMode: 2 is power down, 1 is deep sleep, 0 is sleep,
**						Sleep peripheral module(s)
** Returned value:		None
**
*****************************************************************************/
void wnDrv_Pmu_Sleep(U8 SleepMode)
{
    LPC_SYSCON->PDAWAKECFG = LPC_SYSCON->PDRUNCFG;
//	  LPC_SYSCON->PDAWAKECFG &= ~BIT10;
    LPC_SYSCON->PDSLEEPCFG |= BIT3|BIT6;
    /* If normal sleep, not deep sleep, don't do anything to SCR reg. */
    switch(SleepMode)
    {
        case MCU_POWER_DOWN:
        SCB->SCR |= NVIC_LP_SLEEPDEEP;
        LPC_PMU->PCON = 0x02;//BIT1;
        break;

        case MCU_SLEEP:
        SCB->SCR = 0x00;
        LPC_PMU->PCON = 0x0;
        break;

        case MCU_DEEP_SLEEP:
        default:
//        SCB->SCR |= NVIC_LP_SLEEPDEEP;
//        LPC_PMU->PCON = BIT0;
        SCB->SCR |= NVIC_LP_SLEEPDEEP;
        LPC_PMU->PCON = BIT0;
        break;
    }
   __WFI();
}
#undef INTERFACE
#endif/*End __WNDRV_PMU_C__*/
/******************************************************************************
**                            End Of File
******************************************************************************/
