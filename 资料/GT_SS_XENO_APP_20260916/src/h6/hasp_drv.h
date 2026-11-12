/*
 * hasp_drv.h
 *
 *  Created on:
 *      Author:
 */

#ifndef SRC_H6_HASP_DRV_H_
#define SRC_H6_HASP_DRV_H_

#include <stdio.h>
#include "hasp_regmap.h"
#include "gtry_platform.h"
#include "gtry_drv_common.h"


typedef void (*p_hasp_int_cb)(uint32_t int_state);

#define HASP_KEY_NUM_MAX			128

//***********************************************LPCFG register offset**********************************************************/
#define LPCFG_DATABIT(n)				((n & 0xf) << 24)
#define LPCFG_BTOPLP(n)					((n & 0x1f) << 16)
#define LPCFG_KDLP(n)					(n & 0x1f)
//*********************************************LPCFG_RECVSTEP register offset***************************************************/
#define LPCFG_RECVSTEP(n)				((n & 0x1FFFF) << 8)
#define LPCFG_DLT(n)					(n & 0x1f)
//*********************************************FDDEL_KDLP register offset***************************************************/
#define FDDEL_KDLP(n)					((n & 0x3FFF) << 8)
//*********************************************FDDEL_BBOTLP register offset***************************************************/
#define FDDEL_KDLP(n)					((n & 0x3FFF) << 8)
//*********************************************FDDEL_BTOPLP register offset***************************************************/
#define FDDEL_BTOPLP(n)					((n & 0x3FFF) << 8)
//*********************************************NOISEV_KDLP register offset***************************************************/
#define NOISEV_KDLP(n)					((n & 0x3FFF) << 8)
//*********************************************NOISEV_BBOTLP register offset***************************************************/
#define NOISEV_BBOTLP(n)				((n & 0x3FFF) << 8
//*********************************************NOISEV_BTOPLP register offset***************************************************/
#define NOISEV_BTOPLP(n)				((n & 0x3FFF) << 8
//*********************************************BBOTLP_PDLT register offset***************************************************/
#define	BBOTLP_PDLT_RELA_MODE			(0<<14)
#define	BBOTLP_PDLT_ABSO_MODE			(1<<14)
#define BBOTLP_PDLT(n)					((n & 0x3FFF) << 8
//*********************************************BBOTLP_NDLT register offset***************************************************/
#define	BBOTLP_NDLT_RELA_MODE			(0<<14)
#define	BBOTLP_NDLT_ABSO_MODE			(1<<14)
#define BBOTLP_NDLT(n)					((n & 0x3FFF) << 8
//*********************************************BTOPLP_PDLT register offset***************************************************/
#define	BTOPLP_PDLT_RELA_MODE			(0<<14)
#define	BTOPLP_PDLT_ABSO_MODE			(1<<14)
#define BTOPLP_PDLT(n)					((n & 0x3FFF) << 8
//*********************************************BTOPLP_NDLT register offset***************************************************/
#define	BTOPLP_NDLT_RELA_MODE			(0<<14)
#define	BTOPLP_NDLT_ABSO_MODE			(1<<14)
#define BTOPLP_NDLT(n)					((n & 0x3FFF) << 8
//***********************************************HASPCTRL register offset**********************************************************/
#define HASP_EN							(1 << 31)
#define KDATALP_EN						(1 << 30)
#define BTOPLP_EN						(1 << 29)
#define BBOTLP_EN						(1 << 28)
#define CHXSCKMODE(n)					(((n) & 0xff) << 20)
#define BIT_NUM(n)						(((n) & 0xf) << 16)
#define CRCCHECK_EN						(1 << 15)
#define CHANNL_NUM(n)					(((n) & 0x7) << 12)
#define SDI_AUTO_CONV_EN				(1 << 11)
#define HASP_CONTINUE_CONV				(1 << 10)
#define HASP_SINGLE_CONV				(0 << 10)
#define AUTO_UPDATE_BTOP_EN				(1 << 9)
#define AUTO_UPDATE_BBOT_EN				(1 << 8)
#define BTOPLP_CAL_EN					(1 << 7)
#define BBOTLP_CAL_EN					(1 << 6)
#define	HASP_DRV_SCK_DIV(n)				(((n) == 0 ? 0 : ((n)-1)) << 0)
//***********************************************TD1 register offset**********************************************************/
#define	TD1(n)							(n & 0x3FFFF)
//***********************************************TD2 register offset**********************************************************/
#define	TD2(n)							(n & 0x3FFFF)
//***********************************************TD3 register offset**********************************************************/
#define	TD3(n)							(n & 0x3FFFF)
//***********************************************TD5 register offset**********************************************************/
#define	TD5(n)							(n & 0x3FFFF)
//***********************************************TD6 register offset**********************************************************/
#define	TD6(n)							(n & 0x3FFFF)
//***********************************************TD7 register offset**********************************************************/
#define	TD7(n)							(n & 0x3FFFF)
//***********************************************TD9 register offset**********************************************************/
#define	TD9(n)							(n & 0x3FFFF)
//***********************************************TD10 register offset**********************************************************/
#define	TD10(n)							(n & 0x3FFFF)
//***********************************************TD11 register offset**********************************************************/
#define	TD11(n)							(n & 0x3FFFF)
//***********************************************TD13 register offset**********************************************************/
#define	TD13(n)							(n & 0x3FFFF)
//***********************************************TD14 register offset**********************************************************/
#define	TD14(n)							(n & 0x3FFFF)
//***********************************************TD15 register offset**********************************************************/
#define	TD15(n)							(n & 0x3FFFF)
//***********************************************TD17 register offset**********************************************************/
#define	TD17(n)							(n & 0x3FFFF)
//***********************************************NSCK register offset**********************************************************/
#define	SECTION4_REG_DATA				(0 << 31)
#define	SECTION4_KEY_DATA				(1 << 31)
#define	NSCK4(n)						((n & 0x7F) << 24)
#define	SECTION3_REG_DATA				(0 << 23)
#define	SECTION3_KEY_DATA				(1 << 23)
#define	NSCK3(n)						((n & 0x7F) << 16)
#define	SECTION2_REG_DATA				(0 << 15)
#define	SECTION2_KEY_DATA				(1 << 15)
#define	NSCK2(n)						((n & 0x7F) << 8)
#define	SECTION1_REG_DATA				(0 << 7)
#define	SECTION1_KEY_DATA				(1 << 7)
#define	NSCK1(n)						(n & 0x7F)
//*****************************************MAX_ABSDATA register offset********************************************************/
#define	MAX_ABSDATA(n)					(n & 0xFFFF)
//*****************************************MIN_ABSDATA register offset********************************************************/
#define	MIN_ABSDATA(n)					(n & 0xFFFF)
//*****************************************RAW2ABSREF register offset********************************************************/
#define	RAW2ABSREF(n)					(n & 0x7FFF)
//*****************************************BTOP_CALCYCLE register offset********************************************************/
#define	BTOP_CALCYCLE(n)				(n & 0xFFFF)
//*****************************************BBOT_CALCYCLE register offset********************************************************/
#define	BBOT_CALCYCLE(n)				(n & 0xFFFF)
//*****************************************BTOP_ADDCNT register offset********************************************************/
#define	BTOP_ADDCNT(n)					(n & 0x3FFF)
//*****************************************BTOP_SUBCNT register offset********************************************************/
#define	BTOP_SUBCNT(n)					(n & 0x3FFF)
//*****************************************BTOP_ADDDLT register offset********************************************************/
#define	BTOP_ADDDLT(n)					(n & 0xFFFF)
//*****************************************BTOP_SUBDLT register offset********************************************************/
#define	BTOP_SUBDLT(n)					(n & 0xFFFF)
//*****************************************BTOP_NHYST register offset********************************************************/
#define	BTOP_NHYST(n)					(n & 0xFFFF)
//*****************************************BTOP_PHYST register offset********************************************************/
#define	BTOP_PHYST(n)					(n & 0xFFFF)
//*****************************************BTOP_NDLT register offset********************************************************/
#define	BTOP_NDLT_RELA_MODE(n)			(0 << 16)
#define	BTOP_NDLT_ABSO_MODE(n)			(1 << 16)
#define	BTOP_NDLT(n)					(n & 0xFFFF)
//*****************************************BTOP_PDLT register offset********************************************************/
#define	BTOP_PDLT_RELA_MODE(n)			(0 << 16)
#define	BTOP_PDLT_ABSO_MODE(n)			(1 << 16)
#define	BTOP_PDLT(n)					(n & 0xFFFF)
//*****************************************BBOT_PDLT register offset********************************************************/
#define	BBOT_PDLT_RELA_MODE(n)			(0 << 16)
#define	BBOT_PDLT_ABSO_MODE(n)			(1 << 16)
#define	BBOT_PDLT(n)					(n & 0xFFFF)
//*****************************************BBOT_PDLT register offset********************************************************/
#define	BBOT_NDLT_RELA_MODE(n)			(0 << 16)
#define	BBOT_NDLT_ABSO_MODE(n)			(1 << 16)
#define	BBOT_NDLT(n)					(n & 0xFFFF)
//*****************************************BBOT_HYST1 register offset********************************************************/
#define	BBOT_HYST1(n)					(n & 0xFFFF)
//*****************************************BBOT_HYST2 register offset********************************************************/
#define	BBOT_HYST2(n)					(n & 0xFFFF)
//*****************************************BBOT_LOCKCNT register offset********************************************************/
#define	BBOT_LOCKCNT(n)					(n & 0xFFFF)
//*****************************************BBOT_ADDDLT register offset********************************************************/
#define	BBOT_ADDDLT(n)					(n & 0xFFFF)
//*****************************************BBOT_SUBDLT register offset********************************************************/
#define	BBOT_SUBDLT(n)					(n & 0xFFFF)
//*****************************************BBOT_LKRLS register offset********************************************************/
#define	BBOT_LKRLS(n)					(n & 0xFFF)
//*****************************************PRESSDNCFG register offset********************************************************/
#define	PRESSDNCFG(n)					(n & 0xFFF)
//*****************************************KEYBUSY_VTH register offset********************************************************/
#define	KEYBUSY_VTH(n)					(n & 0xFFF)
//*****************************************START register offset********************************************************/
#define	HASP_START						(1)
#define	HASP_STOP						(0)
//*****************************************INTEN register offset**********************************************************/
#define HASP_ALL_INT					(0x1f)
#define KEY_BUSY_INT					(0x1 << 4)
#define DATA_ERR_INT					(0x1 << 3)
#define CRC_ERR_INT						(0x1 << 2)
#define TRANS_END_INT					(0x1 << 1)
#define ONCE_TRANS_END_INT				(0x1 << 0)
//*****************************************INTST register offset**********************************************************/
#define HASP_INT_ALL_MASK				(0x1f)
#define KEY_BUSY_INT_MASK				(0x1 << 4)
#define DATA_ERR_INT_MASK				(0x1 << 3)
#define CRC_ERR_INT_MASK				(0x1 << 2)
#define TRANS_END_INT_MASK				(0x1 << 1)
#define ONCE_TRANS_END_INT_MASK			(0x1 << 0)
//*****************************************SPECXTP_SCALE register offset**********************************************************/
#define SPECXTP_SCALE(n)				(n & 0x3FF)
//*****************************************SPECXTP_STEP register offset**********************************************************/
#define SPECXTP_RATE(n)					((n & 0xFFE) << 8)
#define SPECXTP_STEP(n)					(n & 0x1F)
//*****************************************clock INIT register offset**********************************************************/
#define INNERCLK_INIT_VAL(n)			(n & 0x7ff)
//*****************************************clock CTRL register offset**********************************************************/
#define SCK_SRC_SYSCLK					(1 << 4)
#define SCK_SRC_INNERCLK				(0 << 4)
#define INNER_CLK_ENABLE				(1 << 3)
#define INNER_CLK_DISABLE				(0 << 3)
#define INNER_CLK_BAND(n)				(n & 7)

