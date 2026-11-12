/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

  
/* Includes ------------------------------------------------------------------*/
#include "gtry_r8080_drv.h"
#include "gtry_gpio_drv.h"
#include "gtry_dma_drv.h"
#include "gtry_qspi_flash_drv.h"


volatile unsigned int PPA_FillPixel_flag = 0x0;//0x1;
volatile unsigned int PPA_FillPixel_frame_size = 0x0;
volatile unsigned int PPA_FillPixel_once_size = 0x0;

p_r8080_int_cb r8080_drv_int_callback = NULL;			/** r8080中断回调函数入口 */


/**
  * @brief  用于R8080驱动简单延时函数
  * @param  nCount ：延时计数值
  * @retval 无
  */
static void R8080_Delay ( __IO unsigned int nCount )
{
	do{}while(nCount --);
}


unsigned int r8080_io_multi_config(r8080_multi_io r8080_group, Core_IM_Types IM_types)
{
	uint32_t offset_no = r8080_group*32;
	uint32_t io_num = 20;

	if(r8080_group >= R8080_IO_GP_MAX || IM_types >= IM_MODE_MAX)
	{
		return 1;
	}

	if((IM_types & IM_I_18BIT_MODE) == IM_I_8BIT_MODE)
	{
		io_num = 12;
	}
	else if((IM_types & IM_I_18BIT_MODE) == IM_I_9BIT_MODE)
	{
		io_num = 13;
	}
	else if((IM_types & IM_I_18BIT_MODE) == IM_I_18BIT_MODE)
	{
		io_num = 22;
	}

	for(uint32_t i = 0; i < io_num; i++)
	{
		GTRY_GPIO->GPIO_CFG[offset_no+i] &= ~(RY_MAX_REUSE << GPIO_CFG_REUSE_POS);
		GTRY_GPIO->GPIO_CFG[offset_no+i] |= ((RY_R8080_REUSE << GPIO_CFG_REUSE_POS)|(RY_GPIO_INPUT_EN << GPIO_CFG_IN_EN_POS));
	}
	return 0;
}


void r8080_module_init (uint32_t scaler, uint32_t fmt_mask)
{
/*
   *   该字段配置WR、RD 信号的的脉冲宽度，最小配置值为1’b1，
   *  实际分频值为配置值+1’b1，WR、RD低电平时间如果该字段为奇数那么为配置值-1除以2个APB周期，
   *  如果该字段为偶数那么为低电平时间为配置值/2个APB时钟周期，配置为6’h0无效。
 * */
	GTRY_R8080->R8080_CS_EN = R8080_CS_ENABLE(1);//结束片选
	GTRY_R8080->R8080_CORE_CTRL = R8080_CTRL_BURST(2)|R8080_CTRL_ENDAUTO(1)|R8080_CTRL_DUMMY(1)|/*R8080_CTRL_ADDR(0)|*/R8080_CTRL_MODE(1);
	GTRY_R8080->R8080_DATA_FMT = fmt_mask;
	GTRY_R8080->R8080_SCALER_CFG = R8080_CLK_SCALER(scaler);
	GTRY_R8080->R8080_FIFO_STS = R8080_TX_CLEAR;
	GTRY_R8080->R8080_CORE_STS = R8080_STATUS_ADDR(1)|R8080_STATUS_WRITE(1)|R8080_STATUS_READ(1);
	GTRY_R8080->R8080_INT_CLR = R8080_INT_TSF_MASK|R8080_INT_SWITCH_MASK;
	GTRY_R8080->R8080_BUF_CNTA = 0x0;
	GTRY_R8080->R8080_BUF_ADDRA = 0x0;
	GTRY_R8080->R8080_BUF_CNTB = 0x0;
	GTRY_R8080->R8080_BUF_ADDRB = 0x0;
}


/**
 * @brief  读取LCD驱动芯片ID函数，可用于测试底层的读写函数
 * @param  无
 * @retval 正常时返回值为LCD驱动芯片ID: LCDID_ILI9341/LCDID_ST7789V/LCDID_ST7789P3
 *         否则返回: LCDID_UNKNOWN
 */
uint32_t r8080_read_device_ID(uint32_t rdid_cmd)
{
	uint32_t id = 0;

	GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_MODE(1);//handle mode
	GTRY_R8080->R8080_CORE_CTRL &= ~R8080_CTRL_WR(1);//enable read cmd;

	GTRY_R8080->R8080_CMD_CFG = rdid_cmd;

	GTRY_R8080->R8080_SCALER_CFG = GTRY_R8080->R8080_SCALER_CFG<<1;

	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_IDLE(1)) == 0x0);
	R8080_ENABLE_CS();//开始片选

	while((GTRY_R8080->R8080_CORE_STS&(R8080_STATUS_WRITE(1)|R8080_STATUS_READ(1))) != (R8080_STATUS_WRITE(1)|R8080_STATUS_READ(1)));

	R8080_Delay(GTRY_R8080->R8080_SCALER_CFG);
	id = GTRY_R8080->R8080_WR_DATA&0xff; //去掉前一次读取结果
