/*
 *	gtry_tsc_drv.c
 *
 *	Created on: 2025.10.23日
 *
 */

#include "gtry_tsc_drv.h"

p_tsc_int_cb tsc_drv_int_callback = NULL;			/** TSC模块中断回调函数入口 */

/**
 * @brief	TSC模块中断初始化函数
 * @param[in] tsc_int_cb：中断回调函数入口
 * \return 无
 */
void tsc_int_init(p_tsc_int_cb tsc_int_cb)
{
	GTRY_TSC->TSC_INT_CTRL |= RY_TSC_INT_CLEAR;

	__nds__plic_set_priority(IRQ_TSC_SOURCE, 1);
	__nds__plic_enable_interrupt(IRQ_TSC_SOURCE);

	tsc_drv_int_callback = tsc_int_cb;

	GTRY_TSC->TSC_INT_CTRL |= RY_TSC_INT_EN;
}

/**
 * \brief	TSC中断服务函数
 * \param[in] 无
 * \return 无
 */
void tsc_irq_handler(void)
{
	if(tsc_drv_int_callback != NULL)
	{
		tsc_drv_int_callback();
	}
	GTRY_TSC->TSC_INT_CTRL |= RY_TSC_INT_CLEAR;
}

/**
 * @brief	TSC模块初始化函数
 * @param[in] setup_time：建立时间 = (setup_time+1)*32 TSC_CLK周期
 * @param[in] trans_time：转换时间 = trans_time*TSC_CLK周期
 * @param[in] cnt2_sel：CNT2_CH值选择 \ref tsc_cnt2_sel_t
 * @param[in] tsc_clk_sel：TSC时钟源选择 \ref tsc_clk_t
 * @param[in] tsc_ref_en：内部参考振荡器使能控制 \ref tsc_ref_t
 * @retval 无
 */
void tsc_init(uint32_t setup_time, uint32_t trans_time, tsc_cnt2_sel_t cnt2_sel, tsc_clk_t tsc_clk_sel , tsc_ref_t tsc_ref_en)
{
	GTRY_TSC->TSC_START = 0x0;
	GTRY_TSC->TSC_CVT_TIME = (trans_time<=0x3fffff?trans_time:0x3fffff);
	GTRY_TSC->TSC_CTRL = ((setup_time<=0x3f?setup_time:0x3f)<<TSC_CFG_SETUP_TIME_POS) | (0x8UL<<TSC_CFG_CAP_POS) | (0x4UL<<TSC_CFG_COMP_IB_POS)
					| (0x4UL<<TSC_CFG_TSC_ICHAR1_POS) | (0x4UL<<TSC_CFG_TSC_ICHAR2_POS ) | (cnt2_sel<<TSC_CFG_CNT2_SEL_POS)
					| (tsc_clk_sel<<TSC_CFG_CLK_SEL_POS) | (tsc_ref_en<<TSC_CFG_INTER_REG_POS) | (TSC_ENABLE_MASK<<TSC_CFG_EN_POS);
}

