/*
 *	gtry_adc_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_ADC_DRV_H_
#define __GTRY_ADC_DRV_H_

#include "gtry_platform.h"

#define		ADC_CHL_NUM						30				/** ADC最大量化通道数目 */
#define		ADC_CH0_PA0						0
#define		ADC_CH1_PA1						1
#define		ADC_CH2_PA2						2
#define		ADC_CH3_PA3						3
#define		ADC_CH4_PA4						4
#define		ADC_CH5_PA5						5
#define		ADC_CH6_PA6						6
#define		ADC_CH7_PA7						7
#define		ADC_CH8_PA8						8
#define		ADC_CH9_PA9						9
#define		ADC_CH10_PA10					10
#define		ADC_CH11_PA11					11
#define		ADC_CH12_PA12					12
#define		ADC_CH13_PA13					13
#define		ADC_CH14_PA14					14
#define		ADC_CH15_PA15					15
#define		ADC_CH16_PA16					16
#define		ADC_CH17_PA17					17
#define		ADC_CH18_PA18					18
#define		ADC_CH19_PA19					19

#define		ADC_CH24_INTER_VIP12			24
#define		ADC_CH25_INTER_VTEMP			25


#define		ADC_CH0_PA0_MASK				(1UL<<0)
#define		ADC_CH1_PA1_MASK				(1UL<<1)
#define		ADC_CH2_PA2_MASK				(1UL<<2)
#define		ADC_CH3_PA3_MASK				(1UL<<3)
#define		ADC_CH4_PA4_MASK				(1UL<<4)
#define		ADC_CH5_PA5_MASK				(1UL<<5)
#define		ADC_CH6_PA6_MASK				(1UL<<6)
#define		ADC_CH7_PA7_MASK				(1UL<<7)
#define		ADC_CH8_PA8_MASK				(1UL<<8)
#define		ADC_CH9_PA9_MASK				(1UL<<9)
#define		ADC_CH10_PA10_MASK				(1UL<<10)
#define		ADC_CH11_PA11_MASK				(1UL<<11)
#define		ADC_CH12_PA12_MASK				(1UL<<12)
#define		ADC_CH13_PA13_MASK				(1UL<<13)
#define		ADC_CH14_PA14_MASK				(1UL<<14)
#define		ADC_CH15_PA15_MASK				(1UL<<15)
#define		ADC_CH16_PA16_MASK				(1UL<<16)
#define		ADC_CH17_PA17_MASK				(1UL<<17)
#define		ADC_CH18_PA18_MASK				(1UL<<18)
#define		ADC_CH19_PA19_MASK				(1UL<<19)

#define		ADC_CH24_INTER_VIP12_MASK		(1UL<<24)
#define		ADC_CH25_INTER_VTEMP_MASK		(1UL<<25)

#define		ADC_CH_ALL_MASK		    		(0x30fffffUL)

/******************		ADC_CONFIG寄存器偏移位 	******************/
#define		ADC_CFG_SAR_CFG_POS				(16UL) 			/** ADC_CONFIG寄存器转换时间偏移位 */
#define		ADC_CFG_DMA_BURST_POS			(6UL) 			/** ADC_CONFIG寄存器DMA burst访问长度偏移位 */
#define		ADC_CFG_DMA_EN_POS				(5UL) 			/** ADC_CONFIG寄存器DMA使能偏移位 */
#define		ADC_CFG_VREF_BUFF_EN_POS		(4UL) 			/** ADC_CONFIG寄存器外部VREF BUFFER使能偏移位 */
#define		ADC_CFG_SAR_SAVE_POS			(3UL) 			/** ADC_CONFIG寄存器SAR SAVE使能偏移位 */
#define		ADC_CFG_MULTI_SCAN_POS			(2UL) 			/** ADC_CONFIG寄存器单/多通道转换配置偏移位 */
#define		ADC_CFG_REPEAT_SCAN_POS			(1UL) 			/** ADC_CONFIG寄存器单次/多次转换配置偏移位 */
#define		ADC_CFG_ENABLE_POS				(0UL) 			/** ADC_CONFIG寄存器ADC模块使能偏移位 */
/******************		ADC_CONFIG寄存器偏移位END		******************/

/**
 * \brief ADC_DMA burst访问长度
 */