//	info("0x%x ", id);
	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_READ(1)) == 0x0);
	R8080_Delay(GTRY_R8080->R8080_SCALER_CFG);
	id = (id<<8)|(GTRY_R8080->R8080_WR_DATA&0xff);
//	info("0x%x ", id);
	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_READ(1)) == 0x0);
	R8080_Delay(GTRY_R8080->R8080_SCALER_CFG);
	id = (id<<8)|(GTRY_R8080->R8080_WR_DATA&0xff);
//	info("0x%x ", id);
	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_READ(1)) == 0x0);
	R8080_Delay(GTRY_R8080->R8080_SCALER_CFG);
	id = (id<<8)|(GTRY_R8080->R8080_WR_DATA&0xff);
//	info("0x%x\r\n", id);
	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_READ(1)) == 0x0);
	R8080_Delay(GTRY_R8080->R8080_SCALER_CFG);
	id = (id<<8)|(GTRY_R8080->R8080_WR_DATA&0xff);

	R8080_DISABLE_CS();//结束片选

	GTRY_R8080->R8080_SCALER_CFG = GTRY_R8080->R8080_SCALER_CFG>>1;

	return id;
}

/**
 * @brief  读取 LCD 的数据
 * @param  无
 * @retval 像素数据
 */
uint32_t r8080_read_data (uint32_t cmd, uint8_t *data, uint32_t size)
{
	GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_MODE(1);//handle mode
	GTRY_R8080->R8080_CORE_CTRL &= ~R8080_CTRL_WR(1);//enable read cmd;

	GTRY_R8080->R8080_CMD_CFG = cmd;

	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_IDLE(1)) == 0x0);
	R8080_ENABLE_CS();//开始片选

	while((GTRY_R8080->R8080_CORE_STS&(R8080_STATUS_WRITE(1)|R8080_STATUS_READ(1))) != (R8080_STATUS_WRITE(1)|R8080_STATUS_READ(1)));
	data[0] = GTRY_R8080->R8080_WR_DATA; //去掉前一次读取结果

	for(uint32_t i = 0; i < size; i++)
	{
		while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_READ(1)) == 0x0);
		data[i] = GTRY_R8080->R8080_WR_DATA;
	}

	R8080_DISABLE_CS();//结束片选
	return size;
}

/**
  * @brief  向LCD驱动芯片写入命令、数据
  * @param  Cmd :要写入的命令
  * @param  data :要写入的数据指针
  * @param  size :要写入的数据长度
  * @retval 无
  */
void r8080_write_data (uint32_t cmd, uint8_t *data, uint32_t size)
{
	GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_MODE(1);//handle mode
	GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_WR(1);//enable write cmd;

	GTRY_R8080->R8080_CMD_CFG = cmd;//输出命令

	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_IDLE(1)) == 0x0);
	R8080_ENABLE_CS();//开始片选

	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_WRITE(1)) == 0x0);
	R8080_Delay(GTRY_R8080->R8080_SCALER_CFG);
	if(size)
	{
		for(uint32_t i = 0; i < size; i ++)
		{
			GTRY_R8080->R8080_WR_DATA = data[i];
			while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_WRITE(1)) == 0x0);
		}
	}

	R8080_DISABLE_CS();//结束片选
}

void r8080_manual_fill_color (uint32_t pixel_cmd, uint32_t pixel_size, uint16_t pixel_color)
{
	uint32_t i = 0;

	GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_MODE(1);//handle mode
	GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_WR(1);//enable write cmd;

	GTRY_R8080->R8080_CMD_CFG = pixel_cmd;

	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_IDLE(1)) == 0x0);
	R8080_ENABLE_CS();//开始片选

	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_WRITE(1)) == 0x0);
	R8080_Delay(GTRY_R8080->R8080_SCALER_CFG);
	if(GTRY_R8080->R8080_DATA_FMT & R8080_CTRL_IM(IM_I_16BIT_MODE))
	{
		for(i = 0; i < pixel_size; i++)
		{
			GTRY_R8080->R8080_WR_DATA = pixel_color;
			while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_WRITE(1)) == 0x0);
		}
	}
	else
	{
		for(i = 0; i < pixel_size; i++)
		{
			GTRY_R8080->R8080_WR_DATA = pixel_color&0xff;
			while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_WRITE(1)) == 0x0);
			GTRY_R8080->R8080_WR_DATA = (pixel_color>>8)&0xff;
			while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_WRITE(1)) == 0x0);
		}
	}

	R8080_DISABLE_CS();//结束片选
}


