/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_hid.h"

#include "usb_device_descriptor.h"

#include "composite.h"

#include "hid_mouse.h"
#include "hid_keyboard.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* hid keyboard endpoint information */
usb_device_endpoint_struct_t g_UsbDeviceHidKeyboardEndpoints[USB_HID_KEYBOARD_ENDPOINT_COUNT] = {
    /* HID keyboard interrupt IN pipe */
    {
        USB_HID_KEYBOARD_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
        USB_ENDPOINT_INTERRUPT,
        FS_HID_KEYBOARD_INTERRUPT_IN_PACKET_SIZE,
    },
	//{
	//	USB_HID_GENERIC_ENDPOINT_OUT | (USB_OUT << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
	//	USB_ENDPOINT_INTERRUPT,
	//	FS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE,
	//	FS_HID_GENERIC_INTERRUPT_OUT_INTERVAL,
	//}
};

/* HID keyboard interface information */
usb_device_interface_struct_t g_UsbDeviceHidKeyboardInterface[] = {{
    USB_HID_KEYBOARD_INTERFACE_ALTERNATE_0, /* The alternate setting of the interface */
    {
        USB_HID_KEYBOARD_ENDPOINT_COUNT, /* Endpoint count */
        g_UsbDeviceHidKeyboardEndpoints, /* Endpoints handle */
    },
    NULL,
}};

usb_device_interfaces_struct_t g_UsbDeviceHidKeyboardInterfaces[USB_HID_KEYBOARD_INTERFACE_COUNT] = {
    {
        USB_HID_KEYBOARD_CLASS,           /* HID keyboard class code */
        USB_HID_KEYBOARD_SUBCLASS,        /* HID keyboard subclass code */
        USB_HID_KEYBOARD_PROTOCOL,        /* HID keyboard protocol code */
        USB_HID_KEYBOARD_INTERFACE_INDEX, /* The interface number of the HID keyboard */
        g_UsbDeviceHidKeyboardInterface,  /* Interfaces handle */
        sizeof(g_UsbDeviceHidKeyboardInterface) / sizeof(usb_device_interface_struct_t),
    },
};

usb_device_interface_list_t g_UsbDeviceHidKeyboardInterfaceList[USB_DEVICE_CONFIGURATION_COUNT] = {
    {
        USB_HID_KEYBOARD_INTERFACE_COUNT, /* The interface count of the HID keyboard */
        g_UsbDeviceHidKeyboardInterfaces, /* The interfaces handle */
    },
};

usb_device_class_struct_t g_UsbDeviceHidKeyboardConfig = {
    g_UsbDeviceHidKeyboardInterfaceList, /* The interface list of the HID keyboard */
    kUSB_DeviceClassTypeHid,             /* The HID class type */
    USB_DEVICE_CONFIGURATION_COUNT,      /* The configuration count */
};

#if 1
/* hid keyboard endpoint information */
usb_device_endpoint_struct_t g_UsbDeviceHidBitmapEndpoints[USB_HID_Bitmap_ENDPOINT_COUNT] = {
    /* HID keyboard interrupt IN pipe */
    {
        USB_HID_Bitmap_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
        USB_ENDPOINT_INTERRUPT,
		USB_HID_Bitmap_REPORT_IN_LENGTH,
    },
};

/* HID keyboard interface information */
usb_device_interface_struct_t g_UsbDeviceHidBitmapInterface[] = {{
    USB_HID_Bitmap_INTERFACE_ALTERNATE_0, /* The alternate setting of the interface */
    {
        USB_HID_Bitmap_ENDPOINT_COUNT, /* Endpoint count */
        g_UsbDeviceHidBitmapEndpoints, /* Endpoints handle */
    },
    NULL,
}};
usb_device_interfaces_struct_t g_UsbDeviceHidBitmapInterfaces[USB_HID_Bitmap_INTERFACE_COUNT] = {
    {
        USB_HID_Bitmap_CLASS,           /* HID keyboard class code */
        USB_HID_Bitmap_SUBCLASS,        /* HID keyboard subclass code */
        USB_HID_Bitmap_PROTOCOL,        /* HID keyboard protocol code */
        USB_HID_Bitmap_INTERFACE_INDEX, /* The interface number of the HID keyboard */
        g_UsbDeviceHidBitmapInterface,  /* Interfaces handle */
        sizeof(g_UsbDeviceHidBitmapInterface) / sizeof(usb_device_interface_struct_t),
    },
};
usb_device_interface_list_t g_UsbDeviceHidBitmapInterfaceList[USB_DEVICE_CONFIGURATION_COUNT] = {
    {
        USB_HID_Bitmap_INTERFACE_COUNT, /* The interface count of the HID keyboard */
        g_UsbDeviceHidBitmapInterfaces, /* The interfaces handle */
    },
};
usb_device_class_struct_t g_UsbDeviceHidBitmapConfig = {
    g_UsbDeviceHidBitmapInterfaceList, /* The interface list of the HID keyboard */
    kUSB_DeviceClassTypeHid,             /* The HID class type */
    USB_DEVICE_CONFIGURATION_COUNT,      /* The configuration count */
};
#endif

