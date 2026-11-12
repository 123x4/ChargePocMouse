/*
 *	gtry_tsc_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_TSC_DRV_H
#define __GTRY_TSC_DRV_H

#include "gtry_platform.h"

/** TSC按键各通道与IO口对应的宏定义 */
#define		TSC_IO0_PA29				0
#define		TSC_IO1_PA30				1
#define		TSC_IO2_PA31				2
#define		TSC_IO3_PA32				3
#define 	TSC_IO4_PA33				4
#define 	TSC_IO5_PA34				5
#define 	TSC_IO6_PA35				6
#define 	TSC_IO7_PA36				7
#define 	TSC_IO8_PA37				8
#define 	TSC_IO9_PA38				9
#define 	TSC_IO10_PA39				10
#define 	TSC_IO11_PA40				11
#define 	TSC_IO12_PA41				12
#define 	TSC_IO13_PA42				13
#define 	TSC_IO14_PA43				14
#define 	TSC_IO15_PA44				15
#define 	TSC_IO16_PA45				16
#define 	TSC_IO17_PA46				17
#define 	TSC_IO18_PA47				18
#define 	TSC_IO19_PB0				19
#define 	TSC_IO20_PB1				20
#define 	TSC_IO21_PB2				21
#define 	TSC_IO22_PB3				22
#define 	TSC_IO23_PB4				23
#define 	TSC_IO24_PB5				24
#define 	TSC_IO25_PB6				25
#define 	TSC_IO26_PB7				26
#define 	TSC_IO27_PB8				27
#define 	TSC_IO28_PB9				28
#define 	TSC_IO29_PB10				29
#define 	TSC_IO30_PB11				30
#define 	TSC_IO31_PB12				31

/** TSC按键各通道MASK与IO口对应的宏定义 */
#define 	TSC_IO0_PA29_MASK			(1UL<<0)
#define 	TSC_IO1_PA30_MASK			(1UL<<1)
#define 	TSC_IO2_PA31_MASK			(1UL<<2)
#define 	TSC_IO3_PA32_MASK			(1UL<<3)
#define 	TSC_IO4_PA33_MASK			(1UL<<4)
#define 	TSC_IO5_PA34_MASK			(1UL<<5)
#define 	TSC_IO6_PA35_MASK			(1UL<<6)
#define 	TSC_IO7_PA36_MASK			(1UL<<7)
#define 	TSC_IO8_PA37_MASK			(1UL<<8)
#define 	TSC_IO9_PA38_MASK			(1UL<<9)
#define 	TSC_IO10_PA39_MASK			(1UL<<10)
#define 	TSC_IO11_PA40_MASK			(1UL<<11)
#define 	TSC_IO12_PA41_MASK			(1UL<<12)
#define 	TSC_IO13_PA42_MASK			(1UL<<13)
#define 	TSC_IO14_PA43_MASK			(1UL<<14)
#define 	TSC_IO15_PA44_MASK			(1UL<<15)
#define 	TSC_IO16_PA45_MASK			(1UL<<16)
#define 	TSC_IO17_PA46_MASK			(1UL<<17)
#define 	TSC_IO18_PA47_MASK			(1UL<<18)
#define 	TSC_IO19_PB0_MASK			(1UL<<19)
#define 	TSC_IO20_PB1_MASK			(1UL<<20)
#define 	TSC_IO21_PB2_MASK			(1UL<<21)
#define 	TSC_IO22_PB3_MASK			(1UL<<22)
#define 	TSC_IO23_PB4_MASK			(1UL<<23)
#define 	TSC_IO24_PB5_MASK			(1UL<<24)
#define 	TSC_IO25_PB6_MASK			(1UL<<25)
#define 	TSC_IO26_PB7_MASK			(1UL<<26)
#define 	TSC_IO27_PB8_MASK			(1UL<<27)
#define 	TSC_IO28_PB9_MASK			(1UL<<28)
#define 	TSC_IO29_PB10_MASK			(1UL<<29)
#define 	TSC_IO30_PB11_MASK			(1UL<<30)
#define 	TSC_IO31_PB12_MASK			(1UL<<31)

#define		TSC_IO_ALL_MASK				(0xffffffffUL)

//************  TSC_INT_CTRL  **************//
#define		RY_TSC_INT_CLEAR			(1UL<<2)
#define		RY_TSC_CVT_COMPLETE			(1UL<<1)
#define		RY_TSC_INT_EN				(1UL<<0)

