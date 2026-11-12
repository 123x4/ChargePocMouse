/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <usb_device_config.h>
#include <usb.h>
#include <usb_device.h>
#include <usb_device_class.h>
#include <usb_device_hid.h>
#include <usb_device_ch9.h>
#include <usb_device_descriptor.h>
#include <composite.h>
#include <hid_keyboard.h>
#include <hid_mouse.h>
#include <fsl_device_registers.h>
#include <fsl_debug_console.h>
#include <pin_mux.h>
#include <clock_config.h>
#include <board.h>
#include <dma_config.h>
#include <led.h>
#include <Flash_iap.h>
#include <flash_zb25v.h>

#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
#include <fsl_sysmpu.h>
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

#if (USB_DEVICE_CONFIG_HID < 2U)
#error USB_DEVICE_CONFIG_HID need to > 1U, Please change the MARCO USB_DEVICE_CONFIG_HID in file "usb_device_config.h".
#endif

#include <fsl_power.h>
#include <usb_phy.h>
#include <fsl_spi.h>
#include <fsl_dma.h>
#include <fsl_spi_dma.h>
#include <fsl_inputmux.h>
#include <hall_spi.h>
#include <fntab_mode.h>
#include <ctimer.h>
#include <resume.h>
#include <testapp_communication.h>
#include <driverapp_communication.h>
#include <macro.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_InitHardware(void);
void USB_DeviceClockInit(void);
void USB_DeviceIsrEnable(void);
#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTaskFn(void *deviceHandle);
#endif

static usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param);
static void USB_DeviceApplicationInit(void);

