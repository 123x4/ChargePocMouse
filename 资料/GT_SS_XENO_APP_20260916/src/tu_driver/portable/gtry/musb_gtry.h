/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024, Brent Kowal (Analog Devices, Inc)
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

#ifndef TUSB_MUSB_GTRY_H_
#define TUSB_MUSB_GTRY_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "tusb_option.h"
#if CFG_TUSB_MCU == OPT_MCU_GTRY6xx
	#include "gtry_platform.h"
//#elif CFG_TUSB_MCU == OPT_MCU_TM4C129
//#elif CFG_TUSB_MCU == OPT_MCU_MSP432E4
  //#include "msp.h"
#else
  #error "Unsupported MCUs"
#endif

#define MUSB_CFG_SHARED_FIFO       0
#define MUSB_CFG_DYNAMIC_FIFO      1
#define MUSB_CFG_DYNAMIC_FIFO_SIZE 4096

const uintptr_t MUSB_BASES[] = { USB_BASE };

// Header supports both device and host modes. Only include what's necessary
#if CFG_TUD_ENABLED

// Mapping of IRQ numbers to port. Currently just 1.
static const IRQn_Type  musb_irqs[] = {
	IRQ_USB_SOURCE
};

static inline void musb_dcd_phy_init(uint8_t rhport){
  (void)rhport;
  //Nothing to do for this part
}

TU_ATTR_ALWAYS_INLINE static inline void musb_dcd_int_enable(uint8_t rhport) {
	/* Priority must be set > 0 to trigger the interrupt */
	__nds__plic_set_priority(musb_irqs[rhport], 3);//3
	// Enable PLIC interrupt PIT source
	__nds__plic_enable_interrupt(musb_irqs[rhport]);
//	/* Enable interrupts in general. */
//	set_csr(NDS_MSTATUS, MSTATUS_MIE);
}

TU_ATTR_ALWAYS_INLINE static inline void musb_dcd_int_disable(uint8_t rhport) {
  	// Disable PLIC interrupt PIT source
//	__nds__plic_disable_interrupt(musb_irqs[rhport]);
//	/* Disable global interrupt for core */
//	clear_csr(NDS_MSTATUS, MSTATUS_MIE);
}

TU_ATTR_ALWAYS_INLINE static inline unsigned musb_dcd_get_int_enable(uint8_t rhport) {
	(void)rhport;
	return 0;//__nds__plic_get_interrupt(musb_irqs[rhport]);
}

TU_ATTR_ALWAYS_INLINE static inline void musb_dcd_int_clear(uint8_t rhport) {
	(void)rhport;
	__nds__plic_complete_interrupt(musb_irqs[rhport]);
}

static inline void musb_dcd_int_handler_enter(uint8_t rhport) {
  (void)rhport;
  //Nothing to do for this part
}

static inline void tusb_time_delay_ms_api(uint32_t ms)
{
	ry_delay_ms(ms);
}

#endif // CFG_TUD_ENABLED

#ifdef __cplusplus
 }
#endif

#endif // TUSB_MUSB_GTRY_H_
