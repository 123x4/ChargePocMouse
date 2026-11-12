/*
 * mfg_mode.c
 *
 *  Created on:
 *      Author:
 */
#include "app_config.h"

#if MFG_EXT_MODE_ENABLE

#include "h6_hw_tab.h"
#include "mfg_mode.h"
#include "h6_hal.h"
#include "h6_offset_cali.h"
#include "gtry_trng_drv.h"
#include "h6_drv.h"
#include "hasp_drv.h"
#include "gtry_gpio_drv.h"
#include "gtry_smu.h"
#include "tusb.h"
#include "keyboard.h"
#include "TFT.h"
#include "usb_descriptors.h"
#include "Puya_Flash.h"
#include "gtry_timer_drv.h"
#include "gtry_wdt_drv.h"
#include "gtry_qspi_flash_drv.h"
#include "gtry6xx_flash_lib.h"
#include "led_rgb.h"
#include "wheel.h"
#include "h6_hal.h"
#include "led_rgb.h"
#include "h6_hal.h"

/*--------------------------------------------------------------------+
 * Global Variables
 *--------------------------------------------------------------------*/
unsigned char vendor_buf_in[CFG_TUD_HID_EP_BUFSIZE]	= {0x0};
unsigned char vendor_buf_out[CFG_TUD_HID_EP_BUFSIZE]	= {0x0};

unsigned char mfg_sign[8];

unsigned char mfg_mode_cmd = 1;

uint16_t H6_rddata_1[HASP_KEY_NUM] = {0};

volatile uint8_t  report_active    = 0;//report switch on or off
volatile uint8_t  report_cmd_type  = 0;
volatile uint8_t  report_data_type = 0;//date type eg.`HALL_DATA`=0x00  `Seg255`=0x01
volatile uint8_t  report_ready     = 0;//80ms ready flag
static   uint32_t report_tick_cnt  = 0;//timer tick

uint8_t SN[SN_LEN];

unsigned short hall_data_average[HASP_KEY_NUM];
unsigned char  hall_data_noise[HASP_KEY_NUM];
unsigned short hall_data_max[HASP_KEY_NUM];
unsigned short hall_data_min[HASP_KEY_NUM];
uint32_t       hall_data_sum[HASP_KEY_NUM];

unsigned char buf_mfg_TBL_flash[FLASH_DATA_PAGESIZE] = {0x0};//top baseline value for H6 in internal flash.
unsigned char buf_mfg_BBL_flash[FLASH_DATA_PAGESIZE] = {0x0};//bottom raw data of HASP in internal flash.
#define REPORT_TICK_COUNT       		 10//80 //80 160ms
#define HALL_DATA_AND_NOISE_SAMPLE_COUNT 500U
#define MFG_HALL_REPORT_VALUE_DEBUG      0  /* Set to 1 only for short UART debug; per-key printf can block USB/WDT. */

extern volatile unsigned char my_sys_freq;

#define TIMER1_CC1_INT_FLAG  (TIMER_INT_CC1_MASK << (TC_NUMBER1 * TIMER_CC_NUM))

extern uint8_t data_report_done;
extern uint8_t kb_report_done;

unsigned char g_gain_value[HASP_KEY_NUM] = {0x0};//GAIN Value

uint16_t mfg_usb_vendor_report_length(void)
{
#if TUD_OPT_HIGH_SPEED
	if (tud_speed_get() == TUSB_SPEED_HIGH)
	{
		return MFG_USB_VENDOR_REPORT_LENGTH;
	}
#endif

	return MFG_USB_VENDOR_REPORT_LENGTH_FS;
}

static uint8_t hall_gain_sanitize(uint8_t raw_gain);
static void hall_gain_log_entry(const char *tag, uint8_t key_index, uint8_t keycode, uint8_t gain);
static void hall_gain_build_reg_buffer(uint16_t *gain_buf, uint32_t size);

#ifndef MFG_FLASH_IRQ_GUARD_USB
#define MFG_FLASH_IRQ_GUARD_USB    1            /* Set to 1 alone to check whether USB IRQ preemption breaks Flash programming. */
#endif

static unsigned int mfg_flash_irq_usb_saved;

static void mfg_flash_irq_guard_enter(void)
{
	#if MFG_FLASH_IRQ_GUARD_USB
		mfg_flash_irq_usb_saved = __nds__plic_get_interrupt(IRQ_USB_SOURCE);
		if (mfg_flash_irq_usb_saved) { __nds__plic_disable_interrupt(IRQ_USB_SOURCE); }
	#endif
}

static void mfg_flash_irq_guard_exit(void)
{
	#if MFG_FLASH_IRQ_GUARD_USB
		if (mfg_flash_irq_usb_saved) { __nds__plic_enable_interrupt(IRQ_USB_SOURCE); }
	#endif
}

static uint8_t mfg_volume_index_sanitize(uint8_t raw_index)
{
	if (raw_index >= VOLUME_MAX)
	{
		/* Clamp the host-provided index before indexing LedVolumeTable to avoid
		 * an out-of-bounds read and a possible RISC-V fault/reset.
		 */
		return (uint8_t)(VOLUME_MAX - 1U);
	}

	return raw_index;
}