#define 	TSC_CFG_SETUP_TIME_POS 		(20UL) 			/** 配置寄存器建立时间偏移位 */
#define 	TSC_CFG_CAP_POS			 	(16UL) 			/** 配置寄存器内部参考振荡器电容偏移位 */
#define 	TSC_CFG_COMP_IB_POS 		(12UL)			/** 配置寄存器比较器电流偏移位 */
#define	 	TSC_CFG_TSC_ICHAR1_POS 		(8UL)			/** 配置寄存器充电电流偏移位 */
#define 	TSC_CFG_TSC_ICHAR2_POS 		(4UL)			/** 配置寄存器偏置电流偏移位 */
#define 	TSC_CFG_CNT2_SEL_POS 		(3UL)			/** 配置寄存器CNT2值选择偏移位 */
#define 	TSC_CFG_CLK_SEL_POS 		(2UL)			/** 配置寄存器时钟源选择偏移位 */
#define 	TSC_CFG_INTER_REG_POS 		(1UL)			/** 配置寄存器内部参考振荡器使能偏移位 */
#define 	TSC_CFG_EN_POS 				(0UL)			/** 配置寄存器TSC使能偏移位 */


#define 	TSC_ENABLE_MASK				(1UL)			/** TSC模块使能 */

/**
 * \brief CNT2_CH值选择
 */
typedef enum
{
	RY_TSC_CNT2V			=  0,		/**< TSC_CNT2_CH值为TSC_CNT2 */
	RY_TSC_CNT2_CNT1V		=  1,		/**< TSC_CNT2_CH值为TSC_CNT2-TSC_CNT1 */
} tsc_cnt2_sel_t;

/**
 * \brief TSC时钟源选择
 */
typedef enum
{
	RY_TSC_HCLK				=  0,		/**< 主时钟 */
	RY_TSC_ILO			=  1,		/**< RTC低频时钟 */
} tsc_clk_t;

/**
 * \brief TSC内部参考振荡器使能控制
 */
typedef enum
{
	RY_TSC_REF_DISEN		=  0,		/**< 内部参考振荡器禁止 */
	RY_TSC_REF_EN			=  1,		/**< 内部参考振荡器使能 */
} tsc_ref_t;


/**
 * @brief	TSC模块设置转换通道MASK
 * @param[in] mask：TSC按键各组与IO口对应的宏定义，如'TSC_IO0_PA29_MASK'宏定义
 * @retval 无
 */
static inline void tsc_set_ch_mask(uint32_t ch_mask)
{
	GTRY_TSC->TSC_CHL_EN = ch_mask;
}

/**
 * @brief	获取TSC模块采样电容通道MASK
 * @param[in] 无
 * @retval 返回转换通道寄存器值
 */
static inline uint32_t tsc_get_ch_gio(void)
{
	return GTRY_TSC->TSC_CHL_EN;
}

/**
 * @brief	TSC模块转换是否完成
 * @param[in] 无
 * @retval 0表示转换完成
 * @retval 非0表示正在转换
 */
static inline uint32_t tsc_get_start_done(void)
{
	return GTRY_TSC->TSC_START;
}

/**
 * @brief	获取TSC模块通道的转化计数值
 * @param[in] ch：需要获取的通道标识，如'TSC_IO0_PA29'宏定义
 * @retval 返回转化计数器值
 */
static inline uint32_t tsc_get_cnt(uint32_t ch)
{
	return (GTRY_TSC->TSC_CNT_CHL[ch]&0x3fff);
}

/**
 * @brief	获取TSC模块通道的参考计数值
 * @param[in] ch：需要获取的通道标识，如'TSC_IO0_PA29'宏定义
 * @retval 返回转化计数器值
 */
static inline uint32_t tsc_get_ref_cnt(uint32_t ch)
{
	return ((GTRY_TSC->TSC_CNT_CHL[ch]>>16)&0x3fff);
}

/**
 * @brief	TSC模块清除中断状态
 * @param[in] 无
 * @retval 无
 */
static inline void tsc_clr_int(void)
{
	GTRY_TSC->TSC_INT_CTRL |= RY_TSC_INT_CLEAR;
}

/**
 * @brief	TSC模块转换开始
 * @param[in] 无
 * @retval 无
 */
static inline void tsc_start(void)
{
	GTRY_TSC->TSC_START = 0x1;
}

/**
 * @brief	TSC模块中断回调函数指针
 * @param[in] 无
 * @retval 无
 */
typedef void (*p_tsc_int_cb)(void);

/**
 * @brief	TSC模块中断初始化函数
 * @param[in] tsc_int_cb：中断回调函数入口
 * \return 无
 */
extern void tsc_int_init(p_tsc_int_cb tsc_int_cb);

/**
 * @brief	TSC模块初始化函数
 * @param[in] setup_time：建立时间 = (setup_time+1)*32 TSC_CLK周期
 * @param[in] trans_time：转换时间 = trans_time*TSC_CLK周期
 * @param[in] cnt2_sel：CNT2_CH值选择 \ref tsc_cnt2_sel_t
 * @param[in] tsc_clk_sel：TSC时钟源选择 \ref tsc_clk_t
 * @param[in] tsc_ref_en：内部参考振荡器使能控制 \ref tsc_ref_t
 * @retval 无
 */
extern void tsc_init(uint32_t setup_time, uint32_t trans_time, tsc_cnt2_sel_t cnt2_sel, tsc_clk_t tsc_clk_sel , tsc_ref_t tsc_ref_en);

#endif	/* __GTRY_TSC_DRV_H */
