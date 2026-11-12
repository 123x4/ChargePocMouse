/*
 * h6_offset_cail.c
 *
 *  Created on: Jun 24, 2026
 *      Author: Lenovo
 */

#include <string.h>
#include "gtry_type.h"
#include "gtry_regmap.h"
#include "hasp_drv.h"
#include "h6_drv.h"
#include "h6_hw_tab.h"
#include "h6_hal.h"
#include "h6_offset_cali.h"
#include "app_config.h"
#include "gtry_wdt_drv.h"
#include "mfg_mode.h"
#include "gtry_qspi_flash_drv.h"
#include "h6_hw_tab.h"

#if H6_OFFSET_CALI

#define H6_OFFSET_KEY_STORAGE_NUM		(HASP_CHANNL_NUM * HASP_CHAIN_LENGTH)
#define H6_IMBALANCE_CALI_MAGIC			0x494D424CU 	/* 'IMBL': imbalance calibration payload marker. */
#define H6_IMBALANCE_CALI_HEADER_SIZE	4U
#define H6_IMBALANCE_CALI_U16_SIZE		2U
#define H6_IMBALANCE_CALI_C36_OFFSET	H6_IMBALANCE_CALI_HEADER_SIZE
#define H6_IMBALANCE_CALI_C37_OFFSET	(H6_IMBALANCE_CALI_C36_OFFSET + (HASP_KEY_NUM * H6_IMBALANCE_CALI_U16_SIZE))
#define H6_IMBALANCE_CALI_PAYLOAD_SIZE	(H6_IMBALANCE_CALI_C37_OFFSET + (HASP_KEY_NUM * H6_IMBALANCE_CALI_U16_SIZE))

#ifndef H6_OFFSET_CALI_VERBOSE_LOG
#define H6_OFFSET_CALI_VERBOSE_LOG		0 /* Keep verbose per-Hall calibration logs off by default to avoid WDT resets. */
#endif

#ifndef H6_OFFSET_CALI_FAIL_LOG
#define H6_OFFSET_CALI_FAIL_LOG			0 /* Print only failed Hall slots so calibration can be debugged without flooding UART. */
#endif

#ifndef H6_IMBALANCE_CALI_SAVE_LOG
#define H6_IMBALANCE_CALI_SAVE_LOG		1 /* Print the saved REG36/REG37 trim table after calibration succeeds. */
#endif

#if WDT_TEST_MODE
#define H6_OFFSET_CALI_FEED_WDT()		wdt_reset_timer() /* Reset WDT immediately; wdt_hal_feed_dog() is throttled by a 1000-call counter. */
#else
#define H6_OFFSET_CALI_FEED_WDT()
#endif

#if H6_OFFSET_CALI_FAIL_LOG
#define H6_OFFSET_CALI_FAIL_PRINTF(...)	printf(__VA_ARGS__) /* Keep failure logs visible even when verbose debug logs are compiled out. */
#else
#define H6_OFFSET_CALI_FAIL_PRINTF(...)
#endif

#if (H6_OFFSET_CALI_VERBOSE_LOG == 0)
#undef debug
#define debug(...)						/* Drop high-volume calibration logs; keep printf stage results visible. */
#endif

static uint8_t h6_imbalance_cali_last_ret = H6_IMBALANCE_CALI_RET_OK; /* Cache the latest 04 10 D4/live-calibration result for debug reads. */

static void h6_imbalance_cali_prepare_self_check(void)
{
	uint8_t key_num = g_hasp_key_num; /* Use the selected layout slot count directly; the product maximum is 112. */

	for (uint8_t i = 0; i < key_num; i++)
	{
		if (chain_miss_buff_reg[i])
		{
			hall_hal_set_self_check_value(i, HALL_SELF_CHECK_CRC_OK_VALUE); /* Mark active slots good until this calibration proves otherwise. */
		}
		else
		{
			hall_hal_set_self_check_value(i, HALL_SELF_CHECK_EMPTY_VALUE); /* Keep empty aligned slots distinguishable in 04 17 0C reports. */
		}
	}
}

