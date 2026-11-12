/*
 *	gtry_pwm_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_PWM_DRV_H_
#define __GTRY_PWM_DRV_H_

#include "gtry_platform.h"

#define 	PWM_GROUP0  				0									/** PWM第0组标识 */
#define 	PWM_GROUP1  				1									/** PWM第1组标识 */
#define 	PWM_GROUP2  				2									/** PWM第2组标识 */
#define 	PWM_GROUP3  				3									/** PWM第3组标识 */
#define 	PWM_GROUP_MASK 				0x18								/** PWM第n组标识MASK */
#define 	PWM_GROUP_POS 				3									/** PWM第n组标识偏移MASK */
#define 	PWM_GROUP0_MASK 			(PWM_GROUP0<<PWM_GROUP_POS)			/** PWM第0组标识MASK */
#define 	PWM_GROUP1_MASK 			(PWM_GROUP1<<PWM_GROUP_POS)			/** PWM第1组标识MASK */
#define 	PWM_GROUP2_MASK 			(PWM_GROUP2<<PWM_GROUP_POS)			/** PWM第2组标识MASK */
#define 	PWM_GROUP3_MASK 			(PWM_GROUP3<<PWM_GROUP_POS)			/** PWM第3组标识MASK */

#define 	PWM_CH_MASK 				0x7									/** PWM每组所有通道MASK */
#define 	PWM_CH_NUMBER				8									/** PWM每组最大通道个数 */

//--------------------------------------------------------------------------------
#define 	PWM_G0_CH0_PA0_PA32			(PWM_GROUP0_MASK|0x00)				/** PWM第0组第0通道标识 */
#define 	PWM_G0_CH1_PA1_PA33  		(PWM_GROUP0_MASK|0x01)				/** PWM第0组第1通道标识 */
#define 	PWM_G0_CH2_PA2_PA34   		(PWM_GROUP0_MASK|0x02)				/** PWM第0组第2通道标识 */
#define 	PWM_G0_CH3_PA3_PA35   		(PWM_GROUP0_MASK|0x03)				/** PWM第0组第3通道标识 */
#define 	PWM_G0_CH4_PA4_PA36   		(PWM_GROUP0_MASK|0x04)				/** PWM第0组第4通道标识 */
#define 	PWM_G0_CH5_PA5_PA37    		(PWM_GROUP0_MASK|0x05)				/** PWM第0组第5通道标识 */
#define 	PWM_G0_CH6_PA6_PA38   		(PWM_GROUP0_MASK|0x06)				/** PWM第0组第6通道标识 */
#define 	PWM_G0_CH7_PA7_PA39			(PWM_GROUP0_MASK|0x07)				/** PWM第0组第7通道标识 */
//--------------------------------------------------------------------------------
#define 	PWM_G1_CH0_PA8_PA40			(PWM_GROUP1_MASK|0x00)				/** PWM第1组第0通道标识 */
#define 	PWM_G1_CH1_PA9_PA41 		(PWM_GROUP1_MASK|0x01)				/** PWM第1组第1通道标识 */
#define 	PWM_G1_CH2_PA10_PA42 		(PWM_GROUP1_MASK|0x02)				/** PWM第1组第2通道标识 */
#define 	PWM_G1_CH3_PA11_PA43 		(PWM_GROUP1_MASK|0x03)				/** PWM第1组第3通道标识 */
#define 	PWM_G1_CH4_PA12_PA44 		(PWM_GROUP1_MASK|0x04)				/** PWM第1组第4通道标识 */
#define 	PWM_G1_CH5_PA13_PA45 		(PWM_GROUP1_MASK|0x05)				/** PWM第1组第5通道标识 */
#define 	PWM_G1_CH6_PA14_PA46 		(PWM_GROUP1_MASK|0x06)				/** PWM第1组第6通道标识 */
#define 	PWM_G1_CH7_PA15_PA47 		(PWM_GROUP1_MASK|0x07)				/** PWM第1组第7通道标识 */
///--------------------------------------------------------------------------------
#define 	PWM_G2_CH0_PA16_PB0 		(PWM_GROUP2_MASK|0x00)				/** PWM第2组第0通道标识 */
#define 	PWM_G2_CH1_PA17_PB1 		(PWM_GROUP2_MASK|0x01)				/** PWM第2组第1通道标识 */
#define 	PWM_G2_CH2_PA18_PB2 		(PWM_GROUP2_MASK|0x02)				/** PWM第2组第2通道标识 */
#define 	PWM_G2_CH3_PA19_PB3 		(PWM_GROUP2_MASK|0x03)				/** PWM第2组第3通道标识 */
#define 	PWM_G2_CH4_PA20_PB4 		(PWM_GROUP2_MASK|0x04)				/** PWM第2组第4通道标识 */
#define 	PWM_G2_CH5_PA21_PB5 		(PWM_GROUP2_MASK|0x05)				/** PWM第2组第5通道标识 */
#define 	PWM_G2_CH6_PA22_PB6			(PWM_GROUP2_MASK|0x06)				/** PWM第2组第6通道标识 */
#define 	PWM_G2_CH7_PA23_PB7			(PWM_GROUP2_MASK|0x07)				/** PWM第2组第7通道标识 */
//--------------------------------------------------------------------------------
#define 	PWM_G3_CH0_PA24_PB8			(PWM_GROUP3_MASK|0x00)				/** PWM第3组第0通道标识 */
#define 	PWM_G3_CH1_PA25_PB9			(PWM_GROUP3_MASK|0x01)				/** PWM第3组第1通道标识 */
#define 	PWM_G3_CH2_PA26_PB10 		(PWM_GROUP3_MASK|0x02)				/** PWM第3组第2通道标识 */
#define 	PWM_G3_CH3_PA27_PB11 		(PWM_GROUP3_MASK|0x03)				/** PWM第3组第3通道标识 */
#define 	PWM_G3_CH4_PA28_PB12 		(PWM_GROUP3_MASK|0x04)				/** PWM第3组第4通道标识 */
#define 	PWM_G3_CH5_PA29_PB13 		(PWM_GROUP3_MASK|0x05)				/** PWM第3组第5通道标识 */
#define 	PWM_G3_CH6_PA30_PB14 		(PWM_GROUP3_MASK|0x06)				/** PWM第3组第6通道标识 */
#define 	PWM_G3_CH7_PA31_PB15  		(PWM_GROUP3_MASK|0x07)				/** PWM第3组第7通道标识 */
//--------------------------------------------------------------------------------

