/*
 *	gtry_argb_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_ARGB_DRV_H_
#define __GTRY_ARGB_DRV_H_

#include "gtry_platform.h"

#define 		ARGB_FIFO_DEPTH          			16				/**< ARGB FIFO深度 */

/* CTRL Register (+0x00) */
#define 		ARGB_CTRL_DMA_REQ_NUM_POS			(8UL)			/**< ARGB DMA请求阈值偏移位 */
#define 		ARGB_CTRL_FORMAT_POS				(4UL)			/**< ARGB发送数据模式偏移位 */
#define 		ARGB_CTRL_ENBALE_POS				(3UL)			/**< ARGB模块使能偏移位 */
#define 		ARGB_CTRL_DMA_EN_POS				(2UL)			/**< ARGB DMA使能偏移位 */
#define 		ARGB_CTRL_MODE_POS					(0UL)			/**< ARGB单次/连续模式偏移位 */

/**
 * \brief ARGB模块使能配置
 */
typedef enum
{
	RY_ARGB_DISEN				=  0,				/**< 模块不使能 */
	RY_ARGB_EN					=  1,				/**< 模块使能 */
} argb_enable_t;

/**
 * \brief 发送数据模式配置
 */
typedef enum
{
	RY_ARGB_LITTLE_ENDIAN		=  0,				/**< 低位优先 */
	RY_ARGB_BIG_ENDIAN_1BYTE	=  1,				/**< 1字节高位优先 */
	RY_ARGB_BIG_ENDIAN_2BYTE	=  2,				/**< 2字节高位优先 */
	RY_ARGB_BIG_ENDIAN_4BYTE	=  3,				/**< 4字节高位优先 */
} argb_format_t;

/**
 * \brief ARGB模式配置
 */
typedef enum
{
	RY_ARGB_SINGLE_MODE			=  0,				/**< 单次模式 */
	RY_ARGB_CONSTANT_MODE		=  1,				/**< 连续模式 */
} argb_mode_t;

#define 		ARGB_CTRL_DMA_EN					0x1				/**< ARGB DMA使能 */
#define 		ARGB_CTRL_DMA_DISEN					0x0				/**< ARGB DMA不使能 */

/* CHNL_CFG Register (+0x08) */
#define 		ARGB_CHNL_CFG_CHMUX_POS				(4UL)			/**< ARGB通道输出复用选择偏移位 */
#define 		ARGB_CHNL_CFG_CHEN_POS				(0UL)			/**< ARGB通道使能偏移位 */

#define 		ARGB_GROUP0_MASK		    		(0x1UL<<0)
#define 		ARGB_GROUP1_MASK		    		(0x1UL<<1)
#define 		ARGB_GROUP2_MASK		    		(0x1UL<<2)
#define 		ARGB_GROUP3_MASK		    		(0x1UL<<3)
#define 		ARGB_GROUP4_MASK		    		(0x1UL<<4)
#define 		ARGB_GROUP5_MASK		    		(0x1UL<<5)
#define 		ARGB_GROUP6_MASK		    		(0x1UL<<6)
#define 		ARGB_GROUP7_MASK		    		(0x1UL<<7)
#define 		ARGB_GROUP_MAX		    			8

#define 		ARGB_CH0_EN		    				(0x0)
#define 		ARGB_CH0_1_EN		    			(0x1)
#define 		ARGB_CH0_2_EN		    			(0x2)
#define 		ARGB_CH0_3_EN		    			(0x3)
#define 		ARGB_CH0_4_EN		    			(0x4)
#define 		ARGB_CH0_5_EN		    			(0x5)
#define 		ARGB_CH0_6_EN		    			(0x6)
#define 		ARGB_CH0_7_EN		    			(0x7)
#define 		ARGB_CH_MAX		    				8

#define 		ARGB_GROUP0_CH0		    			PA0
#define 		ARGB_GROUP0_CH1		    			PA1
#define 		ARGB_GROUP0_CH2		    			PA2
#define 		ARGB_GROUP0_CH3		    			PA3
#define 		ARGB_GROUP0_CH4		    			PA4
#define 		ARGB_GROUP0_CH5		    			PA5
#define 		ARGB_GROUP0_CH6		    			PA6
#define 		ARGB_GROUP0_CH7		    			PA7

