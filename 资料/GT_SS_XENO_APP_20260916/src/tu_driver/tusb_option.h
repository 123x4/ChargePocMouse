/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#ifndef _TUSB_OPTION_H_
#define _TUSB_OPTION_H_


#include "common/tusb_compiler.h"

// Version is release as major.minor.revision eg 1.0.0
#define TUSB_VERSION_MAJOR     0
#define TUSB_VERSION_MINOR     18
#define TUSB_VERSION_REVISION  0

#define TUSB_VERSION_NUMBER    (TUSB_VERSION_MAJOR * 10000 + TUSB_VERSION_MINOR * 100 + TUSB_VERSION_REVISION)
#define TUSB_VERSION_STRING    TU_XSTRING(TUSB_VERSION_MAJOR) "." TU_XSTRING(TUSB_VERSION_MINOR) "." TU_XSTRING(TUSB_VERSION_REVISION)

//--------------------------------------------------------------------+
// Supported MCUs
// CFG_TUSB_MCU must be defined to one of following value
//--------------------------------------------------------------------+

#define OPT_MCU_NONE                0


// TI
#define OPT_MCU_MSP430x5xx        500 ///< TI MSP430x5xx
#define OPT_MCU_MSP432E4          510 ///< TI MSP432E4xx
#define OPT_MCU_TM4C123           511 ///< TI Tiva-C 123x
#define OPT_MCU_TM4C129           512 ///< TI Tiva-C 129x


// GTRY
#define OPT_MCU_GTRY6xx        	  8600 ///< GTRY6xx


// Check if configured MCU is one of listed
// Apply _TU_CHECK_MCU with || as separator to list of input
#define _TU_CHECK_MCU(_m)    (CFG_TUSB_MCU == _m)
#define TU_CHECK_MCU(...)    (TU_ARGS_APPLY(_TU_CHECK_MCU, ||, __VA_ARGS__))

//--------------------------------------------------------------------+
// Supported OS
//--------------------------------------------------------------------+

#define OPT_OS_NONE       1  ///< No RTOS
#define OPT_OS_FREERTOS   2  ///< FreeRTOS
#define OPT_OS_MYNEWT     3  ///< Mynewt OS
#define OPT_OS_CUSTOM     4  ///< Custom OS is implemented by application
#define OPT_OS_PICO       5  ///< Raspberry Pi Pico SDK
#define OPT_OS_RTTHREAD   6  ///< RT-Thread
#define OPT_OS_RTX4       7  ///< Keil RTX 4
#define OPT_OS_ZEPHYR     8  ///< Zephyr

//--------------------------------------------------------------------+
// Mode and Speed
//--------------------------------------------------------------------+

// Low byte is operational mode
#define OPT_MODE_NONE           0x0000 ///< Disabled
#define OPT_MODE_DEVICE         0x0001 ///< Device Mode
#define OPT_MODE_HOST           0x0002 ///< Host Mode

// High byte is max operational speed (corresponding to tusb_speed_t)
#define OPT_MODE_DEFAULT_SPEED  0x0000 ///< Default (max) speed supported by MCU
#define OPT_MODE_LOW_SPEED      0x0100 ///< Low Speed
#define OPT_MODE_FULL_SPEED     0x0200 ///< Full Speed
#define OPT_MODE_HIGH_SPEED     0x0400 ///< High Speed
#define OPT_MODE_SPEED_MASK     0xff00

//--------------------------------------------------------------------+
// Include tusb_config.h
//--------------------------------------------------------------------+

// Allow to use command line to change the config name/location
#ifdef CFG_TUSB_CONFIG_FILE
  #include CFG_TUSB_CONFIG_FILE
#else
  #include "tusb_config.h"
#endif

#include "common/tusb_mcu.h"


//--------------------------------------------------------------------
// RootHub Mode detection
//--------------------------------------------------------------------

//------------- Root hub as Device -------------//

#if defined(CFG_TUSB_RHPORT0_MODE) && ((CFG_TUSB_RHPORT0_MODE) & OPT_MODE_DEVICE)
  #define TUD_RHPORT_MODE     (CFG_TUSB_RHPORT0_MODE)
  #define TUD_OPT_RHPORT      0
