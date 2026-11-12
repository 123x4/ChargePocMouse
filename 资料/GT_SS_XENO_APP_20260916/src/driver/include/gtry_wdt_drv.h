/*
 *	gtry_wdt_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_WDT_DRV_H
#define __GTRY_WDT_DRV_H

#include "gtry_platform.h"

#define		WDT_RESTART_NUM				0xcafe
#define		WDT_WR_EN_NUM				0x5aa5

/* CTRL Register (+0x10) */
#define		WDT_CFG_RST_TIME_POS 		(8UL) 		/** RST_TIME配置寄存器偏移位 */
#define		WDT_CFG_EXPIRED_TIME_POS 	(4UL) 		/** EXPIRED_TIME配置寄存器偏移位 */
#define		WDT_CFG_RST_EN_POS 			(3UL)		/** RST_EN配置寄存器偏移位 */
#define		WDT_CFG_ENABLE_POS 			(0UL)		/** WDT_ENABLE配置寄存器偏移位 */

/**
 * \brief RST_TIME配置
 */
typedef enum
{
	RY_WDT_RST_TIME_3P9ms			=  0,		/**< 128 ilo periods, 3.906ms */
	RY_WDT_RST_TIME_7P8ms			=  1,		/**< 256 ilo periods, 7.812ms */
	RY_WDT_RST_TIME_15P6ms			=  2,		/**< 512 ilo periods, 15.625ms */
	RY_WDT_RST_TIME_31P2ms			=  3,		/**< 1024 ilo periods, 31.25ms */
	RY_WDT_RST_TIME_62P5ms			=  4,		/**< 2048 ilo periods, 62.5ms */
	RY_WDT_RST_TIME_125ms			=  5,		/**< 4096 ilo periods, 125ms */
	RY_WDT_RST_TIME_250ms			=  6,		/**< 8192 ilo periods, 250ms */
	RY_WDT_RST_TIME_500ms			=  7,		/**< 16384 ilo periods, 500ms */
} wdt_rst_time_t;

/**
 * \brief EXPIRED_TIME配置
 */
typedef enum
{
	RY_WDT_EXPIRED_TIME_1P9ms		=  0,		/**< 64 ilo periods, 1.953ms */
	RY_WDT_EXPIRED_TIME_7P8ms		=  1,		/**< 256 ilo periods, 7.812ms */
	RY_WDT_EXPIRED_TIME_31P2ms		=  2,		/**< 1024 ilo periods, 31.25ms */
	RY_WDT_EXPIRED_TIME_62P5ms		=  3,		/**< 2048 ilo periods, 62.5ms */
	RY_WDT_EXPIRED_TIME_125ms		=  4,		/**< 4096 ilo periods, 125ms */
	RY_WDT_EXPIRED_TIME_250ms		=  5,		/**< 8192 ilo periods, 250ms */
	RY_WDT_EXPIRED_TIME_500ms		=  6,		/**< 16384 ilo periods, 500ms */
	RY_WDT_EXPIRED_TIME_1s			=  7,		/**< 32768 ilo periods, 1s */
} wdt_expired_time_t;

#define		WDT_RST_EN				(0x1ul<<WDT_CFG_RST_EN_POS)		/** WDT模块复位使能 */
#define		WDT_ENABLE				(0x1ul<<WDT_CFG_ENABLE_POS)		/** WDT模块使能 */

/**
 * @brief	WDT配置
 * @param[in] expired_time：过期时间配置 \ref wdt_expired_time_t
 * @param[in] rst_time：复位时间配置 \ref wdt_rst_time_t
 * @param[in] rst_enable：复位使能配置，'0'不使能复位，'1'使能复位
 * @retval 无
 */
extern void wdt_cfg(wdt_expired_time_t expired_time, wdt_rst_time_t rst_time, uint32_t rst_enable);

/**
 * @brief	WDT模块使能
 * @param[in] 无
 * @retval 无
 */
extern void wdt_enable(void);

/**
 * @brief	WDT模块失能
 * @param[in] 无
 * @retval 无
 */
extern void wdt_disable(void);

/**
 * @brief	WDT重置计数
 * @param[in] 无
 * @retval 无
 */
extern void wdt_reset_timer(void);

/**
 * @brief	WDT获取计时器过期状态
 * @param[in] 无
 * @retval WDT计时器过期状态，'0'未过期，'1'过期
 */
static inline uint32_t wdt_get_expired_status(void)
{
	return GTRY_WDT->WDT_STATUS;
}

/**
 * @brief	WDT清除计时器过期状态
 * @param[in] 无
 * @retval 无
 */
static inline void wdt_clr_expired_status(void)
{
	GTRY_WDT->WDT_STATUS = 0x1;
}

#endif /* _RY_WDT_DRV_H */
