/*
 *	gtry_pwm_drv.c
 *
 *	Created on: 2025.10.23
 *
 */

#include "gtry_pwm_drv.h"
#include "gtry_gpio_drv.h"
#include "gtry_dma_drv.h"

p_pwm_int_cb pwm_drv_int_callback = NULL;			/** PWM中断回调函数入口 */

/**
 * \brief	PWM中断初始化
 * \param[in] int_mask：需要配置的中断MASK
 * \param[in] pwm_int_cb：PWM中断回调函数入口
 * \return 无
 */
void pwm_int_init(uint32_t int_mask, p_pwm_int_cb pwm_int_cb)
{
	GTRY_PWM->PWM_INT_MASK = int_mask;

	__nds__plic_set_priority(IRQ_PWM_SOURCE, 1);
	__nds__plic_enable_interrupt(IRQ_PWM_SOURCE);

	pwm_drv_int_callback = pwm_int_cb;
}

/**
 * \brief	PWM中断服务函数
 * \param[in] 无
 * \return 无
 */
void pwm_irq_handler(void)
{
	uint32_t status = GTRY_PWM->PWM_INT_STS;
	if(pwm_drv_int_callback != NULL)
	{
		pwm_drv_int_callback(status);
	}
	GTRY_PWM->PWM_INT_CLR = status;
}

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
uint8_t pwm_single_config(pwm_reuse_t reuse, uint32_t pwmch, pwm_mode_t mode, uint32_t totalcycle, uint32_t dutytime, uint8_t ploarity)
{
	uint32_t fclk_factor = g_sysclk_freq/1000000;

	if((~(PWM_GROUP_MASK|PWM_CH_MASK)) & pwmch)
	{
		return 1;
	}

	if(totalcycle*fclk_factor > PWM_MAX_CYCLE || dutytime > totalcycle || totalcycle == 0)
	{
		return 2;
	}

	uint32_t pwm_group = (pwmch&PWM_GROUP_MASK)>>PWM_GROUP_POS;
	uint32_t pwm_channel = (pwmch&PWM_CH_MASK);
	uint32_t pwm_mask = 1<<((PWM_CH_NUMBER*pwm_group)+pwm_channel);

	GTRY_GPIO->GPIO_CFG[(PWM_CH_NUMBER*pwm_group)+pwm_channel+reuse*32] &= ~(RY_MAX_REUSE<<GPIO_CFG_REUSE_POS);
	GTRY_GPIO->GPIO_CFG[(PWM_CH_NUMBER*pwm_group)+pwm_channel+reuse*32] |= (RY_PWM_REUSE<<GPIO_CFG_REUSE_POS);

	pwm_mode_set(pwm_group, mode);
	pwm_update_en(1);
	pwm_set_cycle(pwm_group, totalcycle*fclk_factor/(((GTRY_PWM->PWM_SCALER >> (pwm_group<<3)) & 0xff) + 1)-1);
	pwm_set_duty(pwmch, dutytime*fclk_factor/(((GTRY_PWM->PWM_SCALER >> (pwm_group<<3)) & 0xff) + 1));

	if(ploarity)
	{
		pwm_polarity_pos(pwm_mask);
	}
	else
	{
		pwm_polarity_neg(pwm_mask);
	}
	return 0;
}

/**
 * \brief	PWM单通道输出关闭
 * \param[in] reuse：PWM复用IO配置 \ref pwm_reuse_t
 * \param[in] pwmch：PWM通道标识
 * \return 0表示配置成功；非0表示配置失败
 */
uint8_t pwm_single_disable(pwm_reuse_t reuse, uint32_t pwmch)
{
	if((~(PWM_GROUP_MASK|PWM_CH_MASK)) & pwmch)
	{
		return 1;
	}

	uint32_t pwm_group = (pwmch&PWM_GROUP_MASK)>>PWM_GROUP_POS;
	uint32_t pwm_channel = (pwmch&PWM_CH_MASK);
	uint32_t pwm_mask = 1<<((PWM_CH_NUMBER*pwm_group)+pwm_channel);

	pwm_chl_disable(pwm_mask);
	if(reuse == RY_PWM_REUSE_P0_31)
	{
		GTRY_GPIO->GPA_CFG[(PWM_CH_NUMBER*pwm_group)+pwm_channel] &= ~(RY_MAX_REUSE<<GPIO_CFG_REUSE_POS);
	}
	else
	{
		if(((PWM_CH_NUMBER*pwm_group)+pwm_channel+32) < 48)
		{
			GTRY_GPIO->GPA_CFG[(PWM_CH_NUMBER*pwm_group)+pwm_channel+32] &= ~(RY_MAX_REUSE<<GPIO_CFG_REUSE_POS);
		}
		else
		{
			GTRY_GPIO->GPB_CFG[(PWM_CH_NUMBER*pwm_group)+pwm_channel] &= ~(RY_MAX_REUSE<<GPIO_CFG_REUSE_POS);
		}
	}
	return 0;
}

