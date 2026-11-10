/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbdesc.c
 * Purpose: USB Descriptors
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
 *          V1.20 Changed string descriptor handling
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/
#include "type.h"
#include "usb.h"
#include "hid.h"
#include "usbcfg.h"
#include "usbdesc.h"
#include "hiduser.h"

#define EP1_PACKET_SIZE 8
#define EP2_PACKET_SIZE 8             
#define EP3_PACKET_SIZE 64            
#define EP4_PACKET_SIZE 64            

/* HID Report Descriptor */

const U8 HID_ReportDescriptorMouse[] =
{

	0x05, 0x01,       	 /*Usage Page (Generic Desktop)*/
	0x09, 0x02,       	 /*Usage (Mouse)*/
	0xA1, 0x01,       	 /*Collection (Application)*/

 //	0x85, 0x01,	 	 /*Report ID (1)*/   						// [ID1]- Total 7 Bytes
 	0x09, 0x01,		 /*Usage (Pointer)*/
 	0xA1, 0x00,               /*Collection (Physical)*/

	0x85, 0x01,							//report ID = 0X01
 	0x05, 0x09,               /*Usage Page (Button)*/
 	0x19, 0x01,               /*Usage Minimum (Button 1)*/
 	0x29, 0x05,               /*Usage Maximum (Button 7)*/
 	0x15, 0x00,               /*Logical Minimum (0)*/
 	0x25, 0x01,               /*Logical Maximum (1)*/

  0x95, 0x05,               /*Report Count (7)*/			    // 7 button bits
  0x75, 0x01,               /*Report Size (1)*/
 	0x81, 0x02,               /*Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)*/

  	0x95, 0x01,	          /*Report Count (5)*/
  	0x75, 0x03,               /*Report Size (1)*/ 		  // Bono:1 bit padding
  	0x81, 0x01,               /*Input (Cnst,Ary,Abs)*/
		
  	0x05, 0x01,               /*Usage Page (Generic Desktop)*/
  	0x09, 0x30,               /*Usage (X)*/
  	0x09, 0x31,               /*Usage (Y)*/
  	0x16, 0x00, 0x80,         /*Logical Minimum (-32767)*/
  	0x26, 0xFF, 0x7F,         /*Logical Maximum (32767)*/
  	0x75, 0x10,               /*Report Size (16)*/
  	0x95, 0x02,               /*Report Count (2)*/					// 2 Word(4 Byte)
  	0x81, 0x06,               /*Input (Data,Var,Rel,NWrp,Lin,Pref,NNul,Bit)*/
	0xc0,											//END COLLECTION
	0xa1, 0x00,  								    //COLLECTION (Physical) 
	0x05, 0x01,   									//USAGE_PAGE (Generic Desktop)
 	0x09, 0x38,               /*Usage (Wheel)*/
 	0x15, 0x81,               /*Logical Minimum(-127)*/
 	0x25, 0x7F,               /*Logical Maximum(127)*/
 	0x75, 0x08,               /*Report Size(8)*/
 	0x95, 0x01,               /*Report Count(1)*/					// 1 Byte
 	0x81, 0x06,               /*Input(Data,Var,Rel,NWrp,Lin,Pref,NNul,Bit)*/
	0xc0,  											//END COLLECTION
	0xa1,  0x00,  								    //COLLECTION (Physical)   
	0x05,  0x0c,									//USAGE_PAGE(Consumer Devices)  
	0x0a,  0x38,  0x02, 							//SWING
	0x95,  0x01,  									//REPORT_COUNT (1)
	0x75,  0x08,	  								//REPORT_SIZE (8)
	0x15,  0x81,   									//LOGICAL_MINIMUM (-127)
	0x25,  0x7f,   									//LOGICAL_MAXIMUM (127)
	0x81,  0x06,   								    //INPUT (Data,Var,Rel)
	0xc0,											//END COLLECTION

 	0xC0,      		  /*End Collection*/
	
  0x05u, 0x0Cu,             /* USAGE_PAGE                              */ 
  0x09u, 0x01u,             /* USAGE                                   */ 
  0xA1u, 0x01u,            /* COLLECTION                              */  
  0x85u, 0x05u,             /* REPORT_ID                               */
  0x19u, 0x00u,             /* USAGE_MINIMUM                           */ 
  0x2Au, 0x3Cu, 0x02u,      /* USAGE_MAXIMUM                           */
  0x15u, 0x00u,             /* LOGICAL_MINIMUM                         */
  0x26u, 0x3Cu, 0x02u,      /* LOGICAL_MAXIMUM                         */
  0x95u, 0x01u,             /* REPORT_COUNT                            */
  0x75u, 0x10u,             /* REPORT_SIZE                             */
  0x81u, 0x00u,             /* INPUT                                   */ 
  0xC0u,                    /* END_COLLECTION                          */ 
	



};