#define 		ARGB_GROUP1_CH0		    			PA8
#define 		ARGB_GROUP1_CH1		    			PA9
#define 		ARGB_GROUP1_CH2		    			PA10
#define 		ARGB_GROUP1_CH3		    			PA11
#define 		ARGB_GROUP1_CH4		    			PA12
#define 		ARGB_GROUP1_CH5		    			PA13
#define 		ARGB_GROUP1_CH6		    			PA14
#define 		ARGB_GROUP1_CH7		    			PA15

#define 		ARGB_GROUP2_CH0		    			PA16
#define 		ARGB_GROUP2_CH1		    			PA17
#define 		ARGB_GROUP2_CH2		    			PA18
#define 		ARGB_GROUP2_CH3		    			PA19
#define 		ARGB_GROUP2_CH4		    			PA20
#define 		ARGB_GROUP2_CH5		    			PA21
#define 		ARGB_GROUP2_CH6		    			PA22
#define 		ARGB_GROUP2_CH7		    			PA23

#define 		ARGB_GROUP3_CH0		    			PA24
#define 		ARGB_GROUP3_CH1		    			PA25
#define 		ARGB_GROUP3_CH2		    			PA26
#define 		ARGB_GROUP3_CH3		    			PA27
#define 		ARGB_GROUP3_CH4		    			PA28
#define 		ARGB_GROUP3_CH5		    			PA29
#define 		ARGB_GROUP3_CH6		    			PA30
#define 		ARGB_GROUP3_CH7		    			PA31

#define 		ARGB_GROUP4_CH0		    			PA32
#define 		ARGB_GROUP4_CH1		    			PA33
#define 		ARGB_GROUP4_CH2		    			PA34
#define 		ARGB_GROUP4_CH3		    			PA35
#define 		ARGB_GROUP4_CH4		    			PA36
#define 		ARGB_GROUP4_CH5		    			PA37
#define 		ARGB_GROUP4_CH6		    			PA38
#define 		ARGB_GROUP4_CH7		    			PA39

#define 		ARGB_GROUP5_CH0		    			PA40
#define 		ARGB_GROUP5_CH1		    			PA41
#define 		ARGB_GROUP5_CH2		    			PA42
#define 		ARGB_GROUP5_CH3		    			PA43
#define 		ARGB_GROUP5_CH4		    			PA44
#define 		ARGB_GROUP5_CH5		    			PA45
#define 		ARGB_GROUP5_CH6		    			PA46
#define 		ARGB_GROUP5_CH7		    			PA47

#define 		ARGB_GROUP6_CH0		    			PB0
#define 		ARGB_GROUP6_CH1		    			PB1
#define 		ARGB_GROUP6_CH2		    			PB2
#define 		ARGB_GROUP6_CH3		    			PB3
#define 		ARGB_GROUP6_CH4		    			PB4
#define 		ARGB_GROUP6_CH5		    			PB5
#define 		ARGB_GROUP6_CH6		    			PB6
#define 		ARGB_GROUP6_CH7		    			PB7

#define 		ARGB_GROUP7_CH0		    			PB8
#define 		ARGB_GROUP7_CH1		    			PB9
#define 		ARGB_GROUP7_CH2		    			PB10
#define 		ARGB_GROUP7_CH3		    			PB11
#define 		ARGB_GROUP7_CH4		    			PB12
#define 		ARGB_GROUP7_CH5		    			PB13
#define 		ARGB_GROUP7_CH6		    			PB14
#define 		ARGB_GROUP7_CH7		    			PB15

/* COM_DATA Register (+0x24~0x30) */
#define 		ARGB_COMDATA0		    			0				/**< ARGB公用数据0 */
#define 		ARGB_COMDATA1		    			1				/**< ARGB公用数据1 */
#define 		ARGB_COMDATA2		    			2				/**< ARGB公用数据2 */
#define 		ARGB_COMDATA3		    			3				/**< ARGB公用数据3 */

/* INT_EN Register (+0x34) */
#define			ARGB_INT_NULL_MASK					0x0				/**< ARGB无中断MASK */
#define			ARGB_INT_FIFO_EMPTY_MASK			(1UL<<2)		/**< ARGB FIFO空中断MASK */
#define			ARGB_INT_FRAME_END_MASK				(1UL<<1)		/**< ARGB帧结束中断MASK */
#define			ARGB_INT_FIFO_ERROR_MASK			(1UL<<0)		/**< ARGB FIFO错误中断MASK */
#define			ARGB_INT_ALL_MASK					0x7				/**< ARGB所有中断MASK */

