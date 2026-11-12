/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */
  
/* Includes ------------------------------------------------------------------*/
#include "gtry_gpio_drv.h"
#include "gtry_dvp_drv.h"


volatile uint32_t dvp_vs_num ;	 /* 帧同步信号标志，在中断函数和main函数里面使用 */
volatile uint32_t dvp_ppa_len ;	 /* ppa一次传输数据长度 */
volatile uint32_t dvp_ppa_num ;	 /* 一帧像素大小传输时总ppa个数 */
volatile uint32_t dvp_ppa_cnt ;	 /* ppa传输次数计数 */

p_dvp_int_cb dvp_drv_int_callback = NULL;			/** dvp中断回调函数入口 */


uint32_t dvp_io_multi_config(dvp_multi_io dvp_group, DVP_Fmt_TypeDef fmt_type)
{
	uint32_t offset_no = dvp_group*32;
	uint32_t io_num = 13;//DVP_FMT_DATA8

	if(dvp_group >= DVP_IO_GP_MAX || fmt_type >= DVP_FMT_MAX)
	{
		return 1;
	}

	if(fmt_type == DVP_FMT_DATA10)
	{
		io_num = 15;
	}
	else if(fmt_type == DVP_FMT_DATA12)
	{
		io_num = 17;
	}
	else if(fmt_type == DVP_FMT_DATA14)
	{
		io_num = 19;
	}
	else if(fmt_type == DVP_FMT_DATA16)
	{
		io_num = 21;
	}
	for(uint32_t i = 0; i < io_num; i++)
	{
		if(i == 4 || i == 3/* || i > 12*/)
			continue;
		GTRY_GPIO->GPIO_CFG[offset_no+i] &= ~(RY_MAX_REUSE << GPIO_CFG_REUSE_POS);
		GTRY_GPIO->GPIO_CFG[offset_no+i] |= ((RY_DVP_REUSE << GPIO_CFG_REUSE_POS)|(RY_GPIO_INPUT_EN << GPIO_CFG_IN_EN_POS));
	}
	return 0;
}


uint32_t dvp_module_init (uint32_t scaler, uint32_t data_sel, uint32_t mode_sel, p_dvp_int_cb dvp_int_cb)
{
	GTRY_DVP->DVP_CAPTURE = 0x0;//dvp module disable capture
	GTRY_DVP->DVP_SCALER = scaler;
	GTRY_DVP->DVP_ENABLE = 0x1;//|= 0x2;//enable xclk output
	GTRY_DVP->DVP_FIFO = 0x1;//clear fofo
	GTRY_DVP->DVP_DMA_BURST = 0x0;//1 bytes trans/4 bytes trans/8 bytes trans
	GTRY_DVP->DVP_CONFIG = ADDR_SWITCH_HANDLE|DVP_VSP_SEL(0)|DVP_HSP_SEL(0)|DVP_CKP_SEL(0)|DVP_MODE_SEL(mode_sel);//0--DE mode/1--VGA mode//2--frame mode
	GTRY_DVP->DVP_DATA_FORMAT = /*DATA_FMT_BGR|*/DATA_FMT_SEL(2)/*|DATA_FMT_MSB*/|DATA_PIN_SET(data_sel);

	if(mode_sel == 1)
	{
		GTRY_DVP->DVP_VS_TIMING = 5;//vsync must more than (5+1) pclks;
	}

	GTRY_DVP->DVP_INT_EN = 0x0;
	//if(GTRY_DVP->DVP_INT_STATUS)
	{
		GTRY_DVP->DVP_INT_CLR = GTRY_DVP->DVP_INT_STS;//0x1;
	}

	if(dvp_int_cb != NULL)
	{
		GTRY_DVP->DVP_INT_STS = 0x1f;
		GTRY_DVP->DVP_INT_EN = OVER_INT_MASK/*|HSI_INT_MASK*/|VSI_INT_MASK|SWITCH_INT_MASK|CFDI_INT_MASK;//0x3;//0x1;//
		dvp_drv_int_callback = dvp_int_cb;

		// Priority must be set > 0 to trigger the interrupt
		__nds__plic_set_priority(IRQ_DVP_SOURCE, 2);//1

		// Enable PLIC interrupt PIT source
		__nds__plic_enable_interrupt(IRQ_DVP_SOURCE);
	}
	else
	{
		GTRY_DVP->DVP_INT_STS = 0x1f;
		GTRY_DVP->DVP_INT_EN = 0x0;//
		dvp_drv_int_callback = NULL;
		// Disable PLIC interrupt PIT source
		__nds__plic_disable_interrupt(IRQ_DVP_SOURCE);
	}
	return 0;
}

