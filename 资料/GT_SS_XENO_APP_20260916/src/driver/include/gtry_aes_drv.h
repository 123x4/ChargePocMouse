/*
 *	gtry_aes_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_AES_DRV_H__
#define __GTRY_AES_DRV_H__

#include "gtry_platform.h"

/*********************		ECB		*********************/
#define 	AES_ECB_INT_ID						0					/** AES_ECB中断回调函数标识号 */
#define 	AES_CCM_INT_ID						1					/** AES_CCM中断回调函数标识号 */
#define 	AES_AAR_INT_ID						2					/** AES_AAR中断回调函数标识号 */

#define     AES_ECB_DATA_LEN_RAM_SIZE   		16					/** AES_ECB模式单个数据结构长度 */
#define     AES_ECB_DATA_NUM_RAM_SIZE   		3					/** AES_ECB模式单个数据结构个数 */

#define 	AES_ECB_KEY_ID						0					/** AES_ECB KEY数据结构标识号 */
#define 	AES_ECB_DATA_ID						1					/** AES_ECB DATA数据结构标识号 */
#define 	AES_ECB_ENCRYPT_ID					2					/** AES_ECB ENCRYPT数据结构标识号 */

#define     AES_ECB_INT_ERROR_MASK    			(0x1<<1)			/** AES_ECB 错误事件中断位MASK */
#define     AES_ECB_INT_END_MASK      			(0x1<<0)			/** AES_ECB 结束事件中断位MASK */
/*********************		ECB_END		*********************/

/*********************		CCM		*********************/
#define 	AES_CCM_SCRATCH_RAM_SIZE			48					/** AES_CCM SCRATCH数据结构长度 */
#define 	AES_CCM_CNF_RAM_SIZE				33					/** AES_CCM CNF数据结构长度 */
#define 	AES_CCM_INPUT_RAM_SIZE				34					/** AES_CCM INPUT数据结构长度 */
#define 	AES_CCM_OUTPUT_RAM_SIZE				34					/** AES_CCM OUTPUT数据结构长度 */

#define     AES_CCM_ENCRYPT_MODE				0x0					/** AES_CCM 加密模式 */
#define     AES_CCM_DECRYPT_MODE				0x1					/** AES_CCM 解密模式 */

#define     AES_CCM_SHORTS_DIS    				0x0					/** AES_CCM 不使能ENDKSGEN事件自动触发CRYPT任务 */
#define     AES_CCM_SHORTS_EN    				0x1					/** AES_CCM 使能ENDKSGEN事件自动触发CRYPT任务 */

#define     AES_CCM_INT_ERROR_MASK				(0x1<<2)			/** AES_CCM 错误事件中断位MASK */
#define     AES_CCM_INT_ENDCRYPT_MASK			(0x1<<1)			/** AES_CCM 加解密结束事件中断位MASK */
#define     AES_CCM_INT_ENDKSGEN_MASK			(0x1<<0)			/** AES_CCM key-stream生成结束事件中断位MASK */
/*********************		CCM_END		*********************/

/*********************		AAR		*********************/
#define 	AES_AAR_SCRATCH_RAM_SIZE			10					/** AES_AAR SCRATCH数据结构长度 */
#define 	AES_AAR_IRK_LEN_RAM_SIZE			16					/** AES_AAR IRK数据结构长度 */
#define 	AES_AAR_IRK_NUM_RAM_SIZE			16					/** AES_AAR IRK数据结构个数 */
#define 	AES_AAR_ADDR_RAM_SIZE				9					/** AES_AAR ADDR数据结构长度 */

#define     AES_AAR_NIRK_MAX   	 				16					/** AES_AAR 最大IRK个数：1~16 */

#define     AES_AAR_INT_NOTRESOLVED_MASK		(0x1<<2)			/** AES_AAR 地址解析失败事件中断位MASK */
#define     AES_AAR_INT_RESOLVED_MASK			(0x1<<1)			/** AES_AAR 地址解析成功事件中断位MASK */
#define     AES_AAR_INT_END_MASK				(0x1<<0)			/** AES_AAR 地址解析结束事件中断位MASK */
/*********************		AAR_END		*********************/

