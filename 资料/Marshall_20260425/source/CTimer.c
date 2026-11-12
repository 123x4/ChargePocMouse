/*
 * ctimer.c
 *
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
#include <fsl_debug_console.h>
#include <pin_mux.h>
#include <board.h>
#include <fsl_ctimer.h>
#include <stdbool.h>
#include <fsl_power.h>
#include <ctimer.h>
/*******************************************************************************************************************************************
 * Definitions
 ******************************************************************************************************************************************/

/*******************************************************************************************************************************************
 * Variable
 ******************************************************************************************************************************************/
void ctimer_match0_callback(uint32_t flags);
/* Array of function pointers for callback for each channel */
ctimer_callback_t ctimer_callback_table[] = {NULL, ctimer_match0_callback, NULL, NULL, NULL, NULL, NULL, NULL};
//------------------------------------------------------------------------------------------------------------------------------------------
/* Match Configuration for Channel 0 */
static ctimer_match_config_t matchConfig0;
/*******************************************************************************************************************************************
 * API
 ******************************************************************************************************************************************/

void ctimer_match0_callback(uint32_t flags)
{
    static uint32_t count            = 0;
    static uint32_t matchUpdateCount = 8;

    if(count)
    {
    	count = 0;
    }
    else
    {
       	count = 1;
    }

	g_UsbDeviceComposite.selfWakeup = 1U;
	//g_UsbDeviceComposite.hwTick++;
    //USB_DeviceUpdateHwTick(g_UsbDeviceComposite.deviceHandle, g_UsbDeviceComposite.hwTick);

//    if (++count > matchUpdateCount)
//    {
//        count = 0;
//        matchConfig0.matchValue >>= 1;
//        matchUpdateCount <<= 1;
//        if (matchUpdateCount == (1 << 8))
//        {
//            matchUpdateCount        = 8;
//            matchConfig0.matchValue = CTIMER_CLK_FREQ / 2;
//        }
        	//CTIMER_SetupMatch(CTIMER, CTIMER_MAT0_OUT, &matchConfig0);
//    }
#if defined(BOARD_HAS_NO_CTIMER_OUTPUT_PIN_CONNECTED_TO_LED)
    /* No timer match output pin connected to a LED
     * toggle LED manually according to match status
     */
    if (CTIMER_GetOutputMatchStatus(CTIMER, CTIMER_EMT0_OUT))
    {
        LED_RED1_ON();
    }
    else
    {
        LED_RED1_OFF();
    }
#endif
}
/******************************************************************************************************************************************/
void CTimer_Init(void)
{
    ctimer_config_t config;

	CTIMER_GetDefaultConfig(&config);

	CTIMER_Init(CTIMER, &config);

	/* Configuration 0 */
	matchConfig0.enableCounterReset = true;
	matchConfig0.enableCounterStop  = false;
	/* 定时器的时钟频率为 96MHz，设置匹配值为 96 000 000/2，定时为 0.5 s。若设匹配值为 96，则定时为 1us。 */
	matchConfig0.matchValue         = 96*1000*100;//96是1us, CTIMER_CLK_FREQ / 2 = 0.5S;
	matchConfig0.outControl         = kCTIMER_Output_NoAction;
	matchConfig0.outPinInitState    = false;
	matchConfig0.enableInterrupt    = true;

	CTIMER_RegisterCallBack(CTIMER, &ctimer_callback_table[0], kCTIMER_MultipleCallback);
	//CTIMER_SetupMatch(CTIMER, CTIMER_MAT0_OUT, &matchConfig0);
	//CTIMER_StartTimer(CTIMER);
}

void CTimer_Start(void)
{
	CTIMER_SetupMatch(CTIMER, CTIMER_MAT0_OUT, &matchConfig0);
	CTIMER_StartTimer(CTIMER);
	//usb_echo("CTIMER_StartTimer\r\n");
}

void CTimer_Stop(void)
{
	CTIMER_StopTimer(CTIMER);
}
//-----------------------------------------------------------------------------------------------------
