/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

#ifndef __GTRY_R8080_DRV_H__
#define	__GTRY_R8080_DRV_H__


/* Includes ------------------------------------------------------------------*/
#include "gtry_platform.h"
#include "gtry_drv_common.h"

/* ----- Control register                                             */

                                /* bits 31-16 are reserved            */
#define R8080_CTRL_IM(m)		((unsigned long)(m)<<8)		/* 配置输出模式 bits 8-11 *//* 配置输出模式 bits 12-15 */
typedef enum
{
  IM_I_8BIT_MODE             = 0x00ul,    /* I8080 8-bit bus interface */
  IM_I_16BIT_MODE              = 0x01ul,    /* I8080 16-bit bus interface    */
  IM_I_9BIT_MODE               = 0x02ul,     /* I8080 9-bit bus interface   */
  IM_I_18BIT_MODE               = 0x03ul,     /* I8080 18-bit bus interface    */
  IM_II_8BIT_MODE             = 0x08ul,    /* II8080 8-bit bus interface */
  IM_II_16BIT_MODE              = 0x09ul,    /* II8080 16-bit bus interface    */
  IM_II_9BIT_MODE               = 0x0aul,     /* II8080 9-bit bus interface    */
  IM_II_18BIT_MODE               = 0x0bul,     /* II8080 18-bit bus interface    */
  IM_MODE_MAX
}Core_IM_Types;

#define R8080_CTRL_DBI(m)		((unsigned long)(m)<<2)		/* 选择数据格式 bits2-4*//* 选择数据格式 bits 9-11*/
typedef enum
{
  MODE_565             = 0x05ul,    /* 101 - 565 mode */
  MODE_666              = 0x06ul,    /* 110 - 666 mode   */
  OTHER     /* other - no use   */
}Core_DBI_Types;

#define R8080_CTRL_MDT(m)		((unsigned long)(m)<<0)		/* 在用来选择数据输出格式 模式6-6-6only bits 0-1 *//* 在用来选择数据输出格式 模式6-6-6only bits 7-8 */
typedef enum
{
  MDT_COMPACT             = 0x00ul,    /* 00 - 紧凑模式 */
  MDT_DISPERSED           = 0x01ul,    /* 01 - 分散模式  */
  MDT_FRONT              = 0x02ul,    /* 01 - 前对齐模式  */
  MDT_AFTER              = 0x03ul    /* 01 - 后对齐模式  */
}Core_MDI_Types;


#define R8080_CTRL_WR(m)  		((unsigned long)(m)<<0)      /* 选择模块读写模式                */
#define R8080_CTRL_MODE(m)  	((unsigned long)(m)<<1)      /* 选择模块输出输入模式                */
//#define R8080_CTRL_ADDR(m)  	((unsigned long)(m)<<2)      /* current addr A/B     */
#define R8080_CTRL_SWITCH	  	((unsigned long)(0x1)<<3)      /* switch addr A/B     */
#define R8080_CTRL_DUMMY(m)  	((unsigned long)(m)<<4)      /* dummy first data enable   */
#define R8080_CTRL_ENDAUTO(m)  	((unsigned long)(m)<<5)      /* auto cs end enable  */
#define R8080_CTRL_BURST(m)  	((unsigned long)(m)<<6)      /* burst size 0->1/1->4/2->8,no other   */

#define R8080_INT_CSEND_MASK  	((unsigned long)(0x1)<<2)      /* CS end interrupt enable. bit  */
#define R8080_INT_TSF_MASK  	((unsigned long)(0x1)<<1)      /* CS end interrupt enable. bit  */
#define R8080_INT_SWITCH_MASK  	((unsigned long)(0x1)<<0)      /* switch ram interrupt enable. bit  */

/* ----- tx status register                                             */
                                /* bits 31-2 are reserved            */
#define R8080_TX_CLEAR	 		((unsigned long)(0x1)<< 2)     /* TXFIFO clear   */
#define R8080_TX_EMPTY(m) 		((unsigned long)(m)<< 1)      /* TXFIFO empty status   */
#define R8080_TX_FULL(m) 		((unsigned long)(m)<< 0)      /* TXFIFO full status   */


