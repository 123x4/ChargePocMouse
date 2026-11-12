/*
 * keyboard.c
 *
 *  Created on:
 *      Author:
 */

#include <string.h>

#include "keyboard.h"
#include "gtry_platform.h"
#include "hasp_drv.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "mfg_mode.h"
#include "gtry_qspi_flash_drv.h"

DrivetData_PBPoint g_DrivetData_PBPoint;

unsigned short top_deadzone = 17;
unsigned short bottom_deadzone = 41;
unsigned short actual_point = 700;

/* Application state follows the largest supported aligned layout: 16 slots
 * across 7 active HASP channels, including empty alignment packets. */
unsigned char spi_key_status[HASP_KEY_NUM];
unsigned short current_node_update[HASP_KEY_NUM];
unsigned short hall_rawdata_dir[HASP_KEY_NUM];

uint8_t per_key_rt_press[HASP_KEY_NUM];
uint8_t per_key_rt_release[HASP_KEY_NUM];

static unsigned char usb_kb_last_report[KB_REPORT_SIZE] = {0};
static unsigned char usb_kb_report[KB_REPORT_SIZE] = {0};

static unsigned char *cur_report = usb_kb_report;
static unsigned char *last_report = usb_kb_last_report;
static unsigned char *tmp_report = usb_kb_last_report;

static int get_layout_index(keyboard_type_t kb, language_type_t lang)
{
	int lang_base;

	switch (lang)
	{
		case LANG_JP:
			lang_base = 0;
			break;
		case LANG_US:
			lang_base = 4;
			break;
		case LANG_UK:
			lang_base = 8;
			break;
		default:
			lang_base = 4;
			break;
	}

	return lang_base + (int)kb;
}

static keyboard_type_t detect_keyboard_type(void)
{
	uint8_t hwid;

	gpio_cfg_input(HWID1_PIN, RY_GPIO_PULLUP_10K);
	gpio_cfg_input(HWID2_PIN, RY_GPIO_PULLUP_10K);
	gpio_cfg_input(HWID3_PIN, RY_GPIO_PULLUP_10K);
	ry_delay_us(50);

	hwid = (uint8_t)((gpio_in_read(HWID1_PIN) << 2) |
					 (gpio_in_read(HWID2_PIN) << 1) |
					 gpio_in_read(HWID3_PIN));

	gpio_cfg_input(HWID1_PIN, RY_GPIO_NOPULL);
	gpio_cfg_input(HWID2_PIN, RY_GPIO_NOPULL);
	gpio_cfg_input(HWID3_PIN, RY_GPIO_NOPULL);

	switch (hwid)
	{
		case 7:
			g_product_series = PRODUCT_XENO;
			return KB_TYPE_96_PERCENT;
		case 5:
			g_product_series = PRODUCT_XENO;
			return KB_TYPE_TKL;
		case 6:
			g_product_series = PRODUCT_XENO;
			return KB_TYPE_65_PERCENT;
		case 3:
			g_product_series = PRODUCT_JARVIS;
			return KB_TYPE_FULL_SIZE;
		case 1:
			g_product_series = PRODUCT_JARVIS;
			return KB_TYPE_TKL;
		case 4:
			g_product_series = PRODUCT_JARVIS;
			return KB_TYPE_65_PERCENT;
		default:
			g_product_series = PRODUCT_XENO;
			return KB_TYPE_96_PERCENT;
	}
}

static language_type_t detect_language_type(void)
{
	uint8_t sel1;
	uint8_t sel2;

	gpio_cfg_input(LANG_SEL1_PIN, RY_GPIO_PULLUP_10K);
	gpio_cfg_input(LANG_SEL2_PIN, RY_GPIO_PULLUP_10K);
	ry_delay_us(50);

	sel1 = gpio_in_read(LANG_SEL1_PIN);
	sel2 = gpio_in_read(LANG_SEL2_PIN);

	gpio_cfg_input(LANG_SEL1_PIN, RY_GPIO_NOPULL);
	gpio_cfg_input(LANG_SEL2_PIN, RY_GPIO_NOPULL);

	if ((sel1 == 0U) && (sel2 == 1U))
	{
		return LANG_JP;
	}
	if ((sel1 == 1U) && (sel2 == 0U))
	{
		return LANG_UK;
	}
	if ((sel1 == 1U) && (sel2 == 1U))
	{
		return LANG_US;
	}
	return LANG_UNKNOWN;
}

static void keyboard_state_reset(void)
{
	memset(spi_key_status, 0, sizeof(spi_key_status));
	memset(current_node_update, 0, sizeof(current_node_update));
	memset(hall_rawdata_dir, 0, sizeof(hall_rawdata_dir));
	memset(per_key_rt_press, RT_DEFAULT_VALUE, sizeof(per_key_rt_press));
	memset(per_key_rt_release, RT_DEFAULT_VALUE, sizeof(per_key_rt_release));
	memset(usb_kb_last_report, 0, sizeof(usb_kb_last_report));
	memset(usb_kb_report, 0, sizeof(usb_kb_report));
}

void keyboard_config_init(void)
{
	keyboard_type_t kb   = detect_keyboard_type();
	language_type_t lang = detect_language_type();
	const layout_cfg_t *cfg = &k_layouts[get_layout_index(kb, lang)];

	g_keyboard_type = (uint8_t)kb;
	g_language_type = (uint8_t)lang;
	usb_descriptors_select_keyboard(g_product_series, g_keyboard_type);
	g_hasp_chain_length = cfg->chain_length;
	g_hasp_key_num = cfg->hasp_num;
	g_key_board_num = cfg->actual_board_num;

	g_space_r_l = cfg->r_l;
	g_space_r_r = cfg->r_r;
	g_space_g_l = cfg->g_l;
	g_space_g_r = cfg->g_r;
	g_space_b_l = cfg->b_l;
	g_space_b_r = cfg->b_r;

	memset(Key_HID_Usage_ID, 0, sizeof(Key_HID_Usage_ID));
	memcpy(Key_HID_Usage_ID, cfg->usage_id, cfg->table_size);
	memcpy(chain_len_buff, cfg->chain_len, sizeof(cfg->chain_len));

	keyboard_state_reset();
}

