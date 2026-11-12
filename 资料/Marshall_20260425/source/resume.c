/*
 * custom_original.c
 *
 */
#include <resume.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <usb_device_config.h>
#include <usb.h>
#include <usb_device.h>
#include <board.h>
#include <usb_device_class.h>
#include <usb_device_hid.h>
#include <usb_device_ch9.h>
#include <usb_device_descriptor.h>
#include <composite.h>
#include <fsl_power.h>
#include <fsl_adapter_gpio.h>
#include <fsl_adapter_timer.h>
#include <dma_config.h>
#include <hall_spi.h>
#include <fsl_debug_console.h>
#include <pin_mux.h>
#include <ctimer.h>
#include <hid_keyboard.h>
#include <led.h>


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variable
 ******************************************************************************/
#define TIMER_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
uint32_t g_halTimerHandle[(HAL_TIMER_HANDLE_SIZE + 3) / 4];
uint32_t g_gpioHandle[(HAL_GPIO_HANDLE_SIZE + 3) / 4];
uint8_t Flag_Suspend = 0;
/*******************************************************************************
 * API
 ******************************************************************************/
void BOARD_DeinitPins(void);
void SW_IntControl(uint8_t enable);
void USB_DeviceSuspendResumeTask(void);
char *SW_GetName(void);
void HW_TimerControl(uint8_t enable);
void USB_PreLowpowerMode(void);
void USB_PostLowpowerMode(void);
uint8_t USB_EnterLowpowerMode(void);
void USB_ControllerSuspended(void);
void USB_LowpowerModeInit(void);

void Hall_To_Sleep(void)
{
	Hall_CLK_Command_SET(TD11_DELAY_US_FreqDouble, 1);
}
void Hall_To_WakeUp(void)
{
	Hall_CLK_Command_SET(TD12_DELAY_US_FreqDouble, TD13_DELAY_US_FreqDouble);
}
/*******************************************************************************/
void USB0_NEEDCLK_IRQHandler(void)
{
}
void USB1_NEEDCLK_IRQHandler(void)
{
}
/*******************************************************************************/
void BOARD_DeinitPins(void)
{
}
void SW_IntControl(uint8_t enable)
{
    if (enable)
    {
    	g_UsbDeviceComposite.selfWakeup = 0U;
    }
    HAL_GpioWakeUpSetting(g_gpioHandle, enable);
}

void SW_Callback(void *param)
{
	g_UsbDeviceComposite.selfWakeup = 1U;
    SW_IntControl(0);
}

void SW_Init(void)
{
    hal_gpio_pin_config_t s_GpioInputPin;
    s_GpioInputPin.direction = kHAL_GpioDirectionIn;
    s_GpioInputPin.port      = BOARD_SW1_GPIO_PORT;
    s_GpioInputPin.pin       = BOARD_SW1_GPIO_PIN;

    HAL_GpioInit(g_gpioHandle, &s_GpioInputPin);
    HAL_GpioInstallCallback(g_gpioHandle, SW_Callback, NULL);
    HAL_GpioSetTriggerMode(g_gpioHandle, kHAL_GpioInterruptFallingEdge);
}

char *SW_GetName(void)
{
    return BOARD_SW1_NAME;
}

void HW_TimerCallback(void *param)
{
	//g_UsbDeviceComposite.hwTick++;
    //USB_DeviceUpdateHwTick(g_UsbDeviceComposite.deviceHandle, g_UsbDeviceComposite.hwTick);
}

void HW_TimerInit(void)
{
    hal_timer_config_t halTimerConfig;
    halTimerConfig.timeout            = 1000;
    halTimerConfig.srcClock_Hz        = TIMER_SOURCE_CLOCK;
    halTimerConfig.instance           = 0U;
    hal_timer_handle_t halTimerHandle = &g_halTimerHandle[0];
    HAL_TimerInit(halTimerHandle, &halTimerConfig);
    HAL_TimerInstallCallback(halTimerHandle, HW_TimerCallback, NULL);
}

