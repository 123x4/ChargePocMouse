/*
 *	gtry_timer_drv.c
 *
 *	Created on: 2025.10.23
 *
 */

#include "gtry_timer_drv.h"
#include "gtry_gpio_drv.h"
#include "gtry_dma_drv.h"

p_timer_int_cb timer_drv_int_callback[3] = {NULL, NULL, NULL};			/** TIMER?????????????*/

/**
 * @brief	TIMER配置指定定时器模块中断处理
 * @param[in] timer_no：定时器通道标识号
 * @param[in] int_mask：中断使能MASK，见'TIMER_INT'前缀宏，仅可以使能PWM/CAP/CC中断的其中一种，不能同时使能
 * @param[in] timer_int_callback：中断回调函数入口
 * @return 无
 */
void timer_int_init(uint32_t timer_no, uint32_t int_mask, p_timer_int_cb timer_int_callback)
{
	GTRY_TIMER->INT_STS_CLEAR = TIMER_INT_ALL_MASK;

	if((int_mask&(TIMER_INT_CAP_MASK))||(int_mask&TIMER_INT_PWM_MASK))
	{
		GTRY_TIMER->INT_EN_MASK |= (int_mask<<timer_no);
	}
	else
	{
		GTRY_TIMER->INT_EN_MASK |= (int_mask<<(timer_no*TIMER_CC_NUM));
	}

	__nds__plic_set_priority(IRQ_TIMER_SOURCE, 1);
	__nds__plic_enable_interrupt(IRQ_TIMER_SOURCE);

	if (timer_no < 3) { timer_drv_int_callback[timer_no] = timer_int_callback; }
}

/**
 * \brief	TIMER中断服务函数
 * \param[in] 无
 * \return 无
 */
void timer_irq_handler(void)
{
	uint32_t status = GTRY_TIMER->TIM_INT_STS;
	for (uint8_t timer_no = 0; timer_no < 3; timer_no++)
	{
		if (timer_drv_int_callback[timer_no] != NULL)
	{
			timer_drv_int_callback[timer_no](status & TIMER_INT_ALL_MASK);
		}
	}
	GTRY_TIMER->INT_STS_CLEAR = status;
}

/**
 * @brief	TIMER模块初始化
 * @param[in] timer_no：定时器模块标识号
 * @param[in] presclar：timer时钟分频系数\ref timer_bit_mode_t
 * @param[in] mode：TIMER模式 \ref timer_mode_t
 * @param[in] bitmode：timer位宽
 * @retval 无
 */
void timer_init(uint32_t timer_no, uint32_t presclar, timer_mode_t mode, timer_bit_mode_t bitmode)
{
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= TIMER_STOP;
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= TIMER_CC_CLEAR;
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= TIMER_CLEAR;
	GTRY_TIMER->TIMn_CONFIG[timer_no] &= ~(0x3UL<<TIMER_CFG_MODE_POS);
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= (mode<<TIMER_CFG_MODE_POS);
	GTRY_TIMER->INT_STS_CLEAR = TIMER_INT_ALL_MASK;
	GTRY_TIMER->TIM_SCALER = presclar-1;
	GTRY_TIMER->TIM_BIT_MODE = bitmode;
}

/**
 * @brief TIMER模块关联触发配置
 * @param[in] timer_no：定时器模块标识号
 * @param[in] id：定时器模块比较器标识号
 * @param[in] short_type：关联配置类型，见'TIMER_SHORT'前缀宏
 * @return 无
 */
void timer_set_shorts(uint32_t timer_no, uint32_t id, uint32_t short_type)
{
	if(short_type&TIMER_SHORT_NULL_MASK)
	{
		GTRY_TIMER->TIM_CC_CLEAR  &= ~(0x1UL<<(id+(timer_no*TIMER_CC_NUM)));
		GTRY_TIMER->TIM_STOP  &= ~(0x1UL<<(id+(timer_no*TIMER_CC_NUM)));
		GTRY_TIMER->TIM_STOP  &= ~(0x1UL<<(timer_no+TIMER_CAP_POS));
		return;
	}
	if(short_type&TIMER_SHORT_CC_CLR_MASK)
	{
		GTRY_TIMER->TIM_CC_CLEAR  |= (0x1UL<<(id+(timer_no*TIMER_CC_NUM)));
	}
	if(short_type&TIMER_SHORT_CC_STOP_MASK)
	{
		GTRY_TIMER->TIM_STOP  |= (0x1UL<<(id+(timer_no*TIMER_CC_NUM)));
	}
	if(short_type&TIMER_SHORT_CAP_STOP_MASK)
	{
		GTRY_TIMER->TIM_STOP  |=  (0x1UL<<(timer_no+TIMER_CAP_POS));
	}
}

