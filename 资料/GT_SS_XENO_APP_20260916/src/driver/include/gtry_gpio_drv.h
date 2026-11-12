/*
 *	gtry_aes_drv.h
 *
 *	Created on: 2025.10.23
 *
 */

#ifndef __GTRY_GPIO_DRV_H
#define __GTRY_GPIO_DRV_H

#include "gtry_platform.h"

/*******************************************************************************
  GPIO PIN MAP
*******************************************************************************/
#define 	PA_GROUP0_MAX 					32							/** GPIO PA_GROUP0最大IO个数 */
#define 	PA_GROUP1_MAX 					16							/** GPIO PA_GROUP1最大IO个数 */
#define 	PB_GROUP_MAX 					16							/** GPIO PB_GROUP最大IO个数 */

#define 	PA_GROUP0_MASK_ALL  			0xffffffff					/** GPIO PA_GROUP0的所有IO MASK */
#define 	PA_GROUP1_MASK_ALL  			0xffff						/** GPIO PA_GROUP1的所有IO MASK */
#define 	PB_GROUP_MASK_ALL  				0xffff						/** GPIO PB_GROUP的所有IO MASK */

#define 	PA_GROUP0 						0							/** GPIO PA_GROUP0的标识号 */
#define 	PA_GROUP1 						1							/** GPIO PA_GROUP1的标识号 */
#define 	PB_GROUP 						2							/** GPIO PB_GROUP的标识号 */
#define 	GPIO_GROUP_MAX					3							/** GPIO GROUP_MAX的标识号 */
#define 	GPIO_GROUP_MASK 				0x60						/** GPIO GROUP所有组的MASK */
#define 	GPIO_GROUP_POS 					5							/** GPIO GROUP的偏移位置 */
#define 	PA_GROUP0_MASK 					(PA_GROUP0<<GPIO_GROUP_POS)	/** GPIO PA_GROUP0组的MASK */
#define 	PA_GROUP1_MASK 					(PA_GROUP1<<GPIO_GROUP_POS)	/** GPIO PA_GROUP1组的MASK */
#define 	PB_GROUP_MASK 					(PB_GROUP<<GPIO_GROUP_POS)	/** GPIO PB_GROUP组的MASK */

#define 	GPIO_NUM_MASK 					0x1F						/** GPIO最大IO个数标识MASK */

/** GPIO GROUP1_A组中每个IO的标识 */
#define 	PA0			(PA_GROUP0_MASK|0x00)
#define 	PA1			(PA_GROUP0_MASK|0x01)
#define 	PA2			(PA_GROUP0_MASK|0x02)
#define 	PA3			(PA_GROUP0_MASK|0x03)
#define 	PA4			(PA_GROUP0_MASK|0x04)
#define 	PA5			(PA_GROUP0_MASK|0x05)
#define 	PA6			(PA_GROUP0_MASK|0x06)
#define 	PA7			(PA_GROUP0_MASK|0x07)
#define 	PA8			(PA_GROUP0_MASK|0x08)
#define 	PA9			(PA_GROUP0_MASK|0x09)
#define 	PA10		(PA_GROUP0_MASK|0x0A)
#define 	PA11		(PA_GROUP0_MASK|0x0B)
#define 	PA12		(PA_GROUP0_MASK|0x0C)
#define 	PA13		(PA_GROUP0_MASK|0x0D)
#define		PA14		(PA_GROUP0_MASK|0x0E)
#define		PA15		(PA_GROUP0_MASK|0x0F)
#define 	PA16		(PA_GROUP0_MASK|0x10)
#define 	PA17		(PA_GROUP0_MASK|0x11)
#define 	PA18		(PA_GROUP0_MASK|0x12)
#define 	PA19		(PA_GROUP0_MASK|0x13)
#define 	PA20		(PA_GROUP0_MASK|0x14)
#define 	PA21		(PA_GROUP0_MASK|0x15)
#define 	PA22		(PA_GROUP0_MASK|0x16)
#define 	PA23		(PA_GROUP0_MASK|0x17)
#define 	PA24		(PA_GROUP0_MASK|0x18)
#define 	PA25		(PA_GROUP0_MASK|0x19)
#define 	PA26		(PA_GROUP0_MASK|0x1A)
#define 	PA27		(PA_GROUP0_MASK|0x1B)
#define 	PA28		(PA_GROUP0_MASK|0x1C)
#define 	PA29		(PA_GROUP0_MASK|0x1D)
#define 	PA30		(PA_GROUP0_MASK|0x1E)
#define 	PA31		(PA_GROUP0_MASK|0x1F)