void HW_TimerControl(uint8_t enable)
{
    if (enable)
    {
        HAL_TimerEnable(g_halTimerHandle);
    }
    else
    {
        HAL_TimerDisable(g_halTimerHandle);
    }
}

void USB_LowpowerModeInit(void)
{
    ////SW_Init();
    //HW_TimerInit();
}

void USB_PreLowpowerMode(void)
{
    __disable_irq();
#if ((defined(USB_DEVICE_CONFIG_LPCIP3511FS)) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U))
    if (SYSCON->USB0NEEDCLKSTAT & (SYSCON_USB0NEEDCLKSTAT_HOST_NEEDCLK_MASK))
    {
        /* enable usb0 host clock */
        CLOCK_EnableClock(kCLOCK_Usbhmr0);
        CLOCK_EnableClock(kCLOCK_Usbhsl0);
        while (SYSCON->USB0NEEDCLKSTAT & (SYSCON_USB0NEEDCLKSTAT_HOST_NEEDCLK_MASK))
        {
            __ASM("nop");
        }
        /* disable usb0 host clock */
        CLOCK_DisableClock(kCLOCK_Usbhsl0);
        CLOCK_DisableClock(kCLOCK_Usbhmr0);
    }
    NVIC_ClearPendingIRQ(USB0_NEEDCLK_IRQn);
#endif
#if ((defined(USB_DEVICE_CONFIG_LPCIP3511HS)) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
    if (SYSCON->USB1NEEDCLKSTAT & (SYSCON_USB1NEEDCLKSTAT_HOST_NEEDCLK_MASK))
    {
        /* enable usb1 host clock */
        CLOCK_EnableClock(kCLOCK_Usbh1);
        while (SYSCON->USB1NEEDCLKSTAT & (SYSCON_USB1NEEDCLKSTAT_HOST_NEEDCLK_MASK))
        {
            __ASM("nop");
        }
        /* disable usb1 host clock */
        CLOCK_DisableClock(kCLOCK_Usbh1);
    }
    NVIC_ClearPendingIRQ(USB1_NEEDCLK_IRQn);
    CLOCK_EnableClock(kCLOCK_Usbh1);
    /*According to reference mannual, device mode setting has to be set by access usb host register */
    *((uint32_t *)(USBHSH_BASE + 0x50)) |= (USBHSH_PORTMODE_SW_CTRL_PDCOM_MASK);
    *((uint32_t *)(USBHSH_BASE + 0x50)) |= USBHSH_PORTMODE_SW_PDCOM_MASK;
    CLOCK_DisableClock(kCLOCK_Usbh1);
    /* add the following code to decrease power consumption, only for L2 suspend mode */
    USBPHY->ANACTRL_CLR          = USBPHY_ANACTRL_DEV_PULLDOWN_MASK;
    USBPHY->USB1_VBUS_DETECT_CLR = USBPHY_USB1_VBUS_DETECT_PWRUP_CMPS_MASK;
    USBPHY->USB1_VBUS_DETECT_SET = USBPHY_USB1_VBUS_DETECT_PWRUP_CMPS(0x02);
#endif

    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK); /*!< Switch to 12MHz first to ensure we can change voltage without
                                          accidentally being below the voltage for current speed */
    ANACTRL->FRO192M_CTRL &= ~(ANACTRL_FRO192M_CTRL_USBCLKADJ_MASK);
    POWER_SetVoltageForFreq(12000000U); /*!< Set voltage for core */
}

uint8_t USB_EnterLowpowerMode(void)
{
    /* Enter Deep Sleep mode */
    POWER_EnterDeepSleep(APP_EXCLUDE_FROM_DEEPSLEEP, 0x0, APP_DEEPSLEEP_WAKEUP_SOURCE, 0x0);
    return kStatus_Success;
}