#define 	PWM_G0_CH0_PA0_PA32_MASK   	(1UL<<0)		/** PWM第0组第0通道标识MASK */
#define 	PWM_G0_CH1_PA1_PA33_MASK  	(1UL<<1)		/** PWM第0组第1通道标识MASK */
#define 	PWM_G0_CH2_PA2_PA34_MASK   	(1UL<<2)		/** PWM第0组第2通道标识MASK */
#define 	PWM_G0_CH3_PA3_PA35_MASK   	(1UL<<3)		/** PWM第0组第3通道标识MASK */
#define 	PWM_G0_CH4_PA4_PA36_MASK   	(1UL<<4)		/** PWM第0组第4通道标识MASK */
#define 	PWM_G0_CH5_PA5_PA37_MASK    (1UL<<5)		/** PWM第0组第5通道标识MASK */
#define 	PWM_G0_CH6_PA6_PA38_MASK   	(1UL<<6)		/** PWM第0组第6通道标识MASK */
#define 	PWM_G0_CH7_PA7_PA39_MASK	(1UL<<7)		/** PWM第0组第7通道标识MASK */

#define 	PWM_G0_MASK  				(0xff<<(PWM_GROUP0*PWM_CH_NUMBER))		/** PWM第0组所有通道标识MASK */
//--------------------------------------------------------------------------------
#define 	PWM_G1_CH0_PA8_PA40_MASK	(1UL<<8)		/** PWM第1组第0通道标识MASK */
#define 	PWM_G1_CH1_PA9_PA41_MASK 	(1UL<<9)		/** PWM第1组第1通道标识MASK */
#define 	PWM_G1_CH2_PA10_PA42_MASK 	(1UL<<10)		/** PWM第1组第2通道标识MASK */
#define 	PWM_G1_CH3_PA11_PA43_MASK 	(1UL<<11)		/** PWM第1组第3通道标识MASK */
#define 	PWM_G1_CH4_PA12_PA44_MASK 	(1UL<<12)		/** PWM第1组第4通道标识MASK */
#define 	PWM_G1_CH5_PA13_PA45_MASK 	(1UL<<13)		/** PWM第1组第5通道标识MASK */
#define 	PWM_G1_CH6_PA14_PA46_MASK 	(1UL<<14)		/** PWM第1组第6通道标识MASK */
#define 	PWM_G1_CH7_PA15_PA47_MASK 	(1UL<<15)		/** PWM第1组第7通道标识MASK */