/** GPIO GROUP2_A组中每个IO的标识 */
#define 	PA32		(PA_GROUP1_MASK|0x00)
#define 	PA33		(PA_GROUP1_MASK|0x01)
#define 	PA34		(PA_GROUP1_MASK|0x02)
#define 	PA35		(PA_GROUP1_MASK|0x03)
#define 	PA36		(PA_GROUP1_MASK|0x04)
#define 	PA37		(PA_GROUP1_MASK|0x05)
#define 	PA38		(PA_GROUP1_MASK|0x06)
#define 	PA39		(PA_GROUP1_MASK|0x07)
#define 	PA40		(PA_GROUP1_MASK|0x08)
#define 	PA41		(PA_GROUP1_MASK|0x09)
#define 	PA42		(PA_GROUP1_MASK|0x0A)
#define 	PA43		(PA_GROUP1_MASK|0x0B)
#define 	PA44		(PA_GROUP1_MASK|0x0C)
#define 	PA45		(PA_GROUP1_MASK|0x0D)
#define 	PA46		(PA_GROUP1_MASK|0x0E)
#define 	PA47		(PA_GROUP1_MASK|0x0F)

/** GPIO GROUP1_A组中每个IO的MASK */
#define 	PA0_MASK	(1UL<<0)
#define 	PA1_MASK	(1UL<<1)
#define 	PA2_MASK	(1UL<<2)
#define 	PA3_MASK	(1UL<<3)
#define 	PA4_MASK	(1UL<<4)
#define 	PA5_MASK	(1UL<<5)
#define 	PA6_MASK	(1UL<<6)
#define 	PA7_MASK	(1UL<<7)
#define 	PA8_MASK	(1UL<<8)
#define 	PA9_MASK	(1UL<<9)
#define 	PA10_MASK	(1UL<<10)
#define 	PA11_MASK	(1UL<<11)
#define 	PA12_MASK	(1UL<<12)
#define 	PA13_MASK	(1UL<<13)
#define 	PA14_MASK	(1UL<<14)
#define 	PA15_MASK	(1UL<<15)
#define 	PA16_MASK	(1UL<<16)
#define 	PA17_MASK	(1UL<<17)
#define 	PA18_MASK	(1UL<<18)
#define 	PA19_MASK	(1UL<<19)
#define 	PA20_MASK	(1UL<<20)
#define 	PA21_MASK	(1UL<<21)
#define 	PA22_MASK	(1UL<<22)
#define 	PA23_MASK	(1UL<<23)
#define 	PA24_MASK	(1UL<<24)
#define		PA25_MASK	(1UL<<25)
#define 	PA26_MASK	(1UL<<26)
#define 	PA27_MASK	(1UL<<27)
#define 	PA28_MASK	(1UL<<28)
#define 	PA29_MASK	(1UL<<29)
#define 	PA30_MASK	(1UL<<30)
#define 	PA31_MASK	(1UL<<31)

