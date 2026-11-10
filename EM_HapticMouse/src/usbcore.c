/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbcore.c
 * Purpose: USB Core Module
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
 *----------------------------------------------------------------------------
 * History:
 *          V1.20 Added vendor specific requests
 *                Changed string descriptor handling
 *                Reworked Endpoint0
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/
#define  __USBCORE_C__

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbdesc.h"
#include "usbuser.h"
#include "uart.h"
#include "usbhw.h"

#if (USB_CLASS)
#if (USB_HID)
#include "hid.h"
#include "hiduser.h"
#endif
#endif
#if (USB_VENDOR)
#include "vendor.h"
#endif

#include "spi.h"
#include "device.h"


//#pragma diag_suppress 111,177,1441

//U8 Total_Length;
//volatile uint8_t USBFS_configurationChanged;

U8 * nsAPI_Install_ReportRate_USBDescript(void)
{

	return (U8 *)USB_ConfigDescriptor;		// 1mSec

}


/*
 *  Reset USB Core
 *    Parameters:      None
 *    Return Value:    None
 */
void nsAPI_USB_ResetCore (void)
{
	g_USBCore_Setting.USB_DeviceStatus  = USB_POWER;
	g_USBCore_Setting.USB_DeviceAddress = 0;
	g_USBCore_Setting.USB_Configuration = 0;
	g_USBCore_Setting.USB_EndPointMask = 0x00010001;
	g_USBCore_Setting.USB_EndPointHalt  = 0x00000000;
	g_USBCore_Setting.USB_EndPointStall = 0x00000000;
#if (SUPPORT_MAC_BOOT_AND_DOS_MODE)
    g_USBCore_Setting.EP0_DosMode=0;
#endif

    HID_Protocol = HID_PROCOTOL_REPORT; //(Add)Abellee 20130102
    g_USBCore_Setting.Enumeration_Finish = FALSE;
#if (ENABLE_UART)
       // xprintf("<RC>",0);
       // Delsyms(5);
#endif


}


/*
 *  USB Request - Setup Stage
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    None
 */

void nsAPI_USB_SetupStage(void)
{
	nsAPI_USB_ReadSetupEP(0x00,(U8 *)&g_USBCore_Setting.SetupPacket);
}


/*
 *  USB Request - Data In Stage
 *    Parameters:      None (global EP0Data)
 *    Return Value:    None
 */

void nsAPI_USB_DataInStage(void)
{
	U32 cnt;

	if(g_USBCore_Setting.EP0Data.Count > USB_MAX_PACKET0)
	{
		cnt = USB_MAX_PACKET0;
	}
	else
	{
		cnt = g_USBCore_Setting.EP0Data.Count;
	}

	cnt = nsAPI_USB_WriteEP(0x80, g_USBCore_Setting.EP0Data.pData, cnt);
	g_USBCore_Setting.EP0Data.pData += cnt;
	g_USBCore_Setting.EP0Data.Count -= cnt;
	//_DEBUG_USBCORE_(xprintf("IN_c=%d",cnt));
}


/*
 *  USB Request - Data Out Stage
 *    Parameters:      None (global EP0Data)
 *    Return Value:    None
 */
#if 0 //No use
void nsAPI_USB_DataOutStage(void)
{
	U32 cnt;

	cnt = nsAPI_USB_ReadEP(0x00, g_USBCore_Setting.EP0Data.pData);
	//_DEBUG_USBCORE_(xprintf("cnt=%x",cnt));
	g_USBCore_Setting.EP0Data.pData += cnt;
	g_USBCore_Setting.EP0Data.Count -= cnt;
}

//(Add)Abellee 20161019 Add FW update
void nsAPI_USB_EP4DataOutStage(void)
{
	nsAPI_USB_ReadEP(0x04, &g_Main_Setting.FirmwareUpdateOutReport[0]); //count fix to 64 bytes, if not , then error or anagin read EP
}
#endif
/*
 *  USB Request - Status In Stage
 *    Parameters:      None
 *    Return Value:    None
 */

void nsAPI_USB_StatusInStage(void)
{
	nsAPI_USB_WriteEP(0x80, NULL, 0);
}


/*
 *  USB Request - Status Out Stage
 *    Parameters:      None
 *    Return Value:    None
 */

void nsAPI_USB_StatusOutStage(void)
{
	nsAPI_USB_ReadEP(0x00, g_USBCore_Setting.EP0Buf);
}


