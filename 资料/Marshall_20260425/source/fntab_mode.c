/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdint.h>
#include <usb.h>
#include <hid_keyboard.h>
#include <hall_spi.h>
#include <fntab_mode.h>
#include <led.h>
#include <Flash_iap.h>
#include <flash_zb25v.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * FUNCTION
 ******************************************************************************/
void SPI0_rapid_trigger_rgb_set(uint8_t index);
void SPI1_rapid_trigger_rgb_set(uint8_t index);
void SPI3_rapid_trigger_rgb_set(uint8_t index);
void SPI4_rapid_trigger_rgb_set(uint8_t index);
void SPI7_rapid_trigger_rgb_set(uint8_t index);
void SPI0_rapid_trigger_rgb_clear(uint8_t index);
void SPI1_rapid_trigger_rgb_clear(uint8_t index);
void SPI3_rapid_trigger_rgb_clear(uint8_t index);
void SPI4_rapid_trigger_rgb_clear(uint8_t index);
void SPI7_rapid_trigger_rgb_clear(uint8_t index);
void rapid_trigger_adjust_start(void);
void FnTabMode_KeyScan(void);
void FNTAB_MODE_DataHandle(void);
void rapid_trigger_adjust_mode_ex(void);
void FNTAB_SETSPI0_RGB(uint8_t index);
void FNTAB_SETSPI1_RGB(uint8_t index);
void FNTAB_SETSPI3_RGB(uint8_t index);
void FNTAB_SETSPI4_RGB(uint8_t index);
void FNTAB_SETSPI7_RGB(uint8_t index);
void rapid_trigger_adjust_reset(void);
void rapid_trigger_adjust_reset_SPI0(uint8_t index);
void rapid_trigger_adjust_reset_SPI1(uint8_t index);
void rapid_trigger_adjust_reset_SPI3(uint8_t index);
void rapid_trigger_adjust_reset_SPI4(uint8_t index);
void rapid_trigger_adjust_reset_SPI7(uint8_t index);
void rapid_trigger_adjust_dec(void);
void rapid_trigger_adjust_dec_SPI0(uint8_t index);
void rapid_trigger_adjust_dec_SPI1(uint8_t index);
void rapid_trigger_adjust_dec_SPI3(uint8_t index);
void rapid_trigger_adjust_dec_SPI4(uint8_t index);
void rapid_trigger_adjust_dec_SPI7(uint8_t index);
void rapid_trigger_adjust_inc(void);
void rapid_trigger_adjust_inc_SPI0(uint8_t index);
void rapid_trigger_adjust_inc_SPI1(uint8_t index);
void rapid_trigger_adjust_inc_SPI3(uint8_t index);
void rapid_trigger_adjust_inc_SPI4(uint8_t index);
void rapid_trigger_adjust_inc_SPI7(uint8_t index);

void SPI0_FNMode_Scan(uint8_t Keyindex);
void SPI1_FNMode_Scan(uint8_t Keyindex);
void SPI2_FNMode_Scan(uint8_t Keyindex);
void SPI3_FNMode_Scan(uint8_t Keyindex);
void SPI4_FNMode_Scan(uint8_t Keyindex);
/*******************************************************************************
 * Variables
 ******************************************************************************/
//-----------------------------------------------------------------------------------------------------------------
uint8_t trigger_point_adjust_flag_SPI0[TRANSFER_SIZE];
uint8_t trigger_point_adjust_flag_SPI1[TRANSFER_SIZE];
uint8_t trigger_point_adjust_flag_SPI3[TRANSFER_SIZE];
uint8_t trigger_point_adjust_flag_SPI4[TRANSFER_SIZE];
uint8_t trigger_point_adjust_flag_SPI7[TRANSFER_SIZE];
uint8_t Flag_FnMode_TurnOnLed_cnt = 0;
/*******************************************************************************
 * Code
 ******************************************************************************/
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : rapid_trigger_adjust_start
 * Description   : handle test mode data in this function;
 *
 * END ****************************************************************************************************************/
