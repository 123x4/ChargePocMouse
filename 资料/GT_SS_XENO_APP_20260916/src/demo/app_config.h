/*
 * app_config.h
 *
 *  Created on:
 *      Author:
 */

#ifndef SRC_DEMO_APP_CONFIG_H_
#define SRC_DEMO_APP_CONFIG_H_

#include "gtry_type.h"
#include "gtry_gpio_drv.h"
#include "gtry_pwm_drv.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+
#define TEST_DEBUG_IO_0				PB13//RX
#define TEST_DEBUG_IO_1				PA30//TP55
#define TEST_DEBUG_IO_2				PA28//TP59
#define TEST_DEBUG_IO_3				PA29//TP58

#define HALL_POWER_IO				PA37   //hall discharge and power
#define LED_EN						PA38
#define LED_LCD_EN					PA39

#define RESET_PIN					PA0

// Optional modules ported . Keep each block independently switchable.
#define LCD_MODULE_ENABLE			0
#define WHEEL_MODULE_ENABLE			1
#define MFG_EXT_MODE_ENABLE			1

#define HWID1_PIN					PA46
#define HWID2_PIN					PA47
#define HWID3_PIN					PA41
#define LANG_SEL1_PIN				PA45
#define LANG_SEL2_PIN				PA44

#define USB_ENABLE					1		//USB enable

#define PWM_ENABLE_MD				1		//pwm enable

#define WDT_TEST_MODE 				1		// WDT TEST

#define LTIMER0_ENABLE_START		1		//Ltimer enable

#define LTIMER_CC_TH				4		//Adjust the time of Ltimer

#define SUSPEND_MODE_ENABLE			1		//suspend sleep enable

#define REPORT_SIZE					(8)
#define OFF_SET_KEY					(2)
#define KB_REPORT_SIZE				REPORT_SIZE
#define NKRO_ENABLE					0

#define HOSC_TEST		       		1
#define HOSC_CALI_CLOCK       		64		//Kernel clock calibration HOSC

#define CONTINUOUS_MODE				0

#define H6_OFFSET_CALI				1

#define HALL_ERR 					0x44

// Keep the TinyUSB remote-wakeup workaround disabled;
#define SUSPEND_SEND_KEY_WAKEUP		0

/* Select the High-Speed manufacturing Vendor HID report length. Full-Speed
 * interrupt endpoints are limited to 64 bytes by USB and use the 64-byte
 * fallback descriptor regardless of this selection. */
#define MFG_USB_VENDOR_REPORT_LENGTH_64                 64U
#define MFG_USB_VENDOR_REPORT_LENGTH_1024               1024U

#ifndef MFG_USB_VENDOR_REPORT_LENGTH
#define MFG_USB_VENDOR_REPORT_LENGTH                    MFG_USB_VENDOR_REPORT_LENGTH_64//switch 64 or 1024
#endif
#define MFG_USB_VENDOR_REPORT_LENGTH_FS                 MFG_USB_VENDOR_REPORT_LENGTH_64

#if (MFG_USB_VENDOR_REPORT_LENGTH != MFG_USB_VENDOR_REPORT_LENGTH_64) && \
    (MFG_USB_VENDOR_REPORT_LENGTH != MFG_USB_VENDOR_REPORT_LENGTH_1024)
#error "MFG_USB_VENDOR_REPORT_LENGTH must be 64U or 1024U"
#endif

#define USB_DESCRIPTOR_VENDOR_REPORT_LENGTH             MFG_USB_VENDOR_REPORT_LENGTH

#define SYS_MODE_NULL		0
#define SYS_MODE_USB		(1<<0)		//Wired mode, HASP single, H6 single, SOF interrupt, timer interrupt.

//--------------------------------------------------------------------+
// DEBUG GPIO
//--------------------------------------------------------------------+
#define GTRY6xx_SHOW_DBG_H_0	gpio_out_set(TEST_DEBUG_IO_0)
#define GTRY6xx_SHOW_DBG_L_0	gpio_out_clear(TEST_DEBUG_IO_0)
#define GTRY6xx_SHOW_DBG_H_1	gpio_out_set(TEST_DEBUG_IO_1)
#define GTRY6xx_SHOW_DBG_L_1	gpio_out_clear(TEST_DEBUG_IO_1)
#define GTRY6xx_SHOW_DBG_H_2	gpio_out_set(TEST_DEBUG_IO_2)
#define GTRY6xx_SHOW_DBG_L_2	gpio_out_clear(TEST_DEBUG_IO_2)
#define GTRY6xx_SHOW_DBG_H_3	gpio_out_set(TEST_DEBUG_IO_3)
#define GTRY6xx_SHOW_DBG_L_3	gpio_out_clear(TEST_DEBUG_IO_3)

extern unsigned char sys_mode_get(void);

extern void sys_mode_set(uint8_t mode);

#if LCD_MODULE_ENABLE
extern void lcd_bl_pwm_init(uint8_t brightness);
extern void lcd_bl_set_brightness(uint8_t brightness);
extern void lcd_bl_pwm_off(void);
#endif

#if WDT_TEST_MODE
	void wdt_hal_cfg(void);
	void wdt_hal_feed_dog(void);
#else
	#define wdt_hal_cfg()
	#define wdt_hal_feed_dog()
#endif
#endif /* SRC_DEMO_APP_CONFIG_H_ */
