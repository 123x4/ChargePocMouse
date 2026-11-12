/*
 *	ry_rtc_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_RTC_DRV_H
#define __GTRY_RTC_DRV_H

#include "gtry_platform.h"

#define		RTC_WRITE_DONE_FLAG			(1UL<<16)			/** rtc模块CTRL寄存器配置完成标志 */

#define		RTC_INT_NULL_MASK			(0UL)				/** rtc模块无中断MASK */
#define		RTC_INT_HSEC_MASK			(1UL<<7)			/** rtc模块Half-second中断MASK */
#define 	RTC_INT_SEC_MASK			(1UL<<6)			/** rtc模块Second中断MASK */
#define 	RTC_INT_MIN_MASK			(1UL<<5)			/** rtc模块Min中断MASK */
#define 	RTC_INT_HOUR_MASK			(1UL<<4)			/** rtc模块Hour中断MASK */
#define 	RTC_INT_DAY_MASK			(1UL<<3)			/** rtc模块Day中断MASK */
#define 	RTC_INT_ALARM_MASK			(1UL<<2)			/** rtc模块ALARM中断MASK */
#define 	RTC_INT_ALARM_WKUP_MASK		(1UL<<1)			/** rtc模块ALARM_WKUP中断MASK */
#define 	RTC_INT_ALL_MASK			(0xfe)				/** rtc模块所有中断MASK */

#define 	RTC_TIME_DAY_POS			(17UL) 				/** rtc时间配置寄存器DAY偏移位 */
#define 	RTC_TIME_HOUR_POS			(12UL) 				/** rtc时间配置寄存器HOUR偏移位 */
#define 	RTC_TIME_MIN_POS			(6UL) 				/** rtc时间配置寄存器MIN偏移位 */
#define 	RTC_TIME_SEC_POS			(0UL) 				/** rtc时间配置寄存器SEC偏移位 */

#define 	RTC_ENABLE_MASK 			(0x1) 				/** rtc使能 */

/**
 * \brief	开启RTC定时
 * \param[in] 无
 * \return 无
 */
static inline void rtc_start(void)
{
	GTRY_RTC->RTC_CTRL |= RTC_ENABLE_MASK;
	while(!(GTRY_RTC->RTC_STATUS&RTC_WRITE_DONE_FLAG));
}

/**
 * \brief	停止RTC定时
 * \param[in] 无
 * \return 无
 */
static inline void rtc_stop(void)
{
	GTRY_RTC->RTC_CTRL &= ~RTC_ENABLE_MASK;
	while(!(GTRY_RTC->RTC_STATUS&RTC_WRITE_DONE_FLAG));
}

/**
 * @brief	RTC模块中断回调函数指针
 * @param[in] intst：rtc中断状态
 * @retval 无
 */
typedef void (*p_rtc_int_cb) (uint32_t intst);

/**
 * @brief	RTC模块中断初始化
 * @param[in] int_mask：rtc模块中断使能位MASK，见'RTC_INT'前缀宏
 * @param[in] rtc_int_cb：rtc模块中断回调函数入口
 * @retval 无
 */
extern void rtc_int_init(uint32_t int_mask, p_rtc_int_cb rtc_int_cb);

/**
 * @brief	RTC模块当前时间设置函数
 * @param[in] day：当前天数，范围0~31
 * @param[in] hour：当前小时数，范围0~23
 * @param[in] min：当前分钟数，范围0~59
 * @param[in] sec：当前秒数，范围0~59
 * @retval 无
 */
extern void rtc_set_time(uint32_t day, uint32_t hour, uint32_t min, uint32_t sec);

/**
 * @brief	RTC模块闹钟时间设置函数
 * @param[in] hour：闹钟小时数，范围0~23
 * @param[in] min：闹钟分钟数，范围0~59
 * @param[in] sec：闹钟秒数，范围0~59
 * @retval 无
 */
extern void rtc_set_alarm(uint32_t hour, uint32_t min, uint32_t sec);

#endif /* __GTRY_RTC_DRV_H */