void rapid_trigger_adjust_start(void)
{
	uint8_t i,j,matrix_index;
	uint16_t Temp_Keycode;

	for(i = 0; i < TRANSFER_SIZE; i++)
	{
		trigger_point_adjust_flag_SPI0[i] = 0;
		trigger_point_adjust_flag_SPI1[i] = 0;
		trigger_point_adjust_flag_SPI3[i] = 0;
		trigger_point_adjust_flag_SPI4[i] = 0;
		trigger_point_adjust_flag_SPI7[i] = 0;
		//---------------------------------------------------------------------------------------------------------------------
		LED_Buffer_R[SPI0_GROUP][i] = 0;
		LED_Buffer_G[SPI0_GROUP][i] = 0;
		LED_Buffer_B[SPI0_GROUP][i] = 0;
		//---------------------------------------------------------------------------------------------------------------------
		LED_Buffer_R[SPI1_GROUP][i] = 0;
		LED_Buffer_G[SPI1_GROUP][i] = 0;
		LED_Buffer_B[SPI1_GROUP][i] = 0;
		//---------------------------------------------------------------------------------------------------------------------
		LED_Buffer_R[SPI3_GROUP][i] = 0;
		LED_Buffer_G[SPI3_GROUP][i] = 0;
		LED_Buffer_B[SPI3_GROUP][i] = 0;
		//---------------------------------------------------------------------------------------------------------------------
		LED_Buffer_R[SPI4_GROUP][i] = 0;
		LED_Buffer_G[SPI4_GROUP][i] = 0;
		LED_Buffer_B[SPI4_GROUP][i] = 0;
		//---------------------------------------------------------------------------------------------------------------------
		LED_Buffer_R[SPI6_GROUP][i] = 0;
		LED_Buffer_G[SPI6_GROUP][i] = 0;
		LED_Buffer_B[SPI6_GROUP][i] = 0;
		//---------------------------------------------------------------------------------------------------------------------
	}

	for(i = 0; i < TRANSFER_SIZE; i++)
	{
		//---------------------------------------------------------------------------------------------------------------------
		/* spi0 */
		Temp_Keycode = KeyBoardLayout_FNMODE[SPI0_GROUP][i];
		if((Temp_Keycode != KEY_PAGEDOWN) && (Temp_Keycode != RESERVE))
		{
			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI0_GROUP][i] & 0x01)
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI0_GROUP][i]++;
			}
			//---------------------------------------------------------------------------------------------------------------------
			SPI0_rapid_trigger_rgb_set(i);
			//---------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* spi1 */
		Temp_Keycode = KeyBoardLayout_FNMODE[SPI1_GROUP][i];
		if((Temp_Keycode != KEY_FN) && (Temp_Keycode != RESERVE))
		{
			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI1_GROUP][i] & 0x01)
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI1_GROUP][i]++;
			}
			//---------------------------------------------------------------------------------------------------------------------
			SPI1_rapid_trigger_rgb_set(i);
			//---------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* spi3 */
		Temp_Keycode = KeyBoardLayout_FNMODE[SPI3_GROUP][i];
		if((Temp_Keycode != KEY_DELETE) && (Temp_Keycode != RESERVE))
		{
			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI3_GROUP][i] & 0x01)
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI3_GROUP][i]++;
			}
			//---------------------------------------------------------------------------------------------------------------------
			SPI3_rapid_trigger_rgb_set(i);
			//---------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* spi4 */
		Temp_Keycode = KeyBoardLayout_FNMODE[SPI4_GROUP][i];
		if((Temp_Keycode != KEY_PAGEUP) && (Temp_Keycode != RESERVE))
		{
			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI4_GROUP][i] & 0x01)
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI4_GROUP][i]++;
			}
			//---------------------------------------------------------------------------------------------------------------------
			SPI4_rapid_trigger_rgb_set(i);
			//---------------------------------------------------------------------------------------------------------------------
		}
		/* spi7 */
		Temp_Keycode = KeyBoardLayout_FNMODE[SPI6_GROUP][i];
		if((Temp_Keycode != KEY_INSERT) && (Temp_Keycode != RESERVE))
		{
			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI6_GROUP][i] & 0x01)
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI6_GROUP][i]++;
			}
			//---------------------------------------------------------------------------------------------------------------------
			SPI7_rapid_trigger_rgb_set(i);
			//---------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------
	}
	//---------------------------------------------------------------------------------------------------------------------
	/* PAGDN */
	LED_Buffer_B[SPI0_GROUP][13] = 0xFF;
	g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][13] = 10;
	g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][13] = 10;
	//---------------------------------------------------------------------------------------------------------------------
	/* FN */
	LED_Buffer_B[SPI1_GROUP][5] = 0xFF;
	g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][5] = 10;
	g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][5] = 10;
	//---------------------------------------------------------------------------------------------------------------------
	/* DEL */
	LED_Buffer_B[SPI3_GROUP][14] = 0xFF;
	g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][14] = 10;
	g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][14] = 10;
	//---------------------------------------------------------------------------------------------------------------------
	/* PGUP */
	LED_Buffer_B[SPI4_GROUP][13] = 0xFF;
	g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][13] = 10;
	g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][13] = 10;
	//---------------------------------------------------------------------------------------------------------------------
	/* INS */
	LED_Buffer_B[SPI6_GROUP][14] = 0xFF;
	g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][14] = 10;
	g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][14] = 10;
	//---------------------------------------------------------------------------------------------------------------------
	if_adjust_all_mode = 1;
	//---------------------------------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : rapid_trigger_adjust_mode_ex
 * Description   : Convert hall raw data to component segment;
 *
 * END ****************************************************************************************************************/
void rapid_trigger_adjust_mode_ex(void)
{
	uint8_t i;

	if(if_adjust_all_mode == 1)
	{
		if_adjust_all_mode = 0;
		for(i = 0; i < TRANSFER_SIZE; i++)
		{
			//---------------------------------------------------------------------------------------------------------------------
			/* SPI0---13是PGDN键 */
			if(i != 13)
			{
				FNTAB_SETSPI0_RGB(i);
			}
			//---------------------------------------------------------------------------------------------------------------------
			/* SPI1---5是FN键 */
			if(i != 5)
			{
				FNTAB_SETSPI1_RGB(i);
			}
			//---------------------------------------------------------------------------------------------------------------------
			/* SPI3---14是DEL键 */
			if(i != 14)
			{
				FNTAB_SETSPI3_RGB(i);
			}
			//---------------------------------------------------------------------------------------------------------------------
			/* SPI4---13是PGUP键 */
			if(i != 13)
			{
				FNTAB_SETSPI4_RGB(i);
			}
			//---------------------------------------------------------------------------------------------------------------------
			/* SPI7---14是INS */
			if((i != 14))
			{
				FNTAB_SETSPI7_RGB(i);
			}
			//---------------------------------------------------------------------------------------------------------------------
		}
	}
	else
	{
		if_adjust_all_mode = 1;
		for(i = 0; i < TRANSFER_SIZE; i++)
		{
			//---------------------------------------------------------------------------------------------------------------------
			/* SPI0---13是PGDN键 */
			if(i != 13)
			{
				SPI0_rapid_trigger_rgb_set(i);
			}
			//---------------------------------------------------------------------------------------------------------------------
			/* SPI1---5是FN键 */
			if(i != 5)
			{
				SPI1_rapid_trigger_rgb_set(i);
			}
			//---------------------------------------------------------------------------------------------------------------------
			/* SPI3---14是DEL键 */
			if(i != 14)
			{
				SPI3_rapid_trigger_rgb_set(i);
			}
			//---------------------------------------------------------------------------------------------------------------------
			/* SPI4---13是PGUP键 */
			if(i != 13)
			{
				SPI4_rapid_trigger_rgb_set(i);
			}
			//---------------------------------------------------------------------------------------------------------------------
			/* SPI7---14是INS */
			if((i != 14))
			{
				SPI7_rapid_trigger_rgb_set(i);
			}
			//---------------------------------------------------------------------------------------------------------------------
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : rapid_trigger_adjust_reset
 * Description   : 恢复之前默认的设定触发值
 *
 * END ****************************************************************************************************************/
void rapid_trigger_adjust_reset(void)
{
	uint8_t i;
	for(i = 0; i < TRANSFER_SIZE; i++)
	{
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI0---13是PGDN键 */
		if(i != 13)
		{
			rapid_trigger_adjust_reset_SPI0(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI1---5是FN键 */
		if(i != 5)
		{
			rapid_trigger_adjust_reset_SPI1(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI3---14是DEL键 */
		if(i != 14)
		{
			rapid_trigger_adjust_reset_SPI3(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI4---13是PGUP键 */
		if(i != 13)
		{
			rapid_trigger_adjust_reset_SPI4(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI7---14是INS */
		if((i != 14))
		{
			rapid_trigger_adjust_reset_SPI7(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : Hall_Rawdata_ConvertTo_Section
 * Description   : Convert hall raw data to component segment;
 *
 * END ****************************************************************************************************************/
void rapid_trigger_adjust_reset_SPI0(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][index] = 8;
		SPI0_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI0[index])
	{
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][index] = 8;
		SPI0_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_reset_SPI1(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][index] = 8;
		SPI1_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI1[index])
	{
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][index] = 8;
		SPI1_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_reset_SPI3(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][index] = 8;
		SPI3_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI3[index])
	{
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][index] = 8;
		SPI3_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_reset_SPI4(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][index] = 8;
		SPI4_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI4[index])
	{
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][index] = 8;
		SPI4_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_reset_SPI7(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][index] = 8;
		SPI7_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI7[index])
	{
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][index] = 8;
		SPI7_rapid_trigger_rgb_set(index);
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : rapid_trigger_adjust_dec
 * Description   : Convert hall raw data to component segment;
 *
 * END ****************************************************************************************************************/
void rapid_trigger_adjust_dec(void)
{
	uint8_t i;
	for(i = 0; i < TRANSFER_SIZE; i++)
	{
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI0---13是PGDN键 */
		if(i != 13)
		{
			rapid_trigger_adjust_dec_SPI0(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI1---5是FN键 */
		if(i != 5)
		{
			rapid_trigger_adjust_dec_SPI1(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI3---14是DEL键 */
		if(i != 14)
		{
			rapid_trigger_adjust_dec_SPI3(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI4---13是PGUP键 */
		if(i != 13)
		{
			rapid_trigger_adjust_dec_SPI4(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI7---14是INS */
		if((i != 14))
		{
			rapid_trigger_adjust_dec_SPI7(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : rapid_trigger_adjust_dec
 * Description   : Convert hall raw data to component segment;
 *
 * END ****************************************************************************************************************/
void rapid_trigger_adjust_dec_SPI0(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][index] > 2)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][index] -= 2;
		}
		SPI0_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI0[index])
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][index] > 2)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][index] -= 2;
		}
		SPI0_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_dec_SPI1(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][index] > 2)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][index] -= 2;
		}
		SPI1_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI1[index])
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][index] > 2)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][index] -= 2;
		}
		SPI1_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_dec_SPI3(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][index] > 2)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][index] -= 2;
		}
		SPI3_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI3[index])
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][index] > 2)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][index] -= 2;
		}
		SPI3_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_dec_SPI4(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][index] > 2)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][index] -= 2;
		}
		SPI4_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI4[index])
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][index] > 2)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][index] -= 2;
		}
		SPI4_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_dec_SPI7(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][index] > 2)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][index] -= 2;
		}
		SPI7_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI7[index])
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][index] > 2)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][index] -= 2;
		}
		SPI7_rapid_trigger_rgb_set(index);
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : rapid_trigger_adjust_inc
 * Description   : Convert hall raw data to component segment;
 *
 * END ****************************************************************************************************************/