void RT_data_init(void)
{
	uint8_t flash_buf[HASP_KEY_NUM * 2U]; /* Keep the persisted press and release RT tables in one contiguous buffer. */
	unsigned int read_len; /* Record the Flash driver's actual read length before accepting the data. */
	uint8_t all_ff = 1U; /* Treat an all-0xFF sector as never programmed. */

	memset(per_key_rt_press, RT_DEFAULT_VALUE, sizeof(per_key_rt_press)); /* Start from safe defaults so every failure path remains usable. */
	memset(per_key_rt_release, RT_DEFAULT_VALUE, sizeof(per_key_rt_release)); /* Initialize the release table with the same fallback policy. */

	read_len = QSPI_EXT_FLASH_read(RT_ADDR, flash_buf, sizeof(flash_buf)); /* Load the complete RT payload written by RT_save_to_flash(). */
	if (read_len != sizeof(flash_buf)) /* Reject partial reads because they cannot represent a complete press/release table. */
	{
		return; /* Keep the default RT values when Flash communication fails or is incomplete. */
	}

	for (uint32_t i = 0; i < sizeof(flash_buf); i++) /* Check whether any byte has ever been programmed in the RT sector. */
	{
		if (flash_buf[i] != 0xFFU) /* A non-erased byte means Flash contains a saved RT table. */
		{
			all_ff = 0U; /* Mark the sector valid enough to restore the two saved tables. */
			break; /* No further blank-sector scan is needed after the first programmed byte. */
		}
	}

	if (all_ff != 0U) /* Do not copy erased bytes into the runtime threshold arrays. */
	{
		return; /* Preserve RT_DEFAULT_VALUE for a Flash sector that has never been saved. */
	}
//  CC 屏蔽这段代码
//	memcpy(per_key_rt_press, flash_buf, HASP_KEY_NUM); /* Restore the first half of Flash as the per-key press RT table. */
//	memcpy(per_key_rt_release, &flash_buf[HASP_KEY_NUM], HASP_KEY_NUM); /* Restore the second half as the per-key release RT table. */
}

unsigned char ana_hal_cali_data_handle(unsigned short segment, unsigned char index)
{
	unsigned short release_delta;
	unsigned short press_delta;
	unsigned short bottom_deadzone_start;
	unsigned short rt_release;
	unsigned short rt_press;

	if (Key_HID_Usage_ID[index] == NONE)
	{
		return 0;
	}

	rt_release = RT_PROTO_TO_INTERNAL(per_key_rt_release[index]);
	rt_press = RT_PROTO_TO_INTERNAL(per_key_rt_press[index]);

	if (segment <= top_deadzone)
	{
		hall_rawdata_dir[index] = 0;
		current_node_update[index] = segment;
		spi_key_status[index] = 0;
		return 0;
	}

	bottom_deadzone_start = LINE_LEN - bottom_deadzone;
	if (spi_key_status[index] && (segment >= bottom_deadzone_start))
	{
		hall_rawdata_dir[index] = 1;
		current_node_update[index] = segment;
		return 1;
	}

	if (hall_rawdata_dir[index])
	{
		if ((segment < (unsigned short)(current_node_update[index] + 3U)) &&
			(segment >= current_node_update[index]))
		{
			return spi_key_status[index];
		}
	}
	else
	{
		if ((segment <= current_node_update[index]) &&
			(segment > (unsigned short)(current_node_update[index] - 3U)))
		{
			return spi_key_status[index];
		}
	}

	if (segment > current_node_update[index])
	{
		hall_rawdata_dir[index] = 1;
	}
	else if (segment < current_node_update[index])
	{
		hall_rawdata_dir[index] = 0;
	}

	if (spi_key_status[index])
	{
		if (segment > current_node_update[index])
		{
			current_node_update[index] = segment;
		}
		else
		{
			release_delta = current_node_update[index] - segment;
			if (release_delta >= rt_release)
			{
				spi_key_status[index] = 0;
				current_node_update[index] = segment;
			}
		}
	}
	else
	{
		if (segment < current_node_update[index])
		{
			current_node_update[index] = segment;
		}
		else
		{
			press_delta = segment - current_node_update[index];
			if (press_delta >= rt_press)
			{
				spi_key_status[index] = 1;
				current_node_update[index] = segment;
			}
		}
	}

	return spi_key_status[index];
}

#if NKRO_ENABLE
#define PROCESS_KEY(n) \
	if (GTRY_HASP->RXLNDATA[n] > KRY_DOWN_VALUE) \
	{ \
		if (ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[n], (unsigned char)(n))) \
		{ \
			key_id = Key_HID_Usage_ID[n]; \
			if (key_id < 0xE0U) \
			{ \
				temp_index = (unsigned char)(key_id >> 3); \
				temp_pos = (unsigned char)(key_id & 7U); \
				cur_report[2U + temp_index] |= (unsigned char)(1U << temp_pos); \
			} \
			else \
			{ \
				temp_pos = (unsigned char)(key_id - 0xE0U); \
				cur_report[0] |= (unsigned char)(1U << temp_pos); \
			} \
		} \
	} \
	else \
	{ \
		current_node_update[n] = 0; \
	}
#else
#define PROCESS_KEY(n) \
	if ((GTRY_HASP->RXLNDATA[n] > KRY_DOWN_VALUE) && ((Key_HID_Usage_ID[n] != NONE))) \
	{ \
		\
		if (ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[n], (unsigned char)(n))) \
		{ \
			key_id = Key_HID_Usage_ID[n]; \
			if (key_id >= 0xE0U) \
			{ \
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U)); \
			} \
			else if (key_count < 6U) \
			{ \
				cur_report[2U + key_count] = key_id; \
				key_count++; \
			} \
		} \
	} \
	else \
	{ \
		current_node_update[n] = 0; \
	}
#endif