uint32_t dvp_set_ppa_buffer (uint32_t DP_DataA, uint32_t DP_DataB, uint32_t once_size)
{
	GTRY_DVP->DVP_DP_ADDRA = (uint32_t)DP_DataA;
	GTRY_DVP->DVP_DP_CNTA = once_size>>1;
	GTRY_DVP->DVP_DP_ADDRB = (uint32_t)DP_DataB;
	GTRY_DVP->DVP_DP_CNTB = once_size>>1;
	dvp_ppa_len = once_size;

	return 0;
}

uint32_t dvp_get_camera_start (uint32_t hlen, uint32_t vlen, uint32_t frame_num)
{
	if((hlen*vlen)%dvp_ppa_len)
	{
		return 1;
	}

	GTRY_DVP->DVP_DE_CNT = vlen;

	GTRY_DVP->DVP_FIFO = 0x1;//clear fofo
	GTRY_DVP->DVP_INT_STS = 0x1f;
	if(dvp_drv_int_callback != NULL)
	{
		GTRY_DVP->DVP_INT_EN = OVER_INT_MASK/*|HSI_INT_MASK*/|VSI_INT_MASK|SWITCH_INT_MASK|CFDI_INT_MASK;//0x3;//0x1;//
	}
	else
	{
		GTRY_DVP->DVP_INT_EN = 0x0;//
	}
	GTRY_DVP->DVP_CONFIG |= /*ADDR_SWITCH_HANDLE|*/RAM_AB_SWITCH_EN|DVP_CAPTURE_SEL(1);//0x6;//switch ramB and ramB, and start capture/receive
	if(frame_num > 1)
	{
		GTRY_DVP->DVP_CONFIG &= ~DVP_CAPTURE_SEL(1);
	}
	GTRY_DVP->DVP_CAPTURE = DVP_CAPTURE_EN;

	dvp_ppa_cnt = 0x0;
	dvp_vs_num = frame_num;
	dvp_ppa_num = hlen*vlen/dvp_ppa_len;

//	GTRY_GPIO->GPA_DOUTCLEAR[DVP_PWDN_GPIOA_PORT] = 0x1ul<<DVP_PWDN_GPIOA_PIN;

	return 0;
}

uint32_t dvp_handle_camera_stop (void)
{
	GTRY_DVP->DVP_INT_EN = 0x0;
	GTRY_DVP->DVP_CAPTURE = 0x0;

	dvp_ppa_cnt = 0x0;
	dvp_vs_num = 0x0;

	return 0;
}

uint32_t dvp_wait_camera_stop (void)
{
	if(GTRY_DVP->DVP_INT_EN == 0x0)
	{
		GTRY_DVP->DVP_CAPTURE = 0x0;
		dvp_ppa_cnt = 0x0;
		dvp_vs_num = 0x0;

		return 0;
	}

	return 1;
}

void dvp_ppa_size_update(unsigned int ppa_size)
{
	if(GTRY_DVP->DVP_STATE&0x1)
	{
		GTRY_DVP->DVP_DP_CNTA = ppa_size>>1;
	}
	else
	{
		GTRY_DVP->DVP_DP_CNTB = ppa_size>>1;
	}
}


// 假设有一个用于处理VGA同步中断的中断服务例程
void dvp_irq_handler(void)
{
	uint32_t status = GTRY_DVP->DVP_INT_STS;

	if(status&SWITCH_INT_MASK)
	{
//		GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_SWITCH;
		if(++dvp_ppa_cnt == (dvp_ppa_num))
		{
			dvp_ppa_cnt = 0x0;
			--dvp_vs_num;
		}
		if(dvp_vs_num == 0x0)
		{
			GTRY_DVP->DVP_CAPTURE = 0x0;
			GTRY_DVP->DVP_INT_EN = 0x0;
		}
		else if(dvp_ppa_cnt == (dvp_ppa_num-1) && dvp_vs_num == 0x1)
		{
			GTRY_DVP->DVP_CONFIG &= ~RAM_AB_SWITCH_EN;//disable switch ramB and ramB

			dvp_drv_int_callback(0x2);
		}
		else
		{
			GTRY_DVP->DVP_CONFIG |= RAM_AB_SWITCH_EN;//switch ramB and ramB
			dvp_drv_int_callback(GTRY_DVP->DVP_STATE&0x1);
		}
	}
	/*else */if(status&CFDI_INT_MASK)//0x2)
	{
		GTRY_DVP->DVP_INT_EN = 0x0;
		GTRY_DVP->DVP_CONFIG &= ~RAM_AB_SWITCH_EN;
	}
	/*else */if(status&VSI_INT_MASK)//0x2)
	{
		//GTRY_DVP->DVP_CONFIG &= ~RAM_AB_SWITCH_EN;
		dvp_drv_int_callback(0x2);
	}
	/*else */if(status&OVER_INT_MASK)//0x2)
	{
		GTRY_DVP->DVP_CONFIG &= ~RAM_AB_SWITCH_EN;
	}
	GTRY_DVP->DVP_INT_CLR = status;
}


/*****END OF FILE****/
