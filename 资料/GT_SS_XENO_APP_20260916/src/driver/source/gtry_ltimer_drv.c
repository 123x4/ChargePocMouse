/*
 *	gtry_ltimer_drv.c
 *
 *	Created on: 2025.10.23
 *
 */

#include "gtry_ltimer_drv.h"

p_ltimer_int_cb ltimer_irq_cb[2] = {NULL};			/** LTIMER模块中断回调函数 */
uint32_t ltimer_int_st[2] = {0};					/** LTIMER模块中断状态缓存 */

/**
 * @brief	LTIMER模块中断处理函数
 * @param[in] ltimer_no：ltimer模块标识号
 * @retval 无
 */
static void ltimer_irq_handler(uint32_t ltimer_no)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;
	ltimer_int_st[ltimer_no] = 0x0;
	if(LTIMER->LTIM_TICK_STS && (LTIMER->LTIM_INT_EN&LTIMER_INT_TICK_MASK))
	{
		LTIMER->LTIM_TICK_STS = 1;
		ltimer_int_st[ltimer_no] |= LTIMER_INT_TICK_MASK;
	}
	if(LTIMER->LTIM_COMPARE[0] && (LTIMER->LTIM_INT_EN&LTIMER_INT_CMP0_MASK))
	{
		LTIMER->LTIM_COMPARE[0] = 1;
		ltimer_int_st[ltimer_no] |= LTIMER_INT_CMP0_MASK;
		if(LTIMER->LTIM_COUNT < LTIMER->LTIM_CC[0] || ((LTIMER->LTIM_OVRFLW == 1) && LTIMER->LTIM_COUNT > (LTIMER->LTIM_CC[0]+30000)))
		{
			ry_delay_us(33+33*LTIMER->LTIM_PRESCALER);
		}
	}
	if( LTIMER->LTIM_COMPARE[1] && (LTIMER->LTIM_INT_EN&LTIMER_INT_CMP1_MASK))
	{
		LTIMER->LTIM_COMPARE[1]=1;
		ltimer_int_st[ltimer_no] |= LTIMER_INT_CMP1_MASK;
		if(LTIMER->LTIM_COUNT < LTIMER->LTIM_CC[1] || ((LTIMER->LTIM_OVRFLW == 1) && LTIMER->LTIM_COUNT > (LTIMER->LTIM_CC[1]+30000)))
		{
			ry_delay_us(33+33*LTIMER->LTIM_PRESCALER);
		}
	}
	if( LTIMER->LTIM_COMPARE[2] && (LTIMER->LTIM_INT_EN&LTIMER_INT_CMP2_MASK))
	{
		LTIMER->LTIM_COMPARE[2]=1;
		ltimer_int_st[ltimer_no] |= LTIMER_INT_CMP2_MASK;
		if(LTIMER->LTIM_COUNT < LTIMER->LTIM_CC[2] || ((LTIMER->LTIM_OVRFLW == 1) && LTIMER->LTIM_COUNT > (LTIMER->LTIM_CC[2]+30000)))
		{
			ry_delay_us(33+33*LTIMER->LTIM_PRESCALER);
		}
	}
	if( LTIMER->LTIM_COMPARE[3] && (LTIMER->LTIM_INT_EN&LTIMER_INT_CMP3_MASK))
	{
		LTIMER->LTIM_COMPARE[3] = 1;
		ltimer_int_st[ltimer_no] |= LTIMER_INT_CMP3_MASK;
		if(LTIMER->LTIM_COUNT < LTIMER->LTIM_CC[3] || ((LTIMER->LTIM_OVRFLW == 1) && LTIMER->LTIM_COUNT > (LTIMER->LTIM_CC[3]+30000)))
		{
			ry_delay_us(33+33*LTIMER->LTIM_PRESCALER);
		}
	}
	if(LTIMER->LTIM_OVRFLW == 1 && (LTIMER->LTIM_INT_EN&LTIMER_INT_OVRFLW_MASK))
	{
		LTIMER->LTIM_OVRFLW = 1;
		ltimer_int_st[ltimer_no] |= LTIMER_INT_OVRFLW_MASK;
	}

	if(ltimer_irq_cb[ltimer_no] != NULL)
	{
		ltimer_irq_cb[ltimer_no](ltimer_no, ltimer_int_st[ltimer_no]);
	}
}

/**
 * @brief	LTIMER0模块中断处理函数
 * @param[in] 无
 * @retval 无
 */
void Ltim0_irq_handler(void)
{
	ltimer_irq_handler(LTIMER_NUMBER0);
}

/**
 * @brief	LTIMER1模块中断处理函数
 * @param[in] 无
 * @retval 无
 */
void Ltim1_irq_handler(void)
{
	ltimer_irq_handler(LTIMER_NUMBER1);
}

/**
 * @brief	LTIMER模块配置
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] prescaler：时钟分频系数
 * @param[in] ticken：tick使能，0表示不使能，非0表示使能
 * @retval 无
 */
void ltimer_cfg(uint32_t ltimer_no, uint32_t prescaler, uint8_t ticken)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;

	__nds__plic_disable_interrupt(IRQ_LTIMER0_SOURCE+ltimer_no);
	
	LTIMER->LTIM_CLEAR = 0x1;
	while(LTIMER->LTIM_COUNT);		/* 须2个低频时钟周期才能完成此操作，等待清除运行操作完成 */
	LTIMER->LTIM_STOP = 0x1;
	LTIMER->LTIM_PRESCALER = prescaler-1;
	
	LTIMER->LTIM_TICK_STS = 0x1;
	LTIMER->LTIM_OVRFLW = 0x1;
	LTIMER->LTIM_COMPARE[0] = 0x1;
	LTIMER->LTIM_COMPARE[1] = 0x1;
	LTIMER->LTIM_COMPARE[2] = 0x1;
	LTIMER->LTIM_COMPARE[3] = 0x1;
	
	LTIMER->LTIM_TICK_EN = ticken? TICK_ENABLE:TICK_DISABLE;
}