/** GPIO GROUP2_A组中每个IO的MASK */
#define 	PA32_MASK	(1UL<<0)
#define 	PA33_MASK	(1UL<<1)
#define 	PA34_MASK	(1UL<<2)
#define 	PA35_MASK	(1UL<<3)
#define 	PA36_MASK	(1UL<<4)
#define 	PA37_MASK	(1UL<<5)
#define 	PA38_MASK	(1UL<<6)
#define 	PA39_MASK	(1UL<<7)
#define 	PA40_MASK	(1UL<<8)
#define 	PA41_MASK	(1UL<<9)
#define 	PA42_MASK	(1UL<<10)
#define 	PA43_MASK	(1UL<<11)
#define 	PA44_MASK	(1UL<<12)
#define 	PA45_MASK	(1UL<<13)
#define 	PA46_MASK	(1UL<<14)
#define 	PA47_MASK	(1UL<<15)

/** GPIO GROUP_B组中每个IO的标识 */
#define 	PB0			(PB_GROUP_MASK|0x00)
#define 	PB1			(PB_GROUP_MASK|0x01)
#define 	PB2			(PB_GROUP_MASK|0x02)
#define 	PB3			(PB_GROUP_MASK|0x03)
#define 	PB4			(PB_GROUP_MASK|0x04)
#define 	PB5			(PB_GROUP_MASK|0x05)
#define 	PB6			(PB_GROUP_MASK|0x06)
#define 	PB7			(PB_GROUP_MASK|0x07)
#define 	PB8			(PB_GROUP_MASK|0x08)
#define 	PB9			(PB_GROUP_MASK|0x09)
#define 	PB10		(PB_GROUP_MASK|0x0A)
#define 	PB11		(PB_GROUP_MASK|0x0B)
#define 	PB12 		(PB_GROUP_MASK|0x0C)
#define 	PB13		(PB_GROUP_MASK|0x0D)
#define 	PB14		(PB_GROUP_MASK|0x0E)
#define 	PB15		(PB_GROUP_MASK|0x0F)

/** GPIO GROUP_B组中每个IO的MASK */
#define 	PB0_MASK	(1UL<<0)
#define 	PB1_MASK	(1UL<<1)
#define 	PB2_MASK	(1UL<<2)
#define 	PB3_MASK	(1UL<<3)
#define 	PB4_MASK	(1UL<<4)
#define 	PB5_MASK	(1UL<<5)
#define 	PB6_MASK	(1UL<<6)
#define 	PB7_MASK 	(1UL<<7)
#define 	PB8_MASK	(1UL<<8)
#define 	PB9_MASK	(1UL<<9)
#define 	PB10_MASK	(1UL<<10)
#define 	PB11_MASK	(1UL<<11)
#define 	PB12_MASK	(1UL<<12)
#define 	PB13_MASK	(1UL<<13)
#define 	PB14_MASK	(1UL<<14)
#define 	PB15_MASK	(1UL<<15)

/**
 * \brief 复用配置
 */
