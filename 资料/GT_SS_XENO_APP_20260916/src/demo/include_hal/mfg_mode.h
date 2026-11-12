/*
 * mfg_mode.h
 *
 *  Created on:
 *      Author:
 */
#include "tusb_config.h"
#include "hasp_drv.h"
#include "h6_hw_tab.h"
#include "led_rgb.h"
#include "gtry_qspi_flash_drv.h"

#ifndef SRC_DEMO_MFG_MODE_MFG_MODE_H_
#define SRC_DEMO_MFG_MODE_MFG_MODE_H_

/*--------------------------------------------------------------------+
 * Constants
 *--------------------------------------------------------------------*/
#define RAW_DATA_DEBUG						0

#define H6_0X33_BIT2_BASELINE_MODE_ENABLE	1
#define H6_0X33_BIT2_BASELINE_MODE_DISABLE	0

//external flash addr
#define REGIOON_ID_ADDR					0x0
#define GAIN_VALUE_ADDR					0x1000
#define LED_BIN_WB_ADDR					0x2000
#define SN_ADDR							0x3000
#define RT_ADDR							0x4000
#define TBL_CALI_ADDR					0x5000
#define BBL_CALI_ADDR					0x6000
#define IMBALANCE_CALI_ADDR				0x7000 // Imbalance calibration

#define	POWER_ON_W_GAIN					0x88
#define	HALLPLATE_SENSITIVE				0x08

#define SN_LEN							20

#define BYTE0_HEADER04					0x04

//Data Type
/*--------------------------1Byte--------------------------------------*/
#define Seg255				        	0x01
#define GAIN_VALUE				    	0x05
#define SELF_CHECK				    	0x0C

/*--------------------------2Byte--------------------------------------*/
#define HALL_DATA			        	0x00
#define Seg4096				        	0x03
#define TBL				        		0x0D// Top    Baseline
#define BBL				       			0x0E// Bottom Baseline

/*--------------------------3Byte--------------------------------------*/
#define Hall_PRESS_SAMPLING_AND_CALI 	0xD1
#define Hall_RELEASE_SAMPLING_AND_CALI  0xD2
#define HALL_DATA_AND_NOISE			    0xD3
#define H6_IMBALANCE_CALI_CMD		    0xD4

#define START_HASP_BOTTOM_CALI_SAVE 	0xE1
#define START_HALL_TBL_CALI_SAVE 		0xE2

extern uint16_t mfg_usb_vendor_report_length(void);

#define VENDOR_REPORT_SIZE				mfg_usb_vendor_report_length()
#define VENDOR_HEADER_SIZE				3

#define FACTORY_CALI_DISCARD_SAMPLES    10
#define FACTORY_CALI_SAMPLE_DELAY_US    100

/* Per-key UART logging inside factory calibration is intentionally disabled by
 * default. These logs are very expensive because printf() is synchronous in
 * this firmware, so enabling them during a USB command can block the main loop
 * long enough to delay or break the USB vendor response path. */
#define FACTORY_CALI_VERBOSE_LOG_EN     0

/* RT (Rapid Trigger) protocol constants */
#define RT_DATA_OFFSET                  4       /* payload starts at Byte 4 */
#define RT_BYTES_PER_KEY                3       /* keycode + RT_Press + RT_Break */
#define PACKET_TYPE_UNIFORM_BYTE2       0x01    /* all keys same value */
#define PACKET_TYPE_UNIFORM_BYTE3       0xFF    /* all keys same value */
#define VENDOR_GAIN_HEADER_SIZE			4

#define RT_PRESS_MIN                  	1
#define RT_BREAK_MIN                  	1
#define RT_PRESS_MAX                  	40
#define RT_BREAK_MAX                  	40

#define BYTES_PER_KEY_1BYTE				2
#define BYTES_PER_KEY_2BYTE				3
#define BYTES_PER_KEY_3BYTE				4

#define KEYS_PER_PACKET_1BYTE			((VENDOR_REPORT_SIZE - VENDOR_HEADER_SIZE) / BYTES_PER_KEY_1BYTE)
#define KEYS_PER_PACKET_2BYTE			((VENDOR_REPORT_SIZE - VENDOR_HEADER_SIZE) / BYTES_PER_KEY_2BYTE)
#define KEYS_PER_PACKET_3BYTE			((VENDOR_REPORT_SIZE - VENDOR_HEADER_SIZE) / BYTES_PER_KEY_3BYTE)

#define SUCCESS							1
/*------------------------Byte 1:CMD HEADER--------------------------------------*/
typedef enum{
	BYTE1_TEST_MODE04					=	0x04,
	BYTE1_NORMAL_MODE05					=	0x05,
	BYTE1_SET_REGION_ID 				= 	0x0A,
	BYTE1_GET_REGION_ID 				= 	0x0B,
	BYTE1_GET_LANG_FW_VERSION 			= 	0x0C,
	BYTE1_CALI		 					= 	0x10,
	BYTE1_GET_HALL_PARAM_1BYTE			=	0x17,
	BYTE1_GET_HALL_PARAM_2BYTE			=	0x18,
	BYTE1_GET_HALL_PARAM_3BYTE			=	0x19,
	BYTE1_SET_GAIN_VALUE				=	0x20,
	BYTE1_SET_PWM						=	0x21,
	BYTE1_SET_LED_BIN					=	0x22,
	BYTE1_GET_LED_BIN					=	0x23,
	BYTE1_SET_LED_WB					=	0x24,//LED White Balance
	BYTE1_GET_LED_WB					=	0x25,//LED White Balance
	BYTE1_SET_SN						=	0x26,
	BYTE1_GET_SN						=	0x27,
	BYTE1_TFT_TEST						=	0x28,
	BYTE1_SET_RT						=	0x40,
	BYTE1_GET_RT						=	0x41,
	BYTE1_ENCODER_TEST					=	0x2A,//TBD
	BYTE1_RESTOR2DEFAULT 				=	0x2B,
	BYTE1_SET_RESET						=	0x2C,
}cmd_type_t;

/*--------------------------------------------------------------------+
 * Extern Variables
 *--------------------------------------------------------------------*/
extern uint8_t kb_report_done;
extern uint8_t data_report_done;

extern unsigned char vendor_buf_in[CFG_TUD_HID_EP_BUFSIZE];
extern unsigned char vendor_buf_out[CFG_TUD_HID_EP_BUFSIZE];

extern unsigned char mfg_sign[8];
extern unsigned char mfg_mode_cmd;
extern uint8_t SN[SN_LEN];

extern volatile uint8_t report_active;
extern volatile uint8_t report_cmd_type;
extern volatile uint8_t report_ready;

/*--------------------------------------------------------------------+
 * Public Function Prototypes
 *--------------------------------------------------------------------*/
extern void send_vendor(void);
extern void TestAPP_Report_HallData(uint8_t order);
extern void TestAPP_Report_SegmentData(uint8_t order);
extern void report_timer1_init(void);
extern void TestMode_Report_Data(void);
extern void mfg_mode_check_usb_vendor_cmd(void);
extern void Set_RGB(uint8_t r, uint8_t g, uint8_t b);
extern uint8_t Flash_Gain_to_Hall_init(void);
extern void hall_gain_load_final_table(uint16_t *gain_table, uint32_t size);

#endif /* SRC_DEMO_MFG_MODE_MFG_MODE_H_ */
