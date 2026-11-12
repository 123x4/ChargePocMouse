/*
 * hasp_drv.c
 *
 *  Created on:
 *      Author:
 */

#include "hasp_drv.h"

#include "gtry_regmap.h"
#include "gtry_platform.h"

p_hasp_int_cb hasp_int_cb = NULL;
uint32_t int_state = 0;



/**
 * \brief     Clear HASP TD timers
 *
 * \param[in] None
 * \return 	  None
 */
void hasp_drv_td_init(void)
{
	GTRY_HASP->TD1 = 0;
	GTRY_HASP->TD2 = 0;
	GTRY_HASP->TD3 = 0;
	GTRY_HASP->TD5 = 0;
	GTRY_HASP->TD6 = 0;
	GTRY_HASP->TD7 = 0;
	GTRY_HASP->TD9 = 0;
	GTRY_HASP->TD10 = 0;
	GTRY_HASP->TD11 = 0;
	GTRY_HASP->TD13 = 0;
	GTRY_HASP->TD14 = 0;
	GTRY_HASP->TD15 = 0;
	GTRY_HASP->TD17 = 0;
}

/**
 * \brief     HASP interrupt initialization
 *
 * \param[in] int_callback: interrupt callback function
 * \return 	  None
 */
void hasp_drv_int_init(p_hasp_int_cb int_callback)
{
	// Priority must be set > 0 to trigger the interrupt
	__nds__plic_set_priority(IRQ_HASP_SOURCE, 1);//

	// Enable PLIC interrupt SPI source
	__nds__plic_enable_interrupt(IRQ_HASP_SOURCE);

	if(int_callback != NULL)
	{
		hasp_int_cb = int_callback;
	}
	else
	{
		hasp_int_cb = NULL;
	}
}

/**
 * \brief     HASP interrupt callback function
 *
 * \param[in] int_state: interrupt status
 * \return 	  None
 */
void hasp_drv_int_callback(uint32_t int_state)
{
//	if(int_state & KEY_BUSY_INT_MASK)
//	{
//		GTRY_HASP->INTEN &= ~KEY_BUSY_INT_MASK;
//		debug("KEY_BUSY_INT_MASK : 0x%x!\n", int_state&KEY_BUSY_INT_MASK);
//	}
//	if(int_state & DATA_ERR_INT_MASK)
//	{
//		debug("HASP DATA ERR INT : 0x%x!\n", int_state&DATA_ERR_INT_MASK);
//	}
//	if(int_state & CRC_ERR_INT_MASK)
//	{
//		debug("HASP CRC ERR INT : 0x%x!\n", int_state&CRC_ERR_INT_MASK);
//	}
//	if(int_state & TRANS_END_INT_MASK)
//	{
//		debug("HASP TRANS END INT : 0x%x!\n", int_state&TRANS_END_INT_MASK);
//	}
//	if(int_state & ONCE_TRANS_END_INT_MASK)
//	{
//		debug("HASP ONCE TRANS END INT : 0x%x!\n", int_state&ONCE_TRANS_END_INT_MASK);
//	}
}

/**
 * \brief     HASP interrupt handler function
 *
 * \param[in] None
 * \return 	  None
 */
void hasp_irq_handler(void)
{
	uint32_t int_state = GTRY_HASP->INTST;

	GTRY_HASP->INTST |= int_state;

	if(hasp_int_cb != NULL)
	{
		hasp_int_cb(int_state);
	}
}

/**
 * \brief     HASP clk init
 *
 * \param[in] None
 * \return 	  None
 */
void hasp_drv_clk_init(void)
{
	GTRY_HASP->INNER_CLK_INIT = INNERCLK_INIT_VAL(((GET_REG32(0xF01001C8) >> 15)&0x7f)<<4);
	GTRY_HASP->INNER_CLK_CTRL = SCK_SRC_INNERCLK|INNER_CLK_ENABLE|INNER_CLK_BAND(GET_REG32(0xF01001C8) >> 29);
}






