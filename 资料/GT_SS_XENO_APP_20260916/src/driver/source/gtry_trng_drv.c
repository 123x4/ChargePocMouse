/*
 *	gtry_trng_drv.c
 *
 *	Created on: 2025.10.23
 *
 */

#include "gtry_trng_drv.h"

unsigned char *trng_data_ptr = NULL;
unsigned int trng_data_count = 0x0;

/**
 * @brief	TRNG模块中断处理函数
 * @param[in] 无
 * @retval 无
 */
void trng_irq_handler(void)
{
	GTRY_TRNG->TRNG_STATUS = 1;
	*trng_data_ptr++ = GTRY_TRNG->TRNG_DATA;
	if(!(--trng_data_count))
	{
		__nds__plic_disable_interrupt(IRQ_TRNG_SOURCE);
		GTRY_TRNG->TRNG_INT_EN = 0;
	}
}

/**
 * @brief	TRNG模块初始化
 * @param[in] 无
 * @retval 无
 */
void trng_init(void)
{
	GTRY_TRNG->TRNG_STOP = 0x1;
	GTRY_TRNG->TRNG_INT_EN = 0;
	GTRY_TRNG->TRNG_STATUS = 1;
	GTRY_TRNG->TRNG_START = 0x1;
}

/**
 * @brief	TRNG模块读取一个随机数——查询方式
 * @param[in] 无
 * @retval 若返回0xFF，表示上次产生多个随机数操作尚未完成；否则返回成功生成的随机数
 */
uint8_t trng_get_byte(void)
{
	if(trng_data_count)
	{
		return 0xFF;
	}

	while(GTRY_TRNG->TRNG_STATUS == 0x0);
	GTRY_TRNG->TRNG_STATUS = 0x1;
	return GTRY_TRNG->TRNG_DATA;
}

/**
 * @brief	TRNG模块读取随机数——中断方式
 * @param[in] buffer：随机数缓存buf
 * @param[in] size：待生成随机数长度
 * @retval 若返回0xFF，表示上次产生多个随机数操作尚未完成；返回0表示操作成功
 */
uint32_t trng_get_data_start(uint8_t *buffer, uint32_t size)
{
	if(trng_data_count || buffer == NULL || size == 0)
	{
		return 0xFF;
	}

	trng_data_ptr = buffer;
	trng_data_count = size;

	GTRY_TRNG->TRNG_STATUS = 0x1;
	__nds__plic_set_priority(IRQ_TRNG_SOURCE, 1);
	__nds__plic_enable_interrupt(IRQ_TRNG_SOURCE);
	GTRY_TRNG->TRNG_INT_EN = 0x1;

    return 0;
}

/**
 * @brief	获取TRNG模块中断方式产生随机数状态
 * @param[in] blocking：‘0’表示不使能阻塞，直接返回当前产生随机数的剩余个数；‘非0’表示阻塞使能，等待随机数产生完成时退出阻塞状态
 * @retval 返回当前产生随机数的剩余个数
 */
uint32_t trng_get_data_end(uint8_t blocking)
{
	if(blocking)
	{
		while(trng_data_count);
	}
	return trng_data_count;
}
