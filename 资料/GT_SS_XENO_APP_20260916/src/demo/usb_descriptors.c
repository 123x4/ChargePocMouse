
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
 *
 */

#include "tusb.h"
#include "usb_descriptors.h"
#include "app_config.h"
#include "h6_hw_tab.h"

/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
//#define _PID_MAP(itf, n)  ( (CFG_TUD_##itf) << (n) )
//#define USB_PID           (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
//                           _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4) )
//#define USB_VID   0xCafe
//#define USB_BCD   0x0200

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
typedef struct
{
  uint8_t product_series;
  uint8_t keyboard_type;
  uint16_t product_id;
  char const *manufacturer;
  char const *product_name;
} usb_keyboard_identity_t;

static usb_keyboard_identity_t const usb_keyboard_identities[] =
{
  { PRODUCT_XENO,   KB_TYPE_96_PERCENT, USB_PID_XENO_96,           "SteelSeries", "Xeno 96" },
  { PRODUCT_XENO,   KB_TYPE_TKL,        USB_PID_XENO_TKL,          "SteelSeries", "Xeno TKL" },
  { PRODUCT_XENO,   KB_TYPE_65_PERCENT, USB_PID_XENO_65,           "SteelSeries", "Xeno 65" },
  { PRODUCT_JARVIS, KB_TYPE_FULL_SIZE,  USB_PID_JARVIS_FULL_SIZED, "SteelSeries", "Jarvis Full Sized" },
  { PRODUCT_JARVIS, KB_TYPE_TKL,        USB_PID_JARVIS_TKL,        "SteelSeries", "Jarvis TKL" },
  { PRODUCT_JARVIS, KB_TYPE_65_PERCENT, USB_PID_JARVIS_65,         "SteelSeries", "Jarvis 65" },
};

static usb_keyboard_identity_t const *selected_usb_keyboard_identity =
  &usb_keyboard_identities[0];

static tusb_desc_device_t desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = USB_BCD,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID_XENO_96,
    .bcdDevice          = KB_VERSION,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

void usb_descriptors_select_keyboard(uint8_t product_series, uint8_t keyboard_type)
{
  size_t i;

  selected_usb_keyboard_identity = &usb_keyboard_identities[0];
  for (i = 0; i < sizeof(usb_keyboard_identities) / sizeof(usb_keyboard_identities[0]); i++)
  {
    if ((usb_keyboard_identities[i].product_series == product_series) &&
        (usb_keyboard_identities[i].keyboard_type == keyboard_type))
    {
      selected_usb_keyboard_identity = &usb_keyboard_identities[i];
      break;
    }
  }

  desc_device.idVendor = USB_VID;
  desc_device.idProduct = selected_usb_keyboard_identity->product_id;
}

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
  return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

/* NKRO keyboard report descriptor: 144-bit bitmap (18 bytes key data) */
#define TUD_HID_REPORT_DESC_KEYBOARD_NKRO(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                    ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD )                    ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                    ,\
    __VA_ARGS__ \
    /* 8 bits Modifier Keys (Shift, Control, Alt) */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
      HID_USAGE_MIN    ( 224                                    )  ,\
      HID_USAGE_MAX    ( 231                                    )  ,\
      HID_LOGICAL_MIN  ( 0                                      )  ,\
      HID_LOGICAL_MAX  ( 1                                      )  ,\
      HID_REPORT_COUNT ( 8                                      )  ,\
      HID_REPORT_SIZE  ( 1                                      )  ,\
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,\
      /* 8 bit reserved */ \
      HID_REPORT_COUNT ( 1                                      )  ,\
      HID_REPORT_SIZE  ( 8                                      )  ,\
      HID_INPUT        ( HID_CONSTANT                           )  ,\
    /* Output 5-bit LED Indicator Kana | Compose | ScrollLock | CapsLock | NumLock */ \
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_LED                   )       ,\
      HID_USAGE_MIN    ( 1                                       ) ,\
      HID_USAGE_MAX    ( 5                                       ) ,\
      HID_REPORT_COUNT ( 5                                       ) ,\
      HID_REPORT_SIZE  ( 1                                       ) ,\
      HID_OUTPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ) ,\
      /* led padding */ \
      HID_REPORT_COUNT ( 1                                       ) ,\
      HID_REPORT_SIZE  ( 3                                       ) ,\
      HID_OUTPUT       ( HID_CONSTANT                            ) ,\
    /* 144-bit key bitmap (NKRO) */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                     ,\
      HID_USAGE_MIN    ( 0                                   )     ,\
      HID_USAGE_MAX_N  ( 143, 2                              )     ,\
      HID_LOGICAL_MIN  ( 0                                   )     ,\
	  HID_LOGICAL_MAX_N( 1,2                                 )     ,\
      HID_REPORT_COUNT ( 144                                 )     ,\
      HID_REPORT_SIZE  ( 1                                   )     ,\
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,\
  HID_COLLECTION_END \

