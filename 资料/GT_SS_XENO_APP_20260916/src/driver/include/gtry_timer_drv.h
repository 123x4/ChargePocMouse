/*
 *	gtry_timer_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_TIMER_DRV_H
#define __GTRY_TIMER_DRV_H

#include "gtry_platform.h"

// TIMER Port Number
typedef enum
{
    TIMER0          = 0,
	TIMER1          = 1,
	TIMER2          = 2,
} RY_TIMER_PORT;

#define		TC_NUMBER0						0					/** timer模块0标识号 */
#define		TC_NUMBER1						1					/** timer模块1标识号 */
#define		TC_NUMBER2						2					/** timer模块2标识号 */

#define		TIMER_CC0						0					/** timer模块比较器0标识号 */
#define		TIMER_CC1						1					/** timer模块比较器1标识号 */
#define		TIMER_CC2						2					/** timer模块比较器2标识号 */
#define		TIMER_CC3						3					/** timer模块比较器3标识号 */
#define		TIMER_CC_NUM					4					/** timer模块比较器数量 */

#define		TIMER_CC0_MASK					(1UL<<0)			/** timer模块比较器0标识号MASK */
#define		TIMER_CC1_MASK					(1UL<<1)			/** timer模块比较器1标识号MASK */
#define		TIMER_CC2_MASK					(1UL<<2)			/** timer模块比较器2标识号MASK */
#define		TIMER_CC3_MASK					(1UL<<3)			/** timer模块比较器3标识号MASK */
#define		TIMER_CC_ALL_MASK				(0xf)				/** timer模块所有比较器标识号MASK */

/* CONFIG Register (+0x00 ~ +0x08) */
#define 	TIMER_CFG_COMP_POS 				(19UL) 				/** CFG寄存器COMP使能偏移位 */
#define 	TIMER_CFG_RELOAD_POS 			(18UL) 				/** CFG寄存器RELOAD使能偏移位 */
#define 	TIMER_CFG_CC_CLR_POS 			(17UL)				/** CFG寄存器CC_CLEAR偏移位 */
#define 	TIMER_CFG_PWM_POS 				(16UL)				/** CFG寄存器PWM使能偏移位 */
#define 	TIMER_CFG_CAP_MODE_POS 			(14UL)				/** CFG寄存器CAP模式偏移位 */
#define 	TIMER_CFG_FILTER_POS 			(13UL)				/** CFG寄存器FILTER使能偏移位 */
#define 	TIMER_CFG_MODE_POS 				(11UL)				/** CFG寄存器通道模式配置偏移位 */
#define 	TIMER_CFG_CC_LOAD_POS 			(3UL)				/** CFG寄存器加载CC值偏移位 */
#define 	TIMER_CFG_CLEAR_POS 			(2UL)				/** CFG寄存器清除计数偏移位 */
#define 	TIMER_CFG_STOP_POS 				(1UL)				/** CFG寄存器TIMER停止偏移位 */
#define 	TIMER_CFG_START_POS 			(0UL)				/** CFG寄存器TIMER启动偏移位 */

/**
 * \brief TIMER位宽配置
 */
typedef enum
{
	RY_TIMER_8BIT				=  0,		/**< 8BIT位宽 */
	RY_TIMER_16BIT				=  1,		/**< 16BIT位宽 */
	RY_TIMER_24BIT				=  2,		/**< 24BIT位宽 */
	RY_TIMER_32BIT				=  3,		/**< 32BIT位宽 */
} timer_bit_mode_t;

/**
 * \brief 比较器使能配置
 */
typedef enum
{
	RY_TIMER_COMP_DISEN			=  0,		/**< COMP不使能 */
	RY_TIMER_COMP_EN			=  1,		/**< COMP使能 */
} timer_comp_en_t;

/**
 * \brief RELOAD使能配置
 */