void rapid_trigger_adjust_inc(void)
{
	uint8_t i;
	for(i = 0; i < TRANSFER_SIZE; i++)
	{
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI0---13是PGDN键 */
		if(i != 13)
		{
			rapid_trigger_adjust_inc_SPI0(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI1---5是FN键 */
		if(i != 5)
		{
			rapid_trigger_adjust_inc_SPI1(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI3---14是DEL键 */
		if(i != 14)
		{
			rapid_trigger_adjust_inc_SPI3(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI4---13是PGUP键 */
		if(i != 13)
		{
			rapid_trigger_adjust_inc_SPI4(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* SPI7---14是INS */
		if((i != 14))
		{
			rapid_trigger_adjust_inc_SPI7(i);
		}
		//---------------------------------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : Hall_Rawdata_ConvertTo_Section
 * Description   : Convert hall raw data to component segment;
 *
 * END ****************************************************************************************************************/
void rapid_trigger_adjust_inc_SPI0(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][index] < 200)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][index] += 10;
		}
		SPI0_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI0[index])
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][index] < 200)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][index] += 10;
		}
		SPI0_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_inc_SPI1(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][index] < 200)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][index] += 10;
		}
		SPI1_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI1[index])
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][index] < 200)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][index] += 10;
		}
		SPI1_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_inc_SPI3(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][index] < 200)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][index] += 10;
		}
		SPI3_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI3[index])
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][index] < 200)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][index] += 10;
		}
		SPI3_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_inc_SPI4(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][index] < 200)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][index] += 10;
		}
		SPI4_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI4[index])
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][index] < 200)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][index] += 10;
		}
		SPI4_rapid_trigger_rgb_set(index);
	}
}
void rapid_trigger_adjust_inc_SPI7(uint8_t index)
{
	if(if_adjust_all_mode)
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][index] < 200)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][index] += 10;
		}
		SPI7_rapid_trigger_rgb_set(index);
	}
	else if(trigger_point_adjust_flag_SPI7[index])
	{
		if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][index] < 200)
		{
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][index] += 10;
		}
		SPI7_rapid_trigger_rgb_set(index);
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : Hall_Rawdata_ConvertTo_Section
 * Description   : Convert hall raw data to component segment;
 *
 * END ****************************************************************************************************************/