void USB_PostLowpowerMode(void)
{
    __enable_irq();
#if ((defined(USB_DEVICE_CONFIG_LPCIP3511HS)) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
    CLOCK_EnableClock(kCLOCK_Usbh1);
    /*According to reference mannual, device mode setting has to be set by access usb host register */
    *((uint32_t *)(USBHSH_BASE + 0x50)) &= ~USBHSH_PORTMODE_SW_PDCOM_MASK;
    *((uint32_t *)(USBHSH_BASE + 0x50)) &= ~USBHSH_PORTMODE_SW_CTRL_PDCOM_MASK;
    CLOCK_DisableClock(kCLOCK_Usbh1);
#endif
}

void USB_ControllerSuspended(void)
{
#if ((defined(USB_DEVICE_CONFIG_LPCIP3511FS)) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U))
    while (SYSCON->USB0NEEDCLKSTAT & (SYSCON_USB0NEEDCLKSTAT_DEV_NEEDCLK_MASK))
    {
        __ASM("nop");
    }
    SYSCON->USB0NEEDCLKCTRL |= SYSCON_USB0NEEDCLKCTRL_POL_FS_DEV_NEEDCLK_MASK;
#endif
#if ((defined(USB_DEVICE_CONFIG_LPCIP3511HS)) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
    while (SYSCON->USB1NEEDCLKSTAT & (SYSCON_USB1NEEDCLKSTAT_DEV_NEEDCLK_MASK))
    {
        __ASM("nop");
    }
    SYSCON->USB1NEEDCLKCTRL |= SYSCON_USB1NEEDCLKCTRL_POL_HS_DEV_NEEDCLK_MASK;
#endif
}

void USB_PowerPreSwitchHook(void)
{
    //HW_TimerControl(0U);

    //BOARD_DeinitPins();

    //DbgConsole_Deinit();

    USB_PreLowpowerMode();
}

void USB_PowerPostSwitchHook(void)
{
	BOARD_BootClockPLL150M();
    USB_PostLowpowerMode();
    //HW_TimerControl(1U);
    //------------------------------------------------------------------------------------------
    /* uart init */
    //BOARD_InitDebugConsole();
    //------------------------------------------------------------------------------------------
}

