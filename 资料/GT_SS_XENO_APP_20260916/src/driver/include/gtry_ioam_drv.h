/*
 *	gtry_ioam_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_IOAM_DRV_H__
#define __GTRY_IOAM_DRV_H__

#include "gtry_platform.h"

#define		IOAM_SAMPLE_NUM_MAX  		24					/** IOAM最大数据采样个数，采样超过该个数值后会停止采样 */

#define		IOAM_INT_EN_BIT  			0x1 				/** IOAM中断使能 */

#define		IOAM_SAMPLE_PERIOD_MAX  	((0x1<<8)-1) 		/** IOAM采样间隔最大周期数，以ILO周期为单位 */

#define		IOAM_CHL0					(0)					/** IOAM通道 0 标识 */
#define		IOAM_CHL1					(1)					/** IOAM通道 1 标识 */
#define		IOAM_CHL2					(2)					/** IOAM通道 2 标识 */
#define		IOAM_CHL3					(3)					/** IOAM通道 3 标识 */
#define		IOAM_CHL4					(4)					/** IOAM通道 4 标识 */
#define		IOAM_CHL5					(5)					/** IOAM通道 5 标识 */
#define		IOAM_CHL6					(6)					/** IOAM通道 6 标识 */
#define		IOAM_CHL7					(7)					/** IOAM通道 7 标识 */
#define		IOAM_CHL8					(8)					/** IOAM通道 8 标识 */
#define		IOAM_CHL9					(9)					/** IOAM通道 9 标识 */

#define		IOAM_CHL0_MASK				(0x1<<0)			/** IOAM通道 0 标识MASK */
#define		IOAM_CHL1_MASK				(0x1<<1)			/** IOAM通道 1 标识MASK */
#define		IOAM_CHL2_MASK				(0x1<<2)			/** IOAM通道 2 标识MASK */
#define		IOAM_CHL3_MASK				(0x1<<3)			/** IOAM通道 3 标识MASK */
#define		IOAM_CHL4_MASK				(0x1<<4)			/** IOAM通道 4 标识MASK */
#define		IOAM_CHL5_MASK				(0x1<<5)			/** IOAM通道 5 标识MASK */
#define		IOAM_CHL6_MASK				(0x1<<6)			/** IOAM通道 6 标识MASK */
#define		IOAM_CHL7_MASK				(0x1<<7)			/** IOAM通道 7 标识MASK */
#define		IOAM_CHL8_MASK				(0x1<<8)			/** IOAM通道 8 标识MASK */
#define		IOAM_CHL9_MASK				(0x1<<9)			/** IOAM通道 9 标识MASK */

#define		IOAM_CH0_PA0				PA0					/** IOAM通道0_PA0 */
#define		IOAM_CH0_PA10				PA10				/** IOAM通道0_PA10 */
#define		IOAM_CH0_PA20				PA20				/** IOAM通道0_PA20 */
#define		IOAM_CH0_PA30				PA30				/** IOAM通道0_PA30 */
#define		IOAM_CH0_PA40				PA40				/** IOAM通道0_PA40 */
#define		IOAM_CH0_PB2				PB2					/** IOAM通道0_PB2 */
#define		IOAM_CH0_PB12				PB12				/** IOAM通道0_PB12 */

#define		IOAM_CH1_PA1				PA1					/** IOAM通道1_PA1 */
#define		IOAM_CH1_PA11				PA11				/** IOAM通道1_PA11 */
#define		IOAM_CH1_PA21				PA21				/** IOAM通道1_PA21 */
#define		IOAM_CH1_PA31				PA31				/** IOAM通道1_PA31 */
#define		IOAM_CH1_PA41				PA41				/** IOAM通道1_PA41 */
#define		IOAM_CH1_PB3				PB3					/** IOAM通道1_PB3 */
#define		IOAM_CH1_PB13				PB13				/** IOAM通道1_PB13 */

#define		IOAM_CH2_PA2				PA2					/** IOAM通道2_PA2 */
#define		IOAM_CH2_PA12				PA12				/** IOAM通道2_PA12 */
#define		IOAM_CH2_PA22				PA22				/** IOAM通道2_PA22 */
#define		IOAM_CH2_PA32				PA32				/** IOAM通道2_PA32 */
#define		IOAM_CH2_PA42				PA42				/** IOAM通道2_PA42 */
#define		IOAM_CH2_PB4				PB4					/** IOAM通道2_PB4 */
#define		IOAM_CH2_PB14				PB14				/** IOAM通道2_PB14 */

#define		IOAM_CH3_PA3				PA3					/** IOAM通道3_PA3 */
#define		IOAM_CH3_PA13				PA13				/** IOAM通道3_PA13 */
#define		IOAM_CH3_PA23				PA23				/** IOAM通道3_PA23 */
#define		IOAM_CH3_PA33				PA33				/** IOAM通道3_PA33 */
#define		IOAM_CH3_PA43				PA43				/** IOAM通道3_PA43 */
#define		IOAM_CH3_PB5				PB5					/** IOAM通道3_PB5 */
#define		IOAM_CH3_PB15				PB15				/** IOAM通道3_PB15 */

