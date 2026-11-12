/*
 *	gtry_rtc_drv.c
 *
 *	Created on: 2025.10.23
 *
 */

#include "gtry_rtc_drv.h"

p_rtc_int_cb rtc_drv_int_callback = NULL;			/** RTC中断回调函数入口 */

/**
 * @brief	RTC模块中断初始化
 * @param[in] int_mask：RTC模块中断使能位MASK，见'RTC_INT'前缀宏
 * @param[in] rtc_int_cb：RTC模块中断回调函数入口
 * @retval 无
 */
void rtc_int_init(uint32_t int_mask, p_rtc_int_cb rtc_int_cb)
{
	GTRY_RTC->RTC_CTRL = 0x0;
	while(!(GTRY_RTC->RTC_STATUS&RTC_WRITE_DONE_FLAG));
	GTRY_RTC->RTC_STATUS = RTC_INT_ALL_MASK;
	GTRY_RTC->RTC_CTRL = int_mask;
	while(!(GTRY_RTC->RTC_STATUS&RTC_WRITE_DONE_FLAG));

	__nds__plic_set_priority(IRQ_RTC_PERIOD_SOURCE, 1);
	__nds__plic_set_priority(IRQ_RTC_ALARM_SOURCE, 1);
	__nds__plic_enable_interrupt(IRQ_RTC_PERIOD_SOURCE);
	__nds__plic_enable_interrupt(IRQ_RTC_ALARM_SOURCE);

	rtc_drv_int_callback = rtc_int_cb;
}

/**
 * \brief	RTC周期中断服务函数
 * \param[in] 无
 * \return 无
 */
void rtc_period_irq_handler(void)
{
	uint32_t status = GTRY_RTC->RTC_STATUS & 0xf8;
	if(rtc_drv_int_callback != NULL)
	{
		rtc_drv_int_callback(status);
	}
	GTRY_RTC->RTC_STATUS = status;
}

/**
 * \brief	RTC闹钟中断服务函数
 * \param[in] 无
 * \return 无
 */
void rtc_alarm_irq_handler(void)
{
	uint32_t status = GTRY_RTC->RTC_STATUS & 0x4;
	GTRY_RTC->RTC_STATUS = status;
	if(rtc_drv_int_callback != NULL)
	{
		rtc_drv_int_callback(status);
	}
}

/**
 * @brief	RTC模块当前时间设置函数
 * @param[in] day：当前天数，范围0~31
 * @param[in] hour：当前小时数，范围0~23
 * @param[in] min：当前分钟数，范围0~59
 * @param[in] sec：当前秒数，范围0~59
 * @retval 无
 */
void rtc_set_time(uint32_t day, uint32_t hour, uint32_t min, uint32_t sec)
{
	while (!(GTRY_RTC->RTC_STATUS & 0x10000));
	GTRY_RTC->RTC_CNTR = ((day << RTC_TIME_DAY_POS) | (hour << RTC_TIME_HOUR_POS) | (min << RTC_TIME_MIN_POS) | (sec << RTC_TIME_SEC_POS));
}

/**
 * @brief	RTC模块闹钟时间设置函数
 * @param[in] hour：闹钟小时数，范围0~23
 * @param[in] min：闹钟分钟数，范围0~59
 * @param[in] sec：闹钟秒数，范围0~59
 * @retval 无
 */
void rtc_set_alarm(uint32_t hour, uint32_t min, uint32_t sec)
{
	while(!(GTRY_RTC->RTC_STATUS & 0x10000));
	GTRY_RTC->RTC_ALARM = ((hour << RTC_TIME_HOUR_POS) | (min << RTC_TIME_MIN_POS) | (sec << RTC_TIME_SEC_POS));
}