typedef enum
{
	RY_NO_REUSE		= 0,				/**< 无复用 */
	RY_JTAG_REUSE	= 1,				/**< 复用JTAG */
	RY_IOAM_REUSE	= 2,				/**< 复用IOAM */
	RY_TIMER_REUSE	= 3,				/**< 复用TIMER */
	RY_UART_REUSE	= 4,				/**< 复用UART */
	RY_SPI_REUSE	= 5,				/**< 复用SPI */
	RY_I2C_REUSE	= 6,				/**< 复用I2C */
	RY_I2S_REUSE	= 7,				/**< 复用I2S */
	RY_R8080_REUSE	= 8,				/**< 复用8080 */
	RY_RGB_REUSE	= 9,				/**< 复用VGA */
	RY_HAL_DRV0_REUSE	= 10,			/**< 复用HAL_DRIVER0 */
	RY_HAL_DRV1_REUSE	= 11,			/**< 复用HAL_DRIVER1 */
	RY_HAL_DRV2_REUSE	= 12,			/**< 复用HAL_DRIVER2 */
	RY_HAL_DRV3_REUSE	= 13,			/**< 复用HAL_DRIVER3 */
	RY_HAL_DRV4_REUSE	= 14,			/**< 复用HAL_DRIVER4 */
	RY_HAL_DRV5_REUSE	= 15,			/**< 复用HAL_DRIVER5 */
	RY_HAL_DRV6_REUSE	= 16,			/**< 复用HAL_DRIVER6 */
	RY_HAL_DRV7_REUSE	= 17,			/**< 复用HAL_DRIVER7 */
	RY_PWM_REUSE	= 18,				/**< 复用PWM */
	RY_DVP_REUSE	= 19,				/**< 复用DVP */
	RY_PWMIO_STOP_REUSE	= 20,			/**< 复用PWMIO_STOP */
	RY_SDIO_REUSE	= 21,				/**< 复用SDIO */
	RY_ARGB_REUSE	= 22,				/**< 复用ARGB */
	RY_ACMP_REUSE	= 23,				/**< 复用COMP */
	RY_OPA_REUSE	= 24,				/**< 复用OPA */
	RY_TSC_REUSE	= 25,				/**< 复用TSC */
	RY_ADC_REUSE	= 26,				/**< 复用ADC */
	RY_MAX_REUSE	= 31,				/**< 复用MAX值 */
} gpio_reuse_t;

/**
 * \brief 驱动选择
 */
typedef enum
{
    RY_GPIO_DRIVE0		= 0,          	/**< DRIVE0 */
    RY_GPIO_DRIVE1		= 1,          	/**< DRIVE1 */
    RY_GPIO_DRIVE2		= 2,          	/**< DRIVE2 */
    RY_GPIO_DRIVE3		= 3,          	/**< DRIVE3 */
    RY_GPIO_DRIVE4		= 4,          	/**< DRIVE4 */
    RY_GPIO_DRIVE5		= 5,          	/**< DRIVE5 */
    RY_GPIO_DRIVE6		= 6,          	/**< DRIVE6 */
    RY_GPIO_DRIVE7		= 7,          	/**< DRIVE7 */
} gpio_drive_t;

/**
 * \brief 选择引脚上下拉使能或禁能
 */
typedef enum
{
    RY_GPIO_NOPULL		= 0,			/**< 禁能上拉电阻 */
    RY_GPIO_PULLUP_5K	= 1,			/**< 使能5K上拉电阻 */
    RY_GPIO_PULLUP_10K	= 2,			/**< 使能10K上拉电阻 */
    RY_GPIO_PULLUP_30K	= 3,			/**< 使能30K上拉电阻 */
    RY_GPIO_PULLDN_5K	= 5,			/**< 使能5K下拉电阻 */
    RY_GPIO_PULLDN_10K	= 6,			/**< 使能10K下拉电阻 */
    RY_GPIO_PULLDN_30K	= 7,			/**< 使能30K下拉电阻 */
} gpio_pull_t;

/**
 * \brief 摆率配置
 */
typedef enum
{
	RY_GPIO_SWING_RATE_HIGH	=  0,		/**< 高摆率 */
	RY_GPIO_SWING_RATE_LOW	=  1,		/**< 低摆率 */
} gpio_sr_t;

/**
 * \brief 迟滞配置
 */
typedef enum
{
	RY_GPIO_INDELAY_DISEN	=  0,		/**< 无迟滞 */
	RY_GPIO_INDELAY_SMALL	=  2,		/**< 小迟滞 */
	RY_GPIO_INDELAY_BIG		=  3,		/**< 大迟滞 */
} gpio_indelay_t;

/**
 * \brief 模拟通道使能配置
 */
typedef enum
{
	RY_GPIO_ANE_DISEN		=  0,		/**< 不使能 */
	RY_GPIO_ANE_EN			=  1,		/**< 使能 */
} gpio_ane_t;

/**
 * \brief 引脚方向
 */