const uint8_t HID_ReportDescriptorKeyboard[] =
{
	/*
  0x05, 0x01,      // USAGE_PAGE (Generic Desktop) 
  0x09, 0x06,      // USAGE (Keyboard) 
  0xa1, 0x01,      // COLLECTION (Application) 
  0x05, 0x07,      //   USAGE_PAGE (Keyboard) 
  0x19, 0xe0,      //   USAGE_MINIMUM (Keyboard //                  LeftControl) 
  0x29, 0xe7,      //   USAGE_MAXIMUM (Keyboard //                  Right GUI) 
  0x15, 0x00,      //   LOGICAL_MINIMUM (0) 
  0x25, 0x01,      //   LOGICAL_MAXIMUM (1) 
  0x75, 0x01,      //   REPORT_SIZE (1) 
  0x95, 0x08,      //   REPORT_COUNT (8) 
  0x81, 0x02,      //   INPUT (Data,Var,Abs) 
  0x95, 0x01,      //   REPORT_COUNT (1) 
  0x75, 0x08,      //   REPORT_SIZE (8) 
  0x25, 0x65,      //   LOGICAL_MAXIMUM (101) 
  0x19, 0x00,      //   USAGE_MINIMUM (Reserved //   (no event indicated)) 
  0x29, 0x65,      //   USAGE_MAXIMUM //   (Keyboard Application) 
  0x81, 0x00,      //   INPUT (Data,Ary,Abs) 
  0xc0,              //    END_COLLECTION 
	*/
/* USAGE_PAGE                              */ 0x05u, 0x01u, 
/* USAGE                                   */ 0x09u, 0x06u, 
/* COLLECTION                              */ 0xA1u, 0x01u, 
/* USAGE_PAGE                              */ 0x05u, 0x07u, 
/* USAGE_MINIMUM                           */ 0x19u, 0xE0u, 
/* USAGE_MAXIMUM                           */ 0x29u, 0xE7u, 
/* LOGICAL_MINIMUM                         */ 0x15u, 0x00u, 
/* LOGICAL_MAXIMUM                         */ 0x25u, 0x01u, 
/* REPORT_SIZE                             */ 0x75u, 0x01u, 
/* REPORT_COUNT                            */ 0x95u, 0x08u, 
/* INPUT                                   */ 0x81u, 0x02u, 
/* REPORT_COUNT                            */ 0x95u, 0x01u, 
/* REPORT_SIZE                             */ 0x75u, 0x08u, 
/* INPUT                                   */ 0x81u, 0x01u, 
/* USAGE_PAGE                              */ 0x05u, 0x07u, 
/* USAGE_MINIMUM                           */ 0x19u, 0x00u, 
/* USAGE_MAXIMUM                           */ 0x29u, 0xA4u, 
/* LOGICAL_MAXIMUM                         */ 0x26u, 0xA4u, 0x00u, 
/* REPORT_COUNT                            */ 0x95u, 0x06u, 
/* INPUT                                   */ 0x81u, 0x00u, 
/* USAGE_PAGE                              */ 0x05u, 0x08u, 
/* LOGICAL_MAXIMUM                         */ 0x25u, 0x01u, 
/* USAGE_MINIMUM                           */ 0x19u, 0x01u, 
/* USAGE_MAXIMUM                           */ 0x29u, 0x05u, 
/* REPORT_SIZE                             */ 0x75u, 0x01u, 
/* REPORT_COUNT                            */ 0x95u, 0x05u, 
/* OUTPUT                                  */ 0x91u, 0x02u, 
/* REPORT_SIZE                             */ 0x75u, 0x01u, 
/* REPORT_COUNT                            */ 0x95u, 0x03u, 
/* OUTPUT                                  */ 0x91u, 0x01u, 
/* END_COLLECTION                          */ 0xC0u,

};