uint32_t r8080_ppa_fill_color (uint32_t pixel_cmd, uint32_t pixel_size, uint16_t pixel_color)
{
	uint32_t i = 0, ret = 0x0;
	uint32_t timeout = 10000, next_rdsize = 0x0;
	uint16_t ppa_bufA[512] = {0x0};
	uint16_t ppa_bufB[512] = {0x0};

	if(pixel_size == 0x0)
	{
		return 1;
	}

	for(i = 0; i < 512; i ++)
	{
		ppa_bufA[i] = pixel_color;
		ppa_bufB[i] = pixel_color;
	}
	next_rdsize = pixel_size > 512? 512:pixel_size;
	GTRY_R8080->R8080_BUF_ADDRA = (unsigned int)ppa_bufA|0xa0000000;
	GTRY_R8080->R8080_BUF_CNTA = next_rdsize>>1;

	pixel_size -= next_rdsize;
	if(pixel_size)
	{
		next_rdsize = pixel_size > 512? 512:pixel_size;
	    GTRY_R8080->R8080_BUF_ADDRB = (unsigned int)ppa_bufB|0xa0000000;
	    GTRY_R8080->R8080_BUF_CNTB = next_rdsize>>1;
//		GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_SWITCH;
	}
	else
	{
		GTRY_R8080->R8080_BUF_CNTB = 0x0;
		GTRY_R8080->R8080_BUF_ADDRB = 0x0;
	}

    // 复位FIFO
	GTRY_R8080->R8080_FIFO_STS = R8080_TX_CLEAR;
	GTRY_R8080->R8080_CORE_CTRL &= ~R8080_CTRL_MODE(1);
	GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_WR(1);//enable write cmd;
	GTRY_R8080->R8080_CORE_STS = R8080_STATUS_ADDR(1)|R8080_STATUS_WRITE(1)|R8080_STATUS_READ(1);
	GTRY_R8080->R8080_INT_EN = 0x0;
	GTRY_R8080->R8080_INT_CLR = R8080_INT_TSF_MASK|R8080_INT_SWITCH_MASK;

	// Enable PLIC interrupt PIT source
	__nds__plic_disable_interrupt(IRQ_R8080_SOURCE);

	GTRY_R8080->R8080_CMD_CFG = pixel_cmd;

	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_IDLE(1)) == 0x0);
	R8080_ENABLE_CS();//开始片选

	do{
		while((GTRY_R8080->R8080_INT_STS & R8080_INT_SWITCH_MASK) == 0x0  && --timeout)
		{
			if(GTRY_R8080->R8080_INT_STS & R8080_INT_TSF_MASK)
			{
				GTRY_R8080->R8080_INT_CLR = R8080_INT_TSF_MASK;
			}
		}
		GTRY_R8080->R8080_INT_CLR = R8080_INT_SWITCH_MASK;

		if(timeout == 0x0)
		{
			R8080_DISABLE_CS();
			ret = 2;
			break;
		}
		else if(pixel_size == 0x0)
		{
			while((GTRY_R8080->R8080_INT_STS&R8080_INT_TSF_MASK) == 0x0);
			GTRY_R8080->R8080_INT_CLR = R8080_INT_TSF_MASK;
			R8080_DISABLE_CS();
			break;
		}

		timeout = 10000;
		pixel_size -= next_rdsize;
		if(pixel_size)
		{
			next_rdsize = pixel_size > 512? 512:pixel_size;
			//GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_SWITCH;//switch ramB and ramB//ok before set addr/cnt
			if(GTRY_R8080->R8080_CORE_STS&R8080_STATUS_ADDR(1))
			{
				GTRY_R8080->R8080_BUF_CNTA = next_rdsize>>1;
			}
			else
			{
				GTRY_R8080->R8080_BUF_CNTB = next_rdsize>>1;
			}
		}
	}while(1);

	return ret;
}