static void h6_imbalance_cali_mark_self_check_error(uint32_t key_idx)
{
	uint8_t key_num = g_hasp_key_num; /* Use the runtime Hall slot count directly instead of calling a helper. */

	if (key_idx < key_num)
	{
		hall_hal_set_self_check_value(key_idx, HALL_SELF_CHECK_OFFSET_CALI_ERROR_VALUE); /* Latch the offset-calibration failure slot. */
	}
}

static void h6_imbalance_cali_finish_self_check(uint32_t ret)
{
	h6_imbalance_cali_last_ret = (uint8_t)ret; /* Preserve the exact return code for command responses and debug logs. */
	if (ret != H6_IMBALANCE_CALI_RET_OK)
	{
		h6_error_sign = 1U; /* Prevent later H6 checks from hiding the imbalance-calibration failure cause. */
	}
}

uint8_t h6_imbalance_cali_get_last_ret(void)
{
	return h6_imbalance_cali_last_ret; /* Return the last H6 imbalance-calibration result byte. */
}

static uint8_t h6_imbalance_cali_flash_is_blank(const uint8_t *data, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		if ((i & 0x1FU) == 0U)
		{
			H6_OFFSET_CALI_FEED_WDT(); /* Flash validation may run during boot before the main loop starts feeding WDT. */
		}
		if (data[i] != 0xFFU)
		{
			return 0U; /* Any programmed byte means Flash already contains calibration data. */
		}
	}
	return 1U; /* A fully erased sector reads as 0xFF and must not be treated as valid calibration. */
}

static void h6_imbalance_cali_fill_header(uint8_t *data)
{
	data[0] = (uint8_t)(H6_IMBALANCE_CALI_MAGIC & 0xFFU); /* Magic byte 0 identifies the payload format. */
	data[1] = (uint8_t)((H6_IMBALANCE_CALI_MAGIC >> 8) & 0xFFU); /* Magic byte 1 identifies the payload format. */
	data[2] = (uint8_t)((H6_IMBALANCE_CALI_MAGIC >> 16) & 0xFFU); /* Magic byte 2 identifies the payload format. */
	data[3] = (uint8_t)((H6_IMBALANCE_CALI_MAGIC >> 24) & 0xFFU); /* Magic byte 3 identifies the payload format. */
}

static uint8_t h6_imbalance_cali_header_is_valid(const uint8_t *data)
{
	uint32_t magic = (uint32_t)data[0]
				   | ((uint32_t)data[1] << 8)
				   | ((uint32_t)data[2] << 16)
				   | ((uint32_t)data[3] << 24); /* Rebuild little-endian magic for validation. */

	if (magic != H6_IMBALANCE_CALI_MAGIC)
	{
		return 0U; /* Reject data that was not written by this imbalance calibration format. */
	}
	return 1U;
}

static void h6_imbalance_cali_write_u16(uint8_t *data, uint32_t offset, uint16_t value)
{
	data[offset] = (uint8_t)(value & 0xFFU); /* Store the low byte first so Flash dumps are little-endian like the MCU. */
	data[offset + 1U] = (uint8_t)((value >> 8) & 0xFFU); /* Store the high byte so the full uint16_t trim is preserved. */
}

static uint16_t h6_imbalance_cali_read_u16(const uint8_t *data, uint32_t offset)
{
	return (uint16_t)data[offset] | ((uint16_t)data[offset + 1U] << 8); /* Rebuild the full uint16_t trim from Flash. */
}

#if H6_IMBALANCE_CALI_SAVE_LOG
static void h6_imbalance_cali_dump_save_values(const uint16_t *c36, const uint16_t *c37, uint32_t slot_num)
{
	printf("imb_save begin slots=%u\r\n", slot_num); /* Mark the start of the compact saved-trim dump. */
	for (uint32_t i = 0; i < slot_num; i++)
	{
		if ((i & 0x0FU) == 0U)
		{
			H6_OFFSET_CALI_FEED_WDT(); /* UART printing can be slow, so keep the watchdog serviced during the dump. */
		}
		if (chain_miss_buff_reg[i])
		{
			printf("imb_save slot=%u chain=%u:%u C36=0x%04x C37=0x%04x\r\n",
					i,
					i % HASP_CHANNL_NUM,
					i / HASP_CHANNL_NUM,
					c36[i],
					c37[i]); /* Print the exact uint16_t values that will be encoded into Flash. */
		}
	}
	printf("imb_save end\r\n"); /* Mark the end so the UART log is easy to search. */
}
#else
static void h6_imbalance_cali_dump_save_values(const uint16_t *c36, const uint16_t *c37, uint32_t slot_num)
{
	(void)c36;
	(void)c37;
	(void)slot_num;
}
#endif