typedef enum
{
	RY_GPIO_HZ				= 0,		/**< 高阻 */
	RY_GPIO_OPEN_DRAIN		= 1,		/**< 开漏 */
	RY_GPIO_OPEN_SOURCE		= 2,		/**< 开源 */
	RY_GPIO_OUT_EN			= 3,		/**< 输出 */
} gpio_dir_t;

/**
 * \brief 输入使能配置
 */
typedef enum
{
	RY_GPIO_INPUT_DISEN		=  0,		/**< 输入不使能 */
	RY_GPIO_INPUT_EN		=  1,		/**< 输入使能 */
} gpio_input_t;

#define GPIO_CFG_REUSE_POS 			(16UL) 			/** CFG寄存器复用配置偏移位 */
#define GPIO_CFG_DRIVE_POS 			(12UL) 			/** CFG寄存器DRIVE偏移位 */
#define GPIO_CFG_PULL_POS 			(8UL)			/** CFG寄存器PULL偏移位 */
#define GPIO_CFG_SR_POS 			(6UL)			/** CFG寄存器摆率SR偏移位 */
#define GPIO_CFG_INDELAY_POS 		(4UL)			/** CFG寄存器迟滞偏移位 */
#define GPIO_CFG_ANE_POS 			(3UL)			/** CFG寄存器ANE偏移位 */
#define GPIO_CFG_DIR_POS 			(1UL)			/** CFG寄存器DIR偏移位 */
#define GPIO_CFG_IN_EN_POS 			(0UL)			/** CFG寄存器输入使能偏移位 */

typedef enum
{
	RY_GPIO_NOSENSE			= 0,		/** 无敏感 */
	RY_GPIO_SENSE_HIGH		= 2,		/** 高电平敏感 */
	RY_GPIO_SENSE_LOW		= 3,		/** 低电平敏感 */
	RY_GPIO_SENSE_FALLING	= 5,		/** 下降沿敏感 */
	RY_GPIO_SENSE_RISING	= 6,		/** 上升沿敏感 */
	RY_GPIO_SENSE_DUAL		= 7,		/** 双边沿敏感 */
} gpio_sense_t;

#define RY_GPIO_PA_CFG_BASE1		(GPIO_BASE+0x100)						/** GROUP1_PA组配置寄存器基地址 */
#define RY_GPIO_PA_CFG_BASE2        (RY_GPIO_PA_CFG_BASE1+32*4)				/** GROUP2_PA组配置寄存器基地址 */
#define RY_GPIO_PB_CFG_BASE         (RY_GPIO_PA_CFG_BASE2+16*4)				/** GROUP_PB组配置寄存器基地址 */

#define RY_GPIO_PA_SENSE_CFG_BASE1	(GPIO_BASE+0x44)						/** GROUP1_PA组配置寄存器基地址 */
#define RY_GPIO_PA_SENSE_CFG_BASE2	(RY_GPIO_PA_SENSE_CFG_BASE1+4*4)		/** GROUP2_PA组配置寄存器基地址 */
#define RY_GPIO_PB_SENSE_CFG_BASE	(RY_GPIO_PA_SENSE_CFG_BASE2+2*4)		/** GROUP_PB组配置寄存器基地址 */

extern __IO uint32_t GTRY_GPIO_CFG[3];				/** GPIO GROUP CONFIG寄存器列表 */
extern __IO uint32_t GTRY_GPIO_SENSE_CFG[3];			/** GPIO GROUP SENSE_CONFIG寄存器列表 */

/**
 * \brief	配置GPIO属性
 * \param[in] pin_number：需要配置的引脚，IO标识
 * \param[in] reuse_config：复用设置\ref gpio_reuse_t
 * \param[in] driver_config：驱动设置 \ref gpio_drive_t
 * \param[in] pull_config：上下拉设置 \ref gpio_pull_t
 * \param[in] dir_config：输出设置 \ref gpio_dir_t
 * \param[in] input_config：输入使能设置\ref gpio_input_t
 * \return 无
 * \note 当只需要配置一个引脚时，可以使用，该函数配置的引脚禁能了信号敏感，输入引脚连接到buffer，所以GPIO-IN此时可读
 */
