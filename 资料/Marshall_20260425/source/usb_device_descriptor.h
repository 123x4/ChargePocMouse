/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_DESCRIPTOR_H__
#define __USB_DEVICE_DESCRIPTOR_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define USB_DEVICE_SPECIFIC_BCD_VERSION (0x0200U)
#define USB_DEVICE_DEMO_BCD_VERSION (0x0006U)//FW version

#if 1
#define USB_DEVICE_VID (0x1A81U)
#define USB_DEVICE_PID (0x2083U)
#endif
#if 0
#define USB_DEVICE_VID (0x373BU)//(0x1A81U)//(0x1FC9U)
#define USB_DEVICE_PID (0x1034U)//(0x2083U)//(0x00A0U)
#endif
#define USB_DEVICE_CLASS (0x00U)
#define USB_DEVICE_SUBCLASS (0x00U)
#define USB_DEVICE_PROTOCOL (0x00U)

#define USB_DEVICE_MAX_POWER (0xFAU)//(0x32U)

#define USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL (sizeof(g_UsbDeviceConfigurationDescriptor))
#define USB_DESCRIPTOR_LENGTH_HID_MOUSE_REPORT (sizeof(g_UsbDeviceHidMouseReportDescriptor))
#define USB_DESCRIPTOR_LENGTH_HID_KEYBOARD_REPORT (sizeof(g_UsbDeviceHidKeyboardReportDescriptor))
#define USB_DESCRIPTOR_LENGTH_HID_Bitmap_REPORT (sizeof(g_UsbDeviceHidBitmapReportDescriptor))
#define USB_DESCRIPTOR_LENGTH_HID_Custom_REPORT (sizeof(g_UsbDeviceHidCustomReportDescriptor))
#define USB_DESCRIPTOR_LENGTH_HID (9U)
#define USB_DESCRIPTOR_LENGTH_STRING0 (sizeof(g_UsbDeviceString0))
#define USB_DESCRIPTOR_LENGTH_STRING1 (sizeof(g_UsbDeviceString1))
#define USB_DESCRIPTOR_LENGTH_STRING2 (sizeof(g_UsbDeviceString2))
#define USB_DESCRIPTOR_LENGTH_STRING3 (sizeof(g_UsbDeviceString3))
#define USB_DESCRIPTOR_LENGTH_STRING4 (sizeof(g_UsbDeviceString4))

#define USB_DEVICE_CONFIGURATION_COUNT (1U)
#define USB_DEVICE_STRING_COUNT (5U)
#define USB_DEVICE_LANGUAGE_COUNT (1U)

#define USB_COMPOSITE_CONFIGURE_INDEX (1U)


//#define USB_HID_GENERIC_ENDPOINT_OUT      (2U)




#define USB_HID_MOUSE_INTERFACE_COUNT (1U)
#define USB_HID_MOUSE_INTERFACE_INDEX (0U)
#define USB_HID_MOUSE_INTERFACE_ALTERNATE_COUNT (1U)
#define USB_HID_MOUSE_INTERFACE_ALTERNATE_0 (0U)
#define USB_HID_MOUSE_IN_BUFFER_LENGTH (8U)
#define USB_HID_MOUSE_ENDPOINT_COUNT (1U)
#define USB_HID_MOUSE_ENDPOINT_IN (1U)

#define USB_HID_MOUSE_REPORT_LENGTH (0x04U)

#define USB_HID_MOUSE_CLASS (0x03U)
#define USB_HID_MOUSE_SUBCLASS (0x01U)
#define USB_HID_MOUSE_PROTOCOL (0x02U)

#define HS_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE (8U)
#define FS_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE (8U)
#define HS_HID_MOUSE_INTERRUPT_IN_INTERVAL (0x01U)//(0x06U) /* 2^(6-1) = 4ms */
#define FS_HID_MOUSE_INTERRUPT_IN_INTERVAL (0x04U)

#define USB_HID_KEYBOARD_CLASS (0x03U)
#define USB_HID_KEYBOARD_SUBCLASS (0x01U)//(0x01U)
#define USB_HID_KEYBOARD_PROTOCOL (0x01U)//(0x01U)
#define USB_HID_KEYBOARD_INTERFACE_COUNT (1U)
#define USB_HID_KEYBOARD_INTERFACE_INDEX (1U)
#define USB_HID_KEYBOARD_INTERFACE_ALTERNATE_COUNT (1U)
#define USB_HID_KEYBOARD_INTERFACE_ALTERNATE_0 (0U)
#define USB_HID_KEYBOARD_IN_BUFFER_LENGTH (8U)
#define USB_HID_KEYBOARD_ENDPOINT_COUNT (1U)
#define USB_HID_KEYBOARD_ENDPOINT_IN (2U)
#define USB_HID_GENERIC_ENDPOINT_OUT      (3U)
#define HS_HID_KEYBOARD_INTERRUPT_IN_PACKET_SIZE (64U)//(8U)
#define FS_HID_KEYBOARD_INTERRUPT_IN_PACKET_SIZE (64U)//(8U)
#define HS_HID_KEYBOARD_INTERRUPT_IN_INTERVAL (0x01U)//(0x06U) /* 2^(6-1) = 4ms */
#define FS_HID_KEYBOARD_INTERRUPT_IN_INTERVAL (0x01U)//(0x04U)