void SPI0_rapid_trigger_rgb_set(uint8_t index)
{
	uint8_t node = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][index] >> 2;

	node -= 1;
	if(node < 5)
	{
		node = 63 - node * 5;
	}
	else if(node < 10)
	{
		node = 43 - (node - 4) * 4;
	}
	else if(node < 15)
	{
		node = 23 - (node - 9) * 2;
	}
	else
	{
		node = 13 - (node - 14) * 2;
	}
	LED_Buffer_R[SPI0_GROUP][index] = Gamma_steps[node] / 2;
	LED_Buffer_G[SPI0_GROUP][index] = 0;
	LED_Buffer_B[SPI0_GROUP][index] = Gamma_steps[node];
}
void SPI1_rapid_trigger_rgb_set(uint8_t index)
{
	uint8_t node = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][index] >> 2;

	node -= 1;
	if(node < 5)
	{
		node = 63 - node * 5;
	}
	else if(node < 10)
	{
		node = 43 - (node - 4) * 4;
	}
	else if(node < 15)
	{
		node = 23 - (node - 9) * 2;
	}
	else
	{
		node = 13 - (node - 14) * 2;
	}
	LED_Buffer_R[SPI1_GROUP][index] = Gamma_steps[node] / 2;
	LED_Buffer_G[SPI1_GROUP][index] = 0;
	LED_Buffer_B[SPI1_GROUP][index] = Gamma_steps[node];
}
void SPI3_rapid_trigger_rgb_set(uint8_t index)
{
	uint8_t node = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][index] >> 2;

	node -= 1;
	if(node < 5)
	{
		node = 63 - node * 5;
	}
	else if(node < 10)
	{
		node = 43 - (node - 4) * 4;
	}
	else if(node < 15)
	{
		node = 23 - (node - 9) * 2;
	}
	else
	{
		node = 13 - (node - 14) * 2;
	}
	LED_Buffer_R[SPI3_GROUP][index] = Gamma_steps[node] / 2;
	LED_Buffer_G[SPI3_GROUP][index] = 0;
	LED_Buffer_B[SPI3_GROUP][index] = Gamma_steps[node];
}
void SPI4_rapid_trigger_rgb_set(uint8_t index)
{
	uint8_t node = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][index] >> 2;

	node -= 1;
	if(node < 5)
	{
		node = 63 - node * 5;
	}
	else if(node < 10)
	{
		node = 43 - (node - 4) * 4;
	}
	else if(node < 15)
	{
		node = 23 - (node - 9) * 2;
	}
	else
	{
		node = 13 - (node - 14) * 2;
	}
	LED_Buffer_R[SPI4_GROUP][index] = Gamma_steps[node] / 2;
	LED_Buffer_G[SPI4_GROUP][index] = 0;
	LED_Buffer_B[SPI4_GROUP][index] = Gamma_steps[node];
}
void SPI7_rapid_trigger_rgb_set(uint8_t index)
{
	uint8_t node = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][index] >> 2;

	node -= 1;
	if(node < 5)
	{
		node = 63 - node * 5;
	}
	else if(node < 10)
	{
		node = 43 - (node - 4) * 4;
	}
	else if(node < 15)
	{
		node = 23 - (node - 9) * 2;
	}
	else
	{
		node = 13 - (node - 14) * 2;
	}
	LED_Buffer_R[SPI6_GROUP][index] = Gamma_steps[node] / 2;
	LED_Buffer_G[SPI6_GROUP][index] = 0;
	LED_Buffer_B[SPI6_GROUP][index] = Gamma_steps[node];
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : Hall_Rawdata_ConvertTo_Section
 * Description   : Convert hall raw data to component segment;
 *
 * END ****************************************************************************************************************/
void SPI0_rapid_trigger_rgb_clear(uint8_t index)
{
	LED_Buffer_R[SPI0_GROUP][index] = 0;
	LED_Buffer_G[SPI0_GROUP][index] = 0;
	LED_Buffer_B[SPI0_GROUP][index] = 0;
}
void SPI1_rapid_trigger_rgb_clear(uint8_t index)
{
	LED_Buffer_R[SPI1_GROUP][index] = 0;
	LED_Buffer_G[SPI1_GROUP][index] = 0;
	LED_Buffer_B[SPI1_GROUP][index] = 0;
}
void SPI3_rapid_trigger_rgb_clear(uint8_t index)
{
	LED_Buffer_R[SPI3_GROUP][index] = 0;
	LED_Buffer_G[SPI3_GROUP][index] = 0;
	LED_Buffer_B[SPI3_GROUP][index] = 0;
}
void SPI4_rapid_trigger_rgb_clear(uint8_t index)
{
	LED_Buffer_R[SPI4_GROUP][index] = 0;
	LED_Buffer_G[SPI4_GROUP][index] = 0;
	LED_Buffer_B[SPI4_GROUP][index] = 0;
}
void SPI7_rapid_trigger_rgb_clear(uint8_t index)
{
	LED_Buffer_R[SPI6_GROUP][index] = 0;
	LED_Buffer_G[SPI6_GROUP][index] = 0;
	LED_Buffer_B[SPI6_GROUP][index] = 0;
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : FNTAB_WhichKeyNeedAdjust
 * Description   : 判断哪些按键在FN Tab模式下能调整触发值
 *
 * END ****************************************************************************************************************/
void FNTAB_SETSPI0_RGB(uint8_t index)
{
	if(trigger_point_adjust_flag_SPI0[index])
	{
		SPI0_rapid_trigger_rgb_set(index);
	}
	else
	{
		SPI0_rapid_trigger_rgb_clear(index);
	}
}
void FNTAB_SETSPI1_RGB(uint8_t index)
{
	if(trigger_point_adjust_flag_SPI1[index])
	{
		SPI1_rapid_trigger_rgb_set(index);
	}
	else
	{
		SPI1_rapid_trigger_rgb_clear(index);
	}
}
void FNTAB_SETSPI3_RGB(uint8_t index)
{
	if(trigger_point_adjust_flag_SPI3[index])
	{
		SPI3_rapid_trigger_rgb_set(index);
	}
	else
	{
		SPI3_rapid_trigger_rgb_clear(index);
	}
}
void FNTAB_SETSPI4_RGB(uint8_t index)
{
	if(trigger_point_adjust_flag_SPI4[index])
	{
		SPI4_rapid_trigger_rgb_set(index);
	}
	else
	{
		SPI4_rapid_trigger_rgb_clear(index);
	}
}
void FNTAB_SETSPI7_RGB(uint8_t index)
{
	if(trigger_point_adjust_flag_SPI7[index])
	{
		SPI7_rapid_trigger_rgb_set(index);
	}
	else
	{
		SPI7_rapid_trigger_rgb_clear(index);
	}
}
/**************************************************************************************************************************************************************
 * FUNCTION
 *************************************************************************************************************************************************************/
void SPI7_FNMode_Scan(uint8_t Keyindex)
{
	uint8_t Hall_Segment = 0;
	DMA_SPI_RX_5_Buffer_Store[Keyindex] = DMA_SPI_RX_5_Buffer[Keyindex]&0x3FFF;
	if((DMA_SPI_RX_5_Buffer_Store[Keyindex] < (DMA_SPI_RX_5_Buffer_BackUp[Keyindex]-200))
	|| (DMA_SPI_RX_5_Buffer_Store[Keyindex] > (DMA_SPI_RX_5_Buffer_BackUp[Keyindex]+200)))
	{
		if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI6(Keyindex))
		{
			//------------------------------------------------------------------------------------------
			DMA_SPI_RX_5_Buffer_BackUp[Keyindex] = DMA_SPI_RX_5_Buffer_Store[Keyindex];
			//------------------------------------------------------------------------------------------
			//Segmented
			Hall_Segment = Hall_Cal_Node_Data_SPI7(Keyindex);
			//------------------------------------------------------------------------------------------
			/* handle Key data */
			if(Hall_Segment > Segment_Last_SPI[SPI6_GROUP][Keyindex])
			{
				/* 没有按键按下的时候等按键按到触发值后才按下，按键按下后就每次分段值变化就来更新分段值 */
				if(!Current_key_state[SPI6_GROUP][Keyindex])
				{
					/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
					if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][Keyindex] >= TriggerPoint_MAX)
					{
						if((Hall_Segment - Segment_Last_SPI[SPI6_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][Keyindex]))
						{
							Segment_Last_SPI[SPI6_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI6_GROUP][Keyindex] = 1;
						}
					}
					else
					{
						if((Hall_Segment - Segment_Last_SPI[SPI6_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][Keyindex] + Hall_Trigger_Adjust_Value_Press))
						{
							Segment_Last_SPI[SPI6_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI6_GROUP][Keyindex] = 1;
						}
					}
				}
				else
				{
					Segment_Last_SPI[SPI6_GROUP][Keyindex] = Hall_Segment;
				}
			}
			//Break
			else if(Hall_Segment < Segment_Last_SPI[SPI6_GROUP][Keyindex])
			{
				/* 有按键按下后,Segment_Last道和Current_key_state更新，抬起的时候抬超过触发值后才更新Segment_Last，没按下的时候，Segment_Last都是之前的 */
				if(Current_key_state[SPI6_GROUP][Keyindex])
				{
					/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
					if(g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][Keyindex] == TriggerPoint_MAX)
					{
						if((Segment_Last_SPI[SPI6_GROUP][Keyindex] - Hall_Segment) >= g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][Keyindex])
						{
							Segment_Last_SPI[SPI6_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI6_GROUP][Keyindex] = 0;
						}
					}
					else
					{
						if((Segment_Last_SPI[SPI6_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][Keyindex] + Hall_Trigger_Adjust_Value_Break))
						{
							Segment_Last_SPI[SPI6_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI6_GROUP][Keyindex] = 0;
						}
					}
				}
				else
				{
					Segment_Last_SPI[SPI6_GROUP][Keyindex] = Hall_Segment;
				}
			}
		}
		//------------------------------------------------------------------------------------------
	}
}
/**************************************************************************************************************************************************************
 * FUNCTION
 *************************************************************************************************************************************************************/