enum HASP_TD
{
	HASP_TD1 = 1,
	HASP_TD2 = 2,
	HASP_TD3 = 3,
	HASP_TD5 = 4,
	HASP_TD6 = 5,
	HASP_TD7 = 6,
	HASP_TD9 = 7,
	HASP_TD10 = 8,
	HASP_TD11 = 9,
	HASP_TD13 = 10,
	HASP_TD14 = 11,
	HASP_TD15 = 12,
	HASP_TD17 = 13,
};

/**
 * \brief     start HASP work
 *
 * \param[in] None
 * \return    None
 */
static inline void hasp_drv_start_work(void)
{
	GTRY_HASP->INTST |= HASP_INT_ALL_MASK;

	GTRY_HASP->START = HASP_START;

	while(!(GTRY_HASP->INTST & ONCE_TRANS_END_INT_MASK));
//	while(!(GTRY_HASP->INTST & TRANS_END_INT_MASK));
}

/**
 * \brief     start HASP work
 *
 * \param[in] None
 * \return    None
 */
static inline void hasp_drv_start_work_not_wait(void)
{
	GTRY_HASP->INTST |= HASP_INT_ALL_MASK;

	GTRY_HASP->START = HASP_START;
}

/**
 * \brief     Stop HASP work
 *
 * \param[in] None
 * \return    None
 */