/* ----- Configuration register                                             */
                                /* bits 31-1 are reserved            */
#define R8080_CS_ENABLE(m) 		((unsigned long)(m)<< 0)      /* 传输使能位, bit0 write 1B low CS , write 0B high CS   */

/* ----- clock scaler register                                             */
                                /* bits 31-6 are reserved            */
/*
   *  该字段配置WR、RD信号的的脉冲宽度，最小配置值为1’b1，
   *  实际分频值为配置值+1’b1，WR、RD低电平时间如果该字段为奇数那么为配置值-1除以2个APB周期，
   *  如果该字段为偶数那么为低电平时间为配置值/2个APB时钟周期，配置为6’h0无效。
 * */
#define R8080_CLK_SCALER(m)		((unsigned long)(m)<<0)     /* CLUT Bank Switch Enable bit        */

/* ----- R8080 Status register                                              */
                                /* bits 31-3 are reserved            */
#define R8080_STATUS_ADDR(m)  	((unsigned long)(m)<<3)      /* current addr A/B     */
#define R8080_STATUS_WRITE(m)	((unsigned long)(m)<<2)     /* 表示发送了一个命令或数据 */
#define R8080_STATUS_READ(m)	((unsigned long)(m)<<1)     /* 表示接收了一个数据 */
#define R8080_STATUS_IDLE(m)	((unsigned long)(m)<<0)		/* 表示空闲状态, 高为空闲, 低为忙 */

#define R8080_ENABLE_CS()		(GTRY_R8080->R8080_CS_EN = R8080_CS_ENABLE(0))
#define R8080_DISABLE_CS()		(GTRY_R8080->R8080_CS_EN = R8080_CS_ENABLE(1))
//#define R8080_GET_CS()		(GTRY_R8080->R8080_CS_EN)
#define R8080_CHK_BUSY_CS()		(GTRY_R8080->R8080_CS_EN == R8080_CS_ENABLE(0))
#define R8080_WR_SCALER(n)		(GTRY_R8080->R8080_SCALER_CFG = R8080_CLK_SCALER(n))


typedef void (*p_r8080_int_cb)(uint32_t ppa_sts, uint32_t pixal_size);  ///< Pointer to \ref p_r8080_int_cb : r8080 Event.


//R8080
typedef enum
{
	R8080_IO_GP0 = 0,
	R8080_IO_GP1,
	R8080_IO_GP_MAX
}r8080_multi_io;


//R8080
typedef enum
{
	R8080_PPA_CHK_DONE = 0,
	R8080_PPA_CHK_SIZE_IN,
	R8080_PPA_CHK_SIZE_OUT,
	R8080_PPA_CHK_TIME_OUT
}r8080_ppa_status;


extern unsigned int r8080_io_multi_config(r8080_multi_io r8080_group, Core_IM_Types IM_types);

extern void r8080_module_init (uint32_t scaler, uint32_t fmt_mask);
extern uint32_t r8080_read_device_ID(uint32_t rdid_cmd);
extern uint32_t r8080_read_data ( uint32_t cmd,  uint8_t *data, uint32_t size);
extern void r8080_write_data (uint32_t cmd, uint8_t *data, uint32_t size);
extern void r8080_manual_fill_color (uint32_t pixel_cmd, uint32_t pixel_size, uint16_t pixel_color);
extern uint32_t r8080_ppa_fill_color (uint32_t pixel_cmd, uint32_t pixel_size, uint16_t pixel_color);


extern void r8080_ppa_fillpixel_int_init(p_r8080_int_cb r8080_int_cb);
extern uint32_t r8080_ppa_fillpixel_start (uint32_t pixel_cmd, uint32_t frame_size, uint32_t pixel_dataA, uint32_t pixel_dataB, uint32_t pixel_once_size);
extern uint32_t r8080_ppa_fillpixel_continue (uint32_t block_times, uint32_t pixel_size);
extern r8080_ppa_status r8080_ppa_fill_pixel_check_end (void);
extern uint32_t r8080_ppa_fill_pixel_force_end (void);
extern uint32_t r8080_ppa_fillpixel_size(void);
extern uint32_t r8080_ppa_fillpixel_status(void);

#endif /* __GTRY_R8080_DRV_H__ */