typedef enum
{
	RY_TIMER_RELOAD_DISEN		=  0,		/**< RELOAD不使能 */
	RY_TIMER_RELOAD_EN			=  1,		/**< RELOAD使能 */
} timer_reload_en_t;

/**
 * \brief PWM模式使能配置
 */
typedef enum
{
	RY_TIMER_PWM_DISEN			=  0,		/**< PWM模式不使能 */
	RY_TIMER_PWM_EN				=  1,		/**< PWM模式使能 */
} timer_pwm_en_t;

/**
 * \brief TIMER捕获边沿模式配置
 */
typedef enum
{
	RY_TIMER_NOCAP				=  0,		/**< CAP模式禁用 */
	RY_TIMER_CAP_FALLING		=  1,		/**< CAP模式为下降沿 */
	RY_TIMER_CAP_RISING			=  2,		/**< CAP模式为上升沿 */
	RY_TIMER_CAP_DUAL			=  3,		/**< CAP模式为双边沿 */
} timer_cap_sense_t;

/**
 * \brief 滤波模式使能配置
 */
typedef enum
{
	RY_TIMER_FILTER_DISEN		=  0,		/**< FILTER模式不使能 */
	RY_TIMER_FILTER_EN			=  1,		/**< FILTER模式使能 */
} timer_filter_en_t;

/**
 * \brief 通道模式配置
 */
typedef enum
{
	RY_TIMER_TIMING				=  0,		/**< TIMER定时模式 */
	RY_TIMER_COUNT				=  3,		/**< TIMER计数模式 */
} timer_mode_t;

#define		TIMER_CC_CLEAR					(1UL<<TIMER_CFG_CC_CLR_POS)				/**< TIMER清零4个CC寄存器 */
#define		TIMER_LOAD_CC(x)				(x<<TIMER_CFG_CC_LOAD_POS)				/**< TIMER加载CC寄存器 */
#define		TIMER_CLEAR						(1UL<<TIMER_CFG_CLEAR_POS)				/**< TIMER清除计数器 */
#define		TIMER_STOP						(1UL<<TIMER_CFG_STOP_POS)				/**< TIMER停止 */
#define		TIMER_START						(1UL<<TIMER_CFG_START_POS)				/**< TIMER启动 */

#define		TIMER_TRIGGER_NUM				0x22

/* PRESCALER Register (+0x20) */
#define 	TIMER_FILTER_VALUE_POS 			(8UL)					/** PRESCALER寄存器滤波系数偏移位 */
#define 	TIMER_PRESCALER_POS 			(0UL)					/** PRESCALER寄存器分频系数偏移位 */

/* INT_EN/INT_CLR/CC_CLEAR/STOP Register */
#define 	TIMER_PWM_POS				 	(20UL)					/** TIMER各配置寄存器PWM参数偏移位 */
#define 	TIMER_CAP_POS				 	(16UL)					/** TIMER各配置寄存器CAP参数偏移位 */

#define 	TIMER_INT_NULL_MASK  		 	(0UL)					/** TIMER无中断MASK */
#define 	TIMER_INT_PWM_MASK  		 	(1UL<<TIMER_PWM_POS)	/** TIMER_PWM中断MASK */
#define 	TIMER_INT_CAP_MASK   			(1UL<<TIMER_CAP_POS) 	/** TIMER_CAP中断MASK */
#define 	TIMER_INT_CC3_MASK   			(1UL<<3) 				/** TIMER_CC3中断MASK */
#define 	TIMER_INT_CC2_MASK   			(1UL<<2) 				/** TIMER_CC2中断MASK */
#define 	TIMER_INT_CC1_MASK   			(1UL<<1) 				/** TIMER_CC1中断MASK */
#define 	TIMER_INT_CC0_MASK   			(1UL<<0) 				/** TIMER_CC0中断MASK */
#define 	TIMER_INT_CC_ALL_MASK   		(0xfUL<<0) 				/** TIMER_CC所有中断MASK */
#define 	TIMER_INT_ALL_MASK   			(0xff0fffUL) 			/** TIMER所有中断MASK */