/**
 * @brief	TIMER模块捕获模式配置
 * @param[in] timer_no：定时器模块标识号
 * @param[in] cap_io：定时器捕获IO通道，见'TIMERx_CAP_IO'前缀宏，其中x为定时器模块标识号
 * @param[in] cap_sense：捕获敏感 \ref timer_cap_sense_t
 * @param[in] pwm_en：PWM捕获是否使能 \ref timer_pwm_en_t
 * @return 无
 */
void  timer_cap_cfg(uint32_t timer_no, uint32_t cap_io, timer_cap_sense_t cap_sense, timer_pwm_en_t pwm_en)
{
	*(__IO uint32_t *)(GTRY_GPIO_CFG[(cap_io & GPIO_GROUP_MASK)>>GPIO_GROUP_POS]+ (cap_io & GPIO_NUM_MASK)*4) &= ~(RY_MAX_REUSE << GPIO_CFG_REUSE_POS);
	*(__IO uint32_t *)(GTRY_GPIO_CFG[(cap_io & GPIO_GROUP_MASK)>>GPIO_GROUP_POS]+ (cap_io & GPIO_NUM_MASK)*4) |=
								((RY_TIMER_REUSE << GPIO_CFG_REUSE_POS) | (RY_GPIO_INPUT_EN << GPIO_CFG_IN_EN_POS));
	GTRY_TIMER->TIMn_CONFIG[timer_no] &= ~((0x1UL<<TIMER_CFG_PWM_POS) | (0x3UL<<TIMER_CFG_CAP_MODE_POS));
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= ((pwm_en<<TIMER_CFG_PWM_POS) | (cap_sense<<TIMER_CFG_CAP_MODE_POS));
}

/**
 * @brief	timer模块重载模式配置
 * @param[in] timer_no：定时器模块标识号
 * @param[in] reload_val：重载值
 * @return 无
 */
void timer_reload_cfg(uint32_t timer_no, uint32_t reload_val)
{
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= (RY_TIMER_RELOAD_EN<<TIMER_CFG_RELOAD_POS);
	GTRY_TIMER->TIMn_RELOAD[timer_no] = reload_val;
}

/**
 * @brief	TIMER模块滤波配置
 * @param[in] timer_no：定时器模块标识号
 * @param[in] filter_val：滤波参数
 * @return 无
 */
void timer_filter_cfg(uint32_t timer_no, uint32_t filter_val)
{
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= (RY_TIMER_FILTER_EN<<TIMER_CFG_FILTER_POS);
	GTRY_TIMER->TIM_SCALER &= 0xff;
	GTRY_TIMER->TIM_SCALER |= (filter_val<<TIMER_FILTER_VALUE_POS);
}

/**
 * @brief	timer模块COMP模式配置
 * @param[in] timer_no：定时器模块标识号
 * @param[in] comp_no：比较器模块标识号
 * @param[in] cap_sense：timer捕获比较器的边沿配置 \ref comp_cap_sense_t
 * @return 无
 */
