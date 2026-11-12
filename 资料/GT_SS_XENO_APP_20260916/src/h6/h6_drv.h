/*
 * h6_drv.h
 *
 *  Created on:
 *      Author:
 */

#ifndef SRC_HALL_H6_DRV_H6_DRV_H_
#define SRC_HALL_H6_DRV_H6_DRV_H_

#include "gtry_type.h"
#include "hasp_drv.h"
#include "app_config.h"

#define H6_CMD_TIMES_TD1			(ry_get_sys_ustick() * 5	)
#define H6_CMD_TIMES_TD2_S			(ry_get_sys_ustick() * 24	)
#define H6_CMD_TIMES_TD2_M			(ry_get_sys_ustick() * 4	)
#define H6_CMD_TIMES_TD3			(ry_get_sys_ustick() * 12	)
#define H6_CMD_TIMES_TD4			(ry_get_sys_ustick() * 18	)
#define H6_CMD_TIMES_TD5			(ry_get_sys_ustick() * 4	)
#define H6_CMD_TIMES_TD6			(ry_get_sys_ustick() * 39	)
#define H6_CMD_TIMES_TD7			(ry_get_sys_ustick() * 28	)
#define H6_CMD_TIMES_TD8			(ry_get_sys_ustick() * 2	)
#define H6_CMD_TIMES_TD9			(ry_get_sys_ustick() * 51	)
#define H6_CMD_TIMES_TD10			(ry_get_sys_ustick() * 128	)
#define H6_CMD_TIMES_TD11			(ry_get_sys_ustick() * 1300	)
#define H6_CMD_TIMES_TD12			(ry_get_sys_ustick() * 51	)

#define H6_CMD_TIMES_WAIT			(ry_get_sys_ustick() * 2	)

#if CONTINUOUS_MODE
	#define H6_200_CMD_TIMES_WAIT_S		0//(200 * 66.8- CYCLE_TIME_SPREAD_SPECTRUM	)
	#define H6_200_CMD_TIMES_TD2_S		(200 * 4)
	#define H6_96_CMD_TIMES_TD2_S		(96 *  4)
	#define H6_48_CMD_TIMES_TD2_S		(48 *  4)
#else
	#define H6_200_CMD_TIMES_WAIT_S		(200 * 68.8)  //(200 * 66.8 - CYCLE_TIME_SPREAD_SPECTRUM	)
	#define H6_200_CMD_TIMES_TD2_S		(200 * 24)
	#define H6_96_CMD_TIMES_TD2_S		(96 * 24)
	#define H6_48_CMD_TIMES_TD2_S		(48 * 24)
#endif

#define H6_200_CMD_TIMES_WAIT_M		(200 * 45.8- CYCLE_TIME_SPREAD_SPECTRUM	)
#define H6_96_CMD_TIMES_WAIT_S		(96  * 63.3- CYCLE_TIME_SPREAD_SPECTRUM	)
#define H6_96_CMD_TIMES_WAIT_M		(96  * 41.3- CYCLE_TIME_SPREAD_SPECTRUM	)
#define H6_48_CMD_TIMES_WAIT_S		(48  * 57.8- CYCLE_TIME_SPREAD_SPECTRUM	)
#define H6_48_CMD_TIMES_WAIT_M		(48  * 35.8- CYCLE_TIME_SPREAD_SPECTRUM	)

#define H6_200_CMD_TIMES_TD1		(200 * 5)
#define H6_96_CMD_TIMES_TD1			(96 * 5)
#define H6_48_CMD_TIMES_TD1			(48 * 5)

#define H6_200_CMD_TIMES_TD4		(200 * 18)
#define H6_96_CMD_TIMES_TD4			(96 * 18)
#define H6_48_CMD_TIMES_TD4			(48 * 18)

#define H6_200_CMD_TIMES_TD5		(200 * 4)
#define H6_96_CMD_TIMES_TD5			(96 * 4)
#define H6_48_CMD_TIMES_TD5			(48 * 4)



#define H6_200_CMD_TIMES_WAIT		(200 * 2)
#define H6_96_CMD_TIMES_WAIT		(96 * 2)
#define H6_48_CMD_TIMES_WAIT		(48 * 2)

#define H6_CONV_SINGLE_CMD			0x0
#define H6_CONV_CONTINUS_CMD		0x1

#define H6_WRITE_CTRL_FLAG			0x1
#define H6_READ_CTRL_FLAG			0x0