#elif defined(CFG_TUSB_RHPORT1_MODE) && ((CFG_TUSB_RHPORT1_MODE) & OPT_MODE_DEVICE)
  #define TUD_RHPORT_MODE     (CFG_TUSB_RHPORT1_MODE)
  #define TUD_OPT_RHPORT      1
#else
  #define TUD_RHPORT_MODE     OPT_MODE_NONE
#endif

#ifndef CFG_TUD_ENABLED
  // fallback to use CFG_TUSB_RHPORTx_MODE
  #define CFG_TUD_ENABLED     (TUD_RHPORT_MODE & OPT_MODE_DEVICE)
#endif

#ifndef CFG_TUD_MAX_SPEED
  // fallback to use CFG_TUSB_RHPORTx_MODE
  #define CFG_TUD_MAX_SPEED   (TUD_RHPORT_MODE & OPT_MODE_SPEED_MASK)
#endif

// For backward compatible
#define TUSB_OPT_DEVICE_ENABLED CFG_TUD_ENABLED

// highspeed support indicator
#define TUD_OPT_HIGH_SPEED    (CFG_TUD_MAX_SPEED ? (CFG_TUD_MAX_SPEED & OPT_MODE_HIGH_SPEED) : TUP_RHPORT_HIGHSPEED)

//------------- Root hub as Host -------------//

#if defined(CFG_TUSB_RHPORT0_MODE) && ((CFG_TUSB_RHPORT0_MODE) & OPT_MODE_HOST)
  #define TUH_RHPORT_MODE  (CFG_TUSB_RHPORT0_MODE)
  #define TUH_OPT_RHPORT   0
#elif defined(CFG_TUSB_RHPORT1_MODE) && ((CFG_TUSB_RHPORT1_MODE) & OPT_MODE_HOST)
  #define TUH_RHPORT_MODE  (CFG_TUSB_RHPORT1_MODE)
  #define TUH_OPT_RHPORT   1
#else
  #define TUH_RHPORT_MODE   OPT_MODE_NONE
#endif

#ifndef CFG_TUH_ENABLED
  // fallback to use CFG_TUSB_RHPORTx_MODE
  #define CFG_TUH_ENABLED     (TUH_RHPORT_MODE & OPT_MODE_HOST)
#endif

#ifndef CFG_TUH_MAX_SPEED
  // fallback to use CFG_TUSB_RHPORTx_MODE
  #define CFG_TUH_MAX_SPEED   (TUH_RHPORT_MODE & OPT_MODE_SPEED_MASK)
#endif

// For backward compatible
#define TUSB_OPT_HOST_ENABLED   CFG_TUH_ENABLED

// highspeed support indicator
#define TUH_OPT_HIGH_SPEED    (CFG_TUH_MAX_SPEED ? (CFG_TUH_MAX_SPEED & OPT_MODE_HIGH_SPEED) : TUP_RHPORT_HIGHSPEED)


//--------------------------------------------------------------------+
// TODO move later
//--------------------------------------------------------------------+

// TUP_MCU_STRICT_ALIGN will overwrite TUP_ARCH_STRICT_ALIGN.
// In case TUP_MCU_STRICT_ALIGN = 1 and TUP_ARCH_STRICT_ALIGN =0, we will not reply on compiler
// to generate unaligned access code.
// LPC_IP3511 Highspeed cannot access unaligned memory on USB_RAM
#if 0
#if TUD_OPT_HIGH_SPEED && TU_CHECK_MCU(OPT_MCU_LPC54XXX, OPT_MCU_LPC55XX)
  #define TUP_MCU_STRICT_ALIGN   1
#else
  #define TUP_MCU_STRICT_ALIGN   0
#endif
#else
#define TUP_MCU_STRICT_ALIGN   0
#endif

//--------------------------------------------------------------------+
// Common Options (Default)
//--------------------------------------------------------------------+

// Debug enable to print out error message
#ifndef CFG_TUSB_DEBUG
  #define CFG_TUSB_DEBUG 0
#endif

// Level where CFG_TUSB_DEBUG must be at least for USBH is logged
#ifndef CFG_TUH_LOG_LEVEL
  #define CFG_TUH_LOG_LEVEL   2
#endif

// Level where CFG_TUSB_DEBUG must be at least for USBD is logged
#ifndef CFG_TUD_LOG_LEVEL
  #define CFG_TUD_LOG_LEVEL   2
#endif

