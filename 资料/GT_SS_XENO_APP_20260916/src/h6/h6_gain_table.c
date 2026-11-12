#include "h6_gain_table.h"

uint8_t h6_hal_normalize_gain_config0(uint8_t gain_config0)
{
	uint8_t lna_gain = (gain_config0 & H6_GAIN_CONFIG0_LNA_MASK) >> H6_GAIN_CONFIG0_LNA_SHIFT;
	uint8_t pga_gain = gain_config0 & H6_GAIN_CONFIG0_PGA_MASK;

	if (pga_gain > H6_GAIN_PGA_MAX)
	{
		pga_gain = H6_GAIN_CONFIG0_PGA_DEFAULT;
	}

	switch (lna_gain)
	{
	case H6_LNA_GAIN_LGA_DEFAULT:
	case H6_LNA_GAIN_16X:
	case H6_LNA_GAIN_24X:
	case H6_LNA_GAIN_48X:
	case H6_LNA_GAIN_96X:
		break;

	default:
		lna_gain = H6_LNA_GAIN_LGA_DEFAULT;
		break;
	}

	return (uint8_t)((lna_gain << H6_GAIN_CONFIG0_LNA_SHIFT) | pga_gain);
}

void h6_gain_table_finalize(uint8_t *gain_table,uint32_t flash_read_len,
							uint32_t key_num,uint8_t default_gain)
{
	uint8_t normalized_default = h6_hal_normalize_gain_config0(default_gain);
	uint8_t flash_complete = (flash_read_len == key_num);

	for (uint32_t i = 0; i < key_num; i++)
	{
		uint8_t gain = normalized_default;

		if ((flash_complete != 0U) && (gain_table[i] != 0xFFU))
		{
			gain = h6_hal_normalize_gain_config0(gain_table[i]);
		}

		gain_table[i] = gain;
	}
}
