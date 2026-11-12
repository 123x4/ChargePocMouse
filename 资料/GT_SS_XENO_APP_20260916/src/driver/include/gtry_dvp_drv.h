/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

#ifndef __GTRY_DVP_DRV_H__
#define	__GTRY_DVP_DRV_H__


/* Includes ------------------------------------------------------------------*/
#include "gtry_regmap.h"
#include "gtry_platform.h"
#include "gtry_drv_common.h"

/******************************* ILI9341 显示屏8080通讯引脚定义 ***************************/

//复用DVP的起始引脚
#define DVP_R0_GPIOA_FIRST      (0)//GPIOA[0]--PA0
#define DVP_R1_GPIOA_FIRST      (32)//GPIOA[0]--PA32

#define ADDR_SWITCH_HANDLE		(0x1ul<<17)//自动切换RAMADDR模式，RAM地址切换后不会拉低RAMAB_EN位；
#define RAM_AB_SWITCH_EN		(0x1ul<<16)//使能该位后，在对RAMA传输完成后切换到RAMB。
#define LCD_CAPTURE_SEL(val)	((val&0x1ul)<<15)//基本型图像行捕获舍弃选择
#define LCD_CAPTURE_CTRL(val)	((val&0x1ul)<<14)//基本型图像行捕获舍弃控制功能
#define PCD_CAPTURE_SEL(val)	((val&0x1ul)<<13)//基本型像素行捕获舍弃选择
#define PCD_CAPTURE_CTRL(val)	((val&0x3ul)<<11)//基本型像素行捕获舍弃控制功能
#define DVP_BFR_CTRL(val)		((val&0x3ul)<<9)//基本型帧率控制功能
#define DVP_VSP_SEL(val)		((val&0x1ul)<<8)//该位在DE模式下无效，配置该位选择vsync有效电平：
#define DVP_HSP_SEL(val)		((val&0x1ul)<<7)//配置该位来选择hsync有效电平
#define DVP_CKP_SEL(val)		((val&0x1ul)<<6)//配置该位选择DVP模块在PCLK的上升沿还是下降沿进行捕获数据：
#define DVP_JPEG_SEL(val)		((val&0x1ul)<<5)//JPEG 格式
#define DVP_CUT_EN				(0x1ul<<4)//剪裁窗口功能使能
#define DVP_MODE_SEL(val)		((val&0x3ul)<<2)//DVP模式选择：DE模式、VGA模式、帧同步模式
#define DVP_CAPTURE_SEL(val)	((val&0x1ul)<<1)//捕获功能模式
#define DVP_CAPTURE_EN			(0x1ul<<0)//捕获功能使能

#define DATA_FMT_BGR			(0x1ul<<10)//BGR数据排序格式
#define DATA_FMT_SEL(val)		((val&0x3ul)<<8)//数据源格式, 进阶YUV422、常规YUV422、RGB565/RGB55、进阶Y8
#define DATA_FMT_NOUSE(val)		((val&0x3ul)<<4)//无未使用数据、2位未使用数据、4位未使用数据、6位未使用数据
#define DATA_FMT_LSB			(0x1ul<<3)//MSB:0, LSB:1
#define DATA_PIN_SET(val)		((val&0x7ul)<<0)//8bit[7:0]、10bit[9:0]、12bit[11:0]、14bit[13:0]、16bit[15:0]

#define OVER_INT_MASK			(0x1ul<<4)//缓存溢出中断，使能该位触发缓存溢出中断。
#define HSI_INT_MASK			(0x1ul<<3)//缓存溢出中断，使能该位触发缓存溢出中断。
#define VSI_INT_MASK			(0x1ul<<2)//垂直同步中断掩码位，使能该位触发垂直同步中断。
#define SWITCH_INT_MASK			(0x1ul<<1)//Ram地址切换中断掩码位，使能该位触发中断。
#define CFDI_INT_MASK			(0x1ul<<0)//捕获中断掩码位，使能该位触发中断。

/**
  * @brief  RGB 层像素格式
  */
typedef enum
{
	DVP_FMT_DATA8  = 0x00, /* 00：数据位 8 位 */
	DVP_FMT_DATA10,		/* 01：数据位 10 位 */
	DVP_FMT_DATA12,		/* 10：数据位 12 位 */
	DVP_FMT_DATA14,		/* 10：数据位 14 位 */
	DVP_FMT_DATA16,		/* 10：数据位 16 位 */
	DVP_FMT_MAX
}DVP_Fmt_TypeDef;

//DVP
typedef enum
{
	DVP_IO_GP0 = 0,
	DVP_IO_GP1,
	DVP_IO_GP_MAX,
}dvp_multi_io;

/**
  \fn          void p_dvp_int_cb (uint32_t event)
  \brief       dvp Events.
  \param[in]   event  dvp Event mask
  \return      none
*/
typedef void (*p_dvp_int_cb) (uint32_t pps_sts);


extern uint32_t dvp_io_multi_config(dvp_multi_io dvp_group, DVP_Fmt_TypeDef fmt_type);
extern uint32_t dvp_module_init (uint32_t scaler, uint32_t data_sel, uint32_t mode_sel, p_dvp_int_cb dvp_int_cb);
extern uint32_t dvp_set_ppa_buffer (uint32_t DP_DataA, uint32_t DP_DataB, uint32_t once_size);
extern uint32_t dvp_get_camera_start (uint32_t hlen, uint32_t vlen, uint32_t frame_num);
extern uint32_t dvp_handle_camera_stop (void);
extern uint32_t dvp_wait_camera_stop (void);

#endif /* __GTRY_DVP_DRV_H__ */