#define 	TIMER_SHORT_NULL_MASK   		(1UL<<0) 				/** TIMER无关联配置类型MASK */
#define 	TIMER_SHORT_CC_CLR_MASK   		(1UL<<1) 				/** TIMER关联配置CC_CLEAR类型MASK */
#define 	TIMER_SHORT_CC_STOP_MASK   		(1UL<<2) 				/** TIMER关联配置CC_STOP类型MASK */
#define 	TIMER_SHORT_CAP_STOP_MASK   	(1UL<<3) 				/** TIMER关联配置CAP_STOP类型MASK */

/* DMA Register (+0x28) */
#define 	TIMER_DMA_REQ_EN_POS 			(12UL)					/** DMA寄存器DMA请求使能偏移位 */
#define 	TIMER_DMA_REQ_CLR_POS 			(8UL)					/** DMA寄存器DMA请求清除偏移位 */
#define 	TIMER_DMA_CC_SEL_POS 			(4UL)					/** DMA寄存器CC配置偏移位 */
#define 	TIMER_DMA_SEL_POS 				(0UL)					/** DMA寄存器TIMER通道配置偏移位 */

#define		TIMER_DMA_REQ_EN				(1UL<<TIMER_DMA_REQ_EN_POS)				/**< TIMER手动使能DMA请求 */
#define		TIMER_DMA_REQ_CLR				(1UL<<TIMER_DMA_REQ_CLR_POS)			/**< TIMER清除DMA请求 */

#define		TIMER_DMA_CH					3						/** DMA通道 */
#define		TIMER_DMA_REQID					9						/** TIMER_DMA请求号 */

/******************		  COMP捕获模式配置宏 		******************/
#define 	COMP_NUMBER1					1						/** comp模块1标识号 */
#define 	COMP_NUMBER2					2						/** comp模块2标识号 */

#define 	COMP_TIMER_SEL_POS 				(10UL)					/** COMP_CSR寄存器比较器输出选择偏移位 */
#define 	COMP_TIMER_CAP_SEN_POS 			(8UL)					/** COMP_CSR寄存器TIMER捕获边沿偏移位 */
#define 	COMP_ENABLE_POS 				(0UL)					/** COMP_CSR寄存器比较器使能偏移位 */

/**
 * \brief COMP_CSR捕获边沿模式配置
 */
typedef enum
{
	RY_COMP_NOCAP				=  0,		/**< CAP模式禁用 */
	RY_COMP_CAP_FALLING			=  1,		/**< CAP模式为下降沿 */
	RY_COMP_CAP_RISING			=  2,		/**< CAP模式为上升沿 */
	RY_COMP_CAP_DUAL			=  3,		/**< CAP模式为双边沿 */
} comp_cap_sense_t;

#define		COMP_ENABLE						0x1

#define		COMP1_FORWARD_INPUT_IO			PA9
#define		COMP1_REVERSE_INPUT_IO			PA13

#define		COMP2_FORWARD_INPUT_IO			PA32
#define		COMP2_REVERSE_INPUT_IO			PA36

/******** TIMER_CAPTURE_IO每个IO的标识 **********/
#define		TIMER0_CAP_IO_PA0				PA0
#define		TIMER0_CAP_IO_PA4				PA4
#define		TIMER0_CAP_IO_PA8				PA8
#define		TIMER0_CAP_IO_PA12				PA12
#define		TIMER0_CAP_IO_PA16				PA16
#define		TIMER0_CAP_IO_PA20				PA20
#define		TIMER0_CAP_IO_PA24				PA24
#define		TIMER0_CAP_IO_PA28				PA28
#define		TIMER0_CAP_IO_PA32				PA32
#define		TIMER0_CAP_IO_PA36				PA36
#define		TIMER0_CAP_IO_PA40				PA40
#define		TIMER0_CAP_IO_PA44				PA44
#define		TIMER0_CAP_IO_PB0				PB0
#define		TIMER0_CAP_IO_PB4				PB4
#define		TIMER0_CAP_IO_PB8				PB8
#define		TIMER0_CAP_IO_PB12				PB12

