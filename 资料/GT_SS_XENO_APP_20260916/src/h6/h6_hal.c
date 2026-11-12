/*
 * h6_hal.c
 *
 *  Created on:
 *      Author:
 */
#include <string.h>
#include "gtry_regmap.h"
#include "gtry_smu.h"
#include "gtry_uart_drv.h"
#include "gtry_wdt_drv.h"
#include "gtry_gpio_drv.h"

#include "app_config.h"

#include "hasp_drv.h"
#include "h6_drv.h"
#include "h6_hw_tab.h"
#include "h6_hal.h"
#include "mfg_mode.h"
#include "led_rgb.h"
#include "gtry_qspi_flash_drv.h"

#if H6_OFFSET_CALI
#include "h6_offset_cali.h"
#endif

uint8_t hall_hal_sys_clk;
static uint16_t hall_clock_cali_value[HASP_KEY_NUM_MAX] = {0};
uint8_t h6_error_sign = 0; /* Latch the first H6 self-check failure so later stages do not overwrite it. */
static uint8_t hall_self_check_value[HASP_KEY_NUM_MAX] = {0}; /* Store one manufacturing SELF_CHECK byte per Hall slot. */

static void hall_hal_clear_self_check_value(void)
{
	uint8_t key_num = g_hasp_key_num; /* Clear only the current product layout slots; supported layouts are at most 112. */

	for (uint8_t i = 0; i < key_num; i++)
	{
		hall_self_check_value[i] = 0U; /* Clear stale product-layout SELF_CHECK results before a new H6 init run. */
	}
	h6_error_sign = 0U; /* Allow the next self-check stage to run until a real failure is recorded. */
}

void hall_hal_set_self_check_value(uint8_t key_idx, uint8_t value)
{
	if (key_idx < HASP_KEY_NUM)
	{
		hall_self_check_value[key_idx] = value; /* Cache one report byte for the manufacturing SELF_CHECK readback. */
	}
}

uint8_t hall_hal_get_self_check_value(uint8_t key_idx)
{
	if (key_idx >= HASP_KEY_NUM)
	{
		return 0; /* Out-of-range requests are reported as no self-check result. */
	}

	return hall_self_check_value[key_idx]; /* Return the cached self-check byte generated during H6 initialization. */
}

uint16_t hall_hal_get_clock_cali_value(uint8_t key_idx)
{
	if (key_idx >= HASP_KEY_NUM_MAX)
	{
		return 0;
	}

	return hall_clock_cali_value[key_idx];
}

#define LED_BIN_DEFAULT_VALUE   0x00
#define LED_WB_R_DEFAULT_VALUE  0x21
#define LED_WB_G_DEFAULT_VALUE  0x3A
#define LED_WB_B_DEFAULT_VALUE  0x50

static uint32_t hall_hal_get_flash_key_num(void)
{
	uint32_t key_num = g_hasp_key_num; /* Use the active layout count saved by the keyboard table. */

	if (key_num > HASP_KEY_NUM)
	{
		key_num = HASP_KEY_NUM; /* Clamp to the project flash-cache slot count to avoid stack overflow. */
	}

	return key_num; /* Return the bounded key count shared by flash payload readers. */
}

static uint16_t hall_hal_calc_bottom_baseline(uint16_t saved_bottom_raw)
{
	if ((saved_bottom_raw > 600U) && ((saved_bottom_raw - 600U) > 3000U))
	{
		return (uint16_t)(saved_bottom_raw - 600U); /* Keep the calibrated bottom margin above the safe floor. */
	}

	return 3000U; /* Fall back to the conservative bottom baseline used by the reference project. */
}

static void hall_hal_apply_default_bottom_baseline_config(uint32_t key_num)
{
	for (uint32_t i = 0; i < key_num; i++)
	{
		GTRY_HASP->BASLTOP[i] = BASLTOP_VALUE; /* Restore default top baseline when Flash has no valid data. */
		GTRY_HASP->BASLBOT[i] = BASLBOT_VALUE; /* Restore default bottom baseline when Flash has no valid data. */

		GTRY_HASP->RAWDLP[i] = RAWDLP_VALUE; /* Initialize raw-data LP filter state with the default value. */
		GTRY_HASP->BTOPLP[i] = BTOPLP_VALUE; /* Keep top-baseline LP state aligned with the default baseline. */
		GTRY_HASP->BBOTLP[i] = BBOTLP_VALUE; /* Keep bottom-baseline LP state aligned with the default baseline. */
		GTRY_HASP->BTOP_INIT[i] = BTOP_INIT_VALUE; /* Keep top-baseline init state consistent with defaults. */
	}
}

static uint8_t hall_hal_load_bottom_baseline_from_flash(void)
{
	uint8_t flash_buf[HASP_KEY_NUM * 2];
	uint32_t key_num = hall_hal_get_flash_key_num(); /* Bound flash access to the active supported key slots. */
	unsigned int expected_len = key_num * 2U; /* Bottom baseline is stored as little-endian uint16 per key. */
	unsigned int actual_read;
	uint8_t all_ff = 1;

	actual_read = QSPI_EXT_FLASH_read(BBL_CALI_ADDR, flash_buf, expected_len); /* Read saved bottom calibration payload. */
	if (actual_read != expected_len)
	{
		hall_hal_apply_default_bottom_baseline_config(key_num); /* Read failure means runtime must use safe defaults. */
		return 0;
	}

	for (uint32_t i = 0; i < expected_len; i++)
	{
		if (flash_buf[i] != 0xFF)
		{
			all_ff = 0; /* Any non-0xFF byte means the sector has calibration content. */
			break;
		}
	}

	if (all_ff)
	{
		hall_hal_apply_default_bottom_baseline_config(key_num); /* Blank Flash means this keyboard was not calibrated. */
		return 0;
	}

	for (uint32_t i = 0; i < key_num; i++)
	{
		uint16_t saved_bottom_raw = (uint16_t)flash_buf[i * 2U] |
									((uint16_t)flash_buf[i * 2U + 1U] << 8); /* Rebuild little-endian raw baseline. */
		uint16_t bottom_baseline = hall_hal_calc_bottom_baseline(saved_bottom_raw); /* Convert raw value to safe baseline. */

		GTRY_HASP->BASLTOP[i] = BASLTOP_VALUE; /* Top baseline still uses the default in this HASP config path. */
		GTRY_HASP->BASLBOT[i] = (uint32_t)bottom_baseline << 2; /* BASLBOT stores the bottom baseline in register format. */

		GTRY_HASP->RAWDLP[i] = RAWDLP_VALUE; /* Reset raw-data LP state when applying saved bottom baseline. */
		GTRY_HASP->BTOPLP[i] = BTOPLP_VALUE; /* Reset top LP state to match the default top baseline. */
		GTRY_HASP->BBOTLP[i] = (uint32_t)bottom_baseline << 18; /* Seed bottom LP state from the restored baseline. */
		GTRY_HASP->BTOP_INIT[i] = BTOP_INIT_VALUE; /* Keep top initialization state unchanged from the default path. */
	}

	return 1;
}

static void hall_hal_set_led_wb_defaults(RGB_pwm *rgb_pwm)
{
	rgb_pwm->led_bin = LED_BIN_DEFAULT_VALUE; /* Use factory LED bin when Flash has no saved bin data. */
	rgb_pwm->r_cur = LED_WB_R_DEFAULT_VALUE; /* Use factory red current when Flash has no saved white balance. */
	rgb_pwm->g_cur = LED_WB_G_DEFAULT_VALUE; /* Use factory green current when Flash has no saved white balance. */
	rgb_pwm->b_cur = LED_WB_B_DEFAULT_VALUE; /* Use factory blue current when Flash has no saved white balance. */
}