//const uint16_t KBCSReportDesc = sizeof(KBCSReportDescriptor);

const uint8_t HID_ReportDescriptorConsumerControl[] =
{
    0x06, 0x01, 0xff,              // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x02,                    //   USAGE (Vendor Usage 2)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x40,                    //   REPORT_COUNT (64)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x09, 0x03,                    //   USAGE (Vendor Usage 3)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x40,                    //   REPORT_COUNT (64)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0xc0,                           // END_COLLECTION
};

const U16 HID_ReportDescMouseSize = sizeof(HID_ReportDescriptorMouse);

const U16 HID_ReportDescKeyboardSize = sizeof(HID_ReportDescriptorKeyboard);

const U16 HID_ReportDescConsumerSize = sizeof(HID_ReportDescriptorConsumerControl);

/* USB Standard Device Descriptor */

const U8 USB_DeviceDescriptor[] =
{
	USB_DEVICE_DESC_SIZE,			/* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE,	/* bDescriptorType */
	WBVAL(0x0200),					/* 2.01 *//* bcdUSB */
	0x00,							/* bDeviceClass */
	0x00,							/* bDeviceSubClass */
	0x00,							/* bDeviceProtocol */
	USB_MAX_PACKET0,				/* bMaxPacketSize0 */
	WBVAL(USB_VID),					/* idVendor */
	WBVAL(USB_PID),					/* idProduct */
	WBVAL(PRODUCT_VER),				/* 1.00 *//* bcdDevice */
	0x01,							/* iManufacturer */
	0x02,							/* iProduct */
	0x00,							/* iSerialNumber */
	0x01							/* bNumConfigurations: one possible configuration*/
};


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor) */
const U8 USB_ConfigDescriptor[] =
{
	//{/* Configuration 1 */
		USB_CONFIGUARTION_DESC_SIZE,		/* bLength */
		USB_CONFIGURATION_DESCRIPTOR_TYPE,		/* bDescriptorType */

		WBVAL(
		USB_CONFIGUARTION_DESC_SIZE+
		(USB_INTERFACE_DESC_SIZE*3)+			//mouse
		(HID_DESC_SIZE*3)+
		(USB_ENDPOINT_DESC_SIZE*4)
		), 	// Totallength
		0x03,								/* bNumInterfaces */		// HansH Modify
		0x01,								/* bConfigurationValue */
		0x00,								/* iConfiguration */
		USB_CONFIG_BUS_POWERED|USB_CONFIG_REMOTE_WAKEUP,	// bmAttributes (0x80 + Remote Wakeup)
//		USB_CONFIG_BUS_POWERED,
		USB_CONFIG_POWER_MA(400),		//98					/* bMaxPower */
//-------------------------------------------------------
	//}

	//-----------------------------------------------------------------------------
	// MOUSE interface decriptor
	//-----------------------------------------------------------------------------
	// interface_descriptor hid_interface_descriptor    (MOUSE)
	//{
		/* Interface 0, Alternate Setting 0, HID Class */
		USB_INTERFACE_DESC_SIZE,			/* bLength */
		USB_INTERFACE_DESCRIPTOR_TYPE,		/* bDescriptorType */
		0x00,								/* bInterfaceNumber */
		0x00,								/* bAlternateSetting */
		0x01,								/* bNumEndpoints */
		USB_DEVICE_CLASS_HUMAN_INTERFACE,	/* bInterfaceClass */
		HID_SUBCLASS_BOOT,	//HID_SUBCLASS_BOOT				/* bInterfaceSubClass */
		HID_PROTOCOL_MOUSE,				/* bInterfaceProtocol */
		0x00,								/* iInterface */
	//}
	//{
		/* HID Class Descriptor */
		/* HID_DESC_OFFSET = 0x0012 */
		HID_DESC_SIZE,					/* bLength */
		HID_HID_DESCRIPTOR_TYPE,		/* bDescriptorType */
		WBVAL(0x0111), 					/* 1.11 *//* bcdHID */
		0x00,							/* bCountryCode */
		0x01,							/* bNumDescriptors */
		HID_REPORT_DESCRIPTOR_TYPE,	/* bDescriptorType */
		WBVAL(HID_REPORT_DESC_MOUSE_SIZE),	/* wDescriptorLength */
	//}
	//{
		/* Endpoint, HID Interrupt In */
		USB_ENDPOINT_DESC_SIZE,			/* bLength */
		USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
		USB_ENDPOINT_IN(HID_EP1_IN),		/* bEndpointAddress */
		USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
		WBVAL(0x0010),				/* wMaxPacketSize */
		0x01,								/* 1ms *//* bInterval */
	//}
#if 1
	//-----------------------------------------------------------------------------
	// KEYBOARD interface decriptor
	//-----------------------------------------------------------------------------
	//{
		 // interface_descriptor hid_interface_descriptor    (KEYBOARD)
		USB_INTERFACE_DESC_SIZE,				/* bLength */
		USB_INTERFACE_DESCRIPTOR_TYPE,			/* bDescriptorType */
		0x01,									/* bInterfaceNumber */
		0x00,									/* bAlternateSetting */
		0x01,									/* bNumEndpoints */
		USB_DEVICE_CLASS_HUMAN_INTERFACE,		/* bInterfaceClass */
		HID_SUBCLASS_NONE,						/* bInterfaceSubClass */
		HID_PROTOCOL_KEYBOARD,					/* bInterfaceProcotol(0=None, 1=Keyb, 2=Mouse) */
		0x00,									/* iInterface */
	//}
	//{
		/* HID Class Descriptor */
		HID_DESC_SIZE,					/* bLength */
		HID_HID_DESCRIPTOR_TYPE,		/* bDescriptorType */
		WBVAL(0x0111), 					/* 1.11 *//* bcdHID */
		0x00,							/* bCountryCode */
		0x01,							/* bNumDescriptors */
		HID_REPORT_DESCRIPTOR_TYPE,	/* bDescriptorType */
		WBVAL(HID_REPORT_DESC_KEYBOARD_SIZE),		/* wDescriptorLength */
	//}
	//{
		/* Endpoint, HID Interrupt In */
		USB_ENDPOINT_DESC_SIZE,			/* bLength */
		USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
		USB_ENDPOINT_IN(HID_EP2_IN),		/* bEndpointAddress */
		USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
		WBVAL(EP2_PACKET_SIZE),				/* wMaxPacketSize */
		0x01,								/* 1ms *//* bInterval */
	//}
	//-----------------------------------------------------------------------------
	// VENDOR DEFINE interface decriptor
	//-----------------------------------------------------------------------------
	//{
		 // interface_descriptor hid_interface_descriptor    (CONSUMER and SYSTEM CONTROL)
		USB_INTERFACE_DESC_SIZE,				/* bLength */
		USB_INTERFACE_DESCRIPTOR_TYPE,			/* bDescriptorType */
		0x02,									/* bInterfaceNumber */
		0x00,									/* bAlternateSetting */
		0x02,									/* bNumEndpoints */
		USB_DEVICE_CLASS_HUMAN_INTERFACE,		/* bInterfaceClass */
		HID_SUBCLASS_NONE,  //HID_SUBCLASS_NONE,						/* bInterfaceSubClass */
		HID_PROTOCOL_NONE,					/* bInterfaceProcotol(0=None, 1=Keyb, 2=Mouse) */
		0x00,									/* iInterface */
	//}
	//{
		/* HID Class Descriptor */
		HID_DESC_SIZE,					/* bLength */
		HID_HID_DESCRIPTOR_TYPE,		/* bDescriptorType */
		WBVAL(0x0111), 					/* 1.11 *//* bcdHID */
		0x00,							/* bCountryCode */
		0x01,							/* bNumDescriptors */
		HID_REPORT_DESCRIPTOR_TYPE,	/* bDescriptorType */
		WBVAL(HID_REPORT_DESC_CONSUMER_SIZE),		/* wDescriptorLength */
	//}

  //{ //(Add)Abellee 20161019 Add FW update
		/* Endpoint, HID Interrupt In */
		USB_ENDPOINT_DESC_SIZE,			/* bLength */
		USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
		USB_ENDPOINT_IN(HID_EP3_IN),		/* bEndpointAddress */
		USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
		WBVAL(EP3_PACKET_SIZE),			/* wMaxPacketSize */
		0x01,								/* 1ms *//* bInterval */
	//}
	//{ //(Add)Abellee 20161019 Add FW update
		/* Endpoint, HID Interrupt Out */
		USB_ENDPOINT_DESC_SIZE,			/* bLength */
		USB_ENDPOINT_DESCRIPTOR_TYPE,		/* bDescriptorType */
		USB_ENDPOINT_OUT(HID_EP4_OUT),		/* bEndpointAddress */
		USB_ENDPOINT_TYPE_INTERRUPT,		/* bmAttributes */
		WBVAL(EP4_PACKET_SIZE),			/* wMaxPacketSize */
		0x01,								/* 1ms *//* bInterval */
	//}
#endif
};

