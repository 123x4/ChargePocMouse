/*
 *	gtry_wdt_drv.c
 *
 *	Created on: 2025.10.23
 *
 */

#include "gtry_wdt_drv.h"

/**
 * @brief	WDT配置
 * @param[in] expired_time：过期时间配置 \ref wdt_expired_time_t
 * @param[in] rst_time：复位时间配置 \ref wdt_rst_time_t
 * @param[in] rst_enable：复位使能配置，'0'不使能复位，'1'使能复位
 * @retval 无
 */
void wdt_cfg(wdt_expired_time_t expired_time, wdt_rst_time_t rst_time, uint32_t rst_enable)
{
	GTRY_WDT->WDT_WR_EN = WDT_WR_EN_NUM;
	GTRY_WDT->WDT_RESTART = WDT_RESTART_NUM;
	GTRY_WDT->WDT_STATUS = 0x1;
	GTRY_WDT->WDT_WR_EN = WDT_WR_EN_NUM;
	GTRY_WDT->WDT_CTRL = (rst_time<<WDT_CFG_RST_TIME_POS) | (expired_time<<WDT_CFG_EXPIRED_TIME_POS)
							| (rst_enable? WDT_RST_EN: 0);
}

/**
 * @brief	WDT模块使能
 * @param[in] 无
 * @retval 无
 */
void wdt_enable(void)
{
	uint32_t ctrl = GTRY_WDT->WDT_CTRL;
	ctrl |= WDT_ENABLE;
	GTRY_WDT->WDT_WR_EN = WDT_WR_EN_NUM;
	GTRY_WDT->WDT_CTRL = ctrl;
}

/**
 * @brief	WDT模块失能
 * @param[in] 无
 * @retval 无
 */
void wdt_disable(void)
{
	uint32_t ctrl = GTRY_WDT->WDT_CTRL;
	ctrl &= ~WDT_ENABLE;
	GTRY_WDT->WDT_WR_EN = WDT_WR_EN_NUM;
	GTRY_WDT->WDT_CTRL = ctrl;
}

/**
 * @brief	WDT重置计数
 * @param[in] 无
 * @retval 无
 */
void wdt_reset_timer(void)
{
	GTRY_WDT->WDT_WR_EN = WDT_WR_EN_NUM;
	GTRY_WDT->WDT_RESTART = WDT_RESTART_NUM;
}