static unsigned int h6_imbalance_cali_save_to_flash(const uint16_t *c36, const uint16_t *c37)
{
	uint8_t flash_buf[H6_IMBALANCE_CALI_PAYLOAD_SIZE];

	if (g_hasp_key_num > HASP_KEY_NUM)
	{
		return 1U; /* Guard the fixed payload buffer from an invalid runtime Hall slot count. */
	}

	memset(flash_buf, 0xFF, sizeof(flash_buf)); /* Keep unused bytes erased-like so dumps are easy to inspect. */
	h6_imbalance_cali_fill_header(flash_buf);

	for (uint32_t i = 0; i < g_hasp_key_num; i++)
	{
		if ((i & 0x1FU) == 0U)
		{
			H6_OFFSET_CALI_FEED_WDT(); /* Building the Flash payload is part of the blocking calibration path. */
		}
		h6_imbalance_cali_write_u16(flash_buf, H6_IMBALANCE_CALI_C36_OFFSET + (i * H6_IMBALANCE_CALI_U16_SIZE), c36[i]); /* Store the full REG36 uint16_t trim for this Hall slot. */
		h6_imbalance_cali_write_u16(flash_buf, H6_IMBALANCE_CALI_C37_OFFSET + (i * H6_IMBALANCE_CALI_U16_SIZE), c37[i]); /* Store the full REG37 uint16_t trim for this Hall slot. */
	}

	h6_imbalance_cali_dump_save_values(c36, c37, g_hasp_key_num);
	H6_OFFSET_CALI_FEED_WDT();
	return QSPI_EXT_FLASH_write(IMBALANCE_CALI_ADDR, flash_buf, sizeof(flash_buf)); /* Erase and program the calibration sector. */
}

uint8_t hall_hal_load_imbalance_cali_from_flash(void)
{
	uint8_t flash_buf[H6_IMBALANCE_CALI_PAYLOAD_SIZE];
	uint16_t c36[HASP_KEY_NUM] = {0};
	uint16_t c37[HASP_KEY_NUM] = {0};
	unsigned int read_len;

	read_len = QSPI_EXT_FLASH_read(IMBALANCE_CALI_ADDR, flash_buf, sizeof(flash_buf)); /* Read saved REG36/REG37 trims. */
	H6_OFFSET_CALI_FEED_WDT();
	if (read_len != sizeof(flash_buf))
	{
		return 0U; /* Read failure means runtime should fall back to live calibration. */
	}
	if (h6_imbalance_cali_flash_is_blank(flash_buf, sizeof(flash_buf)))
	{
		return 0U; /* Blank Flash means this keyboard has not stored imbalance calibration yet. */
	}
	if (h6_imbalance_cali_header_is_valid(flash_buf) == 0U)
	{
		return 0U; /* Header mismatch protects against wrong-layout calibration data. */
	}

	for (uint32_t i = 0; i < g_hasp_key_num; i++)
	{
		if ((i & 0x1FU) == 0U)
		{
			H6_OFFSET_CALI_FEED_WDT(); /* Restoring all saved Hall trims still runs before the main loop can feed WDT. */
		}
		uint16_t reg36 = h6_imbalance_cali_read_u16(flash_buf, H6_IMBALANCE_CALI_C36_OFFSET + (i * H6_IMBALANCE_CALI_U16_SIZE)); /* Recover the full saved REG36 value. */
		uint16_t reg37 = h6_imbalance_cali_read_u16(flash_buf, H6_IMBALANCE_CALI_C37_OFFSET + (i * H6_IMBALANCE_CALI_U16_SIZE)); /* Recover the full saved REG37 value. */

		if (chain_miss_buff_reg[i])
		{
			if ((reg36 > 0x3FU) || (reg37 > 0x3FU))
			{
				return 0U; /* REG36/REG37 are 6-bit trim fields; reject corrupted Flash data. */
			}
			c36[i] = reg36; /* Queue REG36 trim for an active Hall slot. */
			c37[i] = reg37; /* Queue REG37 trim for an active Hall slot. */
		}
	}

	h6_drv_write_reg_cmd(CONGFIG_REG36, c36, g_hasp_key_num); /* Restore all saved REG36 trims after H6 reset/init. */

//	h6_drv_read_reg_cmd(CONGFIG_REG36, g_hasp_key_num);
//	for(uint8_t i = 0; i < g_hasp_key_num; i++)
//	{
//		printf("c36->%d:%d value = 0x%02x\r\n", i%HASP_CHANNL_NUM, i/HASP_CHANNL_NUM, (GTRY_HASP->RXRAM[i] & 0xFF));
//	}

	h6_drv_write_reg_cmd(CONGFIG_REG37, c37, g_hasp_key_num); /* Restore all saved REG37 trims after H6 reset/init. */

//	h6_drv_read_reg_cmd(CONGFIG_REG37, g_hasp_key_num);
//	for(uint8_t i = 0; i < g_hasp_key_num; i++)
//	{
//		printf("c37->%d:%d value = 0x%02x\r\n", i%HASP_CHANNL_NUM, i/HASP_CHANNL_NUM, (GTRY_HASP->RXRAM[i] & 0xFF));
//	}

	return 1U;
}

