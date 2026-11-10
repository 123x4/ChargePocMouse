/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbuser.c
 * Purpose: USB Custom User Module
 * Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC family microcontroller devices only. Nothing
 *      else gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/
#define _USBUSER_C_

#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbuser.h"
#include "hiduser.h"
#include "device.h"
#include "uart.h"

#include "led1.h"
//#include "optical.h"
#include "SPI.h"
#include "wnDrv_Pmu.h"
#include "optical_3389.h"
/*
 *  USB Power Event Callback
 *   Called automatically on USB Power Event
 *    Parameter:       power: On(TRUE)/Off(FALSE)
 */
extern void USBFS_SOF_ISR_EntryCallback(void);

#if USB_POWER_EVENT
void USB_Power_Event (uint32_t  power) {
}
#endif


/*
 *  USB Reset Event Callback
 *   Called automatically on USB Reset Event
 */

#if USB_RESET_EVENT
void USB_Reset_Event(void){
nsAPI_USB_ResetCore();
}
#endif


/*
 *  USB Suspend Event Callback
 *   Called automatically on USB Suspend Event
 */

#if USB_SUSPEND_EVENT
void USB_Suspend_Event (void) {
//	Led_ShutDown();
//	Sensor_Shutdown();
//	Usb_Stop();
////	Timer_WatchDogCount1_Stop();
////	Watch_Dog_Timer_Disable();

//  NVIC_DisableIRQ(I2C_IRQn);
//	NVIC_DisableIRQ(UART_IRQn);		
//  wnDrv_Pmu_Init();
//  wnDrv_Pmu_Sleep(MCU_DEEP_SLEEP); 		
//  wnDrv_Usbhw_Suspend();
	
}
#endif


/*
 *  USB Resume Event Callback
 *   Called automatically on USB Resume Event
 */

#if USB_RESUME_EVENT
void USB_Resume_Event (void) {
}
#endif


/*
 *  USB Remote Wakeup Event Callback
 *   Called automatically on USB Remote Wakeup Event
 */

#if USB_WAKEUP_EVENT
void USB_WakeUp_Event (void) {
}
#endif


/*
 *  USB Start of Frame Event Callback
 *   Called automatically on USB Start of Frame Event
 */

#if USB_SOF_EVENT
void USB_SOF_Event (void) {
	
		USBFS_SOF_ISR_EntryCallback();
}
#endif


/*
 *  USB Set Configuration Event Callback
 *   Called automatically on USB Set Configuration Request
 */

#if USB_CONFIGURE_EVENT
void nsAPI_USB_Configure_Event(void)
{
	if(g_USBCore_Setting.USB_Configuration)
	{
//		nsAPI_USB_WriteEP(HID_EP1_IN, &g_Main_Setting.MouseInReport[0],MOUSE_IN_REPORT_SIZE);
//		nsAPI_USB_WriteEP(HID_EP2_IN, &g_Main_Setting.KeyboardInReport[0],KEYBOARD_IN_REPORT_SIZE);
//		nsAPI_USB_WriteEP(HID_EP3_IN, &g_Main_Setting.InReport[0],CUSTOMER_IN_REPORT_SIZE);    
//    nsAPI_USB_WriteEP(HID_EP4_IN, &g_Main_Setting.FirmwareUpdateInReport[0],FWUPDATE_IN_REPORT_SIZE);    //Firmware update //(Add)Abellee 20161019 Add FW update
		g_USBCore_Setting.Enumeration_Finish = TRUE; 
	}
	
}
#endif


/*
 *  USB Set Interface Event Callback
 *   Called automatically on USB Set Interface Request
 */

#if USB_INTERFACE_EVENT
void USB_Interface_Event (void) {
}
#endif


/*
 *  USB Set/Clear Feature Event Callback
 *   Called automatically on USB Set/Clear Feature Request
 */

#if USB_FEATURE_EVENT
void USB_Feature_Event (void) {
}
#endif


#define P_EP(n) ((USB_EP_EVENT & (1 << (n))) ? nsAPI_USB_EndPoint##n : NULL)

/* USB Endpoint Events Callback Pointers */
void (* const USB_P_EP[USB_LOGIC_EP_NUM]) (U32 event) = {
	P_EP(0),
	P_EP(1),
	P_EP(2),
	P_EP(3),
	P_EP(4),     
};


/*
 *  USB Endpoint 1 Event Callback
 *   Called automatically on USB Endpoint 1 Event
 *    Parameter:       event
 */

void nsAPI_USB_EndPoint1(U32 event)
{
	switch (event)
	{
		case USB_EVT_IN:
			g_Main_Setting.EPInOutReq |= EP1_IN;
		break;

		case USB_EVT_OUT:
			g_Main_Setting.EPInOutReq |= EP1_OUT;
		break;
	}
}


/*
 *  USB Endpoint 2 Event Callback
 *   Called automatically on USB Endpoint 2 Event
 *    Parameter:       event
 */

void nsAPI_USB_EndPoint2(U32 event)
{
	switch (event)
	{
		case USB_EVT_IN:
		g_Main_Setting.EPInOutReq |= EP2_IN;
		break;

		case USB_EVT_OUT:
		g_Main_Setting.EPInOutReq |= EP2_OUT;
		break;
	}
}

/*
 *  USB Endpoint 3 Event Callback
 *   Called automatically on USB Endpoint 3 Event
 *    Parameter:       event
 */

void nsAPI_USB_EndPoint3(U32 event)
{
	switch (event)
	{
		case USB_EVT_IN:
		g_Main_Setting.EPInOutReq |= EP3_IN;
		break;

		case USB_EVT_OUT:
		g_Main_Setting.EPInOutReq |= EP3_OUT;
		break;
	}
}

/*
 *  USB Endpoint 4 Event Callback
 *   Called automatically on USB Endpoint 4 Event
 *    Parameter:       event
 */
void nsAPI_USB_EndPoint4(U32 event)
{
	switch (event)
	{
		case USB_EVT_IN:
		g_Main_Setting.EPInOutReq |= EP4_IN;
		break;

		case USB_EVT_OUT:
		g_Main_Setting.EPInOutReq |= EP4_OUT;
		break;
	}
}

void nsAPI_USB_EndPointClr(void)
{
	g_Main_Setting.EPInOutReq &= ~(EP4_OUT | EP4_IN |EP3_OUT | EP3_IN |EP2_OUT | EP2_IN | EP1_OUT | EP1_IN);
}
