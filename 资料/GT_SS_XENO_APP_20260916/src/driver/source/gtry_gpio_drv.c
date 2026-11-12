/*
 *	gtry_gpio_drv.c
 *
 *	Created on: 2025.10.23
 *
 */

#include "gtry_gpio_drv.h"

__IO uint32_t GTRY_GPIO_CFG[GPIO_GROUP_MAX] = {RY_GPIO_PA_CFG_BASE1, RY_GPIO_PA_CFG_BASE2, RY_GPIO_PB_CFG_BASE};
__IO uint32_t GTRY_GPIO_SENSE_CFG[GPIO_GROUP_MAX] = {RY_GPIO_PA_SENSE_CFG_BASE1, RY_GPIO_PA_SENSE_CFG_BASE2, RY_GPIO_PB_SENSE_CFG_BASE};

p_gpio_int_cb gpio_drv_int_callback = NULL;							/** GPIO中断回调函数入口 */

/**
 * \brief	GPIO中断初始化
 * \param[in] int_group_en_mask： 需要配置的引脚所在GPIO GROUP标识MASK
 * \param[in] gpio_int_cb：GPIO中断回调函数入口
 * \return 无
 */
void gpio_int_init(p_gpio_int_cb gpio_int_cb)
{
	GTRY_GPIO->GPIO_INT_EN[PA_GROUP0] = 0x0;
	GTRY_GPIO->GPIO_INT_EN[PA_GROUP1] = 0x0;
	GTRY_GPIO->GPIO_INT_EN[PB_GROUP] = 0x0;

	GTRY_GPIO->GPIO_INT_STS[PA_GROUP0] = 0xffffffff;
	GTRY_GPIO->GPIO_INT_STS[PA_GROUP1] = 0xffff;
	GTRY_GPIO->GPIO_INT_STS[PB_GROUP] = 0xffff;

	__nds__plic_set_priority(IRQ_GPIO_SOURCE, 1);
	__nds__plic_enable_interrupt(IRQ_GPIO_SOURCE);

	gpio_drv_int_callback = gpio_int_cb;
}

/**
 * \brief	GPIO中断服务函数
 * \param[in] 无
 * \return 无
 */
void gpio_irq_handler(void)
{
	uint32_t int_io_mask = 0;

	for(uint32_t i = 0; i < GPIO_GROUP_MAX; i++)
	{
		if(GTRY_GPIO->GPIO_INT_STS[i])
		{
			int_io_mask = GTRY_GPIO->GPIO_INT_STS[i];
			if(gpio_drv_int_callback != NULL)
			{
				gpio_drv_int_callback((i<<GPIO_GROUP_POS), int_io_mask);
			}
			GTRY_GPIO->GPIO_INT_STS[i] = int_io_mask;
		}
	}
}

void jtag_io_multi_config(void)
{
	for(uint32_t i = 0; i < 5; i++)
	{
		GTRY_GPIO->GPIO_CFG[i] &= ~(RY_MAX_REUSE << GPIO_CFG_REUSE_POS);
		GTRY_GPIO->GPIO_CFG[i] |= ((RY_JTAG_REUSE << GPIO_CFG_REUSE_POS)|(RY_GPIO_INPUT_EN << GPIO_CFG_IN_EN_POS));
	}
}

void jtag_io_multi_disconfig(void)
{
	for(uint32_t i = 0; i < 5; i++)
	{
		GTRY_GPIO->GPIO_CFG[i] &= ~(RY_MAX_REUSE << GPIO_CFG_REUSE_POS);
	}
}