typedef enum
{
	RY_ADC_BURST_1TIMES			=  0,		/**< 1次传输 */
	RY_ADC_BURST_2TIMES			=  1,		/**< 2次传输 */
	RY_ADC_BURST_4TIMES			=  2,		/**< 4次传输 */
	RY_ADC_BURST_8TIMES			=  3,		/**< 8次传输 */
} adc_dma_burst_t;

/**
 * \brief ADC转换模式
 */
typedef enum
{
	RY_ADC_MODE_SCSC			=  0,		/**< 单通道单次转换 */
	RY_ADC_MODE_SCCC			=  1,		/**< 单通道连续转换 */
	RY_ADC_MODE_MCSC			=  2,		/**< 多通道单次转换 */
	RY_ADC_MODE_MCCC			=  3,		/**< 多通道连续转换 */
} adc_mode_t;

#define		ADC_DMA_EN						(1<<ADC_CFG_DMA_EN_POS)				/**< ADC使能DMA */
#define 	ADC_VREF_BUFFER_ON     			(1<<ADC_CFG_VREF_BUFF_EN_POS)		/**< 打开ADC VREF BUFFER使能 */
#define		ADC_SAR_SAVE_EN					(1<<ADC_CFG_SAR_SAVE_POS)			/**< ADC使能SAR转换数据保存至CHANNEL_REG */

#define		ADC_SAMPLE_MULTI				(1<<ADC_CFG_MULTI_SCAN_POS)			/**< ADC多通道转换 */
#define		ADC_SAMPLE_REPEAT				(1<<ADC_CFG_REPEAT_SCAN_POS)		/**< ADC连续转换 */

#define		ADC_ENABLE						(1<<ADC_CFG_ENABLE_POS)				/**< ADC模块使能 */

/******************		FIFO寄存器偏移位 		******************/
#define		ADC_FIFO_CLEAR_POS 				(1UL)			/** FIFO寄存器清除FIFO数据偏移位 */
#define		ADC_FIFO_CLR_ST_POS 			(0UL)			/** FIFO寄存器FIFO数据清除状态偏移位 */
/******************		FIFO寄存器偏移位END 		******************/

#define		ADC_FIFO_CLEAR					(1<<ADC_FIFO_CLEAR_POS)			/**< ADC清除FIFO数据 */
#define		ADC_FIFO_CLR_ST					(1<<ADC_FIFO_CLR_ST_POS)		/**< ADC FIFO数据清除状态 */

/******************		INT中断偏移位 		******************/
#define		ADC_INT_CAP_POS 				(2UL)			/** ADC电容中断偏移位 */
#define		ADC_INT_COMP_POS 				(1UL)			/** ADC比较器中断偏移位 */
#define		ADC_INT_SAR_POS 				(0UL)			/** ADC通道转换中断偏移位 */
/******************		INT中断偏移位END 		******************/

#define 	ADC_INT_ALL_MASK				(0x7UL)			/** ADC所有中断掩码 */
#define 	ADC_INT_CAP_MASK      	   		(1<<2UL)        /** ADC电容中断掩码 */
#define 	ADC_INT_COMP_MASK     	   		(1<<1UL)        /** ADC比较器中断掩码 */
#define 	ADC_INT_SAR_MASK      	   		(1<<0UL)        /** ADC通道转换中断掩码 */

/**
 * @brief	使能ADC模块
 * @param[in] 无
 * @retval 无
 */
static inline void adc_enable(void)
{
	GTRY_ADC->ADC_CONFIG |= ADC_ENABLE;
}
/**
 * @brief	禁止ADC模块
 * @param[in] 无
 * @retval 无
 */
static inline void adc_disable(void)
{
	GTRY_ADC->ADC_CONFIG &= ~ADC_ENABLE;
}
/**
 * @brief	设置ADC模块外部VREF BUFFER
 * @param[in] buffer_on：为'0'不使能，'非0'使能BUFFER_ON，
 * @retval 无
 */
static inline void adc_set_buffer(uint32_t buffer_on)
{
	buffer_on? (GTRY_ADC->ADC_CONFIG |= ADC_VREF_BUFFER_ON):(GTRY_ADC->ADC_CONFIG &= ~ADC_VREF_BUFFER_ON);
}
/**
 * @brief	配置ADC模块量化转换通道
 * @param[in] channel：量化转换通道，每一个BIT位对应一个通道，参见'ADC_CH'前缀MASK宏
 * @retval 无
 */