/* Apply the synchronized gain table without changing REG29/REG2A, which the
 * following offset-calibration sequence configures to its own fixed values. */
static void h6_offset_cali_apply_gain_from_flash(void)
{
	uint16_t gain_table[HASP_KEY_NUM] = {0};

	hall_gain_load_final_table(gain_table, g_hasp_key_num);
	h6_drv_write_reg_cmd(CONGFIG_REGB, gain_table, g_hasp_key_num);
}

/**
 * \brief     Obtain the average conversion value
 *
 * \param[in] None
 * \return    0: successful, Other: failed
 */
uint32_t get_H6_convert_avg(uint32_t *avg_buf ,uint32_t num)
{
	uint32_t i =0 ,j = 0;
	uint32_t test_times = num;
	uint32_t val_crc;
	uint16_t conv_data[HASP_KEY_NUM] = {0};
	uint8_t valid_sample[HASP_KEY_NUM] = {0};
	uint32_t count = 0;
	uint32_t sum[HASP_KEY_NUM] = {0};
	uint32_t nsck_shadow = hasp_drv_get_nsck();
	uint32_t ctrl_shadow = hasp_drv_get_ctrl();

	h6_hal_variable_time_inst(0);
	hasp_drv_set_ctrl(hasp_drv_get_ctrl()|HASP_CONTINUE_CONV);	//Activate the HASP continuous conversion mode
	hasp_drv_start_work();										//HASP start

	while(test_times--)
	{
		H6_OFFSET_CALI_FEED_WDT();
		ry_delay_us(40);
		count = 0;
		memset(valid_sample, 0, sizeof(valid_sample)); /* Track which active Hall slots produced a CRC-valid sample in this round. */
		for(i = 0; i < g_hasp_key_num ; i++)
		{
			if ((i & 0x0FU) == 0U)
			{
				H6_OFFSET_CALI_FEED_WDT(); /* Long active-slot scans must not rely on the outer main-loop feed point. */
			}
			if(chain_miss_buff_reg[i])
			{
				val_crc = 0;
				conv_data[i] = GTRY_HASP->RXRAWDATA[i];
				for(j = 0; j < 14; j = j+2)
				{
					val_crc ^= ((conv_data[i] >> j) & 0x3);
				}
				if(val_crc == (conv_data[i] >> j))
				{
					sum[i] += conv_data[i] & 0x3fff;
					valid_sample[i] = 1U; /* This slot contributed one valid sample to the average. */
					count++;
				}
				else
				{
					h6_imbalance_cali_mark_self_check_error(i); /* Report the exact slot whose raw sample failed CRC. */
					debug("hall chain->%d:%d sample = %d, hall crc is error!\r\n", i%HASP_CHANNL_NUM,i/HASP_CHANNL_NUM,conv_data[i]);
				}
			}
			else
			{
				continue;
			}
		}
		val_crc = 0;

		/* Compare with real active Hall keys; aligned empty slots do not produce CRC-valid samples. */
		if(count != g_key_board_num)
		{
			for (i = 0; i < g_hasp_key_num; i++)
			{
				if (chain_miss_buff_reg[i] && (valid_sample[i] == 0U))
				{
					h6_imbalance_cali_mark_self_check_error(i); /* Mark active slots missing from the valid-sample count. */
				}
			}
			H6_OFFSET_CALI_FEED_WDT();
			hasp_drv_nsck_set(nsck_shadow);
			hasp_drv_set_ctrl(ctrl_shadow);
			return 1;	//There is an abnormal CRC chip during the conversion process.
		}
	}

	for(i = 0; i < g_hasp_key_num ; i++)
	{
		if ((i & 0x0FU) == 0U)
		{
			H6_OFFSET_CALI_FEED_WDT(); /* Averaging cleanup can still be reached after a long conversion burst. */
		}
		if(chain_miss_buff_reg[i])
		avg_buf[i] = (sum[i]/num);
	}
	H6_OFFSET_CALI_FEED_WDT();
	hasp_drv_nsck_set(nsck_shadow);
	hasp_drv_set_ctrl(ctrl_shadow);
	return 0;
}