#define 	PWM_G1_MASK  				(0xff<<(PWM_GROUP1*PWM_CH_NUMBER))		/** PWM第1组所有通道标识MASK */
///--------------------------------------------------------------------------------
#define 	PWM_G2_CH0_PA16_PB0_MASK 	(1UL<<16)		/** PWM第2组第0通道标识MASK */
#define 	PWM_G2_CH1_PA17_PB1_MASK 	(1UL<<17)		/** PWM第2组第1通道标识MASK */
#define 	PWM_G2_CH2_PA18_PB2_MASK 	(1UL<<18)		/** PWM第2组第2通道标识MASK */
#define 	PWM_G2_CH3_PA19_PB3_MASK 	(1UL<<19)		/** PWM第2组第3通道标识MASK */
#define 	PWM_G2_CH4_PA20_PB4_MASK 	(1UL<<20)		/** PWM第2组第4通道标识MASK */
#define 	PWM_G2_CH5_PA21_PB5_MASK 	(1UL<<21)		/** PWM第2组第5通道标识MASK */
#define 	PWM_G2_CH6_PA22_PB6_MASK	(1UL<<22)		/** PWM第2组第6通道标识MASK */
#define 	PWM_G2_CH7_PA23_PB7_MASK	(1UL<<23)		/** PWM第2组第7通道标识MASK */

#define 	PWM_G2_MASK  				(0xff<<(PWM_GROUP2*PWM_CH_NUMBER))		/** PWM第2组所有通道标识MASK */
//--------------------------------------------------------------------------------
#define 	PWM_G3_CH0_PA24_PB8_MASK	(1UL<<24)		/** PWM第3组第0通道标识MASK */
#define 	PWM_G3_CH1_PA25_PB9_MASK	(1UL<<25)		/** PWM第3组第1通道标识MASK */
#define 	PWM_G3_CH2_PA26_PB10_MASK 	(1UL<<26)		/** PWM第3组第2通道标识MASK */
#define 	PWM_G3_CH3_PA27_PB11_MASK 	(1UL<<27)		/** PWM第3组第3通道标识MASK */
#define 	PWM_G3_CH4_PA28_PB12_MASK 	(1UL<<28)		/** PWM第3组第4通道标识MASK */
#define 	PWM_G3_CH5_PA29_PB13_MASK 	(1UL<<29)		/** PWM第3组第5通道标识MASK */
#define 	PWM_G3_CH6_PA30_PB14_MASK 	(1UL<<30)		/** PWM第3组第6通道标识MASK */
#define 	PWM_G3_CH7_PA31_PB15_MASK  	(1UL<<31)		/** PWM第3组第7通道标识MASK */

#define 	PWM_G3_MASK  				(0xff<<(PWM_GROUP3*PWM_CH_NUMBER))		/** PWM第3组所有通道标识MASK */

#define 	PWM_ALL_MASK  				(0xffffffff)		/** PWM所有通道标识MASK */

#define 	PWM_MAX_CYCLE  				(0xffffffUL)		/** PWM最大可配置周期数 */

#define		PWM_CLK_DIV(n)				(n-1)    			/** PWM时钟分频 **/

#define 	PWM_POLARITY_POS  			(1)					/** PWM输出正极性标识 */
#define 	PWM_POLARITY_NEG  			(0)					/** PWM输出负极性标识 */

/**
 * \brief PWM复用IO配置
 */
typedef enum
{
    RY_PWM_REUSE_P0_31		=  0,		/**< PWM复用到PA0~PA31 */
	RY_PWM_REUSE_P32_63		=  1,		/**< PWM复用到PA32~PB15 */
} pwm_reuse_t;

/**
 * \brief PWM计数模式控制
 */
typedef enum
{
    RY_PWM_NONE_COUNT	=  0,			/**< 不计数模式 */
    RY_PWM_UP_COUNT		=  1,			/**< 加计数模式 */
    RY_PWM_DOWN_COUNT	=  2,			/**< 减计数模式 */
	RY_PWM_BIDIR_COUNT 	=  3,			/**< 双向计数模式 */
} pwm_mode_t;

/**
 * \brief PWM ADC模式在双向计数模式下产生AD采样信号的时段
 */
typedef enum
{
    RY_PWM_ADC_TRIG_NONE		=  0,		/**< 不触发ADC采样信号 */
    RY_PWM_ADC_TRIG_RISING		=  1,		/**< 上升沿触发ADC采样信号 */
	RY_PWM_ADC_TRIG_FALLING		=  2,		/**< 下降沿触发ADC采样信号 */
	RY_PWM_ADC_TRIG_DUAL		=  3,		/**< 边沿跳变触发ADC采样信号 */
} pwm_adc_trig_t;