void timer_comp_cfg(uint32_t timer_no, uint32_t comp_no, comp_cap_sense_t cap_sense)
{
	if(comp_no == COMP_NUMBER1)
	{
	    GTRY_ACMP_OPA->ACMP1_CSR &= ~((0x7UL<<COMP_TIMER_SEL_POS) | (0x3UL<<COMP_TIMER_CAP_SEN_POS) | (0x1UL<<COMP_ENABLE_POS));
	    GTRY_ACMP_OPA->ACMP1_CSR |= (((timer_no+1)<<COMP_TIMER_SEL_POS) | (cap_sense<<COMP_TIMER_CAP_SEN_POS) | (COMP_ENABLE<<COMP_ENABLE_POS));
	    gpio_cfg(COMP1_FORWARD_INPUT_IO,RY_ACMP_REUSE,RY_GPIO_DRIVE4,RY_GPIO_NOPULL,RY_GPIO_HZ,RY_GPIO_INPUT_EN);	//配置comp1正相输入端
	    gpio_cfg(COMP1_REVERSE_INPUT_IO,RY_ACMP_REUSE,RY_GPIO_DRIVE4,RY_GPIO_NOPULL,RY_GPIO_HZ,RY_GPIO_INPUT_EN);		//配置comp1反相输入端
	}
	else
	{
	    GTRY_ACMP_OPA->ACMP2_CSR &= ~((0x7UL<<COMP_TIMER_SEL_POS) | (0x3UL<<COMP_TIMER_CAP_SEN_POS) | (0x1UL<<COMP_ENABLE_POS));
	    GTRY_ACMP_OPA->ACMP2_CSR |= (((timer_no+1)<<COMP_TIMER_SEL_POS) | (cap_sense<<COMP_TIMER_CAP_SEN_POS) | (COMP_ENABLE<<COMP_ENABLE_POS));
	    gpio_cfg(COMP2_FORWARD_INPUT_IO,RY_ACMP_REUSE,RY_GPIO_DRIVE4,RY_GPIO_NOPULL,RY_GPIO_HZ,RY_GPIO_INPUT_EN);	//配置comp2正相输入端
	    gpio_cfg(COMP2_REVERSE_INPUT_IO,RY_ACMP_REUSE,RY_GPIO_DRIVE4,RY_GPIO_NOPULL,RY_GPIO_HZ,RY_GPIO_INPUT_EN);		//配置comp2反相输入端
	}
	GTRY_TIMER->TIMn_CONFIG[timer_no] |= (RY_TIMER_COMP_EN<<TIMER_CFG_COMP_POS);
}

/**
 * @brief	TIMER模块DMA模式配置
 * @param[in] timer_no：定时器模块标识号
 * @return 无
 */
void timer_dma_cfg(uint32_t timer_no, uint32_t id)
{
	GTRY_TIMER->TIM_DMA |= TIMER_DMA_REQ_CLR;
	GTRY_TIMER->TIM_DMA = (((timer_no+1)<<TIMER_DMA_SEL_POS) | ((id+1)<<TIMER_DMA_CC_SEL_POS));
}

/**
 * @brief	TIMER模块主动发起DMA请求
 * @param[in] 无
 * @return 无
 */
void timer_dma_req_en(void)
{
	GTRY_TIMER->TIM_DMA |= TIMER_DMA_REQ_EN;
}

/**
 * @brief	TIMER模块清除DMA请求
 * @param[in] 无
 * @return 无
 */
void timer_dma_req_clr(void)
{
	GTRY_TIMER->TIM_DMA |= TIMER_DMA_REQ_CLR;
}

/**
 * @brief	TIMER模块DMA数据传输配置
 * @param[in] addr：传输目标地址
 * @param[in] *data：传输数据指针
 * @param[in] num：传输数据个数
 * @return 无
 */
void timer_dma_trans_data(uint32_t addr, uint32_t *data, uint32_t num)
{
	dma_channel_configure(DMA1_PERIPH, TIMER_DMA_CH,(unsigned long)data|0xa0000000,(unsigned long)addr,num,
							DMA_CH_CTRL_SBINF(DMA_INF_IDX1) | DMA_CH_CTRL_DBINF(DMA_INF_IDX1) | DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
							DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE) | DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE) |
							DMA_CH_CTRL_DMODE_HANDSHAKE | DMA_CH_CTRL_SRCADDR_INC | DMA_CH_CTRL_DSTADDR_FIX |
							DMA_CH_CTRL_DSTREQ(TIMER_DMA_REQID) | DMA_CH_CTRL_EN_INTABT | DMA_CH_CTRL_EN_INTERR |
							DMA_CH_CTRL_EN_INTTC | DMA_CH_CTRL_ENABLE, NULL);
}