/**
 * \brief     h6 offset calibration
 *
 * \param[in] None
 * \return    0: calibration successful, Other: calibration failed
 */
unsigned int H6_offset_cali_b(void)
{
	uint32_t i = 0 , j = 0, ret = 0 , k = 0;
	uint32_t avg1[HASP_KEY_NUM] = {0};
	uint32_t avg2[HASP_KEY_NUM] = {0};
	uint32_t min_d1[HASP_KEY_NUM] = {0} , min_d2[HASP_KEY_NUM] = {0};
	uint16_t C36_1[HASP_KEY_NUM] = {0} , C36_2[HASP_KEY_NUM] = {0} , C37_1[HASP_KEY_NUM] = {0} , C37_2[HASP_KEY_NUM] = {0};
	uint16_t C36[HASP_KEY_NUM] = {0} , C37[HASP_KEY_NUM] = {0};
	uint32_t D37_1[HASP_KEY_NUM] = {0} ,D37_2[HASP_KEY_NUM] = {0} , D37[HASP_KEY_NUM] = {0} ;
	uint32_t conv_num = 10;
	uint32_t Target = 800;

	uint32_t ctrl_shadow = hasp_drv_get_ctrl();
	uint32_t nsck_shadow = hasp_drv_get_nsck();

	h6_imbalance_cali_prepare_self_check(); /* Reuse SELF_CHECK as the per-key result map for 04 10 D4. */

	h6_offset_cali_apply_gain_from_flash(); /* Run offset calibration with persisted gains, or the default when none exist. */
	h6_hal_reg_multi_write(CONGFIG_REGC , 0x83 , g_hasp_key_num);

	for(j = 0; j< g_hasp_key_num ; j++)
	{
		min_d1[j] = 0xffffffff;
		min_d2[j] = 0xffffffff;
	}

	for(k = 0; k < 1 ; k++)
	{
		/**0xF=0x04，0x10=0x20，连续转换时间配置为40us**/
		/**0xF = 0x04, 0x10 = 0x20, continuous conversion time set to 40 µs**/
		h6_hal_reg_multi_write(CONGFIG_REGF , 0x0A , g_hasp_key_num);
		h6_hal_reg_multi_write(CONGFIG_REG10 , 0x20 , g_hasp_key_num);

		/**使能连续转换**/
		/**Enable continuous conversion**/
		h6_hal_reg_multi_write(CONGFIG_REG33, 0x80 , g_hasp_key_num);

		/**0x32=0x4f**/
		h6_hal_reg_multi_write(CONGFIG_REG32 , 0x4F , g_hasp_key_num);

		/**0x26=0x0,0x27=0x0**/
		h6_hal_reg_multi_write(CONGFIG_REG26 , 0x00 , g_hasp_key_num);
		h6_hal_reg_multi_write(CONGFIG_REG27 , 0x00 , g_hasp_key_num);

		/**0x29=0x1，0x2a=0x0**/
		h6_hal_reg_multi_write(CONGFIG_REG29 , 0x01 , g_hasp_key_num);
		h6_hal_reg_multi_write(CONGFIG_REG2A , 0x00 , g_hasp_key_num);

		/**0x3b=0x86，0x3b=0x00，延时1ms，遍历0x36[5:0]**/
		/**0x3b=0x86, 0x3b=0x00, 1 ms delay, iterate through 0x36[5:0]**/
		h6_hal_reg_multi_write(TESTENCTRL , 0x86 , g_hasp_key_num);
		h6_hal_reg_multi_write(TESTENCTRL , 0x00 , g_hasp_key_num);
		debug("*********************************************************************\n");

		wdt_reset_timer();//H6_OFFSET_CALI_FEED_WDT();
		ry_delay_ms(1);

		/**每变更一次延时1ms，转换10次求平均值，记录两个转换平均值最接近D_buffer的0x36寄存器配置，
		一个是从正方向最接近D_buffer的0x36配置，记为C36_1，另一个是从负方向最接近D_buffer的0x36配置，记为C36_2**/
		/**Introduce a 1 ms delay between each change; perform 100 conversions and calculate the average;
		 * record the two configurations of register 0x36 in D_buffer whose averages are closest to D_buffer,
        one being the configuration of 0x36 in D_buffer closest from the positive direction, denoted as C36_1,
        and the other being the configuration of 0x36 in D_buffer closest from the negative direction, denoted as C36_2**/
		for(i = 0; i <= 0x3F ; i++)
		{
			h6_hal_reg_multi_write(CONGFIG_REG36 , i , g_hasp_key_num);
			h6_hal_reg_multi_write(CONGFIG_REG13 , 0x7f , g_hasp_key_num);
			ry_delay_us(200);
			if(get_H6_convert_avg(avg1,conv_num))
			{
				ret = H6_IMBALANCE_CALI_RET_REG36_POS_CRC;	// Error occurred during conversion (CRC error)
				break;
			}
			h6_hal_reg_multi_write(CONGFIG_REG13 , 0x00 , g_hasp_key_num);
			ry_delay_us(200);
			if(get_H6_convert_avg(avg2,conv_num))
			{
				ret = H6_IMBALANCE_CALI_RET_REG36_NEG_CRC;	// Error occurred during conversion (CRC error)
				break;
			}

			for(j = 0; j < g_hasp_key_num ; j++)
			{
				if(chain_miss_buff_reg[j])
				{
					if(avg1[j] > avg2[j])
					{
						if((avg1[j] - avg2[j]) < min_d1[j])
						{
							min_d1[j] = (avg1[j] - avg2[j]);
							C36_1[j] = i;
						}
					}
					else
					{
						if((avg2[j] - avg1[j]) < min_d2[j])
						{
							min_d2[j] = (avg2[j] - avg1[j]);
							C36_2[j] = i;
						}
					}
				}
			}
		}

		if(ret)
		{
			break;
		}

		for(j = 0; j < g_hasp_key_num ; j++)
		{
			if(chain_miss_buff_reg[j])
			{
				if(min_d1[j] == 0xffffffff)
				{
					C36_1[j] = C36_2[j];
				}
				if(min_d2[j] == 0xffffffff)
				{
					C36_2[j] = C36_1[j];
				}
			}
			else
			{
				continue;
			}
		}

		/** Configure register 0x36 as C36_1 and delay for 1ms. **/
//		h6_drv_wr_reg_time_inst();
		h6_drv_write_reg_cmd(CONGFIG_REG36, C36_1 , g_hasp_key_num);
		h6_drv_read_reg_cmd(CONGFIG_REG36, g_hasp_key_num);
		for(j = 0; j < g_hasp_key_num ; j++)
		{
			min_d1[j] = 0xffffffff;
			min_d2[j] = 0xffffffff;
		}

		wdt_reset_timer();//H6_OFFSET_CALI_FEED_WDT();
		ry_delay_ms(1);

		/** 遍历0x37[5:0]，每变更一次延时1ms，转换10次求平均值，均值最接近D_buffer的0x37配置记录下来，记为C37_1，
		并把对应的转换值与D_buffer的差值的绝对值记录下来，记为D37_1 **/
		/** Iterate through 0x37[5:0], introducing a 1 ms delay between each change;
		 * perform 100 conversions and calculate the average; record the 0x37 configuration whose average value is closest to that in D_buffer, denoting it as C37_1,
        and record the absolute value of the difference between the corresponding converted value and the value in D_buffer, denoting it as D37_1 **/
		for(i = 0; i <= 0x3F ; i++)
		{
			h6_hal_reg_multi_write(CONGFIG_REG37 , i , g_hasp_key_num);
			h6_hal_reg_multi_write(CONGFIG_REG13 , 0x7f , g_hasp_key_num);
			ry_delay_us(200);
			if(get_H6_convert_avg(avg1,conv_num))
			{
				ret = H6_IMBALANCE_CALI_RET_REG37_1_POS_CRC;	// Error occurred during conversion (CRC error)
				break;
			}
			h6_hal_reg_multi_write(CONGFIG_REG13 , 0x00 , g_hasp_key_num);
			ry_delay_us(200);
			if(get_H6_convert_avg(avg2,conv_num))
			{
				ret = H6_IMBALANCE_CALI_RET_REG37_1_NEG_CRC;	// Error occurred during conversion (CRC error)
				break;
			}

			for(j = 0; j < g_hasp_key_num ; j++)
			{
				if(chain_miss_buff_reg[j])
				{
					if(avg1[j] > avg2[j])
					{
						D37_1[j] = avg1[j] - avg2[j];
					}
					else
					{
						D37_1[j] = avg2[j]- avg1[j];
					}
					if(D37_1[j] < min_d1[j])
					{
						min_d1[j] = D37_1[j];
						C37_1[j] = i;
					}
				}
			}
		}
		for(j = 0; j < g_hasp_key_num ; j++)
		{
			if(chain_miss_buff_reg[j])
			{
				D37_1[j] =  min_d1[j];
			}
		}

		if(ret)
		{
			break;
		}

		/** Configure register 0x36 as C36_2 and delay for 1ms **/
		h6_drv_write_reg_cmd(CONGFIG_REG36, C36_2 , g_hasp_key_num);
		h6_drv_read_reg_cmd(CONGFIG_REG36, g_hasp_key_num);
		for(j = 0; j < g_hasp_key_num ; j++)
		{
			min_d1[j] = 0xffffffff;
			min_d2[j] = 0xffffffff;
		}

		wdt_reset_timer();//H6_OFFSET_CALI_FEED_WDT();
		ry_delay_ms(1);

		/** 遍历0x37[5:0]，每变更一次延时1ms，转换10次求平均值，均值最接近D_buffer的0x37配置记录下来，
		记为C37_2，并把对应的转换值与D_buffer的差值的绝对值记录下来，记为D37_2 **/
		/** Iterate through 0x37[5:0], introducing a 1 ms delay between each change;
		 * perform 100 conversions and calculate the average; record the 0x37 configuration whose average value is closest to that in D_buffer,
        denoted as C37_2, and record the absolute value of the difference between the corresponding converted value and the value in D_buffer, denoted as D37_2 **/
		for(i = 0; i <= 0x3F ; i++)
		{
			h6_hal_reg_multi_write(CONGFIG_REG37 , i , g_hasp_key_num);
			h6_hal_reg_multi_write(CONGFIG_REG13 , 0x7f , g_hasp_key_num);
			ry_delay_us(200);
			if(get_H6_convert_avg(avg1,conv_num))
			{
				ret = H6_IMBALANCE_CALI_RET_REG37_2_POS_CRC;		// Error occurred during conversion (CRC error)
				break;
			}
			h6_hal_reg_multi_write(CONGFIG_REG13 , 0x00 , g_hasp_key_num);
			ry_delay_us(200);
			if(get_H6_convert_avg(avg2,conv_num))
			{
				ret = H6_IMBALANCE_CALI_RET_REG37_2_NEG_CRC;	// Error occurred during conversion (CRC error)
				break;
			}

			for(j = 0; j < g_hasp_key_num ; j++)
			{
				if(chain_miss_buff_reg[j])
				{
					if(avg1[j] > avg2[j])
					{
						D37_2[j] = avg1[j] - avg2[j];
					}
					else
					{
						D37_2[j] = avg2[j]- avg1[j];
					}
					if(D37_2[j] < min_d2[j])
					{
						min_d2[j] = D37_2[j];
						C37_2[j] = i;
					}
				}
			}
		}
		for(j = 0; j < g_hasp_key_num ; j++)
		{
			if(chain_miss_buff_reg[j])
			{
				D37_2[j] = min_d2[j];
			}
		}
	}

	/* Only if ret == 0 is the final D37 selection and target check performed */
	if (ret != H6_IMBALANCE_CALI_RET_OK)
	{
	    hasp_drv_nsck_set(nsck_shadow);
	    hasp_drv_set_ctrl(ctrl_shadow);
	    h6_imbalance_cali_finish_self_check(ret);
	    debug("ret=%d\r\n", ret);
	    return ret;
	}

	wdt_reset_timer();//H6_OFFSET_CALI_FEED_WDT();

	/** 比较D37_1和D37_2，选择值更小的记为D37，把相应的C36_1/C36_2记为C36,
		以及C37_1/C37_2记为C37，并把C36和C37作为最终校准值写入寄存器0x36和0x37 **/
	/** Compare D37_1 and D37_2, select the smaller value and denote it as D37, denote the corresponding C36_1/C36_2 as C36,
        and C37_1/C37_2 as C37, and write C36 and C37 as the final calibration values to registers 0x36 and 0x37 **/
	for(j = 0; j < g_hasp_key_num ; j++)
	{
		if(chain_miss_buff_reg[j])
		{
			if(D37_1[j] < D37_2[j])
			{
				D37[j] = D37_1[j];
				C36[j] = C36_1[j];
				C37[j] = C37_1[j];
			}
			else
			{
				D37[j] = D37_2[j];
				C36[j] = C36_2[j];
				C37[j] = C37_2[j];
			}
		}
	}

	for(j = 0; j < g_hasp_key_num ; j++)
	{
		if(chain_miss_buff_reg[j])
		{
			if(D37[j] <= Target)
			{
				debug("hall chain->%d:%d REG36 = 0x%02x	 REG37 = 0x%02x	%d(<=%d) Pass\r\n", j%HASP_CHANNL_NUM,j/HASP_CHANNL_NUM,C36[j],C37[j],D37[j] , Target);
			}
			else
			{
				ret = H6_IMBALANCE_CALI_RET_LIMIT_FAIL;	// Imbalance calibration deviation is greater than the target value
				h6_imbalance_cali_mark_self_check_error(j); /* Report this over-target Hall through the SELF_CHECK data type. */
				debug("hall chain->%d:%d REG36 = 0x%02x	 REG37 = 0x%02x	%d(>%d) NG\r\n", j%HASP_CHANNL_NUM,j/HASP_CHANNL_NUM,C36[j],C37[j],D37[j] , Target);
			}
		}
		else
		{
			debug("hall chain->%d:%d it is hall null!\r\n", j%HASP_CHANNL_NUM,j/HASP_CHANNL_NUM);
		}
	}

	wdt_reset_timer();//H6_OFFSET_CALI_FEED_WDT();

	if (ret == H6_IMBALANCE_CALI_RET_OK)
	{
		h6_drv_write_reg_cmd(CONGFIG_REG36, C36 , g_hasp_key_num);
		h6_drv_write_reg_cmd(CONGFIG_REG37, C37 , g_hasp_key_num);

		unsigned int flash_ret = h6_imbalance_cali_save_to_flash(C36, C37); /* Persist successful REG36/REG37 trims for the next boot. */

		hasp_drv_nsck_set(nsck_shadow);
		hasp_drv_set_ctrl(ctrl_shadow);

		if (flash_ret != 0U)
		{
			printf("imbalance save failed ret=%u\r\n", flash_ret); /* Keep calibration result valid but expose Flash failure in logs. */
		}
	}

	h6_imbalance_cali_finish_self_check(ret); /* Finalize the shared SELF_CHECK state for command readback. */
	return ret;
}


#endif