static inline void gpio_cfg(uint32_t pin_number, gpio_reuse_t reuse_config, gpio_drive_t driver_config,
							gpio_pull_t pull_config, gpio_dir_t dir_config, gpio_input_t input_config)
{
	*(__IO uint32_t *)(GTRY_GPIO_CFG[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS]+ (pin_number&GPIO_NUM_MASK)*4) =
							   	   (reuse_config << GPIO_CFG_REUSE_POS) | (driver_config << GPIO_CFG_DRIVE_POS)
							   	   | (pull_config << GPIO_CFG_PULL_POS) | (RY_GPIO_SWING_RATE_HIGH << GPIO_CFG_SR_POS)
								   | (RY_GPIO_INDELAY_SMALL << GPIO_CFG_INDELAY_POS) | (RY_GPIO_ANE_DISEN << GPIO_CFG_ANE_POS)
								   | (dir_config<< GPIO_CFG_DIR_POS) | (input_config<< GPIO_CFG_IN_EN_POS);
}
/**
 * \brief	将给定的引脚配置成输入
 * \param[in] pin_number：需要配置的引脚，IO标识
 * \param[in] pull_config：上下拉设置 \ref gpio_pull_t
 * \return 无
 * \note 当只需要配置一个引脚时，可以使用，该函数配置的引脚禁能了信号敏感，输入引脚连接到buffer，所以GPIO-IN此时可读
 */
static inline void gpio_cfg_input(uint32_t pin_number, gpio_pull_t pull_config)
{
	*(__IO uint32_t *)(GTRY_GPIO_CFG[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS]+ (pin_number&GPIO_NUM_MASK)*4) =
									(RY_NO_REUSE << GPIO_CFG_REUSE_POS) | (RY_GPIO_DRIVE4 << GPIO_CFG_DRIVE_POS)
									| (pull_config << GPIO_CFG_PULL_POS) | (RY_GPIO_SWING_RATE_HIGH << GPIO_CFG_SR_POS)
									| (RY_GPIO_INDELAY_SMALL << GPIO_CFG_INDELAY_POS) | (RY_GPIO_ANE_DISEN << GPIO_CFG_ANE_POS)
									| (RY_GPIO_HZ << GPIO_CFG_DIR_POS) | (RY_GPIO_INPUT_EN << GPIO_CFG_IN_EN_POS);
}
/**
 * \brief	将给定的引脚配置成输出
 * \param[in] pin_number：需要配置的引脚，IO标识
 * \param[in] driver_config：驱动设置 \ref gpio_drive_t
 * \return 无
 * \note 当只需要配置一个引脚时，可以使用，该函数配置的引脚禁能了信号敏感
 */
static inline void gpio_cfg_output(uint32_t pin_number, gpio_drive_t driver_config)
{
	*(__IO uint32_t *)(GTRY_GPIO_CFG[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS]+ (pin_number&GPIO_NUM_MASK)*4) =
									(RY_NO_REUSE << GPIO_CFG_REUSE_POS) | (driver_config << GPIO_CFG_DRIVE_POS)
									| (RY_GPIO_NOPULL << GPIO_CFG_PULL_POS) | (RY_GPIO_SWING_RATE_HIGH << GPIO_CFG_SR_POS)
									| (RY_GPIO_INDELAY_SMALL << GPIO_CFG_INDELAY_POS) | (RY_GPIO_ANE_DISEN << GPIO_CFG_ANE_POS)
									| (RY_GPIO_OUT_EN << GPIO_CFG_DIR_POS) | (RY_GPIO_INPUT_DISEN << GPIO_CFG_IN_EN_POS);
}
/**
 * \brief	给定引脚的电平置位
 * \param[in] pin_number：需要置位的引脚，IO标识
 * \return 无
 * \note 需要将该引脚设置成输出
 */
