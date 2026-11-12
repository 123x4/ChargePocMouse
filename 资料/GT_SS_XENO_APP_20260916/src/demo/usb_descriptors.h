/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

#include <stdint.h>

#define EPNUM_HID   		0x81

#define EPNUM_IN_VENDOR 	0x82  //wr

#define EPNUM_OUT_VENDOR 	0x02  //wr

#define USB_VID                    0x1038
#define USB_PID_XENO_96            0x1654
#define USB_PID_XENO_TKL           0x1656
#define USB_PID_XENO_65            0x1658
#define USB_PID_JARVIS_FULL_SIZED  0x165A
#define USB_PID_JARVIS_TKL         0x165C
#define USB_PID_JARVIS_65          0x165E

#define USB_BCD   	  		0x0200
#define KB_VERSION	  		0x0001

#define FW_VERSION	  		0x00000014
#define FW_VERSION_INNER	0x00000010

enum
{
  REPORT_ID_KEYBOARD = 1,
  REPORT_ID_MOUSE,
  REPORT_ID_STYLUS_PEN,
  REPORT_ID_CONSUMER_CONTROL,
  REPORT_ID_GAMEPAD,
  REPORT_ID_COUNT
};

void usb_descriptors_select_keyboard(uint8_t product_series, uint8_t keyboard_type);

#endif /* USB_DESCRIPTORS_H_ */
