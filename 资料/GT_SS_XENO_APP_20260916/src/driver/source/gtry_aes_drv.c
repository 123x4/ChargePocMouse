/*
 *	gtry_aes_drv.c
 *
 *	Created on: 2025.10.23
 *
 */


#include "gtry_aes_drv.h"


static uint32_t aes_int_st = 0x0;									/** AES中断状态，AES各模式共用 */

p_aes_int_cb aes_drv_int_callback[3] = {NULL};					/** AES中断回调函数数组 */

/*
 * CCM data structure
 */
struct aes_ccm_data_st aes_ccm_data __attribute__((section(".section_sram"))) = {0x0};
static uint8_t g_scr_ptr[AES_CCM_SCRATCH_RAM_SIZE] __attribute__((section(".section_sram"))) = {0x0};
static uint8_t g_input_ptr[AES_CCM_INPUT_RAM_SIZE] __attribute__((section(".section_sram"))) = {0x0};
static uint8_t g_output_ptr[AES_CCM_OUTPUT_RAM_SIZE] __attribute__((section(".section_sram"))) = {0x0};

/*
 * AAR data structure
 */
static uint8_t g_aarAdd_Ptr[AES_AAR_ADDR_RAM_SIZE] __attribute__((section(".section_sram"))) = {0x0};
static uint8_t g_aarIrk_Ptr[AES_AAR_IRK_NUM_RAM_SIZE][AES_AAR_IRK_LEN_RAM_SIZE] __attribute__((section(".section_sram"))) = {{0x0}};
static uint8_t g_aarScr_Ptr[AES_AAR_SCRATCH_RAM_SIZE] __attribute__((section(".section_sram"))) = {0x0};

/*
 * ECB data structure
 */
static uint8_t  ecb_data[AES_ECB_DATA_NUM_RAM_SIZE*AES_ECB_DATA_LEN_RAM_SIZE] __attribute__((section(".section_sram"))) = {0x0};   ///< ECB data structure for RNG peripheral to access.
static uint8_t* ecb_key;        		///< Key:        Starts at ecb_data
static uint8_t* ecb_cleartext;  		///< Cleartext:  Starts at ecb_data + 16 bytes.
static uint8_t* ecb_ciphertext; 		///< Ciphertext: Starts at ecb_data + 32 bytes.

/**
 * @brief	AES中断处理函数
 * @param[in] 无
 * @retval 无
 */
void aes_irq_handler(void)
{
	if((GTRY_AES->AES_ECB_END || GTRY_AES->AES_ECB_ERROR)&& aes_drv_int_callback[AES_ECB_INT_ID] != NULL)
	{
		aes_int_st = (GTRY_AES->AES_ECB_ERROR<<1)|GTRY_AES->AES_ECB_END;
		GTRY_AES->AES_ECB_END=0;
		GTRY_AES->AES_ECB_ERROR=0;
		aes_drv_int_callback[AES_ECB_INT_ID](aes_int_st);
	}
	else if((GTRY_AES->AES_CCM_END_KSGEN || GTRY_AES->AES_CCM_END_CRYPT || GTRY_AES->AES_CCM_ERROR)&& aes_drv_int_callback[AES_CCM_INT_ID] != NULL)
	{
		aes_int_st = (GTRY_AES->AES_CCM_ERROR<<2)|(GTRY_AES->AES_CCM_END_CRYPT<<1)|GTRY_AES->AES_CCM_END_KSGEN;
		GTRY_AES->AES_CCM_END_KSGEN=0;
		GTRY_AES->AES_CCM_END_CRYPT=0;
		GTRY_AES->AES_CCM_ERROR = 0;
		aes_drv_int_callback[AES_CCM_INT_ID](aes_int_st);
		if(GTRY_AES->AES_CCM_SHORTS == 0x0)
		{
			GTRY_AES->AES_CCM_CRYPT = 0x1;
		}
	}
	else if((GTRY_AES->AES_AAR_NOT_RESOLVED || GTRY_AES->AES_AAR_RESOLVED || GTRY_AES->AES_AAR_END)&& aes_drv_int_callback[AES_AAR_INT_ID] != NULL)
	{
		aes_int_st = (GTRY_AES->AES_AAR_NOT_RESOLVED<<2)|(GTRY_AES->AES_AAR_RESOLVED<<1)|GTRY_AES->AES_AAR_END;
		GTRY_AES->AES_AAR_END=0;
		GTRY_AES->AES_AAR_RESOLVED=0;
		GTRY_AES->AES_AAR_NOT_RESOLVED = 0;
		aes_drv_int_callback[AES_AAR_INT_ID](aes_int_st);
	}
}