void USB_DeviceSuspendResumeTask(void)
{
    if (g_UsbDeviceComposite.isResume)
    {
    	g_UsbDeviceComposite.isResume = 0U;
        if (kStatus_MouseIdle != g_UsbDeviceComposite.suspend)
        {
        	g_UsbDeviceComposite.suspend = kStatus_MouseResumed;
        }
    }

    switch (g_UsbDeviceComposite.suspend)
    {
        case kStatus_MouseIdle:
            break;
        case kStatus_MouseStartSuspend:
        	g_UsbDeviceComposite.suspend = kStatus_MouseSuspending;

        	Flag_Suspend = 1;
        	/* 关灯，发现不关灯的时候hall的灯光会随着定时器自动唤醒而闪灯 */
        	Ledoff();
            break;
        case kStatus_MouseSuspending:
            //usb_echo("USB device suspended.\r\n");
            if (g_UsbDeviceComposite.remoteWakeup)
            {
                //usb_echo("Please Press wakeup switch(%s) to remote wakeup the host.\r\n", SW_GetName());
            }
            //----------------------------------------------------------------------------------------------------------------------------------------
            /* hall进睡眠 */
            Hall_To_Sleep();
            //----------------------------------------------------------------------------------------------------------------------------------------
            /* 定时器开启 */
            CTimer_Start();
            //----------------------------------------------------------------------------------------------------------------------------------------
            g_UsbDeviceComposite.suspend = kStatus_MouseSuspended;
            break;
        case kStatus_MouseSuspended:
			USB_PowerPreSwitchHook();
			if (g_UsbDeviceComposite.remoteWakeup)
			{
				//SW_IntControl(1);
			}
			USB_DeviceSetStatus(g_UsbDeviceComposite.deviceHandle, kUSB_DeviceStatusBusSuspend, NULL);
			if (kStatus_Success != USB_EnterLowpowerMode())
			{
				g_UsbDeviceComposite.selfWakeup = 1U;
				USB_PowerPostSwitchHook();
				//usb_echo("Enter VLPS mode failed!\r\n");
			}
			else
			{
				USB_PowerPostSwitchHook();
			}
            if (g_UsbDeviceComposite.remoteWakeup)
            {
                //SW_IntControl(0);
            }
            if (g_UsbDeviceComposite.attach)
            {
            	//g_UsbDeviceComposite.suspend = kStatus_MouseStartResume;
            }
            else
            {
            	//g_UsbDeviceComposite.suspend = kStatus_MouseIdle;
            }
			//----------------------------------------------------------------------------------------------------------------------------------------
			/* hall退出睡眠 */
			Hall_To_WakeUp();
			//----------------------------------------------------------------------------------------------------------------------------------------
			/* 定时器关闭开启 */
			CTimer_Stop();
			//----------------------------------------------------------------------------------------------------------------------------------------
			hall_mcu_delay_ms(1);
			//----------------------------------------------------------------------------------------------------------------------------------------
			/* 扫键 */
			if(KeyScan_Suspend())
			{
				//usb_echo("Keypress!%d AA\r\n",g_UsbDeviceComposite.isResume );
				//------------------------------------------------------------------------------------------------------------------------------------
				if (g_UsbDeviceComposite.attach)
				{
					//usb_echo("MouseStartResume!\r\n");
					g_UsbDeviceComposite.suspend = kStatus_MouseStartResume;
				}
				else
				{
					//usb_echo("MouseIdle!\r\n");
					g_UsbDeviceComposite.suspend = kStatus_MouseIdle;
				}
			}
			else
			{
				//usb_echo("MouseSuspending!\r\n");
				g_UsbDeviceComposite.suspend = kStatus_MouseSuspending;
			}
            break;
        case kStatus_MouseStartResume:
            if (g_UsbDeviceComposite.selfWakeup)
            {
            	//usb_echo("A\\r\n");
            	g_UsbDeviceComposite.selfWakeup = 0U;
                if (g_UsbDeviceComposite.remoteWakeup)
                {
                	/* 唤醒PC */
                    if (kStatus_USB_Success == USB_DeviceSetStatus(g_UsbDeviceComposite.deviceHandle, kUSB_DeviceStatusBusResume, NULL))
                    {
                        //usb_echo("Remote wakeup the host.%d AA\r\n",g_UsbDeviceComposite.isResume );
                        Flag_Suspend = 0;
                        g_UsbDeviceComposite.suspend = kStatus_MouseResuming;
                    }
                    else
                    {
                        //usb_echo("Send resume signal failed.\r\n");
                        g_UsbDeviceComposite.suspend = kStatus_MouseStartResume;
                    }
                }
                else
                {
                	//usb_echo("E!\r\n");
                	g_UsbDeviceComposite.suspend = kStatus_MouseResuming;
                }
            }
            else
            {
            	//usb_echo("F!\r\n");
            	g_UsbDeviceComposite.suspend = kStatus_MouseResumed;
            }
            break;
        case kStatus_MouseResuming:
            break;
        case kStatus_MouseResumed:
        	//usb_echo("kStatus_MouseResumed!\r\n");
			//----------------------------------------------------------------------------------------------------------------------------------------
			/* hall退出睡眠 */
			Hall_To_WakeUp();
			//----------------------------------------------------------------------------------------------------------------------------------------
			/* 定时器关闭开启 */
			CTimer_Stop();
			//----------------------------------------------------------------------------------------------------------------------------------------
        	/* RGB Variable init */
        	LED_Variable_Init();
    		//usb_echo("A!\r\n");
        	Flag_Suspend = 0;
    		g_UsbDeviceComposite.suspend = kStatus_MouseIdle;
            break;
        default:
        	g_UsbDeviceComposite.suspend = kStatus_MouseIdle;
        	Flag_Suspend = 0;
            break;
    }
}
//-----------------------------------------------------------------------------------------------------