static void hall_hal_led_wb_init_from_flash(void)
{
	unsigned int read_len;
	unsigned int write_ret;
	uint8_t flash_blank = 1;
	uint8_t *flash_bytes = (uint8_t *)&g_RGB_pwm; /* Read directly into the shared RGB white-balance cache. */

	read_len = QSPI_EXT_FLASH_read(LED_BIN_WB_ADDR, flash_bytes, sizeof(g_RGB_pwm)); /* Restore LED bin/WB from Flash. */
	if (read_len != sizeof(g_RGB_pwm))
	{
		info("LED/WB init flash read failed, addr=0x%X, read_len=%u, expected=%u\r\n",
			 LED_BIN_WB_ADDR,
			 read_len,
			 (unsigned int)sizeof(g_RGB_pwm));
		hall_hal_set_led_wb_defaults(&g_RGB_pwm); /* Read failure keeps startup deterministic with defaults. */
		return;
	}

	for (uint32_t i = 0; i < sizeof(g_RGB_pwm); i++)
	{
		if (flash_bytes[i] != 0xFF)
		{
			flash_blank = 0; /* A non-blank byte means manufacturing has already saved LED/WB data. */
			break;
		}
	}

	if (flash_blank)
	{
		hall_hal_set_led_wb_defaults(&g_RGB_pwm); /* Blank Flash uses the same defaults as the old hardcoded path. */
		write_ret = QSPI_EXT_FLASH_write(LED_BIN_WB_ADDR, (const uint8_t *)&g_RGB_pwm, sizeof(g_RGB_pwm)); /* Persist defaults once. */
		if (write_ret != 0)
		{
			info("LED/WB init flash write default failed, addr=0x%X, ret=%u\r\n",
				 LED_BIN_WB_ADDR,
				 write_ret);
		}
	}
}

void h6_hal_set_sys_clk(uint8_t freq_current)
{
	hall_hal_sys_clk = freq_current;
}
/**
 * \brief     Frequency configuration
 * \param[in] None
 * \return    None
 */
void h6_hal_freq_config(void)
{
	GTRY_HASP->SCK_SPEC_SCALE = FREQUENCY_RANGE;
	GTRY_HASP->SCK_SPEC_STEP = (FREQUENCY_RATE<<8 | FREQUENCY_STEP) ;
}

/**
 * \brief     HASP HAL GPIO initialization
 *
 * \param[in] None
 * \return    None
 */


/**
 * \brief     HASP HAL GPIO initialization
 *
 * \param[in] None
 * \return    None
 */
#ifndef H6_HASP_GPIO_CFG_DEBUG
#define H6_HASP_GPIO_CFG_DEBUG 0
#endif

//void h6_hal_hasp_gpio_init(void)
//{
//    for (int group = 0; group < HASP_CHANNL_NUM; group++)
//    {
//    	//      Configure SCK
//        GTRY_GPIO->GPA_CFG[sck_pins[group]] = 	  ((0x11 - group) << GPIO_CFG_REUSE_POS) 	//HASP pin multiplexing		 复用HASP 引脚
//												| (POWER_LEVEL_CLK << GPIO_CFG_DRIVE_POS) 	//Drive strength level set	 驱动挡位
//												| (0<<GPIO_CFG_PULL_POS) 					//Pull-up/pull-down disabled 无上下拉
//												| (SR_LEVEL_CLK << GPIO_CFG_SR_POS)			//slew rate		   			 摆率
//												| (2 << GPIO_CFG_INDELAY_POS)				//Small hysteresis 			 小迟滞
//												| (0 << GPIO_CFG_ANE_POS)					//Analog channel disabled	 模拟通道不使能
//												| (0 << GPIO_CFG_DIR_POS)					//high impedance state		 高阻
//												| (1 << GPIO_CFG_IN_EN_POS);				//Input enabled				 输入使能
//        // Configure SDO ->  hall SDI
//        GTRY_GPIO->GPA_CFG[sdo_pins[group]] = 	  ((0x11 - group) << GPIO_CFG_REUSE_POS) 	//HASP pin multiplexing		 复用HASP 引脚
//												| (POWER_LEVEL_SDO << GPIO_CFG_DRIVE_POS) 	//Drive strength level set	 驱动挡位
//												| (0<<GPIO_CFG_PULL_POS) 					//Pull-up/pull-down disabled 无上下拉
//												| (SR_LEVEL_SDO << GPIO_CFG_SR_POS)			//slew rate		   			 摆率
//												| (2 << GPIO_CFG_INDELAY_POS)				//Small hysteresis 			 小迟滞
//												| (0 << GPIO_CFG_ANE_POS)					//Analog channel disabled	 模拟通道不使能
//												| (0 << GPIO_CFG_DIR_POS)					//high impedance state		 高阻
//												| (1 << GPIO_CFG_IN_EN_POS);				//Input enabled				 输入使能
//        // Configure SDI <- hall SDO
//        GTRY_GPIO->GPA_CFG[sdi_pins[group]] = 	  ((0x11 - group) << GPIO_CFG_REUSE_POS) 	//HASP pin multiplexing		 复用HASP 引脚
//												| (POWER_LEVEL_SDI << GPIO_CFG_DRIVE_POS) 	//Drive strength level set	 驱动挡位
//												| (0<<GPIO_CFG_PULL_POS) 					//Pull-up/pull-down disabled 无上下拉
//												| (SR_LEVEL_SDI << GPIO_CFG_SR_POS)			//slew rate		   			 摆率
//												| (2 << GPIO_CFG_INDELAY_POS)				//Small hysteresis 			 小迟滞
//												| (0 << GPIO_CFG_ANE_POS)					//Analog channel disabled	 模拟通道不使能
//												| (0 << GPIO_CFG_DIR_POS)					//high impedance state		 高阻
//												| (1 << GPIO_CFG_IN_EN_POS);				//Input enabled				 输入使能
//
//#if H6_HASP_GPIO_CFG_DEBUG
//        printf("hasp_gpio[%d] SCK pin=0x%lx cfg=0x%08lx SDI pin=0x%lx cfg=0x%08lx SDO pin=0x%lx cfg=0x%08lx\r\n",
//               group,
//               (unsigned long)sck_pins[group], (unsigned long)GTRY_GPIO->GPA_CFG[sck_pins[group]],
//               (unsigned long)sdi_pins[group], (unsigned long)GTRY_GPIO->GPA_CFG[sdi_pins[group]],
//               (unsigned long)sdo_pins[group], (unsigned long)GTRY_GPIO->GPA_CFG[sdo_pins[group]]);
//#endif
//    }
//}

//////ori
void h6_hal_hasp_gpio_init(void)
{
    for (int group = 0; group < HASP_CHANNL_NUM; group++)
    {
        uint32_t mux_val = (0x11 - group) << 16;  // Multiplexing function selection
//      Configure SCK                                                               Clear 5 reset   Multiplexing   Input enable   Set drive
        GTRY_GPIO->GPA_CFG[sck_pins[group]] =(GTRY_GPIO->GPA_CFG[sck_pins[group]] & ~(0x1f << 16)) | mux_val | 0x1 |  (POWER_LEVEL_CLK << 12);

        // Configure SDO
        GTRY_GPIO->GPA_CFG[sdo_pins[group]] =(GTRY_GPIO->GPA_CFG[sdo_pins[group]] & ~(0x1f << 16)) | mux_val | 0x1	|  (POWER_LEVEL_SDO << 12);

        // Configure SDI
        GTRY_GPIO->GPA_CFG[sdi_pins[group]] =(GTRY_GPIO->GPA_CFG[sdi_pins[group]] & ~(0x1f << 16)) | mux_val | 0x1	| (POWER_LEVEL_SDI << 12);
    }
}

uint32_t Get_Hosc_Fre_Div(void)
{
	uint32_t time_max = 0x0;
	uint32_t time_min = 0xFFFFFFFF;
	uint32_t time_total = 0x0;
	uint32_t time_temp = 0x0;
	uint32_t time_record[14] = {0x0};

	SET_REG32(0xE90000C8, 0x1);
	for(uint32_t i = 0x0;i<14;i++)
	{
		while(!((GTRY_GPIO->GPIO_DATAIN[0]>>10)&0x1));
		time_record[i] = GTRY_PLMT->MTIME;
		while((GTRY_GPIO->GPIO_DATAIN[0]>>10)&0x1);
	}
	while(GET_REG32(0xE90000C8));

	for(uint32_t i = 0x0;i<13;i++)
	{
		time_temp = time_record[i+1] - time_record[i];
		if(time_temp > time_max)
		{
			time_max = time_temp;
		}
		if(time_temp < time_min)
		{
			time_min = time_temp;
		}
		time_total += time_temp;
	}
	return 100000000*(13-2)/(time_total-time_max-time_min);
}