#define HS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE (64U)//(8U)
#define FS_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE (64U)//(8U)
#define HS_HID_GENERIC_INTERRUPT_OUT_INTERVAL    (0x01U) /* 2^(4-1) = 1ms */
#define FS_HID_GENERIC_INTERRUPT_OUT_INTERVAL    (0x01U)
#define USB_HID_KEYBOARD_REPORT_LENGTH	 		(64U)//(64U)//(0x09U)//(0x08U)
#define USB_HID_GENERIC_REPORT_IN_LENGTH (64U)
#define USB_HID_Consumer_REPORT_IN_LENGTH (8U)

#if 1
#define USB_HID_Custom_INTERFACE_COUNT (1U)
#define USB_HID_Custom_INTERFACE_INDEX (2U)
#define USB_HID_Custom_INTERFACE_ALTERNATE_COUNT (1U)
#define USB_HID_Custom_INTERFACE_ALTERNATE_0 (0U)
#define USB_HID_Custom_IN_BUFFER_LENGTH (8U)
#define USB_HID_Custom_ENDPOINT_COUNT (2U)
#define USB_HID_Custom_ENDPOINT_OUT (3U)
#define USB_HID_Custom_ENDPOINT_IN (4U)
#define USB_HID_Custom_REPORT_LENGTH (0x04U)
#define USB_HID_Custom_CLASS (0x03U)
#define USB_HID_Custom_SUBCLASS (0x00U)
#define USB_HID_Custom_PROTOCOL (0x00U)
#define HS_HID_Custom_INTERRUPT_IN_PACKET_SIZE (64U)
#define FS_HID_Custom_INTERRUPT_IN_PACKET_SIZE (64U)
#define HS_HID_Custom_INTERRUPT_IN_INTERVAL (0x01U) /* 2^(6-1) = 4ms */
#define FS_HID_Custom_INTERRUPT_IN_INTERVAL (0x01U)
#define HS_HID_Custom_INTERRUPT_OUT_PACKET_SIZE (64U)//(8U)
#define FS_HID_Custom_INTERRUPT_OUT_PACKET_SIZE (64U)//(8U)
#define HS_HID_Custom_INTERRUPT_OUT_INTERVAL    (0x01U) /* 2^(4-1) = 1ms */
#define FS_HID_Custom_INTERRUPT_OUT_INTERVAL    (0x01U)
#define USB_HID_Custom_REPORT_IN_LENGTH (64U)
#define USB_HID_Custom_REPORT_OUT_LENGTH (64U)
#endif

#if 1
#define USB_HID_Bitmap_INTERFACE_COUNT (1U)
#define USB_HID_Bitmap_INTERFACE_INDEX (3U)
#define USB_HID_Bitmap_INTERFACE_ALTERNATE_COUNT (1U)
#define USB_HID_Bitmap_INTERFACE_ALTERNATE_0 (0U)
#define USB_HID_Bitmap_ENDPOINT_COUNT (1U)
#define USB_HID_Bitmap_ENDPOINT_IN (5U)
#define USB_HID_Bitmap_CLASS (0x03U)
#define USB_HID_Bitmap_SUBCLASS (0x01U)
#define USB_HID_Bitmap_PROTOCOL (0x01U)
#define HS_HID_Bitmap_INTERRUPT_IN_PACKET_SIZE (64U)
#define FS_HID_Bitmap_INTERRUPT_IN_PACKET_SIZE (64U)
#define HS_HID_Bitmap_INTERRUPT_IN_INTERVAL (0x01U) /* 2^(6-1) = 4ms */
#define FS_HID_Bitmap_INTERRUPT_IN_INTERVAL (0x01U)
#define USB_HID_Bitmap_REPORT_IN_LENGTH (64U)
#endif

#define USB_HID_SETREPORT_BUFFER_LENGTH (64U)
#define USB_COMPOSITE_INTERFACE_COUNT (USB_HID_KEYBOARD_INTERFACE_COUNT + USB_HID_MOUSE_INTERFACE_COUNT + USB_HID_Custom_INTERFACE_COUNT + USB_HID_Bitmap_INTERFACE_COUNT)
/*******************************************************************************
 * API
 ******************************************************************************/

/* Configure the device according to the USB speed. */
extern usb_status_t USB_DeviceSetSpeed(usb_device_handle handle, uint8_t speed);
#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
/* Get device qualifier descriptor request */
usb_status_t USB_DeviceGetDeviceQualifierDescriptor(
    usb_device_handle handle, usb_device_get_device_qualifier_descriptor_struct_t *deviceQualifierDescriptor);
#endif
/* Get device descriptor request */
usb_status_t USB_DeviceGetDeviceDescriptor(usb_device_handle handle,
                                           usb_device_get_device_descriptor_struct_t *deviceDescriptor);

/* Get device configuration descriptor request */
usb_status_t USB_DeviceGetConfigurationDescriptor(
    usb_device_handle handle, usb_device_get_configuration_descriptor_struct_t *configurationDescriptor);

/* Get device string descriptor request */
usb_status_t USB_DeviceGetStringDescriptor(usb_device_handle handle,
                                           usb_device_get_string_descriptor_struct_t *stringDescriptor);

/* Get hid descriptor request */
usb_status_t USB_DeviceGetHidDescriptor(usb_device_handle handle,
                                        usb_device_get_hid_descriptor_struct_t *hidDescriptor);

/* Get hid report descriptor request */
usb_status_t USB_DeviceGetHidReportDescriptor(usb_device_handle handle,
                                              usb_device_get_hid_report_descriptor_struct_t *hidReportDescriptor);

/* Get hid physical descriptor request */
usb_status_t USB_DeviceGetHidPhysicalDescriptor(usb_device_handle handle,
                                                usb_device_get_hid_physical_descriptor_struct_t *hidPhysicalDescriptor);

#endif /* __USB_DEVICE_DESCRIPTOR_H__ */