static inline void hasp_drv_stop_work(void)
{
	GTRY_HASP->INTST &= TRANS_END_INT;
	if(GTRY_HASP->START & HASP_START)
	{
		GTRY_HASP->START =	HASP_STOP;
		while(!(GTRY_HASP->INTST & TRANS_END_INT));
	}
}

/**
 * \brief     Set HASP nsck setting
 *
 * \param[in] nsck_cfg : configuration value
 * \return 	  None
 */
static inline void hasp_drv_nsck_set(unsigned int nsck_cfg)
{
	GTRY_HASP->NSCK = nsck_cfg;
}

/**
 * \brief     Get HASP nsck setting
 *
 * \param[in] None
 * \return 	  HASP nsck setting
 */
static inline unsigned int hasp_drv_get_nsck(void)
{
	return GTRY_HASP->NSCK;
}

/**
 * \brief     Configure HASP CTRL register value
 *
 * \param[in] reg_cfg : register configuration value
 * \return 	  None
 */
static inline void hasp_drv_set_ctrl(unsigned int reg_cfg)
{
	 GTRY_HASP->CTRL = reg_cfg;
}

/**
 * \brief     Get HASP CTRL register value
 *
 * \param[in] None
 * \return 	  register value
 */
static inline unsigned int hasp_drv_get_ctrl(void)
{
	return GTRY_HASP->CTRL;
}

static inline void hasp_drv_td_set(unsigned int td_no , unsigned int td_timer)
{
	*(unsigned int *)(HASP_BASE+0x34+(td_no-1)*4) = td_timer;
}

static inline void hasp_drv_td_clear(void)
{
	GTRY_HASP->TD1 = 0;
	GTRY_HASP->TD2 = 0;
	GTRY_HASP->TD3 = 0;
	GTRY_HASP->TD5 = 0;
	GTRY_HASP->TD6 = 0;
	GTRY_HASP->TD7 = 0;
}
extern void hasp_drv_td_set(unsigned int td_no , unsigned int td_timer);

extern void hasp_drv_td_init(void);

extern void hasp_drv_int_init(p_hasp_int_cb int_callback);

extern void hasp_drv_irq_handler(void);

extern void hasp_drv_int_callback(uint32_t int_state);

extern void hasp_drv_clk_init(void);

#endif /* SRC_H6_HASP_DRV_H_ */