void SPI4_FNMode_Scan(uint8_t Keyindex)
{
	uint8_t Hall_Segment = 0;
	DMA_SPI_RX_4_Buffer_Store[Keyindex] = DMA_SPI_RX_4_Buffer[Keyindex]&0x3FFF;
	if((DMA_SPI_RX_4_Buffer_Store[Keyindex] < (DMA_SPI_RX_4_Buffer_BackUp[Keyindex]-200))
	|| (DMA_SPI_RX_4_Buffer_Store[Keyindex] > (DMA_SPI_RX_4_Buffer_BackUp[Keyindex]+200)))
	{
		if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI4(Keyindex))
		{
			//------------------------------------------------------------------------------------------
			DMA_SPI_RX_4_Buffer_BackUp[Keyindex] = DMA_SPI_RX_4_Buffer_Store[Keyindex];
			//------------------------------------------------------------------------------------------
			//Segmented
			Hall_Segment = Hall_Cal_Node_Data_SPI4(Keyindex);
			//------------------------------------------------------------------------------------------
			/* handle Key data */
			if(Hall_Segment > Segment_Last_SPI[SPI4_GROUP][Keyindex])
			{
				/* 没有按键按下的时候等按键按到触发值后才按下，按键按下后就每次分段值变化就来更新分段值 */
				if(!Current_key_state[SPI4_GROUP][Keyindex])
				{
					/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
					if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][Keyindex] >= TriggerPoint_MAX)
					{
						if((Hall_Segment - Segment_Last_SPI[SPI4_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][Keyindex]))
						{
							Segment_Last_SPI[SPI4_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI4_GROUP][Keyindex] = 1;
						}
					}
					else
					{
						if((Hall_Segment - Segment_Last_SPI[SPI4_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][Keyindex] + Hall_Trigger_Adjust_Value_Press))
						{
							Segment_Last_SPI[SPI4_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI4_GROUP][Keyindex] = 1;
						}
					}
				}
				else
				{
					Segment_Last_SPI[SPI4_GROUP][Keyindex] = Hall_Segment;
				}
			}
			//Break
			else if(Hall_Segment < Segment_Last_SPI[SPI4_GROUP][Keyindex])
			{
				/* 有按键按下后,Segment_Last道和Current_key_state更新，抬起的时候抬超过触发值后才更新Segment_Last，没按下的时候，Segment_Last都是之前的 */
				if(Current_key_state[SPI4_GROUP][Keyindex])
				{
					/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
					if(g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][Keyindex] == TriggerPoint_MAX)
					{
						if((Segment_Last_SPI[SPI4_GROUP][Keyindex] - Hall_Segment) >= g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][Keyindex])
						{
							Segment_Last_SPI[SPI4_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI4_GROUP][Keyindex] = 0;
						}
					}
					else
					{
						if((Segment_Last_SPI[SPI4_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][Keyindex] + Hall_Trigger_Adjust_Value_Break))
						{
							Segment_Last_SPI[SPI4_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI4_GROUP][Keyindex] = 0;
						}
					}
				}
				else
				{
					Segment_Last_SPI[SPI4_GROUP][Keyindex] = Hall_Segment;
				}
			}
		}
		//------------------------------------------------------------------------------------------
	}
}
/**************************************************************************************************************************************************************
 * FUNCTION
 *************************************************************************************************************************************************************/
