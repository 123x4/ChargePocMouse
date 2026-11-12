/*
 * Copyright (c) 2019
 * All rights reserved.
 *
 */
#ifndef __GTRY_SMU_H__
#define __GTRY_SMU_H__

#include "gtry_type.h"
#include "gtry_core_v5.h"
#include "gtry_regmap.h"

/*****************************************************************************
 * System clock
 ****************************************************************************/
#define KHz                     1000
#define MHz                     1000000

#define ILOFREQ                 (32768)
#define XTAL32KFREQ             (32768)
#define XTALFREQ                (16 * MHz)
#define IMOFREQ                 (48 * MHz)
#define PLLFREQ                 (200 * MHz)

extern volatile unsigned int g_sysclk_freq;
extern volatile unsigned int g_hclk_freq;
extern volatile unsigned int g_pclk_freq;

typedef enum SMU_SYSCLK_SEL
{
	SMU_SYSCLK_200MHZ = 0,
	SMU_SYSCLK_96MHZ,
	SMU_SYSCLK_48MHZ,
	SMU_SYSCLK_SELMAX
}smu_sysclk_sel_t;

typedef enum SMU_CLK_DIV
{
	SMU_HCLK_PCLK_DIV1_1 = 0,
	SMU_HCLK_PCLK_DIV1_2,
	SMU_HCLK_PCLK_DIV1_4,
	SMU_HCLK_PCLK_DIVMAX
}smu_clk_div_t;
#define HCLK_PCLK_DIV_SET(div)		((div)<<2)
#define HCLK_PCLK_DIV_MASK			(0x3ul<<2)

typedef enum SMU_32K_SEL
{
	SMU_32K_ILO = 0,
	SMU_32K_XTAL
}smu_32k_sel_t;

typedef enum SMU_XTAL32K_REUSE
{
	SMU_XTAL32K_REUSE_NULL = 0,
	SMU_XTAL32K_PA30_31 = 0,
	SMU_XTAL32K_PA11_12
}smu_xtal32k_sel_t;

#define SMU_PCLK_UART0_EN_MASK	(0x1ul<<0)
#define SMU_PCLK_UART1_EN_MASK	(0x1ul<<1)
#define SMU_PCLK_UART2_EN_MASK	(0x1ul<<2)
#define SMU_PCLK_SPI0_EN_MASK	(0x1ul<<4)
#define SMU_PCLK_SPI1_EN_MASK	(0x1ul<<5)
#define SMU_PCLK_SPI2_EN_MASK	(0x1ul<<6)
#define SMU_PCLK_SPI3_EN_MASK	(0x1ul<<7)
#define SMU_PCLK_SPI4_EN_MASK	(0x1ul<<8)
#define SMU_PCLK_SPI5_EN_MASK	(0x1ul<<9)
#define SMU_PCLK_SPI6_EN_MASK	(0x1ul<<10)
#define SMU_PCLK_PWM_EN_MASK	(0x1ul<<11)
#define SMU_PCLK_TIMER_EN_MASK	(0x1ul<<12)
#define SMU_PCLK_GPIO_EN_MASK	(0x1ul<<13)
#define SMU_PCLK_I2C0_EN_MASK	(0x1ul<<14)
#define SMU_PCLK_I2C1_EN_MASK	(0x1ul<<15)
#define SMU_PCLK_WDT_EN_MASK	(0x1ul<<16)
#define SMU_PCLK_USB_EN_MASK	(0x1ul<<18)
#define SMU_PCLK_RGB_EN_MASK	(0x1ul<<19)
#define SMU_PCLK_DVP_EN_MASK	(0x1ul<<20)
#define SMU_PCLK_R8080_EN_MASK	(0x1ul<<22)
#define SMU_PCLK_ALL_EN_MASK	(0x5DFFF7ul)

typedef enum SMU_LOWPOWER_MODE
{
	SMU_LOWPOWER_SLEEP = 0,
	SMU_LOWPOWER_DEEP_SLEEP,
	SMU_LOWPOWER_SHUT_DOWN,
	SMU_LOWPOWER_MAX
}smu_lowpower_mode_t;

#define SMU_PCSWKUP_SRC_ENL_MASK		((0x1ul<<IRQ_RTC_PERIOD_SOURCE)|(0x1ul<<IRQ_RTC_ALARM_SOURCE)|(0x1ul<<IRQ_HASP_SOURCE)|(0x1ul<<IRQ_SPI0_SOURCE)| \
										(0x1ul<<IRQ_I2C0_SOURCE)|(0x1ul<<IRQ_GPIO_SOURCE)|(0x1ul<<IRQ_UART0_SOURCE)| \
										(0x1ul<<IRQ_UART1_SOURCE)|(0x1ul<<IRQ_DMA0_SOURCE)|(0x1ul<<IRQ_DMA1_SOURCE)| \
										(0x1ul<<IRQ_UART2_SOURCE)|(0x1ul<<IRQ_I2C1_SOURCE)|(0x1ul<<IRQ_USBDMA_SOURCE)| \
										(0x1ul<<IRQ_USB_SOURCE)|(0x1ul<<IRQ_SPI1_SOURCE)|(0x1ul<<IRQ_SPI2_SOURCE)|(0x1ul<<IRQ_SPI3_SOURCE))