/**
 * \brief	HOSC Frequency Calibration Initialization
 * \param[in] f_target: Target frequency; Unit: MHz; This parameter is limited to any integer within the range of 20 to 90.
 * \param[in] f_tol: Error range; Unit: ±0.1%
 * \return 0 indicates successful calibration; any other value indicates a failure in calibration.
 */
uint32_t Hosc_Cali(uint32_t f_target, uint32_t f_tol)
{
	if((f_target < 20) || (f_target > 90) || (f_tol == 0))
	{
		return 1;
	}

	uint32_t reg_io_val = GET_REG32(0xF0900100+10*4);
	uint32_t reg0x30_val = GET_REG32(0xE9000030);
	uint32_t reg0x68_val = GET_REG32(0xE9000068);

	uint32_t f_tgt_div = ((f_target*MHz)>>6);
	uint32_t f_tgt_thre_h = (f_tgt_div*(1000+f_tol)/1000);
	uint32_t f_tgt_thre_l = (f_tgt_div*(1000-f_tol)/1000);

	SET_REG32(0xF0900100+10*4, 0x4021|(RY_HAL_DRV0_REUSE<<16));
	SET_REG32(0xE9000030, 0x8fff703f);
	SET_REG32(0xE9000068, 0x1);

	uint32_t frag = 0x0;
	uint32_t f_nearest = f_tgt_div;
	uint32_t f_temp = 0x0;
	for(uint32_t i = 0x0;i<0x7;i++)
	{
		SET_REG32(0xE90000DC, (0x7FUL<<3)|0x4);
		SET_REG32(0xE90000E0, (0x1UL<<3)|i);
		ry_delay_us(100);
		f_temp = Get_Hosc_Fre_Div();
		f_temp = (f_temp > f_tgt_div)? (f_temp - f_tgt_div): (f_tgt_div - f_temp);
		if(f_temp < f_nearest)
		{
			f_nearest = f_temp;
			frag = i;
		}
	}

	SET_REG32(0xE90000E0, (0x1UL<<3)|frag);

	uint32_t rough_low = 0x0;
	uint32_t rough_high = 0xFF;
	uint32_t rough = 0x0;

	uint32_t f_cali_overcnt = 10;
	do{
		rough = ((rough_low + rough_high) >> 1)&0xFF;
		SET_REG32(0xE90000DC, (rough<<3)|0x4);
		ry_delay_us(100);
		f_temp = Get_Hosc_Fre_Div();
		if((f_temp <= f_tgt_thre_h)&&((f_temp >= f_tgt_thre_l)))
		{
			break;
		}
		else if(f_temp > f_tgt_thre_h)
		{
			rough_high = rough-1;
		}
		else
		{
			rough_low = rough+1;
		}
	}while(--f_cali_overcnt);
	SET_REG32(0xF0900100+10*4, reg_io_val);
	SET_REG32(0xE9000030, reg0x30_val);
	SET_REG32(0xE9000068, reg0x68_val);
	if(f_cali_overcnt == 0)
	{
		return 2;
	}
	SET_REG32(0xE90000E0, (0x1UL<<3)|frag);
	SET_REG32(0xE90000DC, (rough<<3)|0x4);
	return 0;
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: hall_hal_hasp_init
 * Description  	: Initialize the HASP clock source, controller mode, and daisy-chain length.
 * END ****************************************************************************************************************/
void h6_hal_hasp_init(void)
{
//	#if	HOSC_TEST
//		printf("before.0xDC=0x%lx,0xE0=0x%lx,0x30=0x%lx,0x68=0x%lx\r\n",
//			   GTRY_HASP->INNER_CLK_INIT,
//			   GTRY_HASP->INNER_CLK_CTRL,
//			   GTRY_HASP->CTRL,
//			   GTRY_HASP->NSCK);
//	#endif

	/*************** HASP Clock Calibration *******************/
//	hasp_drv_clk_init();

#if	HOSC_TEST
	// Keep HOSC calibration compiled only when the global HOSC_TEST switch is enabled.
//	if (H6_HOSC_TEST_ENABLE)				// Run calibration only for 96% and full-size keyboards.
	{
	/**************** HOSC clock cali to 64M ****************/
		uint8 cali_ret  = 5;//Initialize to the non-calibration failure value
//		GTRY6xx_SHOW_DBG_H_0;
		cali_ret = Hosc_Cali(HOSC_CALI_CLOCK, 10);	// Calibrate HOSC to the configured target clock.  1.5ms
//		GTRY6xx_SHOW_DBG_L_0;
		printf("cali_ret=%d\r\n",cali_ret);			// Print the calibration result for bring-up/debug.
	}
#endif

	GTRY_HASP->CTRL =	HASP_EN|							// HASP enable
						CHXSCKMODE(0xff)|					// Channel SCK configuration
						BIT_NUM(0xf)|						// Single Hall HASP data bit configuration (+1)
						CHANNL_NUM(HASP_CHANNL_NUM-1)|		// Supported Hall HASP daisy chain count configuration
						HASP_DRV_SCK_DIV(HASP_SCK_DIV); 	// SCK divider setting

	GTRY_HASP->NSCK  = NSCK1(g_hasp_chain_length)|SECTION1_REG_DATA;

//#if	HOSC_TEST
//	printf("after.0xDC=0x%lx,0xE0=0x%lx,0x30=0x%lx,0x68=0x%lx\r\n",
//	       GTRY_HASP->INNER_CLK_INIT,
//	       GTRY_HASP->INNER_CLK_CTRL,
//		   GTRY_HASP->CTRL,
//		   GTRY_HASP->NSCK);
//#endif

}

/**
 * \brief     hall crc error detect
 * \param[in] None
 * \return    None
 */
uint32_t h6_hal_detect_crc_error_hall(void)
{
	uint32_t nsck_shadow = hasp_drv_get_nsck();
	uint32_t ctrl_shadow = hasp_drv_get_ctrl();

	uint32_t ret=1, k = 0, crc_err_num = 0 , chain_null_num = 0;
	uint16_t value_crc;
	uint8_t key_num = g_hasp_key_num; /* Use the selected layout slot count directly; supported layouts use at most 112 slots. */

	hasp_drv_nsck_set(NSCK1(g_hasp_chain_length)|SECTION1_KEY_DATA);
	hasp_drv_set_ctrl(hasp_drv_get_ctrl()&(~HASP_CONTINUE_CONV));

	for(uint8_t n = 0; n < HASP_CRC_ERROR_HALL_TIMES; n++)
	{
		ry_delay_us(200);
		hasp_drv_start_work();

		for(uint8_t i = 0; i < key_num; i++)
		{
			if(chain_miss_buff_reg[i] == 0)
			{
				//GTRY_HASP->RXRAWDATA[i] = 0x0;
				chain_null_num++;
				hall_hal_set_self_check_value(i, HALL_SELF_CHECK_EMPTY_VALUE); /* Report empty Hall-chain slots consistently with the reference project. */

//				debug("hall chain->%d:%d sample = %d, it is hall null!\r\n", i%HASP_CHANNL_NUM,i/HASP_CHANNL_NUM,(GTRY_HASP->RXRAWDATA[i] & 0x3FFF));
				continue;
			}
			value_crc = 0;
			for(k = 0; k < 14; k = k+2)
			{
				value_crc ^= ((GTRY_HASP->RXRAWDATA[i]>>k) & 0x3);
			}
			if(value_crc == (GTRY_HASP->RXRAWDATA[i]>>14))
			{
				if (hall_hal_get_self_check_value(i) < HALL_SELF_CHECK_CLOCK_CALI_ERROR_VALUE)
				{
					hall_hal_set_self_check_value(i, HALL_SELF_CHECK_CRC_OK_VALUE); /* Mark CRC-good Halls only when no earlier error was latched. */
				}
//				debug("hall chain->%d:%d sample = %d, hall crc is ok!\r\n", i%HASP_CHANNL_NUM,i/HASP_CHANNL_NUM,(GTRY_HASP->RXRAWDATA[i] & 0x3FFF));
			}
			else
			{
				crc_err_num++;
				hall_hal_set_self_check_value(i, HALL_SELF_CHECK_CRC_ERROR_VALUE); /* Latch this stage-specific CRC failure for SELF_CHECK reporting. */
//				debug("hall chain->%d:%d sample = %d, hall crc is error!\r\n", i%HASP_CHANNL_NUM,i/HASP_CHANNL_NUM,GTRY_HASP->RXRAWDATA[i]);
			}
		}
		wdt_reset_timer();
		if((g_hasp_key_num - crc_err_num - chain_null_num)!= g_key_board_num)
		{
//			debug("hall crc error count (%d)\n", crc_err_num);
			ret= 0;
		}
		else
		{
//			debug("hall crc error count (%d)\n", crc_err_num);
		}
		crc_err_num = 0;
		chain_null_num = 0;
	}
	hasp_drv_nsck_set(nsck_shadow);
	hasp_drv_set_ctrl(ctrl_shadow);
	if(ret)
	{
		printf("******hall sample crc detecting successfully!!!!!!******\r\n");

	}else
	{
		h6_error_sign = 1U; /* Keep failed CRC status visible to manufacturing SELF_CHECK reads. */
		printf("******hall sample crc error detecting failed!!!!!!******\r\n");
	}
	return ret;
}

/**
 * \brief     HASP linearization table setup
 *
 * \param[in] *tab: Pointer to key raw data table
 * \param[in] num: Length of raw data table
 * \param[in] table_len: Length of linearization table after expansion
 * \return    None
 */
void h6_hal_hasp_linetab_build(uint16_t* tab ,uint32_t num ,uint32_t table_len)
{
	uint32_t i = 0 , j = 0 , k = 0,len = 0 ,section_len = 0,linetab_data = 0, reg_data = 0;
	uint32_t section_start = 0;

	section_len = table_len/(num-1);
	for(i = 0 ; i < (num - 1) ;i++ )
	{
		len = tab[i+1] - tab[i];
		if(i)
		{
			section_start = 1;
			section_len = table_len/(num-1);
		}
		for(j = section_start; j <= len ;j++)
		{
			linetab_data = (uint32_t)(j * (section_len / ((float)len)) + 0.5);
			reg_data = linetab_data+i*section_len;
			if(reg_data)
			{
				GTRY_HASP->LINETB[k] = reg_data - 1;
			}
			else
			{
				GTRY_HASP->LINETB[k] = reg_data;
			}
			k++;
			if(k>=8192)
			{
				k = 8191;
			}
		}
	}
	if(i<8191)
	{
		while(k<8192)
		{
			GTRY_HASP->LINETB[k] = linetab_data+(i-1)*section_len-1;
			k++;
		}
	}
}

void h6_hal_single_time_inst(uint8_t do_pwm_write)
{
	uint16_t td_value;
//	hasp_drv_stop_work();
//	while(!(GTRY_HASP->INTST & ONCE_TRANS_END_INT_MASK));

	h6_drv_interrupt_time_inst();
//	td_value = freq_main_td1[hall_hal_sys_clk];
//	hasp_drv_td_set(HASP_TD2, td_value);
//	td_value = freq_main_td2_s[hall_hal_sys_clk];
//	hasp_drv_td_set(HASP_TD3, td_value);
//	td_value = freq_main_wait[hall_hal_sys_clk];
//	hasp_drv_td_set(HASP_TD5, td_value);

	if (do_pwm_write)
	{
		td_value = freq_main_m[hall_hal_sys_clk];
		hasp_drv_td_set(HASP_TD1, td_value);
		td_value = freq_main_td4[hall_hal_sys_clk];
		hasp_drv_td_set(HASP_TD6, td_value);
		td_value = freq_main_td5[hall_hal_sys_clk];
		hasp_drv_td_set(HASP_TD7, td_value);

	}
	else
	{
		td_value = freq_main_s[hall_hal_sys_clk];
		hasp_drv_td_set(HASP_TD1, td_value);
		hasp_drv_td_set(HASP_TD6, 0);
		hasp_drv_td_set(HASP_TD7, 0);
	}
}

/**
 * \brief		The hall selects different timings in different modes,
 *
 * \param[in] 	f_current : Current system frequency
 * \param[in] 	do_pwm_write : Is it necessary to include PWM
 * \param[in] 	bt_mode
 * \return 		None
 */
void h6_hal_variable_time_inst(uint8_t do_pwm_write)
{
    uint16_t td_value;
    hasp_drv_stop_work();
//    while(!(GTRY_HASP->INTST & ONCE_TRANS_END_INT_MASK));

    hasp_drv_td_clear();
    hasp_drv_nsck_set(NSCK1(g_hasp_chain_length)|SECTION1_KEY_DATA);

#if CONTINUOUS_MODE
    td_value = freq_main_td1[hall_hal_sys_clk];
    hasp_drv_td_set(HASP_TD2, td_value);

    td_value = freq_main_td2_s[hall_hal_sys_clk];
    hasp_drv_td_set(HASP_TD3, td_value);

	td_value = freq_main_wait[hall_hal_sys_clk];
	hasp_drv_td_set(HASP_TD5, td_value);
#else
    td_value = freq_main_td1[hall_hal_sys_clk];
    hasp_drv_td_set(HASP_TD2, td_value);
    td_value = freq_main_td2_s[hall_hal_sys_clk];
    hasp_drv_td_set(HASP_TD3, td_value);
	td_value = freq_main_wait[hall_hal_sys_clk];
	hasp_drv_td_set(HASP_TD5, td_value);
#endif

	if (do_pwm_write)
	{
		td_value = freq_main_m[hall_hal_sys_clk];
	    hasp_drv_td_set(HASP_TD1, td_value);
	    td_value = freq_main_td4[hall_hal_sys_clk];
		hasp_drv_td_set(HASP_TD6, td_value);
		td_value = freq_main_td5[hall_hal_sys_clk];
		hasp_drv_td_set(HASP_TD7, td_value);

	}
	else
	{
		td_value = freq_main_s[hall_hal_sys_clk];
	    hasp_drv_td_set(HASP_TD1, td_value);
		hasp_drv_td_set(HASP_TD6, 0);
		hasp_drv_td_set(HASP_TD7, 0);
	}
}

//void h6_hal_variable_time_inst(uint8_t do_pwm_write)
//{
//    uint16_t td_value;
//    hasp_drv_stop_work();
////    while(!(GTRY_HASP->INTST & ONCE_TRANS_END_INT_MASK));
//
//    hasp_drv_td_clear();
//    hasp_drv_nsck_set(NSCK1(g_hasp_chain_length)|SECTION1_KEY_DATA);
////	h6_drv_convert_time_inst();
//
////    td_value = freq_main_td1[hall_hal_sys_clk];
////    hasp_drv_td_set(HASP_TD2, td_value);
////    td_value = freq_main_td2_s[hall_hal_sys_clk];
////    hasp_drv_td_set(HASP_TD3, td_value);
////	td_value = freq_main_wait[hall_hal_sys_clk];
////	hasp_drv_td_set(HASP_TD5, td_value);
////
////	if (do_pwm_write)
////	{
////		td_value = freq_main_m[hall_hal_sys_clk];
////	    hasp_drv_td_set(HASP_TD1, td_value);
////	    td_value = freq_main_td4[hall_hal_sys_clk];
////		hasp_drv_td_set(HASP_TD6, td_value);
////		td_value = freq_main_td5[hall_hal_sys_clk];
////		hasp_drv_td_set(HASP_TD7, td_value);
////
////	}
////	else
////	{
////		td_value = freq_main_s[hall_hal_sys_clk];
////	    hasp_drv_td_set(HASP_TD1, td_value);
////		hasp_drv_td_set(HASP_TD6, 0);
////		hasp_drv_td_set(HASP_TD7, 0);
////	}
//}

/**
 * \brief HASP sampling start (supports multiple working modes)
 *
 * Configures HASP hardware according to the passed mode parameter and starts the sampling process.
 * This function encapsulates several sampling modes: interrupt single chain, PWM full chain, continuous full chain, single full chain.
 * It calls driver-level basic operation functions to complete register configuration and initialization.
 *
 * \param[in] mode Sampling mode selection
 *                 - 0: Interrupt mode, single chain usage
 *                 - 1: Interrupt mode, all chains usage
 *                 - 2: Continuous mode, all chains usage
 *                 - 3: Single-shot mode, all chains usage
 * \return None
 */
void h6_hal_start_sample(uint8_t mode)
{
    uint32_t ctrl = hasp_drv_get_ctrl();

    // Clear all conflicting bits that will be reconfigured per mode
    ctrl &= ~(HASP_CONTINUE_CONV | HASP_SINGLE_CONV | CHXSCKMODE(0xff) | SDI_AUTO_CONV_EN);
    // CRC check is enabled in all modes
    ctrl |= CRCCHECK_EN;

    switch (mode)
    {
        case MODE_INTERRUPT_SINGLE_CHAIN_SAMPLE:
            ctrl |= SDI_AUTO_CONV_EN;						// Single chain, SDI auto conversion
            break;

        case MODE_INTERRUPT_ALL_CHAIN_SAMPLE:
            ctrl |= CHXSCKMODE(0xff);						// All chains, no SDI auto
            break;

        case MODE_CONTINUOUS_ALL_CHAIN_SAMPLE:
            ctrl |= HASP_CONTINUE_CONV | CHXSCKMODE(0xff);	// Continuous, all chains
            break;

        case MODE_SINGLE_ALL_CHAIN_SAMPLE:
            ctrl |= HASP_SINGLE_CONV | CHXSCKMODE(0xff);	// Single shot, all chains
            break;

        default:
            return;
    }

    hasp_drv_set_ctrl(ctrl);

    // Continuous mode requires an extra start trigger
    if (mode == MODE_CONTINUOUS_ALL_CHAIN_SAMPLE)
    {
        hasp_drv_start_work();
    }
}

/**
 * \brief     HASP parameter configuration
 *
 * \param[in] None
 * \return    None
 */
unsigned int h6_hal_hasp_config(void)
{
	GTRY_HASP->MAX_ABSDATA = MAX_ABSDATA_VALUE;
	GTRY_HASP->MIN_ABSDATA = MIN_ABSDATA_VALUE;
	GTRY_HASP->RAW2ABS_REF = RAW2ABS_REF_VALUE;

	GTRY_HASP->LPCFG = LPCFG_DATABIT(0)
						|LPCFG_BTOPLP(22)
						|LPCFG_KDLP(0);

	GTRY_HASP->LPCFG_RECVSTEP = 0;

	GTRY_HASP->FDDEL_KDLP =	FDDEL_KDLP_VALUE;
	GTRY_HASP->NOISEV_KDLP = NOISEV_KDLP_VALUE;
	GTRY_HASP->FDDEL_BTOPLP = FDDEL_BTOPLP_VALUE;
	GTRY_HASP->NOISEV_BTOPLP = NOISEV_BTOPLP_VALUE;
	GTRY_HASP->BTOPLP_PDLT = BTOPLP_PDLT_VALUE;
	GTRY_HASP->BTOPLP_NDLT = BTOPLP_NDLT_VALUE;

	// Baseline TOP
	GTRY_HASP->BTOP_CALCYCLE = BTOP_CALCYCLE_VALUE;
	GTRY_HASP->BTOP_ADDCNT = BTOP_ADDCNT_VALUE;
	GTRY_HASP->BTOP_SUBCNT = BTOP_SUBCNT_VALUE;
	GTRY_HASP->BTOP_ADDDLT = BTOP_ADDDLT_VALUE;
	GTRY_HASP->BTOP_SUBDLT = BTOP_SUBDLT_VALUE;
	GTRY_HASP->BTOP_PHYST = BTOP_PHYST_VALUE;
	GTRY_HASP->BTOP_NHYST = BTOP_NHYST_VALUE;
	GTRY_HASP->BTOP_PDLT = BTOP_PDLT_VALUE;
	GTRY_HASP->BTOP_NDLT = BTOP_NDLT_VALUE;

	// Baseline BOTTOM
	GTRY_HASP->BBOT_CALCYCLE = BBOT_CALCYCLE_VALUE;
	GTRY_HASP->BBOT_LOCKCNT = BBOT_LOCKCNT_VALUE;
	GTRY_HASP->BBOT_ADDDLT = BBOT_ADDDLT_VALUE;
	GTRY_HASP->BBOT_SUBDLT = BBOT_SUBDLT_VALUE;
	GTRY_HASP->BBOT_HYST1 = BBOT_HYST1_VALUE;
	GTRY_HASP->BBOT_HYST2 = BBOT_HYST2_VALUE;
	GTRY_HASP->BBOT_PDLT = BBOT_PDLT_VALUE;
	GTRY_HASP->BBOT_NDLT = BBOT_NDLT_VALUE;

	GTRY_HASP->BBOT_LKRLS = BBOT_LKRLS_VALUE;
	GTRY_HASP->PRESSDN_CFG = PRESSDN_CFG_VALUE;

	for(uint8_t i = 0; i < g_hasp_key_num; i++)
	{
		GTRY_HASP->OFFSET[i]  = KEY_OFFSET_REF;
		GTRY_HASP->DATAERR[i] = DATAERR_INIT_VALUE;
	}

	/* Restore bottom baseline from Flash; invalid or blank data falls back to default HASP filter seeds. */
	hall_hal_load_bottom_baseline_from_flash();

	GTRY_HASP->CTRL &= ~(AUTO_UPDATE_BTOP_EN
						|BTOPLP_CAL_EN
						|BTOPLP_EN

						|AUTO_UPDATE_BBOT_EN
						|BBOTLP_CAL_EN
						|BBOTLP_EN

						|KDATALP_EN);

	GTRY_HASP->CTRL |= (/*AUTO_UPDATE_BTOP_EN |
						BTOPLP_CAL_EN |*/
						BTOPLP_EN |

						AUTO_UPDATE_BBOT_EN|
						BBOTLP_CAL_EN|
						BBOTLP_EN

						/*KDATALP_EN*/);
	return 0;
}

/**
 * \brief     Hall multi-chip write
 *
 * \param[in] addr : Register address
 * \param[in] data : Data to write
 * \param[in] size : Number of chips
 * \return    None
 */
void h6_hal_reg_multi_write(uint8_t addr, uint16_t data,uint32_t size)
{
	uint32_t i = 0;
	uint16_t wr_buf[HASP_KEY_NUM];

	for(i = 0; i < size; i ++)
	{
		wr_buf[i] = data;
	}
	h6_drv_write_reg_cmd(addr,wr_buf, size);
}


/**
 * \brief  Write top rawdata (14-bit) into H6 registers 0x2D (low byte) and
 *         0x2E (high byte) for all keys.  These two registers together form
 *         the manual baseline initial value that H6 uses when BASL_EN=1.
 *
 * \param[in] top_rawdata  array of 14-bit top rawdata values per key
 * \param[in] size         number of keys (typically g_hasp_key_num)
 */
void hall_hal_write_top_baseline(uint16_t *top_rawdata, uint32_t size)
{
	uint16_t reg2d_buf[HASP_KEY_NUM_MAX];
	uint16_t reg2e_buf[HASP_KEY_NUM_MAX];

	for (uint32_t i = 0; i < size; i++)
	{
		reg2d_buf[i] = top_rawdata[i] & 0xFF;        /* low  8 bits -> REG 0x2D */
		reg2e_buf[i] = (top_rawdata[i] >> 8) & 0x3F; /* high 6 bits -> REG 0x2E */
	}

	hasp_drv_stop_work();//stop hasp auto sampling
	h6_drv_write_reg_cmd(CONGFIG_REG2D, reg2d_buf, size);

	hasp_drv_stop_work();//stop hasp auto sampling
	h6_drv_write_reg_cmd(CONGFIG_REG2E, reg2e_buf, size);

#if HALL_TOP_BASELINE_PRINT_EN
//	hall_hal_dump_top_baseline_regs(size);
#endif
}

uint8_t hall_hal_load_top_baseline_from_flash(void)
{
	uint8_t flash_buf[HASP_KEY_NUM * 2];
	uint16_t top_rawdata[HASP_KEY_NUM];
	uint32_t key_num = hall_hal_get_flash_key_num(); /* Bound the restore length to the current supported layout size. */
	unsigned int expected_len = key_num * 2U; /* Top baseline is stored as little-endian uint16 per key. */
	unsigned int actual_read;
	uint8_t all_ff = 1;

	actual_read = QSPI_EXT_FLASH_read(TBL_CALI_ADDR, flash_buf, expected_len); /* Load saved top-baseline calibration. */
	if (actual_read != expected_len)
	{
		return 0; /* Read failure keeps the original auto-fill baseline path enabled. */
	}

	for (uint32_t i = 0; i < expected_len; i++)
	{
		if (flash_buf[i] != 0xFF)
		{
			all_ff = 0; /* Any programmed byte means a production calibration payload exists. */
			break;
		}
	}

	if (all_ff)
	{
		return 0; /* Blank Flash means no manual top baseline should be applied. */
	}

	for (uint32_t i = 0; i < key_num; i++)
	{
		top_rawdata[i] = (uint16_t)flash_buf[i * 2U] |
						 ((uint16_t)flash_buf[i * 2U + 1U] << 8); /* Rebuild each saved 14-bit raw value. */
	}

	hall_hal_write_top_baseline(top_rawdata, key_num); /* Program H6 REG 0x2D/0x2E with restored manual baseline. */
	return 1;
}


/*
 * REG 0x29 and REG 0x2A are LNA-dependent trimming registers. They must be
 * derived from REG 0x0B[7:4] every time the gain changes.
 */
static void h6_hal_get_lna_trim_regs(uint8_t gain_config0, uint16_t *reg29, uint16_t *reg2a)
{
	uint8_t lna_gain =
		(h6_hal_normalize_gain_config0(gain_config0) & H6_GAIN_CONFIG0_LNA_MASK) >>
		H6_GAIN_CONFIG0_LNA_SHIFT;

	switch (lna_gain)
	{
	case H6_LNA_GAIN_16X:
		*reg29 = 0x4E;
		*reg2a = 0x32;
		break;
	case H6_LNA_GAIN_24X:
		*reg29 = 0x60;//0x60
		*reg2a = 0x28;
		break;

	case H6_LNA_GAIN_48X:
		*reg29 = 0xA0;
		*reg2a = 0x40;
		break;

	case H6_LNA_GAIN_96X:
		*reg29 = 0xF0;
		*reg2a = 0x60;
		break;

	default:
		*reg29 = 0x4E;
		*reg2a = 0x32;
		break;
	}
}

/*
 * Write REG 0x0B together with its dependent REG 0x29/0x2A values. Keeping the
 * three writes behind one API prevents Flash restore and manufacturing writes
 * from leaving the analog LNA trim registers out of sync with REG 0x0B[7:4].
 */
void h6_hal_write_gain_config0_table(uint16_t *gain_config0, uint32_t size)
{
	uint16_t reg0b_buf[HASP_KEY_NUM_MAX];
	uint16_t reg29_buf[HASP_KEY_NUM_MAX];
	uint16_t reg2a_buf[HASP_KEY_NUM_MAX];

	for (uint32_t i = 0; i < size; i++)
	{
		uint8_t normalized_gain = h6_hal_normalize_gain_config0((uint8_t)gain_config0[i]);

		reg0b_buf[i] = normalized_gain;
		h6_hal_get_lna_trim_regs(normalized_gain, &reg29_buf[i], &reg2a_buf[i]);
	}
	wdt_reset_timer();

	hasp_drv_stop_work();//stop hasp auto sampling
	h6_drv_write_reg_cmd(CONGFIG_REGB, reg0b_buf, size);

	hasp_drv_stop_work();//stop hasp auto sampling
	h6_drv_write_reg_cmd(CONGFIG_REG29, reg29_buf, size);

	hasp_drv_stop_work();//stop hasp auto sampling
	h6_drv_write_reg_cmd(CONGFIG_REG2A, reg2a_buf, size);
}

/*
 * Uniform-gain helper for default initialization and host commands that apply
 * the same REG 0x0B value to every H6 device in the chain.
 */
void h6_hal_write_gain_config0_uniform(uint8_t gain_config0, uint32_t size)
{
	uint16_t gain_buf[HASP_KEY_NUM_MAX];
	uint8_t normalized_gain = h6_hal_normalize_gain_config0(gain_config0);

	for (uint32_t i = 0; i < size; i++)
	{
		gain_buf[i] = normalized_gain;
	}

	h6_hal_write_gain_config0_table(gain_buf, size);
}
/**
 * \brief       Hall batch write PWM
 *
 * \param[in] pwmch : PWM channel
 * \param[in] data : Data to write
 * \param[in] size : Number of chips
 * \return    None
 */
void h6_hal_pwm_multi_write(uint16_t pwmch, uint16_t data, uint32_t size)
{
	uint32_t i = 0;
	uint16_t wr_buf[HASP_KEY_NUM] = {0};
	for(i = 0; i < size; i++)
	{
		wr_buf[i] = pwmch|data;
	}
	h6_drv_w_pwm_reg_cmd(wr_buf, size);
}

/**
 * \brief     h6 clock calibration
 *
 * \param[in] None
 * \return    1: calibration successful, 0: calibration failed
 */
unsigned int h6_hal_clock_cali(void)
{
	uint8_t i,j;
	uint32_t cali_num = 0;
	uint8_t key_num = g_hasp_key_num; /* Use the selected layout slot count directly; supported layouts use at most 112 slots. */
	uint8_t clock_fail[HASP_KEY_NUM] = {0}; /* Track the failed Hall slots from the latest clock calibration attempt. */

	for(j = 0; j < HALL_CHECK_COUNTS; j++)
	{
		memset(clock_fail, 0, sizeof(clock_fail)); /* Keep only the failed keys from the latest calibration attempt. */
		h6_drv_read_instru_count_cmd(g_hasp_key_num);
		for(i = 0; i < key_num; i ++)
		{
			hall_clock_cali_value[i] = (uint16_t)(GTRY_HASP->RXRAM[i] & 0x3ff);
			if(!chain_miss_buff_reg[i])
			{
//				printf("hall chain->%d:%d clock count = 0x%x, it is hall null!\r\n", i%HASP_CHANNL_NUM,i/HASP_CHANNL_NUM, GTRY_HASP->RXRAM[i]);
				hall_hal_set_self_check_value(i, HALL_SELF_CHECK_EMPTY_VALUE); /* Mark empty chain slots the same way as the reference project. */
			}
			else
			{

				if((((GTRY_HASP->RXRAM[i]&0x3ff) >= 0x7c) && ((GTRY_HASP->RXRAM[i]&0x3ff) <= 0x84)))
				{
					cali_num++;
					hall_hal_set_self_check_value(i, HALL_SELF_CHECK_CRC_OK_VALUE); /* Mark this Hall good for the clock-calibration stage. */
//					printf("hall chain->%d:%d clock count = 0x%x, clock cali ok!\r\n", i%HASP_CHANNL_NUM,i/HASP_CHANNL_NUM, GTRY_HASP->RXRAM[i]);
				}
				else
				{
//					printf("hall chain->%d:%d clock count = 0x%x, clock cali failed!\r\n", i%HASP_CHANNL_NUM,i/HASP_CHANNL_NUM, GTRY_HASP->RXRAM[i]);
					clock_fail[i] = 1U; /* Remember which Hall IC failed the final clock-count range check. */
				}
			}
		}
		wdt_reset_timer();
//		printf("cali_num:%d\n",cali_num);
		if(cali_num == g_key_board_num)
		{
			break;
		}
		cali_num = 0;
	}
	ry_delay_ms(20);

	if(j == HALL_CHECK_COUNTS)
	{
		for(i = 0; i < key_num; i++)
		{
			if(chain_miss_buff_reg[i] && clock_fail[i])
			{
				hall_hal_set_self_check_value(i, HALL_SELF_CHECK_CLOCK_CALI_ERROR_VALUE); /* Report the bad Hall slot when all calibration retries fail. */
			}
		}
		h6_error_sign = 1U; /* Stop later self-check stages from overwriting the first detected failure cause. */
		printf("******hall clock calibration failed!!!!!!******\r\n");
		return 0;
	}
	else
	{
		printf("******hall clock calibration successfully!!!!!!******\r\n");
		return 1;
	}

}

/**
 * \brief     space left and right current
 *
 * \param[in] r_cur : red   channel current
 * \param[in] g_cur : blue  channel current
 * \param[in] b_cur : green channel current
 * \return    None
 */
void h6_w_current_space_LR(uint16_t r_cur, uint16_t g_cur, uint16_t b_cur)
{

	uint16_t wr_buf[HASP_KEY_NUM_MAX] = {0};

	wr_buf[g_space_r_l] = r_cur;
	wr_buf[g_space_r_r] = r_cur;
	wr_buf[g_space_g_l] = g_cur;
	wr_buf[g_space_g_r] = g_cur;
	wr_buf[g_space_b_l] = b_cur;
	wr_buf[g_space_b_r] = b_cur;

	hasp_drv_stop_work();//stop hasp auto sampling
	h6_drv_write_reg_cmd(PWM0_CURR_REG, wr_buf, g_hasp_key_num);
}

void h6_w_pwm_space_LR(uint16_t r, uint16_t g, uint16_t b)
{
	uint16_t wr_buf[HASP_KEY_NUM_MAX] = {0};

	wr_buf[g_space_r_l] = H6_PWM_LED0_DUTY | r;
	wr_buf[g_space_r_r] = H6_PWM_LED0_DUTY | r;
	wr_buf[g_space_g_l] = H6_PWM_LED0_DUTY | g;
	wr_buf[g_space_g_r] = H6_PWM_LED0_DUTY | g;
	wr_buf[g_space_b_l] = H6_PWM_LED0_DUTY | b;
	wr_buf[g_space_b_r] = H6_PWM_LED0_DUTY | b;

	h6_drv_w_pwm_reg_cmd(wr_buf, g_hasp_key_num);
}

void stop_sampling_pwm_duty_adjust(uint16_t r_duty, uint16_t g_duty, uint16_t b_duty)
{
	h6_hal_pwm_multi_write(H6_PWM_LED1_DUTY, r_duty, g_hasp_key_num);
	h6_hal_pwm_multi_write(H6_PWM_LED2_DUTY, g_duty, g_hasp_key_num);
	h6_hal_pwm_multi_write(H6_PWM_LED3_DUTY, b_duty, g_hasp_key_num);
	h6_w_pwm_space_LR(r_duty, g_duty, b_duty);

	h6_drv_w_pwm_time_clr();
}

/**
 * \brief     h6 PWM typical configuration
 * \param[in] None
 * \return    None
 */
void h6_hal_pwm_init(void)
{
	hall_hal_led_wb_init_from_flash(); /* Restore LED bin and RGB white-balance current before programming H6. */

	h6_hal_reg_multi_write(PWM0_CURR_REG, 0x0, g_hasp_key_num);
	h6_hal_reg_multi_write(PWM1_CURR_REG, R_CUR, g_hasp_key_num); /* Apply restored red-channel current. */
	h6_hal_reg_multi_write(PWM2_CURR_REG, G_CUR, g_hasp_key_num); /* Apply restored green-channel current. */
	h6_hal_reg_multi_write(PWM3_CURR_REG, B_CUR, g_hasp_key_num); /* Apply restored blue-channel current. */

	h6_hal_reg_multi_write(CONGFIG_REG9, 0x25, g_hasp_key_num);
	h6_hal_pwm_multi_write(H6_PWM_TOTAL_PERIOD,257,g_hasp_key_num);

	h6_hal_pwm_multi_write(H6_PWM_LED1_DUTY,0xFF,g_hasp_key_num);//r
	h6_hal_pwm_multi_write(H6_PWM_LED2_DUTY,0xFF,g_hasp_key_num);//g
	h6_hal_pwm_multi_write(H6_PWM_LED3_DUTY,0xFF,g_hasp_key_num);//b

	h6_w_current_space_LR(R_CUR, G_CUR, B_CUR); /* Keep left/right spacebar current aligned with restored RGB current. */
	h6_w_pwm_space_LR(0xFF, 0xFF, 0xFF);
}

/**
 * \brief     h6 soft reset
 * \param[in] None
 * \return    None
 */
void h6_hal_soft_reset(void)
{
	uint32_t nsck_shadow = hasp_drv_get_nsck();
	uint32_t ctrl_shadow = hasp_drv_get_ctrl();

	hasp_drv_set_ctrl(hasp_drv_get_ctrl()&(~HASP_CONTINUE_CONV));

	GTRY_HASP->START = HASP_STOP;
	hasp_drv_td_init();

	h6_drv_soft_rst_time_inst();
	hasp_drv_nsck_set(NSCK1(0));

	hasp_drv_start_work_not_wait();
	ry_delay_ms(2);

	hasp_drv_nsck_set(nsck_shadow);
	hasp_drv_set_ctrl(ctrl_shadow);
}

/**
 * \brief     h6 typical configuration
 * \param[in] None
 * \return    None
 */
void h6_hal_type_mode(void)
{
	h6_hal_reg_multi_write(CONGFIG_REGC , 0x83 ,g_hasp_key_num);

	//Write fix value
	h6_hal_reg_multi_write(CONGFIG_REG26 , 0x0A , g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG27 , 0x05 , g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG28 , 0x04 , g_hasp_key_num);

	h6_hal_reg_multi_write(CONGFIG_REG13 , 0x7F ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG15 , 0x05 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG16 , 0x02 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG18 , 0x03 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG1C , 0xFF ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG1E , 0x80 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG20 , 0x80 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG25 , 0x29 ,g_hasp_key_num);
}

/**
 * \brief     h6 low power mode 1
 * \param[in] None
 * \return    None
 */
void h6_hal_lp_mode1(void)
{
	h6_hal_reg_multi_write(CONGFIG_REG13 , 0x5F ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG15 , 0x85 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG16 , 0x01 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG18 , 0x03 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG1C , 0x7F ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG1E , 0x80 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG20 , 0x80 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG25 , 0x28 ,g_hasp_key_num);
}

/**
 * \brief     h6 low power mode 2
 * \param[in] None
 * \return    None
 */
void h6_hal_lp_mode2(void)
{
	h6_hal_reg_multi_write(CONGFIG_REG13 , 0x7F ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG15 , 0x05 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG16 , 0x01 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG18 , 0x0D ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG1C , 0x7F ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG1E , 0x40 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG20 , 0x40 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG25 , 0x26 ,g_hasp_key_num);
}

/**
 * \brief     h6 typical configuration
 * \param[in] None
 * \return    None
 *
 * During offset calibration, the Hall-related registers are modified;
 * after offset calibration, all relevant registers other than registers 0x36 and 0x37 must be reset to their initial values.
 */
void offset_cali_reg_recovery(void)
{
	uint16_t gain_table[HASP_KEY_NUM] = {0};

	/**0xF = 0x04, 0x10 = 0x20, continuous conversion time set to 28 µs**/
	h6_hal_reg_multi_write(CONGFIG_REGF  , 0x07,g_hasp_key_num);//Samp  interval of 28us
	h6_hal_reg_multi_write(CONGFIG_REG10 , 0x20 , g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG32, 0x0F | (KEY_DOWN_INCREASES<<6) ,g_hasp_key_num);

	/* Apply the same synchronized table used by calibration, including REG0B's
	 * LNA-dependent REG29/REG2A trims for normal operation. */
	hall_gain_load_final_table(gain_table, g_hasp_key_num);
	h6_hal_write_gain_config0_table(gain_table, g_hasp_key_num);

	//Write fix value
	h6_hal_reg_multi_write(CONGFIG_REG26 , 0x0A , g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG27 , 0x05 , g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG28 , 0x04 , g_hasp_key_num);

	/**Enable continuous conversion**/
	h6_hal_reg_multi_write(CONGFIG_REG33,  0x84, g_hasp_key_num);//0x84
}

/**
 * \brief     h6 low power mode 2
 * \param[in] None
 * \return    None
 */
void h6_hal_reg_set_mode()
{
	uint8_t ret = 0;
	uint16_t gain_table[HASP_KEY_NUM] = {0};

	/* Build one complete table for every Flash outcome, then keep REG0B and its
	 * LNA-dependent REG29/REG2A trims synchronized. */
	hall_gain_load_final_table(gain_table, g_hasp_key_num);
	h6_hal_write_gain_config0_table(gain_table, g_hasp_key_num);

	h6_hal_type_mode();

	h6_hal_reg_multi_write(CONGFIG_REGF , 0x07,g_hasp_key_num);//Samp  interval of 28us//07
	h6_hal_reg_multi_write(CONGFIG_REG1B, 0x1 ,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG2B, H6_TH,g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG2C, 0x40 ,g_hasp_key_num);

	h6_hal_reg_multi_write(CONGFIG_REG32, 0x01 | (KEY_DOWN_INCREASES<<6) ,g_hasp_key_num);//0x0F

	ret = hall_hal_load_top_baseline_from_flash(); /* Try to restore production top baseline into H6 REG 0x2D/0x2E. */
	if (ret)
	{
		/* Calibrated Flash data is already loaded, so skip the auto-fill trigger. */
	}
	else
	{
		h6_hal_reg_multi_write(CONGFIG_REG2F, 0x02, g_hasp_key_num); /* Select auto-fill baseline configuration page. */
		h6_hal_reg_multi_write(CONGFIG_REG33, (1<<7)|(1<<0), g_hasp_key_num); /* Trigger original top-baseline auto-fill. */

		ry_delay_us(200);
		h6_hal_reg_multi_write(CONGFIG_REG33, (1<<7), g_hasp_key_num); /* Clear auto-fill trigger bit after settling time. */
	}
	h6_hal_reg_multi_write(CONGFIG_REG2F, 0x14, g_hasp_key_num);


	h6_hal_reg_multi_write(CONGFIG_REG30, 0x20, g_hasp_key_num);
	h6_hal_reg_multi_write(CONGFIG_REG31, 0x20, g_hasp_key_num);

//	h6_hal_reg_multi_write(CONGFIG_REGA,  HALLPLATE_SENSITIVE, g_hasp_key_num);
//	h6_drv_read_reg_cmd(CONGFIG_REGA, g_hasp_key_num);
//	for(uint8_t i = 0; i < g_hasp_key_num; i++)
//	{
//		printf("hall chain->%d:%d REGA = 0x%02x\r\n", i%HASP_CHANNL_NUM, i/HASP_CHANNL_NUM, (GTRY_HASP->RXRAM[i] & 0xFF));
//	}

	h6_hal_reg_multi_write(CONGFIG_REG33, 0x84, g_hasp_key_num);//0x84  0x14
}

unsigned int h6_hal_reg_write_read_verify(void)
{
	uint32_t ret = 1;
	uint16_t wr_buf[HASP_KEY_NUM];
	uint8_t key_num = g_hasp_key_num; /* Use the selected layout slot count directly;*/

	for(uint8_t j = 0; j < HALL_REG_READ_WRITE_VERIFY_COUNTS;j ++)
	{
		for(uint8_t i = 0; i < key_num; i ++)
		{
			if(!chain_miss_buff_reg[i])
			{
				wr_buf[i] = 0xff;
			}
			else
			{
				wr_buf[i] = i;
			}
		}
		h6_drv_write_reg_cmd(PWM0_CURR_REG,wr_buf, key_num);

		h6_drv_read_reg_cmd(PWM0_CURR_REG, key_num);


		for(uint8_t i = 0; i < key_num; i++)
		{
			if(chain_miss_buff_reg[i] == 0)
			{
				hall_hal_set_self_check_value(i, HALL_SELF_CHECK_EMPTY_VALUE); /* Preserve empty-slot status for SELF_CHECK reports. */
//				debug("hall chain->%d:%d reg05 value = %d, it is hall null!\r\n", i%HASP_CHANNL_NUM,i/HASP_CHANNL_NUM, (GTRY_HASP->RXRAM[i] & 0xFF));
			}
			else
			{
				if(wr_buf[i] != (GTRY_HASP->RXRAM[i] & 0xFF))
				{
					hall_hal_set_self_check_value(i, HALL_SELF_CHECK_REG_VERIFY_ERROR_VALUE); /* Mark this register readback failure directly; no second pass is needed. */
//					debug("hall chain->%d:%d reg05 value = %d, read failed!\r\n", i%HASP_CHANNL_NUM,i/HASP_CHANNL_NUM, (GTRY_HASP->RXRAM[i] & 0xFF));
					ret = 0;
				}
				else
				{
//					debug("hall chain->%d:%d reg05 value = %d, read ok!\r\n", i%HASP_CHANNL_NUM,i/HASP_CHANNL_NUM, (GTRY_HASP->RXRAM[i] & 0xFF));
				}
			}
		}
		wdt_reset_timer();
	}
	h6_hal_reg_multi_write(PWM0_CURR_REG, 0x0, g_hasp_key_num);
	if(ret)
	{
		printf("******hall reg write read verify successfully!!!!!!******\r\n");
	}else
	{
		h6_error_sign = 1U; /* Keep the reference-project behavior: later checks should not overwrite this failure. */
		printf("******hall reg write read verify failed!!!!!!******\r\n");
	}
	return ret;
}

/**
 * \brief     	HASP HAL layer initialization example
 *
 * \param[in] 	p_hasp_int_cb int_callback: HASP interrupt callback function
 * \param[in] 	freq_current : Current system frequency
 *
 * \return    None
 */
void h6_hal_init(p_hasp_int_cb int_callback)
{
	uint8_t clock_cali = 0;

	calc_chain_miss_buff_reg();
	calc_chain_miss_buff_led();

	hasp_drv_int_init(int_callback);	// HASP interrupt initialization function
	h6_hal_hasp_gpio_init();			// HASP pin multiplexing function
	h6_hal_hasp_init();				// HASP initialization function
	h6_hal_hasp_linetab_build(Hall_Linearization_table, LINE_SECTION ,LINE_LEN);/** Linearization table setup **/

	h6_hal_soft_reset();				//Hall soft reset

	/* Retry Hall frequency calibration up to 10 times; current h6_hal_clock_cali() returns 1 on success. */
	while (clock_cali < 10)		//10 Hall frequency calibration
	{
		hall_hal_clear_self_check_value(); /* Clear failed retry data so only the final result is reported. */
		if (h6_hal_clock_cali() != 0)
		{
			break; /* Exit immediately after the first successful calibration. */
		}

		clock_cali++;
		printf("H6_clock_cali_test error! retry=%u\r\n", clock_cali);
		wdt_hal_feed_dog();
	}

	if (clock_cali == 10)
	{
		printf("H6_clock_cali_test failed after 10 retries, continue init.\r\n");
	}

#if H6_OFFSET_CALI
	if(h6_error_sign == 0U)
	{
	#if H6_IMBALANCE_CALI_USE_FLASH_CACHE
		if(hall_hal_load_imbalance_cali_from_flash())
		{
			printf("offset_cali_flash_load\r\n"); /* Saved REG36/REG37 trims were restored, so live imbalance calibration can be skipped. */
		}
		else
	#endif
		{
			unsigned int offset_cali_ret = H6_offset_cali_b(); /* Run live imbalance calibration only when no valid Flash cache exists. */
			if(offset_cali_ret != 0U)
			{
				printf("offset_cali_failed ret=%u\r\n", offset_cali_ret); /* Keep boot alive so the host can read failed Hall slots through SELF_CHECK. */
			}

			printf("offset_cali_success\r\n");
		}
	}
#endif

	if(h6_error_sign == 0U)
	{
		if(h6_hal_reg_write_read_verify() == 0)
		{
			printf("H6_reg_write_read_verify failed, continue init for manufacturing SELF_CHECK report.\r\n"); /* Keep running so the host can read the failed Hall slot. */
		}
	}

	h6_hal_reg_set_mode();			//Hall basic register config

	h6_hal_variable_time_inst(0);//hall variable timing instruction

	if(h6_error_sign == 0U)
	{
		if(h6_hal_detect_crc_error_hall() == 0)//Hall for detecting CRC errors
		{
			printf("H6_crc_detect failed, continue init for manufacturing SELF_CHECK report.\r\n"); /* Preserve the CRC failure cache instead of blocking boot. */
		}
	}

	h6_hal_pwm_init();	 				//Pwm init
	h6_hal_hasp_config();				//Hasp data register config
	h6_hal_freq_config();				//CLK spread spectrum config

	h6_hal_fill_crc_null_hall();		//CRC data padding without hall position

	GTRY_HASP->INTEN |= CRC_ERR_INT  ;//| DATA_ERR_INT | KEY_BUSY_INT
}

/**
 * \brief    Hall stop sampling
 *
 * \param[in] None
 * \return    None
 */
void hall_hal_stop_sample(void)
{

}





