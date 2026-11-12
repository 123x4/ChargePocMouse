/*
 * Copyright (c) 2019
 * All rights reserved.
 *
 */

#ifndef __GTRY_PLATFORM_H__
#define __GTRY_PLATFORM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gtry_type.h"
#include "gtry_smu.h"


/*****************************************************************************
 * Board specified
 ****************************************************************************/

// R8080 (I8080 and II8080 LCD interface) [Driver_R8080]
// Configuration settings for Driver_R8080 in component ::Drivers:R8080
#define GTRY_R8080_EN					1

// RGB (RGB LCD interface) [Driver_RGB]
// Configuration settings for Driver_RGB in component ::Drivers:RGB
#define GTRY_RGB_EN						1

// DVP (DVP interface) [Driver_DVP]
// Configuration settings for Driver_DVP in component ::Drivers:DVP
#define GTRY_DVP_EN						1

// USB Controller [Driver_USBD and Driver_USBH]
// Configuration settings for Driver_USBD in component ::Drivers:USB Device
// Configuration settings for Driver_USBH in component ::Drivers:USB Host
#define GTRY_USB_EN						1


// ACMP (ACMP) [Driver_ACMP]
// Configuration settings for Driver_ACMP in component ::Drivers:ACMP
#define GTRY_ACMP_EN					1

// OPA (OPA) [Driver_OPA]
// Configuration settings for Driver_OPA in component ::Drivers:OPA
#define GTRY_OPA_EN						1

// TSC (TSC) [Driver_TSC]
// Configuration settings for Driver_TSC in component ::Drivers:TSC
#define GTRY_TSC_EN						1

// IOAM (IOAM) [Driver_IOAM]
// Configuration settings for Driver_IOAM in component ::Drivers:IOAM
#define GTRY_IOAM_EN					1

// RTC Controller [Driver_RTC]
// Configuration settings for Driver_RTC in component ::Drivers:RTC
#define GTRY_RTC_EN						1

// WDT Controller [Driver_WDT]
// Configuration settings for Driver_WDT in component ::Drivers:WDT
#define GTRY_WDT_EN						1

// LTIMER Controller [Driver_LTIMER]
// Configuration settings for Driver_LTIMER in component ::Drivers:LTIMER
#define GTRY_LTIMER_EN					1

// TRNG Controller [Driver_TRNG]
// Configuration settings for Driver_TRNG in component ::Drivers:TRNG
#define GTRY_RNG_EN						1

// AES Controller [Driver_AES]
// Configuration settings for Driver_AES in component ::Drivers:AES
#define GTRY_AES_EN						1


// UART Port Number
typedef enum
{
    UART_NO0 = 0,
    UART_NO1,
    UART_NO2,
    UART_NUM_MAXS,
} RYRV_UART_NO;

//printf uart num set
#define UART_PRINT_NO					1//0~2 == UART_NO0~2


// UART0 (Universal asynchronous receiver transmitter) [Driver_UART]
// Configuration settings for Driver_UART in component ::Drivers:UART
#define GTRY_UART0_EN					1
// DMA
//  Tx
//    Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_UART0_DMA_TX_EN            0
#define GTRY_UART0_DMA_TX_CH            0
#define GTRY_UART0_DMA_TX_REQID         0
//  Rx
//    Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_UART0_DMA_RX_EN            0
#define GTRY_UART0_DMA_RX_CH            1
#define GTRY_UART0_DMA_RX_REQID         1

// UART1 (Universal asynchronous receiver transmitter) [Driver_UART]
// Configuration settings for Driver_UART in component ::Drivers:UART
#define GTRY_UART1_EN                   1
// DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_UART1_DMA_TX_EN            0
#define GTRY_UART1_DMA_TX_CH            2
#define GTRY_UART1_DMA_TX_REQID         2
//   Rx
//     Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_UART1_DMA_RX_EN            0
#define GTRY_UART1_DMA_RX_CH            3
#define GTRY_UART1_DMA_RX_REQID         3

