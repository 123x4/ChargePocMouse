/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbcore.h
 * Purpose: USB Core Definitions
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

#ifndef __USBCORE_H__
#define __USBCORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Type.h"
#include "usbcfg.h"
#include "usb.h"
	
#ifdef  __USBCORE_C__
//#define INTERFACE
#define INTERFACE
#else
#define INTERFACE extern
#endif

#if 1
#define _DEBUG_USBCORE_(x) x 
#else
#define _DEBUG_USBCORE_(x)
#endif

#if 0
#define _DEBUG_USBCORE_2_(x) x
#else
#define _DEBUG_USBCORE_2_(x)
#endif

#if REMOTE_WAKEUP_ENABLE 
#define USB_DEVICE_STATUS_NONE NONE 
#define USB_DEVICE_STATUS_SUSPEND BIT0
#define USB_DEVICE_STATUS_RESUME BIT1
#endif

/* USB Endpoint Data Structure */
typedef struct _USB_EP_DATA_
{
	U8  *pData;
	U16 Count;
} USB_EP_DATA;

typedef struct _LPC11Uxx_USBCORE_SETTING_
{
	U16 USB_DeviceStatus;
	U8 USB_NumInterfaces;
	U8 USB_DeviceAddress;
	U8 USB_Configuration;
	#if REMOTE_WAKEUP_ENABLE
    U8 USB_Device_Status;   
#endif    
    BOOLEAN Enumeration_Finish;
	U8 USB_AltSetting[USB_IF_NUM];/* USB Core Global Variables */
	U8 EP0Buf[USB_MAX_PACKET0];/* USB Endpoint 0 Buffer */
	U32 USB_EndPointMask;
	U32 USB_EndPointHalt;
	U32 USB_EndPointStall;/* EP must stay stalled */
	USB_EP_DATA EP0Data;/* USB Endpoint 0 Data Info */
	USB_SETUP_PACKET SetupPacket;/* USB Setup Packet */
	U8  EP0_SetReq;
	U8	EP0_GetReq;
	U8  EP0_DosMode;
//#if REMOTE_WAKEUP_ENABLE
//    U8 USB_Device_Status;   
//#endif    
//    BOOLEAN Enumeration_Finish;
	
}LPC11Uxx_USBCORE_SETTING;




INTERFACE LPC11Uxx_USBCORE_SETTING g_USBCore_Setting;

/* USB Core Functions */
INTERFACE void nsAPI_USB_ResetCore(void);
//INTERFACE __inline BOOLEAN nsAPI_USB_ReqGetStatus(void);
extern __inline BOOLEAN nsAPI_USB_ReqGetStatus(void);
//INTERFACE __inline BOOLEAN nsAPI_USB_ReqGetDescriptor(void);
extern  __inline BOOLEAN nsAPI_USB_ReqGetDescriptor(void);
INTERFACE __inline BOOLEAN nsAPI_USB_ReqSetAddress(void);
INTERFACE __inline BOOLEAN nsAPI_USB_ReqGetInterface(void);
//INTERFACE __inline BOOLEAN nsAPI_USB_ReqSetInterface(void);
extern __inline BOOLEAN nsAPI_USB_ReqSetInterface(void);
//INTERFACE __inline BOOLEAN nsAPI_USB_ReqSetConfiguration(void);
extern __inline BOOLEAN nsAPI_USB_ReqSetConfiguration(void);
//INTERFACE __inline BOOLEAN nsAPI_USB_ReqGetConfiguration(void);
extern __inline BOOLEAN nsAPI_USB_ReqGetConfiguration(void);
INTERFACE void nsAPI_USB_DataOutStage(void);
INTERFACE void nsAPI_USB_EP4DataOutStage(void); //(Add)Abellee 20161019 Add FW update
INTERFACE void nsAPI_USB_StatusOutStage(void);
INTERFACE void nsAPI_USB_SetupStage(void);
INTERFACE void nsAPI_USBCore_Stall_i(void);

extern __inline BOOLEAN nsAPI_USB_ReqSetClrFeature(U32 sc);


#undef INTERFACE
#ifdef __cplusplus
}
#endif
#endif  /* __USBCORE_H__ */
