/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_COMPOSITE_H__
#define __USB_DEVICE_COMPOSITE_H__

#include "usb_device_class.h"
#include "usb_device_descriptor.h"
#include "fsl_spi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)
#define CONTROLLER_ID kUSB_ControllerEhci0
#endif
#if defined(USB_DEVICE_CONFIG_KHCI) && (USB_DEVICE_CONFIG_KHCI > 0U)
#define CONTROLLER_ID kUSB_ControllerKhci0
#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U)
#define CONTROLLER_ID kUSB_ControllerLpcIp3511Fs0
#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)
#define CONTROLLER_ID kUSB_ControllerLpcIp3511Hs0
#endif

#define DMA_TEST  1
#define DEMO_DMA_CHANNEL1                1
#define DEMO_DMA_CHANNEL0                0
#define DEMO_DMA_CHANNEL2                2
#define DEMO_DMA_CHANNEL3                3


/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define SPI_MASTER_IRQHandler       	FLEXCOMM3_IRQHandler

#define SPI_UPGRADE_WAIT				0xA0
#define SPI_UPGRADE_SWITCH				0xA1
#define SPI_UPGRADE_DOING				0xA2
#define SPI_UPGRADE_SUCCESS				0xA3
#define SPI_UPGRADE_FAILED				0xA4

#define CMD_UPGRADE_READY				0x30
#define CMD_UPGRADE_START				0x31
#define CMD_UPGRADE_DATA				0x32
#define CMD_UPGRADE_END					0x33
#define CMD_UPGRADE_ACK					0x34
#define CMD_UPGRADE_NACK				0x35
//------------------------------------------------------------------------------------------
/* KB different mode */
#define Normal_MODE						0x00
#define FN_TAB_MODE						0x01
#define Send_CMD_MODE					0x02
#define Get_OriginalSampleData_MODE		0x03

#define BUFFER_SIZE (80)         //5*16
#define BUFFER_SIZE_SLAVE0 (30)
#define BUFFER_SIZE_SLAVE1 (30)
#define BUFFER_SIZE_SLAVE2 (30)
#define BUFFER_SIZE_SLAVE3 (30)
#define BUFFER_SIZE_SLAVE4 (30)
/************************************************************************************************/

#define Group  	 	5
#define Key_Num 	21

#define ADJUST_MIN	2
#define ADJUST_MAX	40

#define SPI_T_R_MAX_NUM							64

#define UART_DEBUG		1

#define USB_DEVICE_INTERRUPT_PRIORITY (3U)

typedef enum _usb_hid_status
{
    kStatus_MouseIdle = 0U,
    kStatus_MouseStartSuspend,
    kStatus_MouseSuspending,
    kStatus_MouseSuspended,
    kStatus_MouseStartResume,
    kStatus_MouseResuming,
    kStatus_MouseResumed,
} usb_hid_status_t;

typedef struct _usb_device_composite_struct
{
     volatile uint64_t hwTick;
     uint64_t startTick;
     usb_device_handle deviceHandle;
     class_handle_t hidMouseHandle;
     class_handle_t hidKeyboardHandle;
     class_handle_t hidBitmapHandle;
     class_handle_t hidCustomHandle;
     uint8_t *buffer;
     uint8_t currentConfiguration;
     uint8_t currentInterfaceAlternateSetting[USB_COMPOSITE_INTERFACE_COUNT];
     uint8_t speed;
     volatile uint8_t attach;
     volatile uint8_t remoteWakeup;
     volatile uint8_t selfWakeup;
     volatile uint8_t isResume;
     volatile usb_hid_status_t suspend;
} usb_device_composite_struct_t;

enum
{
	MODE_NORMAL = 0,
	MODE_TEST,
	MODE_UPGRADE,
	MODE_FNTAB,
};

extern usb_device_composite_struct_t g_UsbDeviceComposite;
extern spi_transfer_t 		xfer;


extern  uint8_t Tick_125us;
extern  uint8_t Tick_10ms;
extern  uint8_t Tick_10ms_LEDUpdatePWM;
extern  uint8_t Tick_FnTabCycle;
extern 	uint16_t Hall_Initial_Fail_Flag;
extern  uint8_t Sample_Hall_Once;
extern  uint8_t TestAPP_ReportData_Type;
extern  uint8_t TestBuff[64];
extern  uint8_t device_mode;

extern uint8_t Begin_cOUNT;

extern  uint8_t fkeyMake;
extern  uint8_t normKbBuffIndex;
extern  uint8_t fnormKeyUpdate;
extern  uint8_t fbitKeyUpdate;
extern  uint8_t Keycode_index;
//extern  uint8_t Key_Total;
extern  uint8_t mcu_color_order;
extern  uint8_t  RY_CMD;

extern uint8_t srcBuff[BUFFER_SIZE];
extern uint8_t destBuff[BUFFER_SIZE];
extern uint8_t CMDBuff[BUFFER_SIZE];
extern uint8_t Receive_OriginalSampleData_Buff[160];
extern unsigned char keycode_buff[64];

extern  uint8_t Callback_FLAG;
extern  uint8_t Key_Make_Value;
extern  uint8_t Key_Breake_Value;
extern  uint8_t Continue_Send_ESC_FLAG;
extern  uint8_t Fn_Tab_press_FLAG;
extern  uint8_t Check_Cycle_Time;
extern  uint8_t Need_TO_ChangeMode_Flag;
extern  uint8_t if_adjust_all_mode;
extern  uint8_t Need_Update_LED_CNT;
extern  uint8_t AllLedon_Light;
extern  uint8_t AllLedon_Mode;
extern	uint8_t Should_Send_Highlight_First;
extern	uint8_t Enter_FN_TAB_MODE;
extern	uint8_t Highlight_CMD;
extern	uint8_t Trigger_had_Change_FLAG;

extern	uint8_t if_can_send_data;
extern	uint8_t Send_TriggerData_TO_PC_FLAG;
extern	uint8_t Trigger_had_Change_FLAG;

extern	uint8_t Jumpto_WokeMdoe_From_CMDMode;

extern	uint32_t Current_FRAME_NR;
extern	uint32_t Last_FRAME_NR;

extern	unsigned char* Custom_buf_in;
extern	unsigned char* Custom_buf_out;

/*******************************************************************************
 * API
 ******************************************************************************/
void hanle_hall_data_in_callback(void);
void USB_DeviceTaskFn(void *deviceHandle);

#endif /* __USB_DEVICE_COMPOSITE_H__ */