/*
 *  Get Status USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOLEAN nsAPI_USB_ReqGetStatus(void)
{
	U32 n, m;

	switch(g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
	{
		case REQUEST_TO_DEVICE:
		g_USBCore_Setting.EP0Data.pData = (U8 *)&g_USBCore_Setting.USB_DeviceStatus;
		break;

		case REQUEST_TO_INTERFACE:
		if((g_USBCore_Setting.USB_Configuration != 0) && (g_USBCore_Setting.SetupPacket.wIndex.WB.L < g_USBCore_Setting.USB_NumInterfaces))
		{
			*((__packed U16 *)g_USBCore_Setting.EP0Buf) = 0;
			g_USBCore_Setting.EP0Data.pData = g_USBCore_Setting.EP0Buf;
		}
		else
			return (FALSE);
		break;

		case REQUEST_TO_ENDPOINT:
		n = g_USBCore_Setting.SetupPacket.wIndex.WB.L & 0x8F;
		m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);

		if(((g_USBCore_Setting.USB_Configuration != 0) || ((n & 0x0F) == 0)) && (g_USBCore_Setting.USB_EndPointMask & m))
		{
			*((__packed U16 *)g_USBCore_Setting.EP0Buf) = (g_USBCore_Setting.USB_EndPointHalt & m) ? 1 : 0;
			g_USBCore_Setting.EP0Data.pData = g_USBCore_Setting.EP0Buf;
		}
		else
			return (FALSE);
		break;
		default:
		return (FALSE);
	}
	return (TRUE);
}


/*
 *  Set/Clear Feature USB Request
 *    Parameters:      sc:    0 - Clear, 1 - Set
 *                            (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOLEAN nsAPI_USB_ReqSetClrFeature(U32 sc)
{
	U32 n, m;

	switch(g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
	{
		case REQUEST_TO_DEVICE:
		if(g_USBCore_Setting.SetupPacket.wValue.W == USB_FEATURE_REMOTE_WAKEUP)
		{
			if(sc)
			{
				NDrv_USB_WakeUpCfg(TRUE);
				g_USBCore_Setting.USB_DeviceStatus |=  USB_GETSTATUS_REMOTE_WAKEUP;
			}
			else
			{
				NDrv_USB_WakeUpCfg(FALSE);
				g_USBCore_Setting.USB_DeviceStatus &= ~USB_GETSTATUS_REMOTE_WAKEUP;
			}
		}
		else
		return (FALSE);
		break;

		case REQUEST_TO_INTERFACE:
		return (FALSE);

		case REQUEST_TO_ENDPOINT:
		n = g_USBCore_Setting.SetupPacket.wIndex.WB.L & 0x8F;
		m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);

		if((g_USBCore_Setting.USB_Configuration != 0) && ((n & 0x0F) != 0) && (g_USBCore_Setting.USB_EndPointMask & m))
		{
			if(g_USBCore_Setting.SetupPacket.wValue.W == USB_FEATURE_ENDPOINT_STALL)
			{
				if(sc)
				{
					nsAPI_USB_SetStallEP(n);
					g_USBCore_Setting.USB_EndPointHalt |=  m;
				}
				else
				{
					if((g_USBCore_Setting.USB_EndPointStall & m) != 0)
					{
						return (TRUE);
					}
					nsAPI_USB_ClrStallEP(n);
					g_USBCore_Setting.USB_EndPointHalt &= ~m;
				}
			}
			else
			{
				return (FALSE);
			}
		}
		else
	  	{
			return (FALSE);
		}
		break;
		default:
		return (FALSE);
	}
	return (TRUE);
}


/*
 *  Set Address USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOLEAN nsAPI_USB_ReqSetAddress(void)
{
	switch(g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
	{
		case REQUEST_TO_DEVICE:
		g_USBCore_Setting.USB_DeviceAddress = (0x80 | g_USBCore_Setting.SetupPacket.wValue.WB.L);
		break;
		default:
		return (FALSE);
	}
	return (TRUE);
}


/*
 *  Get Descriptor USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOLEAN nsAPI_USB_ReqGetDescriptor(void)
//BOOLEAN nsAPI_USB_ReqGetDescriptor(void)
{
	U8  *pD;
	U32 len, n;
	//_DEBUG_USBCORE_(xprintf("rc=%d",g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient));
	switch(g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
	{

		case REQUEST_TO_DEVICE:		//0
			//_DEBUG_USBCORE_(xprintf("wh=%d",g_USBCore_Setting.SetupPacket.wValue.WB.H));
		switch(g_USBCore_Setting.SetupPacket.wValue.WB.H)
		{
			case USB_DEVICE_DESCRIPTOR_TYPE:		//1
				g_USBCore_Setting.EP0Data.pData = (U8 *)USB_DeviceDescriptor;
				len = USB_DEVICE_DESC_SIZE;
			break;

			case USB_CONFIGURATION_DESCRIPTOR_TYPE:		//2

	//			pD=nsAPI_Install_ReportRate_USBDescript();   
				pD = (U8 *)USB_ConfigDescriptor;      

				if(((USB_CONFIGURATION_DESCRIPTOR *)pD)->bLength == 0)
				return (FALSE);

				g_USBCore_Setting.EP0Data.pData = pD;
				len = ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
			break;

			#if LPM_SUPPORT
			case USB_BOS_TYPE:
				pD = (uint8_t *)USB_BOSDescriptor;
				for (n = 0; n != g_USBCore_Setting.SetupPacket.wValue.WB.L; n++)
				{
					if(((USB_BOS_DESCRIPTOR *)pD)->bLength != 0)
					{
						pD += ((USB_BOS_DESCRIPTOR *)pD)->wTotalLength;
					}
				}

				if(((USB_BOS_DESCRIPTOR *)pD)->bLength == 0)
				return (FALSE);

				g_USBCore_Setting.EP0Data.pData = pD;
				len = ((USB_BOS_DESCRIPTOR *)pD)->wTotalLength;
			break;
			#endif

			case USB_STRING_DESCRIPTOR_TYPE:		//3
				pD = (uint8_t *)USB_StringDescriptor;
				for (n = 0; n != g_USBCore_Setting.SetupPacket.wValue.WB.L; n++)
				{
					if(((USB_STRING_DESCRIPTOR *)pD)->bLength != 0)
					{
						pD += ((USB_STRING_DESCRIPTOR *)pD)->bLength;
					}
				}

				if(((USB_STRING_DESCRIPTOR *)pD)->bLength == 0)
				return (FALSE);

				g_USBCore_Setting.EP0Data.pData = pD;
				len = ((USB_STRING_DESCRIPTOR *)g_USBCore_Setting.EP0Data.pData)->bLength;
			break;
			default:
			return (FALSE);
		}
		break;

		case REQUEST_TO_INTERFACE:
		switch(g_USBCore_Setting.SetupPacket.wValue.WB.H)
		{
#if USB_HID
			case HID_HID_DESCRIPTOR_TYPE:

			g_USBCore_Setting.EP0Data.pData = (nsAPI_Install_ReportRate_USBDescript() + HID_DESC_OFFSET + (g_USBCore_Setting.SetupPacket.wIndex.WB.L * 25));//modify 20120806

			len = HID_DESC_SIZE;
			break;

			case HID_REPORT_DESCRIPTOR_TYPE:
			//if(SetupPacket.wIndex.WB.L != USB_HID_IF_NUM0)
			//return (FALSE);/* Only Single HID Interface is supported */

			switch(g_USBCore_Setting.SetupPacket.wIndex.WB.L)
			{
				case 0:
				len = HID_ReportDescMouseSize;
				g_USBCore_Setting.EP0Data.pData = (U8 *)HID_ReportDescriptorMouse;
				break;


				case 1:
				len = HID_ReportDescKeyboardSize;
				g_USBCore_Setting.EP0Data.pData = (U8 *)HID_ReportDescriptorKeyboard;
				break;

				case 2:
				len = HID_ReportDescConsumerSize;
				g_USBCore_Setting.EP0Data.pData = (U8 *)HID_ReportDescriptorConsumerControl;
				break;

				default:
				return (FALSE);
				//break;
			}
			break;

			case HID_PHYSICAL_DESCRIPTOR_TYPE:
			return (FALSE);/* HID Physical Descriptor is not supported */