#define		TIMER1_CAP_IO_PA1				PA1
#define		TIMER1_CAP_IO_PA5				PA5
#define		TIMER1_CAP_IO_PA9				PA9
#define		TIMER1_CAP_IO_PA13				PA13
#define		TIMER1_CAP_IO_PA17				PA17
#define		TIMER1_CAP_IO_PA21				PA21
#define		TIMER1_CAP_IO_PA25				PA25
#define		TIMER1_CAP_IO_PA29				PA29
#define		TIMER1_CAP_IO_PA33				PA33
#define		TIMER1_CAP_IO_PA37				PA37
#define		TIMER1_CAP_IO_PA41				PA41
#define		TIMER1_CAP_IO_PA45				PA45
#define		TIMER1_CAP_IO_PB1				PB1
#define		TIMER1_CAP_IO_PB5				PB5
#define		TIMER1_CAP_IO_PB9				PB9
#define		TIMER1_CAP_IO_PB13				PB13

#define		TIMER2_CAP_IO_PA2				PA2
#define		TIMER2_CAP_IO_PA6				PA6
#define		TIMER2_CAP_IO_PA10				PA10
#define		TIMER2_CAP_IO_PA14				PA14
#define		TIMER2_CAP_IO_PA18				PA18
#define		TIMER2_CAP_IO_PA22				PA22
#define		TIMER2_CAP_IO_PA26				PA26
#define		TIMER2_CAP_IO_PA30				PA30
#define		TIMER2_CAP_IO_PA34				PA34
#define		TIMER2_CAP_IO_PA38				PA38
#define		TIMER2_CAP_IO_PA42				PA42
#define		TIMER2_CAP_IO_PA46				PA46
#define		TIMER2_CAP_IO_PB2				PB2
#define		TIMER2_CAP_IO_PB6				PB6
#define		TIMER2_CAP_IO_PB10				PB10
#define		TIMER2_CAP_IO_PB14				PB14

/**
 * @brief	TIMER模块比较器CC值设置
 * @param[in] timer_no：定时器模块标识号
 * @param[in] id：定时器模块比较器标识号
 * @param[in] cc_val：比较器CC值，小于比较器位宽最大值
 * @return 无
 */
static inline void timer_set_ccn(uint32_t timer_no, uint32_t id, uint32_t cc_val)
{
	GTRY_TIMER->TIMn_CCx[timer_no][id] = cc_val;
}

/**
 * @brief	获取TIMER模块比较器CC值
 * @param[in] timer_no：定时器模块标识号
 * @param[in] id：定时器模块比较器标识号
 * @return 返回指定timer模块比较器CC值
 */
static inline uint32 timer_get_ccn(uint32_t timer_no, uint32_t id)
{
	return GTRY_TIMER->TIMn_CCx[timer_no][id];
}

/**
 * @brief	设置TIMER模块起始计数值
 * @param[in] timer_no：定时器模块标识号
 * @return 无
 */
static inline void timer_set_count(uint32_t timer_no, uint32_t cnt_val)
{
	GTRY_TIMER->TIMn_CNT_CFG[timer_no] = cnt_val;
}

/**
 * @brief	获取TIMER模块当前计数值
 * @param[in] timer_no：定时器模块标识号
 * @return 返回当前timer通道的计数值
 */
static inline uint32 timer_get_count(uint32_t timer_no)
{
	return GTRY_TIMER->TIMn_CNT_CFG[timer_no];
}

/**
 * @brief	获取TIMER模块CC比较事件
 * @param[in] timer_no：定时器模块标识号
 * @param[in] id：定时器模块比较器标识号
 * @return 返回捕获到的指定timer通道比较器的事件状态
 */
