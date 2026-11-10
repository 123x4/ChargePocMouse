//#include "watch_dog_timer.h"
#include "timer.h"
#include "gloab.h"
#include "usb.h"
//#include "gpio_isr.h"
#include "led1.h"
#include "led2.h"
#include "device.h"
#include "optical_3325.h"
//#include "optical_3389.h"
#include "../inc/SPI.h"
#include "usbhw.h"
#include "usbcore.h"
#include "custom.h"
//#include "i2c_slave.h"
//#include "i2c_master.h"
//#include "spi_hardware.h"
#include "uart.h"
#include "suspend.h"
#include "GPIO.h"
#include "wnDrv_Pmu.h"
#include "usbuser.h"
#include "pc_communicate.h"
#include "production_line_test.h"
#include "my_board.h"

UINT8 suspend_UsbActive_timerOut = 5;
//extern UINT8 mouse_temp[8] ;
//extern UINT8 keyboard_temp[8] ;
//extern UINT8 mouse_multimedia_temp[3];
extern REPORT_POINTER report_data ;//¢ADC_PD;
uint8_t  sus_res=0;
extern  uint32_t timer16_0_counter;
extern uint32_t timer16_resume_counter;
void Suspend_Check(void)
{
	uint8_t i;

	#if (ENABLE_UART)
		InitUART(9600) ;
		xprintf("c=%d",g_USBCore_Setting.USB_Device_Status);
	#endif

    wnApi_Events_Suspend_Handler();
	
	while(suspendFlag == 1)
	{
		nsAPI_SSP_Delay1us(300);
		wnDrv_Pmu_Sleep(MCU_DEEP_SLEEP);
	}

	if(wnApi_Events_Resume_Handler())
	{ 
		for(i=0;i<60;i++)
		{
			nsAPI_SSP_Delay1us(10);
		}
#if 1
    #if (defined(IRPT_V4) || defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))
    #else
		optical_init();
    #endif
//		if(!ProductionLine_Start_Get())Led_Init();
		Custom_Load_ModeSetting();
#endif

	} 

	if(g_USBCore_Setting.Enumeration_Finish == TRUE)  //Add these for wake up screen. 20190103
	{	
#if 1
		if(sus_res)
		{
			if(nsAPI_USB_EP_Status(HID_EP1_IN))	
			{
				report_data.mouse_report[0]=0x01;
//				nsAPI_USB_WriteEP(HID_EP1_IN, &report_data.mouse_report[0], MOUSE_IN_REPORT_SIZE);
				USB_LoadInEP_Data(EP_1_MOUSE, report_data.mouse_report, 8);
				sus_res=0;
			}
			de_determineUsbAct = 0; 
//			Led_Init();	
		}
#endif
	}
}