void hanle_hall_data_in_callback(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

spi_master_config_t masterConfig 	= {0};
uint32_t sourceClock             = 0U;

uint8_t Tick_125us = 0;
uint8_t Tick_10ms_LEDUpdatePWM = 0;
uint8_t Tick_10ms = 0;
uint8_t Tick_FnTabCycle = 2;

uint8_t Flag_USB_Connect_First = 0;

uint16_t Hall_Initial_Fail_Flag = 0;
uint8_t Sample_Hall_Once = 1;
uint8_t TestAPP_ReportData_Type = 0;
uint8_t TestBuff[64];
uint8_t device_mode = MODE_NORMAL;

uint8_t Begin_cOUNT = 1;

uint8_t Continue_Send_ESC_FLAG =0;
uint8_t FenDuanZhi_Break = 1;
uint8_t Fn_Tab_press_FLAG = 0;
uint8_t if_adjust_all_mode = 0;
uint8_t Check_Cycle_Time = 0;
uint8_t Need_TO_ChangeMode_Flag = 0;
uint8_t Need_Update_LED_CNT = 0;
uint8_t  mcu_color_order = 0;
uint8_t  RY_CMD = 0;
uint8_t  Highlight_CMD = 0;
uint8_t AllLedon_Mode = 0;
uint8_t AllLedon_Light = 6;
uint8_t Should_Send_Highlight_First = 0;
uint8_t Enter_FN_TAB_MODE = 0;
uint8_t if_can_send_data = 1;
uint8_t Jumpto_WokeMdoe_From_CMDMode = 0;

spi_transfer_t 		xfer            = {0};

uint8_t Key_Make_Value = 10;
uint8_t Key_Breake_Value  = 5;

uint8_t Send_TriggerData_TO_PC_FLAG = 0;
uint8_t Trigger_had_Change_FLAG = 0;

uint8_t normKbBuffIndex = 2;
uint8_t fnormKeyUpdate_backup = 0;
uint8_t fbitKeyUpdate = 0;
uint8_t fnormKeyUpdate = 0;
uint8_t Keycode_index = 2;

uint32_t Current_FRAME_NR = 0;
uint32_t Last_FRAME_NR = 0;

uint8_t normBuffer[64];
uint8_t srcBuff[BUFFER_SIZE];
uint8_t destBuff[BUFFER_SIZE];
uint8_t CMDBuff[BUFFER_SIZE];
uint8_t Receive_OriginalSampleData_Buff[160];
unsigned char keycode_buff[64] = {0};

uint8_t upgrade_status=SPI_UPGRADE_WAIT;
uint32_t subpacketnum=0;
static unsigned char updatecmd = 0;

uint8_t datanumber=0;

unsigned char* Custom_buf_in;
unsigned char* Custom_buf_out;


static unsigned char upgrade_wrdata[SPI_T_R_MAX_NUM];
static unsigned char upgrade_rddata[SPI_T_R_MAX_NUM];

static unsigned char upgrade_flag=0;
static unsigned char upgrade_number=0;
static unsigned char goto_boot_flag=0;

static unsigned char First_goto_boot_flag=0;

//------------------------------------------------------------------------------------------
#if DMA_TEST

volatile bool isTransferCompleted = false;
dma_handle_t masterTxHandle;
dma_handle_t masterRxHandle;
spi_dma_handle_t masterHandle;

uint8_t masterRxData[TRANSFER_SIZE] = {0};
uint8_t masterTxData[TRANSFER_SIZE] = {0};
uint32_t masterTxData_DMA[TRANSFER_SIZE] = {0};
uint32_t masterRxData_DMA[TRANSFER_SIZE] = {0};
uint32_t GPIO_Control_Data[40] = {0};

static dma_handle_t s_DMA_Handle0;
static dma_handle_t s_DMA_Handle1;
static dma_handle_t s_DMA_Handle2;
static dma_handle_t s_DMA_Handle3;
static volatile bool s_Transfer0_Done = false;
static volatile bool s_Transfer1_Done = false;
static volatile bool s_Transfer2_Done = false;
static volatile bool s_Transfer3_Done = false;

DMA_ALLOCATE_LINK_DESCRIPTORS(s_dma_table1, 20U);
#endif
//------------------------------------------------------------------------------------------

extern usb_device_class_struct_t g_UsbDeviceHidMouseConfig;
extern usb_device_class_struct_t g_UsbDeviceHidKeyboardConfig;
extern usb_device_class_struct_t g_UsbDeviceHidBitmapConfig;
extern usb_device_class_struct_t g_UsbDeviceHidCustomConfig;
usb_device_composite_struct_t g_UsbDeviceComposite;

/* Set class configurations */
usb_device_class_config_struct_t g_CompositeClassConfig[USB_COMPOSITE_INTERFACE_COUNT] = {
    {
        USB_DeviceHidKeyboardCallback, /* HID keyboard class callback pointer */
        (class_handle_t)NULL,          /* The HID class handle, This field is set by USB_DeviceClassInit */
        &g_UsbDeviceHidKeyboardConfig, /* The HID keyboard configuration, including class code, subcode, and protocol,
                                  class
                                  type, transfer type, endpoint address, max packet size, etc.*/
    },
    {
        USB_DeviceHidMouseCallback, /* HID mouse class callback pointer */
        (class_handle_t)NULL,       /* The HID class handle, This field is set by USB_DeviceClassInit */
        &g_UsbDeviceHidMouseConfig, /* The HID mouse configuration, including class code, subcode, and protocol, class
                               type,
                               transfer type, endpoint address, max packet size, etc.*/
    },
   {
		USB_DeviceHidBitmapCallback, /* HID mouse class callback pointer */
		(class_handle_t)NULL,       /* The HID class handle, This field is set by USB_DeviceClassInit */
		&g_UsbDeviceHidBitmapConfig, /* The HID mouse configuration, including class code, subcode, and protocol, class
								   type,
								   transfer type, endpoint address, max packet size, etc.*/
	},
    {
        USB_DeviceHidCustomCallback, /* HID mouse class callback pointer */
        (class_handle_t)NULL,       /* The HID class handle, This field is set by USB_DeviceClassInit */
        &g_UsbDeviceHidCustomConfig, /* The HID mouse configuration, including class code, subcode, and protocol, class
                                   type,
                                   transfer type, endpoint address, max packet size, etc.*/
    }
};

/* Set class configuration list */
usb_device_class_config_list_struct_t g_UsbDeviceCompositeConfigList = {
    g_CompositeClassConfig,        /* Class configurations */
    USB_DeviceCallback,            /* Device callback pointer */
    USB_COMPOSITE_INTERFACE_COUNT, /* Class count */
};

static dma_channel_trigger_t s_channelTrigger = {
    .type  = kDMA_FallingEdgeTrigger,
    .burst = kDMA_SingleTransfer,
    .wrap  = kDMA_NoWrap,
};

/*******************************************************************************
 * Code
 ******************************************************************************/
#if (defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U))
void USB0_IRQHandler(void)
{
    USB_DeviceLpcIp3511IsrFunction(g_UsbDeviceComposite.deviceHandle);
}
#endif
#if (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
void USB1_IRQHandler(void)
{
    //hall_mcu_TX_disable();
	//if(Put_Data_To_EP_FLAG)
	{
	//	INterupt_CNT++;
	}
    USB_DeviceLpcIp3511IsrFunction(g_UsbDeviceComposite.deviceHandle);
    //if(INterupt_CNT<2)
    {
    //	usb_TEST = 1;
    //	Check_Cycle_Time++;
    //	if(Check_Cycle_Time>4)
    //	{
    //		Check_Cycle_Time = 0;
    //	}
    }
    //else
    {
    //	INterupt_CNT = 0;
    //	Put_Data_To_EP_FLAG = 0;
    }
    //hall_mcu_TX_enable();
}
#endif
void USB_DeviceClockInit(void)
{
#if defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U)
    /* enable USB IP clock */
    CLOCK_EnableUsbfs0DeviceClock(kCLOCK_UsbfsSrcFro, CLOCK_GetFroHfFreq());
#if defined(FSL_FEATURE_USB_USB_RAM) && (FSL_FEATURE_USB_USB_RAM)
    for (int i = 0; i < FSL_FEATURE_USB_USB_RAM; i++)
    {
        ((uint8_t *)FSL_FEATURE_USB_USB_RAM_BASE_ADDRESS)[i] = 0x00U;
    }
#endif

#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)
    /* enable USB IP clock */
    CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_UsbPhySrcExt, BOARD_XTAL0_CLK_HZ);
    CLOCK_EnableUsbhs0DeviceClock(kCLOCK_UsbSrcUnused, 0U);
    USB_EhciPhyInit(CONTROLLER_ID, BOARD_XTAL0_CLK_HZ, NULL);
#if defined(FSL_FEATURE_USBHSD_USB_RAM) && (FSL_FEATURE_USBHSD_USB_RAM)
    for (int i = 0; i < FSL_FEATURE_USBHSD_USB_RAM; i++)
    {
        ((uint8_t *)FSL_FEATURE_USBHSD_USB_RAM_BASE_ADDRESS)[i] = 0x00U;
    }
#endif
#endif
}
void USB_DeviceIsrEnable(void)
{
    uint8_t irqNumber;
#if defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U)
    uint8_t usbDeviceIP3511Irq[] = USB_IRQS;
    irqNumber                    = usbDeviceIP3511Irq[CONTROLLER_ID - kUSB_ControllerLpcIp3511Fs0];
#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)
    uint8_t usbDeviceIP3511Irq[] = USBHSD_IRQS;
    irqNumber                    = usbDeviceIP3511Irq[CONTROLLER_ID - kUSB_ControllerLpcIp3511Hs0];
#endif
    /* Install isr, set priority, and enable IRQ. */
    NVIC_SetPriority((IRQn_Type)irqNumber, USB_DEVICE_INTERRUPT_PRIORITY);
    EnableIRQ((IRQn_Type)irqNumber);
}
#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTaskFn(void *deviceHandle)
{
#if defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U)
    USB_DeviceLpcIp3511TaskFunction(deviceHandle);
#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)
    USB_DeviceLpcIp3511TaskFunction(deviceHandle);
#endif
}
#endif

