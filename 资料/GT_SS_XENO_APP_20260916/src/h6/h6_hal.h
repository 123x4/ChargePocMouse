/*
 * H6_hal.h
 *
 *  Created on:
 *      Author:
 */

#ifndef SRC_H6_H6_HAL_H_
#define SRC_H6_H6_HAL_H_

#include "hasp_drv.h"
#include "h6_gain_table.h"

#define HALL_CRC_PRINT_EN 					1
#define HALL_GAIN_EN 						0
#define HALL_TOP_BASELINE_PRINT_EN			0

//0~7
#define POWER_LEVEL_CLK 				1//4
#define POWER_LEVEL_SDO 				1//4
#define POWER_LEVEL_SDI 				1//4

//0->high slew rate 	1->low slew rate
#define SR_LEVEL_CLK 					1
#define SR_LEVEL_SDO 					1
#define SR_LEVEL_SDI 					1

#define MODE_INTERRUPT_SINGLE_CHAIN_SAMPLE	0
#define MODE_INTERRUPT_ALL_CHAIN_SAMPLE		1
#define MODE_CONTINUOUS_ALL_CHAIN_SAMPLE	2
#define MODE_SINGLE_ALL_CHAIN_SAMPLE		3

#ifndef HALL_SELF_CHECK_EMPTY_VALUE
#define HALL_SELF_CHECK_EMPTY_VALUE             0x01U /* Empty Hall-chain slot marker used by manufacturing SELF_CHECK reads. */
#endif
#ifndef HALL_SELF_CHECK_CRC_OK_VALUE
#define HALL_SELF_CHECK_CRC_OK_VALUE            0x02U /* Good Hall slot marker used by manufacturing SELF_CHECK reads. */
#endif
#ifndef HALL_SELF_CHECK_CLOCK_CALI_ERROR_VALUE
#define HALL_SELF_CHECK_CLOCK_CALI_ERROR_VALUE  0x44U /* H6 clock calibration failed for this Hall slot. */
#endif
#ifndef HALL_SELF_CHECK_OFFSET_CALI_ERROR_VALUE
#define HALL_SELF_CHECK_OFFSET_CALI_ERROR_VALUE 0x45U /* H6 imbalance/offset calibration failed for this Hall slot. */
#endif
#ifndef HALL_SELF_CHECK_REG_VERIFY_ERROR_VALUE
#define HALL_SELF_CHECK_REG_VERIFY_ERROR_VALUE  0x46U /* H6 register write/read verification failed for this Hall slot. */
#endif
#ifndef HALL_SELF_CHECK_CRC_ERROR_VALUE
#define HALL_SELF_CHECK_CRC_ERROR_VALUE         0x47U /* H6 runtime raw-data CRC detection failed for this Hall slot. */
#endif

extern void h6_hal_write_gain_config0_table(uint16_t *gain_config0, uint32_t size);
extern void h6_hal_write_gain_config0_uniform(uint8_t gain_config0, uint32_t size);
extern uint16_t hall_hal_get_clock_cali_value(uint8_t key_idx);
extern void hall_hal_set_self_check_value(uint8_t key_idx, uint8_t value); /* Update one manufacturing SELF_CHECK byte from another H6 diagnostic stage. */
extern uint8_t hall_hal_get_self_check_value(uint8_t key_idx); /* Read the cached manufacturing SELF_CHECK byte for one Hall slot. */
extern uint8_t h6_error_sign; /* Expose the H6 self-check failure latch for reference-compatible diagnostics. */

extern void hall_hal_write_top_baseline(uint16_t *top_rawdata, uint32_t size);
extern uint8_t hall_hal_load_top_baseline_from_flash(void);

extern void h6_hal_set_sys_clk(uint8_t freq_current);

extern void h6_hal_init(p_hasp_int_cb int_callback);

extern void h6_hal_start_sample(uint8_t mode);

extern void h6_hal_stop_sample(void);

extern void h6_hal_reg_set_mode();
extern void offset_cali_reg_recovery(void);

extern void h6_hal_variable_time_inst(uint8_t do_pwm_write);

extern void h6_hal_reg_multi_write(uint8_t addr, uint16_t data,uint32_t size);

extern void h6_hal_pwm_multi_write(uint16_t pwmch, uint16_t data, uint32_t size);
extern void h6_w_current_space_LR(uint16_t r_cur, uint16_t g_cur, uint16_t b_cur);
extern void h6_w_pwm_space_LR(uint16_t r, uint16_t g, uint16_t b);
extern void stop_sampling_pwm_duty_adjust(uint16_t r_duty, uint16_t g_duty, uint16_t b_duty);

extern void h6_hal_single_time_inst(uint8_t do_pwm_write);

#endif /* SRC_H6_H6_HAL_H_ */