static inline void adc_set_channel(uint32_t channel)
{
	GTRY_ADC->ADC_SWITCH_CFG = channel;
}
/**
 * @brief	获取ADC模块量化转换通道
 * @param[in] 无
 * @retval 返回量化转换通道，每一个BIT位对应一个通道，参见'ADC_CH'前缀MASK宏
 */
static inline uint32_t adc_get_channel(void)
{
	return GTRY_ADC->ADC_SWITCH_CFG;
}
/**
 * @brief	开始ADC转换
 * @param[in] 无
 * @retval 无
 */
static inline void adc_convert_start(void)
{
	GTRY_ADC->ADC_CONV_CFG = 0x1;
}
/**
 * @brief	获取ADC转换状态
 * @param[in] 无
 * @retval 返回'1'表示ADC转换中，返回'0'表示未在转换
 */
static inline uint32_t adc_get_convert_status(void)
{
	return GTRY_ADC->ADC_CONV_CFG;
}
/**
 * @brief	清除ADC模块的中断
 * @param[in] int_mask：中断源MASK，参见'ADC_INT'前缀宏
 * @retval 无
 */
static inline void adc_clr_int(uint32_t int_mask)
{
	GTRY_ADC->ADC_INT_CLR = int_mask;
}
/**
 * @brief	配置ADC模块的中断
 * @param[in] int_mask：中断源MASK，参见'ADC_INT'前缀宏
 * @retval 无
 */
static inline void adc_en_int(uint32_t int_mask)
{
	GTRY_ADC->ADC_INT_EN = int_mask;
}
/**
 * @brief	获取ADC模块的中断状态
 * @param[in] 无
 * @retval 返回中断状态
 */
static inline uint32_t adc_int_status(void)
{
	return GTRY_ADC->ADC_INT_STS;
}
/**
 * @brief	设置ADC模块DMA读取数据个数
 * @param[in] size：DMA读取数据个数，通常设置为使能通道个数*重复转换次数
 * @retval 无
 */
static inline void adc_set_dma_size(uint32_t size)
{
	GTRY_ADC->ADC_DMA_SIZE = size;
}
/**
 * @brief	获取ADC通道转换结果
 * @param[in] channel：ADC通道号，参见'ADC_CH'前缀宏
 * @retval 返回ADC转换结果
 */
static inline uint32_t adc_get_sample_value(uint32_t channel)
{
	return GTRY_ADC->ADC_CHANNEL[channel];
}

/**
 * @brief	ADC模块中断回调函数指针
 * @param[in] event：ADC中断状态源
 * @retval 无
 */
typedef void (*p_adc_int_cb)(uint32_t event);
/**
 * \brief	ADC中断初始化
 * \param[in] int_mask：需要配置的中断MASK
 * \param[in] adc_int_cb：ADC中断回调函数入口
 * \return 无
 */
extern void adc_int_init(uint32_t int_mask, p_adc_int_cb int_callback);
/**
 * @brief	ADC模块执行校准操作
 * @param[in] 无
 * @retval 无
 */
extern void adc_calibrate(void);
/**
 * @brief	ADC模块初始化函数
 * @param[in] mode：ADC转换模式，包括单/多通道单次/连续转换模式、以及是否使能VREF BUFFER
 * @param[in] chl_sel：30通道，用ADC_CHx_PAx_MASK宏来设置，可'或'操作
 * @param[in] cvt_time：ADC通道转换时间，配置值：0~65535，实际转换时间等于'配置值+1'个ADC时钟周期
 * @param[in] buffer_en：VREF BUFFER使能配置，'0'不使能，'非0'使能
 * @retval 无
 */
extern void adc_init(adc_mode_t mode, uint32_t chl_sel, uint32_t cvt_time, uint32_t buffer_en);
/**
 * @brief	ADC模块DMA配置函数
 * @param[in] dma_burst：DMA burst访问长度 \ref adc_dma_burst_t
 * @param[in] res_buf：DMA模式下ADC转换输出的BUF指针
 * @param[in] size：DMA读取数据个数，通常设置为使能通道个数*重复转换次数
 * @retval 无
 */
extern void adc_dma_cfg(adc_dma_burst_t dma_burst, void const *res_buf, uint32_t size);

/**
 * @brief	ADC模块获取使能通道数
 * @param[in] 无
 * @retval 返回当前ADC使能的通道数
 */
extern uint32_t adc_get_ch_num(void);

#endif	/* __GTRY_ADC_DRV_H_ */
