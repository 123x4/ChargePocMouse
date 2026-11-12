/*
 *	gtry_argb_drv.c
 *
 *	Created on: 2025.10.23
 *
 */
#include "gtry_gpio_drv.h"
#include "gtry_argb_drv.h"
#include "gtry_dma_drv.h"

p_argb_int_cb argb_drv_int_callback = NULL;		/** ARGB中断回调函数入口 */

/**
 * \brief	ARGB中断初始化
 * \param[in] int_mask：需要配置的中断MASK
 * \param[in] argb_int_cb：ARGB中断回调函数入口
 * \return 无
 */
void argb_int_init(uint32_t int_mask, p_argb_int_cb argb_int_cb)
{
	GTRY_ARGB->ARGB_STATUS = ARGB_ST_FRAME_END | ARGB_ST_FIFO_ERROR;
	GTRY_ARGB->ARGB_INT_EN = int_mask;

	__nds__plic_set_priority(IRQ_ARGB_SOURCE, 1);
	__nds__plic_enable_interrupt(IRQ_ARGB_SOURCE);

	argb_drv_int_callback = argb_int_cb;
}

/**
 * \brief	ARGB中断服务函数
 * \param[in] 无
 * \return 无
 */
void argb_irq_handler(void)
{
	uint32_t status = GTRY_ARGB->ARGB_STATUS;
	if(argb_drv_int_callback != NULL)
	{
		argb_drv_int_callback(status);
	}
	GTRY_ARGB->ARGB_STATUS = status;
}

/**
 * \brief	ARGB初始化
 * \param[in] format：发送数据模式 \ref argb_format_t
 * \param[in] mode：模式配置 \ref argb_mode_t
 * \param[in] argb_en：模块使能/失能配置 \ref argb_enable_t
 * \return 无
 */
void argb_init(argb_format_t format, argb_mode_t mode, argb_enable_t argb_en)
{
	GTRY_ARGB->ARGB_CTRL &= ~((0x3<<ARGB_CTRL_FORMAT_POS) | (0x1<<ARGB_CTRL_ENBALE_POS) | (0x1<<ARGB_CTRL_MODE_POS));
	GTRY_ARGB->ARGB_CTRL |= ((format<<ARGB_CTRL_FORMAT_POS) | (argb_en<<ARGB_CTRL_ENBALE_POS) | (mode<<ARGB_CTRL_MODE_POS));
}

/**
 * \brief	ARGB LED配置
 * \param[in] led_num：每条通道的LED灯数量，最大为8192
 * \param[in] data_byte：LED灯的显示数据配置，单位BYTE
 * \param[in] t0h_ns：LED灯传输0的高电平宽度配置，单位ns
 * \param[in] t1h_ns：LED灯传输1的高电平宽度配置，单位ns
 * \param[in] rst_ns：LED灯传输RESET的高电平宽度配置，单位ns
 * \return 无
 */
void argb_led_cfg(uint32_t led_num, uint32_t data_byte, uint32_t t0h_ns , uint32_t t1h_ns, uint32_t rst_ns)
{
	GTRY_ARGB->ARGB_LED_NUM = led_num-1;
	GTRY_ARGB->ARGB_LED_BYTE = data_byte-1;
	GTRY_ARGB->ARGB_T0H_WIDTH = (t0h_ns*(g_pclk_freq/1000000)/1000)-1;
	GTRY_ARGB->ARGB_T1H_WIDTH = (t1h_ns*(g_pclk_freq/1000000)/1000)-1;
	GTRY_ARGB->ARGB_BIT_WIDTH = (GTRY_ARGB->ARGB_T0H_WIDTH + GTRY_ARGB->ARGB_T1H_WIDTH)+1;
	GTRY_ARGB->ARGB_RST_WIDTH = (rst_ns*(g_pclk_freq/1000000)/1000)-1;
}

/**
 * \brief	ARGB通道（组）初始化
 * \param[in] group_mask：需要使能的GROUP_MASK，见'ARGB_GROUP'前缀宏
 * \param[in] ch_sel：组内通道使能，见'ARGB_CH'前缀宏
 * \return 无
 */
void argb_group_ch_cfg(uint32_t group_mask, uint32_t ch_sel)
{
	GTRY_ARGB->ARGB_CHL_CFG = (group_mask<<ARGB_CHNL_CFG_CHMUX_POS) | (ch_sel<<ARGB_CHNL_CFG_CHEN_POS);
	for(uint32_t group_flag = 0x0; group_flag < ARGB_GROUP_MAX; group_flag++)
	{
		if((group_mask>>group_flag)&0x1)
		{
			for(uint32_t ch_flag = 0x0; ch_flag <= ch_sel; ch_flag++)
			{
				GTRY_GPIO->GPIO_CFG[group_flag*ARGB_CH_MAX+ch_flag] &=
							~((RY_MAX_REUSE << GPIO_CFG_REUSE_POS) | (0x3UL << GPIO_CFG_DIR_POS));
				GTRY_GPIO->GPIO_CFG[group_flag*ARGB_CH_MAX+ch_flag] |=
							((RY_ARGB_REUSE << GPIO_CFG_REUSE_POS) | (RY_GPIO_OUT_EN << GPIO_CFG_DIR_POS));
			}
		}
	}
}