/**
 * @brief	AES_ECB初始化
 * @param[in] aes_callback：aes_ecb中断回调函数入口指针
 * @return 无
 */
void aes_ecb_init(p_aes_int_cb aes_callback)
{
	__nds__plic_set_priority(IRQ_AES_SOURCE, 1);
	__nds__plic_enable_interrupt(IRQ_AES_SOURCE);

    ecb_key = ecb_data;
    ecb_cleartext  = ecb_data + AES_ECB_DATA_LEN_RAM_SIZE;
    ecb_ciphertext = ecb_data + (AES_ECB_DATA_LEN_RAM_SIZE + AES_ECB_DATA_LEN_RAM_SIZE);

    GTRY_AES->AES_ECB_DATA_PTR = (uint32_t)ecb_data;

	GTRY_AES->AES_ECB_STOP = 0x1;
	if(aes_callback != NULL)
	{
		aes_drv_int_callback[AES_ECB_INT_ID] = aes_callback;
		GTRY_AES->AES_ECB_INT_EN_SET = AES_ECB_INT_ERROR_MASK|AES_ECB_INT_END_MASK;
	}
	else
	{
		GTRY_AES->AES_ECB_INT_EN_CLR = AES_ECB_INT_ERROR_MASK|AES_ECB_INT_END_MASK;
		aes_drv_int_callback[AES_ECB_INT_ID] = NULL;
	}
}

/**
 * @brief	AES_ECB停止该模块并关闭该模块电源
 * @param[in] 无
 * @return 无
 */
void aes_ecb_uninit(void)
{
	__nds__plic_disable_interrupt(IRQ_AES_SOURCE);
    GTRY_AES->AES_ECB_STOP = 0x1;
    GTRY_AES->AES_ECB_INT_EN_CLR = AES_ECB_INT_ERROR_MASK|AES_ECB_INT_END_MASK;
}

/**
 * @brief	AES_ECB设置KEY数据
 * @param[in] key：KEY数据buf指针，长度必须为16
 * @return 无
 */
void aes_ecb_set_key(const uint8_t * key)
{
    MEMCPY(ecb_key, key, AES_ECB_DATA_LEN_RAM_SIZE);
}

/**
 * @brief	AES_ECB加密函数
 * @param[in] ecb_data_ptr：存放ECB数据的起始地址，该起始地址下分配16字节给秘钥，分配16字节给明文，分配16字节给密文，共计48字节大小
 * @return 返回数据可以判断加密是否正确，'0'表示加密成功，'非0'表示加密失败
 */
uint32_t aes_ecb_crypt(uint8_t *dest_buf, const uint8_t *src_buf)
{
	uint32_t counter = 0x1000000;

    if(src_buf != ecb_cleartext)
    {
    	MEMCPY(ecb_cleartext, src_buf, AES_ECB_DATA_LEN_RAM_SIZE);
    }

    GTRY_AES->AES_ECB_START = 0x1;

	while(aes_drv_int_callback[AES_ECB_INT_ID] != NULL && aes_int_st == 0x0)
    {
		counter--;
        if(counter == 0)
        {
            return 1;
        }
    }
	while(aes_drv_int_callback[AES_ECB_INT_ID] == NULL && GTRY_AES->AES_ECB_END == 0x0 && GTRY_AES->AES_ECB_ERROR == 0x0)
    {
		counter--;
        if(counter == 0)
        {
            return 2;
        }
    }
	if((AES_ECB_INT_ERROR_MASK&aes_int_st) || GTRY_AES->AES_ECB_ERROR)
	{
		GTRY_AES->AES_ECB_END = 0x0;
		GTRY_AES->AES_ECB_ERROR = 0x0;
		aes_int_st = 0x0;
		return 3;
	}
	aes_int_st = 0x0;
	GTRY_AES->AES_ECB_END = 0x0;
	GTRY_AES->AES_ECB_ERROR = 0x0;

	if(dest_buf != ecb_ciphertext)
    {
		MEMCPY(dest_buf, ecb_ciphertext, AES_ECB_DATA_LEN_RAM_SIZE);
    }

	return 0;
}

/**
 * \brief	AES_CCM初始化函数
 * \param[in] en_shorts：'非0'则使能ENDKSGEN事件自动触发CRYPT任务
 * \param[in] ccm_scratch_ptr：秘钥流存储区域，临时数据起始地址
 * \param[in] aes_callback：aes_ccm中断回调函数入口指针
 * \return 无
 */