// Memory section for placing buffer used for usb transferring. If MEM_SECTION is different for
// host and device use: CFG_TUD_MEM_SECTION, CFG_TUH_MEM_SECTION instead
#ifndef CFG_TUSB_MEM_SECTION
  #define CFG_TUSB_MEM_SECTION
#endif

// Alignment requirement of buffer used for usb transferring. if MEM_ALIGN is different for
// host and device controller use: CFG_TUD_MEM_ALIGN, CFG_TUH_MEM_ALIGN instead
#ifndef CFG_TUSB_MEM_ALIGN
  #define CFG_TUSB_MEM_ALIGN      TU_ATTR_ALIGNED(4)
#endif

#ifndef CFG_TUSB_MEM_DCACHE_LINE_SIZE
  #ifndef CFG_TUSB_MEM_DCACHE_LINE_SIZE_DEFAULT
  #define CFG_TUSB_MEM_DCACHE_LINE_SIZE_DEFAULT 1
  #endif

  #define CFG_TUSB_MEM_DCACHE_LINE_SIZE CFG_TUSB_MEM_DCACHE_LINE_SIZE_DEFAULT
#endif

// OS selection
#ifndef CFG_TUSB_OS
  #define CFG_TUSB_OS           OPT_OS_NONE
#endif

#ifndef CFG_TUSB_OS_INC_PATH
  #ifndef CFG_TUSB_OS_INC_PATH_DEFAULT
  #define CFG_TUSB_OS_INC_PATH_DEFAULT
  #endif

  #define CFG_TUSB_OS_INC_PATH  CFG_TUSB_OS_INC_PATH_DEFAULT
#endif

//--------------------------------------------------------------------
// Device Options (Default)
//--------------------------------------------------------------------

// Attribute to place data in accessible RAM for device controller (default: CFG_TUSB_MEM_SECTION)
#ifndef CFG_TUD_MEM_SECTION
  #define CFG_TUD_MEM_SECTION     CFG_TUSB_MEM_SECTION
#endif

// Attribute to align memory for device controller (default: CFG_TUSB_MEM_ALIGN)
#ifndef CFG_TUD_MEM_ALIGN
  #define CFG_TUD_MEM_ALIGN       CFG_TUSB_MEM_ALIGN
#endif

#ifndef CFG_TUD_MEM_DCACHE_ENABLE
  #ifndef CFG_TUD_MEM_DCACHE_ENABLE_DEFAULT
  #define CFG_TUD_MEM_DCACHE_ENABLE_DEFAULT  0
  #endif

  #define CFG_TUD_MEM_DCACHE_ENABLE   CFG_TUD_MEM_DCACHE_ENABLE_DEFAULT
#endif

#ifndef CFG_TUD_MEM_DCACHE_LINE_SIZE
  #define CFG_TUD_MEM_DCACHE_LINE_SIZE CFG_TUSB_MEM_DCACHE_LINE_SIZE
#endif

#ifndef CFG_TUD_ENDPOINT0_SIZE
  #define CFG_TUD_ENDPOINT0_SIZE  64
#endif

#ifndef CFG_TUD_INTERFACE_MAX
  #define CFG_TUD_INTERFACE_MAX   16
#endif

// default to max hardware endpoint, but can be smaller to save RAM
#ifndef CFG_TUD_ENDPPOINT_MAX
  #define CFG_TUD_ENDPPOINT_MAX   TUP_DCD_ENDPOINT_MAX
#endif

#if CFG_TUD_ENDPPOINT_MAX > TUP_DCD_ENDPOINT_MAX
  #error "CFG_TUD_ENDPPOINT_MAX must be less than or equal to TUP_DCD_ENDPOINT_MAX"
#endif

// USB 2.0 7.1.20: compliance test mode support
#ifndef CFG_TUD_TEST_MODE
  #define CFG_TUD_TEST_MODE       0
#endif

//------------- Device Class Driver -------------//
#ifndef CFG_TUD_BTH
  #define CFG_TUD_BTH             0
#endif

#if CFG_TUD_BTH && !defined(CFG_TUD_BTH_ISO_ALT_COUNT)
#error CFG_TUD_BTH_ISO_ALT_COUNT must be defined to tell Bluetooth driver the number of ISO endpoints to use
#endif

#ifndef CFG_TUD_CDC
  #define CFG_TUD_CDC             0
#endif