#endif
			default:
			return (FALSE);
		}
		break;
		default:
		return (FALSE);
	}

	if(g_USBCore_Setting.EP0Data.Count > len)	// mark
	g_USBCore_Setting.EP0Data.Count = len;
	//_DEBUG_USBCORE_(xprintf("n=%d",len));
	//_DEBUG_USBCORE_(xprintf("c=%d",g_USBCore_Setting.EP0Data.Count));

	return (TRUE);
}


/*
 *  Get Configuration USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOLEAN nsAPI_USB_ReqGetConfiguration(void)
{
	switch (g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
	{
		case REQUEST_TO_DEVICE:
		g_USBCore_Setting.EP0Data.pData = &g_USBCore_Setting.USB_Configuration;
		break;
		default:
		return (FALSE);
	}
	return (TRUE);
}


/*
 *  Set Configuration USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOLEAN nsAPI_USB_ReqSetConfiguration(void)
{
	USB_COMMON_DESCRIPTOR *pD;
	U32 alt = 0,n, m;

	switch(g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
	{
		case REQUEST_TO_DEVICE:
		if(g_USBCore_Setting.SetupPacket.wValue.WB.L)
		{
			//pD = (USB_COMMON_DESCRIPTOR *)USB_ConfigDescriptor;
			 pD = (USB_COMMON_DESCRIPTOR *)nsAPI_Install_ReportRate_USBDescript();
			while(pD->bLength)
			{
				switch (pD->bDescriptorType)
				{
					case USB_CONFIGURATION_DESCRIPTOR_TYPE:
					if(((USB_CONFIGURATION_DESCRIPTOR *)pD)->bConfigurationValue == g_USBCore_Setting.SetupPacket.wValue.WB.L)
					{
						g_USBCore_Setting.USB_Configuration = g_USBCore_Setting.SetupPacket.wValue.WB.L;
						g_USBCore_Setting.USB_NumInterfaces = ((USB_CONFIGURATION_DESCRIPTOR *)pD)->bNumInterfaces;

						for (n = 0; n < USB_IF_NUM; n++)
						{
							g_USBCore_Setting.USB_AltSetting[n] = 0;
						}

						for (n = 1; n < USB_LOGIC_EP_NUM; n++)
						{
							if(g_USBCore_Setting.USB_EndPointMask & (1 << n))
							{
								nsAPI_USB_DisableEP(n);
							}

							if(g_USBCore_Setting.USB_EndPointMask & ((1 << 16) << n))
							{
								nsAPI_USB_DisableEP(n | 0x80);
							}
						}

						g_USBCore_Setting.USB_EndPointMask = 0x00010001;
						g_USBCore_Setting.USB_EndPointHalt = 0x00000000;
						g_USBCore_Setting.USB_EndPointStall= 0x00000000;
						NDrv_USB_Configure(TRUE);

						if(((USB_CONFIGURATION_DESCRIPTOR *)pD)->bmAttributes & USB_CONFIG_POWERED_MASK)
						{
							g_USBCore_Setting.USB_DeviceStatus |=  USB_GETSTATUS_SELF_POWERED;
						}
						else
						{
							g_USBCore_Setting.USB_DeviceStatus &= ~USB_GETSTATUS_SELF_POWERED;
						}
					}
					else
					{
						//(U8 *)pD += ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
						pD =  (USB_COMMON_DESCRIPTOR *)((uint8_t *)pD + ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength);
						continue;
					}
					break;

					case USB_INTERFACE_DESCRIPTOR_TYPE:
					alt = ((USB_INTERFACE_DESCRIPTOR *)pD)->bAlternateSetting;
					break;

					case USB_ENDPOINT_DESCRIPTOR_TYPE:
					if(alt == 0)
					{
						n = ((USB_ENDPOINT_DESCRIPTOR *)pD)->bEndpointAddress & 0x8F;
						m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);
						g_USBCore_Setting.USB_EndPointMask |= m;
						//nsAPI_USB_ConfigEP((USB_ENDPOINT_DESCRIPTOR *)pD);
						nsAPI_USB_EnableEP(n);
						nsAPI_USB_ResetEP(n);
					}
					break;
				}
			//	(U8 *)pD += pD->bLength;
				pD = (USB_COMMON_DESCRIPTOR *) ((uint8_t *)pD + (pD->bLength));
			}
		}
		else
		{
			g_USBCore_Setting.USB_Configuration = 0;

			for(n = 1; n < USB_LOGIC_EP_NUM; n++)
			{
				if(g_USBCore_Setting.USB_EndPointMask & (1 << n))
				{
					nsAPI_USB_DisableEP(n);

				}

				if(g_USBCore_Setting.USB_EndPointMask & ((1 << 16) << n))
				{
					nsAPI_USB_DisableEP(n | 0x80);
				}
			}

			g_USBCore_Setting.USB_EndPointMask  = 0x00010001;
			g_USBCore_Setting.USB_EndPointHalt  = 0x00000000;
			g_USBCore_Setting.USB_EndPointStall = 0x00000000;
			NDrv_USB_Configure(FALSE);
		}

		if(g_USBCore_Setting.USB_Configuration != g_USBCore_Setting.SetupPacket.wValue.WB.L)
		return (FALSE);
		break;
		default:
		return (FALSE);
	}
	return (TRUE);
}


/*
 *  Get Interface USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOLEAN nsAPI_USB_ReqGetInterface(void)
{
	switch (g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
	{
		case REQUEST_TO_INTERFACE:
		if((g_USBCore_Setting.USB_Configuration != 0) && (g_USBCore_Setting.SetupPacket.wIndex.WB.L < g_USBCore_Setting.USB_NumInterfaces))
		{
			g_USBCore_Setting.EP0Data.pData = g_USBCore_Setting.USB_AltSetting + g_USBCore_Setting.SetupPacket.wIndex.WB.L;
		}
		else
			return (FALSE);

		break;
		default:
		return (FALSE);
	}
	return (TRUE);
}


/*
 *  Set Interface USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__inline BOOLEAN nsAPI_USB_ReqSetInterface(void)
{
	USB_COMMON_DESCRIPTOR *pD;
	U32 ifn = 0, alt = 0, old = 0, msk = 0,n,m,set;

	switch (g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
	{
		case REQUEST_TO_INTERFACE:
		if(g_USBCore_Setting.USB_Configuration == 0)
		return (FALSE);

		set = FALSE;
		//pD  = (USB_COMMON_DESCRIPTOR *)USB_ConfigDescriptor;
		pD  = (USB_COMMON_DESCRIPTOR *)nsAPI_Install_ReportRate_USBDescript();

		while(pD->bLength)
		{
			switch (pD->bDescriptorType)
			{
				case USB_CONFIGURATION_DESCRIPTOR_TYPE:
				if(((USB_CONFIGURATION_DESCRIPTOR *)pD)->bConfigurationValue != g_USBCore_Setting.USB_Configuration)
				{
					//(uint8_t *)pD += ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
					pD =  (USB_COMMON_DESCRIPTOR *)((uint8_t *)pD + ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength);
					continue;
				}
				break;

				case USB_INTERFACE_DESCRIPTOR_TYPE:
				ifn = ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber;
				alt = ((USB_INTERFACE_DESCRIPTOR *)pD)->bAlternateSetting;
				msk = 0;
				if((ifn == g_USBCore_Setting.SetupPacket.wIndex.WB.L) && (alt == g_USBCore_Setting.SetupPacket.wValue.WB.L))
				{
					set = TRUE;
					old = g_USBCore_Setting.USB_AltSetting[ifn];
					g_USBCore_Setting.USB_AltSetting[ifn] = (U8)alt;
				}
				break;

				case USB_ENDPOINT_DESCRIPTOR_TYPE:
				if(ifn == g_USBCore_Setting.SetupPacket.wIndex.WB.L)
				{
					n = ((USB_ENDPOINT_DESCRIPTOR *)pD)->bEndpointAddress & 0x8F;
					m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);

					if(alt == g_USBCore_Setting.SetupPacket.wValue.WB.L)
					{
						g_USBCore_Setting.USB_EndPointMask |=  m;
						g_USBCore_Setting.USB_EndPointHalt &= ~m;
						//nsAPI_USB_ConfigEP((USB_ENDPOINT_DESCRIPTOR *)pD);
						nsAPI_USB_EnableEP(n);
						nsAPI_USB_ResetEP(n);
						msk |= m;
					}
					else if ((alt == old) && ((msk & m) == 0))
					{
						g_USBCore_Setting.USB_EndPointMask &= ~m;
						g_USBCore_Setting.USB_EndPointHalt &= ~m;
						nsAPI_USB_DisableEP(n);
					}
				}
				break;
			}
		//(U8 *)pD += pD->bLength;
			pD = (USB_COMMON_DESCRIPTOR *) ((uint8_t *)pD + (pD->bLength));
		}
		break;
		default:
		return (FALSE);
	}
	return (set);
}


/*
 *  USB Endpoint 0 Event Callback
 *    Parameters:      event
 *    Return Value:    none
 */

