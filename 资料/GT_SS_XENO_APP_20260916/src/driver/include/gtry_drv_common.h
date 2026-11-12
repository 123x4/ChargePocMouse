/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

#ifndef __GTRY_DRV_COMMON_H
#define __GTRY_DRV_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define GTRY_DRIVER_VERSION_MAJOR_MINOR(major,minor) (((major) << 8) | (minor))

/**
\brief Driver Version
*/
typedef struct _GTRY_DRIVER_VERSION {
  uint16_t api;                         ///< API version
  uint16_t drv;                         ///< Driver version
} GTRY_DRIVER_VERSION;

/* General return codes */
#define RY_DRIVER_OK                	0 ///< Operation succeeded
#define RY_DRIVER_ERROR             	-1 ///< Unspecified error
#define RY_DRIVER_ERROR_BUSY        	-2 ///< Driver is busy
#define RY_DRIVER_ERROR_TIMEOUT     	-3 ///< Timeout occurred
#define RY_DRIVER_ERROR_UNSUPPORTED		-4 ///< Operation not supported
#define RY_DRIVER_ERROR_PARAMETER   	-5 ///< Parameter error
#define RY_DRIVER_ERROR_SPECIFIC    	-6 ///< Start of driver specific errors

/* Driver Module General return codes MASK */
#define RY_UART_DRIVER_RET_MASK			(0x1ul<<31) ///< usart Operation return head mask
#define RY_TSC_DRIVER_RET_MASK			(0x1ul<<30) ///< usart Operation return head mask
#define RY_WDT_DRIVER_RET_MASK			(0x1ul<<29) ///< usart Operation return head mask
#define RY_RTC_DRIVER_RET_MASK			(0x1ul<<28) ///< usart Operation return head mask
#define RY_ARGB_DRIVER_RET_MASK			(0x1ul<<27) ///< usart Operation return head mask
#define RY_GPIO_DRIVER_RET_MASK			(0x1ul<<26) ///< usart Operation return head mask
#define RY_I2C_DRIVER_RET_MASK			(0x1ul<<25) ///< usart Operation return head mask
#define RY_SPI_DRIVER_RET_MASK			(0x1ul<<24) ///< usart Operation return head mask
#define RY_DMA_DRIVER_RET_MASK			(0x1ul<<23) ///< usart Operation return head mask
#define RY_PWM_DRIVER_RET_MASK			(0x1ul<<22) ///< usart Operation return head mask
#define RY_TIMER_DRIVER_RET_MASK        (0x1ul<<21) ///< usart Operation return head mask
#define RY_LTIMER_DRIVER_RET_MASK		(0x1ul<<20) ///< usart Operation return head mask
#define RY_RNG_DRIVER_RET_MASK			(0x1ul<<19) ///< usart Operation return head mask
#define RY_IOAM_DRIVER_RET_MASK			(0x1ul<<18) ///< usart Operation return head mask
#define RY_I2S_DRIVER_RET_MASK			(0x1ul<<17) ///< usart Operation return head mask
#define RY_AES_DRIVER_RET_MASK			(0x1ul<<16) ///< usart Operation return head mask
#define RY_ADC_DRIVER_RET_MASK			(0x1ul<<15) ///< usart Operation return head mask
#define RY_COPA_DRIVER_RET_MASK			(0x1ul<<14) ///< usart Operation return head mask
#define RY_USB_DRIVER_RET_MASK			(0x1ul<<13) ///< usart Operation return head mask
#define RY_HASP_DRIVER_RET_MASK			(0x1ul<<12) ///< usart Operation return head mask  //wr
#define RY_RGB_DRIVER_RET_MASK			(0x1ul<<11) ///< usart Operation return head mask
#define RY_DVP_DRIVER_RET_MASK			(0x1ul<<10) ///< usart Operation return head mask
#define RY_R8080_DRIVER_RET_MASK		(0x1ul<<9) ///< usart Operation return head mask
#define RY_SDIO_DRIVER_RET_MASK			(0x1ul<<8) ///< usart Operation return head mask
#define RY_COMMON_DRIVER_RET_MASK		(0xfful) ///< usart Operation return head mask

/**
\brief General power states
*/ 
typedef enum _GTRY_POWER_STATE {
	RY_POWER_OFF,                        ///< Power off: no operation possible
	RY_POWER_LOW,                        ///< Low Power mode: retain state, detect and signal wake-up events
	RY_POWER_FULL                        ///< Power on: full operation at maximum performance
} RY_POWER_STATE;


/* log */
//#define DEBUG_OUTPUT
#define INFO_OUTPUT


#ifdef 	DEBUG_OUTPUT
#define debug(...)  printf(__VA_ARGS__)
#else
#define debug(...)
#endif

#ifdef 	INFO_OUTPUT
#define info(...)   printf(__VA_ARGS__)
#else
#define info(...)
#endif



#endif /* __GTRY_DRV_COMMON_H */