void aes_ccm_init(uint32_t en_shorts, p_aes_int_cb aes_callback)
{
    uint32_t int_mask = 0x0;

	__nds__plic_set_priority(IRQ_AES_SOURCE, 1);
	__nds__plic_enable_interrupt(IRQ_AES_SOURCE);

    GTRY_AES->AES_CCM_AAR_ENABLE = 0x1;

	if(en_shorts)
	{
		GTRY_AES->AES_CCM_SHORTS = AES_CCM_SHORTS_EN;
		int_mask = AES_CCM_INT_ERROR_MASK|AES_CCM_INT_ENDCRYPT_MASK;//|AES_CCM_INT_ENDKSGEN_MASK;
	}
	else
	{
		GTRY_AES->AES_CCM_SHORTS = 0x0;
		int_mask = AES_CCM_INT_ERROR_MASK|AES_CCM_INT_ENDCRYPT_MASK|AES_CCM_INT_ENDKSGEN_MASK;
	}

    MEMSET(g_scr_ptr, 0, AES_CCM_SCRATCH_RAM_SIZE);
    MEMSET(&aes_ccm_data, 0, AES_CCM_CNF_RAM_SIZE);
	GTRY_AES->AES_CCM_SCRATCH_PTR = (uint32_t)g_scr_ptr;
    GTRY_AES->AES_CCM_CNF_PTR = (uint32_t)&aes_ccm_data;

    GTRY_AES->AES_CCM_END_KSGEN = 0x0;
    GTRY_AES->AES_CCM_END_CRYPT = 0x0;
    GTRY_AES->AES_CCM_ERROR = 0x0;
	//GTRY_AES->AES_CCM_MIC_STATUS = 0x0;

    if(aes_callback != NULL)
	{
		aes_drv_int_callback[AES_CCM_INT_ID] = aes_callback;
		GTRY_AES->AES_CCM_INT_EN_SET = int_mask;//AES_CCM_INT_ERROR_MASK|AES_CCM_INT_ENDCRYPT_MASK|AES_CCM_INT_ENDKSGEN_MASK;
	}
	else
	{
		GTRY_AES->AES_CCM_INT_EN_CLR = AES_CCM_INT_ERROR_MASK|AES_CCM_INT_ENDCRYPT_MASK|AES_CCM_INT_ENDKSGEN_MASK;
		aes_drv_int_callback[AES_CCM_INT_ID] = NULL;
	}
}

/**
 * \brief	AES_CCM模块关闭电源函数
 * \param[in] 无
 * \return 无
 */
void aes_ccm_uninit(void)
{
	__nds__plic_disable_interrupt(IRQ_AES_SOURCE);
	GTRY_AES->AES_CCM_INT_EN_CLR = AES_CCM_INT_ERROR_MASK|AES_CCM_INT_ENDCRYPT_MASK|AES_CCM_INT_ENDKSGEN_MASK;
//	GTRY_AES->AES_CCM_AAR_ENABLE = 0x0;
	GTRY_AES->AES_CCM_SHORTS = 0x0;
}

/**
 * @brief	AES_CCM设置KEY数据
 * @param[in] key：KEY数据buf指针，长度必须为16
 * @return 无
 */
void aes_ccm_set_key(const uint8_t *key)
{
	MEMCPY(aes_ccm_data.aes, key, 16);
}

/**
 * \brief	AES_CCM模块停止运行
 * \param[in] 无
 * \return 无
 */
void aes_ccm_stop(void)
{
	GTRY_AES->AES_CCM_INT_EN_CLR = AES_CCM_INT_ERROR_MASK|AES_CCM_INT_ENDCRYPT_MASK|AES_CCM_INT_ENDKSGEN_MASK;
//	GTRY_AES->AES_CCM_AAR_ENABLE = 0x0;
	GTRY_AES->AES_CCM_SHORTS = 0x0;
	GTRY_AES->AES_CCM_END_KSGEN = 0x0;
	GTRY_AES->AES_CCM_END_CRYPT = 0x0;
	GTRY_AES->AES_CCM_ERROR = 0x0;
}

/**
 * \brief	AES_CCM加解密执行函数
 * \param[in] ccm_mode：'0'表示加密模式，'非0'表示解密模式
 * \param[in] cnfptr：秘钥数据起始地址
 * \param[in] inputptr：待加密数据起始地址
 * \param[in] outputptr：加密数据输出起始地址
 * \return 返回值判断加密是否正确，'0'表示加密成功，'非0'表示加解密错误
 */