static unsigned int mfg_flash_write_trace(const char *tag, unsigned int flash_addr, const uint8_t *data, unsigned int size)
{
	unsigned int ret;

	/* Print both sides of every manufacturing Flash write so the last UART
	 * line shows whether reset happens inside QSPI_EXT_FLASH_write().
	 */
	mfg_flash_irq_guard_enter();
	ret = QSPI_EXT_FLASH_write(flash_addr, data, size);
	mfg_flash_irq_guard_exit();

	return ret;
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: send_vendor
 * Description  	: Send the current vendor HID report after the previous USB IN transfer has completed.
 * END ****************************************************************************************************************/
void send_vendor(void)
{
    while(data_report_done == 0)
    {
        tud_task();
        ry_delay_us(5);
    }
    data_report_done = 0;
    tud_hid_n_report(1, 0, vendor_buf_in, VENDOR_REPORT_SIZE);
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: report_timer1_callback
 * Description  	: Timer1 CC1 interrupt callback used to generate the periodic manufacturing report trigger.
 * END ****************************************************************************************************************/
static void report_timer1_callback(uint32 int_status)
{
	if (!(int_status & TIMER1_CC1_INT_FLAG))
		return;

	/* Data transmission is slower in test mode to prevent the testing software
	 * from using too much memory and causing the computer to slow down */
	if (report_active)
	{
		if (++report_tick_cnt >= REPORT_TICK_COUNT)
		{
			report_tick_cnt = 0;
			report_ready = 1;
		}
		timer_trigger_clear(TC_NUMBER1);
		timer_start(TC_NUMBER1);
	}
}

static const uint32_t report_timer1_compare[3] = {
/*  200MHz     96MHz      48MHz   */
    10000,     4800,      2400
};

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: report_timer1_init
 * Description  	: Initialize Timer1/CC1 as the 1 ms scheduler for manufacturing report timing.
 * END ****************************************************************************************************************/
void report_timer1_init(void)
{
	timer_init(TC_NUMBER1, 20, RY_TIMER_TIMING, RY_TIMER_16BIT);

	timer_set_ccn(TC_NUMBER1, TIMER_CC1, report_timer1_compare[my_sys_freq]);

	timer_set_shorts(TC_NUMBER1, TIMER_CC1, TIMER_SHORT_CC_STOP_MASK);

	/* Register Timer1 through the common timer callback table instead of the
	 * old single Timer1 callback symbol, so timer_irq_handler() dispatches it
	 * from timer_drv_int_callback[TC_NUMBER1]. */
	timer_int_init(TC_NUMBER1, TIMER_INT_CC1_MASK, report_timer1_callback);
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: hall_seg4000_to_255
 * Description  	: Convert a 0~4000 hall segment reading into a rounded 0~255 report value.
 * END ****************************************************************************************************************/
static uint8_t hall_seg4000_to_255(uint32_t seg4000)
{
	seg4000 &= 0x0FFF;
	return (uint8_t)(((uint32_t)seg4000 * 255U + 2000U) / 4000U);
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: config_hasp_key_td
 * Description  	: Once other sequences have been configured, you will need to switch to the key-scanning sequence
 * END ****************************************************************************************************************/
void config_hasp_key_td()
{
	hasp_drv_stop_work();
	ry_delay_us(5);

	h6_hal_variable_time_inst(0);//hall variable timing instruction
	hasp_drv_start_work();
	ry_delay_us(5);
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: hal_hasp_baseline_on_off
 * Description  	: Enable or disable H6 baseline mode during factory calibration sampling.
 * END ****************************************************************************************************************/
void hal_hasp_baseline_on_off(bool on_off)
{
	hasp_drv_stop_work();
	ry_delay_us(5);

	if(on_off)
	{
		/* Re-enable BASL_EN in REG33
		 * REG33 = 0x94: bit7=1 BASL_EN, bit4=continuous, bit2=BASL tracking
		 * Now H6 uses 0x2D/0x2E value as baseline -> output 0~8000 */
		h6_hal_reg_multi_write(CONGFIG_REG33, 0x94, g_hasp_key_num);
	}
	else
	{
		//H6 0x33 REG BIT2 baseline mode off
		h6_hal_reg_multi_write(CONGFIG_REG33, 0x90, HASP_KEY_NUM);
	}

	hasp_drv_td_clear();			//Clear HASP TD time
	h6_hal_variable_time_inst(0);//hall variable timing instruction

	/* Step 3: Start HASP to get one round of ADC data */
	hasp_drv_start_work();
	ry_delay_us(5);
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: hall_read_value
 * Description  	: Read one hall-related value for the specified key according to the requested data type.
 * Return       	: Selected manufacturing data value for the current key.
 * END ****************************************************************************************************************/
static unsigned int hall_read_value(uint8_t data_type, uint8_t key_idx)
{
	switch (data_type)
	{
		case Seg255:	  return hall_seg4000_to_255((uint16_t)GTRY_HASP->RXLNDATA[key_idx]);
		case SELF_CHECK:  return hall_hal_get_self_check_value(key_idx); /* Report the cached H6 self-check result instead of the raw clock count. */
		case HALL_DATA:
		#if RAW_DATA_DEBUG
			hal_hasp_baseline_on_off(H6_0X33_BIT2_BASELINE_MODE_DISABLE);//raw data for 8000-16000
		#endif
			return ((GTRY_HASP->RXRAM[key_idx]) >> 16) & 0x3FFF;//	return ((GTRY_HASP->RAWDLP[key_idx]) >> 16) & 0x3FFF;  GTRY_HASP->RXRAWDATA[key_idx] & 0x3FFF
		case Seg4096:	  return (GTRY_HASP->RXLNDATA[key_idx]);//(GTRY_HASP->RAWDLP[key_idx]) >> 16
		case TBL:         return ((GTRY_HASP->BASLTOP[key_idx]) >> 2) & 0x3FFF;
		case BBL:         return ((GTRY_HASP->BASLBOT[key_idx]) >> 2) & 0x3FFF;
		default:          return 0;
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Report_HallGeneric
 * Description  	: Package and send generic 1-byte or 2-byte per-key hall data for manufacturing reads.
 * END ****************************************************************************************************************/
static void TestAPP_Report_HallGeneric(uint8_t cmd_byte, uint8_t data_type)
{
	uint8_t buf_index = VENDOR_HEADER_SIZE;
	uint8_t keys_in_packet = 0;
	uint8_t is_2byte = (cmd_byte == BYTE1_GET_HALL_PARAM_2BYTE);
//	printf("c=%x,is=%d\r\n",cmd_byte,is_2byte);

	uint8_t keys_per_pkt = is_2byte ? KEYS_PER_PACKET_2BYTE : KEYS_PER_PACKET_1BYTE;

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = cmd_byte;
	vendor_buf_in[2] = data_type;

	for (uint8_t k = 0; k < g_hasp_key_num; k++)
	{
		uint8_t keycode = Key_HID_Usage_ID[k];
		if (keycode == 0x00) continue;
		uint16_t val = hall_read_value(data_type, k);

#if MFG_HALL_REPORT_VALUE_DEBUG
		wdt_reset_timer();
		printf("%d,%d\r\n", k, val);
		wdt_reset_timer();
		tud_task();
#endif

		vendor_buf_in[buf_index++] = keycode;
		vendor_buf_in[buf_index++] = val & 0xFF;//1Byte report

		if (is_2byte)
		{
			vendor_buf_in[buf_index++] = (val >> 8) & 0xFF;//2Byte report
		}
		keys_in_packet++;

		if (keys_in_packet >= keys_per_pkt)
		{
			send_vendor();

			memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
			vendor_buf_in[0] = BYTE0_HEADER04;
			vendor_buf_in[1] = cmd_byte;
			vendor_buf_in[2] = data_type;
			buf_index = VENDOR_HEADER_SIZE;
			keys_in_packet = 0;
		}
	}

	if (keys_in_packet > 0)
	{
		send_vendor();
	}

#if RAW_DATA_DEBUG
	if(data_type == HALL_DATA)
	{
		h6_hal_reg_multi_write(CONGFIG_REG33, 0x94, g_hasp_key_num);
		config_hasp_key_td();
	}
#endif

}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: factory_cali_read_data
 * Description  	: Sample hall data repeatedly and calculate per-key average, max/min, and noise values.
 * END ****************************************************************************************************************/
void factory_cali_read_data(uint16_t cnt)
{
	uint16_t i;
	uint8_t j;
	uint16_t temp;
	volatile uint16_t discard_sample;
	uint16_t sample_cnt = cnt;

	if (sample_cnt == 0)
	{
		sample_cnt = 1;
	}

	for(j = 0; j < g_hasp_key_num; j++)
	{
		hall_data_sum[j] = 0;
		hall_data_average[j] = 0;
		hall_data_noise[j] = 0;
		hall_data_max[j] = 0;
		hall_data_min[j] = 0;
	}

	/* After switching baseline mode and restarting HASP, discard several
	 * consecutive samples so the first reported statistics are not polluted by
	 * stale pipeline data or transient settling values. */
	for (i = 0; i < FACTORY_CALI_DISCARD_SAMPLES; i++)
	{
		ry_delay_us(FACTORY_CALI_SAMPLE_DELAY_US);

		for (j = 0; j < g_hasp_key_num; j++)
		{
			if (!chain_miss_buff_reg[j]) continue;

			discard_sample = (uint16_t)(((GTRY_HASP->RXRAM[j]) >> 16) & 0x3FFF);
			(void)discard_sample;
		}
	}

	for(i = 0; i < sample_cnt; i++)
	{
		ry_delay_us(FACTORY_CALI_SAMPLE_DELAY_US);

		for(j = 0; j < g_hasp_key_num; j++)
		{
			if (!chain_miss_buff_reg[j]) continue;

			temp = (uint16_t)(((GTRY_HASP->RXRAM[j]) >> 16) & 0x3FFF);
			hall_data_sum[j] += temp;

			if(temp > hall_data_max[j]) hall_data_max[j] = temp;
			if(temp < hall_data_min[j]) hall_data_min[j] = temp;
		}
	}

	for(j = 0; j < g_hasp_key_num; j++)
	{
		if (!chain_miss_buff_reg[j])
		{
#if FACTORY_CALI_VERBOSE_LOG_EN
			/* Keep this disabled in normal USB manufacturing flow.
			 * printf() goes through a polling UART backend, so printing one line
			 * per key can block this command handler for a long time and starve
			 * the USB task / response path. */
			printf("idx=%d valid=0 chain=%d pos=%d rxram=0x%08X hall=skip noise=skip\r\n",
			       j,
			       j % HASP_CHANNL_NUM,
			       j / HASP_CHANNL_NUM,
			       GTRY_HASP->RXRAM[j]);
#endif
			continue;
		}

		hall_data_average[j] = hall_data_sum[j] / sample_cnt;

		if (hall_data_max[j] >= hall_data_min[j])
		{
			uint16_t noise = hall_data_max[j] - hall_data_min[j];
			hall_data_noise[j] = (noise > 0xFF) ? 0xFF : (uint8_t)noise;
		}

#if FACTORY_CALI_VERBOSE_LOG_EN
		/* This log is useful for one-off deep debug,
		 * If you enable it, do not expect normal USB timing. */
		printf("idx=%d valid=%d chain=%d pos=%d rxram=0x%08X hall=%u noise=%u,seg=%d,BASLTOP=%d\r\n",
		       j,
		       chain_miss_buff_reg[j],
		       j % HASP_CHANNL_NUM,
		       j / HASP_CHANNL_NUM,
		       GTRY_HASP->RXRAM[j],
		       hall_data_average[j],
		       hall_data_noise[j],GTRY_HASP->RXLNDATA[j],GTRY_HASP->BASLTOP[j]);
#endif
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_user_cali_start
 * Description  	: 3Byte format (cmd 04 19 D3)
 *   [0]=0x04  [1]=0x19  [2]=DataType
 *   [3..62]  up to 15 keys x 4 bytes (keycode + value_L + value_H + noise)
 * END ****************************************************************************************************************/
void TestAPP_user_cali_start(uint8_t order)
{
	uint8_t buf_index = VENDOR_HEADER_SIZE;
	uint8_t keys_in_packet = 0;
	unsigned int hall_value = 0xFFFFFFFF;

	if (order == HALL_DATA_AND_NOISE)
	{
		factory_cali_read_data(HALL_DATA_AND_NOISE_SAMPLE_COUNT);
	}

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = BYTE1_GET_HALL_PARAM_3BYTE;
	vendor_buf_in[2] = order;

	for (uint8_t k = 0; k < g_hasp_key_num; k++)
	{
		uint8_t keycode = Key_HID_Usage_ID[k];
		if (keycode == 0x00) continue;

		vendor_buf_in[buf_index++] = keycode;
		if (order == HALL_DATA_AND_NOISE)
		{
			hall_value = hall_data_average[k];
			vendor_buf_in[buf_index++] =  hall_value & 0xFF;
			vendor_buf_in[buf_index++] = (hall_value >> 8) & 0xFF;
			vendor_buf_in[buf_index++] = hall_data_noise[k];
		}
		keys_in_packet++;

		if (keys_in_packet >= KEYS_PER_PACKET_3BYTE)
		{
			send_vendor();

			memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
			vendor_buf_in[0] = BYTE0_HEADER04;
			vendor_buf_in[1] = BYTE1_GET_HALL_PARAM_3BYTE;
			vendor_buf_in[2] = order;
			buf_index = VENDOR_HEADER_SIZE;
			keys_in_packet = 0;
		}
	}

	if (keys_in_packet > 0)//last packet
	{
		send_vendor();
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_set_factory_cali
 * Description  	: Run factory calibration sampling and report the averaged hall/noise result to the host.
 * END ****************************************************************************************************************/
void TestAPP_set_factory_cali(uint8_t order,uint8_t count)
{
	uint8_t  buf_index = VENDOR_HEADER_SIZE;
	uint8_t  keys_in_packet = 0;
	uint16_t top_rawdata[HASP_KEY_NUM]={0};
	uint16_t bottom_rawdata[HASP_KEY_NUM]={0};

	mfg_flash_irq_guard_enter();

	if (order == Hall_RELEASE_SAMPLING_AND_CALI)
	{
		/*  Disable BASL_EN (bit7=1, bit4=1 keeps continuous mode,
		 * but bit3=0 turns off baseline tracking)
		 * REG33 = 0x90: bit7=BASL_EN off sense, bit4=continuous,
		 * BASL tracking disabled -> H6 outputs raw ADC 8000~16000 */
		hal_hasp_baseline_on_off(H6_0X33_BIT2_BASELINE_MODE_DISABLE);

		/* Read each key's top rawdata (14-bit) from RXRAWDATA register */
		for (uint8_t k = 0; k < g_hasp_key_num; k++)
		{
			top_rawdata[k] = GTRY_HASP->RXRAWDATA[k] & 0x3FFF;
//			info("top_rawdata[%d] = %d\r\n" ,k,top_rawdata[k]);
		}

		/* Stop HASP again before writing H6 registers */
		hasp_drv_stop_work();
		ry_delay_us(5);

		/* Write top rawdata into H6 REG 0x2D (low) and 0x2E (high)
		 * This sets the manual baseline initial value for each key */
		hall_hal_write_top_baseline(top_rawdata, g_hasp_key_num);

		/* Also save to buf_mfg_TBL_flash for later Flash storage by 0xE3 */
		for (uint8_t k = 0; k < g_hasp_key_num; k++)
		{
			buf_mfg_TBL_flash[k * 2]     = top_rawdata[k] & 0xFF;
			buf_mfg_TBL_flash[k * 2 + 1] = (top_rawdata[k] >> 8) & 0xFF;
		}

		hal_hasp_baseline_on_off(H6_0X33_BIT2_BASELINE_MODE_ENABLE);
	}
	else if(order == Hall_PRESS_SAMPLING_AND_CALI)
	{
		for (uint8_t k = 0; k < g_hasp_key_num; k++)
		{
			bottom_rawdata[k] = GTRY_HASP->RXRAWDATA[k] & 0x3FFF;
//			info("bottom_rawdata[%d] = %d\r\n" ,k,bottom_rawdata[k]);

			buf_mfg_BBL_flash[k * 2]     = bottom_rawdata[k] & 0xFF;
			buf_mfg_BBL_flash[k * 2 + 1] = (bottom_rawdata[k] >> 8) & 0xFF;
		}
	}

	/* (D1 and D2): Sample Cali_num times, compute average and noise */
//	GTRY6xx_SHOW_DBG_H_0;
	factory_cali_read_data(count);
//	GTRY6xx_SHOW_DBG_L_0;

	/* Report calibration result per key: (Keycode + Hall_L + Hall_H + Noise) */
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = BYTE1_CALI;
	vendor_buf_in[2] = order;

	for (uint8_t k = 0; k < g_hasp_key_num; k++)
	{
		uint8_t keycode = Key_HID_Usage_ID[k];
		if (keycode == 0x00) continue;

		vendor_buf_in[buf_index++] = keycode;
		vendor_buf_in[buf_index++] = (uint8_t)(hall_data_average[k]);
		vendor_buf_in[buf_index++] = (uint8_t)(hall_data_average[k] >> 8);
		vendor_buf_in[buf_index++] = hall_data_noise[k];
		keys_in_packet++;

		if (keys_in_packet >= KEYS_PER_PACKET_3BYTE)
		{
			send_vendor();

			memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
			vendor_buf_in[0] = BYTE0_HEADER04;
			vendor_buf_in[1] = BYTE1_CALI;
			vendor_buf_in[2] = order;
			buf_index = VENDOR_HEADER_SIZE;
			keys_in_packet = 0;
		}
	}

	mfg_flash_irq_guard_exit();

	if (keys_in_packet > 0)
	{
		send_vendor();
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: mfg_dump_saved_cali_flash
 * Description  	: Read back saved calibration data from external Flash and print each value for debug checking.
 * END ****************************************************************************************************************/
#if 0
static void mfg_dump_saved_cali_flash(const char *tag, uint32_t flash_addr, uint16_t key_num)
{
	/* Use one local page-sized buffer so we can read the saved bytes back
	 * without touching the original source buffer. */
	uint8_t flash_buf[FLASH_DATA_PAGESIZE] = {0};

	/* Each saved calibration item uses 2 bytes in little-endian order:
	 * low byte first, high byte second. */
	uint16_t read_len = key_num * 2;
	uint16_t actual_read;

	/* Guard the local buffer size. This also prevents out-of-bounds access if
	 * key_num is larger than what one page-sized debug dump can hold. */
	if (read_len > FLASH_DATA_PAGESIZE)
	{
		read_len = FLASH_DATA_PAGESIZE;
	}

	/* QSPI_EXT_FLASH_read() returns the actual number of bytes read, not a
	 * boolean status code. A full success means actual_read == read_len. */
	actual_read = QSPI_EXT_FLASH_read(flash_addr, flash_buf, read_len);
	if (actual_read != read_len)
	{
		info("%s flash read incomplete, addr=0x%X expect=%u actual=%u\r\n",
		     tag,
		     flash_addr,
		     read_len,
		     actual_read);
		return;
	}

	/* Print a short banner so the start/end of the dump is easy to find in the
	 * UART log when many manufacturing messages are mixed together. */
	info("------ %s flash dump start (addr=0x%X, keys=%u, bytes=%u) ------\r\n",
	     tag,
	     flash_addr,
	     key_num,
	     read_len);

	/* Reconstruct each 16-bit value from the little-endian byte pair that was
	 * written into Flash, then print both decimal and hex representations. */
	for (uint16_t i = 0; i < key_num; i++)
	{
		uint16_t value = (uint16_t)flash_buf[i * 2] |
		                ((uint16_t)flash_buf[i * 2 + 1] << 8);
		info("%s[%u] = %u (0x%04X)\r\n", tag, i, value, value);
	}

	/* Print a closing banner so the full dump block is visually complete. */
	info("------ %s flash dump end ------\r\n", tag);
}
#endif

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_cali_BBL_end
 * Description  	: Bottom baseline calibration end (0xE2): save bottom rawdata to Flash.
 * @param order     START_HASP_BOTTOM_CALI_SAVE (0xE2)
 * @param save_flag 0x01 = save to Flash, 0x00 = discard
 * END ****************************************************************************************************************/
void TestAPP_cali_BBL_end(uint8_t order, uint8_t save_flag)
{
	uint16_t write_len = g_hasp_key_num * 2;

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = BYTE1_CALI;
	vendor_buf_in[2] = order;

	if (save_flag == 0x01)
	{
		/* Save the bottom calibration payload first, then read the same Flash
		 * region back immediately so the UART log shows what was actually
		 * stored in non-volatile memory. */
//		QSPI_EXT_FLASH_erase_sector(BBL_CALI_ADDR);
		(void)mfg_flash_write_trace("BBL", BBL_CALI_ADDR, buf_mfg_BBL_flash, write_len);
		/* read the same Flash region back immediately so the UART log shows what was actually
		 * stored in non-volatile memory.*/
//		mfg_dump_saved_cali_flash("BBL", BBL_CALI_ADDR, g_hasp_key_num);
		vendor_buf_in[3] = 0x01;
	}
	else
	{
		vendor_buf_in[3] = 0x00;
	}

	send_vendor();

//	mfg_dump_saved_cali_flash("BBL", BBL_CALI_ADDR, g_hasp_key_num);
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_cali_TBL_end
 * Description  	:  When save_flag == 0x01, write buf_mfg_TBL_flash (filled during 0xD2) to
 *  external Flash at TBL_CALI_ADDR.  On next power-up, h6_hal_reg_set_mode()
 *  will load this data and write it to H6 REG 0x2D/0x2E as manual baseline.
 * 
 *  When save_flag == 0x00, calibration failed, do not save.
 * 
 * @param order     START_HALL_TBL_CALI_SAVE (0xE3)
 * @param save_flag 0x01 = save to Flash, 0x00 = discard
 * END ****************************************************************************************************************/
void TestAPP_cali_TBL_end(uint8_t order, uint8_t save_flag)
{
	unsigned int write_len = g_hasp_key_num * 2;

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = BYTE1_CALI;
	vendor_buf_in[2] = order;

	if (save_flag == 0x01)
	{
		/* Erase the 4KB sector at TBL_CALI_ADDR before writing */
//		QSPI_EXT_FLASH_erase_sector(TBL_CALI_ADDR);

		/* Write top rawdata (little-endian byte pairs) to external Flash */
		(void)mfg_flash_write_trace("TBL", TBL_CALI_ADDR, buf_mfg_TBL_flash, write_len);

		/* Read the just-written top baseline data back from Flash and print it.
		 * This verifies the persistent TBL payload instead of only trusting the
		 * RAM buffer that existed before the write. */
//		mfg_dump_saved_cali_flash("TBL", TBL_CALI_ADDR, g_hasp_key_num);

		vendor_buf_in[3] = 0x01; /* ACK: save success */
	}
	else
	{
		vendor_buf_in[3] = 0x00; /* ACK: not saved */
	}

	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Set_PWM
 * Description  	: Set the RGB PWM duty and period used by the LED manufacturing test command.
 * END ****************************************************************************************************************/
void TestAPP_Set_PWM(uint8_t order)
{
	uint8_t R_PWM 		= vendor_buf_out[2];
	uint8_t G_PWM		= vendor_buf_out[3];
	uint8_t B_PWM 		= vendor_buf_out[4];
	uint8_t volume_index = mfg_volume_index_sanitize(vendor_buf_out[5]);
	uint16_t PWM_period  = LedVolumeTable[volume_index];
//	smu_global_int_disable();

//	hasp_drv_stop_work();
//	h6_hal_pwm_multi_write(H6_PWM_TOTAL_PERIOD, PWM_period, g_hasp_key_num);
	Set_RGB(R_PWM,G_PWM,B_PWM);

	if(PWM_period == 0)
	{
		Set_RGB(0,0,0);
	}

	h6_drv_w_pwm_time_clr();

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;
	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: led_bin_wb_flash_read
 * Description  	: Read the combined LED bin and white-balance payload from external Flash and
 *                  validate that the full RGB_pwm structure was returned.
 * Return       	: Actual number of bytes read from LED_BIN_WB_ADDR.
 * END ****************************************************************************************************************/
static unsigned int led_bin_wb_flash_read(RGB_pwm *rgb_pwm)
{
	unsigned int read_len = QSPI_EXT_FLASH_read(LED_BIN_WB_ADDR, (uint8_t *)rgb_pwm, sizeof(*rgb_pwm));

	if (read_len != sizeof(*rgb_pwm))
	{
		info("ERROR: LED/WB flash read failed, addr=0x%X, read_len=%u, expected=%u\r\n",
		     LED_BIN_WB_ADDR,
		     read_len,
		     (unsigned int)sizeof(*rgb_pwm));
	}

	return read_len;
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: led_bin_wb_flash_write
 * Description  	: Write the combined LED bin and white-balance payload to external Flash and
 *                  report write failures through the debug log.
 * Return       	: 0 when the write succeeds, otherwise the Flash driver error code.
 * END ****************************************************************************************************************/
static unsigned int led_bin_wb_flash_write(const RGB_pwm *rgb_pwm)
{
	unsigned int ret = mfg_flash_write_trace("LED_WB", LED_BIN_WB_ADDR, (const uint8_t *)rgb_pwm, sizeof(*rgb_pwm));

	if (ret != 0)
	{
		info("ERROR: LED/WB flash write failed, addr=0x%X, ret=%u\r\n",
		     LED_BIN_WB_ADDR,
		     ret);
	}

	return ret;
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Set_led_bin
 * Description  	: Save the LED bin parameter received from the manufacturing host.
 * END ****************************************************************************************************************/
void TestAPP_Set_led_bin(uint8_t order)
{
	RGB_pwm flash_rgb_pwm;

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;

	LED_BIN = vendor_buf_out[2];//g_RGB_pwm.led_bin
//	info("led_bin = %x\r\n",LED_BIN);

	if (led_bin_wb_flash_read(&flash_rgb_pwm) == sizeof(flash_rgb_pwm))
	{
		flash_rgb_pwm.led_bin = LED_BIN;

		if (led_bin_wb_flash_write(&flash_rgb_pwm) == 0)
		{
			g_RGB_pwm = flash_rgb_pwm;
		}
	}

	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Get_led_bin
 * Description  	: Read back the saved LED bin parameter and report it to the manufacturing host.
 * END ****************************************************************************************************************/
void TestAPP_Get_led_bin(uint8_t order)
{
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;

	if (led_bin_wb_flash_read(&g_RGB_pwm) == sizeof(g_RGB_pwm))
	{
		vendor_buf_in[2] = LED_BIN;
	}

	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Set_white_balance
 * Description  	: Program LED white-balance current values and optionally save them to Flash.
 * END ****************************************************************************************************************/
void TestAPP_Set_white_balance(uint8_t order,uint8_t save_flag)
{
	RGB_pwm flash_rgb_pwm;

	R_CUR	= vendor_buf_out[2];
	G_CUR	= vendor_buf_out[3];
	B_CUR	= vendor_buf_out[4];

	h6_hal_reg_multi_write(PWM1_CURR_REG, R_CUR, g_hasp_key_num);
	h6_hal_reg_multi_write(PWM2_CURR_REG, G_CUR, g_hasp_key_num);
	h6_hal_reg_multi_write(PWM3_CURR_REG, B_CUR, g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG9, 0x2F, g_hasp_key_num);
	h6_w_current_space_LR(R_CUR, G_CUR, B_CUR);

	if(save_flag)
	{
		if (led_bin_wb_flash_read(&flash_rgb_pwm) == sizeof(flash_rgb_pwm))
		{
			flash_rgb_pwm.r_cur = R_CUR;
			flash_rgb_pwm.g_cur = G_CUR;
			flash_rgb_pwm.b_cur = B_CUR;

			if (led_bin_wb_flash_write(&flash_rgb_pwm) == 0)
			{
				g_RGB_pwm = flash_rgb_pwm;
			}
		}
	}

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;
	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Get_white_balance
 * Description  	: Read and report the saved LED white-balance current values.
 * END ****************************************************************************************************************/
void TestAPP_Get_white_balance(uint8_t order)
{
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;

	if (led_bin_wb_flash_read(&g_RGB_pwm) == sizeof(g_RGB_pwm))
	{
		vendor_buf_in[2] = R_CUR;
		vendor_buf_in[3] = G_CUR;
		vendor_buf_in[4] = B_CUR;
	}
	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Set_SN
 * Description  	: Save the device serial number received from the manufacturing host.
 * END ****************************************************************************************************************/
void TestAPP_Set_SN(uint8_t order)
{
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;

	memcpy(SN, &vendor_buf_out[2],sizeof(SN));

	(void)mfg_flash_write_trace("SN", SN_ADDR, SN, sizeof(SN));

	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Get_SN
 * Description  	: Read and report the device serial number stored in external Flash.
 * END ****************************************************************************************************************/
void TestAPP_Get_SN(uint8_t order)
{
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;

	QSPI_EXT_FLASH_read(SN_ADDR,SN,sizeof(SN));

	memcpy(&vendor_buf_in[2], SN, sizeof(SN));

	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 * Function Name	: TestAPP_TFT_test
 *
 * Vendor command:
 *   Byte0 = 0x04
 *   Byte1 = 0x28
 *
 * Payload:
 *   Byte2 = R (0..255)
 *   Byte3 = G (0..255)
 *   Byte4 = B (0..255)
 *   Byte5 = Brightness level
 *           0x00..0x0A maps to 0%..100% in 10% increments
 *   Byte6 = Display mode
 *           0 -> power off the TFT panel
 *           1 -> power on and fill the panel with a solid RGB color
 *           2 -> draw the border-frame test pattern
 *           3 -> draw the full-screen grid test pattern
 *
 *     - R=G=B=0   : draw black/white grid
 *     - any non-0 : draw RGB grid
 *
 * Color conversion:
 *   The display constants used by this project are arranged as BGR565,
 *   so the incoming 8-bit RGB components are converted before solid-fill
 *   rendering.
 END ****************************************************************************************************************/
void TestAPP_TFT_test(uint8_t order)
{
	uint8_t brightness_level = vendor_buf_out[5];
	uint8_t shape = vendor_buf_out[6];
	uint8_t red = vendor_buf_out[2];
	uint8_t green = vendor_buf_out[3];
	uint8_t blue = vendor_buf_out[4];
	uint8_t brightness_percent;
	uint16_t color;

	if (brightness_level > 0x0A)
	{
		brightness_level = 0x0A;
	}

	/* Panel color constants in this project use BGR565 ordering. */
	color = ((uint16_t)(blue & 0xF8) << 8) |
			((uint16_t)(green & 0xFC) << 3) |
			((uint16_t)(red >> 3));
	brightness_percent = brightness_level * 10;

	mfg_flash_irq_guard_enter();

	if (shape == 0)
	{
		LCD_sleep();
	}
	else
	{
		LCD_wakeup();
		lcd_bl_set_brightness(brightness_percent);

		if (shape == 2)
		{
			LCD_DrawBorderFrame();
		}
		else if (shape == 3)
		{
			/* Reuse the RGB payload as a temporary grid-style selector until the
			 * external production-test protocol defines an explicit grid mode. */
			LCD_DrawTestGrid((red != 0) || (green != 0) || (blue != 0));
		}
		else
		{
			LCD_Fill_Color(color);
		}
	}

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;

	mfg_flash_irq_guard_exit();

	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: RT_save_to_flash
 * Description  	: Save the per-key RT press/release tables to external Flash at RT_ADDR.
 * 
 *   Flash layout:
 *   [0   .. 111] = per_key_rt_press[112]
 *   [112 .. 223] = per_key_rt_release[112]
 * END ****************************************************************************************************************/
static void RT_save_to_flash(void)
{
	uint8_t flash_buf[HASP_KEY_NUM * 2];

	memcpy(&flash_buf[0],            per_key_rt_press,   HASP_KEY_NUM);
	memcpy(&flash_buf[HASP_KEY_NUM], per_key_rt_release, HASP_KEY_NUM);

	/* QSPI_EXT_FLASH_write auto-erases the 4KB sector before writing */
	unsigned int ret = mfg_flash_write_trace("RT", RT_ADDR, flash_buf, sizeof(flash_buf));
	if (ret != 0)
		info("ERROR: RT save to flash failed, ret=%d\r\n", ret);
}

/* FUNCTION ************************************************************************************************************
 * Function Name	: TestAPP_Set_RT
 * TestAPP_Set_RT  (cmd 0x40)

 * Protocol (PC -> Device):
 *   Byte 0 = 0x04
 *   Byte 1 = 0x40
 *   Byte 2 = Packet_size  (valid data byte count from Byte 4)
 *   Byte 3 = Packet_type
 *            0xFF: uniform mode (all keys same), Byte 4=RT_Press, Byte 5=RT_Break
 *            else: Bit[7:4]=PACKET_TOTAL, Bit[3:0]=PACKET_INDEX
 *                  Byte 4+ = (keycode, RT_Press, RT_Break) * N
 *
 * 	 Response (Device -> PC):
 *   Echo header (Byte 0-3), rest = 0
 * END ****************************************************************************************************************/
void TestAPP_Set_RT(uint8_t order)
{
	uint8_t packet_size = vendor_buf_out[2];
	uint8_t packet_type = vendor_buf_out[3];

	if ((packet_size == PACKET_TYPE_UNIFORM_BYTE2) && vendor_buf_out[3] == PACKET_TYPE_UNIFORM_BYTE3)
	{
		/* Uniform mode: apply same RT values to all valid keys */
		uint8_t rt_p = vendor_buf_out[RT_DATA_OFFSET];
		uint8_t rt_b = vendor_buf_out[RT_DATA_OFFSET + 1];

		if (!rt_p) rt_p = RT_PRESS_MIN;
		if (!rt_b) rt_b = RT_BREAK_MIN;
		if (rt_p > RT_PRESS_MAX) rt_p = RT_PRESS_MAX;
		if (rt_b > RT_BREAK_MAX) rt_b = RT_BREAK_MAX;

		for (uint8_t k = 0; k < g_hasp_key_num; k++)
		{
			if (Key_HID_Usage_ID[k] != 0) 
			{
				per_key_rt_press[k]   = rt_p;
				per_key_rt_release[k] = rt_b;
			}
		}
		RT_save_to_flash();
	} 
	else 
	{
		/* Per-key mode: parse (keycode, RT_Press, RT_Break) tuples */
		uint8_t num_keys = packet_size / RT_BYTES_PER_KEY;
		for (uint8_t i = 0; i < num_keys; i++)
		{
			uint8_t keycode = vendor_buf_out[RT_DATA_OFFSET + i * RT_BYTES_PER_KEY];
			uint8_t rt_p    = vendor_buf_out[RT_DATA_OFFSET + i * RT_BYTES_PER_KEY + 1];
			uint8_t rt_b    = vendor_buf_out[RT_DATA_OFFSET + i * RT_BYTES_PER_KEY + 2];

			if (!rt_p) rt_p = RT_PRESS_MIN;
			if (!rt_b) rt_b = RT_BREAK_MIN;
			if (rt_p > RT_PRESS_MAX) rt_p = RT_PRESS_MAX;
			if (rt_b > RT_BREAK_MAX) rt_b = RT_BREAK_MAX;

			/* Find the physical key index that owns this keycode */
			for (uint8_t j = 0; j < g_hasp_key_num; j++)
			{
				if (Key_HID_Usage_ID[j] == keycode) 
				{
					per_key_rt_press[j]   = rt_p;
					per_key_rt_release[j] = rt_b;
					break;
				}
			}
		}

		/* Save to Flash after the last packet arrives */
		uint8_t pkt_total = (packet_type >> 4) & 0x0F;
		uint8_t pkt_index =  packet_type       & 0x0F;
		if (pkt_index >= pkt_total - 1)
		{
			RT_save_to_flash();
		}
	}

	/* ACK: echo header bytes back to host */
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = BYTE1_SET_RT;
	vendor_buf_in[2] = packet_size;
	vendor_buf_in[3] = packet_type;
	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 * Function Name	: TestAPP_Get_RT
 * TestAPP_Get_RT  (cmd 0x41)
 *
 * Protocol (PC -> Device):
 *   Byte 0 = 0x04, Byte 1 = 0x41
 *
 * Response (Device -> PC) 閿燂�?one or more packets:
 *   Byte 0 = 0x04
 *   Byte 1 = 0x41
 *   Byte 2 = Packet_size  (valid data bytes in this packet)
 *   Byte 3 = Packet_type  (Bit[7:4]=total, Bit[3:0]=index)
 *   Byte 4+ = (keycode, RT_Press, RT_Break) * N
 *
 *   Description  	: Package and report all per-key RT press/release settings to the manufacturing host.
 * END ****************************************************************************************************************/
void TestAPP_Get_RT(uint8_t order)
{
	/* Count valid keys (keycode != 0) */
	uint8_t valid_count = 0;
	for (uint8_t k = 0; k < g_hasp_key_num; k++)
	{
		if (Key_HID_Usage_ID[k] != 0)
			valid_count++;
	}

	/* Calculate total packets needed */
	uint8_t pkt_total = (valid_count + KEYS_PER_PACKET_2BYTE - 1) / KEYS_PER_PACKET_2BYTE;
	if (pkt_total == 0) pkt_total = 1;

	uint8_t pkt_index     = 0;
	uint8_t keys_in_pkt   = 0;
	uint8_t buf_pos       = RT_DATA_OFFSET;

	/* Prepare the first packet header */
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = BYTE1_GET_RT;

	for (uint8_t k = 0; k < g_hasp_key_num; k++)
	{
		uint8_t keycode = Key_HID_Usage_ID[k];
		if (keycode == 0)
			continue;

		vendor_buf_in[buf_pos++] = keycode;
		vendor_buf_in[buf_pos++] = per_key_rt_press[k];
		vendor_buf_in[buf_pos++] = per_key_rt_release[k];
		keys_in_pkt++;

		if (keys_in_pkt >= KEYS_PER_PACKET_2BYTE)
		{
			/* Packet full: fill header and send */
			vendor_buf_in[2] = keys_in_pkt * RT_BYTES_PER_KEY;
			vendor_buf_in[3] = (pkt_total << 4) | pkt_index;
			send_vendor();

			pkt_index++;
			keys_in_pkt = 0;
			buf_pos     = RT_DATA_OFFSET;

			memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
			vendor_buf_in[0] = BYTE0_HEADER04;
			vendor_buf_in[1] = BYTE1_GET_RT;
		}
	}

	/* Send remaining keys in the last (possibly partial) packet */
	if (keys_in_pkt > 0) 
	{
		vendor_buf_in[2] = keys_in_pkt * RT_BYTES_PER_KEY;
		vendor_buf_in[3] = (pkt_total << 4) | pkt_index;
		send_vendor();
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Encoder_test
 * Description  	: Enter encoder manufacturing test mode and send an acknowledge packet to the host.
 * END ****************************************************************************************************************/
void TestAPP_Encoder_test(uint8_t order)
{
	wheel_set_encoder_test_mode(1);

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;
	vendor_buf_in[2] = 0x00;
	vendor_buf_in[3] = 0x00;

	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Enter_test_mode
 * Description  	: Enter manufacturing test mode and stop any active periodic report stream.
 * END ****************************************************************************************************************/
void Enter_test_mode(void)
{
	/* Stop any ongoing periodic report before entering test mode */
	report_active = 0;
	report_ready  = 0;
	timer_stop(TC_NUMBER1);

	mfg_mode_cmd = 0;
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = BYTE1_TEST_MODE04;
	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Exit_test_mode
 * Description  	: Exit manufacturing test mode and restore normal encoder/TFT runtime state.
 * END ****************************************************************************************************************/
void Exit_test_mode(void)
{
	mfg_mode_cmd =  1;
	report_active = 0;
	report_ready  = 0;
	timer_stop(TC_NUMBER1);
	wheel_set_encoder_test_mode(0);
	LCD_Fill_White();

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = BYTE1_NORMAL_MODE05;
	send_vendor();

	h6_hal_variable_time_inst(0);//hall variable timing instruction
	hasp_drv_start_work();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: SET_Restore_default
 * Description  	: restore-default.
 * END ****************************************************************************************************************/
void SET_Restore_default(void)
{
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = 0x04;
	vendor_buf_in[1] = BYTE1_RESTOR2DEFAULT;
	vendor_buf_in[2] = SUCCESS;
	/*do restore default here*/

	//QSPI_EXT_FLASH_erase_sector(GAIN_VALUE_ADDR);
	//QSPI_EXT_FLASH_erase_sector(TBL_CALI_ADDR);
	//QSPI_EXT_FLASH_erase_sector(BBL_CALI_ADDR);

	send_vendor();

}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: SET_RESET
 * Description  	: Reply to the reset command, wait for USB IN completion, then issue a software reset.
 * END ****************************************************************************************************************/
void SET_RESET(void)
{
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = 0x04;
	vendor_buf_in[1] = BYTE1_SET_RESET;
	vendor_buf_in[2] = SUCCESS;
	send_vendor();

	//wait for this packet to be completed
    while(data_report_done == 0)
    {
        tud_task();
        ry_delay_us(5);
    }
    ry_delay_ms(2);

    smu_soft_reset();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestMode_Set_Region_ID
 * Description  	: Save the region ID provided by the manufacturing host into external Flash.
 * END ****************************************************************************************************************/
void TestMode_Set_Region_ID(uint8_t order)
{
	uint8_t region_ID = 0;

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;

	region_ID = vendor_buf_out[2];
	info("region_ID = %x\r\n",region_ID);

	(void)mfg_flash_write_trace("REGION", REGIOON_ID_ADDR, (const uint8_t *)&region_ID, sizeof(region_ID));

	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestMode_Get_Region_ID
 * Description  	: Read back the saved region ID and report it to the manufacturing host.
 * END ****************************************************************************************************************/
void TestMode_Get_Region_ID(uint8_t order)
{
	uint8_t region_ID = 0;

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;

	debug_read_external_flash(REGIOON_ID_ADDR,(uint8_t *)&region_ID,sizeof(region_ID));

	vendor_buf_in[2] = region_ID;

	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: hall_gain_load_final_table
 * Description  	: Build and synchronize the complete per-key gain table from Flash and defaults.
 * END ****************************************************************************************************************/
void hall_gain_load_final_table(uint16_t *gain_table, uint32_t size)
{
	unsigned int read_len;

	/* g_gain_value is the only per-key RAM table. A short read falls back to a
	 * complete default table; erased entries fall back individually. */
	read_len = QSPI_EXT_FLASH_read(GAIN_VALUE_ADDR, g_gain_value, size);
	h6_gain_table_finalize(g_gain_value, read_len, size, POWER_ON_W_GAIN);
	wdt_reset_timer();
	hall_gain_build_reg_buffer(gain_table, size);

	#if HALL_GAIN_EN
	printf("GAIN final table built, read_len=%u expected=%u\r\n",
	     read_len,
	     (unsigned int)size);
	#endif
}

uint8_t Flash_Gain_to_Hall_init(void)
{
	uint16_t gain_table[HASP_KEY_NUM] = {0};

	hall_gain_load_final_table(gain_table, g_hasp_key_num);
	h6_hal_write_gain_config0_table(gain_table, g_hasp_key_num);
	return 1U;
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Gain_save_to_flash
 * Description  	: Save the cached per-key gain table to external Flash.
 * END ****************************************************************************************************************/
static void Gain_save_to_flash(void)
{
	uint8_t flash_buf[HASP_KEY_NUM] = {0};
	uint8_t ii;

	for (ii = 0; ii < HASP_KEY_NUM; ii++)
	{
		g_gain_value[ii] = hall_gain_sanitize(g_gain_value[ii]);
		flash_buf[ii] = g_gain_value[ii];

		if (ii < g_hasp_key_num)
		{
			hall_gain_log_entry("GAIN_FLASH_SAVE", ii, Key_HID_Usage_ID[ii], g_gain_value[ii]);
		}
	}

	/* QSPI_EXT_FLASH_write auto-erases the 4KB sector before writing */
	unsigned int ret = mfg_flash_write_trace("GAIN", GAIN_VALUE_ADDR, (uint8_t*)flash_buf, sizeof(flash_buf));
	#if HALL_GAIN_EN
	if (ret != 0)
	{
		info("ERROR: GAIN save to flash failed, ret=%d\r\n", ret);
	}
	else
	{
		info("GAIN flash save OK, bytes=%u\r\n", (unsigned int)sizeof(flash_buf));
	}
	#endif
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: hall_gain_sanitize
 * Description  	: Normalize the REG0B LNA/PGA fields using the H6 HAL rules.
 * Return       	: Normalized gain value safe for RAM, H6 register, and Flash use.
 * END ****************************************************************************************************************/
static uint8_t hall_gain_sanitize(uint8_t raw_gain)
{
	uint8_t normalized_gain = h6_hal_normalize_gain_config0(raw_gain);

	if (normalized_gain != raw_gain)
	{
		#if HALL_GAIN_EN
		info("GAIN 0x%02X has invalid LNA/PGA fields, normalized to 0x%02X\r\n",
		     raw_gain,
		     normalized_gain);
		#endif
	}

	return normalized_gain;
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: hall_gain_log_entry
 * Description  	: Print one formatted gain debug line when HALL_GAIN_EN logging is enabled.
 * END ****************************************************************************************************************/
static void hall_gain_log_entry(const char *tag, uint8_t key_index, uint8_t keycode, uint8_t gain)
{
	#if HALL_GAIN_EN
	info("[%s] idx=%u keycode=0x%02X gain=0x%02X(%u)\r\n",
	     tag,
	     key_index,
	     keycode,
	     gain,
	     gain);
	#endif
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: hall_gain_build_reg_buffer
 * Description  	: Convert the cached gain table into the per-key REG0B write buffer order.
 * END ****************************************************************************************************************/
static void hall_gain_build_reg_buffer(uint16_t *gain_buf, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		uint8_t sanitized_gain = hall_gain_sanitize(g_gain_value[i]);
		g_gain_value[i] = sanitized_gain;
		gain_buf[i] = sanitized_gain;
	}
}

/**
 * \brief       calc_chain_get_index
 *
 * \param[in] chain_keycode :  Get the index according to the keycode.
 * \return    index
 */
uint8_t calc_chain_get_index(uint8_t chain_keycode)
{
	for (uint8_t i = 0; i < g_hasp_key_num; i++)
	{
		if (Key_HID_Usage_ID[i] == chain_keycode)
        {
            return i;
        }

	}
    return 0xFF;
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Set_gain
 * Description  	: Update uniform or per-key gain settings, then synchronize RAM, H6 REG0B, and Flash.
 * END ****************************************************************************************************************/
void TestAPP_Set_gain(uint8 order)
{
	uint8_t packet_size = vendor_buf_out[2];
	uint8_t packet_type = vendor_buf_out[3] & 0x0F;   	  //packet_index  缁楊剙鍤戦敓锟�?
	uint8_t packet_sum = (vendor_buf_out[3] >> 4 )& 0x0F; //packet_sum	娑擄拷閸忚鲸婀侀崙鐘插瘶
	uint8_t start_index = 4;							  //缁楊剙娲撴稉鐚檡te瀵拷婵绱舵潏鎾存箒閺佸牏娈慓ain閺佺増锟�?
	uint8_t packet_index = packet_type;
	uint8_t packet_total = packet_sum;
	uint8_t data_size = 0;
	uint16_t gain_buf[HASP_KEY_NUM];

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = BYTE1_SET_GAIN_VALUE;

	if ((packet_size == PACKET_TYPE_UNIFORM_BYTE2) && (vendor_buf_out[3] == PACKET_TYPE_UNIFORM_BYTE3))
	{
		uint8_t new_gain = hall_gain_sanitize(vendor_buf_out[4]);
		#if HALL_GAIN_EN
		debug("reg0B value = %d\r\n", new_gain);
		#endif

		/* Update REG0B together with its LNA-dependent REG29/REG2A trims. */
		h6_hal_write_gain_config0_uniform(new_gain, g_hasp_key_num);

		for (uint8_t kk = 0; kk < HASP_KEY_NUM; kk++)
		{
			g_gain_value[kk] = new_gain;
			 if (kk < g_hasp_key_num)
			 {
				hall_gain_log_entry("GAIN_SET_UNIFORM", kk, Key_HID_Usage_ID[kk], g_gain_value[kk]);
			 }
		}
		Gain_save_to_flash();

		vendor_buf_in[2] = PACKET_TYPE_UNIFORM_BYTE2;
		vendor_buf_in[3] = PACKET_TYPE_UNIFORM_BYTE3;
	}
	else
	{
		/*
		 * Per-key mode payload is encoded as repeated (keycode, gain) pairs.
		 * Incoming keycodes are matched against Key_HID_Usage_ID so the host can
		 * use the same HID IDs for both setting and reading gain values.
		 */
		data_size = packet_size;
		if (data_size > (VENDOR_REPORT_SIZE - start_index))
		{
			data_size = VENDOR_REPORT_SIZE - start_index;
		}
		/* Keep only complete (keycode, gain) pairs and ignore a trailing odd byte. */
		data_size &= (uint8_t)~0x01;

		for (uint8_t j = 0; j < data_size; j += 2)
		{
			uint8_t keycode = vendor_buf_out[j + start_index];
			uint8_t new_gain = hall_gain_sanitize(vendor_buf_out[j + start_index + 1]);
			uint8_t key_index;

			if (keycode == 0x00)
			{
				continue;
			}

			key_index = calc_chain_get_index(keycode);
			if (key_index == 0xFF)
			{
				#if HALL_GAIN_EN
				info("Unknown gain keycode: 0x%02X\r\n", keycode);
				#endif
				continue;
			}

			g_gain_value[key_index] = new_gain;
			hall_gain_log_entry("GAIN_SET_KEY", key_index, keycode, g_gain_value[key_index]);
		}

		if ((packet_total != 0) && (packet_index >= (packet_total - 1)))
		{
			hall_gain_build_reg_buffer(gain_buf, g_hasp_key_num);
			/* Apply every per-key REG0B value and its matching trim registers. */
			h6_hal_write_gain_config0_table(gain_buf, g_hasp_key_num);
			Gain_save_to_flash();
		}
	}

	send_vendor();

//	config_hasp_key_td();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Get_gain
 * Description  	: Read gain values from H6 REG0B and report them with HID keycodes in manufacturing format.
 * 
 * Handle "04 17 05" gain readback.
 * The response format matches the generic 1-byte Hall report:
 *   Byte 0 = 0x04
 *   Byte 1 = 0x17
 *   Byte 2 = 0x05
 *   Byte 3+ = (keycode, gain) pairs
 * END ****************************************************************************************************************/
void TestAPP_Get_gain(uint8_t order)
{
	uint8_t buf_index = VENDOR_HEADER_SIZE;
	uint8_t keys_in_packet = 0;

	/* Read H6 REG0B from every active Hall slot before packaging the response. */
	h6_drv_read_reg_cmd(CONGFIG_REGB, g_hasp_key_num);
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = BYTE1_GET_HALL_PARAM_1BYTE;
	vendor_buf_in[2] = order;

	for (uint8_t i = 0; i < g_hasp_key_num; i++)
	{
		uint8_t keycode = Key_HID_Usage_ID[i];
		uint8_t read_gain;

		if (keycode == 0x00)
		{
			continue;
		}

		read_gain = (uint8_t)(GTRY_HASP->RXRAM[i] & 0xFF);
		g_gain_value[i] = read_gain;

//		hall_gain_log_entry("GAIN_READBACK", i, keycode, g_gain_value[i]);

		#if HALL_GAIN_EN
		debug("hall chain->%d:%d reg0B value = %d\r\n",
		      i % HASP_CHANNL_NUM,
		      i / HASP_CHANNL_NUM,
		      read_gain);
		#endif

		vendor_buf_in[buf_index++] = keycode;
		vendor_buf_in[buf_index++] = read_gain;
		keys_in_packet++;

		if (keys_in_packet >= KEYS_PER_PACKET_1BYTE)
		{
			send_vendor();

			memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
			vendor_buf_in[0] = BYTE0_HEADER04;
			vendor_buf_in[1] = BYTE1_GET_HALL_PARAM_1BYTE;
			vendor_buf_in[2] = order;
			buf_index = VENDOR_HEADER_SIZE;
			keys_in_packet = 0;
		}
	}

	if (keys_in_packet > 0)
	{
		send_vendor();
	}

}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestMode_cali_Data
 * Description  	: Dispatch calibration sub-commands according to the current report_data_type value.
 * END ****************************************************************************************************************/
void TestMode_cali_Data(void)
{
	switch (report_data_type)
	{

	#if H6_OFFSET_CALI
		case H6_IMBALANCE_CALI_CMD:
		{
			mfg_flash_irq_guard_enter();
			unsigned int ret = H6_offset_cali_b(); /* 04 10 D4 triggers H6 imbalance calibration on demand. */
			memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE); /* Build a compact ACK packet before the per-key SELF_CHECK report. */
			vendor_buf_in[0] = BYTE0_HEADER04;
			vendor_buf_in[1] = BYTE1_CALI;
			vendor_buf_in[2] = H6_IMBALANCE_CALI_CMD;
			vendor_buf_in[3] = (uint8_t)ret; /* ret==0 means all active Hall slots passed imbalance calibration. */
			mfg_flash_irq_guard_exit();

			send_vendor();
			TestAPP_Report_HallGeneric(BYTE1_GET_HALL_PARAM_1BYTE, SELF_CHECK); /* Send keycode + SELF_CHECK status so failed keys are visible immediately. */

			mfg_flash_irq_guard_enter();
			offset_cali_reg_recovery();
			mfg_flash_irq_guard_exit();
			break;
		}
	#endif

		case Hall_PRESS_SAMPLING_AND_CALI:
		case Hall_RELEASE_SAMPLING_AND_CALI:
			TestAPP_set_factory_cali(report_data_type,vendor_buf_out[3]);
			break;

		case START_HASP_BOTTOM_CALI_SAVE:
			TestAPP_cali_BBL_end(report_data_type,vendor_buf_out[3]);
			break;
		case START_HALL_TBL_CALI_SAVE:
			TestAPP_cali_TBL_end(report_data_type,vendor_buf_out[3]);
			break;

		default:
			break;
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestMode_Report_lang_FW_ver
 * Description  	: Report language type, keyboard type, firmware version, and flash ID to the host.
 * END ****************************************************************************************************************/
void TestMode_Report_lang_FW_ver(uint8_t order)
{
	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = order;
	vendor_buf_in[2] = g_language_type;
	vendor_buf_in[3] = g_keyboard_type;

	vendor_buf_in[4] = (uint8_t) FW_VERSION;
	vendor_buf_in[5] = (uint8_t)(FW_VERSION >> 8);
	vendor_buf_in[6] = (uint8_t)(FW_VERSION >> 16);
	vendor_buf_in[7] = (uint8_t)(FW_VERSION >> 24);

	vendor_buf_in[8] = (uint8_t) Flash_id;
	vendor_buf_in[9] = (uint8_t)(Flash_id >> 8);
	vendor_buf_in[10]= (uint8_t)(Flash_id >> 16);

	send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestMode_Report_Data
 * Description  	: Send the pending manufacturing report according to the active command and data type.
 * END ****************************************************************************************************************/
void TestMode_Report_Data(void)
{
	switch (report_cmd_type)
	{
		case BYTE1_GET_HALL_PARAM_1BYTE:
			if (report_data_type == GAIN_VALUE)
			{
				TestAPP_Get_gain(report_data_type);
				/* Gain readback is a one-shot command: stop periodic reporting. */
				report_active = 0;
				report_ready = 0;
				timer_stop(TC_NUMBER1);
			}
			else
			{
				TestAPP_Report_HallGeneric(report_cmd_type, report_data_type);
			}
			break;

		case BYTE1_GET_HALL_PARAM_2BYTE:
			TestAPP_Report_HallGeneric(report_cmd_type, report_data_type);
			break;

		case BYTE1_GET_HALL_PARAM_3BYTE:
			TestAPP_user_cali_start(report_data_type);
			break;

		default:
			break;
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Confirm_maker_mode
 * Description  	: Send the simple acknowledge packet used when confirming manufacturing mode entry.
 * END ****************************************************************************************************************/
void Confirm_maker_mode(void)
{
    vendor_buf_in[1] = 0x04;
    send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Led_rgb_order
 * Description  	: Handle the RGB LED manufacturing command that updates PWM duty and brightness period.
 * END ****************************************************************************************************************/
void Led_rgb_order(void)
{
	uint16_t brightness;

    if(vendor_buf_out[1] == 0)
    {
    	h6_hal_pwm_multi_write(H6_PWM_LED1_DUTY, vendor_buf_out[2], g_hasp_key_num);
    	h6_hal_pwm_multi_write(H6_PWM_LED2_DUTY, vendor_buf_out[3], g_hasp_key_num);
    	h6_hal_pwm_multi_write(H6_PWM_LED3_DUTY, vendor_buf_out[4], g_hasp_key_num);
    	h6_w_pwm_space_LR(vendor_buf_out[2], vendor_buf_out[3], vendor_buf_out[4]);

        brightness = (vendor_buf_out[5] << 8) | (vendor_buf_out[6]);
        if((255 <= brightness && brightness <= 2550))
        {
        	h6_hal_pwm_multi_write(H6_PWM_TOTAL_PERIOD, brightness, g_hasp_key_num);
        }

        h6_drv_w_pwm_time_clr();
    }
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Get_version_order
 * Description  	: Report the current firmware version through the legacy manufacturing command path.
 * END ****************************************************************************************************************/
void Get_version_order(void)
{
	vendor_buf_in[1] = 0x04;

    if(vendor_buf_out[1] == 0)
    {
    	vendor_buf_in[2] = (uint8_t) FW_VERSION;
    	vendor_buf_in[3] = (uint8_t)(FW_VERSION >> 8);
    	vendor_buf_in[4] = (uint8_t)(FW_VERSION >> 16);
    	vendor_buf_in[5] = (uint8_t)(FW_VERSION >> 24);
    }
    send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Cmd_switch_tft_color
 * Description  	: Handle the legacy TFT power-control command used by manufacturing tools.
 * END ****************************************************************************************************************/
void Cmd_switch_tft_color(void)
{
	vendor_buf_in[1] = 0x05;

	switch (vendor_buf_out[1])
	{
		case 0x00:
	        LCD_power_off();
			break;

		case 0x01:
			LCD_power_on();
			break;
	}

    send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Turn_off_LED
 * Description  	: Handle the legacy LED enable/disable manufacturing command.
 * END ****************************************************************************************************************/
void Turn_off_LED(void)
{
 	vendor_buf_in[1] = 0x06;

     if(vendor_buf_out[1] == 0)
     {
         gpio_out_clear(LED_EN);
     }
 	else if(vendor_buf_out[1] == 1)
 	{
 		gpio_out_set(LED_EN);
 	}

     send_vendor();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Test_extFlash
 * Description  	: Execute simple external-Flash write, read, or clock-stress test actions for debug.
 * Return       	: Always returns 0.
 * END ****************************************************************************************************************/
uint8_t Test_extFlash(uint8_t cmd_buff)
{
	uint8_t buffer_Flash[16];

	if(cmd_buff == 1)
	{
		info("write\r\n");
		for(int i = 0; i < 100; i++)
		{
			copy_internal_flash_to_external_flash(0,64);
		}

	}
	else if(cmd_buff == 2)
	{
		info("read\r\n");
		debug_read_external_flash(0,buffer_Flash,16);
	}
	else if(cmd_buff == 3)
	{
		uint8_t buf_clk_test[16];
//		info("SCK timing test start\r\n");
		for(int i = 0; i < 500; i++)
		{
			debug_read_external_flash(i & 0xFFF, buf_clk_test, 16);
		}
//		info("SCK timing test done\r\n");
	}

	return 0;
}

/*--------------------------------------------------------------------+
 * Dispatch table for 0x04 manufacturing commands
 *--------------------------------------------------------------------*/
typedef void (*cmd_handler_t)(uint8_t);

#define REPORT_DATA_TYPE_KEEP_CURRENT      0
#define REPORT_DATA_TYPE_USE_CMD_VALUE     1

typedef struct {
	uint8_t cmd;                           /* 0x04 sub-command ID from vendor_buf_out[1]. */
	cmd_handler_t handler;                /* Command handler for this sub-command. */
	uint8_t set_report_data_type_to_cmd;  /* 1: set report_data_type = cmd before handler runs.
	                                       * 0: handler manages report_data_type by itself. */
} cmd_entry_t;

static void handle_enter_test_mode_cmd(uint8_t cmd);
static void handle_exit_test_mode_cmd(uint8_t cmd);
static void handle_cali_cmd(uint8_t cmd);
static void handle_lang_fw_version_cmd(uint8_t cmd);
static void handle_hall_report_cmd(uint8_t cmd);
static void handle_set_led_wb_cmd(uint8_t cmd);
static void handle_restore_default_cmd(uint8_t cmd);
static void handle_set_reset_cmd(uint8_t cmd);
static uint8_t dispatch_header04_cmd(uint8_t cmd);
static void handle_header04_packet(void);

/* Table-driven dispatch for all 0x04 manufacturing commands.
 * The last column controls whether report_data_type should be updated
 * with the command byte before calling the handler. */
static const cmd_entry_t header04_cmd_table[] =
{
	{ BYTE1_TEST_MODE04,            handle_enter_test_mode_cmd,  REPORT_DATA_TYPE_KEEP_CURRENT  },
	{ BYTE1_NORMAL_MODE05,          handle_exit_test_mode_cmd,   REPORT_DATA_TYPE_KEEP_CURRENT  },
	{ BYTE1_CALI,                   handle_cali_cmd,             REPORT_DATA_TYPE_KEEP_CURRENT  },
	{ BYTE1_GET_LANG_FW_VERSION,    handle_lang_fw_version_cmd,  REPORT_DATA_TYPE_KEEP_CURRENT  },
	{ BYTE1_GET_HALL_PARAM_1BYTE,   handle_hall_report_cmd,      REPORT_DATA_TYPE_KEEP_CURRENT  },
	{ BYTE1_GET_HALL_PARAM_2BYTE,   handle_hall_report_cmd,      REPORT_DATA_TYPE_KEEP_CURRENT  },
	{ BYTE1_GET_HALL_PARAM_3BYTE,   handle_hall_report_cmd,      REPORT_DATA_TYPE_KEEP_CURRENT  },
	{ BYTE1_SET_LED_WB,             handle_set_led_wb_cmd,       REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_RESTOR2DEFAULT,         handle_restore_default_cmd,  REPORT_DATA_TYPE_KEEP_CURRENT  },
	{ BYTE1_SET_RESET,              handle_set_reset_cmd,        REPORT_DATA_TYPE_KEEP_CURRENT  },
	{ BYTE1_SET_REGION_ID,          TestMode_Set_Region_ID,      REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_GET_REGION_ID,          TestMode_Get_Region_ID,      REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_SET_GAIN_VALUE,         TestAPP_Set_gain,            REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_SET_PWM,                TestAPP_Set_PWM,             REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_SET_LED_BIN,            TestAPP_Set_led_bin,         REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_GET_LED_BIN,            TestAPP_Get_led_bin,         REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_GET_LED_WB,             TestAPP_Get_white_balance,   REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_SET_SN,                 TestAPP_Set_SN,              REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_GET_SN,                 TestAPP_Get_SN,              REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_TFT_TEST,               TestAPP_TFT_test,            REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_SET_RT,                 TestAPP_Set_RT,              REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_GET_RT,                 TestAPP_Get_RT,              REPORT_DATA_TYPE_USE_CMD_VALUE },
	{ BYTE1_ENCODER_TEST,           TestAPP_Encoder_test,        REPORT_DATA_TYPE_USE_CMD_VALUE },
};

#define HEADER04_CMD_COUNT (sizeof(header04_cmd_table) / sizeof(header04_cmd_table[0]))

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: mfg_cmd_requires_test_mode
 * Description  	: Check whether the specified 0x04 sub-command is allowed only after entering test mode.
 * Return       	: 1 when test mode is required, otherwise 0.
 * 
 *     0x04 0x04 (enter test mode) is always allowed
 *   - 0x04 0x05 (exit test mode) is always allowed
 *   - every other cmd_type_t command requires mfg_mode_cmd == 0
 * END ****************************************************************************************************************/
static uint8_t mfg_cmd_requires_test_mode(uint8_t cmd)
{
	switch (cmd)
	{
		case BYTE1_TEST_MODE04:
		case BYTE1_NORMAL_MODE05:
			return 0;

		default:
			return 1;
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: handle_enter_test_mode_cmd
 * Description  	: Handle sub-command 0x04 and enter manufacturing test mode immediately.
 * END ****************************************************************************************************************/
static void handle_enter_test_mode_cmd(uint8_t cmd)
{
	(void)cmd;
	Enter_test_mode();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: handle_exit_test_mode_cmd
 * Description  	: Handle sub-command 0x05 and exit manufacturing test mode.
 * END ****************************************************************************************************************/
static void handle_exit_test_mode_cmd(uint8_t cmd)
{
	(void)cmd;
	Exit_test_mode();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: handle_cali_cmd
 * Description  	: Handle calibration commands. The calibration subtype is taken from vendor_buf_out[2].
 * END ****************************************************************************************************************/
static void handle_cali_cmd(uint8_t cmd)
{
	(void)cmd;
	report_data_type = vendor_buf_out[2];
	TestMode_cali_Data();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: handle_lang_fw_version_cmd
 * Description  	: Report language, keyboard type, firmware version and flash metadata for manufacturing tools.
 * END ****************************************************************************************************************/
static void handle_lang_fw_version_cmd(uint8_t cmd)
{
	(void)cmd;
	report_data_type = vendor_buf_out[1];
	TestMode_Report_lang_FW_ver(report_data_type);
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: handle_hall_report_cmd
 * Description  	: Configure and trigger hall-data reporting for 0x17/0x18/0x19 commands.
 *                  The report subtype comes from vendor_buf_out[2].
 * END ****************************************************************************************************************/
static void handle_hall_report_cmd(uint8_t cmd)
{
	report_cmd_type  = cmd;
	report_data_type = vendor_buf_out[2];
	report_tick_cnt  = 0;
	report_ready     = 0;

	if ((cmd == BYTE1_GET_HALL_PARAM_1BYTE) && (report_data_type == GAIN_VALUE))
	{
		/* "04 17 05" should report all key gain values only once. */
		report_active = 0;
		timer_stop(TC_NUMBER1);
	}
	else
	{
		report_active = 1;
		timer_trigger_clear(TC_NUMBER1);
		timer_start(TC_NUMBER1);
	}
	TestMode_Report_Data();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: handle_set_led_wb_cmd
 * Description  	: Handle LED white-balance programming. The white-balance value is taken from vendor_buf_out[5].
 * END ****************************************************************************************************************/
static void handle_set_led_wb_cmd(uint8_t cmd)
{
	TestAPP_Set_white_balance(cmd, vendor_buf_out[5]);
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: handle_restore_default_cmd
 * Description  	: Restore manufacturing-related parameters back to default values.
 * END ****************************************************************************************************************/
static void handle_restore_default_cmd(uint8_t cmd)
{
	(void)cmd;
	SET_Restore_default();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: handle_set_reset_cmd
 * Description  	: Handle the reset command path through SET_RESET().
 * END ****************************************************************************************************************/
static void handle_set_reset_cmd(uint8_t cmd)
{
	(void)cmd;
	SET_RESET();
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: dispatch_header04_cmd
 * Description  	: Find the matching 0x04 manufacturing sub-command and dispatch it through the command table.
 * Return       	: 1 if the sub-command is handled, otherwise 0.
 * END ****************************************************************************************************************/
static uint8_t dispatch_header04_cmd(uint8_t cmd)
{
	for (uint8_t i = 0; i < HEADER04_CMD_COUNT; i++)
	{
		if (header04_cmd_table[i].cmd == cmd)
		{
			if (header04_cmd_table[i].set_report_data_type_to_cmd)
			{
				report_data_type = cmd;
			}

			header04_cmd_table[i].handler(cmd);
			return 1;
		}
	}

	return 0;
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: handle_header04_packet
 * Description  	: Validate manufacturing-mode access for the current 0x04 packet, then dispatch the sub-command.
 * END ****************************************************************************************************************/
static void handle_header04_packet(void)
{
	uint8_t cmd = vendor_buf_out[1];

	if (mfg_cmd_requires_test_mode(cmd) && (mfg_mode_cmd != 0))
	{
		/* Ignore manufacturing/test commands until the host explicitly
		 * enters test mode with 0x04 0x04. */
		return;
	}

	(void)dispatch_header04_cmd(cmd);
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: mfg_mode_check_usb_vendor_cmd
 * Description  	: Decode the current vendor packet and dispatch it to the matching manufacturing command handler.
 * END ****************************************************************************************************************/
void mfg_mode_check_usb_vendor_cmd(void)
{
	if (!vendor_buf_out[0])
		return;

	switch (vendor_buf_out[0])
	{
		case BYTE0_HEADER04:
		{
			handle_header04_packet();
			break;
		}

	/*CMD script for internal testing to verify that the PCBA hardware is working correctly*/
	case 0x19:
		Led_rgb_order();
		break;

	case 0x20:
		Get_version_order();
		break;

	case 0x21:
		Cmd_switch_tft_color();
		break;

	case 0x22:
		Turn_off_LED();
		break;

	case 0x23:
		Test_extFlash(vendor_buf_out[1]);
		break;

	case 0x24:
		mfg_flash_irq_guard_enter();

		QSPI_EXT_FLASH_erase_chip();
	  	vendor_buf_in[1] = 0x24;

		mfg_flash_irq_guard_exit();
	    send_vendor();
		break;

	case 0x25:
		mfg_flash_irq_guard_enter();

		QSPI_EXT_FLASH_erase_sector(IMBALANCE_CALI_ADDR);
	  	vendor_buf_in[1] = 0x25;

		mfg_flash_irq_guard_exit();
	    send_vendor();
		break;

	default:
		break;
	}

	vendor_buf_out[0] = 0;
}

#else

#include <stdint.h>
#include "mfg_mode.h"
#include "h6_hal.h"

unsigned char vendor_buf_in[CFG_TUD_HID_EP_BUFSIZE] = {0x0};
unsigned char vendor_buf_out[CFG_TUD_HID_EP_BUFSIZE] = {0x0};
unsigned char mfg_sign[8] = {0x0};
unsigned char mfg_mode_cmd = 1;
volatile uint8_t report_active = 0;
volatile uint8_t report_cmd_type = 0;
volatile uint8_t report_ready = 0;
unsigned char g_gain_value[HASP_KEY_NUM] = {0x0};
uint16_t mfg_usb_vendor_report_length(void)
{
	return MFG_USB_VENDOR_REPORT_LENGTH_FS;
}

void send_vendor(void)
{
	/* Keep vendor-report callers linkable when manufacturing mode is compiled out. */
}

void TestAPP_Report_HallData(uint8_t order)
{
	(void)order;                         /* Report commands are ignored while manufacturing mode is disabled. */
}

void TestAPP_Report_SegmentData(uint8_t order)
{
	(void)order;                         /* Report commands are ignored while manufacturing mode is disabled. */
}

void report_timer1_init(void)
{
	/* Do not start the manufacturing report timer when the module is disabled. */
}

void TestMode_Report_Data(void)
{
	/* No periodic manufacturing report is generated when the module is disabled. */
}

void mfg_mode_check_usb_vendor_cmd(void)
{
	vendor_buf_out[0] = 0;               /* Drop pending vendor commands so stale packets cannot linger. */
}

uint8_t Flash_Gain_to_Hall_init(void)
{
	uint16_t gain_table[HASP_KEY_NUM] = {0};

	hall_gain_load_final_table(gain_table, g_hasp_key_num);
	h6_hal_write_gain_config0_table(gain_table, g_hasp_key_num);
	return 1U;
}

void hall_gain_load_final_table(uint16_t *gain_table, uint32_t size)
{
	unsigned int read_len;

	read_len = QSPI_EXT_FLASH_read(GAIN_VALUE_ADDR, g_gain_value, size);
	h6_gain_table_finalize(g_gain_value, read_len, size, POWER_ON_W_GAIN);

	for (uint32_t i = 0; i < size; i++)
	{
		gain_table[i] = g_gain_value[i];
	}
}

#endif /* MFG_EXT_MODE_ENABLE */