/* MODESEL Register (+0xc8) */
#define 	PWM_DMA_MODE_EN				(1UL<<3)			/** 使能PWM DMA模式 */
#define 	PWM_GPIO_ADC_COMP_EN		(1UL<<2)			/** 使能GPIO中断触发ADC采样输出 */
#define 	PWM_ADC_COMP_EN				(1UL<<1)			/** 使能ADC采样输出 */
#define 	PWM_STOP_MODE_EN			(1UL<<0)			/** 使能PWM急停模式 */

/******************		INT中断 		******************/
#define		PWM_INT_G3_POS 				(4UL)				/** 第3组PWM中断偏移位 */
#define		PWM_INT_G2_POS 				(3UL)				/** 第2组PWM中断偏移位 */
#define		PWM_INT_G1_POS 				(2UL)				/** 第1组PWM中断偏移位 */
#define		PWM_INT_G0_POS 				(1UL)				/** 第0组PWM中断偏移位 */
#define		PWM_INT_DMA_POS 			(0UL)				/** PWM DMA中断偏移位 */

#define 	PWM_INT_ALL_MASK			(0x1fUL)				/** PWM所有中断MASK */
#define 	PWM_INT_G3_MASK      	   	(1<<PWM_INT_G3_POS) 	/** PWM第3组中断MASK */
#define 	PWM_INT_G2_MASK      	   	(1<<PWM_INT_G2_POS) 	/** PWM第2组中断MASK */
#define 	PWM_INT_G1_MASK      	   	(1<<PWM_INT_G2_POS) 	/** PWM第1组中断MASK */
#define 	PWM_INT_G0_MASK      	   	(1<<PWM_INT_G2_POS) 	/** PWM第0组中断MASK */
#define 	PWM_INT_DMA_MASK      	   	(1<<PWM_INT_DMA_POS) 	/** PWM DMA中断MASK */
#define 	PWM_INT_NONE_MASK			(0x0UL)					/** PWM无中断MASK */

/* STOP_MODE Register (+0xe4) */
#define		PWM_ACMP_STOP_EN_POS 		(6UL)				/** PWM使能COMP急停偏移位 */
#define		PWM_IO_STOP_EN_POS 			(5UL)				/** PWM使能IO急停偏移位 */
#define		PWM_STOP_TRIG_POS 			(4UL)				/** PWM触发急停偏移位 */
#define		PWM_STOP_G3_POS 			(3UL)				/** PWM第3组急停偏移位 */
#define		PWM_STOP_G2_POS 			(2UL)				/** PWM第2组急停偏移位 */
#define		PWM_STOP_G1_POS 			(1UL)				/** PWM第1组急停偏移位 */
#define		PWM_STOP_G0_POS 			(0UL)				/** PWM第0组急停偏移位 */

#define 	PWM_ACMP_STOP_EN			(1<<PWM_ACMP_STOP_EN_POS)			/** 启用PWM急停模式以后，写1使能该位，模块可通过COMP进行急停 */
#define 	PWM_IO_STOP_EN				(1<<PWM_IO_STOP_EN_POS)				/** 启用PWM急停模式以后，写1使能IO急停，PWM可通过外部输入IO进行急停 */
#define 	PWM_STOP_TRIG				(1<<PWM_STOP_TRIG_POS)				/** 启用PWM急停模式以后，使能该位触发急停 */
#define 	PWM_G3_STOP_MASK			(1<<PWM_STOP_G3_POS)				/** 第3组PWM急停使能 */
#define 	PWM_G2_STOP_MASK			(1<<PWM_STOP_G2_POS)				/** 第2组PWM急停使能 */
#define 	PWM_G1_STOP_MASK			(1<<PWM_STOP_G1_POS)				/** 第1组PWM急停使能 */
#define 	PWM_G0_STOP_MASK			(1<<PWM_STOP_G0_POS)				/** 第0组PWM急停使能 */

/* DMA_MODE Register (+0xe8) */
#define		PWM_DMA_CH_SEL_POS 			(16UL)				/** DMA模式PWM组选择偏移位 */
#define		PWM_DMA_CNT_POS 			(0UL)				/** DMA模式产生请求计数值偏移位 */

/**
 * \brief	PWM通道使能
 * \param[in] pwm_mask：PWM通道标识MASK，支持'或'操作
 * \return 无
 */
