/*
 * h6_drv.c
 *
 *  Created on:
 *      Author:
 */

#include <string.h>
#include "h6_drv.h"
#include "h6_hw_tab.h"
#include "app_config.h"

const uint32_t freq_main_m[3] = {
									H6_200_CMD_TIMES_WAIT_M,
									H6_96_CMD_TIMES_WAIT_M,
									H6_48_CMD_TIMES_WAIT_M

								};

const uint32_t freq_main_s[3] = {
									H6_200_CMD_TIMES_WAIT_S,
									H6_96_CMD_TIMES_WAIT_S,
									H6_48_CMD_TIMES_WAIT_S
								};

const uint32_t freq_main_td1[3] = {
									H6_200_CMD_TIMES_TD1,
									H6_96_CMD_TIMES_TD1,
									H6_48_CMD_TIMES_TD1
									};

const uint32_t freq_main_td2_s[3]={	H6_200_CMD_TIMES_TD2_S ,
									H6_96_CMD_TIMES_TD2_S,
									H6_48_CMD_TIMES_TD2_S};

const uint32_t freq_main_td4[3] = {	H6_200_CMD_TIMES_TD4 ,
									H6_96_CMD_TIMES_TD4,
									H6_48_CMD_TIMES_TD4};

const uint32_t freq_main_td5[3] = {	H6_200_CMD_TIMES_TD5 ,
									H6_96_CMD_TIMES_TD5,
									H6_48_CMD_TIMES_TD5};

const uint32_t freq_main_wait[3] ={	H6_200_CMD_TIMES_WAIT,
									H6_96_CMD_TIMES_WAIT,
									H6_48_CMD_TIMES_WAIT};
/**
 * \brief     h6 read counter count command
 *
 * \param[in] *rd_data : Buffer to store read data
 * \param[in] size : Number of chips
 * \return    None
 */
void h6_drv_read_instru_count_cmd(uint32_t size)
{
	uint32_t ctrl_shadow = hasp_drv_get_ctrl();

	hasp_drv_td_clear();
	hasp_drv_set_ctrl(hasp_drv_get_ctrl() &(~HASP_CONTINUE_CONV));

	h6_drv_freq_calibrate_time_inst();
	hasp_drv_start_work();

	hasp_drv_set_ctrl(ctrl_shadow);
}

/**
 * \brief     h6 write register command
 *
 * \param[in] *wr_buf : Buffer with data to write
 * \param[in] size : Number of chips
 * \return    None
 */
void h6_drv_write_reg_cmd(uint8_t addr , uint16_t *wr_data, uint16_t size)
{
	uint32_t i = 0 , j = 0;
	uint32_t data_crc = {0};

	uint32_t ctrl_shadow = hasp_drv_get_ctrl();
	uint32_t nsck_shadow = hasp_drv_get_nsck();
    hasp_drv_nsck_set( NSCK1(g_hasp_chain_length)|SECTION1_REG_DATA);
	hasp_drv_set_ctrl(hasp_drv_get_ctrl() &(~HASP_CONTINUE_CONV));

	for( i = 0; i < size; i++)
	{
		data_crc = 0;
		for(j = 0; j < 8; j++)
		{
			data_crc ^= ((wr_data[i]>>j) & 0x1);
		}
		for(j = 0; j < 6; j++)
		{
			data_crc ^= ((addr>>j) & 0x1);
		}
		if(chain_miss_buff_reg[i])
		{
			GTRY_HASP->TXDATA[i+(g_hasp_chain_length - chain_len_buff[i%HASP_CHANNL_NUM])*HASP_CHANNL_NUM] =
					(data_crc<<14)|(uint16_t)((H6_GET_8BIT_ADDRESS(H6_WRITE_CTRL_FLAG, addr)<<8) | (wr_data[i] & 0xff));
		}
	}

	hasp_drv_td_clear();
	h6_drv_wr_reg_time_inst();

	hasp_drv_start_work();

	hasp_drv_nsck_set(nsck_shadow);
	hasp_drv_set_ctrl(ctrl_shadow);
}

/**
 * \brief     h6 write PWM register data
 *
 * \param[in] *wr_data : Buffer with data to write
 * \param[in] size : Number of chips
 * \return    None
 */
void h6_drv_w_pwm_reg_cmd(uint16_t *wr_data, uint32_t size)
{
	for(unsigned int i = 0; i < size; i++)
	{
		if(chain_miss_buff_reg[i])
		{
			GTRY_HASP->TXDATA[i+(g_hasp_chain_length - chain_len_buff[i%HASP_CHANNL_NUM])*HASP_CHANNL_NUM] = wr_data[i];
		}
	}

	h6_drv_w_pwm_time_inst();
	hasp_drv_start_work();
}

/**
 * \brief     h6 read register command
 *
 * \param[in] *rd_data : Buffer to store read data
 * \param[in] size : Number of chips
 * \return    None
 */
void h6_drv_read_reg_cmd(uint16_t addr, uint16_t size)
{
	uint32_t nsck_shadow = hasp_drv_get_nsck();
	uint32_t ctrl_shadow = hasp_drv_get_ctrl();
	hasp_drv_nsck_set(NSCK1(g_hasp_chain_length)|SECTION1_REG_DATA|
					  NSCK2(g_hasp_chain_length)|SECTION2_REG_DATA);
	hasp_drv_set_ctrl(hasp_drv_get_ctrl()&(~HASP_CONTINUE_CONV));
	for(unsigned int i = 0; i < size; i++)
	{
		GTRY_HASP->TXDATA[i] = (uint16_t)(H6_GET_8BIT_ADDRESS(H6_READ_CTRL_FLAG, addr)<<8);
	}
	hasp_drv_td_clear();
	h6_drv_wr_reg_time_inst();
	hasp_drv_start_work();
	hasp_drv_nsck_set(nsck_shadow);
	hasp_drv_set_ctrl(ctrl_shadow);
}

/**
 * \brief     Hall sleep command
 *
 * \param[in] enter_sleep : Sleep enable parameter (1: enter sleep, 0: wakeup)
 * \return    None
 */
void h6_drv_sleep_cmd(uint32_t enter_sleep)
{
	uint32_t nsck_shadow = hasp_drv_get_nsck();
	uint32_t ctrl_shadow = hasp_drv_get_ctrl();
	hasp_drv_nsck_set(NSCK1(0)|SECTION1_REG_DATA);
	hasp_drv_set_ctrl(hasp_drv_get_ctrl()&(~HASP_CONTINUE_CONV));
	hasp_drv_td_clear();
	if(enter_sleep)
	{
		h6_drv_sleep_time_inst();
	}
	else
	{
		h6_drv_wakeup_time_inst();
	}
	hasp_drv_start_work();
	hasp_drv_nsck_set(nsck_shadow);
	hasp_drv_set_ctrl(ctrl_shadow);
}