#define SMU_PCSWKUP_SRC_ENH_MASK		((0x1ul<<(IRQ_SPI4_SOURCE-32))|(0x1ul<<(IRQ_SPI5_SOURCE-32))|(0x1ul<<(IRQ_SPI6_SOURCE-32))| \
										(0x1ul<<(IRQ_PWM_SOURCE-32))|(0x1ul<<(IRQ_TIMER_SOURCE-32))|(0x1ul<<(IRQ_TRNG_SOURCE-32))| \
										(0x1ul<<(IRQ_IOAM_SOURCE-32))|(0x1ul<<(IRQ_I2S_SOURCE-32))|(0x1ul<<(IRQ_R8080_SOURCE-32))| \
										(0x1ul<<(IRQ_ACMP0_SOURCE-32))|(0x1ul<<(IRQ_ACMP1_SOURCE-32))|(0x1ul<<(IRQ_RGB_SOURCE-32))| \
										(0x1ul<<(IRQ_ADC_SOURCE-32))|(0x1ul<<(IRQ_TSC_SOURCE-32))|(0x1ul<<(IRQ_DVP_SOURCE-32))| \
										(0x1ul<<(IRQ_AES_SOURCE-32))|(0x1ul<<(IRQ_LTIMER0_SOURCE-32))|(0x1ul<<(IRQ_LTIMER1_SOURCE-32))| \
										(0x1ul<<(IRQ_SDIO_SOURCE-32))|(0x1ul<<(IRQ_ARGB_SOURCE-32)))

#define SMU_DPSLPWKUP_SRC_ALL_MASK		(0x3Ful)
#define SMU_DPSLPWKUP_SRC_USB_MASK		(0x1ul<<5)
#define SMU_DPSLPWKUP_SRC_JTAG_MASK		(0x1ul<<4)
#define SMU_DPSLPWKUP_SRC_LTIMER1_MASK	(0x1ul<<3)
#define SMU_DPSLPWKUP_SRC_LTIMER0_MASK	(0x1ul<<2)
#define SMU_DPSLPWKUP_SRC_RTC_MASK		(0x1ul<<1)
#define SMU_DPSLPWKUP_SRC_GPIO_MASK		(0x1ul<<0)
#define SMU_SLPWKUP_SRC_ALL_MASK		(0x0ul)

static inline void ry_delay_us(unsigned int delay_us)
{
	unsigned long long cur_mtimer = GTRY_PLMT->MTIME+((unsigned long long)delay_us*(g_pclk_freq/1000000));
	while(cur_mtimer > GTRY_PLMT->MTIME);
}

static inline void ry_delay_ms(unsigned int delay_ms)
{
	unsigned long long cur_mtimer = GTRY_PLMT->MTIME+((unsigned long long)delay_ms*(g_pclk_freq/1000));
	while(cur_mtimer > GTRY_PLMT->MTIME);
}

static inline void smu_periph_enable(unsigned int periph_mask)
{
	GTRY_SMU->SMU_PCLK_EN |= periph_mask;
}

static inline void smu_periph_diable(unsigned int periph_mask)
{
	GTRY_SMU->SMU_PCLK_EN &= ~periph_mask;
}

static inline void smu_usbphy_powerdown(void)
{
	SET_REG32(USB_BASE+0x3FC, GET_REG32(USB_BASE+0x3FC)|0x4);
	GTRY_USB->USB_POWER |= 0x03;
}

static inline unsigned long long ry_get_mtime_us(void)
{
	return GTRY_PLMT->MTIME/(g_pclk_freq/1000000);
}

static inline unsigned long long ry_get_mtime_ms(void)
{
	return GTRY_PLMT->MTIME/(g_pclk_freq/1000);
}

static inline unsigned int ry_get_sys_ustick(void)
{
	return (g_hclk_freq/1000000);
}

static inline void ry_reset_io_disable(void)
{
	GTRY_SMU->SMU_PWR_CFG0 &= ~(0xFul<<12);
	GTRY_SMU->SMU_PWR_CFG0 |= (0xAul<<12);
}

static inline void ry_reset_io_enable(void)
{
	GTRY_SMU->SMU_PWR_CFG0 &= ~(0xFul<<12);
}