uint32_t aes_ccm_execute(uint32_t ccm_mode, uint8_t *inputptr, uint8_t *outputptr)
{
    MEMSET(g_input_ptr, 0, AES_CCM_INPUT_RAM_SIZE);
    MEMSET(g_output_ptr, 0, AES_CCM_OUTPUT_RAM_SIZE);
	MEMCPY(g_input_ptr, inputptr, AES_CCM_INPUT_RAM_SIZE);
	
    GTRY_AES->AES_CCM_IN_PTR = (uint32_t)g_input_ptr;
    GTRY_AES->AES_CCM_OUT_PTR = (uint32_t)g_output_ptr;

	GTRY_AES->AES_CCM_SHORTS = AES_CCM_SHORTS_EN;
	GTRY_AES->AES_CCM_END_KSGEN = 0x0;
	GTRY_AES->AES_CCM_END_CRYPT = 0x0;
	GTRY_AES->AES_CCM_ERROR = 0x0;

	if(ccm_mode)
	{
		GTRY_AES->AES_CCM_MODE = AES_CCM_DECRYPT_MODE;
//		aes_ccm_data.direction = 1; /* 方向位，从机发送给主机为0,即加密;主机发送给从机为1，即解密 */
	}
	else
	{
		GTRY_AES->AES_CCM_MODE = AES_CCM_ENCRYPT_MODE;
//		aes_ccm_data.direction = 0; /* 方向位，从机发送给主机为0,即加密;主机发送给从机为1，即解密 */
	}

    GTRY_AES->AES_CCM_KSGEN = 0x1;

	while(aes_drv_int_callback[AES_CCM_INT_ID] != NULL && (aes_int_st&AES_CCM_INT_ENDCRYPT_MASK) == 0x0)
    {
		if(aes_int_st&AES_CCM_INT_ERROR_MASK)
		{
			GTRY_AES->AES_CCM_END_KSGEN = 0x0;
			GTRY_AES->AES_CCM_END_CRYPT = 0x0;
			GTRY_AES->AES_CCM_ERROR = 0x0;
			aes_int_st = 0x0;
			return 1;
		}
	}

	while(aes_drv_int_callback[AES_CCM_INT_ID] == NULL && GTRY_AES->AES_CCM_END_KSGEN == 0x0)
    {
		if(GTRY_AES->AES_CCM_ERROR)
		{
			GTRY_AES->AES_CCM_END_KSGEN = 0x0;
			GTRY_AES->AES_CCM_END_CRYPT = 0x0;
			GTRY_AES->AES_CCM_ERROR = 0x0;
			return 2;
		}
	}

	aes_int_st = 0x0;
	if(aes_drv_int_callback[AES_CCM_INT_ID] == NULL && GTRY_AES->AES_CCM_SHORTS == 0x0)
	{
		GTRY_AES->AES_CCM_CRYPT = 0x1;
	}
	while(aes_drv_int_callback[AES_CCM_INT_ID] == NULL && GTRY_AES->AES_CCM_END_CRYPT == 0x0)
	{
		if(GTRY_AES->AES_CCM_ERROR)
		{
			GTRY_AES->AES_CCM_KSGEN = 0x0;
			GTRY_AES->AES_CCM_END_KSGEN = 0x0;
			GTRY_AES->AES_CCM_END_CRYPT = 0x0;
			GTRY_AES->AES_CCM_ERROR = 0x0;
			return 3;
		}
	}
	if(GTRY_AES->AES_CCM_MODE == AES_CCM_DECRYPT_MODE && GTRY_AES->AES_CCM_MIC_STATUS == 0x0)
	{
		GTRY_AES->AES_CCM_KSGEN = 0x0;
		GTRY_AES->AES_CCM_END_KSGEN = 0x0;
		GTRY_AES->AES_CCM_END_CRYPT = 0x0;
		GTRY_AES->AES_CCM_ERROR = 0x0;
		return 4;
	}
	if(aes_drv_int_callback[AES_CCM_INT_ID] == NULL)
	{
		GTRY_AES->AES_CCM_KSGEN = 0x0;
		GTRY_AES->AES_CCM_END_KSGEN = 0x0;
		GTRY_AES->AES_CCM_END_CRYPT = 0x0;
		GTRY_AES->AES_CCM_ERROR = 0x0;
	}

	MEMCPY(outputptr, g_output_ptr, AES_CCM_OUTPUT_RAM_SIZE);
    return 0;
}

/**
 * \brief	AES_AAR地址解析初始化函数
 * \param[in] aar_scratch_ptr：秘钥流存储区域，临时数据起始地址
 * \param[in] aes_callback：aes_aar中断回调函数入口指针
 * \return 无
 */