#if NKRO_ENABLE
uint8_t const desc_hid_report[] =
{
	TUD_HID_REPORT_DESC_KEYBOARD_NKRO()
};
#else
uint8_t const desc_hid_report[] =
{
	TUD_HID_REPORT_DESC_KEYBOARD()
};
#endif

#define VENDOR_REPORT_DESCRIPTOR(_report_length) \
	0x06, 0x00, 0xff, \
	0x09, 0x00, \
	0xa1, 0x01, \
	0x15, 0x00, \
	0x26, 0x94, 0x00, \
	0x19, 0x00, \
	0x2a, 0x94, 0x00, \
	0x75, 0x08, \
	0x96, U16_TO_U8S_LE(_report_length), \
	0x09, 0x00, \
	0x81, 0x02, \
	0x09, 0x00, \
	0x91, 0x02, \
	0xc0

uint8_t const desc_vendor_report_hs[] =
{
	VENDOR_REPORT_DESCRIPTOR(MFG_USB_VENDOR_REPORT_LENGTH)
};

uint8_t const desc_vendor_report_fs[] =
{
	VENDOR_REPORT_DESCRIPTOR(MFG_USB_VENDOR_REPORT_LENGTH_FS)
};
// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance)
{
  //(void) instance;
	if (instance == 0)
	{
		return desc_hid_report;
	}
	else if(instance == 1)
	{
#if TUD_OPT_HIGH_SPEED
		return (tud_speed_get() == TUSB_SPEED_HIGH) ? desc_vendor_report_hs : desc_vendor_report_fs;
#else
		return desc_vendor_report_fs;
#endif
	}
	return NULL;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum
{
  ITF_NUM_HID=0,
  ITF_NUM_VENDOR=1, //
  ITF_NUM_TOTAL=2
};

#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_INOUT_DESC_LEN)//in out

//#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_DESC_LEN)//only support in

#define CONFIGURATION_DESCRIPTOR(_vendor_report_length) \
	TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 490), \
	TUD_HID_DESCRIPTOR(ITF_NUM_HID, 4, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), EPNUM_HID, KB_REPORT_SIZE, 1), \
	TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_VENDOR, 5, HID_ITF_PROTOCOL_NONE, sizeof(desc_vendor_report_hs), EPNUM_OUT_VENDOR, EPNUM_IN_VENDOR, _vendor_report_length, 1)

uint8_t const desc_configuration_hs[] =
{
	CONFIGURATION_DESCRIPTOR(MFG_USB_VENDOR_REPORT_LENGTH)
};

uint8_t const desc_configuration_fs[] =
{
	CONFIGURATION_DESCRIPTOR(MFG_USB_VENDOR_REPORT_LENGTH_FS)
};

#if TUD_OPT_HIGH_SPEED
// Per USB specs: high speed capable device must report device_qualifier and other_speed_configuration

// other speed configuration
uint8_t desc_other_speed_config[CONFIG_TOTAL_LEN];

// device qualifier is mostly similar to device descriptor since we don't change configuration based on speed
tusb_desc_device_qualifier_t const desc_device_qualifier =
{
  .bLength            = sizeof(tusb_desc_device_qualifier_t),
  .bDescriptorType    = TUSB_DESC_DEVICE_QUALIFIER,			//0x06
  .bcdUSB             = USB_BCD,							//0x0200

  .bDeviceClass       = 0x00,
  .bDeviceSubClass    = 0x00,
  .bDeviceProtocol    = 0x00,

  .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,				//64
  .bNumConfigurations = 0x01,
  .bReserved          = 0x00
};

// Invoked when received GET DEVICE QUALIFIER DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete.
// device_qualifier descriptor describes information about a high-speed capable device that would
// change if the device were operating at the other speed. If not highspeed capable stall this request.
uint8_t const* tud_descriptor_device_qualifier_cb(void)
{
  return (uint8_t const*) &desc_device_qualifier;
}