#ifndef CFG_TUD_MSC
  #define CFG_TUD_MSC             0
#endif

#ifndef CFG_TUD_HID
  #define CFG_TUD_HID             0
#endif

#ifndef CFG_TUD_AUDIO
  #define CFG_TUD_AUDIO           0
#endif

#ifndef CFG_TUD_VIDEO
  #define CFG_TUD_VIDEO           0
#endif

#ifndef CFG_TUD_MIDI
  #define CFG_TUD_MIDI            0
#endif

#ifndef CFG_TUD_VENDOR
  #define CFG_TUD_VENDOR          0
#endif

#ifndef CFG_TUD_USBTMC
  #define CFG_TUD_USBTMC          0
#endif

#ifndef CFG_TUD_DFU_RUNTIME
  #define CFG_TUD_DFU_RUNTIME     0
#endif

#ifndef CFG_TUD_DFU
  #define CFG_TUD_DFU             0
#endif

#ifndef CFG_TUD_ECM_RNDIS
  #ifdef CFG_TUD_NET
    #warning "CFG_TUD_NET is renamed to CFG_TUD_ECM_RNDIS"
    #define CFG_TUD_ECM_RNDIS   CFG_TUD_NET
  #else
    #define CFG_TUD_ECM_RNDIS   0
  #endif
#endif

#ifndef CFG_TUD_NCM
  #define CFG_TUD_NCM         0
#endif

//--------------------------------------------------------------------
// Host Options (Default)
//--------------------------------------------------------------------
#if CFG_TUH_ENABLED
  #ifndef CFG_TUH_DEVICE_MAX
    #define CFG_TUH_DEVICE_MAX 1
  #endif

  #ifndef CFG_TUH_ENUMERATION_BUFSIZE
    #define CFG_TUH_ENUMERATION_BUFSIZE 256
  #endif
#endif // CFG_TUH_ENABLED

// Attribute to place data in accessible RAM for host controller (default: CFG_TUSB_MEM_SECTION)
#ifndef CFG_TUH_MEM_SECTION
  #define CFG_TUH_MEM_SECTION   CFG_TUSB_MEM_SECTION
#endif

// Attribute to align memory for host controller
#ifndef CFG_TUH_MEM_ALIGN
  #define CFG_TUH_MEM_ALIGN     CFG_TUSB_MEM_ALIGN
#endif

#ifndef CFG_TUH_MEM_DCACHE_ENABLE
  #ifndef CFG_TUH_MEM_DCACHE_ENABLE_DEFAULT
  #define CFG_TUH_MEM_DCACHE_ENABLE_DEFAULT  0
  #endif

  #define CFG_TUH_MEM_DCACHE_ENABLE   CFG_TUH_MEM_DCACHE_ENABLE_DEFAULT
#endif

#ifndef CFG_TUH_MEM_DCACHE_LINE_SIZE
  #define CFG_TUH_MEM_DCACHE_LINE_SIZE CFG_TUSB_MEM_DCACHE_LINE_SIZE
#endif

//------------- CLASS -------------//

#ifndef CFG_TUH_HUB
  #define CFG_TUH_HUB    0
#endif

#ifndef CFG_TUH_CDC
  #define CFG_TUH_CDC    0
#endif

// FTDI is not part of CDC class, only to re-use CDC driver API
#ifndef CFG_TUH_CDC_FTDI
  #define CFG_TUH_CDC_FTDI 0
#endif

// List of product IDs that can use the FTDI CDC driver. 0x0403 is FTDI's VID
#ifndef CFG_TUH_CDC_FTDI_VID_PID_LIST
  #define CFG_TUH_CDC_FTDI_VID_PID_LIST \
    {0x0403, 0x6001}, /* Similar device to SIO above */ \
    {0x0403, 0x6006}, /* FTDI's alternate PID for above */ \
    {0x0403, 0x6010}, /* Dual channel device */ \
    {0x0403, 0x6011}, /* Quad channel hi-speed device */ \
    {0x0403, 0x6014}, /* Single channel hi-speed device */ \
    {0x0403, 0x6015}, /* FT-X series (FT201X, FT230X, FT231X, etc) */ \
    {0x0403, 0x6040}, /* Dual channel hi-speed device with PD */ \
    {0x0403, 0x6041}, /* Quad channel hi-speed device with PD */ \
    {0x0403, 0x6042}, /* Dual channel hi-speed device with PD */ \
    {0x0403, 0x6043}, /* Quad channel hi-speed device with PD */ \
    {0x0403, 0x6044}, /* Dual channel hi-speed device with PD */ \
    {0x0403, 0x6045}, /* Dual channel hi-speed device with PD */ \
    {0x0403, 0x6048}, /* Quad channel automotive grade hi-speed device */ \
    {0x0403, 0x8372}, /* Product Id SIO application of 8U100AX */ \
    {0x0403, 0xFBFA}, /* Product ID for FT232RL */ \
    {0x0403, 0xCD18}, /* ??? */