//#define PROCESS_KEY(n) \
//		if (ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[n], (unsigned char)(n))) \
//		{ \
//			key_id = Key_HID_Usage_ID[n]; \
//			if (key_id >= 0xE0U) \
//			{ \
//				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U)); \
//			} \
//			else if (key_count < 6U) \
//			{ \
//				cur_report[2U + key_count] = key_id; \
//				key_count++; \
//			} \
//		} \
//		else \
//		{ \
//			current_node_update[n] = 0; \
//		}
//#endif

#define PROCESS_CHAIN_7(base) \
	PROCESS_KEY((base) + 0) PROCESS_KEY((base) + 1) PROCESS_KEY((base) + 2) \
	PROCESS_KEY((base) + 3) PROCESS_KEY((base) + 4) PROCESS_KEY((base) + 5) \
	PROCESS_KEY((base) + 6)

static void scan_universal(void)
{
	unsigned char key_id;
#if NKRO_ENABLE
	unsigned char temp_index;
	unsigned char temp_pos;
#else
	unsigned char key_count = 0;
#endif

	PROCESS_CHAIN_7(0)   PROCESS_CHAIN_7(7)   PROCESS_CHAIN_7(14)  PROCESS_CHAIN_7(21)
	PROCESS_CHAIN_7(28)  PROCESS_CHAIN_7(35)  PROCESS_CHAIN_7(42)  PROCESS_CHAIN_7(49)
	PROCESS_CHAIN_7(56)  PROCESS_CHAIN_7(63)  PROCESS_CHAIN_7(70)  PROCESS_CHAIN_7(77)
	PROCESS_CHAIN_7(84)  PROCESS_CHAIN_7(91)  PROCESS_CHAIN_7(98)  PROCESS_CHAIN_7(105)
}

unsigned int  check_hall_sleep_update(uint16 th)
{
	uint8 key_n = 0;

	do{
		if(chain_miss_buff_reg[key_n])
		{
			if(GTRY_HASP->RXLNDATA[key_n] > th)
			{
				return 1;
			}
		}
	}while(++key_n < HASP_KEY_NUM);
	return 0;
}

unsigned int check_kb_report_update(void)
{
	unsigned int i;

	memset(cur_report, 0, KB_REPORT_SIZE);

//	GTRY6xx_SHOW_DBG_H_0;
	scan_universal();
	//Use_RXLNDATA_chain_quick();
//	GTRY6xx_SHOW_DBG_H_0;

#if USB_ENABLE

	for (i = 0; i < KB_REPORT_SIZE; i++)
	{
		if (cur_report[i] != last_report[i])
		{
			if (dcd_edpt_fast_data_in(0, 0x81, cur_report, KB_REPORT_SIZE))
			{
				tmp_report = last_report;
				last_report = cur_report;
				cur_report = tmp_report;
			}
			break;
		}
	}

#endif

	return 0;
}

