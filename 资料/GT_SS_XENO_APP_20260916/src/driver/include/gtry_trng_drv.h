/*
 *	gtry_rng_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_TRNG_DRV_H
#define __GTRY_TRNG_DRV_H

#include "gtry_platform.h"


/**
 * @brief	TRNG模块初始化
 * @param[in] 无
 * @retval 无
 */
void trng_init(void);

/**
 * @brief	TRNG模块读取一个随机数——查询方式
 * @param[in] 无
 * @retval 若返回0xFF，表示上次产生多个随机数操作尚未完成；否则返回成功生成的随机数
 */
uint8_t trng_get_byte(void);

/**
 * @brief	TRNG模块读取随机数——中断方式
 * @param[in] buffer：随机数缓存buf
 * @param[in] size：待生成随机数长度
 * @retval 若返回0xFF，表示上次产生多个随机数操作尚未完成；返回0表示操作成功
 */
uint32_t trng_get_data_start(uint8_t *buffer, uint32_t size);

/**
 * @brief	获取TRNG模块中断方式产生随机数状态
 * @param[in] blocking：‘0’表示不使能阻塞，直接返回当前产生随机数的剩余个数；‘非0’表示阻塞使能，等待随机数产生完成时退出阻塞状态
 * @retval 返回当前产生随机数的剩余个数
 */
uint32_t trng_get_data_end(uint8_t blocking);

#endif	/* __GTRY_TRNG_DRV_H */