static inline void pwm_chl_enable(uint32_t pwm_mask)
{
	GTRY_PWM->PWM_ENABLE |= pwm_mask;
}

/**
 * \brief	PWM通道关闭
 * \param[in] pwm_mask：PWM通道标识MASK，支持'或'操作
 * \return 无
 */
static inline void pwm_chl_disable(uint32_t pwm_mask)
{
	GTRY_PWM->PWM_ENABLE &= ~pwm_mask;
}

/**
 * \brief	PWM通道比较值更新使能
 * \param[in] enable：PWM比较值更新使能
 * \return 无
 */
static inline void pwm_update_en(uint32_t enable)
{
	GTRY_PWM->PWM_UPDATE_EN = enable;
}

/**
 * \brief	PWM时钟分频选择
 * \param[in] pwm_group：PWM组
 * \param[in] scaler：PWM时钟分频比 \ref PWM_CLK_DIV(n)
 * \return 无
 */
static inline void pwm_clk_scaler(uint32_t pwm_group, uint32_t scaler)
{
	GTRY_PWM->PWM_SCALER &= ~(0xffUL<<(pwm_group<<3));
	GTRY_PWM->PWM_SCALER |= (scaler&0xff)<<(pwm_group<<3);
}

/**
 * \brief	PWM通道输出正极性配置
 * \param[in] pwm_mask：PWM通道标识MASK
 * \return 无
 */
static inline void pwm_polarity_pos(uint32_t pwm_mask)
{
	GTRY_PWM->PWM_POLARITY |= pwm_mask;
}


/**
 * \brief	PWM通道输出负极性配置
 * \param[in] pwm_mask：PWM通道标识MASK
 * \return 无
 */
static inline void pwm_polarity_neg(uint32_t pwm_mask)
{
	GTRY_PWM->PWM_POLARITY &= ~pwm_mask;
}

/**
 * \brief	PWM通道组周期数配置
 * \param[in] pwm_group：PWM第n组标识
 * \param[in] cycle：PWM输出周期数，实际周期为cycle*PWM时钟周期
 * \return 无
 */
static inline void pwm_set_cycle(uint8_t pwm_group, uint32_t cycle)
{
	if(cycle > PWM_MAX_CYCLE)
	{
		GTRY_PWM->PWM_CYCLE[pwm_group] = PWM_MAX_CYCLE;
	}
	else
	{
		GTRY_PWM->PWM_CYCLE[pwm_group] = cycle;
	}
}

/**
 * \brief	PWM通道组第m个通道占空周期数配置
 * \param[in] pwm_ch：PWM第n组第m个通道标识
 * \param[in] duty：PWM输出占空周期数，实际周期为duty*PWM时钟周期
 * \return 无
 */
static inline void pwm_set_duty(uint32_t pwm_ch, uint32_t duty)
{
	if(duty > PWM_MAX_CYCLE)
	{
		GTRY_PWM->PWM_COMPARE[pwm_ch] = PWM_MAX_CYCLE;
	}
	else
	{
		GTRY_PWM->PWM_COMPARE[pwm_ch] = duty;
	}
}

/**
 * \brief	PWM通道组计数模式设置
 * \param[in] pwm_group：PWM第n组标识
 * \param[in] count_mode：PWM计数模式 \ref pwm_mode_t
 * \return 无
 */
static inline void pwm_mode_set(uint8_t pwm_group, pwm_mode_t count_mode)
{
	GTRY_PWM->PWM_CONFIG[pwm_group] = count_mode;
}

/**
 * \brief	PWM通道组计数模式清除
 * \param[in] pwm_group：PWM第n组标识
 * \return 无
 */
static inline void pwm_mode_clr(uint8_t pwm_group)
{
	GTRY_PWM->PWM_CONFIG[pwm_group] = RY_PWM_NONE_COUNT;
}

/**
 * \brief	PWM STOP模式使能
 * \param[in] 无
 * \return 无
 */
static inline void pwm_stop_en(void)
{
	GTRY_PWM->PWM_MODESEL |= PWM_STOP_MODE_EN;
}

/**
 * \brief	PWM通道组STOP配置
 * \param[in] pwm_group：PWM第n组标识
 * \return 无
 */
static inline void pwm_stop_group_en(uint32_t pwm_group)
{
	GTRY_PWM->PWM_STOP_MODE |= (0x1UL<<pwm_group);
}

/**
 * \brief	PWM使能ACMP触发STOP
 * \param[in] 无
 * \return 无
 */