void SPI3_FNMode_Scan(uint8_t Keyindex)
{
	uint8_t Hall_Segment = 0;
	DMA_SPI_RX_3_Buffer_Store[Keyindex] = DMA_SPI_RX_3_Buffer[Keyindex]&0x3FFF;
	if((DMA_SPI_RX_3_Buffer_Store[Keyindex] < (DMA_SPI_RX_3_Buffer_BackUp[Keyindex]-200))
	|| (DMA_SPI_RX_3_Buffer_Store[Keyindex] > (DMA_SPI_RX_3_Buffer_BackUp[Keyindex]+200)))
	{
		if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI3(Keyindex))
		{
			//------------------------------------------------------------------------------------------
			DMA_SPI_RX_3_Buffer_BackUp[Keyindex] = DMA_SPI_RX_3_Buffer_Store[Keyindex];
			//------------------------------------------------------------------------------------------
			//Segmented
			Hall_Segment = Hall_Cal_Node_Data_SPI3(Keyindex);
			//------------------------------------------------------------------------------------------
			/* handle Key data */
			if(Hall_Segment > Segment_Last_SPI[SPI3_GROUP][Keyindex])
			{
				/* 没有按键按下的时候等按键按到触发值后才按下，按键按下后就每次分段值变化就来更新分段值 */
				if(!Current_key_state[SPI3_GROUP][Keyindex])
				{
					/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
					if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][Keyindex] >= TriggerPoint_MAX)
					{
						if((Hall_Segment - Segment_Last_SPI[SPI3_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][Keyindex]))
						{
							Segment_Last_SPI[SPI3_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI3_GROUP][Keyindex] = 1;
						}
					}
					else
					{
						if((Hall_Segment - Segment_Last_SPI[SPI3_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][Keyindex] + Hall_Trigger_Adjust_Value_Press))
						{
							Segment_Last_SPI[SPI3_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI3_GROUP][Keyindex] = 1;
						}
					}
				}
				else
				{
					Segment_Last_SPI[SPI3_GROUP][Keyindex] = Hall_Segment;
				}
			}
			//Break
			else if(Hall_Segment < Segment_Last_SPI[SPI3_GROUP][Keyindex])
			{
				/* 有按键按下后,Segment_Last道和Current_key_state更新，抬起的时候抬超过触发值后才更新Segment_Last，没按下的时候，Segment_Last都是之前的 */
				if(Current_key_state[SPI3_GROUP][Keyindex])
				{
					/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
					if(g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][Keyindex] == TriggerPoint_MAX)
					{
						if((Segment_Last_SPI[SPI3_GROUP][Keyindex] - Hall_Segment) >= g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][Keyindex])
						{
							Segment_Last_SPI[SPI3_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI3_GROUP][Keyindex] = 0;
						}
					}
					else
					{
						if((Segment_Last_SPI[SPI3_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][Keyindex] + Hall_Trigger_Adjust_Value_Break))
						{
							Segment_Last_SPI[SPI3_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI3_GROUP][Keyindex] = 0;
						}
					}
				}
				else
				{
					Segment_Last_SPI[SPI3_GROUP][Keyindex] = Hall_Segment;
				}
			}
		}
		//------------------------------------------------------------------------------------------
	}
}
/**************************************************************************************************************************************************************
 * FUNCTION
 *************************************************************************************************************************************************************/
void SPI1_FNMode_Scan(uint8_t Keyindex)
{
	uint8_t Hall_Segment = 0;
	DMA_SPI_RX_2_Buffer_Store[Keyindex] = DMA_SPI_RX_2_Buffer[Keyindex]&0x3FFF;
	if((DMA_SPI_RX_2_Buffer_Store[Keyindex] < (DMA_SPI_RX_2_Buffer_BackUp[Keyindex]-200))
	|| (DMA_SPI_RX_2_Buffer_Store[Keyindex] > (DMA_SPI_RX_2_Buffer_BackUp[Keyindex]+200)))
	{
		if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI1(Keyindex))
		{
			//------------------------------------------------------------------------------------------
			DMA_SPI_RX_2_Buffer_BackUp[Keyindex] = DMA_SPI_RX_2_Buffer_Store[Keyindex];
			//------------------------------------------------------------------------------------------
			//Segmented
			Hall_Segment = Hall_Cal_Node_Data_SPI1(Keyindex);
			//------------------------------------------------------------------------------------------
			/* handle Key data */
			if(Hall_Segment > Segment_Last_SPI[SPI1_GROUP][Keyindex])
			{
				/* 没有按键按下的时候等按键按到触发值后才按下，按键按下后就每次分段值变化就来更新分段值 */
				if(!Current_key_state[SPI1_GROUP][Keyindex])
				{
					/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
					if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][Keyindex] >= TriggerPoint_MAX)
					{
						if((Hall_Segment - Segment_Last_SPI[SPI1_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][Keyindex]))
						{
							Segment_Last_SPI[SPI1_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI1_GROUP][Keyindex] = 1;
						}
					}
					else
					{
						if((Hall_Segment - Segment_Last_SPI[SPI1_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][Keyindex] + Hall_Trigger_Adjust_Value_Press))
						{
							Segment_Last_SPI[SPI1_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI1_GROUP][Keyindex] = 1;
						}
					}
				}
				else
				{
					Segment_Last_SPI[SPI1_GROUP][Keyindex] = Hall_Segment;
				}
			}
			//Break
			else if(Hall_Segment < Segment_Last_SPI[SPI1_GROUP][Keyindex])
			{
				/* 有按键按下后,Segment_Last道和Current_key_state更新，抬起的时候抬超过触发值后才更新Segment_Last，没按下的时候，Segment_Last都是之前的 */
				if(Current_key_state[SPI1_GROUP][Keyindex])
				{
					/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
					if(g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][Keyindex] == TriggerPoint_MAX)
					{
						if((Segment_Last_SPI[SPI1_GROUP][Keyindex] - Hall_Segment) >= g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][Keyindex])
						{
							Segment_Last_SPI[SPI1_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI1_GROUP][Keyindex] = 0;
						}
					}
					else
					{
						if((Segment_Last_SPI[SPI1_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][Keyindex] + Hall_Trigger_Adjust_Value_Break))
						{
							Segment_Last_SPI[SPI1_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI1_GROUP][Keyindex] = 0;
						}
					}
				}
				else
				{
					Segment_Last_SPI[SPI1_GROUP][Keyindex] = Hall_Segment;
				}
			}
		}
		//------------------------------------------------------------------------------------------
	}
}
/**************************************************************************************************************************************************************
 * FUNCTION
 *************************************************************************************************************************************************************/