/* The Device callback */
static usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;
    uint16_t *temp16   = (uint16_t *)param;
    uint8_t *temp8     = (uint8_t *)param;

    switch (event)
    {
        case kUSB_DeviceEventBusReset:
        {
            /* USB bus reset signal detected */
            g_UsbDeviceComposite.attach               = 0U;
            g_UsbDeviceComposite.currentConfiguration = 0U;
            g_UsbDeviceComposite.remoteWakeup         = 0U;
            g_UsbDeviceComposite.suspend              = kStatus_MouseIdle;
            g_UsbDeviceComposite.isResume             = 0U;
            error                                     = kStatus_USB_Success;
#if (defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)) || \
    (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
            /* Get USB speed to configure the device, including max packet size and interval of the endpoints. */
            if (kStatus_USB_Success == USB_DeviceClassGetSpeed(CONTROLLER_ID, &g_UsbDeviceComposite.speed))
            {
                USB_DeviceSetSpeed(handle, g_UsbDeviceComposite.speed);
            }
#endif
        }
        break;

#if (defined(USB_DEVICE_CONFIG_DETACH_ENABLE) && (USB_DEVICE_CONFIG_DETACH_ENABLE > 0U))
        case kUSB_DeviceEventAttach:
        {
        	g_UsbDeviceComposite.attach = 1;
#if (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)) || \
    (defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U))
#else
            /*Add one delay here to make the DP pull down long enough to allow host to detect the previous
             * disconnection.*/
            SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
            USB_DeviceRun(g_UsbDeviceComposite.deviceHandle);
#endif
        }
        break;
        case kUSB_DeviceEventDetach:
        {
        	g_UsbDeviceComposite.attach = 0;
#if (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)) || \
    (defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U))

#else
            USB_DeviceStop(g_UsbDeviceHidMouse.deviceHandle);
#endif
        }
        break;