/* USB String Descriptor (optional) */
const U8 USB_StringDescriptor[] =
{
	/* Index 0x00: LANGID Codes */
	0x04,                              /* bLength */
	USB_STRING_DESCRIPTOR_TYPE,/* bDescriptorType*/
	WBVAL(0x0409), /* US English *//* wLANGID */
	/* Index 0x01: Manufacturer */
	(5*2 + 2),/* bLength (7 Char + Type + lenght)*/
	USB_STRING_DESCRIPTOR_TYPE,/* bDescriptorType*/
	'G',0,
	't',0,
	'e',0,
	'c',0,
	'h',0,
	/* Index 0x02: Product */
	(20*2 + 2),/* bLength ( 24 Char + Type + lenght) */
	USB_STRING_DESCRIPTOR_TYPE,/* bDescriptorType */
	'G',0,
	't',0,
	'e',0,
	'c',0,
	'h',0,
	' ',0,
	'M',0,
	'a',0,
	'g',0,
	'n',0,
	'e',0,
	't',0,
	'i',0,
	'c',0,
	' ',0,
	'M',0,
	'o',0,
	'u',0,
	's',0,
	'e',0,
};

#if LPM_SUPPORT
/* In order to test Link Power Management(LPM), bcdUSB needs to be 0x0201(USB 2.0
Extension) instead of 0x0200. Below BOS descriptor is for LPM test only. However,
current USBC2.0 CV test requires both Super Speed Device Capability Descriptor
and Device Capability descriptor to be present and wSpeedSupport should be 0x000F
instead of 0x0003 indicating it's a Super Speed Device or USBCV test will fail.
For USB2.0 extention and LPM support, the device is not necessary a Super Speed
Device. To be discussed with USB-IF. */