#define H6_CONV_16BIT_CMD			0x0
#define H6_CONV_4BIT_CMD			0x1
#define H6_CONV_2BIT_CMD			0x2

#define	CONVERT						0x0
#define	COARSE_CONVERT				0x1

#define H6_WD_CTRL_MASK				0x80
#define H6_WD_CTRL_POS				7
#define H6_CRC_CTRL_POS				6
#define H6_WD_ADDR_MASK				(0x3Fu)

#define H6_REG_DATA_SIZE			8

#define H6_OSC_PASS_WORD			0xFA

#define H6_PWM_TOTAL_PERIOD			1<<15
#define H6_PWM_LED0_DUTY			0<<13
#define H6_PWM_LED1_DUTY            1<<13
#define H6_PWM_LED2_DUTY            2<<13
#define H6_PWM_LED3_DUTY            3<<13

#define TEST_MODE_LOMG_TIMER		3
#define TEST_MODE_SHORT_TIMER		2

/** Convert a 7-bit address into an 8-bit address with read/write bits and CRC */
#define H6_GET_8BIT_ADDRESS(wd,addr) ((uint8_t)((wd) << H6_WD_CTRL_POS) |(uint8_t)((addr) & H6_WD_ADDR_MASK))

/***********************************************Hall register definition*****************************************************/
#define PWM0_DUTY_REG					(0x0)
#define PWM1_DUTY_REG					(0x1)
#define PWM2_DUTY_REG					(0x2)
#define PWM3_DUTY_REG					(0x3)
#define PWM_CYCL_REG					(0x4)
#define PWM0_CURR_REG					(0x5)
#define PWM1_CURR_REG					(0x6)
#define PWM2_CURR_REG					(0x7)
#define PWM3_CURR_REG					(0x8)
#define CONGFIG_REG9					(0x9)
#define CONGFIG_REGA					(0xA)
#define CONGFIG_REGB					(0xB)
#define CONGFIG_REGC					(0xC)
#define CONGFIG_REGD					(0xD)
#define CONGFIG_REGE					(0xE)
#define CONGFIG_REGF					(0xF)
#define CONGFIG_REG10					(0x10)
#define CONGFIG_REG11					(0x11)
#define CONGFIG_REG12					(0x12)
#define CONGFIG_REG13					(0x13)
#define CONGFIG_REG14					(0x14)
#define CONGFIG_REG15					(0x15)
#define CONGFIG_REG16					(0x16)
#define CONGFIG_REG17					(0x17)
#define CONGFIG_REG18					(0x18)
#define CONGFIG_REG19					(0x19)
#define CONGFIG_REG1A					(0x1A)
#define CONGFIG_REG1B					(0x1B)
#define CONGFIG_REG1C					(0x1C)
#define CONGFIG_REG1D					(0x1D)
#define CONGFIG_REG1E					(0x1E)
#define CONGFIG_REG1F					(0x1F)
#define CONGFIG_REG20					(0x20)
#define CONGFIG_REG21					(0x21)
#define CONGFIG_REG22					(0x22)
#define CONGFIG_REG23					(0x23)
#define CONGFIG_REG24					(0x24)
#define CONGFIG_REG25					(0x25)
#define CONGFIG_REG26					(0x26)
#define CONGFIG_REG27					(0x27)
#define CONGFIG_REG28					(0x28)
#define CONGFIG_REG29					(0x29)
#define CONGFIG_REG2A					(0x2A)
#define CONGFIG_REG2B					(0x2B)
#define CONGFIG_REG2C					(0x2C)
#define CONGFIG_REG2D					(0x2D)
#define CONGFIG_REG2E					(0x2E)
#define CONGFIG_REG2F					(0x2F)
#define CONGFIG_REG30					(0x30)
#define CONGFIG_REG31					(0x31)
#define CONGFIG_REG32					(0x32)
#define CONGFIG_REG33					(0x33)
#define CONGFIG_REG34					(0x34)
#define CONGFIG_REG35					(0x35)
#define CONGFIG_REG36					(0x36)
#define CONGFIG_REG37					(0x37)
#define CONGFIG_REG38					(0x38)
#define CONGFIG_REG39					(0x39)
#define CONGFIG_REG3A					(0x3A)
#define TESTENCTRL						(0x3B)
#define BASLINE							(0x3C)
#define OSCFRQCTRL						(0x3D)
#define H6_FUSE_CTRL					(0x3E)
#define MASK_NUM						(0x3F)