#endif
        case kUSB_DeviceEventSuspend:
        {
            /* USB device bus suspend signal detected */
            if (g_UsbDeviceComposite.attach)
            {
                //usb_echo("USB device start suspend\r\n");
                USB_ControllerSuspended();
                g_UsbDeviceComposite.startTick = g_UsbDeviceComposite.hwTick;
                g_UsbDeviceComposite.suspend   = kStatus_MouseStartSuspend;
                error                         = kStatus_USB_Success;
            }
        }
        break;
        case kUSB_DeviceEventResume:
        {
            /* USB device bus resume signal detected */
            if ((g_UsbDeviceComposite.attach) && (kStatus_MouseIdle != g_UsbDeviceComposite.suspend))
            {
            	g_UsbDeviceComposite.isResume = 1U;
                //usb_echo("USB device start resume\r\n");
                error = kStatus_USB_Success;
            }
        }
        break;
        case kUSB_DeviceEventSetRemoteWakeup:
            if (param)
            {
                if (g_UsbDeviceComposite.attach)
                {
                	g_UsbDeviceComposite.remoteWakeup = *temp8;
                    //usb_echo("USB device remote wakeup state: %d\r\n", g_UsbDeviceComposite.remoteWakeup);
                    error = kStatus_USB_Success;
                }
            }
            break;
        case kUSB_DeviceEventSetConfiguration:
            if (0U == (*temp8))
            {
            	g_UsbDeviceComposite.attach               = 0U;
            	g_UsbDeviceComposite.currentConfiguration = 0U;
            	g_UsbDeviceComposite.remoteWakeup         = 0U;
            	g_UsbDeviceComposite.suspend              = kStatus_MouseIdle;
            	g_UsbDeviceComposite.isResume             = 0U;
                error                                     = kStatus_USB_Success;
            }
            else if (USB_COMPOSITE_CONFIGURE_INDEX == (*temp8))
            {
                /* Set device configuration request */
                g_UsbDeviceComposite.attach = 1U;
                USB_DeviceHidMouseSetConfigure(g_UsbDeviceComposite.hidMouseHandle, *temp8);
                USB_DeviceHidKeyboardSetConfigure(g_UsbDeviceComposite.hidKeyboardHandle, *temp8);
                USB_DeviceHidBitmapSetConfigure(g_UsbDeviceComposite.hidBitmapHandle, *temp8);
                USB_DeviceHidCustomSetConfigure(g_UsbDeviceComposite.hidCustomHandle, *temp8);
                g_UsbDeviceComposite.currentConfiguration = *temp8;
                error                                     = kStatus_USB_Success;
                usb_init_last_command_done = 1;
            }
            else
            {
                /* no action, return kStatus_USB_InvalidRequest. */
            }
            break;
        case kUSB_DeviceEventSetInterface:
            if (g_UsbDeviceComposite.attach)
            {
                /* Set device interface request */
                uint8_t interface        = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                uint8_t alternateSetting = (uint8_t)(*temp16 & 0x00FFU);
                if (interface < USB_COMPOSITE_INTERFACE_COUNT)
                {
                    g_UsbDeviceComposite.currentInterfaceAlternateSetting[interface] = alternateSetting;
                    USB_DeviceHidMouseSetInterface(g_UsbDeviceComposite.hidMouseHandle, interface, alternateSetting);
                    USB_DeviceHidKeyboardSetInterface(g_UsbDeviceComposite.hidKeyboardHandle, interface,alternateSetting);
                    USB_DeviceHidBitmapSetInterface(g_UsbDeviceComposite.hidBitmapHandle, interface,alternateSetting);
                    USB_DeviceHidKeyboardSetInterface(g_UsbDeviceComposite.hidCustomHandle, interface,alternateSetting);

                    error = kStatus_USB_Success;
                    usb_init_last_command_done = 1;
                }
            }
            break;
        case kUSB_DeviceEventGetConfiguration:
            if (param)
            {
                /* Get current configuration request */
                *temp8 = g_UsbDeviceComposite.currentConfiguration;
                error  = kStatus_USB_Success;
            }
            break;
        case kUSB_DeviceEventGetInterface:
            if (param)
            {
                /* Get current alternate setting of the interface request */
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                if (interface < USB_COMPOSITE_INTERFACE_COUNT)
                {
                    *temp16 = (*temp16 & 0xFF00U) | g_UsbDeviceComposite.currentInterfaceAlternateSetting[interface];
                    error   = kStatus_USB_Success;
                }
                else
                {
                    error = kStatus_USB_InvalidRequest;
                }
            }
            break;
        case kUSB_DeviceEventGetDeviceDescriptor:
            if (param)
            {
                /* Get device descriptor request */
                error = USB_DeviceGetDeviceDescriptor(handle, (usb_device_get_device_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetConfigurationDescriptor:
            if (param)
            {
                /* Get device configuration descriptor request */
                error = USB_DeviceGetConfigurationDescriptor(handle,
                                                             (usb_device_get_configuration_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetStringDescriptor:
            if (param)
            {
                /* Get device string descriptor request */
                error = USB_DeviceGetStringDescriptor(handle, (usb_device_get_string_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetHidDescriptor:
            if (param)
            {
                /* Get hid descriptor request */
                error = USB_DeviceGetHidDescriptor(handle, (usb_device_get_hid_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetHidReportDescriptor:
            if (param)
            {
                /* Get hid report descriptor request */
                error =
                    USB_DeviceGetHidReportDescriptor(handle, (usb_device_get_hid_report_descriptor_struct_t *)param);
            }
            break;
#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
        case kUSB_DeviceEventGetDeviceQualifierDescriptor:
            if (param)
            {
                /* Get Qualifier descriptor request */
                error = USB_DeviceGetDeviceQualifierDescriptor(
                    handle, (usb_device_get_device_qualifier_descriptor_struct_t *)param);
            }
            break;
#endif
        case kUSB_DeviceEventGetHidPhysicalDescriptor:
            if (param)
            {
                /* Get hid physical descriptor request */
                error = USB_DeviceGetHidPhysicalDescriptor(handle,
                                                           (usb_device_get_hid_physical_descriptor_struct_t *)param);
            }
            break;
        default:
            break;
    }

    return error;
}

/* Application initialization */
static void USB_DeviceApplicationInit(void)
{
    USB_DeviceClockInit();
#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
    SYSMPU_Enable(SYSMPU, 0);
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

    /* Set composite device to default state */
    g_UsbDeviceComposite.speed             = USB_SPEED_HIGH;
    g_UsbDeviceComposite.attach            = 0U;
    g_UsbDeviceComposite.hidMouseHandle    = (class_handle_t)NULL;
    g_UsbDeviceComposite.hidKeyboardHandle = (class_handle_t)NULL;
    g_UsbDeviceComposite.hidBitmapHandle   = (class_handle_t)NULL;
    g_UsbDeviceComposite.hidCustomHandle   = (class_handle_t)NULL;
    g_UsbDeviceComposite.deviceHandle      = NULL;
    g_UsbDeviceComposite.remoteWakeup = 0U;
    g_UsbDeviceComposite.suspend      = kStatus_MouseIdle;
    g_UsbDeviceComposite.selfWakeup   = 0U;
    g_UsbDeviceComposite.isResume     = 0U;

    if (kStatus_USB_Success !=
        USB_DeviceClassInit(CONTROLLER_ID, &g_UsbDeviceCompositeConfigList, &g_UsbDeviceComposite.deviceHandle))
    {
        //usb_echo("USB device composite demo init failed\r\n");
        return;
    }
    else
    {
		#if UART_DEBUG
			usb_echo("USB device composite demo\r\n");
		#endif
        /* Get the HID keyboard class handle */
        g_UsbDeviceComposite.hidKeyboardHandle = g_UsbDeviceCompositeConfigList.config[0].classHandle;
        /* Get the HID mouse class handle */
        g_UsbDeviceComposite.hidMouseHandle = g_UsbDeviceCompositeConfigList.config[1].classHandle;
        g_UsbDeviceComposite.hidBitmapHandle = g_UsbDeviceCompositeConfigList.config[2].classHandle;
        g_UsbDeviceComposite.hidCustomHandle = g_UsbDeviceCompositeConfigList.config[3].classHandle;

        USB_DeviceHidKeyboardInit(&g_UsbDeviceComposite);
        USB_DeviceHidMouseInit(&g_UsbDeviceComposite);
        USB_DeviceHidBitmapInit(&g_UsbDeviceComposite);
        USB_DeviceHidCustomInit(&g_UsbDeviceComposite);
    }

    USB_DeviceIsrEnable();

    /* Start the device function */
    /*Add one delay here to make the DP pull down long enough to allow host to detect the previous disconnection.*/
    SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    USB_DeviceRun(g_UsbDeviceComposite.deviceHandle);
}

#define USB_RESET_TIME_OUT_TH  (1000*10) //15s
unsigned int usb_reset_counter =0;
unsigned char usb_reset_start = 0;
void usb_reset_check(void)
{
	if(usb_reset_start)
	{
		usb_reset_counter++;
		if(usb_reset_counter>USB_RESET_TIME_OUT_TH)
		{
			NVIC_SystemReset();
		}
	}
}

void Custom_driver_init(void)
{
	Custom_buf_out = USB_DeviceHidCustomGetReportBuffOut();
	Custom_buf_in = USB_DeviceHidCustomGetReportBuffIn();
}

void SPI_MASTER_IRQHandler(void)
{
	//hall_mcu_TX_disable();
	//BuffBACKUP[0] = ( SPI_FIFOSTAT(EXAMPLE_SPI_MASTER) );
	//BuffBACKUP[1] = ( SPI_FIFOSTAT(EXAMPLE_SPI_MASTER) )>>8;
	//BuffBACKUP[2] = ( SPI_FIFOSTAT(EXAMPLE_SPI_MASTER) )>>16;
	//BuffBACKUP[3] = ( SPI_FIFOSTAT(EXAMPLE_SPI_MASTER) )>>24;
	SPI_DisableInterrupts(SPI7_MASTER, kSPI_RxLvlIrq);
	//SDK_ISR_EXIT_BARRIER;
	//hall_mcu_TX_enable();
}

/* User callback function for DMA transfer. */
void DMA0_Callback(dma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    if (transferDone)
    {
        s_Transfer0_Done = true;
    }
}

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION)) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif
{
    uint32_t i = 0;
    uint32_t j = 0U;
    unsigned char Data[64] = {0};
    spi_transfer_t masterXfer;

    /* set BOD VBAT level to 1.65V */
    POWER_SetBodVbatLevel(kPOWER_BodVbatLevel1650mv, kPOWER_BodHystLevel50mv, false);
    //------------------------------------------------------------------------------------------
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    //------------------------------------------------------------------------------------------
#if 1
    /* Attach 12 MHz clock to SPI2-SPI7 and uart */
//	CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM6);	//for spi 7
//	CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM4);	//for spi 4
	CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM7);	//for spi 3
	CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM5);	//for spi 2
	CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM4);	//for spi 1
	CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM1);	//for spi 0
	//------------------------------------------------------------------------------------------
    /* Reset FLEXCOMM for SPI */
//    RESET_PeripheralReset(kFC6_RST_SHIFT_RSTn);	//for spi 7
//    RESET_PeripheralReset(kFC4_RST_SHIFT_RSTn);	//for spi 4
    RESET_PeripheralReset(kFC7_RST_SHIFT_RSTn);	//for spi 3
    RESET_PeripheralReset(kFC5_RST_SHIFT_RSTn);	//for spi 2
    RESET_PeripheralReset(kFC4_RST_SHIFT_RSTn);	//for spi 1
    RESET_PeripheralReset(kFC1_RST_SHIFT_RSTn);	//for spi 0
    //------------------------------------------------------------------------------------------
#endif
    /* pin init */
    BOARD_InitPins();
    //------------------------------------------------------------------------------------------
    /* clock init */
    BOARD_BootClockPLL150M();
    //------------------------------------------------------------------------------------------
    /* uart init */
    BOARD_InitDebugConsole();
    //------------------------------------------------------------------------------------------
#if 1
    NVIC_ClearPendingIRQ(USB0_IRQn);
    NVIC_ClearPendingIRQ(USB0_NEEDCLK_IRQn);
    NVIC_ClearPendingIRQ(USB1_IRQn);
    NVIC_ClearPendingIRQ(USB1_NEEDCLK_IRQn);
    //------------------------------------------------------------------------------------------
    POWER_DisablePD(kPDRUNCFG_PD_USB0_PHY); /*< Turn on USB0 Phy */
    POWER_DisablePD(kPDRUNCFG_PD_USB1_PHY); /*< Turn on USB1 Phy */
    //------------------------------------------------------------------------------------------
    /* Use 12 MHz clock for some of the Ctimers */
    CLOCK_AttachClk(kFRO_HF_to_CTIMER2);
    //------------------------------------------------------------------------------------------
    /* reset the IP to make sure it's in reset state. */
    RESET_PeripheralReset(kUSB0D_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB0HSL_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB0HMR_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB1H_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB1D_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB1_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSB1RAM_RST_SHIFT_RSTn);
    //------------------------------------------------------------------------------------------
#if (defined USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS)
    CLOCK_EnableClock(kCLOCK_Usbh1);
    /* Put PHY powerdown under software control */
    *((uint32_t *)(USBHSH_BASE + 0x50)) = USBHSH_PORTMODE_SW_PDCOM_MASK;
    /* According to reference mannual, device mode setting has to be set by access usb host register */
    *((uint32_t *)(USBHSH_BASE + 0x50)) |= USBHSH_PORTMODE_DEV_ENABLE_MASK;
    /* enable usb1 host clock */
    CLOCK_DisableClock(kCLOCK_Usbh1);
#endif
    //------------------------------------------------------------------------------------------
#if (defined USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS)
    POWER_DisablePD(kPDRUNCFG_PD_USB0_PHY); /*< Turn on USB Phy */
    CLOCK_SetClkDiv(kCLOCK_DivUsb0Clk, 1, false);
    CLOCK_AttachClk(kFRO_HF_to_USB0_CLK);
    /* enable usb0 host clock */
    CLOCK_EnableClock(kCLOCK_Usbhsl0);
    /*According to reference mannual, device mode setting has to be set by access usb host register */
    *((uint32_t *)(USBFSH_BASE + 0x5C)) |= USBFSH_PORTMODE_DEV_ENABLE_MASK;
    /* disable usb0 host clock */
    CLOCK_DisableClock(kCLOCK_Usbhsl0);
#endif
    //------------------------------------------------------------------------------------------
    USB_DeviceApplicationInit();
    //------------------------------------------------------------------------------------------
#if ((defined(USB_DEVICE_CONFIG_LOW_POWER_MODE)) && (USB_DEVICE_CONFIG_LOW_POWER_MODE > 0U))
    USB_LowpowerModeInit();
#endif
    //------------------------------------------------------------------------------------------
#if ((defined(USB_DEVICE_CONFIG_LOW_POWER_MODE)) && (USB_DEVICE_CONFIG_LOW_POWER_MODE > 0U))
    //HW_TimerControl(1);
#endif
    //------------------------------------------------------------------------------------------
    //wait for usb init ok
	while(usb_init_last_command_done==0)
	{
		wwdt_feed();
#if USB_DEVICE_CONFIG_USE_TASK
        USB_DeviceTaskFn(g_UsbDeviceComposite.deviceHandle);
#endif
		usb_reset_check();
	}
#endif
	//------------------------------------------------------------------------------------------
	hall_mcu_delay_ms(10);
    //------------------------------------------------------------------------------------------
    CTimer_Init();

    //------------------------------------------------------------------------------------------
    /* dma init and config */
    DMA_CONFIG();
	//------------------------------------------------------------------------------------------
    hall_mcu_delay_ms(1);
    //------------------------------------------------------------------------------------------
	/* 上电后先断电上电一下sensor */
	HALL_RGB_POWERON();

    //------------------------------------------------------------------------------------------
	Custom_driver_init();
	//------------------------------------------------------------------------------------------
    hall_mcu_delay_ms(10);
    //------------------------------------------------------------------------------------------
    /* Flash ZB25V init */
    //FlashZB25V_Init();
	//------------------------------------------------------------------------------------------
    /* 5528 FLASH init */
    Flash_IntervalFlash_Init();
    //------------------------------------------------------------------------------------------
    /* Variable init */
    Variable_init();
    //------------------------------------------------------------------------------------------
    /* Hall sensor config */
    HallSensor_INIT();
    //------------------------------------------------------------------------------------------
    Hall_Sample_Rawdata();
    //------------------------------------------------------------------------------------------
    //Hall_read_REG();
    //------------------------------------------------------------------------------------------
	device_mode = MODE_NORMAL;
	//------------------------------------------------------------------------------------------
	/* 定时器开启 */
	//CTimer_Start();
    while (1U)
    {
    	wwdt_feed();
#if USB_DEVICE_CONFIG_USE_TASK
		USB_DeviceTaskFn(g_UsbDeviceComposite.deviceHandle);
#endif

#if ((defined(USB_DEVICE_CONFIG_LOW_POWER_MODE)) && (USB_DEVICE_CONFIG_LOW_POWER_MODE > 0U))
    	USB_DeviceSuspendResumeTask();
#endif
		if((Tick_125us))
		{
//			usb_echo("enter while \r\n");
			//---------------------------------------------------------------------------------------------
			Tick_125us = 0;

			//HALLChain_Check_FREQ_DETECH();
#if 1
//			if(Tick_SETGain_Need_Wait < 10)
//			{
//				Tick_SETGain_Need_Wait++;
//			}
			//---------------------------------------------------------------------------------------------
//			if(g_UsbDeviceComposite.attach)
			{
//				if(Flag_Suspend == 0)
				{
					//----------------------------------------------------------------------------------------------
					/* 检测ESD */
					//HALL_ESD_CheckAndReset();
					//----------------------------------------------------------------------------------------------
//					if(Flag_Need_adjust_Gain)
//					{
//						Flag_Need_adjust_Gain = 0;
//						Tick_SETGain_Need_Wait = 0;
//						Sample_Hall_Once = 1;
//						Tick_10ms = 0;
//						//Hall_set_gain();
//					}
//					else
					{
						#if 0
						if(++Tick_10ms_LEDUpdatePWM > 80)
						{
							/* 每10ms进来检测一次是否需要更新RGB，更新的话直接写RGB */
							/* 写RGB=353us */
							LED_Handle();
						}
						else
						#endif
						{
							if(device_mode == MODE_NORMAL)
							{
								{
									//if(Tick_SETGain_Need_Wait >= 10)
									{
//										hall_mcu_delay_us(100);
//										TEST_PIN_SET_LOW();
//										hall_mcu_delay_us(100);
//										TEST_PIN_SET_HIGH();
//										HALLChain_Check_FREQ_DETECH();
										//Hall_Sample_Rawdata();


//										TEST_PIN_SET_HIGH();
										//Hall_CLK_Command_SET(TD14_DELAY_US,TD10_DELAY_US);
//										KeyData_Process();
										//Hall_CLK_Command_SET(TD3_DELAY_US,TD5_DELAY_US);
//										TEST_PIN_SET_LOW();
									}
//									else
//									{
//										//看到写亮度后读到的raw data会异常，所以写完灯光后等ADC建立时间达成后再进行正常的raw data处理
//										Hall_Sample_Rawdata();
//									}
								}
							}
//							else if(device_mode == MODE_TEST)
//							{
//								TestMode_Process();
//							}
//							else if(device_mode == MODE_FNTAB)
//							{
//								//FNTabMode_Process();
//							}
						}
						//----------------------------------------------------------------------------------------------
						//sys_led_scan();
						//----------------------------------------------------------------------------------------------
						//ProcessMacroKey();
						//----------------------------------------------------------------------------------------------
						//DriverAPP_Process();
						//----------------------------------------------------------------------------------------------
						/* send any interface data. */
						USB_DeviceHidKeyboardReport();
						//----------------------------------------------------------------------------------------------
						/* Check whether the MCU Flash needs to be updated. */
						//VariableUpdate_MCUFlash_Process();
						//-----------------------------------------------------------------------------------------------
						//Reset_DefultSetting();
						//-----------------------------------------------------------------------------------------------
					}
				}
			}
//			else
//			{
//				if(Flag_USB_Connect_First == 0)
//				{
//					//usb_echo("Flag_USB_Connect_First\n");
//					Flag_USB_Connect_First = 1;
//		            //----------------------------------------------------------------------------------------------------------------------------------------
//		            /* hall进睡眠 */
//		            Hall_To_Sleep();
//		            //----------------------------------------------------------------------------------------------------------------------------------------
//				}
//			}
#endif
		}
    }
}
