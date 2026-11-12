/*
 * custom_original.h
 *
 *  Created on: 2023年7月30日
 *      Author: Hao.Luo
 */

#ifndef RESUME_H_
#define RESUME_H_

#include "usb.h"
#include "usb_device_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if ((defined(USB_DEVICE_CONFIG_LPCIP3511FS)) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U))
#define APP_EXCLUDE_FROM_DEEPSLEEP  (kPDRUNCFG_PD_LDOFLASHNV | kPDRUNCFG_PD_USB0_PHY | kPDRUNCFG_PD_LDOMEM)
#define APP_DEEPSLEEP_WAKEUP_SOURCE (WAKEUP_GPIO_GLOBALINT0 | WAKEUP_USB0_NEEDCLK | WAKEUP_USB0)
#endif
#if ((defined(USB_DEVICE_CONFIG_LPCIP3511HS)) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
#define APP_EXCLUDE_FROM_DEEPSLEEP \
    (kPDRUNCFG_PD_LDOFLASHNV | kPDRUNCFG_PD_USB1_PHY | kPDRUNCFG_PD_LDOMEM | kPDRUNCFG_PD_LDOUSBHS | kPDRUNCFG_PD_FRO192M)
#define APP_DEEPSLEEP_WAKEUP_SOURCE (WAKEUP_GPIO_GLOBALINT0 | WAKEUP_USB1_NEEDCLK | WAKEUP_USB1 | WAKEUP_CTIMER2)
#endif
/*******************************************************************************
 * Variable
 ******************************************************************************/
extern uint8_t Flag_Suspend;
/*******************************************************************************
 * API
 ******************************************************************************/
extern void USB_DeviceSuspendResumeTask(void);
extern void USB_ControllerSuspended(void);
extern void USB_LowpowerModeInit(void);
extern void HW_TimerControl(uint8_t enable);
extern void Hall_To_Sleep(void);

#endif /* RESUME_H_ */
