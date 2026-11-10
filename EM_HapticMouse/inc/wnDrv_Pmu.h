/****************************************************************************
 *   $Id:: wnDrv_Pmu.h 4043 2010-07-29 19:02:40Z usb01267                         $
 *   Project: NXP LPC11xx PMU example
 *
 *   Description:
 *     This file contains PMU code header definition.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#ifndef __WNDRV_PMU_H__
#define __WNDRV_PMU_H__
#include "Type.h"
//#include "wnBoard.h"
#include "button.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __WNDRV_PMU_C__
#define INTERFACE
#else
#define INTERFACE extern
#endif

#if 0
#if(ENABLE_UART)
#define __DEBUG_WNDRV_PMU(x) x
#else
#define __DEBUG_WNDRV_PMU(x)
#endif
#else
#define __DEBUG_WNDRV_PMU(x)
#endif

#define GPIO_PORT0 0
#define GPIO_PORT1 1

#define MCU_SLEEP 0
#define MCU_DEEP_SLEEP 1
#define MCU_POWER_DOWN 2

/* System Low Power */
#define NVIC_LP_SEVONPEND (0x10)
#define NVIC_LP_SLEEPDEEP (0x04)
#define NVIC_LP_SLEEPONEXIT (0x02)

#define IRC_OUT_PD BIT0
#define IRC_PD BIT1
#define FLASH_PD BIT2
#define BOD_PD BIT3
#define ADC_PD BIT4
#define SYS_OSC_PD BIT5
#define WDT_OSC_PD BIT6
#define SYS_PLL_PD BIT7

INTERFACE void wnDrv_Pmu_Init(void);
INTERFACE void wnDrv_Pmu_Sleep(U8 SleepMode);

#undef INTERFACE
#ifdef __cplusplus
}
#endif
#endif /* End  __WNDRV_PMU_H__ */
/*****************************************************************************
**                            End Of File
******************************************************************************/
