/*
 *	gtry_adc_drv.c
 *
 *	Created on: 2025.10.23
 *
 */

#include "gtry_adc_drv.h"
#include "gtry_dma_drv.h"

p_adc_int_cb adc_drv_int_callback = NULL;			/** ADC中断回调函数入口 */

/**
 * \brief	ADC中断初始化
 * \param[in] int_mask：需要配置的中断MASK
 * \param[in] adc_int_cb：ADC中断回调函数入口
 * \return 无
 */
void adc_int_init(uint32_t int_mask, p_adc_int_cb adc_int_cb)
{
	GTRY_ADC->ADC_INT_CLR = ADC_INT_ALL_MASK;
	GTRY_ADC->ADC_INT_EN = int_mask;

	__nds__plic_set_priority(IRQ_ADC_SOURCE, 1);
	__nds__plic_enable_interrupt(IRQ_ADC_SOURCE);

	adc_drv_int_callback = adc_int_cb;
}

/**
 * \brief	ADC中断服务函数
 * \param[in] 无
 * \return 无
 */
void adc_irq_handler(void)
{
	uint32_t status = GTRY_ADC->ADC_INT_STS;
	if(adc_drv_int_callback != NULL)
	{
		adc_drv_int_callback(status);
	}
	GTRY_ADC->ADC_INT_CLR = status;
}

/**
 * @brief	ADC模块执行校准操作
 * @param[in] 无
 * @retval 无
 */
void adc_calibrate(void)
{
	GTRY_ADC->ADC_CONFIG |= ADC_ENABLE;
	GTRY_ADC->ADC_COMP_CFG = 0x1;
	while(GTRY_ADC->ADC_COMP_CFG);
	GTRY_ADC->ADC_INT_CLR |= ADC_INT_CAP_MASK;
	GTRY_ADC->ADC_CALIB_CFG = 0x1;
	while(GTRY_ADC->ADC_CALIB_CFG);
	GTRY_ADC->ADC_INT_CLR |= ADC_INT_COMP_MASK;

	uint64 c_val = 0x0;
	for(uint32_t i = 0x0;i<14;i++)
	{
		c_val += GTRY_ADC->ADC_CAP_WEIGHT[i];
	}
	for(uint32_t i = 0x0;i<14;i++)
	{
		uint64 c_val_temp = GTRY_ADC->ADC_CAP_WEIGHT[i];
		c_val_temp <<= 12;
		c_val_temp *= 4095;
		uint64 c_val_temp1 = c_val_temp;
		c_val_temp /= c_val;
		if((c_val_temp1*10/c_val%10)>=5)
		{
			c_val_temp++;
		}
		GTRY_ADC->ADC_CAP_WEIGHT[i] = (uint32_t)(c_val_temp&0xfffffff);
	}
}

/**
 * @brief	ADC模块初始化函数
 * @param[in] mode：ADC转换模式，包括单/多通道单次/连续转换模式，\ref adc_mode_t
 * @param[in] chl_sel：共30通道，用ADC_CHx_PAx_MASK宏来设置，可通过'或'操作使能多个通道
 * @param[in] cvt_time：ADC通道转换时间，配置值：0~65535，实际转换时间等于'配置值+1'个ADC时钟周期
 * @param[in] buffer_en：VREF BUFFER使能配置，'0'不使能，'非0'使能
 * @retval 无
 */
void adc_init(adc_mode_t mode, uint32_t chl_sel, uint32_t cvt_time, uint32_t buffer_en)
{
	adc_set_channel(chl_sel);
	GTRY_ADC->ADC_CONFIG = ((cvt_time<=0xffff?cvt_time:0xffff)<<ADC_CFG_SAR_CFG_POS) | (buffer_en?ADC_VREF_BUFFER_ON:0) |
							(mode<<ADC_CFG_REPEAT_SCAN_POS) | (((mode<<ADC_CFG_REPEAT_SCAN_POS)&ADC_SAMPLE_REPEAT)?ADC_DMA_EN:0)
							| ADC_SAR_SAVE_EN;
	if((mode<<ADC_CFG_REPEAT_SCAN_POS)&ADC_SAMPLE_REPEAT)
	{
		dma_initialize(DMA1_PERIPH);
	}
	adc_enable();
}

/**
 * @brief	ADC模块DMA配置函数
 * @param[in] dma_burst：DMA burst访问长度 \ref adc_dma_burst_t
 * @param[in] res_buf：DMA模式下ADC转换输出的BUF指针
 * @param[in] size：DMA读取数据个数，通常设置为使能通道个数*重复转换次数
 * @retval 无
 */
void adc_dma_cfg(adc_dma_burst_t dma_burst, void const *res_buf, uint32_t size)
{
	GTRY_ADC->ADC_CONFIG &= ~(0x3UL<<ADC_CFG_DMA_BURST_POS);
	GTRY_ADC->ADC_CONFIG |= (dma_burst<<ADC_CFG_DMA_BURST_POS);
	GTRY_ADC->ADC_DMA_SIZE = size;
	dma_channel_configure(DMA1_PERIPH, GTRY_ADC_DMA_RX_CH, (unsigned long)(&GTRY_ADC->ADC_READ_DATA), (unsigned long)res_buf|0xa0000000, size,
							 DMA_CH_CTRL_SBINF(DMA_INF_IDX0) | DMA_CH_CTRL_DBINF(DMA_INF_IDX0) |
							 DMA_CH_CTRL_SBSIZE(dma_burst) |
							 //DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
							 DMA_CH_CTRL_SWIDTH(DMA_WIDTH_HALFWORD) | DMA_CH_CTRL_DWIDTH(DMA_WIDTH_HALFWORD) |
							 DMA_CH_CTRL_SMODE_HANDSHAKE | DMA_CH_CTRL_SRCADDR_FIX | DMA_CH_CTRL_DSTADDR_INC |
							 DMA_CH_CTRL_SRCREQ(GTRY_ADC_DMA_RX_REQID) | DMA_CH_CTRL_EN_INTABT | DMA_CH_CTRL_EN_INTERR |
							 DMA_CH_CTRL_EN_INTTC | DMA_CH_CTRL_ENABLE, NULL);
}

/**
 * @brief	ADC模块获取使能通道数
 * @param[in] 无
 * @retval 返回当前ADC使能的通道数
 */
uint32_t adc_get_ch_num(void)
{
	uint32_t ch_num = 0x0;
	for(uint32_t i = 0x0;i<ADC_CHL_NUM;i++)
	{
		if((GTRY_ADC->ADC_SWITCH_CFG>>i)&0x1)
		{
			ch_num++;
		}
	}
	return ch_num;
}