/**
 * \brief	PWM STOP模式配置
 * \param[in] pwm_mask：PWM通道标识MASK
 * \param[in] ploarity：PWM STOP极性
 * \param[in] acmp_stop_en：PWM ACMP急停使能
 * \param[in] io_stop_en：PWM IO急停使能
 * \param[in] stop_pin：PWM急停IO
 * \return 无
 */
void pwm_stop_cfg(uint32_t pwm_mask, uint8_t ploarity, uint8_t acmp_stop_en, uint8_t io_stop_en, uint32_t stop_pin)
{
	GTRY_PWM->PWM_STOP &= ~pwm_mask;
	GTRY_PWM->PWM_STOP |= (ploarity ? pwm_mask : 0);
	GTRY_PWM->PWM_STOP_MODE &= ~0x6fUL;
	GTRY_PWM->PWM_STOP_MODE |= ((pwm_mask & 0xff)?(PWM_G0_STOP_MASK):(0))|
								(((pwm_mask >> 8) & 0xff)?(PWM_G1_STOP_MASK):(0))|
								(((pwm_mask >> 16 )& 0xff)?(PWM_G2_STOP_MASK):(0))|
								(((pwm_mask >> 24 )& 0xff)?(PWM_G3_STOP_MASK):(0));
	GTRY_PWM->PWM_STOP_MODE |= (acmp_stop_en ? PWM_ACMP_STOP_EN : 0);
	GTRY_PWM->PWM_STOP_MODE |= (io_stop_en ? PWM_IO_STOP_EN : 0);
	if(io_stop_en)
	{
		gpio_cfg(stop_pin, RY_PWMIO_STOP_REUSE, RY_GPIO_DRIVE4, RY_GPIO_NOPULL, RY_GPIO_HZ, RY_GPIO_INPUT_EN);
	}
	pwm_stop_en();
}

/**
 * \brief	PWM DMA模式配置
 * \param[in] pwm_ch：PWM通道标识
 * \param[in] dma_cnt：PWM产生DMA请求的计数值
 * \param[in] duty_buf：DMA模式下PWM输出的BUF指针
 * \param[in] num：DMA读取数据个数
 * \return 无
 */
void pwm_dma_cfg(uint32_t pwm_ch, uint32_t dma_cnt, uint32_t *duty_buf, uint32_t num)
{
	uint32_t pwm_group = (pwm_ch & PWM_GROUP_MASK) >> PWM_GROUP_POS;
	uint32_t pwm_channel = pwm_ch & PWM_CH_MASK;
	uint32_t pwm_mask = (PWM_CH_NUMBER*pwm_group)+pwm_channel;

	GTRY_PWM->PWM_MODESEL &= ~PWM_DMA_MODE_EN;
	GTRY_PWM->PWM_DMA_CONFIG = 0;
	dma_initialize(DMA1_PERIPH);
	dma_channel_configure(DMA1_PERIPH, GTRY_PWM_DMA_RX_CH, (unsigned long)duty_buf|0xa0000000,
								   (unsigned long)(&(GTRY_PWM->PWM_COMPARE[pwm_mask])), num,
								   DMA_CH_CTRL_SBINF(DMA_INF_IDX1) | DMA_CH_CTRL_DBINF(DMA_INF_IDX1) |
								   DMA_CH_CTRL_SBSIZE(DMA_BSIZE_32) | DMA_CH_CTRL_SWIDTH(DMA_WIDTH_WORD) |
								   DMA_CH_CTRL_DWIDTH(DMA_WIDTH_WORD) | DMA_CH_CTRL_DMODE_HANDSHAKE |
								   DMA_CH_CTRL_SRCADDR_INC | DMA_CH_CTRL_DSTADDR_INC |
								   DMA_CH_CTRL_DSTREQ(GTRY_PWM_DMA_RX_REQID) |
								   DMA_CH_CTRL_EN_INTABT | DMA_CH_CTRL_EN_INTERR |
								   DMA_CH_CTRL_EN_INTTC | DMA_CH_CTRL_ENABLE, NULL);
	GTRY_PWM->PWM_DMA_CONFIG = ((pwm_group << PWM_DMA_CH_SEL_POS) | (dma_cnt&0xffff));
	GTRY_PWM->PWM_MODESEL |= PWM_DMA_MODE_EN;
}

/**
 * \brief	PWM ADC模式配置
 * \param[in] pwm_mask：PWM通道标识MASK
 * \param[in] trig_mode：PWM沿触发ADC采样模式 \ref pwm_adc_trig_t
 * \return 无
 */
void pwm_adc_cfg(uint32_t pwm_mask, pwm_adc_trig_t trig_mode)
{
	GTRY_PWM->PWM_MODESEL |= PWM_ADC_COMP_EN;
	GTRY_PWM->PWM_ADC_SEL = trig_mode;
	GTRY_PWM->PWM_ADC_MASK = pwm_mask;
}