static inline void gpio_out_set(uint32_t pin_number)
{
	GTRY_GPIO->GPIO_DOUTSET[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS] = 0x1<<(pin_number&GPIO_NUM_MASK);
}
/**
 * \brief	给定多路引脚的电平置位
 * \param[in] pin_group：需要配置的引脚所在GPIO GROUP标识
 * \param[in] pin_mask：需要配置的引脚，IO标识MASK
 * \return 无
 * \note 需要将该引脚设置成输出
 */
static inline void gpio_out_multi_set(uint32_t pin_group, uint32_t pin_mask)
{
	GTRY_GPIO->GPIO_DOUTSET[pin_group] = pin_mask;
}
/**
 * \brief	给定引脚的电平清零
 * \param[in] pin_number：需要清零的引脚，IO标识
 * \return 无
 * \note 需要将该引脚设置成输出
 */
static inline void gpio_out_clear(uint32_t pin_number)
{
	GTRY_GPIO->GPIO_DOUTCLEAR[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS] = 0x1<<(pin_number&GPIO_NUM_MASK);
}

/**
 * \brief	给定多路引脚的电平清零
 * \param[in] pin_group：需要配置的引脚所在GPIO GROUP标识
 * \param[in] pin_mask：需要配置的引脚，IO标识MASK
 * \return 无
 * \note 需要将该引脚设置成输出
 */
static inline void gpio_out_multi_clear(uint32_t pin_group, uint32_t pin_mask)
{
	GTRY_GPIO->GPIO_DOUTCLEAR[pin_group] = pin_mask;
}

/**
 * \brief	给定引脚的电平翻转
 * \param[in] pin_number：需要清零的引脚，IO标识
 * \return 无
 * \note 需要将该引脚设置成输出才有效
 */
static inline void gpio_out_toggle(uint32_t pin_number)
{
	GTRY_GPIO->GPIO_DATAOUT[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS] ^= 0x1<<(pin_number&GPIO_NUM_MASK);
}

/**
 * \brief	给定多路引脚的电平翻转
 * \param[in] pin_group：需要配置的引脚所在GPIO GROUP标识
 * \param[in] pin_mask：需要配置的引脚，IO标识MASK
 * \return 无
 * \note 需要将该引脚设置成输出才有效
 */
static inline void gpio_out_multi_toggle(uint32_t pin_group, uint32_t pin_mask)
{
	GTRY_GPIO->GPIO_DATAOUT[pin_group] ^= pin_mask;
}

/**
 * \brief	读取给定引脚的电平
 * \param[in] pin_number：需要读取的引脚
 * \return 0表示引脚为低电平；1表示引脚为高电平
 */
static inline uint32_t gpio_in_read(uint32_t pin_number)
{
    uint32_t result = 0;
    result = (GTRY_GPIO->GPIO_DATAIN[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS]>>(pin_number&GPIO_NUM_MASK)) & 1UL;
    return  result;
}

/**
 * \brief	读取给定多路引脚的电平
 * \param[in] pin_group：需要配置的引脚所在GPIO GROUP标识
 * \return 多路对应位的电平
 * \retval 对应位0表示引脚为低电平
 * \retval 对应位1表示引脚为高电平
 */
static inline uint32_t gpio_in_multi_read(uint32_t pin_group)
{
	return GTRY_GPIO->GPIO_DATAIN[pin_group];
}

/**
 * \brief	将给定的引脚配置成输入敏感
 * \param[in] pin_number：需要配置的引脚，IO标识
 * \param[in] pull_config：上下拉设置参考 \ref gpio_pull_t
 * \param[in] sense_config：设置敏感信号 \ref gpio_sense_t
 * \return 无
 * \note 设置成边沿敏感时需要高频时钟开启
 */