void Use_RXLNDATA_chain_quick(void)
{

	unsigned char key_id;
	unsigned char key_count = 0;
	/* 0 */
	if((GTRY_HASP->RXLNDATA[0]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[0], 0))
		{
			key_id = Key_HID_Usage_ID[0];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[0] = 0;
	}

	/* 1 */
	if((GTRY_HASP->RXLNDATA[1]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[1], 1))
		{
			key_id = Key_HID_Usage_ID[1];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[1] = 0;
	}

	/* 2 */
	if((GTRY_HASP->RXLNDATA[2]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[2], 2))
		{
			key_id = Key_HID_Usage_ID[2];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[2] = 0;
	}

	/* 3 */
	if((GTRY_HASP->RXLNDATA[3]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[3], 3))
		{
			key_id = Key_HID_Usage_ID[3];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[3] = 0;
	}

	/* 4 */
	if((GTRY_HASP->RXLNDATA[4]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[4], 4))
		{
			key_id = Key_HID_Usage_ID[4];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[4] = 0;
	}

	/* 5 */
	if((GTRY_HASP->RXLNDATA[5]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[5], 5))
		{
			key_id = Key_HID_Usage_ID[5];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[5] = 0;
	}

	/* 6 */
	if((GTRY_HASP->RXLNDATA[6]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[6], 6))
		{
			key_id = Key_HID_Usage_ID[6];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[6] = 0;
	}

	/* 7 */
	if((GTRY_HASP->RXLNDATA[7]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[7], 7))
		{
			key_id = Key_HID_Usage_ID[7];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[7] = 0;
	}

	/* 8 */
	if((GTRY_HASP->RXLNDATA[8]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[8], 8))
		{
			key_id = Key_HID_Usage_ID[8];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[8] = 0;
	}

	/* 9 */
	if((GTRY_HASP->RXLNDATA[9]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[9], 9))
		{
			key_id = Key_HID_Usage_ID[9];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[9] = 0;
	}

	/* 10 */
	if((GTRY_HASP->RXLNDATA[10]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[10], 10))
		{
			key_id = Key_HID_Usage_ID[10];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[10] = 0;
	}

	/* 11 */
	if((GTRY_HASP->RXLNDATA[11]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[11], 11))
		{
			key_id = Key_HID_Usage_ID[11];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[11] = 0;
	}

	/* 12 */
	if((GTRY_HASP->RXLNDATA[12]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[12], 12))
		{
			key_id = Key_HID_Usage_ID[12];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[12] = 0;
	}

	/* 13 */
	if((GTRY_HASP->RXLNDATA[13]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[13], 13))
		{
			key_id = Key_HID_Usage_ID[13];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[13] = 0;
	}

	/* 14 */
	if((GTRY_HASP->RXLNDATA[14]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[14], 14))
		{
			key_id = Key_HID_Usage_ID[14];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[14] = 0;
	}

	/* 15 */
	if((GTRY_HASP->RXLNDATA[15]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[15], 15))
		{
			key_id = Key_HID_Usage_ID[15];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[15] = 0;
	}

	/* 16 */
	if((GTRY_HASP->RXLNDATA[16]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[16], 16))
		{
			key_id = Key_HID_Usage_ID[16];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[16] = 0;
	}

	/* 17 */
	if((GTRY_HASP->RXLNDATA[17]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[17], 17))
		{
			key_id = Key_HID_Usage_ID[17];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[17] = 0;
	}

	/* 18 */
	if((GTRY_HASP->RXLNDATA[18]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[18], 18))
		{
			key_id = Key_HID_Usage_ID[18];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[18] = 0;
	}

	/* 19 */
	if((GTRY_HASP->RXLNDATA[19]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[19], 19))
		{
			key_id = Key_HID_Usage_ID[19];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[19] = 0;
	}

	/* 20 */
	if((GTRY_HASP->RXLNDATA[20]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[20], 20))
		{
			key_id = Key_HID_Usage_ID[20];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[20] = 0;
	}

	/* 21 */
	if((GTRY_HASP->RXLNDATA[21]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[21], 21))
		{
			key_id = Key_HID_Usage_ID[21];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[21] = 0;
	}

	/* 22 */
	if((GTRY_HASP->RXLNDATA[22]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[22], 22))
		{
			key_id = Key_HID_Usage_ID[22];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[22] = 0;
	}

	/* 23 */
	if((GTRY_HASP->RXLNDATA[23]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[23], 23))
		{
			key_id = Key_HID_Usage_ID[23];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[23] = 0;
	}

	/* 24 */
	if((GTRY_HASP->RXLNDATA[24]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[24], 24))
		{
			key_id = Key_HID_Usage_ID[24];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[24] = 0;
	}

	/* 25 */
	if((GTRY_HASP->RXLNDATA[25]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[25], 25))
		{
			key_id = Key_HID_Usage_ID[25];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[25] = 0;
	}

	/* 26 */
	if((GTRY_HASP->RXLNDATA[26]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[26], 26))
		{
			key_id = Key_HID_Usage_ID[26];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[26] = 0;
	}

	/* 27 */
	if((GTRY_HASP->RXLNDATA[27]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[27], 27))
		{
			key_id = Key_HID_Usage_ID[27];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[27] = 0;
	}

	/* 28 */
	if((GTRY_HASP->RXLNDATA[28]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[28], 28))
		{
			key_id = Key_HID_Usage_ID[28];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[28] = 0;
	}

	/* 29 */
	if((GTRY_HASP->RXLNDATA[29]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[29], 29))
		{
			key_id = Key_HID_Usage_ID[29];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[29] = 0;
	}

	/* 30 */
	if((GTRY_HASP->RXLNDATA[30]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[30], 30))
		{
			key_id = Key_HID_Usage_ID[30];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[30] = 0;
	}

	/* 31 */
	if((GTRY_HASP->RXLNDATA[31]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[31], 31))
		{
			key_id = Key_HID_Usage_ID[31];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[31] = 0;
	}

	/* 32 */
	if((GTRY_HASP->RXLNDATA[32]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[32], 32))
		{
			key_id = Key_HID_Usage_ID[32];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[32] = 0;
	}

	/* 33 */
	if((GTRY_HASP->RXLNDATA[33]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[33], 33))
		{
			key_id = Key_HID_Usage_ID[33];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[33] = 0;
	}

	/* 34 */
	if((GTRY_HASP->RXLNDATA[34]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[34], 34))
		{
			key_id = Key_HID_Usage_ID[34];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[34] = 0;
	}

	/* 35 */
	if((GTRY_HASP->RXLNDATA[35]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[35], 35))
		{
			key_id = Key_HID_Usage_ID[35];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[35] = 0;
	}

	/* 36 */
	if((GTRY_HASP->RXLNDATA[36]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[36], 36))
		{
			key_id = Key_HID_Usage_ID[36];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[36] = 0;
	}

	/* 37 */
	if((GTRY_HASP->RXLNDATA[37]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[37], 37))
		{
			key_id = Key_HID_Usage_ID[37];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[37] = 0;
	}

	/* 38 */
	if((GTRY_HASP->RXLNDATA[38]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[38], 38))
		{
			key_id = Key_HID_Usage_ID[38];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[38] = 0;
	}

	/* 39 */
	if((GTRY_HASP->RXLNDATA[39]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[39], 39))
		{
			key_id = Key_HID_Usage_ID[39];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[39] = 0;
	}

	/* 40 */
	if((GTRY_HASP->RXLNDATA[40]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[40], 40))
		{
			key_id = Key_HID_Usage_ID[40];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[40] = 0;
	}

	/* 41 */
	if((GTRY_HASP->RXLNDATA[41]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[41], 41))
		{
			key_id = Key_HID_Usage_ID[41];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[41] = 0;
	}

	/* 42 */
	if((GTRY_HASP->RXLNDATA[42]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[42], 42))
		{
			key_id = Key_HID_Usage_ID[42];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[42] = 0;
	}

	/* 43 */
	if((GTRY_HASP->RXLNDATA[43]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[43], 43))
		{
			key_id = Key_HID_Usage_ID[43];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[43] = 0;
	}

	/* 44 */
	if((GTRY_HASP->RXLNDATA[44]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[44], 44))
		{
			key_id = Key_HID_Usage_ID[44];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[44] = 0;
	}

	/* 45 */
	if((GTRY_HASP->RXLNDATA[45]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[45], 45))
		{
			key_id = Key_HID_Usage_ID[45];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[45] = 0;
	}

	/* 46 */
	if((GTRY_HASP->RXLNDATA[46]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[46], 46))
		{
			key_id = Key_HID_Usage_ID[46];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[46] = 0;
	}

	/* 47 */
	if((GTRY_HASP->RXLNDATA[47]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[47], 47))
		{
			key_id = Key_HID_Usage_ID[47];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[47] = 0;
	}

	/* 48 */
	if((GTRY_HASP->RXLNDATA[48]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[48], 48))
		{
			key_id = Key_HID_Usage_ID[48];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[48] = 0;
	}

	/* 49 */
	if((GTRY_HASP->RXLNDATA[49]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[49], 49))
		{
			key_id = Key_HID_Usage_ID[49];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[49] = 0;
	}

	/* 50 */
	if((GTRY_HASP->RXLNDATA[50]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[50], 50))
		{
			key_id = Key_HID_Usage_ID[50];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[50] = 0;
	}

	/* 51 */
	if((GTRY_HASP->RXLNDATA[51]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[51], 51))
		{
			key_id = Key_HID_Usage_ID[51];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[51] = 0;
	}

	/* 52 */
	if((GTRY_HASP->RXLNDATA[52]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[52], 52))
		{
			key_id = Key_HID_Usage_ID[52];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[52] = 0;
	}

	/* 53 */
	if((GTRY_HASP->RXLNDATA[53]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[53], 53))
		{
			key_id = Key_HID_Usage_ID[53];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[53] = 0;
	}

	/* 54 */
	if((GTRY_HASP->RXLNDATA[54]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[54], 54))
		{
			key_id = Key_HID_Usage_ID[54];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[54] = 0;
	}

	/* 55 */
	if((GTRY_HASP->RXLNDATA[55]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[55], 55))
		{
			key_id = Key_HID_Usage_ID[55];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[55] = 0;
	}

	/* 56 */
	if((GTRY_HASP->RXLNDATA[56]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[56], 56))
		{
			key_id = Key_HID_Usage_ID[56];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[56] = 0;
	}

	/* 57 */
	if((GTRY_HASP->RXLNDATA[57]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[57], 57))
		{
			key_id = Key_HID_Usage_ID[57];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[57] = 0;
	}

	/* 58 */
	if((GTRY_HASP->RXLNDATA[58]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[58], 58))
		{
			key_id = Key_HID_Usage_ID[58];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[58] = 0;
	}

	/* 59 */
	if((GTRY_HASP->RXLNDATA[59]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[59], 59))
		{
			key_id = Key_HID_Usage_ID[59];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[59] = 0;
	}

	/* 60 */
	if((GTRY_HASP->RXLNDATA[60]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[60], 60))
		{
			key_id = Key_HID_Usage_ID[60];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[60] = 0;
	}

	/* 62 */
	if((GTRY_HASP->RXLNDATA[62]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[62], 62))
		{
			key_id = Key_HID_Usage_ID[62];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[62] = 0;
	}

	/* 63 */
	if((GTRY_HASP->RXLNDATA[63]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[63], 63))
		{
			key_id = Key_HID_Usage_ID[63];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[63] = 0;
	}

	/* 64 */
	if((GTRY_HASP->RXLNDATA[64]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[64], 64))
		{
			key_id = Key_HID_Usage_ID[64];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[64] = 0;
	}

	/* 65 */
	if((GTRY_HASP->RXLNDATA[65]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[65], 65))
		{
			key_id = Key_HID_Usage_ID[65];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[65] = 0;
	}

	/* 66 */
	if((GTRY_HASP->RXLNDATA[66]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[66], 66))
		{
			key_id = Key_HID_Usage_ID[66];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[66] = 0;
	}

	/* 67 */
	if((GTRY_HASP->RXLNDATA[67]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[67], 67))
		{
			key_id = Key_HID_Usage_ID[67];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[67] = 0;
	}

	/* 69 */
	if((GTRY_HASP->RXLNDATA[69]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[69], 69))
		{
			key_id = Key_HID_Usage_ID[69];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[69] = 0;
	}

	/* 70 */
	if((GTRY_HASP->RXLNDATA[70]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[70], 70))
		{
			key_id = Key_HID_Usage_ID[70];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[70] = 0;
	}

	/* 71 */
	if((GTRY_HASP->RXLNDATA[71]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[71], 71))
		{
			key_id = Key_HID_Usage_ID[71];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[71] = 0;
	}

	/* 72 */
	if((GTRY_HASP->RXLNDATA[72]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[72], 72))
		{
			key_id = Key_HID_Usage_ID[72];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[72] = 0;
	}

	/* 73 */
	if((GTRY_HASP->RXLNDATA[73]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[73], 73))
		{
			key_id = Key_HID_Usage_ID[73];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[73] = 0;
	}

	/* 74 */
	if((GTRY_HASP->RXLNDATA[74]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[74], 74))
		{
			key_id = Key_HID_Usage_ID[74];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[74] = 0;
	}

	/* 77 */
	if((GTRY_HASP->RXLNDATA[77]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[77], 77))
		{
			key_id = Key_HID_Usage_ID[77];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[77] = 0;
	}

	/* 78 */
	if((GTRY_HASP->RXLNDATA[78]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[78], 78))
		{
			key_id = Key_HID_Usage_ID[78];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[78] = 0;
	}

	/* 79 */
	if((GTRY_HASP->RXLNDATA[79]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[79], 79))
		{
			key_id = Key_HID_Usage_ID[79];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[79] = 0;
	}

	/* 80 */
	if((GTRY_HASP->RXLNDATA[80]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[80], 80))
		{
			key_id = Key_HID_Usage_ID[80];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[80] = 0;
	}

	/* 81 */
	if((GTRY_HASP->RXLNDATA[81]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[81], 81))
		{
			key_id = Key_HID_Usage_ID[81];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[81] = 0;
	}

	/* 84 */
	if((GTRY_HASP->RXLNDATA[84]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[84], 84))
		{
			key_id = Key_HID_Usage_ID[84];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[84] = 0;
	}

	/* 85 */
	if((GTRY_HASP->RXLNDATA[85]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[85], 85))
		{
			key_id = Key_HID_Usage_ID[85];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[85] = 0;
	}

	/* 86 */
	if((GTRY_HASP->RXLNDATA[86]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[86], 86))
		{
			key_id = Key_HID_Usage_ID[86];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[86] = 0;
	}

	/* 87 */
	if((GTRY_HASP->RXLNDATA[87]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[87], 87))
		{
			key_id = Key_HID_Usage_ID[87];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[87] = 0;
	}

	/* 92 */
	if((GTRY_HASP->RXLNDATA[92]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[92], 92))
		{
			key_id = Key_HID_Usage_ID[92];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[92] = 0;
	}

	/* 93 */
	if((GTRY_HASP->RXLNDATA[93]) > KRY_DOWN_VALUE)
	{
		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[93], 93))
		{
			key_id = Key_HID_Usage_ID[93];
			if (key_id >= 0xE0U)
			{
				cur_report[0] |= (unsigned char)(1U << (key_id - 0xE0U));
			}
			else if (key_count < 6U)
			{
				cur_report[2U + key_count] = key_id;
				key_count++;
			}
		}
	}
	else
	{
		current_node_update[93] = 0;
	}