struct __attribute__ ((packed)) aes_ccm_data_st
{
    uint8_t aes[16];                /* AES Key */
    uint8_t packet_counter[5];		/* 小端模式 */
    uint8_t reserved0[3];
    uint8_t direction;				/* bit 0有效 */
    uint8_t iv[8];					/* IV[0] LSO  IV[7] MSO */
};

extern struct aes_ccm_data_st aes_ccm_data __attribute__((section(".section_sram")));

/**
 * @brief	AES模块中断回调函数指针
 * @param[in] int_st：AES中断状态源
 * @retval 无
 */
typedef void (*p_aes_int_cb)(uint32_t int_st);

/**
 * @brief	AES_ECB初始化
 * @param[in] aes_callback：aes_ecb中断回调函数入口指针
 * @return 无
 */
extern void aes_ecb_init(p_aes_int_cb aes_callback);

/**
 * @brief	AES_ECB停止该模块并关闭该模块电源
 * @param[in] 无
 * @return 无
 */
extern void aes_ecb_uninit(void);

/**
 * @brief	AES_ECB设置KEY数据
 * @param[in] key：KEY数据buf指针，长度必须为16
 * @return 无
 */
extern void aes_ecb_set_key(const uint8_t *key);

/**
 * @brief	AES_ECB加密函数
 * @param[in] ecb_data_ptr：存放ECB数据的起始地址，该起始地址下分配16字节给秘钥，分配16字节给明文，分配16字节给密文，共计48字节大小
 * @return 返回数据可以判断加密是否正确，'0'表示加密成功，'非0'表示加密失败
 */
extern uint32_t aes_ecb_crypt(uint8_t *dest_buf, const uint8_t *src_buf);

/**
 * \brief	AES_CCM初始化函数
 * \param[in] en_shorts：'非0'则使能ENDKSGEN事件自动触发CRYPT任务
 * \param[in] ccm_scratch_ptr：秘钥流存储区域，临时数据起始地址
 * \param[in] aes_callback：aes_ccm中断回调函数入口指针
 * \return 无
 */
extern void aes_ccm_init(uint32_t en_shorts, p_aes_int_cb aes_callback);

/**
 * \brief	AES_CCM模块关闭电源函数
 * \param[in] 无
 * \return 无
 */
extern void aes_ccm_uninit(void);

/**
 * @brief	AES_CCM设置KEY数据
 * @param[in] key：KEY数据buf指针，长度必须为16
 * @return 无
 */
extern void aes_ccm_stop(void);

/**
 * \brief	AES_CCM模块停止运行
 * \param[in] 无
 * \return 无
 */
extern void aes_ccm_set_key(const uint8_t *key);

/**
 * \brief	AES_CCM加解密执行函数
 * \param[in] ccm_mode：'0'表示加密模式，'非0'表示解密模式
 * \param[in] cnfptr：秘钥数据起始地址
 * \param[in] inputptr：待加密数据起始地址
 * \param[in] outputptr：加密数据输出起始地址
 * \return 返回值判断加密是否正确，'0'表示加密成功，'非0'表示加解密错误
 */
extern uint32_t aes_ccm_execute(uint32_t ccm_mode, uint8_t *inputptr, uint8_t *outputptr);

/**
 * \brief	AES_AAR地址解析初始化函数
 * \param[in] aar_scratch_ptr：秘钥流存储区域，临时数据起始地址
 * \param[in] aes_callback：aes_aar中断回调函数入口指针
 * \return 无
 */
extern void aes_aar_init(uint8_t *aar_scratch_ptr, p_aes_int_cb aes_callback);

/**
 * \brief	AES_AAR模块关闭电源函数
 * \param[in] 无
 * \return 无
 */
extern void aes_aar_uninit(void);

/**
 * \brief	AES_AAR解析地址函数
 * \param[in] irkptr：IRK数据结构起始地址
 * \param[in] irknum：IRK数据结构中有效的IRK数量，范围为1~16
 * \param[in] addrptr：addrptr解析数据起始地址
 * \return 返回值判断解析是否正确，'0'表示解析失败，'非0'表示返回解析后的状态信息
 */
extern uint32_t aes_aar_analysis(uint8_t *irkptr, uint32_t irknum, uint8_t*addrptr);



#endif	/* __GTRY_AES_DRV_H__ */