void r8080_ppa_fillpixel_int_init(p_r8080_int_cb r8080_int_cb)
{
	if(r8080_int_cb == NULL)
	{
		GTRY_R8080->R8080_INT_EN = 0x0;
		GTRY_R8080->R8080_INT_CLR = R8080_INT_CSEND_MASK|R8080_INT_TSF_MASK|R8080_INT_SWITCH_MASK;
		r8080_drv_int_callback = NULL;

		// Disable PLIC interrupt PIT source
		__nds__plic_disable_interrupt(IRQ_R8080_SOURCE);
	}
	else
	{
		GTRY_R8080->R8080_INT_EN = R8080_INT_CSEND_MASK|/*R8080_INT_TSF_MASK|*/R8080_INT_SWITCH_MASK;;
		GTRY_R8080->R8080_INT_CLR = R8080_INT_CSEND_MASK|R8080_INT_TSF_MASK|R8080_INT_SWITCH_MASK;
		r8080_drv_int_callback = r8080_int_cb;

		// Priority must be set > 0 to trigger the interrupt
		__nds__plic_set_priority(IRQ_R8080_SOURCE, 2);//1

		// Enable PLIC interrupt PIT source
		__nds__plic_enable_interrupt(IRQ_R8080_SOURCE);
	}
}

uint32_t r8080_ppa_fillpixel_start (uint32_t pixel_cmd, uint32_t frame_size, uint32_t pixel_dataA, uint32_t pixel_dataB, uint32_t pixel_once_size)
{
	if(frame_size == 0x0 || pixel_once_size == 0x0 || frame_size < pixel_once_size)
	{
		return 1;
	}
	PPA_FillPixel_flag = 0x0;
	PPA_FillPixel_frame_size = frame_size;
	PPA_FillPixel_once_size = pixel_once_size;

    GTRY_R8080->R8080_BUF_ADDRA = pixel_dataA;
	GTRY_R8080->R8080_BUF_CNTA = PPA_FillPixel_once_size>>1;
	GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_SWITCH;

	PPA_FillPixel_frame_size -= PPA_FillPixel_once_size;
	if(PPA_FillPixel_frame_size)
	{
		GTRY_R8080->R8080_BUF_ADDRB = pixel_dataB;
		GTRY_R8080->R8080_BUF_CNTB = 0x0;
	}
	else
	{
		GTRY_R8080->R8080_BUF_CNTB = 0x0;
		GTRY_R8080->R8080_BUF_ADDRB = 0x0;
	}

	// 复位FIFO
	GTRY_R8080->R8080_FIFO_STS = R8080_TX_CLEAR;
	GTRY_R8080->R8080_CORE_CTRL &= ~R8080_CTRL_MODE(1);
	GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_WR(1);//enable write cmd;
	GTRY_R8080->R8080_CORE_STS = R8080_STATUS_ADDR(1)|R8080_STATUS_WRITE(1)|R8080_STATUS_READ(1);

	GTRY_R8080->R8080_CMD_CFG = pixel_cmd;

	while((GTRY_R8080->R8080_CORE_STS&R8080_STATUS_IDLE(1)) == 0x0);
	R8080_ENABLE_CS();//开始片选

	return 0;
}



uint32_t r8080_ppa_fillpixel_continue (uint32_t block_times, uint32_t pixel_size)
{
//	unsigned int timeout = 10000;
	unsigned int tmp_size = pixel_size;

	if(PPA_FillPixel_frame_size == 0x0)
	{
		return 1;
	}

//	if(block_en == 0x0 && (GTRY_R8080->R8080_INT_STATUS&R8080_INT_SWITCH_MASK) == 0x0)
//	{
//		return 2;
//	}
	while((GTRY_R8080->R8080_INT_STS&R8080_INT_SWITCH_MASK) == 0x0 && --block_times);

	if(block_times == 0x0)
	{
//		R8080_DISABLE_CS();
		return 2;
	}
	GTRY_R8080->R8080_INT_CLR = R8080_INT_SWITCH_MASK|R8080_INT_TSF_MASK;

	if(PPA_FillPixel_frame_size < pixel_size)
	{
		tmp_size = PPA_FillPixel_frame_size;
	}

	if(GTRY_R8080->R8080_CORE_STS&R8080_STATUS_ADDR(1))
	{
		GTRY_R8080->R8080_BUF_CNTB = tmp_size>>1;
	}
	else
	{
		GTRY_R8080->R8080_BUF_CNTA = tmp_size>>1;
	}
	PPA_FillPixel_frame_size -= tmp_size;

	return 0;
}


uint32_t r8080_ppa_fill_pixel_force_end (void)
{
	PPA_FillPixel_flag = 0x1;
	PPA_FillPixel_frame_size = 0x0;
	PPA_FillPixel_once_size = 0x0;

	R8080_DISABLE_CS();

	GTRY_R8080->R8080_BUF_CNTA = 0x0;
	GTRY_R8080->R8080_BUF_ADDRA = 0x0;
	GTRY_R8080->R8080_BUF_CNTB = 0x0;
	GTRY_R8080->R8080_BUF_ADDRB = 0x0;
	GTRY_R8080->R8080_INT_CLR = R8080_INT_CSEND_MASK|R8080_INT_TSF_MASK|R8080_INT_SWITCH_MASK;

	return 0;
}