// Invoked when received GET OTHER SEED CONFIGURATION DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
// Configuration descriptor in the other speed e.g if high speed then this is for full speed and vice versa
uint8_t const* tud_descriptor_other_speed_configuration_cb(uint8_t index)
{
  (void) index; // for multiple configurations

  // Other-speed configuration must advertise the endpoint limits of the
  // opposite bus speed rather than reusing the active-speed descriptor.
  if (tud_speed_get() == TUSB_SPEED_HIGH)
  {
    memcpy(desc_other_speed_config, desc_configuration_fs, CONFIG_TOTAL_LEN);
  }
  else
  {
    memcpy(desc_other_speed_config, desc_configuration_hs, CONFIG_TOTAL_LEN);
  }
  desc_other_speed_config[1] = TUSB_DESC_OTHER_SPEED_CONFIG;

  return desc_other_speed_config;
}

#endif // highspeed

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  (void) index; // for multiple configurations

#if TUD_OPT_HIGH_SPEED
  return (tud_speed_get() == TUSB_SPEED_HIGH) ? desc_configuration_hs : desc_configuration_fs;
#else
  return desc_configuration_fs;
#endif
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// String Descriptor Index
enum {
  STRID_LANGID = 0,
  STRID_MANUFACTURER,
  STRID_PRODUCT,
  STRID_SERIAL,
};


char const *string_desc_arr[] =
{
  (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
  "SteelSeries",         		 // 1: Manufacturer
  "Xeno 96",                     // 2: Product
  NULL,                          // 3: Serials will use unique ID if possible
  "Keyboard Interface",          // 4: Interface 1 String
  "Vendor Interface",            // 5: Interface 2 String
};

//char const *string_desc_arr[] =
//{
//  (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
//  "test1",         		 // 1: Manufacturer
//  "test2",   // 2: Product
//  NULL,                          // 3: Serials will use unique ID if possible
//  "Keyboard Interface",          // 4: Interface 1 String
//  "Vendor Interface",            // 5: Interface 2 String
//};


// Get board unique ID for USB serial number. Return number of bytes. Note max_len is typically 16
//TU_ATTR_WEAK size_t board_get_unique_id(uint8_t id[], size_t max_len);


// Get USB Serial number string from unique ID if available. Return number of character.
// Input is string descriptor from index 1 (index 0 is type + len)
static inline size_t board_usb_get_serial(uint16_t desc_str1[], size_t max_chars) {
  uint8_t uid[16] TU_ATTR_ALIGNED(4);
  size_t uid_len;

  // TODO work with make, but not working with esp32s3 cmake
//  if ( board_get_unique_id ) {
//    uid_len = board_get_unique_id(uid, sizeof(uid));
//  }
//  else
  {
    // fixed serial string is 01234567889ABCDEF
    uint32_t* uid32 = (uint32_t*) (uintptr_t) uid;
    uid32[0] = 0x79336D32;
    uid32[1] = 0x00003432;
	uid_len  = 6;

  }

  if ( uid_len > max_chars / 2 ) uid_len = max_chars / 2;

  for ( size_t i = 0; i < uid_len; i++ ) {
    for ( size_t j = 0; j < 2; j++ ) {
      const char nibble_to_hex[16] = {
          '0', '1', '2', '3', '4', '5', '6', '7',
          '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
      };
      uint8_t const nibble = (uid[i] >> (j * 4)) & 0xf;
      desc_str1[i * 2 + (1 - j)] = nibble_to_hex[nibble]; // UTF-16-LE
    }
  }

  return 2 * uid_len;
}


static uint16_t _desc_str[32 + 1];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void) langid;
  size_t chr_count;

  switch ( index ) {
    case STRID_LANGID:
      memcpy(&_desc_str[1], string_desc_arr[0], 2);
      chr_count = 1;
      break;

    case STRID_SERIAL:
      chr_count = board_usb_get_serial(_desc_str + 1, 32);
      break;

    default:
    {
      // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
      // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

      if ( !(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) ) return NULL;

      const char *str = string_desc_arr[index];

      if (index == STRID_MANUFACTURER)
        str = selected_usb_keyboard_identity->manufacturer;
      else if (index == STRID_PRODUCT)
        str = selected_usb_keyboard_identity->product_name;

      // Cap at max char
      chr_count = strlen(str);
      size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1; // -1 for string type
      if ( chr_count > max_count ) chr_count = max_count;

      // Convert ASCII string into UTF-16
      for ( size_t i = 0; i < chr_count; i++ ) {
        _desc_str[1 + i] = str[i];
      }
      break;
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

  return _desc_str;
}
