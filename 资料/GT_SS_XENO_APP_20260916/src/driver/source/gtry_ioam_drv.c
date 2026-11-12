/*
 *	gtry_ioam_drv.c
 *
 *	Created on: 2025.10.23
 *
 */

#include "gtry_ioam_drv.h"

p_ioam_int_cb ioam_drv_int_callback = NULL;			/** IOAM中断回调函数 */

/**
 * @brief	IOAM模块中断初始化
 * @param[in] int_sample_num：采样数据个数阈值，当采样个数等于该值时触发中断
 * @param[in] ioam_callback：ioam模块中断回调函数入口，详见'p_ioam_int_cb'
 * @retval 无
 */
void ioam_int_init(uint32_t int_sample_num, p_ioam_int_cb ioam_callback)
{
	if(ioam_callback == NULL)
	{
		GTRY_IOAM->IOAM_INT_MASK = 0x0;
		ioam_drv_int_callback = NULL;
	}
	else
	{
		GTRY_IOAM->IOAM_SAMP_TH = int_sample_num<=IOAM_SAMPLE_NUM_MAX? int_sample_num:IOAM_SAMPLE_NUM_MAX;
		GTRY_IOAM->IOAM_INT_MASK = IOAM_INT_EN_BIT;
		ioam_drv_int_callback = ioam_callback;

		// Priority must be set > 0 to trigger the interrupt
		__nds__plic_set_priority(IRQ_IOAM_SOURCE, 1);

		// Enable PLIC interrupt PIT source
		__nds__plic_enable_interrupt(IRQ_IOAM_SOURCE);

	}
}

/**
 * @brief	IOAM模块中断处理函数
 * @param[in] 无
 * @retval 无
 */
void ioam_irq_handler(void)
{
	uint32_t status = GTRY_IOAM->IOAM_INT_STATUS;
	GTRY_IOAM->IOAM_INT_CLEAR = status;
	if(ioam_drv_int_callback != NULL)
	{
		ioam_drv_int_callback();
	}
}

/**
 * @brief	IOAM模块初始化
 * @param[in] chl_mask：采样通道标识MASK
 * @param[in] samperiod：采样通道采样间隔周期数，参数范围0~255
 * @retval 无
 */
void ioam_init(uint32_t chl_mask, uint32_t samperiod)
{
	GTRY_IOAM->IOAM_EN = 0x0;
    GTRY_IOAM->IOAM_CHL_SEL = chl_mask;
    GTRY_IOAM->IOAM_RON_SAP_INTV = 0x4;
    GTRY_IOAM->IOAM_SAMP_INTV = samperiod>IOAM_SAMPLE_PERIOD_MAX? IOAM_SAMPLE_PERIOD_MAX:samperiod;
}


/**
 * @brief	IOAM停止该模块并关闭该模块电源
 * @param[in] 无
 * @retval 无
 */
void ioam_uninit(void)
{
	GTRY_IOAM->IOAM_EN = 0x0;
	GTRY_IOAM->IOAM_INT_MASK  = 0x0;
}

/**
 * @brief	获取IOAM模块已采样数据个数
 * @param[in] 无
 * @retval 返回已采样数据个数，最大为IOAM_SAMPLE_NUM_MAX，如果返回值超过IOAM_SAMPLE_NUM_MAX，则也只IOAM_SAMPLE_NUM_MAX个数据有效
 */
uint32_t ioam_get_sampnum(void)
{
	return GTRY_IOAM->IOAM_SAMP_NUM;
}

/**
 * @brief	获取IOAM模块已开启采样的通道
 * @param[in] 无
 * @retval 返回已开启采样的通道，可用IOAM通道标识MASK进行区别
 */
uint32_t ioam_get_active_chl(void)
{
	return GTRY_IOAM->IOAM_CHL_ON;
}

/**
 * @brief	获取IOAM模块当前通道的采样数据
 * @param[in] chl_num：采样通道标识
 * @retval 返回当前通道的采样数据
 */
uint32_t ioam_get_chl_val(uint32_t chl_num)
{
	return GTRY_IOAM->IOAM_CHL_VAL[chl_num];
}

/**
 * @brief	IOAM指定通道开启采样
 * @param[in] chl_mask：采样通道标识MASK
 * @retval 无
 */
void ioam_start(uint32_t channel)
{
	GTRY_IOAM->IOAM_CHL_ON = channel;
    GTRY_IOAM->IOAM_EN = 0x1;
}

/**
 * @brief	IOAM所有通道停止采样
 * @param[in] 无
 * @retval 无
 */
void ioam_stop(void)
{
    GTRY_IOAM->IOAM_EN = 0x0;
}