static inline void gpio_cfg_sense_input(uint32_t pin_number, gpio_pull_t pull_config, gpio_sense_t sense_config)
{
	*(__IO uint32_t *)(GTRY_GPIO_CFG[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS]+ (pin_number&GPIO_NUM_MASK)*4) =
									(RY_NO_REUSE << GPIO_CFG_REUSE_POS) | (RY_GPIO_DRIVE4 << GPIO_CFG_DRIVE_POS)
									| (pull_config << GPIO_CFG_PULL_POS) | (RY_GPIO_SWING_RATE_HIGH << GPIO_CFG_SR_POS)
									| (RY_GPIO_INDELAY_SMALL << GPIO_CFG_INDELAY_POS) | (RY_GPIO_ANE_DISEN << GPIO_CFG_ANE_POS)
									| (RY_GPIO_HZ << GPIO_CFG_DIR_POS) | (RY_GPIO_INPUT_EN << GPIO_CFG_IN_EN_POS);
	*(__IO uint32_t *)(GTRY_GPIO_SENSE_CFG[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS]+ (((pin_number&GPIO_NUM_MASK)/8)*4)) &=
									~(0xfUL<<(((pin_number&GPIO_NUM_MASK)%8)*4));
	*(__IO uint32_t *)(GTRY_GPIO_SENSE_CFG[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS]+ (((pin_number&GPIO_NUM_MASK)/8)*4)) |=
									(sense_config<<(((pin_number&GPIO_NUM_MASK)%8)*4));
}

/**
 * \brief	GPIO中断状态清除
 * \param[in] pin_number：需要配置的引脚，IO标识
 * \return 无
 */
static inline void gpio_int_sts_clear(uint32_t pin_number)
{
	GTRY_GPIO->GPIO_INT_STS[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS] |= 0x1<<(pin_number&GPIO_NUM_MASK);
}

/**
 * \brief	GPIO多路中断状态同时清除
 * \param[in] pin_group：需要配置的引脚所在GPIO GROUP标识
 * \param[in] pin_mask：需要配置的引脚，IO标识MASK
 * \return 无
 */
static inline void gpio_int_sts_multi_clear(uint32_t pin_group, uint32_t pin_mask)
{
	GTRY_GPIO->GPIO_INT_STS[pin_group] = pin_mask;
}

/**
 * \brief	打开GPIO中断
 * \param[in] pin_number : 需要配置的引脚，IO标识
 * \return 无
 */
static inline void gpio_cfg_en_int(uint32_t pin_number)
{
	GTRY_GPIO->GPIO_INT_EN[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS] |= 0x1<<(pin_number&GPIO_NUM_MASK);
}

/**
 * \brief	关闭GPIO中断
 * \param[in] pin_number：需要配置的引脚，IO标识
 * \return 无
 */
static inline void gpio_cfg_dis_int(uint32_t pin_number)
{
	GTRY_GPIO->GPIO_INT_EN[(pin_number & GPIO_GROUP_MASK)>>GPIO_GROUP_POS] &= ~(0x1<<(pin_number&GPIO_NUM_MASK));
}

/**
 * \brief	GPIO中断回调函数指针
 * \param[in] gpio_int_group：需要配置的引脚所在GPIO GROUP标识
 * \param[in] gpio_src：发生中断的引脚，IO标识
 * \return 无
 */
typedef void (*p_gpio_int_cb)(uint32_t gpio_group, uint32_t gpio_int_mask);

/**
 * \brief	GPIO中断初始化
 * \param[in] int_group_en_mask：需要配置的引脚所在GPIO GROUP标识MASK
 * \param[in] gpio_int_cb：GPIO中断回调函数入口
 * \return 无
 */
extern void gpio_int_init(p_gpio_int_cb gpio_int_cb);

//JTAG
extern void jtag_io_multi_config(void);
extern void jtag_io_multi_disconfig(void);

#endif	/* _RY_GPIO_DRV_H */
