#/*
 * keyboard.h
 *
 *  Created on:
 *      Author:
 */

#ifndef SRC_DEMO_KEYBOARD_H_
#define SRC_DEMO_KEYBOARD_H_

#include "app_config.h"
#include "h6_hw_tab.h"

#define TRANSFER_SIZE				21U
#define TRANSFER_GROUP				7U

#define RT_DEFAULT_VALUE			3U
#define RT_PROTO_TO_INTERNAL(v)		((unsigned short)(v) * 1U)

typedef struct
{
	uint16_t Segment_SET_Trigger_Press[TRANSFER_GROUP][TRANSFER_SIZE];
	uint16_t Segment_SET_Trigger_Break[TRANSFER_GROUP][TRANSFER_SIZE];
} DrivetData_PBPoint;

extern DrivetData_PBPoint g_DrivetData_PBPoint;

extern unsigned short top_deadzone;
extern unsigned short bottom_deadzone;
extern unsigned short actual_point;

extern unsigned char spi_key_status[HASP_KEY_NUM];
extern unsigned short current_node_update[HASP_KEY_NUM];
extern unsigned short hall_rawdata_dir[HASP_KEY_NUM];

extern uint8_t per_key_rt_press[HASP_KEY_NUM];
extern uint8_t per_key_rt_release[HASP_KEY_NUM];

void keyboard_config_init(void);
void RT_data_init(void);
unsigned char ana_hal_cali_data_handle(unsigned short segment, unsigned char index);
unsigned int check_hall_sleep_update(uint16_t th);
unsigned int check_kb_report_update(void);
void keyboard_suppress_report_until_release(void);
extern void Use_RXLNDATA_chain_quick(void);
#endif /* SRC_DEMO_KEYBOARD_H_ */