#define		IOAM_CH4_PA4				PA4					/** IOAM通道4_PA4 */
#define		IOAM_CH4_PA14				PA14				/** IOAM通道4_PA14 */
#define		IOAM_CH4_PA24				PA24				/** IOAM通道4_PA24 */
#define		IOAM_CH4_PA34				PA34				/** IOAM通道4_PA34 */
#define		IOAM_CH4_PA44				PA44				/** IOAM通道4_PA44 */
#define		IOAM_CH4_PB6				PB6					/** IOAM通道4_PB6 */

#define		IOAM_CH5_PA5				PA5					/** IOAM通道5_PA5 */
#define		IOAM_CH5_PA15				PA15				/** IOAM通道5_PA15 */
#define		IOAM_CH5_PA25				PA25				/** IOAM通道5_PA25 */
#define		IOAM_CH5_PA35				PA35				/** IOAM通道5_PA35 */
#define		IOAM_CH5_PA45				PA45				/** IOAM通道5_PA45 */
#define		IOAM_CH5_PB7				PB7					/** IOAM通道5_PB7 */

#define		IOAM_CH6_PA6				PA6					/** IOAM通道6_PA6 */
#define		IOAM_CH6_PA16				PA16				/** IOAM通道6_PA16 */
#define		IOAM_CH6_PA26				PA26				/** IOAM通道6_PA26 */
#define		IOAM_CH6_PA36				PA36				/** IOAM通道6_PA36 */
#define		IOAM_CH6_PA46				PA46				/** IOAM通道6_PA46 */
#define		IOAM_CH6_PB8				PB8					/** IOAM通道6_PB8 */

#define		IOAM_CH7_PA7				PA7					/** IOAM通道7_PA7 */
#define		IOAM_CH7_PA17				PA17				/** IOAM通道7_PA17 */
#define		IOAM_CH7_PA27				PA27				/** IOAM通道7_PA27 */
#define		IOAM_CH7_PA37				PA37				/** IOAM通道7_PA37 */
#define		IOAM_CH7_PA47				PA47				/** IOAM通道7_PA47 */
#define		IOAM_CH7_PB9				PB9					/** IOAM通道7_PB9 */

#define		IOAM_CH8_PA8				PA8					/** IOAM通道8_PA8 */
#define		IOAM_CH8_PA18				PA18				/** IOAM通道8_PA18 */
#define		IOAM_CH8_PA28				PA28				/** IOAM通道8_PA28 */
#define		IOAM_CH8_PA38				PA38				/** IOAM通道8_PA38 */
#define		IOAM_CH8_PB0				PB0					/** IOAM通道8_PB0 */
#define		IOAM_CH8_PB10				PB10				/** IOAM通道8_PB10 */

#define		IOAM_CH9_PA9				PA9					/** IOAM通道9_PA9 */
#define		IOAM_CH9_PA19				PA19				/** IOAM通道9_PA19 */
#define		IOAM_CH9_PA29				PA29				/** IOAM通道9_PA29 */
#define		IOAM_CH9_PA39				PA39				/** IOAM通道9_PA39 */
#define		IOAM_CH9_PB1				PB1					/** IOAM通道9_PB1 */
#define		IOAM_CH9_PB11				PB11				/** IOAM通道9_PB11 */

/**
 * @brief	IOAM中断回调函数指针
 * @param[in] 无
 * @retval 无
 */
typedef void (*p_ioam_int_cb)(void);

/**
 * @brief	IOAM模块中断初始化
 * @param[in] int_sample_num：采样数据个数阈值，当采样个数等于该值时触发中断
 * @param[in] ioam_callback：ioam模块中断回调函数入口，详见‘p_ioam_int_cb’
 * @retval 无
 */
extern void ioam_int_init(uint32_t int_sample_num, p_ioam_int_cb ioam_callback);

/**
 * @brief	IOAM模块初始化
 * @param[in] chl_mask：采样通道标识MASK
 * @param[in] samperiod：采样通道采样间隔周期数，参数范围0~255
 * @retval 无
 */
extern void ioam_init(uint32_t chl_mask, uint32_t samperiod);

/**
 * @brief	IOAM停止该模块并关闭该模块电源
 * @param[in] 无
 * @retval 无
 */
extern void ioam_uninit(void);

/**
 * @brief	获取IOAM模块已采样数据个数
 * @param[in] 无
 * @retval 返回已采样数据个数，最大为IOAM_SAMPLE_NUM_MAX，如果返回值超过IOAM_SAMPLE_NUM_MAX，则也只IOAM_SAMPLE_NUM_MAX个数据有效
 */
extern uint32_t ioam_get_sampnum(void);

/**
 * @brief	获取IOAM模块已开启采样的通道
 * @param[in] 无
 * @retval 返回已开启采样的通道，可用IOAM通道标识MASK进行区别
 */
extern uint32_t ioam_get_active_chl(void);

/**
 * @brief	获取IOAM模块当前通道的采样数据
 * @param[in] chl_num：采样通道标识
 * @retval 返回当前通道的采样数据
 */
extern uint32_t ioam_get_chl_val(uint32_t chl_num);


/**
 * @brief	IOAM指定通道开启采样
 * @param[in] chl_mask：采样通道标识MASK
 * @retval 无
 */
extern void ioam_start(uint32_t chl_mask);

/**
 * @brief	IOAM所有通道停止采样
 * @param[in] 无
 * @retval 无
 */
extern void ioam_stop(void);

#endif	/* __GTRY_IOAM_DRV_H__ */