static inline uint32 timer_get_cc_status(uint32_t timer_no, uint32_t id)
{
	return (GTRY_TIMER->TIM_INT_STS>>(timer_no*TIMER_CC_NUM + id))&0x1;
}
/**
 * @brief	清除TIMER模块CC比较事件
 * @param[in] timer_no：定时器模块标识号
 * @param[in] id：定时器模块比较器标识号
 * @return	无
 */
static inline void timer_clr_cc_status(uint32_t timer_no, uint32_t id)
{
	GTRY_TIMER->INT_STS_CLEAR = (0x1UL<<(timer_no*TIMER_CC_NUM + id));
}

/**
 * @brief	获取TIMER模块CAP捕获事件
 * @param[in] timer_no：定时器模块标识号
 * @return 返回捕获到的指定timer通道CAP捕获的事件状态
 */
static inline uint32 timer_get_cap_status(uint32_t timer_no)
{
	return (GTRY_TIMER->TIM_INT_STS>>(timer_no + TIMER_CAP_POS))&0x1;
}
/**
 * @brief	清除TIMER模块CAP捕获事件
 * @param[in] timer_no：定时器模块标识号
 * @return 无
 */
static inline void timer_clr_cap_status(uint32_t timer_no)
{
	GTRY_TIMER->INT_STS_CLEAR = (0x1UL<<(timer_no + TIMER_CAP_POS));
}

/**
 * @brief	获取TIMER模块PWM捕获事件
 * @param[in] timer_no：定时器模块标识号
 * @return 返回捕获到的指定timer通道PWM捕获的事件状态
 */
static inline uint32 timer_get_pwm_status(uint32_t timer_no)
{
	return (GTRY_TIMER->TIM_INT_STS>>(timer_no + TIMER_PWM_POS))&0x1;
}
/**
 * @brief	清除TIMER模块PWM捕获事件
 * @param[in] timer_no：定时器模块标识号
 * @return 无
 */
static inline void timer_clr_pwm_status(uint32_t timer_no)
{
	GTRY_TIMER->INT_STS_CLEAR = (0x1UL<<(timer_no + TIMER_PWM_POS));
}

/**
 * @brief	清除TIMER通道所有事件
 * @param[in] timer_no：定时器模块标识号
 * @return 无
 */
static inline void timer_clr_all_status(uint32_t timer_no)
{
	GTRY_TIMER->INT_STS_CLEAR = (0xfUL<<(timer_no*TIMER_CC_NUM)) | (0x1UL<<(timer_no + TIMER_CAP_POS)) | (0x1UL<<(timer_no + TIMER_PWM_POS));
}

/**
 * @brief	TIMER模块清除4个CC寄存器值
 * @param[in] timer_no：定时器模块标识号
 * @return 无
 */
static inline void timer_cc_clear(uint32 timer_no)
{
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= TIMER_CC_CLEAR;
}

/**
 * @brief	TIMER模块触发清除当前计数
 * @param[in] timer_no：定时器模块标识号
 * @return 无
 */
static inline void timer_trigger_clear(uint32 timer_no)
{
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= TIMER_CLEAR;
}

/**
 * @brief	启动运行TIMER模块
 * @param[in] timer_no: 定时器模块标识号
 * @return 无
 */
static inline void timer_start(uint32 timer_no)
{
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= TIMER_START;
	GTRY_TIMER->TIM_TRIGGER = TIMER_TRIGGER_NUM;
}

/**
 * @brief	停止运行TIMER模块
 * @param[in] timer_no: 定时器模块标识号
 * @return 无
 */
static inline void timer_stop(uint32 timer_no)
{
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= TIMER_STOP;
}

/**
 * \brief	TIMER中断回调函数指针
 * @param[in] intst：timer中断状态
 * \return 无
 */
