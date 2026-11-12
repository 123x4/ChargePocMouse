/*
 *	gtry_ltimer_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_LTIMER_DRV_H__
#define __GTRY_LTIMER_DRV_H__

#include "gtry_platform.h"

#define		LTIMER_NUMBER0					0					/** ltimer0模块标识号 */
#define		LTIMER_NUMBER1					1					/** ltimer1模块标识号 */

#define 	LTIMER_CC0						0					/** ltimer ID0标识号 */
#define 	LTIMER_CC1						1					/** ltimer ID1标识号 */
#define 	LTIMER_CC2						2					/** ltimer ID2标识号 */
#define 	LTIMER_CC3						3					/** ltimer ID3标识号 */
#define 	LTIMER_CC_NUM					4					/** ltimer ID数目 */

#define 	LTIMER_INT_NULL_MASK			(0x0)				/** ltimer模块无中断MASK */
#define 	LTIMER_INT_TICK_MASK			(0x1UL<<0)			/** ltimer模块TICK中断MASK */
#define 	LTIMER_INT_OVRFLW_MASK  		(0x1UL<<1)			/** ltimer模块溢出中断MASK */
#define 	LTIMER_INT_CMP0_MASK			(0x1UL<<16)			/** ltimer模块比较器0中断MASK */
#define 	LTIMER_INT_CMP1_MASK			(0x1UL<<17)			/** ltimer模块比较器1中断MASK */
#define 	LTIMER_INT_CMP2_MASK			(0x1UL<<18)			/** ltimer模块比较器2中断MASK */
#define 	LTIMER_INT_CMP3_MASK			(0x1UL<<19)			/** ltimer模块比较器3中断MASK */
#define 	LTIMER_INT_ALL_MASK				(0xF0003)			/** ltimer模块所有中断MASK */

#define 	LTIMER_BIT_WIDE					24					/** ltimer模块计数位宽 */

#define 	TICK_ENABLE						(1)					/** ltimer模块TICK使能 */
#define 	TICK_DISABLE					(0)					/** ltimer模块TICK关闭 */

/**
 * @brief     ltimer模块开始运行
 * @param[in] ltimer_no：ltimer模块识别号
 * @retval    无
 */
static inline void ltimer_start(uint32_t ltimer_no)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;
	LTIMER->LTIM_START = 0x1;
}

/**
 * @brief	LTIMER触发溢出测试
 * @param[in] ltimer_no：ltimer模块识别号
 * @retval 无
 */
static inline void ltimer_trig_ovrflw(uint32_t ltimer_no)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;
	LTIMER->LTIM_TRIG_OVRFLW = 0x1;
}

/**
 * @brief	LTIMER获取当前计数值
 * @param[in] ltimer_no：ltimer模块识别号
 * @retval 返回当前计数值
 */
static inline uint32_t ltimer_get_count(uint32_t ltimer_no)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;
	return LTIMER->LTIM_COUNT;
}

/**
 * @brief	LTIMER中断使能
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] int_mask：要使能的中断
 * @retval 无
 */
static inline void ltimer_int_set(uint32_t ltimer_no, uint32_t int_mask)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;
	LTIMER->LTIM_INT_EN_SET = int_mask;
}

/**
 * @brief	LTIMER清除中断使能
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] int_mask：要清除的中断
 * @retval 无
 */
static inline void ltimer_int_clear(uint32_t ltimer_no, uint32_t int_mask)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;
	LTIMER->LTIM_INT_EN_CLR = int_mask;
}

/**
 * @brief	LTIMER获取中断使能状态
 * @param[in] ltimer_no：ltimer模块识别号
 * @retval 返回中断使能状态
 */
static inline uint32_t ltimer_get_inten(uint32_t ltimer_no)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;
	return LTIMER->LTIM_INT_EN;
}

/**
 * @brief	LTIMER模块中断回调函数指针
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] intst：ltimer中断状态
 * @retval 无
 */
typedef void (*p_ltimer_int_cb) (uint32_t ltimer_no, uint32_t intst);

/**
 * @brief	LTIMER模块配置
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] prescaler：时钟分频系数
 * @param[in] ticken：tick使能，0表示不使能，非0表示使能
 * @retval 无
 */
extern void ltimer_cfg(uint32_t ltimer_no, uint32_t prescaler, uint8_t ticken);

/**
 * @brief	LTIMER模块停止运行，计数值复位
 * @param[in] ltimer_no：ltimer模块识别号
 * @retval 无
 */
extern void ltimer_stop(uint32_t ltimer_no);

/**
 * @brief	LTIMER模块清除运行状态，计数值复位
 * @param[in] ltimer_no：ltimer模块识别号
 * @retval 无
 */
extern void ltimer_clear(uint32_t ltimer_no);

/**
 * @brief	LTIMER模块设置指定ID的CC值
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] id：ltimer模块ID识别号
 * @param[in] val：待设置的CC值，最大24bit，为0不产生比较事件
 * @retval 无
 */
extern void ltimer_set_ccn(uint32_t ltimer_no, uint32_t id, uint32_t val);

/**
 * @brief	LTIMER模块获取指定ID的CC值
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] id：ltimer模块ID识别号
 * @retval 返回已设置ID的设置的CC值
 */
extern uint32_t ltimer_get_ccn(uint32_t ltimer_no, uint32_t id);

/**
 * @brief	LTIMER模块中断初始化
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] int_mask：ltimer模块中断使能位MASK
 * @param[in] ltimer_int_cb：ltimer模块中断回调函数入口
 * @retval 无
 */
extern void ltimer_int_init(uint32_t ltimer_no, uint32_t int_mask, p_ltimer_int_cb ltimer_int_cb);

/**
 * @brief	获取LTIMER模块事件状态
 * @param[in] ltimer_no：ltimer模块识别号
 * @retval 返回事件状态，见'LTIMER_INT'前缀宏
 */
extern uint32_t ltimer_get_event(uint32_t ltimer_no);

/**
 * @brief	清除LTIMER模块事件状态
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] event：待清除事件状态，见'LTIMER_INT'前缀宏
 * @retval 无
 */
extern void ltimer_clear_event(uint32_t ltimer_no, uint32_t event);

#endif	/* __GTRY_LTIMER_DRV_H__ */