/* USB BOS Descriptor */
const uint8_t USB_BOSDescriptor[] = {
  USB_BOS_DESC_SIZE,                 /* bLength */
  USB_BOS_TYPE,                      /* bDescriptorType, BOS */
  WBVAL(                             /* wTotalLength */
    USB_BOS_DESC_SIZE +
    USB_SS_DEVICE_CAPABILITY_SIZE +
    USB_DEVICE_CAPABILITY_SIZE
  ),
  0x02,                               /* bNumDeviceCaps, One Device Capability desc. */
  /* Device Capabilities, USB 2.0 Extension Descriptor */
  USB_SS_DEVICE_CAPABILITY_SIZE,     /* bLength */
  USB_DEVICE_CAPABILITY_TYPE,        /* bDescriptorType, DEVICE CAPABILITY */
  0x03,                              /* bDeviceCapabilityType, USB Super Speed */
  0x00,                              /* bmAttributes */
  WBVAL(0x000F),                     /* wSpeedsSupported, All speeds are supported. */
  0x02,                              /* bFunctionalitySupport, Full Speed */
  0x00,                              /* bU1DevExitLat */
  WBVAL(0x0000),                     /* bU2DevExitLat */
  /* Device Capabilities, USB 2.0 Extension Descriptor */
  USB_DEVICE_CAPABILITY_SIZE,        /* bLength */
  USB_DEVICE_CAPABILITY_TYPE,        /* bDescriptorType, DEVICE CAPABILITY */
  0x02,                              /* bDeviceCapabilityType, USB2.0 EXTENSION */
  (0x1<<1),                          /* bmAttributes, bit1 to 1, LPM supported. */
  0x00,
  0x00,
  0x00
};
#endif