typedef void (*p_timer_int_cb) (uint32 intst);

/**
 * @brief	TIMER配置指定定时器模块中断处理
 * @param[in] timer_no：定时器通道标识号
 * @param[in] int_mask：中断使能MASK，见'TIMER_INT'前缀宏，仅可以使能PWM/CAP/CC中断的其中一种，不能同时使能
 * @param[in] timer_int_callback：中断回调函数入口
 * @return 无
 */
extern void timer_int_init(uint32_t timer_no, uint32_t int_mask, p_timer_int_cb timer_int_callback);
extern p_timer_int_cb timer_drv_int_callback[3];

/**
 * @brief	TIMER模块初始化
 * @param[in] timer_no：定时器模块标识号
 * @param[in] presclar：timer时钟分频系数\ref timer_bit_mode_t
 * @param[in] mode：TIMER模式 \ref timer_mode_t
 * @param[in] bitmode：timer位宽
 * @retval 无
 */
extern void timer_init(uint32_t timer_no, uint32_t presclar, timer_mode_t mode, timer_bit_mode_t bitmode);

/**
 * @brief TIMER模块关联触发配置
 * @param[in] timer_no：定时器模块标识号
 * @param[in] id：定时器模块比较器标识号
 * @param[in] short_type：关联配置类型，见'TIMER_SHORT'前缀宏
 * @return 无
 */
extern void timer_set_shorts(uint32_t timer_no, uint32_t id, uint32_t short_type);

/**
 * @brief	TIMER模块捕获模式配置
 * @param[in] timer_no：定时器模块标识号
 * @param[in] cap_io：定时器捕获IO通道，见'TIMERx_CAP_IO'前缀宏，其中x为定时器模块标识号
 * @param[in] cap_sense：捕获敏感 \ref timer_cap_sense_t
 * @param[in] pwm_en：PWM捕获是否使能 \ref timer_pwm_en_t
 * @return 无
 */
extern void timer_cap_cfg(uint32_t timer_no, uint32_t cap_io, timer_cap_sense_t cap_sense, timer_pwm_en_t pwm_en);

/**
 * @brief	timer模块重载模式配置
 * @param[in] timer_no：定时器模块标识号
 * @param[in] reload_val：重载值
 * @return 无
 */
extern void timer_reload_cfg(uint32_t timer_no, uint32_t reload_val);

/**
 * @brief	TIMER模块滤波配置
 * @param[in] timer_no：定时器模块标识号
 * @param[in] filter_val：滤波参数
 * @return 无
 */
extern void timer_filter_cfg(uint32_t timer_no, uint32_t filter_val);

/**
 * @brief	timer模块COMP模式配置
 * @param[in] timer_no：定时器模块标识号
 * @param[in] comp_no：比较器模块标识号
 * @param[in] cap_sense：timer捕获比较器的边沿配置 \ref comp_cap_sense_t
 * @return 无
 */
extern void timer_comp_cfg(uint32_t timer_no, uint32_t comp_no, comp_cap_sense_t cap_sense);

/**
 * @brief	TIMER模块DMA模式配置
 * @param[in] timer_no：定时器模块标识号
 * @return 无
 */
extern void timer_dma_cfg(uint32_t timer_no, uint32_t id);

/**
 * @brief	TIMER模块主动发起DMA请求
 * @param[in] 无
 * @return 无
 */
extern void timer_dma_req_en(void);

/**
 * @brief	TIMER模块清除DMA请求
 * @param[in] 无
 * @return 无
 */
extern void timer_dma_req_clr(void);

/**
 * @brief	TIMER模块DMA数据传输配置
 * @param[in] addr：传输目标地址
 * @param[in] *data：传输数据指针
 * @param[in] num：传输数据个数
 * @return 无
 */
extern void timer_dma_trans_data(uint32_t addr, uint32_t *data, uint32_t num);

#endif	/* __GTRY_TIMER_DRV_H */