/**
 * \brief	ARGB公用数据位宽配置
 * \param[in] com_bits：公用数据位宽配置，单位bit，最大支持128bit即4个32bit数据
 * \return 无
 */
void argb_combits_cfg(uint32_t com_bits)
{
	if(com_bits>0x80)
	{
		return;
	}
	GTRY_ARGB->ARGB_COM_BITS = com_bits;
}

/**
 * \brief	ARGB公用数据配置
 * \param[in] data：公用数据指针
 * \return 无
 */
void argb_comdata_cfg(uint32_t data_no , uint32_t data)
{
	GTRY_ARGB->ARGB_COM_DATA[data_no] = data;
}

/**
 * \brief	ARGB向FIFO内写入单个数据
 * \param[in] data：写入数据
 * \return 无
 */
void argb_trans_one_data(uint32_t data)
{
	if(!(GTRY_ARGB->ARGB_STATUS&ARGB_ST_FIFO_FULL))
	{
		GTRY_ARGB->ARGB_FIFO_ADDR = data;
	}
}

/**
 * \brief	ARGB清除FIFO内数据
 * \param[in] 无
 * \return 无
 */
void argb_fifo_clear(void)
{
	GTRY_ARGB->ARGB_FIFO_CLR = 0x1;
	while(!(GTRY_ARGB->ARGB_STATUS&ARGB_ST_FIFO_EMPTY));
}

/**
 * \brief ARGB DMA传输初始化
 * \param[in] dma_en：DMA使能/失能
 * \param[in] dma_req_num：DMA请求阈值配置
 * \return 无
 */
void argb_dma_cfg(uint32_t dma_en, uint32_t dma_req_num)
{
	GTRY_ARGB->ARGB_CTRL &= ~((0x1f<<ARGB_CTRL_DMA_REQ_NUM_POS) | (0x1<<ARGB_CTRL_DMA_EN_POS));
	if(dma_en)
	{
		GTRY_ARGB->ARGB_CTRL |= ((dma_req_num<<ARGB_CTRL_DMA_REQ_NUM_POS) | (ARGB_CTRL_DMA_EN<<ARGB_CTRL_DMA_EN_POS));
		dma_initialize(DMA1_PERIPH);
	}
	else
	{
		GTRY_ARGB->ARGB_CTRL |= ((dma_req_num<<ARGB_CTRL_DMA_REQ_NUM_POS) | (ARGB_CTRL_DMA_DISEN<<ARGB_CTRL_DMA_EN_POS));
	}
}

/**
 * @brief	ARGB模块DMA数据传输配置
 * @param[in] *data：传输数据指针
 * @param[in] num：传输数据个数
 * @return 无
 */
void argb_dma_trans_data(uint32_t *data , uint32_t num)
{
	dma_channel_configure (DMA1_PERIPH, GTRY_ARGB_DMA_RX_CH,(unsigned long)data|0xa0000000,(unsigned long)(&GTRY_ARGB->ARGB_FIFO_ADDR),num,
						DMA_CH_CTRL_SBINF(DMA_INF_IDX1) | DMA_CH_CTRL_DBINF(DMA_INF_IDX1) | DMA_CH_CTRL_SBSIZE(DMA_BSIZE_8)	 |
						DMA_CH_CTRL_SWIDTH(DMA_WIDTH_WORD) | DMA_CH_CTRL_DWIDTH(DMA_WIDTH_WORD) |
						DMA_CH_CTRL_DMODE_HANDSHAKE | DMA_CH_CTRL_SRCADDR_INC	| DMA_CH_CTRL_DSTADDR_FIX |
						DMA_CH_CTRL_DSTREQ(GTRY_ARGB_DMA_RX_REQID) | DMA_CH_CTRL_EN_INTABT | DMA_CH_CTRL_EN_INTERR |
						DMA_CH_CTRL_EN_INTTC | DMA_CH_CTRL_ENABLE,NULL);
}

/**
 * \brief	获取ARGB模块状态
 * \param[in] 无
 * \return 返回状态，见'ARGB_ST'前缀宏
 */
uint32_t argb_get_status(void)
{
	return GTRY_ARGB->ARGB_STATUS;
}

/**
 * \brief	清除ARGB模块状态
 * \param[in] 无
 * \return 无
 */
void argb_clear_status(void)
{
	GTRY_ARGB->ARGB_STATUS = ARGB_ST_FRAME_END | ARGB_ST_FIFO_ERROR;
}

/**
 * \brief	启动ARGB模块发送数据
 * \param[in] 无
 * \return 无
 */
void argb_start(void)
{
	GTRY_ARGB->ARGB_START = 0x1;
}

/**
 * \brief	停止ARGB模块
 * \param[in] 无
 * \return 无
 */
void argb_stop(void)
{
	GTRY_ARGB->ARGB_CTRL &= ~(RY_ARGB_EN<<ARGB_CTRL_ENBALE_POS);
}