#endif

// CP210X is not part of CDC class, only to re-use CDC driver API
#ifndef CFG_TUH_CDC_CP210X
  #define CFG_TUH_CDC_CP210X 0
#endif

// List of product IDs that can use the CP210X CDC driver. 0x10C4 is Silicon Labs' VID
#ifndef CFG_TUH_CDC_CP210X_VID_PID_LIST
  #define CFG_TUH_CDC_CP210X_VID_PID_LIST \
  { 0x10C4, 0xEA60 }, /* Silicon Labs factory default */ \
  { 0x10C4, 0xEA61 }, /* Silicon Labs factory default */ \
  { 0x10C4, 0xEA70 }  /* Silicon Labs Dual Port factory default */
#endif

#ifndef CFG_TUH_CDC_CH34X
  // CH34X is not part of CDC class, only to re-use CDC driver API
  #define CFG_TUH_CDC_CH34X 0
#endif

// List of product IDs that can use the CH34X CDC driver
#ifndef CFG_TUH_CDC_CH34X_VID_PID_LIST
  #define CFG_TUH_CDC_CH34X_VID_PID_LIST \
    { 0x1a86, 0x5523 }, /* ch341 chip */ \
    { 0x1a86, 0x7522 }, /* ch340k chip */ \
    { 0x1a86, 0x7523 }, /* ch340 chip */ \
    { 0x1a86, 0xe523 }, /* ch330 chip */ \
    { 0x4348, 0x5523 }, /* ch340 custom chip */ \
    { 0x2184, 0x0057 }, /* overtaken from Linux Kernel driver /drivers/usb/serial/ch341.c */ \
    { 0x9986, 0x7523 }  /* overtaken from Linux Kernel driver /drivers/usb/serial/ch341.c */
#endif

#ifndef CFG_TUH_CDC_PL2303
  // PL2303 is not part of CDC class, only to re-use CDC driver API
  #define CFG_TUH_CDC_PL2303 0
#endif

#ifndef CFG_TUH_CDC_PL2303_VID_PID_QUIRKS_LIST
  // List of product IDs that can use the PL2303 CDC driver
  #define CFG_TUH_CDC_PL2303_VID_PID_LIST \
  { 0x067b, 0x2303 }, /* initial 2303 */ \
  { 0x067b, 0x2304 }, /* TB */ \
  { 0x067b, 0x23a3 }, /* GC */ \
  { 0x067b, 0x23b3 }, /* GB */ \
  { 0x067b, 0x23c3 }, /* GT */ \
  { 0x067b, 0x23d3 }, /* GL */ \
  { 0x067b, 0x23e3 }, /* GE */ \
  { 0x067b, 0x23f3 }  /* GS */
#endif

#ifndef CFG_TUH_HID
  #define CFG_TUH_HID    0
#endif

#ifndef CFG_TUH_MIDI
  #define CFG_TUH_MIDI   0
#endif

#ifndef CFG_TUH_MSC
  #define CFG_TUH_MSC    0
#endif

#ifndef CFG_TUH_VENDOR
  #define CFG_TUH_VENDOR 0
#endif

#ifndef CFG_TUH_API_EDPT_XFER
  #define CFG_TUH_API_EDPT_XFER 0
#endif

//------------------------------------------------------------------
// Configuration Validation
//------------------------------------------------------------------
#if CFG_TUD_ENDPOINT0_SIZE > 64
  #error Control Endpoint Max Packet Size cannot be larger than 64
#endif

extern bool dcd_edpt_fast_data_in(uint8_t rhport, uint8_t ep_addr, uint8_t * buffer, uint16_t total_bytes);

#endif /* _TUSB_OPTION_H_ */

/** @} */