/**
 * @brief	LTIMER模块停止运行，计数值复位
 * @param[in] ltimer_no：ltimer模块识别号
 * @retval 无
 */
void ltimer_stop(uint32_t ltimer_no)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;
	
	LTIMER->LTIM_CLEAR = 0x1;
	while(LTIMER->LTIM_COUNT);				/* 须2个低频时钟周期才能完成此操作，等待清除运行操作完成 */
	LTIMER->LTIM_STOP = 0x1;
}

/**
 * @brief	LTIMER模块清除运行状态，计数值复位
 * @param[in] ltimer_no：ltimer模块识别号
 * @retval 无
 */
void ltimer_clear(uint32_t ltimer_no)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;

	LTIMER->LTIM_CLEAR = 0x1;
	while(LTIMER->LTIM_COUNT);				/* 须2个低频时钟周期才能完成此操作，等待清除运行操作完成 */
}

/**
 * @brief	LTIMER模块设置指定ID的CC值
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] id：ltimer模块ID识别号
 * @param[in] val：待设置的CC值，最大24bit，为0不产生比较事件
 * @retval 无
 */
void ltimer_set_ccn(uint32_t ltimer_no, uint32_t id, uint32_t val)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;

	LTIMER->LTIM_CC[id] = (val&0xffffff)? val:1;
}

/**
 * @brief	LTIMER模块获取指定ID的CC值
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] id：ltimer模块ID识别号
 * @retval 返回已设置ID的设置的CC值
 */
uint32_t ltimer_get_ccn(uint32_t ltimer_no, uint32_t id)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;

	return LTIMER->LTIM_CC[id];
}

/**
 * @brief	LTIMER模块中断初始化
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] int_mask：ltimer模块中断使能位MASK
 * @param[in] ltimer_int_cb：ltimer模块中断回调函数入口
 * @retval 无
 */
void ltimer_int_init(uint32_t ltimer_no, uint32_t int_mask, p_ltimer_int_cb ltimer_int_cb)
{
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;

	if(LTIMER->LTIM_TICK_EN == TICK_DISABLE)
	{
		int_mask &= (~LTIMER_INT_TICK_MASK);
	}
	LTIMER->LTIM_INT_EN_CLR = LTIMER_INT_ALL_MASK;
	LTIMER->LTIM_TICK_STS = 0x1;
	LTIMER->LTIM_OVRFLW = 0x1;
	LTIMER->LTIM_COMPARE[0] = 0x1;
	LTIMER->LTIM_COMPARE[1] = 0x1;
	LTIMER->LTIM_COMPARE[2] = 0x1;
	LTIMER->LTIM_COMPARE[3] = 0x1;
	LTIMER->LTIM_INT_EN_SET = int_mask;
	ltimer_irq_cb[ltimer_no] = ltimer_int_cb;
	__nds__plic_set_priority(IRQ_LTIMER0_SOURCE+ltimer_no, 1);
	__nds__plic_enable_interrupt(IRQ_LTIMER0_SOURCE+ltimer_no);
}

/**
 * @brief	获取LTIMER模块事件状态
 * @param[in] ltimer_no：ltimer模块识别号
 * @retval 返回事件状态，见'LTIMER_INT'前缀宏
 */
uint32_t ltimer_get_event(uint32_t ltimer_no)
{
	uint32_t tmpevent = 0;
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;
	
	if(LTIMER->LTIM_TICK_STS == 1)
	{  
		tmpevent |= LTIMER_INT_TICK_MASK;
	}
	if(LTIMER->LTIM_OVRFLW == 1)
	{
		tmpevent |= LTIMER_INT_OVRFLW_MASK;
	}
	if(LTIMER->LTIM_COMPARE[0])
	{
		tmpevent |= LTIMER_INT_CMP0_MASK;
	}
	if(LTIMER->LTIM_COMPARE[1])
	{
		tmpevent |= LTIMER_INT_CMP1_MASK;
	}
	if(LTIMER->LTIM_COMPARE[2])
	{
		tmpevent |= LTIMER_INT_CMP2_MASK;
	}
	if(LTIMER->LTIM_COMPARE[3])
	{
		tmpevent |= LTIMER_INT_CMP3_MASK;
	}	
	return tmpevent;
}

/**
 * @brief	清除LTIMER模块事件状态
 * @param[in] ltimer_no：ltimer模块识别号
 * @param[in] event：待清除事件状态，见'LTIMER_INT'前缀宏
 * @retval 无
 */
void ltimer_clear_event(uint32_t ltimer_no, uint32_t event)
{	
	LTIM_RegDef *LTIMER = ltimer_no? GTRY_LTIM1:GTRY_LTIM0;

	if(event & LTIMER_INT_TICK_MASK)
	{  
		LTIMER->LTIM_TICK_STS = 0x1;
	}
	if(event & LTIMER_INT_OVRFLW_MASK)
	{
		LTIMER->LTIM_OVRFLW = 0x1;
	}
	if(event & LTIMER_INT_CMP0_MASK)
	{
		LTIMER->LTIM_COMPARE[0] = 0x1;
	}
	if(event & LTIMER_INT_CMP1_MASK)
	{
		LTIMER->LTIM_COMPARE[1] = 0x1;
	}
	if(event & LTIMER_INT_CMP2_MASK)
	{
		LTIMER->LTIM_COMPARE[2] = 0x1;
	}
	if(event & LTIMER_INT_CMP3_MASK)
	{
		LTIMER->LTIM_COMPARE[3] = 0x1;
	}	
}