//	unsigned char key_id, temp_index, temp_pos;
//
//	wdt_reset_timer();
//
//	/* 0 */
//	if((GTRY_HASP->RXLNDATA[0]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[0], 0))
//		{
//			key_id = Key_HID_Usage_ID[0];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[0] = 0;
//	}
//
//	/* 7 */
//	if((GTRY_HASP->RXLNDATA[7]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[7], 7))
//		{
////			test_pin2_OFF;
//			key_id = Key_HID_Usage_ID[7];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[7] = 0;
//	}
////	test_pin2_ON;
//	/* 14 */
//	if((GTRY_HASP->RXLNDATA[14]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[14], 14))
//		{
//			key_id = Key_HID_Usage_ID[14];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[14] = 0;
//	}
//
//	/* 21 */
//	if((GTRY_HASP->RXLNDATA[21]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[21], 21))
//		{
//			key_id = Key_HID_Usage_ID[21];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[21] = 0;
//	}
//
//	/* 28 */
//	if((GTRY_HASP->RXLNDATA[28]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[28], 28))
//		{
//			key_id = Key_HID_Usage_ID[28];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[28] = 0;
//	}
//
//	/* 35 */
//	if((GTRY_HASP->RXLNDATA[35]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[35], 35))
//		{
//			key_id = Key_HID_Usage_ID[35];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[35] = 0;
//	}
//
//	/* 42 */
//	if((GTRY_HASP->RXLNDATA[42]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[42], 42))
//		{
//			key_id = Key_HID_Usage_ID[42];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[42] = 0;
//	}
//
//	/* 49 */
//	if((GTRY_HASP->RXLNDATA[49]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[49], 49))
//		{
//			key_id = Key_HID_Usage_ID[49];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[49] = 0;
//	}
//
//	/* 56 */
//	if((GTRY_HASP->RXLNDATA[56]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[56], 56))
//		{
//			key_id = Key_HID_Usage_ID[56];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[56] = 0;
//	}
//
//	/* 63 */
//	if((GTRY_HASP->RXLNDATA[63]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[63], 63))
//		{
//			key_id = Key_HID_Usage_ID[63];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[63] = 0;
//	}
//
//	/* 70 */
//	if((GTRY_HASP->RXLNDATA[70]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[70], 70))
//		{
//			key_id = Key_HID_Usage_ID[70];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[70] = 0;
//	}
//
//	/* 77 */
//	if((GTRY_HASP->RXLNDATA[77]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[77], 77))
//		{
//			key_id = Key_HID_Usage_ID[77];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[77] = 0;
//	}
//
//	/* 84 */
//	if((GTRY_HASP->RXLNDATA[84]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[84], 84))
//		{
//			key_id = Key_HID_Usage_ID[84];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[84] = 0;
//	}
//
//	/* 1 */
//	if((GTRY_HASP->RXLNDATA[1]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[1], 1))
//		{
//			key_id = Key_HID_Usage_ID[1];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[1] = 0;
//	}
//
//	/* 8 */
//	if((GTRY_HASP->RXLNDATA[8]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[8], 8))
//		{
//			key_id = Key_HID_Usage_ID[8];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[8] = 0;
//	}
//
//	/* 15 */
//	if((GTRY_HASP->RXLNDATA[15]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[15], 15))
//		{
//			key_id = Key_HID_Usage_ID[15];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[15] = 0;
//	}
//
//	/* 22 */
//	if((GTRY_HASP->RXLNDATA[22]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[22], 22))
//		{
//			key_id = Key_HID_Usage_ID[22];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[22] = 0;
//	}
//
//	/* 29 */
//	if((GTRY_HASP->RXLNDATA[29]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[29], 29))
//		{
//			key_id = Key_HID_Usage_ID[29];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[29] = 0;
//	}
//
//	/* 36 */
//	if((GTRY_HASP->RXLNDATA[36]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[36], 36))
//		{
//			key_id = Key_HID_Usage_ID[36];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[36] = 0;
//	}
//
//	/* 43 */
//	if((GTRY_HASP->RXLNDATA[43]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[43], 43))
//		{
//			key_id = Key_HID_Usage_ID[43];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[43] = 0;
//	}
//
//	/* 50 */
//	if((GTRY_HASP->RXLNDATA[50]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[50], 50))
//		{
//			key_id = Key_HID_Usage_ID[50];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[50] = 0;
//	}
//
//	/* 57 */
//	if((GTRY_HASP->RXLNDATA[57]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[57], 57))
//		{
//			key_id = Key_HID_Usage_ID[57];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[57] = 0;
//	}
//
//	/* 64 */
//	if((GTRY_HASP->RXLNDATA[64]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[64], 64))
//		{
//			key_id = Key_HID_Usage_ID[64];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[64] = 0;
//	}
//
//	/* 71 */
//	if((GTRY_HASP->RXLNDATA[71]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[71], 71))
//		{
//			key_id = Key_HID_Usage_ID[71];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[71] = 0;
//	}
//
//	/* 78 */
//	if((GTRY_HASP->RXLNDATA[78]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[78], 78))
//		{
//			key_id = Key_HID_Usage_ID[78];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[78] = 0;
//	}
//
//	/* 85 */
//	if((GTRY_HASP->RXLNDATA[85]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[85], 85))
//		{
//			key_id = Key_HID_Usage_ID[85];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[85] = 0;
//	}
//
//	/* 92 */
//	if((GTRY_HASP->RXLNDATA[92]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[92], 92))
//		{
//			key_id = Key_HID_Usage_ID[92];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[92] = 0;
//	}
//
//	/* 2 */
//	if((GTRY_HASP->RXLNDATA[2]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[2], 2))
//		{
//			key_id = Key_HID_Usage_ID[2];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[2] = 0;
//	}
//
//	/* 9 */
//	if((GTRY_HASP->RXLNDATA[9]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[9], 9))
//		{
//			key_id = Key_HID_Usage_ID[9];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[9] = 0;
//	}
//
//	/* 16 */
//	if((GTRY_HASP->RXLNDATA[16]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[16], 16))
//		{
//			key_id = Key_HID_Usage_ID[16];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[16] = 0;
//	}
//
//	/* 23 */
//	if((GTRY_HASP->RXLNDATA[23]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[23], 23))
//		{
//			key_id = Key_HID_Usage_ID[23];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[23] = 0;
//	}
//
//	/* 30 */
//	if((GTRY_HASP->RXLNDATA[30]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[30], 30))
//		{
//			key_id = Key_HID_Usage_ID[30];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[30] = 0;
//	}
//
//	/* 37 */
//	if((GTRY_HASP->RXLNDATA[37]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[37], 37))
//		{
//			key_id = Key_HID_Usage_ID[37];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[37] = 0;
//	}
//
//
//
//	/* 44 */
//	if((GTRY_HASP->RXLNDATA[44]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[44], 44))
//		{
//			key_id = Key_HID_Usage_ID[44];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[44] = 0;
//	}
//
//	/* 51 */
//	if((GTRY_HASP->RXLNDATA[51]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[51], 51))
//		{
//			key_id = Key_HID_Usage_ID[51];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[51] = 0;
//	}
//
//	/* 58 */
//	if((GTRY_HASP->RXLNDATA[58]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[58], 58))
//		{
//			key_id = Key_HID_Usage_ID[58];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[58] = 0;
//	}
//
//	/* 65 */
//	if((GTRY_HASP->RXLNDATA[65]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[65], 65))
//		{
//			key_id = Key_HID_Usage_ID[65];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[65] = 0;
//	}
//
//	/* 72 */
//	if((GTRY_HASP->RXLNDATA[72]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[72], 72))
//		{
//			key_id = Key_HID_Usage_ID[72];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[72] = 0;
//	}
//
//	/* 79 */
//	if((GTRY_HASP->RXLNDATA[79]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[79], 79))
//		{
//			key_id = Key_HID_Usage_ID[79];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[79] = 0;
//	}
//
//	/* 86 */
//	if((GTRY_HASP->RXLNDATA[86]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[86], 86))
//		{
//			key_id = Key_HID_Usage_ID[86];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[86] = 0;
//	}
//
//	/* 93 */
//	if((GTRY_HASP->RXLNDATA[93]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[93], 93))
//		{
//			key_id = Key_HID_Usage_ID[93];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[93] = 0;
//	}
//
//	/* 3 */
//	if((GTRY_HASP->RXLNDATA[3]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[3], 3))
//		{
//			key_id = Key_HID_Usage_ID[3];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[3] = 0;
//	}
//
//	/* 10 */
//	if((GTRY_HASP->RXLNDATA[10]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[10], 10))
//		{
//			key_id = Key_HID_Usage_ID[10];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[10] = 0;
//	}
//
//	/* 17 */
//	if((GTRY_HASP->RXLNDATA[17]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[17], 17))
//		{
//			key_id = Key_HID_Usage_ID[17];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[17] = 0;
//	}
//
//	/* 24 */
//	if((GTRY_HASP->RXLNDATA[24]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[24], 24))
//		{
//			key_id = Key_HID_Usage_ID[24];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[24] = 0;
//	}
//
//	/* 31 */
//	if((GTRY_HASP->RXLNDATA[31]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[31], 31))
//		{
//			key_id = Key_HID_Usage_ID[31];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[31] = 0;
//	}
//
//	/* 38 */
//	if((GTRY_HASP->RXLNDATA[38]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[38], 38))
//		{
//			key_id = Key_HID_Usage_ID[38];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[38] = 0;
//	}
//
//	/* 45 */
//	if((GTRY_HASP->RXLNDATA[45]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[45], 45))
//		{
//			key_id = Key_HID_Usage_ID[45];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[45] = 0;
//	}
//
//	/* 52 */
//	if((GTRY_HASP->RXLNDATA[52]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[52], 52))
//		{
//			key_id = Key_HID_Usage_ID[52];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[52] = 0;
//	}
//
//	/* 59 */
//	if((GTRY_HASP->RXLNDATA[59]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[59], 59))
//		{
//			key_id = Key_HID_Usage_ID[59];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[59] = 0;
//	}
//
//	/* 66 */
//	if((GTRY_HASP->RXLNDATA[66]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[66], 66))
//		{
//			key_id = Key_HID_Usage_ID[66];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[66] = 0;
//	}
//
//	/* 73 */
//	if((GTRY_HASP->RXLNDATA[73]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[73], 73))
//		{
//			key_id = Key_HID_Usage_ID[73];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[73] = 0;
//	}
//
//	/* 80 */
//	if((GTRY_HASP->RXLNDATA[80]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[80], 80))
//		{
//			key_id = Key_HID_Usage_ID[80];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[80] = 0;
//	}
//
//	/* 87 */
//	if((GTRY_HASP->RXLNDATA[87]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[87], 87))
//		{
//			key_id = Key_HID_Usage_ID[87];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[87] = 0;
//	}
//
//	/* 4 */
//	if((GTRY_HASP->RXLNDATA[4]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[4], 4))
//		{
//			key_id = Key_HID_Usage_ID[4];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[4] = 0;
//	}
//
//	/* 11 */
//	if((GTRY_HASP->RXLNDATA[11]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[11], 11))
//		{
//			key_id = Key_HID_Usage_ID[11];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[11] = 0;
//	}
//
//	/* 18 */
//	if((GTRY_HASP->RXLNDATA[18]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[18], 18))
//		{
//			key_id = Key_HID_Usage_ID[18];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[18] = 0;
//	}
//
//
//	/* 25 */
//	if((GTRY_HASP->RXLNDATA[25]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[25], 25))
//		{
//			key_id = Key_HID_Usage_ID[25];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[25] = 0;
//	}
//
//	/* 32 */
//	if((GTRY_HASP->RXLNDATA[32]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[32], 32))
//		{
//			key_id = Key_HID_Usage_ID[32];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[32] = 0;
//	}
//
//	/* 39 */
//	if((GTRY_HASP->RXLNDATA[39]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[39], 39))
//		{
//			key_id = Key_HID_Usage_ID[39];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[39] = 0;
//	}
//
//	/* 46 */
//	if((GTRY_HASP->RXLNDATA[46]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[46], 46))
//		{
//			key_id = Key_HID_Usage_ID[46];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[46] = 0;
//	}
//
//	/* 53 */
//	if((GTRY_HASP->RXLNDATA[53]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[53], 53))
//		{
//			key_id = Key_HID_Usage_ID[53];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[53] = 0;
//	}
//
//	/* 60 */
//	if((GTRY_HASP->RXLNDATA[60]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[60], 60))
//		{
//			key_id = Key_HID_Usage_ID[60];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[60] = 0;
//	}
//
//	/* 67 */
//	if((GTRY_HASP->RXLNDATA[67]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[67], 67))
//		{
//			key_id = Key_HID_Usage_ID[67];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[67] = 0;
//	}
//
//	/* 74 */
//	if((GTRY_HASP->RXLNDATA[74]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[74], 74))
//		{
//			key_id = Key_HID_Usage_ID[74];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[74] = 0;
//	}
//
//	/* 81 */
//	if((GTRY_HASP->RXLNDATA[81]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[81], 81))
//		{
//			key_id = Key_HID_Usage_ID[81];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[81] = 0;
//	}
//
//	/* 5 */
//	if((GTRY_HASP->RXLNDATA[5]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[5], 5))
//		{
//			key_id = Key_HID_Usage_ID[5];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[5] = 0;
//	}
//
//	/* 12 */
//	if((GTRY_HASP->RXLNDATA[12]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[12], 12))
//		{
//			key_id = Key_HID_Usage_ID[12];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[12] = 0;
//	}
//
//	/* 19 */
//	if((GTRY_HASP->RXLNDATA[19]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[19], 19))
//		{
//			key_id = Key_HID_Usage_ID[19];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[19] = 0;
//	}
//
//	/* 26 */
//	if((GTRY_HASP->RXLNDATA[26]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[26], 26))
//		{
//			key_id = Key_HID_Usage_ID[26];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[26] = 0;
//	}
//
//	/* 33 */
//	if((GTRY_HASP->RXLNDATA[33]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[33], 33))
//		{
//			key_id = Key_HID_Usage_ID[33];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[33] = 0;
//	}
//
//	/* 40 */
//	if((GTRY_HASP->RXLNDATA[40]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[40], 40))
//		{
//			key_id = Key_HID_Usage_ID[40];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[40] = 0;
//	}
//
//	/* 47 */
//	if((GTRY_HASP->RXLNDATA[47]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[47], 47))
//		{
//			key_id = Key_HID_Usage_ID[47];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[47] = 0;
//	}
//
//	/* 54 */
//	if((GTRY_HASP->RXLNDATA[54]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[54], 54))
//		{
//			key_id = Key_HID_Usage_ID[54];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[54] = 0;
//	}
//
//
//	/* 6 */
//	if((GTRY_HASP->RXLNDATA[6]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[6], 6))
//		{
//			key_id = Key_HID_Usage_ID[6];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[6] = 0;
//	}
//
//	/* 13 */
//	if((GTRY_HASP->RXLNDATA[13]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[13], 13))
//		{
//			key_id = Key_HID_Usage_ID[13];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[13] = 0;
//	}
//
//	/* 20 */
//	if((GTRY_HASP->RXLNDATA[20]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[20], 20))
//		{
//			key_id = Key_HID_Usage_ID[20];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[20] = 0;
//	}
//
//	/* 27 */
//	if((GTRY_HASP->RXLNDATA[27]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[27], 27))
//		{
//			key_id = Key_HID_Usage_ID[27];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[27] = 0;
//	}
//
//	/* 34 */
//	if((GTRY_HASP->RXLNDATA[34]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[34], 34))
//		{
//			key_id = Key_HID_Usage_ID[34];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[34] = 0;
//	}
//
//	/* 41 */
//	if((GTRY_HASP->RXLNDATA[41]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[41], 41))
//		{
//			key_id = Key_HID_Usage_ID[41];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[41] = 0;
//	}
//
//	/* 48 */
//	if((GTRY_HASP->RXLNDATA[48]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[48], 48))
//		{
//			key_id = Key_HID_Usage_ID[48];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[48] = 0;
//	}
//
//	/* 55 */
//	if((GTRY_HASP->RXLNDATA[55]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[55], 55))
//		{
//			key_id = Key_HID_Usage_ID[55];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[55] = 0;
//	}
//
//	/* 62 */
//	if((GTRY_HASP->RXLNDATA[62]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[62], 62))
//		{
//			key_id = Key_HID_Usage_ID[33];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[62] = 0;
//	}
//
//	/* 69 */
//	if((GTRY_HASP->RXLNDATA[69]) > KRY_DOWN_VALUE)
//	{
//		if(ana_hal_cali_data_handle(GTRY_HASP->RXLNDATA[69], 69))
//		{
//			key_id = Key_HID_Usage_ID[69];
//			if(key_id < 0xE0)
//			{
//				temp_index = key_id >> 3;
//				temp_pos   = key_id & 7;
//				cur_report[2 + temp_index] |= 1 << temp_pos;
//			}
//			else
//			{
//				temp_pos = key_id - 0xE0;
//				cur_report[0] |= 1 << temp_pos;
//			}
//		}
//	}
//	else
//	{
//		current_node_update[69] = 0;
//	}
//

}