typedef enum{
	POWER_VDDIOB_INTERNAL_OFF	=0x0ul,		//inner VDDIOB LDO is turned off, and at this time,
											//power is supplied through the VDDIOB pin from an external power source.
											//The external power supply cannot exceed VDD33.
	POWER_VDDIOB_INTERNAL_LDO	=0x1ul,		//inner VDDIOB LDO is open. At this time, power is supplied from the internal VDDIOB LDO.
											//The specific output level is configured by the LDO.
	POWER_VDDIOB_INTERNAL_VDD	=0x2ul,		//inner VDDIOB is bypassed to the chip's power supply pin VDD33, which now powers it.
} smu_pwr_vddiob_t;

typedef enum{
	POWER_VDDIOB_LDO_DEF		=0x0ul,		//inner VDDIOB LDO default
	POWER_VDDIOB_LDO_V1P6		=0x0ul,		//inner VDDIOB LDO 1.6V
	POWER_VDDIOB_LDO_V1P7		=0x1ul,		//inner VDDIOB LDO 1.7V
	POWER_VDDIOB_LDO_V1P8		=0x2ul,		//inner VDDIOB LDO 1.8V
	POWER_VDDIOB_LDO_V1P9		=0x3ul,		//inner VDDIOB LDO 1.9V
	POWER_VDDIOB_LDO_V2P0		=0x4ul,		//inner VDDIOB LDO 2.0V
	POWER_VDDIOB_LDO_V2P1		=0x5ul,		//inner VDDIOB LDO 2.1V
	POWER_VDDIOB_LDO_V2P2		=0x6ul,		//inner VDDIOB LDO 2.2V
	POWER_VDDIOB_LDO_V2P3		=0x7ul,		//inner VDDIOB LDO 2.3V
	POWER_VDDIOB_LDO_V2P4		=0x8ul,		//inner VDDIOB LDO 2.4V
	POWER_VDDIOB_LDO_V2P5		=0x9ul,		//inner VDDIOB LDO 2.5V
	POWER_VDDIOB_LDO_V2P6		=0xAul,		//inner VDDIOB LDO 2.6V
	POWER_VDDIOB_LDO_V2P7		=0xBul,		//inner VDDIOB LDO 2.7V
	POWER_VDDIOB_LDO_V2P8		=0xCul,		//inner VDDIOB LDO 2.8V
	POWER_VDDIOB_LDO_V2P9		=0xDul,		//inner VDDIOB LDO 2.9V
	POWER_VDDIOB_LDO_V3P0		=0xEul,		//inner VDDIOB LDO 3.0V
	POWER_VDDIOB_LDO_V3P1		=0xFul,		//inner VDDIOB LDO 3.1V
} smu_pwr_vddiob_lvl;

static inline void smu_power_vddiob_config(smu_pwr_vddiob_t mode, smu_pwr_vddiob_lvl level)
{
	GTRY_SMU->SMU_PWR_CFG0 = (GTRY_SMU->SMU_PWR_CFG0 & (~0xF30000ul)) | (level<<20) | (mode<<16);
}

static inline void smu_power_vdd12_set(void)
{
	GTRY_SMU->SMU_PWR_CFG0 = (GTRY_SMU->SMU_PWR_CFG0&0xFFFFFF8F)|0x30;
}

static inline void smu_power_vdd13_set(void)
{
	GTRY_SMU->SMU_PWR_CFG0 = (GTRY_SMU->SMU_PWR_CFG0&0xFFFFFF8F)|0x40;
}

typedef enum{
	TSNS_CFG_STEP_3MV		=0x0ul,		//TSNS_CONFIG 3mV/C, 0.9V@25C
	TSNS_CFG_STEP_4MV		=0x1ul,		//TSNS_CONFIG 4mV/C, 1.2V@25C
} smu_tsns_cfg_t;

static inline void smu_tsns_init(smu_tsns_cfg_t config)
{
	GTRY_SMU->SMU_PWR_CFG1 = (GTRY_SMU->SMU_PWR_CFG1 & (~0x600000ul)) | (0x1ul<<22) | (config<<21);
}

static inline void smu_tsns_uninit(void)
{
	GTRY_SMU->SMU_PWR_CFG1 = (GTRY_SMU->SMU_PWR_CFG1 & (~0x400000ul));
}

/*
 * Exported functions
 */
extern void smu_periph_enable(unsigned int periph_mask);
extern void smu_periph_disable(unsigned int periph_mask);
extern void smu_sysclk_init(smu_sysclk_sel_t sysclk);
extern void smu_hclk_48Mhz_to_8Mhz(void);
extern void smu_hclk_96Mhz_to_8Mhz(void);
extern void smu_hclk_resume(void);
extern void smu_32kclk_init(smu_32k_sel_t source, smu_xtal32k_sel_t xtal32k_sel);
extern void smu_vddiob_dischg(void);