void nsAPI_USB_EndPoint0(U32 event)
{
	BOOLEAN bSetupClassOk = FALSE,bOutClassOk = FALSE;

//	_DEBUG_USBCORE_(xprintf("e=%x",event));
//	_DEBUG_USBCORE_(xprintf("Co=%x",g_USBCore_Setting.EP0Data.Count));
//  TimerDelayMsec(1);
	switch(event)
	{
		case USB_EVT_SETUP:			//1
			nsAPI_USB_SetupStage();		//Read Setup Packet
			//USB_DirCtrlEP(g_USBCore_Setting.SetupPacket.bmRequestType.BM.Dir);
			g_USBCore_Setting.EP0Data.Count = g_USBCore_Setting.SetupPacket.wLength;/* Number of bytes to transfer */
			//_DEBUG_USBCORE_(xprintf("\r\n Sc=%x",g_USBCore_Setting.SetupPacket.wLength));

			//_DEBUG_USBCORE_(xprintf("\r\n RT=%x",g_USBCore_Setting.SetupPacket.bmRequestType.BM.Type));

			switch(g_USBCore_Setting.SetupPacket.bmRequestType.BM.Type)
			{
				case REQUEST_STANDARD:		//0

					//_DEBUG_USBCORE_(xprintf("R=%x",g_USBCore_Setting.SetupPacket.bRequest));

					switch(g_USBCore_Setting.SetupPacket.bRequest)
					{
						case USB_REQUEST_GET_STATUS:

							if(nsAPI_USB_ReqGetStatus())
							nsAPI_USB_DataInStage();
							else
							nsAPI_USBCore_Stall_i();
						break;

						case USB_REQUEST_CLEAR_FEATURE:

							if(nsAPI_USB_ReqSetClrFeature(0))
							{
							nsAPI_USB_StatusInStage();
								#if USB_FEATURE_EVENT
								USB_Feature_Event();
								#endif
							}
							else
							nsAPI_USBCore_Stall_i();

						break;

						case USB_REQUEST_SET_FEATURE:

                            #if (SUPPORT_MAC_BOOT_AND_DOS_MODE)
                            //if (g_USBCore_Setting.SetupPacket.wValue.W == 0x0001)//Device Remote Wakeup=0x0001
							//    g_USBCore_Setting.EP0_DosMode = 0; //Disk & BIOS Mode
							#endif
							if(nsAPI_USB_ReqSetClrFeature(1))
							{
								nsAPI_USB_StatusInStage();
								#if USB_FEATURE_EVENT
								USB_Feature_Event();
								#endif
							}
							else
							nsAPI_USBCore_Stall_i();
						break;

						case USB_REQUEST_SET_ADDRESS:
							if(nsAPI_USB_ReqSetAddress())
							nsAPI_USB_StatusInStage();
							else
							nsAPI_USBCore_Stall_i();

						break;

						case USB_REQUEST_GET_DESCRIPTOR:		//R=6
							if(nsAPI_USB_ReqGetDescriptor())
							nsAPI_USB_DataInStage();
							else
							nsAPI_USBCore_Stall_i();

						break;

						case USB_REQUEST_SET_DESCRIPTOR:
							/*stall_o:*/nsAPI_USB_SetStallEP(0x00);/* not supported */
							g_USBCore_Setting.EP0Data.Count = 0;
						break;

						case USB_REQUEST_GET_CONFIGURATION:
							if(nsAPI_USB_ReqGetConfiguration())
							nsAPI_USB_DataInStage();
							else
							nsAPI_USBCore_Stall_i();
						break;

						case USB_REQUEST_SET_CONFIGURATION:
                            #if (ENABLE_UART)
                                //xprintf("<SConf>",0);
                            #endif
							if(nsAPI_USB_ReqSetConfiguration())
							{
								nsAPI_USB_StatusInStage();
								#if USB_CONFIGURE_EVENT
								nsAPI_USB_Configure_Event();
								#endif
							}
							else
							nsAPI_USBCore_Stall_i();
						break;

						case USB_REQUEST_GET_INTERFACE:
							if(nsAPI_USB_ReqGetInterface())
							nsAPI_USB_DataInStage();
							else
							nsAPI_USBCore_Stall_i();
						break;

						case USB_REQUEST_SET_INTERFACE:
							if(nsAPI_USB_ReqSetInterface())
							{
								nsAPI_USB_StatusInStage();
								#if USB_INTERFACE_EVENT
								USB_Interface_Event();
								#endif
							}
							else
							nsAPI_USBCore_Stall_i();
						break;

						default:
							nsAPI_USBCore_Stall_i();
					}
				break;/* end case REQUEST_STANDARD */

				#if USB_CLASS
				case REQUEST_CLASS:
					//_DEBUG_USBCORE_(xprintf("\r\n Rc=%x",g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient));

					switch(g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
					{
						case REQUEST_TO_DEVICE:
						goto stall_i;/* not supported */
						//break;

						case REQUEST_TO_INTERFACE:
							#if USB_HID
							//_DEBUG_USBCORE_(xprintf("\r\n L=%x",g_USBCore_Setting.SetupPacket.wIndex.WB.L));

							//if(g_USBCore_Setting.SetupPacket.wIndex.WB.L == USB_HID_IF_NUM0||\
							//g_USBCore_Setting.SetupPacket.wIndex.WB.L == USB_HID_IF_NUM1)/* IF number correct? */
							//{
								//_DEBUG_USBCORE_(xprintf("\r\n R=%x",g_USBCore_Setting.SetupPacket.bRequest));

								switch(g_USBCore_Setting.SetupPacket.bRequest)
								{
									case HID_REQUEST_GET_REPORT:

									//if(HID_GetReport())
									//{
										//g_USBCore_Setting.EP0Data.pData = g_USBCore_Setting.EP0Buf;/* point to data to be sent */
										//g_USBCore_Setting.EP0Data.pData = g_Main_Setting.ConsumerInReport;
										//nsAPI_USB_DataInStage();/* send requested data */
										//goto setup_class_ok;
										//g_USBCore_Setting.EP0Data.Count -= 8;
										bSetupClassOk = TRUE;
									//}

								//xprintf("\r\n S=%x",g_USBCore_Setting.EP0Buf[0]);
								//xprintf(",%x",g_USBCore_Setting.EP0Buf[1]);
								//xprintf(",%x",g_USBCore_Setting.EP0Buf[2]);
								//xprintf(",%x",g_USBCore_Setting.EP0Buf[3]);

									g_USBCore_Setting.EP0_GetReq =1;
									nsAPI_USB_ClrGetReqStatus(0x80);
									break;

									case HID_REQUEST_SET_REPORT:
									{
										//U8 i ;

										//nsAPI_USB_StatusInStage();
										nsAPI_USB_SetupStage();/* receive data */
										nsAPI_USB_StatusInStage();
										g_USBCore_Setting.EP0Data.pData = g_USBCore_Setting.EP0Buf;                              /* data to be received */
										bSetupClassOk = TRUE;
									}
									break;

									case HID_REQUEST_GET_IDLE:
									if(HID_GetIdle())
									{
										g_USBCore_Setting.EP0Data.pData = g_USBCore_Setting.EP0Buf;                            /* point to data to be sent */
										nsAPI_USB_DataInStage();/* send requested data */
										//goto setup_class_ok;
										bSetupClassOk = TRUE;
									}
									break;

									case HID_REQUEST_SET_IDLE:
									if(HID_SetIdle())
									{
										nsAPI_USB_StatusInStage();/* send Acknowledge */
										//goto setup_class_ok;
										bSetupClassOk = TRUE;
									}
									break;

									case HID_REQUEST_GET_PROTOCOL:
//On PC starts up, BIOS enables attached keyboard and mouse, when these device supports 'Boot' option.
//In some implementation, the report format can be different on 'Boot' and usual 'Report' state.
//Set_Protocol switches the state between 'Boot' and 'Report'.
//This request is supported by devices in the boot subclass. The wValue field
//dictates which protocol should be used.
//When initialized, all devices default to report protocol. However the host should
//not make any assumptions about the device¡¦s state and should set the desired
//protocol whenever initializing a device.

                                    //Class     SubClass                Protocol        Meaning
                                    //03h       00h - None
                                    //          01h - Boot Interface    00h None
                                    //                                  01h Keyboard
                                    //                                  02h Mouse

									if(HID_GetProtocol())
									{
										g_USBCore_Setting.EP0Data.pData = g_USBCore_Setting.EP0Buf;                            /* point to data to be sent */
                                        #if (SUPPORT_MAC_BOOT_AND_DOS_MODE)
                                        // Read SubClass Code
                                        //if (g_USBCore_Setting.EP0Buf[0] == HID_PROCOTOL_BOOT)
										//    g_USBCore_Setting.EP0_DosMode=1;	//Mac ,BootMode
										//else
                                        //    g_USBCore_Setting.EP0_DosMode=0;
										#endif
										nsAPI_USB_DataInStage();/* send requested data */
										//goto setup_class_ok;
										bSetupClassOk = TRUE;

									}
									break;

									case HID_REQUEST_SET_PROTOCOL:
                                    if (g_USBCore_Setting.SetupPacket.wIndex.WB.L != 0x00)
                                    {
                                        #if (ENABLE_UART)
                                        //xprintf("<InterF%lx>",g_USBCore_Setting.SetupPacket.wIndex.W);
                                        #endif
                                        break;
                                    }
                                    #if (ENABLE_UART)
                                    else
                                        //xprintf("<InterF0>",0);
                                    #endif

                                    #if (ENABLE_UART)
                                    //if (g_USBCore_Setting.SetupPacket.wValue.WB.L == HID_PROCOTOL_BOOT)
                                	  //  //xprintf("<SP0>",0);
                                    //else
                                      // xprintf("<SP1>",0);
                                    #endif

                                    #if (SUPPORT_MAC_BOOT_AND_DOS_MODE)
                                    if (g_USBCore_Setting.SetupPacket.wValue.WB.L == HID_PROCOTOL_BOOT)
                                    {
                                        g_USBCore_Setting.EP0_DosMode=1;
                                        g_Main_Setting.MouseInReport[0] = 0; //Have not report ID
                                    }
                                    else
                                    {
                                        g_USBCore_Setting.EP0_DosMode=0;
                                        g_Main_Setting.MouseInReport[0] = 0x01; //ReportID = 0x01
                                    }
                                    #endif
									if(HID_SetProtocol())
									{
										nsAPI_USB_StatusInStage();/* send Acknowledge */
										//goto setup_class_ok;
										bSetupClassOk = TRUE;
									}

									break;
								}
							//}
							#endif/* USB_HID */
							//goto stall_i;/* not supported */
							/* end case REQUEST_TO_INTERFACE */
						case REQUEST_TO_ENDPOINT:
							//goto stall_i;
							/* end case REQUEST_TO_ENDPOINT */
						default:
							#if 0
							goto stall_i;
							#else
							if(!bSetupClassOk){
							nsAPI_USBCore_Stall_i();
							}
							#endif
					}
				//	setup_class_ok:/* request finished successfully */
				break;  /* end case REQUEST_CLASS */
				#endif  /* USB_CLASS */

				#if USB_VENDOR
				case REQUEST_VENDOR:
					switch (g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
					{
						case REQUEST_TO_DEVICE:
						if(!USB_ReqVendorDev(TRUE))
						{
							goto stall_i;/* not supported */
						}
						break;

						case REQUEST_TO_INTERFACE:
						if(!USB_ReqVendorIF(TRUE))
						{
							goto stall_i;/* not supported */
						}
						break;

						case REQUEST_TO_ENDPOINT:
						if (!USB_ReqVendorEP(TRUE))
						{
							goto stall_i;/* not supported */
						}
						break;

						default:
						goto stall_i;
					}

					if(g_USBCore_Setting.SetupPacket.wLength)
					{
						if (g_USBCore_Setting.SetupPacket.bmRequestType.BM.Dir == REQUEST_DEVICE_TO_HOST)
						{
							nsAPI_USB_DataInStage();
						}
					}
					else
					{
						nsAPI_USB_StatusInStage();
					}
				break;  /* end case REQUEST_VENDOR */
				#endif  /* USB_VENDOR */

				default:
					stall_i:
					nsAPI_USBCore_Stall_i();
				break;
			}
			break;  /* end case USB_EVT_SETUP */

		case USB_EVT_OUT:
		{
			//U8 i;
			//xprintf("\r\n-----USB_EVT_OUT--%x--",g_USBCore_Setting.SetupPacket.bmRequestType.BM.Dir);
			if(g_USBCore_Setting.SetupPacket.bmRequestType.BM.Dir == REQUEST_HOST_TO_DEVICE)
			{
                //_DEBUG_USBCORE_(xprintf("C=%x",g_USBCore_Setting.EP0Data.Count));

				if(g_USBCore_Setting.EP0Data.Count)/* still data to receive ? */
				{

                    //_DEBUG_USBCORE_(xprintf("RT=%x",g_USBCore_Setting.SetupPacket.bmRequestType.BM.Type));
                    switch (g_USBCore_Setting.SetupPacket.bmRequestType.BM.Type)
                    {
                    	case REQUEST_STANDARD:
                    	nsAPI_USBCore_Stall_i();
                    	break;

                #if (USB_CLASS)
                    	case REQUEST_CLASS:
                    	//_DEBUG_USBCORE_(xprintf("Rc=%x",g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient));
                    	switch (g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
                    	{
                    		case REQUEST_TO_DEVICE:

                    		nsAPI_USBCore_Stall_i();
                    		break;
                    		case REQUEST_TO_INTERFACE:
                    #if USB_HID

                    		//xprintf("\r\n INTERFACE HID %x",g_USBCore_Setting.SetupPacket.wIndex.WB.L );
                    		if(g_USBCore_Setting.SetupPacket.wIndex.WB.L == USB_HID_IF_NUM0||\
                    		g_USBCore_Setting.SetupPacket.wIndex.WB.L == USB_HID_IF_NUM1||\
                    		g_USBCore_Setting.SetupPacket.wIndex.WB.L == USB_HID_IF_NUM2)/* IF number correct? */
                    		{
                    			//_DEBUG_USBCORE_(xprintf("\r\n TR=%x",g_USBCore_Setting.SetupPacket.bRequest));
                    			switch(g_USBCore_Setting.SetupPacket.bRequest)
                    			{
                    				case HID_REQUEST_SET_REPORT:
                    				g_USBCore_Setting.EP0_SetReq = 1;

                    					nsAPI_USB_StatusInStage();/* send Acknowledge */
                    					//goto out_class_ok;
                    					bOutClassOk = TRUE;

                    				break;
                    			}
                    		}
                        #endif  /* USB_HID */

                    		//goto stall_i;
                    		/* end case REQUEST_TO_INTERFACE */
                    		case REQUEST_TO_ENDPOINT:
                    		//goto stall_i;
                    		/* end case REQUEST_TO_ENDPOINT */
                    		default:
                    		//goto stall_i;
                    		if(!bOutClassOk){
                    		nsAPI_USBCore_Stall_i();
                    		}
                    	}
                    //	out_class_ok:/* request finished successfully */
                    	break; /* end case REQUEST_CLASS */
                    #endif  /* USB_CLASS */

	                #if USB_VENDOR
						case REQUEST_VENDOR:
						switch (g_USBCore_Setting.SetupPacket.bmRequestType.BM.Recipient)
						{
							case REQUEST_TO_DEVICE:
							if (!USB_ReqVendorDev(FALSE))
							{
								goto stall_i;/* not supported */
							}
							break;

							case REQUEST_TO_INTERFACE:
							if (!USB_ReqVendorIF(FALSE))
							{
								goto stall_i;/* not supported */
							}
							break;

							case REQUEST_TO_ENDPOINT:
							if (!USB_ReqVendorEP(FALSE))
							{
								goto stall_i;/* not supported */
							}
							break;
							default:
							goto stall_i;
						}
						nsAPI_USB_StatusInStage();
						break;  /* end case REQUEST_VENDOR */
	                #endif  /* USB_VENDOR */

						default:
						nsAPI_USBCore_Stall_i();
						break;
					}

				}
			}
			else
			{
				nsAPI_USB_StatusOutStage();/* receive Acknowledge */
			}
		}
		break;  /* end case USB_EVT_OUT */

		case USB_EVT_IN:		//3
			if(g_USBCore_Setting.SetupPacket.bmRequestType.BM.Dir == REQUEST_DEVICE_TO_HOST)
			{
				nsAPI_USB_DataInStage();/* send data */
			}
			else
			{
				if(g_USBCore_Setting.USB_DeviceAddress & 0x80)
				{
					g_USBCore_Setting.USB_DeviceAddress &= 0x7F;
					nsAPI_USB_SetAddress(g_USBCore_Setting.USB_DeviceAddress);
				}
			}
		break;  /* end case USB_EVT_IN */

		case USB_EVT_OUT_STALL:
			nsAPI_USB_ClrStallEP(0x00);
		break;

		case USB_EVT_IN_STALL:
			nsAPI_USB_ClrStallEP(0x80);
		break;
	}
}

void nsAPI_USBCore_Stall_i(void)
{
	nsAPI_USB_SetStallEP(0x80);
	g_USBCore_Setting.EP0Data.Count = 0;
}

#undef __USBCORE_C__