void aes_aar_init(uint8_t *aar_scratch_ptr, p_aes_int_cb aes_callback)
{
	__nds__plic_set_priority(IRQ_AES_SOURCE, 1);
	__nds__plic_enable_interrupt(IRQ_AES_SOURCE);

    GTRY_AES->AES_CCM_AAR_ENABLE = 0x0;

    MEMCPY(g_aarScr_Ptr, aar_scratch_ptr, AES_AAR_SCRATCH_RAM_SIZE);

    GTRY_AES->AES_AAR_SCRATCH_PTR = (uint32_t)g_aarScr_Ptr;

    GTRY_AES->AES_AAR_END = 0x0;
    GTRY_AES->AES_AAR_RESOLVED = 0x0;
    GTRY_AES->AES_AAR_NOT_RESOLVED = 0x0;

    if(aes_callback != NULL)
	{
		aes_drv_int_callback[AES_AAR_INT_ID] = aes_callback;
		GTRY_AES->AES_AAR_INT_EN_SET = AES_AAR_INT_NOTRESOLVED_MASK|AES_AAR_INT_RESOLVED_MASK|AES_AAR_INT_END_MASK;
	}
	else
	{
		GTRY_AES->AES_AAR_INT_EN_CLR = AES_AAR_INT_NOTRESOLVED_MASK|AES_AAR_INT_RESOLVED_MASK|AES_AAR_INT_END_MASK;
		aes_drv_int_callback[AES_AAR_INT_ID] = NULL;
	}
}

/**
 * \brief	AES_AAR模块关闭电源函数
 * \param[in] 无
 * \return 无
 */
void aes_aar_uninit(void)
{
	__nds__plic_disable_interrupt(IRQ_AES_SOURCE);
	GTRY_AES->AES_AAR_INT_EN_CLR = AES_AAR_INT_NOTRESOLVED_MASK|AES_AAR_INT_RESOLVED_MASK|AES_AAR_INT_END_MASK;
    //GTRY_AES->AES_CCM_AAR_ENABLE = 0x1;
}

/**
 * \brief	AES_AAR解析地址函数
 * \param[in] irkptr：IRK数据结构起始地址
 * \param[in] irknum：IRK数据结构中有效的IRK数量，范围为1~16
 * \param[in] addrptr：addrptr解析数据起始地址
 * \return 返回值判断解析是否正确，'0'表示解析失败，'非0'表示返回解析后的状态信息
 */
uint32_t aes_aar_analysis(uint8_t *irkptr, uint32_t irknum, uint8_t *addrptr)
{
    MEMCPY(*g_aarIrk_Ptr, irkptr, AES_AAR_IRK_NUM_RAM_SIZE*AES_AAR_IRK_LEN_RAM_SIZE);
    MEMCPY(g_aarAdd_Ptr, addrptr, AES_AAR_ADDR_RAM_SIZE);

    GTRY_AES->AES_AAR_IRK_PTR = (uint32_t)g_aarIrk_Ptr;    //解析地址指针
    GTRY_AES->AES_AAR_ADD_PTR = (uint32_t)g_aarAdd_Ptr;   //解析地址指针

    GTRY_AES->AES_AAR_END = 0x0;
    GTRY_AES->AES_AAR_RESOLVED = 0x0;
    GTRY_AES->AES_AAR_NOT_RESOLVED = 0x0;

	GTRY_AES->AES_AAR_NIRK = irknum>AES_AAR_NIRK_MAX? AES_AAR_NIRK_MAX:irknum;

	GTRY_AES->AES_AAR_START = 0x1;

	while(aes_drv_int_callback[AES_AAR_INT_ID] != NULL && aes_int_st == 0x0)
	{
		//aes_int_st = 0x0;
	}

	while(aes_drv_int_callback[AES_AAR_INT_ID] == NULL && GTRY_AES->AES_AAR_END == 0x0)
	{

	}
	if((aes_int_st&AES_AAR_INT_NOTRESOLVED_MASK) || (GTRY_AES->AES_AAR_NOT_RESOLVED))
	{
		GTRY_AES->AES_AAR_END = 0x0;
		GTRY_AES->AES_AAR_RESOLVED = 0x0;
		GTRY_AES->AES_AAR_NOT_RESOLVED = 0x0;
		aes_int_st = 0x0;
		return 0;
	}
	aes_int_st = 0x0;

	GTRY_AES->AES_AAR_END = 0x0;
	GTRY_AES->AES_AAR_RESOLVED = 0x0;
	GTRY_AES->AES_AAR_NOT_RESOLVED = 0x0;

    return GTRY_AES->AES_AAR_STATUS;
}