// UART2 (Universal asynchronous receiver transmitter) [Driver_UART]
// Configuration settings for Driver_UART in component ::Drivers:UART
#define GTRY_UART2_EN                   1
// DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_UART2_DMA_TX_EN            0
#define GTRY_UART2_DMA_TX_CH            4
#define GTRY_UART2_DMA_TX_REQID         4
//   Rx
//     Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_UART2_DMA_RX_EN            0
#define GTRY_UART2_DMA_RX_CH            5
#define GTRY_UART2_DMA_RX_REQID         5


// I2C Port Number
typedef enum
{
	I2C_NO0 = 0,
	I2C_NO1,
	I2C_NUM_MAXS,
} RYRV_I2C_NO;

// I2C0 (Inter-integrated Circuit Interface) [Driver_I2C]
// Configuration settings for Driver_I2C in component ::Drivers:I2C
#define GTRY_I2C0_EN                    1
// DMA
//   Tx/Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_I2C0_DMA_TR_EN				1
#define GTRY_I2C0_DMA_TR_CH				6
#define GTRY_I2C0_DMA_TR_REQID			6



// I2C1 (Inter-integrated Circuit Interface) [Driver_I2C]
// Configuration settings for Driver_I2C in component ::Drivers:I2C
#define GTRY_I2C1_EN                    1
// DMA
//   Tx/Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_I2C1_DMA_TR_EN             1
#define GTRY_I2C1_DMA_TR_CH             6
#define GTRY_I2C1_DMA_TR_REQID          7


// PWM0 Controller [Driver_PWM0]
// Configuration settings for Driver_PWM0 in component ::Drivers:PWM
#define GTRY_PWM_EN                     1
// DMA
//   Rx
//     Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_PWM_DMA_RX_EN             	1
#define GTRY_PWM_DMA_RX_CH             	7
#define GTRY_PWM_DMA_RX_REQID          	8

// TIMER [Driver_TIMER]
// Configuration settings for Driver_TIMER in component ::Drivers:TIMER
#define GTRY_TIMER_EN                   1
// DMA
//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_TIMER_DMA_RX_EN			0
#define GTRY_TIMER_DMA_RX_CH            0
#define GTRY_TIMER_DMA_RX_REQID         9


// SDIO Interface [Driver_SDIO]
// Configuration settings for Driver_SDIO in component ::Drivers:SDIO
#define GTRY_SDIO_EN                    1
// DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SDIO_DMA_TX_EN             0
#define GTRY_SDIO_DMA_TX_CH             1
#define GTRY_SDIO_DMA_TX_REQID          10
//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SDIO_DMA_RX_EN             0
#define GTRY_SDIO_DMA_RX_CH             2
#define GTRY_SDIO_DMA_RX_REQID          11


// ADC Interface [Driver_ADC]
// Configuration settings for Driver_ADC in component ::Drivers:ADC
#define GTRY_ADC_EN                     1
// DMA
//   Rx
//     Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_ADC_DMA_RX_EN              1
#define GTRY_ADC_DMA_RX_CH              3
#define GTRY_ADC_DMA_RX_REQID           12


// I2S (Integrated Interchip Sound) [Driver_I2S]
// Configuration settings for Driver_I2S in component ::Drivers:I2S
#define GTRY_I2S_EN                     1
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_I2S_DMA_TX_EN              1
#define GTRY_I2S_DMA_TX_CH              4
#define GTRY_I2S_DMA_TX_REQID           13
//   Rx
//     Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_I2S_DMA_RX_EN              1
#define GTRY_I2S_DMA_RX_CH              5
#define GTRY_I2S_DMA_RX_REQID           14

// ARGB Interface [Driver_ARGB]
// Configuration settings for Driver_ARGB in component ::Drivers:ARGB
#define GTRY_ARGB_EN                    1
// DMA
//   Rx
//     Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_ARGB_DMA_RX_EN             1
#define GTRY_ARGB_DMA_RX_CH             6
#define GTRY_ARGB_DMA_RX_REQID          15