void SPI0_FNMode_Scan(uint8_t Keyindex)
{
	uint8_t Hall_Segment = 0;
	DMA_SPI_RX_1_Buffer_Store[Keyindex] = DMA_SPI_RX_1_Buffer[Keyindex]&0x3FFF;
	if((DMA_SPI_RX_1_Buffer_Store[Keyindex] < (DMA_SPI_RX_1_Buffer_BackUp[Keyindex]-200))
	|| (DMA_SPI_RX_1_Buffer_Store[Keyindex] > (DMA_SPI_RX_1_Buffer_BackUp[Keyindex]+200)))
	{
		if(ROWDATA_CRC_SUCCESS ==  Hall_Check_Rawdata_CRC_SPI0(Keyindex))
		{
			//------------------------------------------------------------------------------------------
			DMA_SPI_RX_1_Buffer_BackUp[Keyindex] = DMA_SPI_RX_1_Buffer_Store[Keyindex];
			//------------------------------------------------------------------------------------------
			//Segmented
			Hall_Segment = Hall_Cal_Node_Data_SPI0(Keyindex);
			//------------------------------------------------------------------------------------------
			/* handle Key data */
			if(Hall_Segment > Segment_Last_SPI[SPI0_GROUP][Keyindex])
			{
				/* 没有按键按下的时候等按键按到触发值后才按下，按键按下后就每次分段值变化就来更新分段值 */
				if(!Current_key_state[SPI0_GROUP][Keyindex])
				{
					/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
					if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][Keyindex] >= TriggerPoint_MAX)
					{
						if((Hall_Segment - Segment_Last_SPI[SPI0_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][Keyindex]))
						{
							Segment_Last_SPI[SPI0_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI0_GROUP][Keyindex] = 1;
						}
					}
					else
					{
						if((Hall_Segment - Segment_Last_SPI[SPI0_GROUP][Keyindex]) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][Keyindex] + Hall_Trigger_Adjust_Value_Press))
						{
							Segment_Last_SPI[SPI0_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI0_GROUP][Keyindex] = 1;
						}
					}
				}
				else
				{
					Segment_Last_SPI[SPI0_GROUP][Keyindex] = Hall_Segment;
				}
			}
			//Break
			else if(Hall_Segment < Segment_Last_SPI[SPI0_GROUP][Keyindex])
			{
				/* 有按键按下后,Segment_Last道和Current_key_state更新，抬起的时候抬超过触发值后才更新Segment_Last，没按下的时候，Segment_Last都是之前的 */
				if(Current_key_state[SPI0_GROUP][Keyindex])
				{
					/* 按最长的时候就不用加了，不然设置成40的时候再加就超了，没法判断按下抬起了 */
					if(g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][Keyindex] == TriggerPoint_MAX)
					{
						if((Segment_Last_SPI[SPI0_GROUP][Keyindex] - Hall_Segment) >= g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][Keyindex])
						{
							Segment_Last_SPI[SPI0_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI0_GROUP][Keyindex] = 0;
						}
					}
					else
					{
						if((Segment_Last_SPI[SPI0_GROUP][Keyindex] - Hall_Segment) >= (g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][Keyindex] + Hall_Trigger_Adjust_Value_Break))
						{
							Segment_Last_SPI[SPI0_GROUP][Keyindex] = Hall_Segment;
							Current_key_state[SPI0_GROUP][Keyindex] = 0;
						}
					}
				}
				else
				{
					Segment_Last_SPI[SPI0_GROUP][Keyindex] = Hall_Segment;
				}
			}
		}
		//------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : rapid_trigger_adjust_handle
 * Description   : 处理fn tab模式下触发值
 *
 * END ****************************************************************************************************************/
void rapid_trigger_adjust_handle(uint8_t key_SPIGroup, uint8_t key_index)
{
	uint16_t i, Keycode;

	Keycode = KeyBoardLayout_FNMODE[key_SPIGroup][key_index];

	if((Keycode != KEY_FN) && (Keycode != KEY_INSERT) && (Keycode != KEY_DELETE) && (Keycode != KEY_PAGEUP) && (Keycode != KEY_PAGEDOWN) && (Keycode != RESERVE))
	{
		if(if_adjust_all_mode == 1)
		{
			rapid_trigger_adjust_mode_ex();
		}
		//------------------------------------------------------------------------------------------
		/* 这里是做在FN tab模式按一个按键亮，再按灭的操作 */
		/* SPI0 */
		if(key_SPIGroup == 0)
		{
			if(trigger_point_adjust_flag_SPI0[key_index] == 0)
			{
				trigger_point_adjust_flag_SPI0[key_index] = 1;
				SPI0_rapid_trigger_rgb_set(key_index);
			}
			else if(trigger_point_adjust_flag_SPI0[key_index] == 1)
			{
				trigger_point_adjust_flag_SPI0[key_index] = 0;
				SPI0_rapid_trigger_rgb_clear(key_index);
			}
		}
		//------------------------------------------------------------------------------------------
		/* SPI1 */
		if(key_SPIGroup == 1)
		{
			if(trigger_point_adjust_flag_SPI1[key_index] == 0)
			{
				trigger_point_adjust_flag_SPI1[key_index] = 1;
				SPI1_rapid_trigger_rgb_set(key_index);
			}
			else if(trigger_point_adjust_flag_SPI1[key_index] == 1)
			{
				trigger_point_adjust_flag_SPI1[key_index] = 0;
				SPI1_rapid_trigger_rgb_clear(key_index);
			}
		}
		//------------------------------------------------------------------------------------------
		/* SPI3 */
		if(key_SPIGroup == 2)
		{
			if(trigger_point_adjust_flag_SPI3[key_index] == 0)
			{
				trigger_point_adjust_flag_SPI3[key_index] = 1;
				SPI3_rapid_trigger_rgb_set(key_index);
			}
			else if(trigger_point_adjust_flag_SPI3[key_index] == 1)
			{
				trigger_point_adjust_flag_SPI3[key_index] = 0;
				SPI3_rapid_trigger_rgb_clear(key_index);
			}
		}
		//------------------------------------------------------------------------------------------
		/* SPI4 */
		if(key_SPIGroup == 3)
		{
			if(trigger_point_adjust_flag_SPI4[key_index] == 0)
			{
				trigger_point_adjust_flag_SPI4[key_index] = 1;
				SPI4_rapid_trigger_rgb_set(key_index);
			}
			else if(trigger_point_adjust_flag_SPI4[key_index] == 1)
			{
				trigger_point_adjust_flag_SPI4[key_index] = 0;
				SPI4_rapid_trigger_rgb_clear(key_index);
			}
		}
		//------------------------------------------------------------------------------------------
		/* SPI7 */
		if(key_SPIGroup == 4)
		{
			if(trigger_point_adjust_flag_SPI7[key_index] == 0)
			{
				trigger_point_adjust_flag_SPI7[key_index] = 1;
				SPI7_rapid_trigger_rgb_set(key_index);
			}
			else if(trigger_point_adjust_flag_SPI7[key_index] == 1)
			{
				trigger_point_adjust_flag_SPI7[key_index] = 0;
				SPI7_rapid_trigger_rgb_clear(key_index);
			}
		}
		//------------------------------------------------------------------------------------------
		Flag_LedMode_RGBUpdate |= LED_Update_IRGB;
		//------------------------------------------------------------------------------------------
	}
	/* 做整体按键亮和灭的操作 */
	else if(Keycode == KEY_DELETE)
	{
		rapid_trigger_adjust_mode_ex();
	}
	/* 触发值恢复初值 */
	else if(Keycode == KEY_INSERT)
	{
		rapid_trigger_adjust_reset();
	}
	/* 触发值增加 */
	else if(Keycode == KEY_PAGEUP)
	{
		rapid_trigger_adjust_dec();
	}
	/* 触发值减小 */
	else if(Keycode == KEY_PAGEDOWN)
	{
		rapid_trigger_adjust_inc();
	}
	/* 保存退出 */
	else if(Keycode == KEY_FN)
	{
		rapid_trigger_adjust_start();
		//---------------------------------------------------------------------------------------------------------------------
		for(i = 0; i < TRANSFER_SIZE; i++)
		{
			//---------------------------------------------------------------------------------------------------------------------
			/* 退出高亮 */
			//---------------------------------------------------------------------------------------------------------------------
			Flag_LedMode_RGBUpdate |= (LED_Update_Clear_Highlight);
			//---------------------------------------------------------------------------------------------------------------------
			if(!g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][i])
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][i] = 2;
			}

			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][i] > 20)
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][i] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][i] - 20;
			}
			else
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][i] = 2;
			}
			//---------------------------------------------------------------------------------------------------------------------
			if(!g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][i])
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][i] = 2;
			}

			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][i] > 20)
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][i] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][i] - 20;
			}
			else
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][i] = 2;
			}
			//---------------------------------------------------------------------------------------------------------------------
			if(!g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][i])
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][i] = 2;
			}

			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][i] > 20)
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][i] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][i] - 20;
			}
			else
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][i] = 2;
			}
			//---------------------------------------------------------------------------------------------------------------------
			if(!g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][i])
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][i] = 2;
			}

			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][i] > 20)
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][i] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][i] - 20;
			}
			else
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][i] = 2;
			}
			//---------------------------------------------------------------------------------------------------------------------
			if(!g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][i])
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][i] = 2;
			}

			if(g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][i] > 20)
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][i] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][i] - 20;
			}
			else
			{
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][i] = 2;
			}
			//---------------------------------------------------------------------------------------------------------------------
			Current_key_state[0][i] = 0;
			Current_key_state[1][i] = 0;
			Current_key_state[2][i] = 0;
			Current_key_state[3][i] = 0;
			Current_key_state[4][i] = 0;
			//---------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------
		/* 保存到flash */
		Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_TrigerPoint;
		Tick_UpdateMCUFlash = 0;
		//---------------------------------------------------------------------------------------------------------------------
		device_mode = MODE_NORMAL;
		//---------------------------------------------------------------------------------------------------------------------
		/* 恢复灯效 */
		Flag_LedMode_ModeInit = 1;
		//---------------------------------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : Hall_Rawdata_ConvertTo_Section
 * Description   : Convert hall raw data to component segment;
 *
 * END ****************************************************************************************************************/
