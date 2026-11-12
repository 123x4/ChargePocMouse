/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021, Ha Thach (tinyusb.org)
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

#ifndef TUSB_MCU_H_
#define TUSB_MCU_H_

//--------------------------------------------------------------------+
// Port/Platform Specific
// TUP stand for TinyUSB Port/Platform (can be renamed)
//--------------------------------------------------------------------+

//------------- Unaligned Memory Access -------------//

#ifdef __ARM_ARCH
  // ARM Architecture set __ARM_FEATURE_UNALIGNED to 1 for mcu supports unaligned access
  #if defined(__ARM_FEATURE_UNALIGNED) && __ARM_FEATURE_UNALIGNED == 1
    #define TUP_ARCH_STRICT_ALIGN   0
  #else
    #define TUP_ARCH_STRICT_ALIGN   1
  #endif
#else
  // TODO default to strict align for others
  // Should investigate other architecture such as risv, xtensa, mips for optimal setting
  #define TUP_ARCH_STRICT_ALIGN   1
#endif

/* USB Controller Attributes for Device, Host or MCU (both)
 * - ENDPOINT_MAX: max (logical) number of endpoint
 * - ENDPOINT_EXCLUSIVE_NUMBER: endpoint number with different direction IN and OUT aren't allowed,
 *                              e.g EP1 OUT & EP1 IN cannot exist together
 * - RHPORT_HIGHSPEED: support highspeed with on-chip PHY
 */


//--------------------------------------------------------------------+
// TI
//--------------------------------------------------------------------+
#if TU_CHECK_MCU(OPT_MCU_MSP430x5xx)
  #define TUP_DCD_ENDPOINT_MAX    8

#elif TU_CHECK_MCU(OPT_MCU_MSP432E4, OPT_MCU_TM4C123, OPT_MCU_TM4C129)
  #define TUP_USBIP_MUSB
  #define TUP_USBIP_MUSB_TI
  #define TUP_DCD_ENDPOINT_MAX    8

//--------------------------------------------------------------------+
// GTRY
//--------------------------------------------------------------------+
#elif TU_CHECK_MCU(OPT_MCU_GTRY6xx)
  #define TUP_USBIP_MUSB
  #define TUP_USBIP_MUSB_GTRY
  #define TUP_DCD_ENDPOINT_MAX    8
#endif

//--------------------------------------------------------------------+
// External USB controller
//--------------------------------------------------------------------+
#if 0
#if defined(CFG_TUH_MAX3421) && CFG_TUH_MAX3421
  #ifndef CFG_TUH_MAX3421_ENDPOINT_TOTAL
    #define CFG_TUH_MAX3421_ENDPOINT_TOTAL  (8 + 4*(CFG_TUH_DEVICE_MAX-1))
  #endif
#endif
#endif

//--------------------------------------------------------------------+
// Default Values
//--------------------------------------------------------------------+

#ifndef TUP_MCU_MULTIPLE_CORE
#define TUP_MCU_MULTIPLE_CORE 0
#endif

#if !defined(TUP_DCD_ENDPOINT_MAX) && defined(CFG_TUD_ENABLED) && CFG_TUD_ENABLED
  #warning "TUP_DCD_ENDPOINT_MAX is not defined for this MCU, default to 8"
  #define TUP_DCD_ENDPOINT_MAX    8
#endif

// Default to fullspeed if not defined
#ifndef TUP_RHPORT_HIGHSPEED
  #define TUP_RHPORT_HIGHSPEED    0
#endif

// fast function, normally mean placing function in SRAM
#ifndef TU_ATTR_FAST_FUNC
  #define TU_ATTR_FAST_FUNC
#endif

// USBIP that support ISO alloc & activate API
#if /*defined(TUP_USBIP_DWC2) || defined(TUP_USBIP_FSDEV) || */defined(TUP_USBIP_MUSB)
  #define TUP_DCD_EDPT_ISO_ALLOC
#endif

#if 0
#if defined(TUP_USBIP_DWC2) // && CFG_TUD_DWC2_DMA_ENABLE == 0
  #define TUP_MEM_CONST_ADDR
#endif
#endif

#endif
