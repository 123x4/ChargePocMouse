/*
 * h6_offset_cail.h
 *
 *  Created on: Jun 24, 2026
 *      Author: Lenovo
 */

#ifndef SRC_H6_H6_OFFSET_CALI_H_
#define SRC_H6_H6_OFFSET_CALI_H_

#include "stdint.h"

#if H6_OFFSET_CALI
#ifndef H6_IMBALANCE_CALI_USE_FLASH_CACHE
#define H6_IMBALANCE_CALI_USE_FLASH_CACHE 		1
#endif

#define H6_IMBALANCE_CALI_RET_OK             	0U /* H6 imbalance calibration succeeded. */
#define H6_IMBALANCE_CALI_RET_REG36_POS_CRC 	2U /* REG36 positive-side average read failed CRC/count validation. */
#define H6_IMBALANCE_CALI_RET_REG36_NEG_CRC 	3U /* REG36 negative-side average read failed CRC/count validation. */
#define H6_IMBALANCE_CALI_RET_REG37_1_POS_CRC 	4U /* First REG37 positive-side average read failed CRC/count validation. */
#define H6_IMBALANCE_CALI_RET_REG37_1_NEG_CRC 	5U /* First REG37 negative-side average read failed CRC/count validation. */
#define H6_IMBALANCE_CALI_RET_REG37_2_POS_CRC 	6U /* Second REG37 positive-side average read failed CRC/count validation. */
#define H6_IMBALANCE_CALI_RET_REG37_2_NEG_CRC 	7U /* Second REG37 negative-side average read failed CRC/count validation. */
#define H6_IMBALANCE_CALI_RET_LIMIT_FAIL     	8U /* Best REG36/REG37 result still exceeds the target imbalance. */

extern unsigned int H6_offset_cali_b(void);
extern uint8_t h6_imbalance_cali_get_last_ret(void);
extern uint8_t hall_hal_load_imbalance_cali_from_flash(void);
#endif

#endif /* SRC_H6_H6_OFFSET_CALI_H_ */