// SPI Port Number
typedef enum
{
	SPI_NO0 = 0,
	SPI_NO1,
	SPI_NO2,
	SPI_NO3,
	SPI_NO4,
	SPI_NO5,
	SPI_NO6,
	SPI_NUM_MAXS,
} RYRV_SPI_NO;

//qspi flash num set
#define QSPI_FLASH_NO					5//0~6 == SPI_NO0~6

// SPI (Serial Peripheral Interface) [Driver_SPI]
// Configuration settings for Driver_SPI in component ::Drivers:SPI
#if(QSPI_FLASH_NO == 0)
#define GTRY_SPI0_EN					0
#else
#define GTRY_SPI0_EN					1
#endif
// SPI0 DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI0_DMA_TX_EN				1
#define GTRY_SPI0_DMA_TX_CH             0
#define GTRY_SPI0_DMA_TX_REQID          2
//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI0_DMA_RX_EN             1
#define GTRY_SPI0_DMA_RX_CH             1
#define GTRY_SPI0_DMA_RX_REQID          3

#if(QSPI_FLASH_NO == 1)
#define GTRY_SPI1_EN					0
#else
#define GTRY_SPI1_EN					1
#endif
// SPI1 DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI1_DMA_TX_EN             1
#define GTRY_SPI1_DMA_TX_CH             2
#define GTRY_SPI1_DMA_TX_REQID          4
//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI1_DMA_RX_EN             1
#define GTRY_SPI1_DMA_RX_CH             3
#define GTRY_SPI1_DMA_RX_REQID          5

#if(QSPI_FLASH_NO == 2)
#define GTRY_SPI2_EN					0
#else
#define GTRY_SPI2_EN					1
#endif
// SPI2 DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI2_DMA_TX_EN             1
#define GTRY_SPI2_DMA_TX_CH             4
#define GTRY_SPI2_DMA_TX_REQID          6
//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI2_DMA_RX_EN             1
#define GTRY_SPI2_DMA_RX_CH             5
#define GTRY_SPI2_DMA_RX_REQID          7

#if(QSPI_FLASH_NO == 3)
#define GTRY_SPI3_EN					0
#else
#define GTRY_SPI3_EN					1
#endif
// SPI3 DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI3_DMA_TX_EN             1
#define GTRY_SPI3_DMA_TX_CH             6
#define GTRY_SPI3_DMA_TX_REQID          8
//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI3_DMA_RX_EN             1
#define GTRY_SPI3_DMA_RX_CH             7
#define GTRY_SPI3_DMA_RX_REQID          9

#if(QSPI_FLASH_NO == 4)
#define GTRY_SPI4_EN					0
#else
#define GTRY_SPI4_EN					1
#endif
// SPI4 DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI4_DMA_TX_EN             1
#define GTRY_SPI4_DMA_TX_CH             0
#define GTRY_SPI4_DMA_TX_REQID          10
//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI4_DMA_RX_EN             1
#define GTRY_SPI4_DMA_RX_CH             1
#define GTRY_SPI4_DMA_RX_REQID          11

#if(QSPI_FLASH_NO == 5)
#define GTRY_SPI5_EN					0
#else
#define GTRY_SPI5_EN					1
#endif
// SPI5 DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI5_DMA_TX_EN             1
#define GTRY_SPI5_DMA_TX_CH             2
#define GTRY_SPI5_DMA_TX_REQID          12
//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI5_DMA_RX_EN             1
#define GTRY_SPI5_DMA_RX_CH             3
#define GTRY_SPI5_DMA_RX_REQID          13

#if(QSPI_FLASH_NO == 6)
#define GTRY_SPI6_EN					0
#else
#define GTRY_SPI6_EN					1
#endif
// SPI6 DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI6_DMA_TX_EN             1
#define GTRY_SPI6_DMA_TX_CH             4
#define GTRY_SPI6_DMA_TX_REQID          14
//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define GTRY_SPI6_DMA_RX_EN             1
#define GTRY_SPI6_DMA_RX_CH             5
#define GTRY_SPI6_DMA_RX_REQID          15


#ifdef __cplusplus
}
#endif

#endif	/* __GTRY_PLATFORM_H__ */