void FnTabMode_KeyScan(void)
{
	uint8_t i = 0;
	uint8_t Hall_Segment = 0;
	//------------------------------------------------------------------------------------------
	//Hall_Sample_Rawdata();
	DMA_StartTransfer_Once_TESTAPP();
	//------------------------------------------------------------------------------------------
	for(i = 0; i<TRANSFER_SIZE; i++)
	{
		//------------------------------------------------------------------------------------------
		if(i < SPI0_TRANSFER_SIZE)
		{
			SPI0_FNMode_Scan(i);
		}
		//------------------------------------------------------------------------------------------
		if(i < SPI1_TRANSFER_SIZE)
		{
			SPI1_FNMode_Scan(i);
		}
		//------------------------------------------------------------------------------------------
		if(i < SPI3_TRANSFER_SIZE)
		{
			SPI3_FNMode_Scan(i);
		}
		//------------------------------------------------------------------------------------------
		if(i < SPI4_TRANSFER_SIZE)
		{
			SPI4_FNMode_Scan(i);
		}
		//------------------------------------------------------------------------------------------
		if(i < SPI7_TRANSFER_SIZE)
		{
			SPI7_FNMode_Scan(i);
		}
		//------------------------------------------------------------------------------------------
	}
	//------------------------------------------------------------------------------------------
}
void FNTAB_MODE_DataHandle(void)
{
	uint8_t i,j;

	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			if(Current_key_state[i][j] != LastCurrent_key_state[i][j])
			{
				LastCurrent_key_state[i][j] = Current_key_state[i][j];
				if((Current_key_state[i][j] == 1))
				{
					rapid_trigger_adjust_handle(i, j);
					//------------------------------------------------------------------------------------------
					Flag_LedMode_RGBUpdate |= LED_Update_IRGB;
					//------------------------------------------------------------------------------------------
				}
			}
		}
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : FNTabMode_Process
 * Description   : 在fn tab 模式下处理数据。
 *
 * END ****************************************************************************************************************/
void FNTabMode_Process(void)
{
	uint8_t i,j;
	if(++Tick_FnTabCycle > 3)
	{
		Tick_FnTabCycle = 0;
		//------------------------------------------------------------------------------------------
		/* 初始化后点十次灯，防止有时候只点一次的话灯亮不规律 */
		if(Flag_FnMode_TurnOnLed_cnt < 10)
		{
			//------------------------------------------------------------------------------------------
			Flag_LedMode_RGBUpdate |= LED_Update_IRGB;
			//------------------------------------------------------------------------------------------
		}
		//------------------------------------------------------------------------------------------
		if(Flag_FNMode_Init)
		{
			Flag_FNMode_Init = 0;
			//------------------------------------------------------------------------------------------
			/* 切换到FN Tab模式后，先做一下初始化的灯光颜色 */
			/* 进FN tab后要高亮 */
			rapid_trigger_adjust_start();
			//------------------------------------------------------------------------------------------
			Flag_LedMode_RGBUpdate |= (LED_Update_IRGB | LED_Update_SET_Highlight);
			//------------------------------------------------------------------------------------------
			Tick_10ms_LEDUpdatePWM = 0;
			Flag_FnMode_TurnOnLed_cnt = 0;
			/* 把FN和tab的按下标志位强行置位，防止切换到FN tab模式后会误触退出模式 */
			Current_key_state[1][5] = 1;
			Current_key_state[2][0] = 1;
			//------------------------------------------------------------------------------------------
			/* 补抬起 */
			for(i = 0; i < TRANSFER_Group; i++)
			{
				for(j = 0; j < TRANSFER_SIZE; j++)
				{
					LastCurrent_key_state[i][j] = Current_key_state[i][j];
				}
			}

			for(i = 0; i < USB_HID_KEYBOARD_REPORT_LENGTH; i++)
			{
				s_KeyboardBuffer1_DKS[i]=0;
				s_KeyboardBuffer2_DKS[i] = 0;
			}
			Has_Key_Report_Interface1_DKS = 1;
			Has_Key_Report_Interface2_DKS = 1;
			//------------------------------------------------------------------------------------------
		}
		else
		{
			FnTabMode_KeyScan();
			FNTAB_MODE_DataHandle();
		}
	}
}