/* ST Register (+0x38) */
#define			ARGB_ST_BUSY						(1UL<<4)		/**< ARGB BUSY状态 */
#define			ARGB_ST_FIFO_FULL					(1UL<<3)		/**< ARGB FIFO满状态 */
#define			ARGB_ST_FIFO_EMPTY					(1UL<<2)		/**< ARGB FIFO空状态 */
#define			ARGB_ST_FRAME_END					(1UL<<1)		/**< ARGB 帧结束状态 */
#define			ARGB_ST_FIFO_ERROR					(1UL<<0)		/**< ARGB FIFO错误状态 */
#define			ARGB_ST_ALL_MASK					0x1f			/**< ARGB所有状态 */

/**
 * \brief	ARGB中断回调函数指针
 * \param[in] event：产生中断的事件MASK
 * \return 无
 */
typedef void (*p_argb_int_cb)(uint32_t event);

/**
 * \brief	ARGB中断初始化
 * \param[in] int_mask：需要配置的中断MASK
 * \param[in] argb_int_cb：ARGB中断回调函数入口
 * \return 无
 */
extern void argb_int_init(uint32_t int_mask, p_argb_int_cb argb_int_cb);

/**
 * \brief	ARGB初始化
 * \param[in] format：发送数据模式 \ref argb_format_t
 * \param[in] mode：模式配置 \ref argb_mode_t
 * \param[in] argb_en：模块使能/失能配置 \ref argb_enable_t
 * \return 无
 */
extern void argb_init(argb_format_t format, argb_mode_t mode, argb_enable_t argb_en);

/**
 * \brief	ARGB LED配置
 * \param[in] led_num：每条通道的LED灯数量，最大为8192
 * \param[in] data_byte：LED灯的显示数据配置，单位BYTE
 * \param[in] t0h_ns：LED灯传输0的高电平宽度配置，单位ns
 * \param[in] t1h_ns：LED灯传输1的高电平宽度配置，单位ns
 * \param[in] rst_ns：LED灯传输RESET的高电平宽度配置，单位ns
 * \return 无
 */
void argb_led_cfg(uint32_t led_num, uint32_t data_byte, uint32_t t0h_ns , uint32_t t1h_ns, uint32_t rst_us);

/**
 * \brief	ARGB通道（组）初始化
 * \param[in] group_mask：需要使能的GROUP_MASK，见'ARGB_GROUP'前缀宏
 * \param[in] ch_sel：组内通道使能，见'ARGB_CH'前缀宏
 * \return 无
 */
extern void argb_group_ch_cfg(uint32_t group_mask, uint32_t ch_sel);

/**
 * \brief	ARGB公用数据位宽配置
 * \param[in] com_bits：公用数据位宽配置，单位bit，最大支持128bit即4个32bit数据
 * \return 无
 */
extern void argb_combits_cfg(uint32_t com_bits);

/**
 * \brief	ARGB公用数据配置
 * \param[in] data：公用数据指针
 * \return 无
 */
extern void argb_comdata_cfg(uint32_t data_no , uint32_t data);

/**
 * \brief	ARGB向FIFO内写入单个数据
 * \param[in] data：写入数据
 * \return 无
 */
extern void argb_trans_one_data(uint32_t data);

/**
 * \brief	ARGB清除FIFO内数据
 * \param[in] 无
 * \return 无
 */
extern void argb_fifo_clear(void);

/**
 * \brief ARGB DMA传输初始化
 * \param[in] dma_en：DMA使能/失能
 * \param[in] dma_req_num：DMA请求阈值配置
 * \return 无
 */
extern void argb_dma_cfg(uint32_t dma_en, uint32_t dma_req_num);

/**
 * @brief	ARGB模块DMA数据传输配置
 * @param[in] *data：传输数据指针
 * @param[in] num：传输数据个数
 * @return 无
 */
extern void argb_dma_trans_data(uint32_t *data , uint32_t num);

/**
 * \brief	获取ARGB模块状态
 * \param[in] 无
 * \return 返回状态，见'ARGB_ST'前缀宏
 */
extern uint32_t argb_get_status(void);

/**
 * \brief	清除ARGB模块状态
 * \param[in] 无
 * \return 无
 */
extern void argb_clear_status(void);

/**
 * \brief	启动ARGB模块发送数据
 * \param[in] 无
 * \return 无
 */
extern void argb_start(void);

/**
 * \brief	停止ARGB模块
 * \param[in] 无
 * \return 无
 */
extern void argb_stop(void);

#endif /* __GTRY_ARGB_DRV_H_ */