#define PWM_W_PWM3_DUTY					(0x3<<13)
#define PWM_W_PWM2_DUTY					(0x2<<13)
#define PWM_W_PWM1_DUTY					(0x1<<13)
#define PWM_W_PWM0_DUTY					(0x0<<13)

#define	PWM_W_CYCLE						(0x1<<15)

extern const uint32_t freq_main_m[];

extern const uint32_t freq_main_s[];

extern const uint32_t freq_main_td1[];

extern const uint32_t freq_main_td2_s[];

extern const uint32_t freq_main_td4[];

extern const uint32_t freq_main_td5[];

extern const uint32_t freq_main_wait[];

static inline void h6_drv_convert_time_inst(void)
{
	hasp_drv_td_set(HASP_TD1 , H6_CMD_TIMES_WAIT);
	hasp_drv_td_set(HASP_TD2 , H6_CMD_TIMES_TD1);
	hasp_drv_td_set(HASP_TD3 , H6_CMD_TIMES_TD2_M);
}

static inline void h6_drv_interrupt_time_inst(void)
{
	hasp_drv_td_set(HASP_TD1 , H6_CMD_TIMES_WAIT);
	hasp_drv_td_set(HASP_TD2 , H6_CMD_TIMES_TD1);
	hasp_drv_td_set(HASP_TD3 , H6_CMD_TIMES_TD2_S);
}

static inline void h6_drv_w_pwm_time_inst(void)
{
	hasp_drv_td_set(HASP_TD5,H6_CMD_TIMES_WAIT);
	hasp_drv_td_set(HASP_TD6,H6_CMD_TIMES_TD4);
	hasp_drv_td_set(HASP_TD7,H6_CMD_TIMES_TD5);
}


static inline void h6_drv_wr_reg_time_inst(void)
{
	hasp_drv_td_set(HASP_TD5,H6_CMD_TIMES_WAIT);
	hasp_drv_td_set(HASP_TD6,H6_CMD_TIMES_TD6);
	hasp_drv_td_set(HASP_TD7,H6_CMD_TIMES_TD5);
}

static inline void h6_drv_w_pwm_time_clr(void)
{
	hasp_drv_td_set(HASP_TD5,0);
	hasp_drv_td_set(HASP_TD6,0);
	hasp_drv_td_set(HASP_TD7,0);
}

static inline void h6_drv_sleep_time_inst(void)
{
	hasp_drv_td_set(HASP_TD1 , H6_CMD_TIMES_WAIT);
	hasp_drv_td_set(HASP_TD2 , H6_CMD_TIMES_TD7);
	hasp_drv_td_set(HASP_TD3 , H6_CMD_TIMES_TD2_M);
}

static inline void h6_drv_wakeup_time_inst(void)
{
	hasp_drv_td_set(HASP_TD1 , H6_CMD_TIMES_WAIT);
	hasp_drv_td_set(HASP_TD2 , H6_CMD_TIMES_TD8);
	hasp_drv_td_set(HASP_TD3 , H6_CMD_TIMES_TD9);
}

static inline void h6_drv_freq_calibrate_time_inst(void)
{
	hasp_drv_td_set(HASP_TD1,H6_CMD_TIMES_WAIT);
	hasp_drv_td_set(HASP_TD2,H6_CMD_TIMES_TD10);
	hasp_drv_td_set(HASP_TD3,H6_CMD_TIMES_TD12);
}

static inline void h6_drv_soft_rst_time_inst(void)
{
	hasp_drv_td_set(HASP_TD1 , H6_CMD_TIMES_TD2_M);
	hasp_drv_td_set(HASP_TD2 , H6_CMD_TIMES_TD11);
	hasp_drv_td_set(HASP_TD3 , H6_CMD_TIMES_TD9);
}

extern void h6_drv_read_instru_count_cmd(uint32_t size);

extern void h6_drv_write_reg_cmd(uint8_t addr , uint16_t *wr_data, uint16_t size);

extern void h6_drv_w_pwm_reg_cmd(uint16_t *wr_data, uint32_t size);

extern void h6_drv_read_reg_cmd(uint16_t addr, uint16_t size);

extern void h6_drv_sleep_cmd(uint32_t enter_sleep);

#endif /* SRC_HALL_H6_DRV_H6_DRV_H_ */