#if 1
/* hid keyboard endpoint information */
usb_device_endpoint_struct_t g_UsbDeviceHidCustomEndpoints[USB_HID_Custom_ENDPOINT_COUNT] = {
    /* HID keyboard interrupt IN pipe */
	{
		USB_HID_Custom_ENDPOINT_OUT | (USB_OUT << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
		USB_ENDPOINT_INTERRUPT,
		FS_HID_Custom_INTERRUPT_OUT_PACKET_SIZE,
		FS_HID_Custom_INTERRUPT_OUT_INTERVAL,
	},
    {
        USB_HID_Custom_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
        USB_ENDPOINT_INTERRUPT,
        FS_HID_KEYBOARD_INTERRUPT_IN_PACKET_SIZE,
    }
};

/* HID keyboard interface information */
usb_device_interface_struct_t g_UsbDeviceHidCustomInterface[] = {{
    USB_HID_Custom_INTERFACE_ALTERNATE_0, /* The alternate setting of the interface */
    {
        USB_HID_Custom_ENDPOINT_COUNT, /* Endpoint count */
        g_UsbDeviceHidCustomEndpoints, /* Endpoints handle */
    },
    NULL,
}};
usb_device_interfaces_struct_t g_UsbDeviceHidCustomInterfaces[USB_HID_Custom_INTERFACE_COUNT] = {
    {
        USB_HID_Custom_CLASS,           /* HID keyboard class code */
        USB_HID_Custom_SUBCLASS,        /* HID keyboard subclass code */
        USB_HID_Custom_PROTOCOL,        /* HID keyboard protocol code */
        USB_HID_Custom_INTERFACE_INDEX, /* The interface number of the HID keyboard */
        g_UsbDeviceHidCustomInterface,  /* Interfaces handle */
        sizeof(g_UsbDeviceHidCustomInterface) / sizeof(usb_device_interface_struct_t),
    },
};
usb_device_interface_list_t g_UsbDeviceHidCustomInterfaceList[USB_DEVICE_CONFIGURATION_COUNT] = {
    {
        USB_HID_Custom_INTERFACE_COUNT, /* The interface count of the HID keyboard */
        g_UsbDeviceHidCustomInterfaces, /* The interfaces handle */
    },
};
usb_device_class_struct_t g_UsbDeviceHidCustomConfig = {
    g_UsbDeviceHidCustomInterfaceList, /* The interface list of the HID keyboard */
    kUSB_DeviceClassTypeHid,             /* The HID class type */
    USB_DEVICE_CONFIGURATION_COUNT,      /* The configuration count */
};
#endif
/* hid mouse endpoint information */
usb_device_endpoint_struct_t g_UsbDeviceHidMouseEndpoints[USB_HID_MOUSE_ENDPOINT_COUNT] = {
    /* HID mouse interrupt IN pipe */
    {
        USB_HID_MOUSE_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
        USB_ENDPOINT_INTERRUPT,
        FS_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE,
    },
};

/* HID mouse interface information */
usb_device_interface_struct_t g_UsbDeviceHidMouseInterface[] = {{
    USB_HID_MOUSE_INTERFACE_ALTERNATE_0, /* The alternate setting of the interface */
    {
        USB_HID_MOUSE_ENDPOINT_COUNT, /* Endpoint count */
        g_UsbDeviceHidMouseEndpoints, /* Endpoints handle */
    },
    NULL,
}};

usb_device_interfaces_struct_t g_UsbDeviceHidMouseInterfaces[USB_HID_MOUSE_INTERFACE_COUNT] = {
    {
        USB_HID_MOUSE_CLASS,           /* HID mouse class code */
        USB_HID_MOUSE_SUBCLASS,        /* HID mouse subclass code */
        USB_HID_MOUSE_PROTOCOL,        /* HID mouse protocol code */
        USB_HID_MOUSE_INTERFACE_INDEX, /* The interface number of the HID mouse */
        g_UsbDeviceHidMouseInterface,  /* Interfaces handle */
        sizeof(g_UsbDeviceHidMouseInterface) / sizeof(usb_device_interface_struct_t),
    },
};

usb_device_interface_list_t g_UsbDeviceHidMouseInterfaceList[USB_DEVICE_CONFIGURATION_COUNT] = {
    {
        USB_HID_MOUSE_INTERFACE_COUNT, /* The interface count of the HID mouse */
        g_UsbDeviceHidMouseInterfaces, /* The interfaces handle */
    },
};

usb_device_class_struct_t g_UsbDeviceHidMouseConfig = {
    g_UsbDeviceHidMouseInterfaceList, /* The interface list of the HID mouse */
    kUSB_DeviceClassTypeHid,          /* The HID class type */
    USB_DEVICE_CONFIGURATION_COUNT,   /* The configuration count */
};

#if 1
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceHidCustomReportDescriptor[] = {
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
#endif

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceHidMouseReportDescriptor[] = {
#if 0
    0x05U, 0x01U, /* Usage Page (Generic Desktop)*/
    0x09U, 0x02U, /* Usage (Mouse) */
    0xA1U, 0x01U, /* Collection (Application) */
    0x09U, 0x01U, /* Usage (Pointer) */

    0xA1U, 0x00U, /* Collection (Physical) */
    0x05U, 0x09U, /* Usage Page (Buttons) */
    0x19U, 0x01U, /* Usage Minimum (01U) */
    0x29U, 0x03U, /* Usage Maximum (03U) */

    0x15U, 0x00U, /* Logical Minimum (0U) */
    0x25U, 0x01U, /* Logical Maximum (1U) */
    0x95U, 0x03U, /* Report Count (3U) */
    0x75U, 0x01U, /* Report Size (1U) */

    0x81U, 0x02U, /* Input(Data, Variable, Absolute) 3U button bit fields */
    0x95U, 0x01U, /* Report Count (1U) */
    0x75U, 0x05U, /* Report Size (5U) */
    0x81U, 0x01U, /* Input (Constant), 5U constant field */

    0x05U, 0x01U, /* Usage Page (Generic Desktop) */
    0x09U, 0x30U, /* Usage (X) */
    0x09U, 0x31U, /* Usage (Y) */
    0x09U, 0x38U, /* Usage (Z) */

    0x15U, 0x81U, /* Logical Minimum (-127) */
    0x25U, 0x7FU, /* Logical Maximum (127) */
    0x75U, 0x08U, /* Report Size (8U) */
    0x95U, 0x03U, /* Report Count (3U) */

    0x81U, 0x06U, /* Input(Data, Variable, Relative), Three position bytes (X & Y & Z)*/
    0xC0U,        /* End collection, Close Pointer collection*/
    0xC0U         /* End collection, Close Mouse collection */
#else
	   //---------Reprot ID 3 (Mouse)--------------------
		0x05,0x01,         //Usage Page(Generic Desktop Controls)
		0x09,0x02,         //Usage(Mouse)
		0xa1,0x01,         //Collection(Application)
		0x85,0x03,         //Report ID(3)
		0x09,0x01,         //Usage(Pointer)
		0xa1,0x00,         //Collection(Physical)
		0x05,0x09,         //Usage Page(Button)
		0x19,0x01,         //Usage Minimum(1)
		0x29,0x05,         //Usage Maximum(5)
		0x15,0x00,         //Logical Minimum(0)
		0x25,0x01,         //Logical Maximum(1)
		0x75,0x01,         //Report Size(1)
		0x95,0x05,         //Report Count(5)
		0x81,0x02,         //Input(Data,Variable,Absolute)
		0x95,0x03,         //Report Count(3)
		0x81,0x01,         //Input(Constant)
		0x05,0x01,         //Usage Page(Generic Desktop Controls)
		0x09,0x30,         //Usage(X)
		0x09,0x31,         //Usage(Y)
		0x16,0x00,0x80,    //Logical Minimum(-32768)
		0x26,0xff,0x7f,    //Logical Maximum(32767)
		0x75,0x10,         //Report Size(16)
		0x95,0x02,         //Report Count(2)
		0x81,0x06,         //Input(Data, Variable, Relative)
		0x09,0x38,         //Usage(Wheel)
		0x15,0x81,         //Logical Minimum(-127)
		0x25,0x7f,         //Logical Maximum(+127)
		0x75,0x08,         //Report Size(8)
		0x95,0x01,         //Report Count(1)
		0x81,0x06,         //Input(Data,Variable,Relative)
		0x05,0x0c,         //Usage Page(Consumer)
		0x0a,0x38,0x02,    //Usage(AC Pan)
		0x95,0x01,         //Report Count(1)
		0x81,0x06,         //Input(Data,Variable,Relative)
		0xc0,0xc0,         //End Collection

		//-------- Report ID 4 (Media key)--------------------
		0x05, 0x0c,  		// usage_page (Consumer Devices)
		0x09, 0x01,  		// Usage (Consumer Control)
		0xa1, 0x01, 		// Collection (Application)
		0x85, 0x04, 		// Report_ID (4)
		0x19, 0x00,     	//USAGE Minimum (Scan Next Track)
		0x2A, 0xFF, 0x07,	//USAGE Maximum (Stop)
		0x15, 0x00,     	//LOGICAL_MINIMUM (0)
		0x26, 0xFF, 0x07,	//LOGICAL_MAXIMUM (1)
		0x95, 0x01,     	//REPORT_COUNT (1)
		0x75, 0x10,     	//REPORT_SIZE (16)
		0x81, 0x00,     	//INPUT (Data,Var,Abs)
		0xC0          		//END_COLLECTION
#endif
};

/* HID keyboard report descriptor */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceHidKeyboardReportDescriptor[] = {
#if 0
	    0x05U, 0x01U, /* Usage Page (Generic Desktop)*/
	    0x09U, 0x06U, /* Usage (Keyboard) */
	    0xA1U, 0x01U, /* Collection (Application) */
	    0x05U, 0x07U, /* Usage Page (Key Codes) */
	    0x19U, 0xE0U, /* Usage Minimum (224) */
	    0x29U, 0xE7U, /* Usage Maximum (231) */
	    0x15U, 0x00U, /* Logical Minimum (0) */
	    0x25U, 0x01U, /* Logical Maximum (1) */
	    0x75U, 0x01U, /* Report Size (1) */
	    0x95U, 0x08U, /* Report Count (8) */
	    0x81U, 0x02U, /* Input(Data, Variable, Absolute) Modifier byte */

	    0x95U, 0x01U, /* Report Count (1U) */
	    0x75U, 0x08U, /* Report Size (8U) */
	    0x81U, 0x01U, /* Input (Constant), Reserved byte */
	    0x95U, 0x05U, /* Report Count (5U) */
	    0x75U, 0x01U, /* Report Size (1U) */

	    0x05U, 0x08U, /* Usage Page (Page# for LEDs) */
	    0x19U, 0x01U, /* Usage Minimum (1U) */
	    0x29U, 0x05U, /* Usage Maximum (5U) */
	    0x91U, 0x02U, /* Output (Data, Variable, Absolute) LED report */
	    0x95U, 0x01U, /* Report Count (1U) */
	    0x75U, 0x03U, /* Report Size (3U) */
	    0x91U, 0x01U, /* Output (Constant), LED report padding */

	    0x95U, 0x06U, /* Report Count (6U)0x06U */
	    0x75U, 0x08U, /* Report Size (8U) */
	    0x15U, 0x00U, /* Logical Minimum (0U) */
	    0x25U, 0x65U, /* Logical Maximum (101U) */
	    0x05U, 0x07U, /* Usage Page (Key Codes) */
	    0x19U, 0x00U, /* Usage Minimum (0U) */
	    0x29U, 0x65U, /* Usage Maximum (101U) */

	    0x81U, 0x00U, /* Input(Data, Array), Key arrays(6U bytes)*/
	    0xC0U,        /* End collection */
#else
	0x05U, 0x01U, /* Usage Page (Generic Desktop)*/
	0x09U, 0x06U, /* Usage (Keyboard) */
	0xA1U, 0x01U, /* Collection (Application) */
	0x05U, 0x07U, /* Usage Page (Key Codes) */
	0x85,  0x01,     //Report ID
	0x19U, 0xE0U, /* Usage Minimum (224) */
	0x29U, 0xE7U, /* Usage Maximum (231) */
	0x15U, 0x00U, /* Logical Minimum (0) */
	0x25U, 0x01U, /* Logical Maximum (1) */
	0x75U, 0x01U, /* Report Size (1) */
	0x95U, 0x08U, /* Report Count (8) */
	0x81U, 0x02U, /* Input(Data, Variable, Absolute) Modifier byte */

	0x95U, 0x01U, /* Report Count (1U) */
	0x75U, 0x08U, /* Report Size (8U) */
	0x81U, 0x01U, /* Input (Constant), Reserved byte */
	0x95U, 0x05U, /* Report Count (5U) */
	0x75U, 0x01U, /* Report Size (1U) */

	0x05U, 0x08U, /* Usage Page (Page# for LEDs) */
	0x19U, 0x01U, /* Usage Minimum (1U) */
	0x29U, 0x05U, /* Usage Maximum (5U) */
	0x91U, 0x02U, /* Output (Data, Variable, Absolute) LED report */
	0x95U, 0x01U, /* Report Count (1U) */
	0x75U, 0x03U, /* Report Size (3U) */
	0x91U, 0x01U, /* Output (Constant), LED report padding */

    0x95U, 0x3DU, /* Report Count (61U) */
	//0x95U, 0x06U, /* Report Count (06U) */
    0x75U, 0x08U, /* Report Size (8U) */
    0x15U, 0x00U, /* Logical Minimum (0U) */
    0x25U, 0xE7U, /* Logical Maximum (101U) */
    0x05U, 0x07U, /* Usage Page (Key Codes) */
    0x19U, 0x00U, /* Usage Minimum (0U) */
    0x29U, 0xE7U, /* Usage Maximum (101U) */

    0x81U, 0x00U, /* Input(Data, Array), Key arrays(6U bytes)*/
    0xC0U,        /* End collection */
#endif
#if 0
	0x06, 0x00, 0xff,                         // USAGE_PAGE (Vendor defined page 1)
	0x09, 0x00,                               // USAGE (Vendor Usage 1)
	0xa1, 0x01,                               // COLLECTION (Application)
	0x85, 0x04,                               // Report ID  //new added
	0x15, 0x00,                               // LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,                         // LOGICAL_MAXIMUM (255)
	0x19, 0x00 ,            		          // Usage Minimum (00)
	0x2a, 0xff , 0x00,                        // Usage Maximum (ff)
	0x75, 0x08,                               // REPORT_SIZE (8 bit)
	0x95, 64-1,     //   REPORT_COUNT (64)
	0x09, 0x00,                               //   USAGE (Vendor Usage 1)
	0x81, 0x02,                               //   INPUT (Data,Var,Abs)
	0x09, 0x00,                               //   USAGE (Vendor Usage 1)
	0x91, 0x02,                               //   OUTPUT (Data,Var,Abs)
	0xc0,                                      // END_COLLECTION*/   //SIZE = 16*2=32
#endif
};

#if 1
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceHidBitmapReportDescriptor[] = {
		0x05U, 0x01U, 			/* Usage Page (Generic Desktop)*/
		0x09U, 0x06U, 			/* Usage (Keyboard) */
		0xA1U, 0x01U, 			/* Collection (Application) */
		0x05U, 0x07U, 			/* Usage Page (Key Codes) */
		0x85,  0x02U,  			/* Report ID */
		0x19U, 0x00U, 			/* Usage Minimum (0U) */
		0x29U, 0xE7U, 			/* Usage Maximum (0x67U) */
		0x15U, 0x00U, 			/* Logical Minimum (0) */
		0x26, 0xff, 0x00,   	/* Logical Maximum (255) */
		0x75U, 0x08U, 			/* Report Size (8) */
		0x95U, 0x3FU, 			/* Report Count (0x3FU) */
		0x81U, 0x00U, 			/* Input(Data, Variable, Absolute) Modifier byte */
	    0xC0U,        			/* End collection */
};
#endif

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceDescriptor[] = {
    USB_DESCRIPTOR_LENGTH_DEVICE, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_DEVICE,   /* DEVICE Descriptor Type */
    USB_SHORT_GET_LOW(USB_DEVICE_SPECIFIC_BCD_VERSION),
    USB_SHORT_GET_HIGH(USB_DEVICE_SPECIFIC_BCD_VERSION), /* USB Specification Release Number in
                                                            Binary-Coded Decimal (i.e., 2.10 is 210H). */
    USB_DEVICE_CLASS,                                    /* Class code (assigned by the USB-IF). */
    USB_DEVICE_SUBCLASS,                                 /* Subclass code (assigned by the USB-IF). */
    USB_DEVICE_PROTOCOL,                                 /* Protocol code (assigned by the USB-IF). */
    USB_CONTROL_MAX_PACKET_SIZE,                         /* Maximum packet size for endpoint zero
                                                            (only 8, 16, 32, or 64 are valid) */
    USB_SHORT_GET_LOW(USB_DEVICE_VID),
    USB_SHORT_GET_HIGH(USB_DEVICE_VID), /* Vendor ID (assigned by the USB-IF) */
    USB_SHORT_GET_LOW(USB_DEVICE_PID),
    USB_SHORT_GET_HIGH(USB_DEVICE_PID), /* Product ID (assigned by the manufacturer) */
    USB_SHORT_GET_LOW(USB_DEVICE_DEMO_BCD_VERSION),
    USB_SHORT_GET_HIGH(USB_DEVICE_DEMO_BCD_VERSION), /* Device release number in binary-coded decimal */
    0x01U,                                           /* Index of string descriptor describing manufacturer */
    0x02U,                                           /* Index of string descriptor describing product */
    0x00U,                                           /* Index of string descriptor describing the
                                                        device's serial number */
    USB_DEVICE_CONFIGURATION_COUNT,                  /* Number of possible configurations */
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceConfigurationDescriptor[] = {
    USB_DESCRIPTOR_LENGTH_CONFIGURE, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_CONFIGURE,   /* CONFIGURATION Descriptor Type */
    USB_SHORT_GET_LOW(USB_DESCRIPTOR_LENGTH_CONFIGURE +
    				   USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_HID +USB_DESCRIPTOR_LENGTH_ENDPOINT +
					   USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_HID +USB_DESCRIPTOR_LENGTH_ENDPOINT+//USB_DESCRIPTOR_LENGTH_ENDPOINT+
					   USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_HID +USB_DESCRIPTOR_LENGTH_ENDPOINT +
					   USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_HID +USB_DESCRIPTOR_LENGTH_ENDPOINT+USB_DESCRIPTOR_LENGTH_ENDPOINT),
    USB_SHORT_GET_HIGH(USB_DESCRIPTOR_LENGTH_CONFIGURE +
    					USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_HID +USB_DESCRIPTOR_LENGTH_ENDPOINT +
						USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_HID +USB_DESCRIPTOR_LENGTH_ENDPOINT+//USB_DESCRIPTOR_LENGTH_ENDPOINT+
						USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_HID +USB_DESCRIPTOR_LENGTH_ENDPOINT +
						USB_DESCRIPTOR_LENGTH_INTERFACE + USB_DESCRIPTOR_LENGTH_HID +USB_DESCRIPTOR_LENGTH_ENDPOINT+USB_DESCRIPTOR_LENGTH_ENDPOINT),
						/* Total length of data returned for this configuration. */
    USB_COMPOSITE_INTERFACE_COUNT,                      /* Number of interfaces supported by this configuration */
    USB_COMPOSITE_CONFIGURE_INDEX,                      /* Value to use as an argument to the
                                                             SetConfiguration() request to select this configuration */
    0x00U,                                              /* Index of string descriptor describing this configuration */
    (USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_D7_MASK) |
        (USB_DEVICE_CONFIG_SELF_POWER << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_SELF_POWERED_SHIFT) |
        (USB_DEVICE_CONFIG_REMOTE_WAKEUP << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_REMOTE_WAKEUP_SHIFT),
    /* Configuration characteristics
         D7: Reserved (set to one)
         D6: Self-powered
         D5: Remote Wakeup
         D4...0: Reserved (reset to zero)
    */
    USB_DEVICE_MAX_POWER,            /* Maximum power consumption of the USB
                                      * device from the bus in this specific
                                      * configuration when the device is fully
                                      * operational. Expressed in 2 mA units
                                      *  (i.e., 50 = 100 mA).
                                      */
	//------------------------------------------------------------------------------------------
    USB_DESCRIPTOR_LENGTH_INTERFACE,  /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_INTERFACE,    /* INTERFACE Descriptor Type */
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    /* Numeric expression that is the total size of the
       Report descriptor. */
    /* The address of the endpoint on the USB device
       described by this descriptor. */
    /* Maximum packet size this endpoint is capable of
       sending or receiving when this configuration is
       selected. */
	//------------------------------------------------------------------------------------------
    USB_HID_MOUSE_INTERFACE_INDEX,   /* Number of this interface. */
    USB_HID_MOUSE_INTERFACE_ALTERNATE_0, /* Value used to select this alternate setting
                                        for the interface identified in the prior field */
    USB_HID_MOUSE_ENDPOINT_COUNT,    /* Number of endpoints used by this
                                          interface (excluding endpoint zero). */
    USB_HID_MOUSE_CLASS,             /* Class code (assigned by the USB-IF). */
    USB_HID_MOUSE_SUBCLASS,          /* Subclass code (assigned by the USB-IF). */
    USB_HID_MOUSE_PROTOCOL,          /* Protocol code (assigned by the USB). */
    0x00U,                           /* Index of string descriptor describing this interface */
	//------------------------------------------------------------------------------------------
    USB_DESCRIPTOR_LENGTH_HID,      /* Numeric expression that is the total size of the
                                       HID descriptor. */
    USB_DESCRIPTOR_TYPE_HID,        /* Constant name specifying type of HID
                                       descriptor. */
    0x11U, 0x01U,                   /* Numeric expression identifying the HID Class
                                       Specification release. */
    0x00U,                          /* Numeric expression identifying country code of
                                       the localized hardware */
    0x01U,                          /* Numeric expression specifying the number of
                                       class descriptors(at least one report descriptor) */
    USB_DESCRIPTOR_TYPE_HID_REPORT, /* Constant name identifying type of class descriptor. */
    USB_SHORT_GET_LOW(USB_DESCRIPTOR_LENGTH_HID_MOUSE_REPORT),
    USB_SHORT_GET_HIGH(USB_DESCRIPTOR_LENGTH_HID_MOUSE_REPORT),
	//------------------------------------------------------------------------------------------
    /* Numeric expression that is the total size of the
       Report descriptor. */
    USB_DESCRIPTOR_LENGTH_ENDPOINT, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_ENDPOINT,   /* ENDPOINT Descriptor Type */
    USB_HID_MOUSE_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
    /* The address of the endpoint on the USB device
       described by this descriptor. */
    USB_ENDPOINT_INTERRUPT, /* This field describes the endpoint's attributes */
    USB_SHORT_GET_LOW(FS_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE),
	USB_SHORT_GET_HIGH(FS_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE),
    /* Maximum packet size this endpoint is capable of
       sending or receiving when this configuration is
       selected. */
    FS_HID_MOUSE_INTERRUPT_IN_INTERVAL, /* Interval for polling endpoint for data transfers. */
	//------------------------------------------------------------------------------------------
    USB_DESCRIPTOR_LENGTH_INTERFACE,  /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_INTERFACE,    /* INTERFACE Descriptor Type */
    USB_HID_KEYBOARD_INTERFACE_INDEX, /* Number of this interface. */
    USB_HID_KEYBOARD_INTERFACE_ALTERNATE_0, /* Value used to select this alternate setting
                                         for the interface identified in the prior field */
    USB_HID_KEYBOARD_ENDPOINT_COUNT,  /* Number of endpoints used by this
                                        interface (excluding endpoint zero). */
    USB_HID_KEYBOARD_CLASS,           /* Class code (assigned by the USB-IF). */
    USB_HID_KEYBOARD_SUBCLASS,        /* Subclass code (assigned by the USB-IF). */
    USB_HID_KEYBOARD_PROTOCOL,        /* Protocol code (assigned by the USB). */
    0x00U,                            /* Index of string descriptor describing this interface */
	//------------------------------------------------------------------------------------------
    USB_DESCRIPTOR_LENGTH_HID,      /* Numeric expression that is the total size of the
                                       HID descriptor. */
    USB_DESCRIPTOR_TYPE_HID,        /* Constant name specifying type of HID
                                       descriptor. */
    0x11U, 0x01U,                   /* Numeric expression identifying the HID Class
                                       Specification release. */
    0x00U,                          /* Numeric expression identifying country code of
                                       the localized hardware */
    0x01U,                          /* Numeric expression specifying the number of
                                       class descriptors(at least one report descriptor) */
    USB_DESCRIPTOR_TYPE_HID_REPORT, /* Constant name identifying type of class descriptor. */
    USB_SHORT_GET_LOW(USB_DESCRIPTOR_LENGTH_HID_KEYBOARD_REPORT),
    USB_SHORT_GET_HIGH(USB_DESCRIPTOR_LENGTH_HID_KEYBOARD_REPORT),
	//------------------------------------------------------------------------------------------
    /* Numeric expression that is the total size of the
       Report descriptor. */
    USB_DESCRIPTOR_LENGTH_ENDPOINT, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_ENDPOINT,   /* ENDPOINT Descriptor Type */
    USB_HID_KEYBOARD_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
    /* The address of the endpoint on the USB device
       described by this descriptor. */
    USB_ENDPOINT_INTERRUPT, /* This field describes the endpoint's attributes */
    USB_SHORT_GET_LOW(FS_HID_KEYBOARD_INTERRUPT_IN_PACKET_SIZE),
    USB_SHORT_GET_HIGH(FS_HID_KEYBOARD_INTERRUPT_IN_PACKET_SIZE),
    /* Maximum packet size this endpoint is capable of
       sending or receiving when this configuration is
       selected. */
    FS_HID_KEYBOARD_INTERRUPT_IN_INTERVAL, /* Interval for polling endpoint for data transfers. */
	//------------------------------------------------------------------------------------------
	//USB_DESCRIPTOR_LENGTH_ENDPOINT, /* Size of this descriptor in bytes */
	//USB_DESCRIPTOR_TYPE_ENDPOINT,   /* ENDPOINT Descriptor Type */
	//USB_HID_GENERIC_ENDPOINT_OUT | (USB_OUT << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
	//USB_ENDPOINT_INTERRUPT, /* This field describes the endpoint's attributes */
	//USB_SHORT_GET_LOW(FS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE),
	//USB_SHORT_GET_HIGH(FS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE),
	//FS_HID_GENERIC_INTERRUPT_OUT_INTERVAL, /* Interval for polling endpoint for data transfers. */
	//------------------------------------------------------------------------------------------
#if 1
	USB_DESCRIPTOR_LENGTH_INTERFACE, /* Size of this descriptor in bytes */
	    USB_DESCRIPTOR_TYPE_INTERFACE,   /* INTERFACE Descriptor Type */
		USB_HID_Custom_INTERFACE_INDEX,   /* Number of this interface. */
	    USB_HID_MOUSE_INTERFACE_ALTERNATE_0, /* Value used to select this alternate setting
	                                        for the interface identified in the prior field */
		USB_HID_Custom_ENDPOINT_COUNT,    /* Number of endpoints used by this
	                                          interface (excluding endpoint zero). */
		USB_HID_Custom_CLASS,             /* Class code (assigned by the USB-IF). */
	    USB_HID_Custom_SUBCLASS,          /* Subclass code (assigned by the USB-IF). */
	    USB_HID_Custom_PROTOCOL,          /* Protocol code (assigned by the USB). */
	    0x00U,                           /* Index of string descriptor describing this interface */
		//------------------------------------------------------------------------------------------
	    USB_DESCRIPTOR_LENGTH_HID,      /* Numeric expression that is the total size of the
	                                       HID descriptor. */
	    USB_DESCRIPTOR_TYPE_HID,        /* Constant name specifying type of HID
	                                       descriptor. */
	    0x11U, 0x01U,                   /* Numeric expression identifying the HID Class
	                                       Specification release. */
	    0x00U,                          /* Numeric expression identifying country code of
	                                       the localized hardware */
	    0x01U,                          /* Numeric expression specifying the number of
	                                       class descriptors(at least one report descriptor) */
	    USB_DESCRIPTOR_TYPE_HID_REPORT, /* Constant name identifying type of class descriptor. */
	    USB_SHORT_GET_LOW(USB_DESCRIPTOR_LENGTH_HID_Custom_REPORT),
	    USB_SHORT_GET_HIGH(USB_DESCRIPTOR_LENGTH_HID_Custom_REPORT),
		//------------------------------------------------------------------------------------------
		USB_DESCRIPTOR_LENGTH_ENDPOINT, /* Size of this descriptor in bytes */
		USB_DESCRIPTOR_TYPE_ENDPOINT,   /* ENDPOINT Descriptor Type */
		USB_HID_Custom_ENDPOINT_OUT | (USB_OUT << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
		USB_ENDPOINT_INTERRUPT, /* This field describes the endpoint's attributes */
		USB_SHORT_GET_LOW(FS_HID_Custom_INTERRUPT_OUT_PACKET_SIZE),
		USB_SHORT_GET_HIGH(FS_HID_Custom_INTERRUPT_OUT_PACKET_SIZE),
		FS_HID_Custom_INTERRUPT_OUT_INTERVAL, /* Interval for polling endpoint for data transfers. */
		//------------------------------------------------------------------------------------------
	    USB_DESCRIPTOR_LENGTH_ENDPOINT, /* Size of this descriptor in bytes */
	    USB_DESCRIPTOR_TYPE_ENDPOINT,   /* ENDPOINT Descriptor Type */
		USB_HID_Custom_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
	    USB_ENDPOINT_INTERRUPT, /* This field describes the endpoint's attributes */
	    USB_SHORT_GET_LOW(FS_HID_Custom_INTERRUPT_IN_PACKET_SIZE),
	    USB_SHORT_GET_HIGH(FS_HID_Custom_INTERRUPT_IN_PACKET_SIZE),
	    FS_HID_Custom_INTERRUPT_IN_INTERVAL, /* Interval for polling endpoint for data transfers. */
		//------------------------------------------------------------------------------------------
#endif
		//------------------------------------------------------------------------------------------
	/* Bitmap interface */
#if 1
	USB_DESCRIPTOR_LENGTH_INTERFACE, /* Size of this descriptor in bytes */
	    USB_DESCRIPTOR_TYPE_INTERFACE,   /* INTERFACE Descriptor Type */
		USB_HID_Bitmap_INTERFACE_INDEX,   /* Number of this interface. */
	    USB_HID_MOUSE_INTERFACE_ALTERNATE_0, /* Value used to select this alternate setting
	                                        for the interface identified in the prior field */
		USB_HID_Bitmap_ENDPOINT_COUNT,    /* Number of endpoints used by this
	                                          interface (excluding endpoint zero). */
		USB_HID_Bitmap_CLASS,             /* Class code (assigned by the USB-IF). */
		USB_HID_Bitmap_SUBCLASS,          /* Subclass code (assigned by the USB-IF). */
		USB_HID_Bitmap_PROTOCOL,          /* Protocol code (assigned by the USB). */
	    0x00U,                           /* Index of string descriptor describing this interface */
		//------------------------------------------------------------------------------------------
	    USB_DESCRIPTOR_LENGTH_HID,      /* Numeric expression that is the total size of the
	                                       HID descriptor. */
	    USB_DESCRIPTOR_TYPE_HID,        /* Constant name specifying type of HID
	                                       descriptor. */
	    0x11U, 0x01U,                   /* Numeric expression identifying the HID Class
	                                       Specification release. */
	    0x00U,                          /* Numeric expression identifying country code of
	                                       the localized hardware */
	    0x01U,                          /* Numeric expression specifying the number of
	                                       class descriptors(at least one report descriptor) */
	    USB_DESCRIPTOR_TYPE_HID_REPORT, /* Constant name identifying type of class descriptor. */
	    USB_SHORT_GET_LOW(USB_DESCRIPTOR_LENGTH_HID_Bitmap_REPORT),
	    USB_SHORT_GET_HIGH(USB_DESCRIPTOR_LENGTH_HID_Bitmap_REPORT),
		//------------------------------------------------------------------------------------------
	    USB_DESCRIPTOR_LENGTH_ENDPOINT, /* Size of this descriptor in bytes */
	    USB_DESCRIPTOR_TYPE_ENDPOINT,   /* ENDPOINT Descriptor Type */
		USB_HID_Bitmap_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
	    USB_ENDPOINT_INTERRUPT, /* This field describes the endpoint's attributes */
	    USB_SHORT_GET_LOW(HS_HID_Bitmap_INTERRUPT_IN_PACKET_SIZE),
	    USB_SHORT_GET_HIGH(HS_HID_Bitmap_INTERRUPT_IN_PACKET_SIZE),
		HS_HID_Bitmap_INTERRUPT_IN_INTERVAL, /* Interval for polling endpoint for data transfers. */
#endif
		//------------------------------------------------------------------------------------------
};

#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceQualifierDescriptor[] = {
    USB_DESCRIPTOR_LENGTH_DEVICE_QUALITIER, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_DEVICE_QUALITIER,   /* DEVICE Descriptor Type */
    USB_SHORT_GET_LOW(USB_DEVICE_SPECIFIC_BCD_VERSION),
    USB_SHORT_GET_HIGH(USB_DEVICE_SPECIFIC_BCD_VERSION), /* USB Specification Release Number in
                                                            Binary-Coded Decimal (i.e., 2.10 is 210H). */
    USB_DEVICE_CLASS,                                    /* Class code (assigned by the USB-IF). */
    USB_DEVICE_SUBCLASS,                                 /* Subclass code (assigned by the USB-IF). */
    USB_DEVICE_PROTOCOL,                                 /* Protocol code (assigned by the USB-IF). */
    USB_CONTROL_MAX_PACKET_SIZE,                         /* Maximum packet size for endpoint zero
                                                            (only 8, 16, 32, or 64 are valid) */
    0x00U,                                               /* Number of Other-speed Configurations */
    0x00U,                                               /* Reserved for future use, must be zero */
};
#endif

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString0[] = {
    2U + 2U,
    USB_DESCRIPTOR_TYPE_STRING,
    0x09U,
    0x04U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString1[] = {
    2U + 2U * 5U, USB_DESCRIPTOR_TYPE_STRING,
    'G',           0x00U,
    't',           0x00U,
    'e',           0x00U,
    'c',           0x00U,
    'h',           0x00U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString2[] = {
    2U + 2U * 5U, USB_DESCRIPTOR_TYPE_STRING,
    'A',           0x00U,
    'T',           0x00U,
    'K',           0x00U,
    '6',           0x00U,
    '8',           0x00U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString3[] = {
    2U + 2U * 16U, USB_DESCRIPTOR_TYPE_STRING,
    'H',           0x00U,
    'I',           0x00U,
    'D',           0x00U,
    ' ',           0x00U,
    'M',           0x00U,
    'O',           0x00U,
    'U',           0x00U,
    'S',           0x00U,
    'E',           0x00U,
    ' ',           0x00U,
    'D',           0x00U,
    'E',           0x00U,
    'V',           0x00U,
    'I',           0x00U,
    'C',           0x00U,
    'E',           0x00U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString4[] = {
    2U + 2U * 19U, USB_DESCRIPTOR_TYPE_STRING,
    'H',           0x00U,
    'I',           0x00U,
    'D',           0x00U,
    ' ',           0x00U,
    'K',           0x00U,
    'E',           0x00U,
    'Y',           0x00U,
    'B',           0x00U,
    'O',           0x00U,
    'A',           0x00U,
    'R',           0x00U,
    'D',           0x00U,
    ' ',           0x00U,
    'D',           0x00U,
    'E',           0x00U,
    'V',           0x00U,
    'I',           0x00U,
    'C',           0x00U,
    'E',           0x00U,
};

uint32_t g_UsbDeviceStringDescriptorLength[USB_DEVICE_STRING_COUNT] = {
    sizeof(g_UsbDeviceString0), sizeof(g_UsbDeviceString1), sizeof(g_UsbDeviceString2),
    sizeof(g_UsbDeviceString3), sizeof(g_UsbDeviceString4),
};

uint8_t *g_UsbDeviceStringDescriptorArray[USB_DEVICE_STRING_COUNT] = {
    g_UsbDeviceString0, g_UsbDeviceString1, g_UsbDeviceString2, g_UsbDeviceString3, g_UsbDeviceString4,
};

usb_language_t g_UsbDeviceLanguage[USB_DEVICE_LANGUAGE_COUNT] = {{
    g_UsbDeviceStringDescriptorArray,
    g_UsbDeviceStringDescriptorLength,
    (uint16_t)0x0409U,
}};

usb_language_list_t g_UsbDeviceLanguageList = {
    g_UsbDeviceString0,
    sizeof(g_UsbDeviceString0),
    g_UsbDeviceLanguage,
    USB_DEVICE_LANGUAGE_COUNT,
};

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Get device descriptor request */
usb_status_t USB_DeviceGetDeviceDescriptor(usb_device_handle handle,
                                           usb_device_get_device_descriptor_struct_t *deviceDescriptor)
{
    deviceDescriptor->buffer = g_UsbDeviceDescriptor;
    deviceDescriptor->length = USB_DESCRIPTOR_LENGTH_DEVICE;
    return kStatus_USB_Success;
}
#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
/* Get device qualifier descriptor request */
usb_status_t USB_DeviceGetDeviceQualifierDescriptor(
    usb_device_handle handle, usb_device_get_device_qualifier_descriptor_struct_t *deviceQualifierDescriptor)
{
    deviceQualifierDescriptor->buffer = g_UsbDeviceQualifierDescriptor;
    deviceQualifierDescriptor->length = USB_DESCRIPTOR_LENGTH_DEVICE_QUALITIER;
    return kStatus_USB_Success;
}
#endif
/* Get device configuration descriptor request */
usb_status_t USB_DeviceGetConfigurationDescriptor(
    usb_device_handle handle, usb_device_get_configuration_descriptor_struct_t *configurationDescriptor)
{
    if (USB_COMPOSITE_CONFIGURE_INDEX > configurationDescriptor->configuration)
    {
        configurationDescriptor->buffer = g_UsbDeviceConfigurationDescriptor;
        configurationDescriptor->length = USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL;
        return kStatus_USB_Success;
    }
    return kStatus_USB_InvalidRequest;
}

/* Get device string descriptor request */
usb_status_t USB_DeviceGetStringDescriptor(usb_device_handle handle,
                                           usb_device_get_string_descriptor_struct_t *stringDescriptor)
{
    if (stringDescriptor->stringIndex == 0U)
    {
        stringDescriptor->buffer = (uint8_t *)g_UsbDeviceLanguageList.languageString;
        stringDescriptor->length = g_UsbDeviceLanguageList.stringLength;
    }
    else
    {
        uint8_t languageId    = 0U;
        uint8_t languageIndex = USB_DEVICE_STRING_COUNT;

        for (; languageId < USB_DEVICE_LANGUAGE_COUNT; languageId++)
        {
            if (stringDescriptor->languageId == g_UsbDeviceLanguageList.languageList[languageId].languageId)
            {
                if (stringDescriptor->stringIndex < USB_DEVICE_STRING_COUNT)
                {
                    languageIndex = stringDescriptor->stringIndex;
                }
                break;
            }
        }

        if (USB_DEVICE_STRING_COUNT == languageIndex)
        {
            return kStatus_USB_InvalidRequest;
        }
        stringDescriptor->buffer = (uint8_t *)g_UsbDeviceLanguageList.languageList[languageId].string[languageIndex];
        stringDescriptor->length = g_UsbDeviceLanguageList.languageList[languageId].length[languageIndex];
    }
    return kStatus_USB_Success;
}

/* Get hid descriptor request */
usb_status_t USB_DeviceGetHidDescriptor(usb_device_handle handle, usb_device_get_hid_descriptor_struct_t *hidDescriptor)
{
    return kStatus_USB_InvalidRequest;
}

/* Get hid report descriptor request */
usb_status_t USB_DeviceGetHidReportDescriptor(usb_device_handle handle,
                                              usb_device_get_hid_report_descriptor_struct_t *hidReportDescriptor)
{
    if (USB_HID_MOUSE_INTERFACE_INDEX == hidReportDescriptor->interfaceNumber)
    {
        hidReportDescriptor->buffer = g_UsbDeviceHidMouseReportDescriptor;
        hidReportDescriptor->length = USB_DESCRIPTOR_LENGTH_HID_MOUSE_REPORT;
    }
    else if (USB_HID_KEYBOARD_INTERFACE_INDEX == hidReportDescriptor->interfaceNumber)
    {
        hidReportDescriptor->buffer = g_UsbDeviceHidKeyboardReportDescriptor;
        hidReportDescriptor->length = USB_DESCRIPTOR_LENGTH_HID_KEYBOARD_REPORT;
    }
    else if (USB_HID_Bitmap_INTERFACE_INDEX == hidReportDescriptor->interfaceNumber)
	{
		hidReportDescriptor->buffer = g_UsbDeviceHidBitmapReportDescriptor;
		hidReportDescriptor->length = USB_DESCRIPTOR_LENGTH_HID_Bitmap_REPORT;
	}
    else if (USB_HID_Custom_INTERFACE_INDEX == hidReportDescriptor->interfaceNumber)
    {
        hidReportDescriptor->buffer = g_UsbDeviceHidCustomReportDescriptor;
        hidReportDescriptor->length = USB_DESCRIPTOR_LENGTH_HID_Custom_REPORT;
    }
    else
    {
        return kStatus_USB_InvalidRequest;
    }
    return kStatus_USB_Success;
}

/* Get hid physical descriptor request */
usb_status_t USB_DeviceGetHidPhysicalDescriptor(usb_device_handle handle,
                                                usb_device_get_hid_physical_descriptor_struct_t *hidPhysicalDescriptor)
{
    return kStatus_USB_InvalidRequest;
}

/* Due to the difference of HS and FS descriptors, the device descriptors and configurations need to be updated to match
 * current speed.
 * As the default, the device descriptors and configurations are configured by using FS parameters for both EHCI and
 * KHCI.
 * When the EHCI is enabled, the application needs to call this function to update device by using current speed.
 * The updated information includes endpoint max packet size, endpoint interval, etc. */
usb_status_t USB_DeviceSetSpeed(usb_device_handle handle, uint8_t speed)
{
    usb_descriptor_union_t *descriptorHead;
    usb_descriptor_union_t *descriptorTail;

    descriptorHead = (usb_descriptor_union_t *)&g_UsbDeviceConfigurationDescriptor[0];
    descriptorTail =
        (usb_descriptor_union_t *)(&g_UsbDeviceConfigurationDescriptor[USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL - 1U]);

    while (descriptorHead < descriptorTail)
    {
        if (descriptorHead->common.bDescriptorType == USB_DESCRIPTOR_TYPE_ENDPOINT)
        {
            if (USB_SPEED_HIGH == speed)
            {
                if (((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                    (USB_HID_MOUSE_ENDPOINT_IN ==
                     (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = HS_HID_MOUSE_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else if (((descriptorHead->endpoint.bEndpointAddress &
                           USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                         (USB_HID_KEYBOARD_ENDPOINT_IN ==
                          (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = HS_HID_KEYBOARD_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_HID_KEYBOARD_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else if (((descriptorHead->endpoint.bEndpointAddress &
                           USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                         (USB_HID_Bitmap_ENDPOINT_IN ==
                          (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = HS_HID_Bitmap_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_HID_Bitmap_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else if (((descriptorHead->endpoint.bEndpointAddress &
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                       USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                      (USB_HID_Custom_ENDPOINT_IN ==
                       (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
             {
                 descriptorHead->endpoint.bInterval = HS_HID_Custom_INTERRUPT_IN_INTERVAL;
                 USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_HID_Custom_INTERRUPT_IN_PACKET_SIZE,
                                                    descriptorHead->endpoint.wMaxPacketSize);
             }
             else if (((descriptorHead->endpoint.bEndpointAddress &
						   USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
						  USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
						 (USB_HID_Custom_ENDPOINT_OUT ==
						  (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
				{
					descriptorHead->endpoint.bInterval = HS_HID_Custom_INTERRUPT_OUT_INTERVAL;
					USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_HID_Custom_INTERRUPT_OUT_PACKET_SIZE,
													   descriptorHead->endpoint.wMaxPacketSize);
				}
                else
                {
                }
            }
            else
            {
                if (((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                    (USB_HID_MOUSE_ENDPOINT_IN ==
                     (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = FS_HID_MOUSE_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else if (((descriptorHead->endpoint.bEndpointAddress &
                           USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                         (USB_HID_KEYBOARD_ENDPOINT_IN ==
                          (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = FS_HID_KEYBOARD_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_HID_KEYBOARD_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else if (((descriptorHead->endpoint.bEndpointAddress &
                           USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                         (USB_HID_Bitmap_ENDPOINT_IN ==
                          (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = FS_HID_Bitmap_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_HID_Bitmap_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else if (((descriptorHead->endpoint.bEndpointAddress &
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                       USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                      (USB_HID_Custom_ENDPOINT_IN ==
                       (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
				 {
					 descriptorHead->endpoint.bInterval = FS_HID_Custom_INTERRUPT_IN_INTERVAL;
					 USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_HID_Custom_INTERRUPT_IN_PACKET_SIZE,
														descriptorHead->endpoint.wMaxPacketSize);
				 }
				 else if (((descriptorHead->endpoint.bEndpointAddress &
							   USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
							  USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
							 (USB_HID_Custom_ENDPOINT_OUT ==
							  (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
				{
					descriptorHead->endpoint.bInterval = FS_HID_Custom_INTERRUPT_OUT_INTERVAL;
					USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_HID_Custom_INTERRUPT_OUT_PACKET_SIZE,
													   descriptorHead->endpoint.wMaxPacketSize);
				}
                else
                {
                }
            }
        }
        descriptorHead = (usb_descriptor_union_t *)((uint8_t *)descriptorHead + descriptorHead->common.bLength);
    }

    if (USB_SPEED_HIGH == speed)
    {
        g_UsbDeviceHidMouseEndpoints[0].maxPacketSize = HS_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE;
    }
    else
    {
        g_UsbDeviceHidMouseEndpoints[0].maxPacketSize = FS_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE;
    }

    if (USB_SPEED_HIGH == speed)
    {
        g_UsbDeviceHidKeyboardEndpoints[0].maxPacketSize = HS_HID_KEYBOARD_INTERRUPT_IN_PACKET_SIZE;
    }
    else
    {
        g_UsbDeviceHidKeyboardEndpoints[0].maxPacketSize = FS_HID_KEYBOARD_INTERRUPT_IN_PACKET_SIZE;
    }
    if (USB_SPEED_HIGH == speed)
	{
    	g_UsbDeviceHidBitmapEndpoints[0].maxPacketSize = HS_HID_Bitmap_INTERRUPT_IN_PACKET_SIZE;
	}
	else
	{
		g_UsbDeviceHidBitmapEndpoints[0].maxPacketSize = FS_HID_Bitmap_INTERRUPT_IN_PACKET_SIZE;
	}
    if (USB_SPEED_HIGH == speed)
    {
        g_UsbDeviceHidCustomEndpoints[0].maxPacketSize = HS_HID_Custom_INTERRUPT_IN_PACKET_SIZE;
        g_UsbDeviceHidCustomEndpoints[1].maxPacketSize = HS_HID_Custom_INTERRUPT_OUT_PACKET_SIZE;
    }
    else
    {
        g_UsbDeviceHidCustomEndpoints[0].maxPacketSize = FS_HID_Custom_INTERRUPT_IN_PACKET_SIZE;
        g_UsbDeviceHidCustomEndpoints[1].maxPacketSize = FS_HID_Custom_INTERRUPT_OUT_PACKET_SIZE;
    }
    return kStatus_USB_Success;
}