#define		SMU_LOWPOWER_SET_BF_SLEEP()			do{SET_REG32(0xF01001B0, GET_REG32(0xF01001C0)); \
													SET_REG32(0xF01001B4, (GET_REG32(0xF01001C4)&0xFFFF8FFF)|0x4000); \
													SET_REG32(0xF01001B8, GET_REG32(0xF01001C8));}while(0)
#define		SMU_LOWPOWER_SET_EN()				SET_REG32(0xF01001BC, 0x200)
#define		SMU_LOWPOWER_SET_DIS()				SET_REG32(0xF01001BC, 0x00)

#define		SMU_LOWPOWER_FLASH_PWRDN()			do{SET_REG32(0xF0B0002C, 0xB9); \
													SET_REG32(0xF0B00020, 0x1000000); \
													SET_REG32(0xF0B00024, 0x0);}while(0)
#define		SMU_LOWPOWER_FLASH_WKUP()			do{SET_REG32(0xF0B0002C, 0xAB); \
													SET_REG32(0xF0B00020, 0x1000000); \
													SET_REG32(0xF0B00024, 0x0);}while(0)

typedef void (*p_wkup_int_cb)(unsigned int wkup_irq_src);  	//< Pointer to \ref p_wkup_int_cb : Signal wkup Event.

/**
 * @brief 	system wkup init
 * @param[in] wkup_int_cb: the interrupted callback function. see 'p_wkup_int_cb'
 * @retval none
 */
extern void smu_enter_lowpower_init(p_wkup_int_cb wkup_int_cb);
extern void smu_enter_lowpower_mode(smu_lowpower_mode_t power_mode, unsigned int wkup_src_mask);
extern void smu_enter_dpsleep_init(p_wkup_int_cb wkup_int_cb);
extern void smu_enter_dpsleep_mode(unsigned int wkup_src_mask);
static inline void smu_enter_dpsleep_quickly(void)
{
	SMU_LOWPOWER_SET_EN();
	GTRY_SMU->SMU_PWR_CFG0 &= 0xFFFFFFFC;//disable EN_LVDET and EN_LVRST
	GTRY_SMU->SMU_PCS_SCRATCH = 0x0;
	GTRY_SMU->SMU_PCS_CTRL = 0x3;

	__asm__ __volatile__ ("wfi");
	GTRY_SMU->SMU_PWR_CFG0 |= 0x3;//enable EN_LVDET and EN_LVRST
	SMU_LOWPOWER_SET_DIS();
}

#define SMU_PCS_WKUP_INT_MASK		(0x1ul<<30)
extern IRQn_Type smu_get_wkup_irq_src(void);
extern void smu_clear_wkup_irq_src(void);
extern void smu_periph_plic_reset(void);

static inline void smu_soft_reset(void)
{
	GTRY_SMU->SMU_PCS_SRST = 0x1;
}

#define SMU_PCS_NOR_POW_RESET		0x3
#define SMU_PCS_SOFT_RESET			0x4
#define SMU_PCS_WDT_RESET			0x5
#define SMU_PCS_HARDWARE_RESET		0x6
#define SMU_PCS_HOST_POW_RESET		0x7
#define SMU_PCS_RESET_MASK			0xF
extern unsigned int smu_get_reset_src(void);

static inline void smu_global_int_enable(void)
{
	// Enable the Machine-External bit in MIE
	set_csr(NDS_MIE, MIP_MEIP);
	// Enable GIE--M-mode interrupt
	set_csr(NDS_MSTATUS, MSTATUS_MIE);
}

static inline void smu_global_int_disable(void)
{
	// Disable the Machine-External bit in MIE
	clear_csr(NDS_MIE, MIP_MEIP);
	// Disable GIE--M-mode interrupt
	clear_csr(NDS_MSTATUS, MSTATUS_MIE);
}

static inline void smu_enter_upgrade(void)
{
	GTRY_SMU->SMU_PCS_SCRATCH = 0x5ac396c5;
	GTRY_SMU->SMU_PCS_SRST = 0x1;
}

static inline unsigned int __nds__plic_get_interrupt (unsigned int source)
{
  unsigned int hart_id = __nds__mfsr(NDS_MHARTID);
  volatile unsigned int *current_ptr = (volatile unsigned int *)(NDS_PLIC_BASE +
                                                                 PLIC_ENABLE_OFFSET +
                                                                 (PLIC_TARGET_INCR(hart_id) << PLIC_ENABLE_SHIFT_PER_TARGET) +
                                                                 ((source >> 5) << 2));
  unsigned int current = *current_ptr;
  return current & (1 << (source & 0x1F));
}

#endif	// __GTRY_SMU_H__