static inline void pwm_stop_acmp_en(void)
{
	GTRY_PWM->PWM_STOP_MODE |= PWM_ACMP_STOP_EN;
}

/**
 * \brief	PWM使能IO触发STOP
 * \param[in] 无
 * \return 无
 */
static inline void pwm_stop_io_en(void)
{
	GTRY_PWM->PWM_STOP_MODE |= PWM_IO_STOP_EN;
}

/**
 * \brief	PWM立即触发STOP
 * \param[in] 无
 * \return 无
 */
static inline void pwm_stop_trig(void)
{
	GTRY_PWM->PWM_STOP_MODE |= PWM_STOP_TRIG;
}

/**
 * \brief	PWM DMA模式使能
 * \param[in] 无
 * \return 无
 */
static inline void pwm_dma_en(void)
{
	GTRY_PWM->PWM_MODESEL |= PWM_DMA_MODE_EN;
}

/**
 * @brief	清除PWM模块的中断
 * @param[in] int_mask：中断源MASK，参见'PWM_INT'前缀宏
 * @retval 无
 */
static inline void pwm_clr_int(uint32_t int_mask)
{
	GTRY_PWM->PWM_INT_CLR = int_mask;
}
/**
 * @brief	配置PWM模块的中断
 * @param[in] int_mask：中断源MASK，参见'PWM_INT'前缀宏
 * @retval 无
 */
static inline void pwm_en_int(uint32_t int_mask)
{
	GTRY_PWM->PWM_INT_MASK = int_mask;
}
/**
 * @brief	获取PWM模块的中断状态
 * @param[in] 无
 * @retval 返回中断状态
 */
static inline uint32_t pwm_get_status(void)
{
	return GTRY_PWM->PWM_INT_STS;
}

/**
 * @brief	PWM模块中断回调函数指针
 * @param[in] event：PWM中断状态源
 * @retval 无
 */
typedef void (*p_pwm_int_cb)(uint32_t event);

/**
 * \brief	PWM中断初始化
 * \param[in] int_mask：需要配置的中断MASK
 * \param[in] pwm_int_cb：PWM中断回调函数入口
 * \return 无
 */
extern void pwm_int_init(uint32_t int_mask, p_pwm_int_cb pwm_int_cb);

/**
 * \brief	PWM单通道输出配置
 * \param[in] reuse：PWM复用IO配置 \ref pwm_reuse_t
 * \param[in] pwmch：PWM通道标识
 * \param[in] mode：PWM通道组模式 \ref pwm_mode_t
 * \param[in] totalcycle：PWM输出周期，单位us
 * \param[in] dutytime：高电平占空的时间，必须小于等于totalcycle
 * \param[in] ploarity：0表示负极性，非0表示正极性
 * \return 0表示配置成功；非0表示配置失败
 */
extern uint8_t pwm_single_config(pwm_reuse_t reuse, uint32_t pwmch, pwm_mode_t mode, uint32_t totalcycle, uint32_t dutytime, uint8_t ploarity);

/**
 * \brief	PWM单通道输出关闭
 * \param[in] reuse：PWM复用IO配置 \ref pwm_reuse_t
 * \param[in] pwmch：PWM通道标识
 * \return 0表示配置成功；非0表示配置失败
 */
extern uint8_t pwm_single_disable(pwm_reuse_t reuse, uint32_t pwmch);

/**
 * \brief	PWM STOP模式配置
 * \param[in] pwm_mask：PWM通道标识MASK
 * \param[in] ploarity：PWM STOP极性
 * \param[in] acmp_stop_en：PWM ACMP急停使能
 * \param[in] io_stop_en：PWM IO急停使能
 * \param[in] stop_pin：PWM急停IO
 * \return 无
 */
extern void pwm_stop_cfg(uint32_t pwm_mask, uint8_t ploarity, uint8_t acmp_stop_en, uint8_t io_stop_en, uint32_t stop_pin);

/**
 * \brief	PWM DMA模式配置
 * \param[in] pwm_ch：PWM通道标识
 * \param[in] dma_cnt：PWM产生DMA请求的计数值
 * \param[in] duty_buf：DMA模式下PWM输出的BUF指针
 * \param[in] num：DMA读取数据个数
 * \return 无
 */
extern void pwm_dma_cfg(uint32_t pwm_ch, uint32_t dma_cnt, uint32_t *duty_buf, uint32_t num);

#endif	/* __GTRY_PWM_DRV_H_ */