r8080_ppa_status r8080_ppa_fill_pixel_check_end (void)
{
	unsigned int timeout = 10000;

	if(PPA_FillPixel_flag == 1)
	{
		return R8080_PPA_CHK_DONE;
	}
	else if(PPA_FillPixel_frame_size)
	{
		return R8080_PPA_CHK_SIZE_IN;
	}

	if(r8080_drv_int_callback == NULL)
	{
		if((GTRY_R8080->R8080_INT_STS&R8080_INT_TSF_MASK) == 0x0)
		{
			return R8080_PPA_CHK_SIZE_OUT;
		}
		GTRY_R8080->R8080_INT_CLR = R8080_INT_CSEND_MASK|R8080_INT_TSF_MASK|R8080_INT_SWITCH_MASK;
		R8080_DISABLE_CS();
		while((GTRY_R8080->R8080_INT_STS&R8080_INT_CSEND_MASK) == 0x0 && --timeout);
		if(timeout == 0)
		{
			return R8080_PPA_CHK_TIME_OUT;
		}
		PPA_FillPixel_flag = 1;
		GTRY_R8080->R8080_INT_CLR = R8080_INT_CSEND_MASK;//|R8080_INT_TSF_MASK|R8080_INT_SWITCH_MASK;
		GTRY_R8080->R8080_BUF_CNTA = 0x0;
		GTRY_R8080->R8080_BUF_ADDRA = 0x0;
		GTRY_R8080->R8080_BUF_CNTB = 0x0;
		GTRY_R8080->R8080_BUF_ADDRB = 0x0;

		return R8080_PPA_CHK_DONE;
	}
	else
	{
		return R8080_PPA_CHK_SIZE_OUT;
	}
}


uint32_t r8080_ppa_fillpixel_size(void)
{
	return PPA_FillPixel_frame_size;
}


uint32_t r8080_ppa_fillpixel_status(void)
{
	return GTRY_R8080->R8080_CORE_STS&R8080_STATUS_ADDR(1);
}

// R8080中断服务函数
void r8080_irq_handler(void) {

	unsigned int next_rdsize = 0x0;
	unsigned int int_status = GTRY_R8080->R8080_INT_EN&GTRY_R8080->R8080_INT_STS;

	if(int_status&R8080_INT_SWITCH_MASK)
	{
		if(PPA_FillPixel_frame_size)
		{
		    next_rdsize = (PPA_FillPixel_frame_size > PPA_FillPixel_once_size? PPA_FillPixel_once_size:PPA_FillPixel_frame_size);
		    if(GTRY_R8080->R8080_CORE_STS&R8080_STATUS_ADDR(1))
		    {
		    	GTRY_R8080->R8080_BUF_CNTB = next_rdsize>>1;
		    }
		    else
		    {
		    	GTRY_R8080->R8080_BUF_CNTA = next_rdsize>>1;
		    }
		    PPA_FillPixel_frame_size -= next_rdsize;
			if(PPA_FillPixel_frame_size)
			{
			    next_rdsize = (PPA_FillPixel_frame_size > PPA_FillPixel_once_size? PPA_FillPixel_once_size:PPA_FillPixel_frame_size);
				r8080_drv_int_callback(GTRY_R8080->R8080_CORE_STS&R8080_STATUS_ADDR(1), next_rdsize);
				//GTRY_R8080->R8080_CORE_CTRL |= R8080_CTRL_SWITCH;//switch ramB and ramB
			}
		}
		else
		{
			GTRY_R8080->R8080_CORE_CTRL &= ~R8080_CTRL_SWITCH;//disable switch ramB and ramB
			GTRY_R8080->R8080_INT_EN |= R8080_INT_TSF_MASK;
		}
	}
	/*else */if(int_status&R8080_INT_TSF_MASK)
	{
		R8080_DISABLE_CS();
	}
	/*else */if(int_status&R8080_INT_CSEND_MASK)
	{
		PPA_FillPixel_flag = 0x1;
		PPA_FillPixel_frame_size = 0;
		PPA_FillPixel_once_size = 0;
		GTRY_R8080->R8080_BUF_CNTA = 0;
		GTRY_R8080->R8080_BUF_CNTB = 0;
	}
    // 清除中断标志
    GTRY_R8080->R8080_INT_CLR = int_status;
}

/******END OF FILE****/
