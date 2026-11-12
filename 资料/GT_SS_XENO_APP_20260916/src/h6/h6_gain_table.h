#ifndef SRC_H6_H6_GAIN_TABLE_H_
#define SRC_H6_H6_GAIN_TABLE_H_

#include <stdint.h>

#define H6_GAIN_CONFIG0_LNA_MASK        0xF0U
#define H6_GAIN_CONFIG0_LNA_SHIFT       4U
#define H6_GAIN_CONFIG0_PGA_MASK        0x0FU
#define H6_GAIN_CONFIG0_PGA_DEFAULT     0x05U
#define H6_GAIN_PGA_MAX                 0x09U

#define H6_LNA_GAIN_LGA_DEFAULT         0x01U
#define H6_LNA_GAIN_16X                 0x00U
#define H6_LNA_GAIN_24X                 0x08U
#define H6_LNA_GAIN_48X                 0x09U
#define H6_LNA_GAIN_96X                 0x0AU

uint8_t h6_hal_normalize_gain_config0(uint8_t gain_config0);

void h6_gain_table_finalize(uint8_t *gain_table,uint32_t flash_read_len,
							uint32_t key_num,uint8_t default_gain);

#endif /* SRC_H6_H6_GAIN_TABLE_H_ */
