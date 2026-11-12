/*
 * Copyright (c) 2019
 * All rights reserved.
 *
 */

#include "gtry_platform.h"
#include "gtry_smu.h"
#include "app_config.h"


volatile unsigned int g_sysclk_freq = IMOFREQ;
volatile unsigned int g_hclk_freq = IMOFREQ;
volatile unsigned int g_pclk_freq = IMOFREQ;
static unsigned int sysclk_src_cfg = 0x0;

void smu_sysclk_init(smu_sysclk_sel_t sysclk)
{
	GTRY_SMU->SMU_CLK_CFG &= ~HCLK_PCLK_DIV_MASK;
    GTRY_SMU->SMU_IMO_CFG1 |= 0x10;
	if(sysclk == SMU_SYSCLK_48MHZ || sysclk >= SMU_SYSCLK_SELMAX)
	{
		smu_power_vdd12_set();
	    GTRY_SMU->SMU_IMO_CFG0 = (GTRY_SMU->SMU_IMO_CFG0&0xFFFF0000)|((GET_REG32(0xF01001C0)>>16)&0xffff);
		GTRY_SMU->SMU_CLK_CFG &= 0xFFFFF8FF;//select IMO
		g_sysclk_freq = IMOFREQ;
		g_hclk_freq = IMOFREQ;
		g_pclk_freq = IMOFREQ;
		GTRY_SMU->SMU_PLL_CFG1 |= 0x80000000;//disable pll
		GTRY_SMU->SMU_PWR_CFG1 &= 0xFFFFFFFB;//disable EN_PLLLDO
	}
	else if(sysclk == SMU_SYSCLK_96MHZ)
	{
		smu_power_vdd12_set();
	    GTRY_SMU->SMU_IMO_CFG0 = (GTRY_SMU->SMU_IMO_CFG0&0xFFFF0000)|(0x9800)|((GET_REG32(0xF01001C8)>>4)&0x7ff);
		GTRY_SMU->SMU_CLK_CFG &= 0xFFFFF8FF;//select IMO
		g_sysclk_freq = IMOFREQ<<1;
		g_hclk_freq = IMOFREQ<<1;
		g_pclk_freq = IMOFREQ<<1;
		GTRY_SMU->SMU_PLL_CFG1 |= 0x80000000;//disable pll
		GTRY_SMU->SMU_PWR_CFG1 &= 0xFFFFFFFB;//disable EN_PLLLDO
	}
	else if(sysclk == SMU_SYSCLK_200MHZ)
	{
		smu_power_vdd13_set();
	    GTRY_SMU->SMU_IMO_CFG0 = (GTRY_SMU->SMU_IMO_CFG0&0xFFFF0000)|((GET_REG32(0xF01001C0)>>16)&0xffff);
		GTRY_SMU->SMU_PWR_CFG1 |= 0x4;//EN_PLLLDO
		GTRY_SMU->SMU_PLL_CFG0 = 0x42000000;//PLL_REFDIV(B29:B24) = 0x2, PLL_FRAC(B23:B0) = 0x0//
		GTRY_SMU->SMU_PLL_CFG1 = 0x3020E032;//sysclk = (imoclk/2)*50/(3*2)	//power little than before
		while((GTRY_SMU->SMU_SYS_STS&0x8) == 0x0);
		GTRY_SMU->SMU_CLK_CFG |= HCLK_PCLK_DIV_SET(SMU_HCLK_PCLK_DIV1_2);//default pclk = 1/2*sysclk = 1/2*hclk
		GTRY_SMU->SMU_CLK_CFG |= 0x400;//select PLL
		g_sysclk_freq = PLLFREQ;
		g_hclk_freq = PLLFREQ;
		g_pclk_freq = PLLFREQ>>1;
	}
    GTRY_SMU->SMU_IMO_CFG1 &= (~0x10);
	sysclk_src_cfg = GTRY_SMU->SMU_CLK_CFG;
}

void smu_hclk_48Mhz_to_8Mhz(void)
{
	GTRY_SMU->SMU_CLK_CFG = (sysclk_src_cfg|(0x5ul<<22));//hclk/6=8
	g_hclk_freq /= 6;
	g_pclk_freq = g_hclk_freq;
	sysclk_src_cfg = GTRY_SMU->SMU_CLK_CFG;
}

void smu_hclk_96Mhz_to_8Mhz(void)
{
	GTRY_SMU->SMU_CLK_CFG = (sysclk_src_cfg|(0xBul<<22));//hclk/12=8
	g_hclk_freq /= 12;
	g_pclk_freq = g_hclk_freq;
	sysclk_src_cfg = GTRY_SMU->SMU_CLK_CFG;
}

void smu_hclk_resume(void)
{
	GTRY_SMU->SMU_CLK_CFG = (sysclk_src_cfg&(~(0x3FFul<<22)));
	if(((sysclk_src_cfg>>22)&0x3FFul) == 0x5ul)
	{
		g_hclk_freq = IMOFREQ;
		g_pclk_freq = IMOFREQ;
	}
	else
	{
		g_hclk_freq = IMOFREQ<<1;
		g_pclk_freq = IMOFREQ<<1;
	}
	sysclk_src_cfg = GTRY_SMU->SMU_CLK_CFG;
}

void smu_32kclk_init(smu_32k_sel_t source, smu_xtal32k_sel_t xtal32k_sel)
{
	if(source == SMU_32K_XTAL)
	{
		GTRY_SMU->SMU_PWR_CFG1 = (GTRY_SMU->SMU_PWR_CFG1 & ~(0x1ul<<29)) | (xtal32k_sel<<29);
		GTRY_SMU->SMU_XTAL_ILO_CFG |= (0x1ul<<16);
		GTRY_SMU->SMU_CLK_CFG |= (0x1ul<<11);
	}
	else
	{
		if(((GET_REG32(0xF01001C4)>>23)&0x3)==0x3)
		{
			GTRY_SMU->SMU_XTAL_ILO_CFG &= ~(0x7ful<<0);
			GTRY_SMU->SMU_XTAL_ILO_CFG |= ((GET_REG32(0xF01001C8)>>22)&0x7f);
		}
		GTRY_SMU->SMU_XTAL_ILO_CFG &= ~(0x1ul<<16);
		GTRY_SMU->SMU_CLK_CFG &= ~(0x1ul<<11);
		GTRY_SMU->SMU_XTAL_ILO_CFG |= (0x1ul<<9);
	}
}

void smu_vddiob_dischg(void)
{
	GTRY_GPIO->GPIO_CFG[63] = 0x3506;
	GTRY_GPIO->GPIO_DOUTSET[2] = 0x8000;
	ry_delay_ms(10);
	GTRY_GPIO->GPIO_CFG[63] = 0x0;
}

p_wkup_int_cb wkup_int_callback = NULL;	/** Entry point for the wkup interrupt callback function. */

/**
 * @brief 	system wkup init
 * @param[in] wkup_int_cb锛歵he interrupted callback function. see 鈥榩_wkup_int_cb鈥�
 * @retval 鏃�
 */
void smu_enter_lowpower_init(p_wkup_int_cb wkup_int_cb)
{
	GTRY_SMU->SMU_PCS_WKUP_ENL = SMU_PCSWKUP_SRC_ENL_MASK;
	GTRY_SMU->SMU_PCS_WKUP_ENH = SMU_PCSWKUP_SRC_ENH_MASK;

	wkup_int_callback = wkup_int_cb;
	GTRY_SMU->SMU_PCS_WKUP_ENL |= (0x1ul<<IRQ_WAKEUP_SOURCE);
	__nds__plic_set_priority(IRQ_WAKEUP_SOURCE, 3);
	__nds__plic_enable_interrupt(IRQ_WAKEUP_SOURCE);

	SMU_LOWPOWER_SET_BF_SLEEP();
}

void smu_enter_lowpower_mode(smu_lowpower_mode_t power_mode, unsigned int wkup_src_mask)
{
	SMU_LOWPOWER_FLASH_PWRDN();
	SMU_LOWPOWER_SET_EN();

	if(power_mode == SMU_LOWPOWER_DEEP_SLEEP || power_mode == SMU_LOWPOWER_SHUT_DOWN)
	{
		GTRY_SMU->SMU_ANA_WKUP_EN = wkup_src_mask;//SMU_SLPWKUP_SRC_ALL_MASK;
		GTRY_SMU->SMU_CLK_CFG = sysclk_src_cfg & 0xFFFFF0FF;//select IMO and ILO;
		GTRY_SMU->SMU_XTAL_ILO_CFG &= 0xFFFEFFFF;//close XTAL32K
		GTRY_SMU->SMU_XTAL_ILO_CFG |= 0x4000;//close XTAL16M
		GTRY_SMU->SMU_PLL_CFG1 |= 0x80000000;//disable pll
		GTRY_SMU->SMU_PWR_CFG1 &= 0xFFFFFFFB;//disable EN_PLLLDO
		GTRY_SMU->SMU_ANA_LP_EN = 0x1;
	}

	GTRY_SMU->SMU_PWR_CFG0 &= 0xFFFFFFFC;//disable EN_LVDET and EN_LVRST
	if(power_mode == SMU_LOWPOWER_DEEP_SLEEP)
	{
		GTRY_SMU->SMU_PCS_SCRATCH = 0x0;
		GTRY_SMU->RESERVED16 = 0x4;
		GTRY_SMU->SMU_PCS_CTRL = 0x3;
	}
	else if(power_mode == SMU_LOWPOWER_SHUT_DOWN)
	{
		GTRY_SMU->SMU_PCS_SCRATCH = 0x0;
		GTRY_SMU->RESERVED16 = 0x0;
		GTRY_SMU->SMU_PCS_CTRL = 0xB;
	}
	else
	{
		GTRY_SMU->SMU_PCS_SCRATCH = 0x0;
		GTRY_SMU->SMU_PCS_CTRL = 0x3;
	}

	if(wkup_src_mask&SMU_DPSLPWKUP_SRC_USB_MASK)
	{
		GTRY_SMU->SMU_CLK_CFG |= ((0xBul<<22));
		GTRY_USB->USB_POWER |= 0x01;
	}
	__asm__("nop");
	__asm__("nop");
	__asm__ __volatile__ ("wfi");

	GTRY_SMU->SMU_PWR_CFG0 |= 0x3;//enable EN_LVDET and EN_LVRST
	if((wkup_src_mask&SMU_DPSLPWKUP_SRC_USB_MASK))
	{
		GTRY_USB->USB_POWER &= (~0x01);
	}
	SMU_LOWPOWER_SET_DIS();
	SMU_LOWPOWER_FLASH_WKUP();
}

/**
 * @brief 	System entering deep sleep configuration initialization.
 * @param[in] wkup_src_mask锛歐ake-up source mask.
 * @retval 	鏃�
 */
void smu_enter_dpsleep_init(p_wkup_int_cb wkup_int_cb)
{
	GTRY_SMU->SMU_PCS_WKUP_ENL = ((0x1ul<<IRQ_RTC_ALARM_SOURCE)|(0x1ul<<IRQ_GPIO_SOURCE)|(0x1ul<<IRQ_USB_SOURCE));
	GTRY_SMU->SMU_PCS_WKUP_ENH = ((0x1ul<<(IRQ_LTIMER0_SOURCE-32))|(0x1ul<<(IRQ_LTIMER1_SOURCE-32)));

	GTRY_SMU->RESERVED16 = 0x4;
	GTRY_SMU->SMU_ANA_LP_EN = 0x1;

	wkup_int_callback = wkup_int_cb;
	GTRY_SMU->SMU_PCS_WKUP_ENL |= (0x1ul<<IRQ_WAKEUP_SOURCE);
	__nds__plic_set_priority(IRQ_WAKEUP_SOURCE, 3);
	__nds__plic_enable_interrupt(IRQ_WAKEUP_SOURCE);

	SMU_LOWPOWER_SET_BF_SLEEP();
}

void smu_enter_dpsleep_mode(unsigned int wkup_src_mask)
{
	SMU_LOWPOWER_FLASH_PWRDN();
	SMU_LOWPOWER_SET_EN();

	GTRY_SMU->SMU_ANA_WKUP_EN = wkup_src_mask;
	GTRY_SMU->SMU_CLK_CFG = sysclk_src_cfg & 0xFFFFF0FF;//select IMO and ILO;
	GTRY_SMU->SMU_XTAL_ILO_CFG &= 0xFFFEFFFF;//close XTAL32K
	GTRY_SMU->SMU_XTAL_ILO_CFG |= 0x4000;//close XTAL16M
	GTRY_SMU->SMU_PLL_CFG1 |= 0x80000000;//disable pll
	GTRY_SMU->SMU_PWR_CFG1 &= 0xFFFFFFFB;//disable EN_PLLLDO
	GTRY_SMU->SMU_PWR_CFG0 &= 0xFFFFFFFC;//disable EN_LVDET and EN_LVRST

	GTRY_SMU->SMU_PCS_SCRATCH = 0x0;
	GTRY_SMU->SMU_PCS_CTRL = 0x3;

	if(wkup_src_mask&SMU_DPSLPWKUP_SRC_USB_MASK)
	{
		GTRY_SMU->SMU_CLK_CFG |= ((0xBul<<22));
		GTRY_USB->USB_POWER |= 0x01;
	}
	
	__asm__("nop");
	__asm__("nop");
	__asm__ __volatile__ ("wfi");

	GTRY_SMU->SMU_PWR_CFG0 |= 0x3;//enable EN_LVDET and EN_LVRST
	if((wkup_src_mask&SMU_DPSLPWKUP_SRC_USB_MASK))
	{
		GTRY_USB->USB_POWER &= (~0x01);
	}
	SMU_LOWPOWER_SET_DIS();
	SMU_LOWPOWER_FLASH_WKUP();
}

IRQn_Type smu_get_wkup_irq_src(void)
{
	for(unsigned int i = 0; i < 32; i++)
	{
		if(GTRY_SMU->SMU_WKUP_SRCL & (0x1ul<<i))
		{
			return (IRQn_Type)i;
		}
	}
	for(unsigned int i = 0; i < 32; i++)
	{
		if(GTRY_SMU->SMU_WKUP_SRCH & (0x1ul<<i))
		{
			return (IRQn_Type)(i+32);
		}
	}
	return IRQ_MAX_SOURCE;
}

void smu_clear_wkup_irq_src(void)
{
	GTRY_SMU->SMU_WKUP_SRCL = 0xFFFFFFFF;
	GTRY_SMU->SMU_WKUP_SRCH = 0xFFFFFFFF;
}

void smu_periph_plic_reset(void)
{
	//reset all peripheral priority and interrupt set
	for(unsigned int i = 0; i < IRQ_MAX_SOURCE; i++)
	{
		__nds__plic_disable_interrupt((IRQn_Type)i);
	}
}


unsigned int smu_get_reset_src(void)
{
	unsigned int reset_sts = 0x0;

	if(GTRY_SMU->SMU_PCS_STS&0x2)
	{
		reset_sts = (GTRY_SMU->SMU_PCS_STS>>3)&SMU_PCS_RESET_MASK;
	}
	return reset_sts;
}

void wakeup_irq_handler(void)
{
	GTRY_SMU->SMU_PCS_STS = SMU_PCS_WKUP_INT_MASK;

	GTRY_SMU->SMU_CLK_CFG &= (~(0x3FFul<<22));
	if((sysclk_src_cfg & 0x700) > 0x100)
	{
		GTRY_SMU->SMU_PLL_CFG1 &= 0x7FFFFFFF;//enable pll
		GTRY_SMU->SMU_PWR_CFG1 |= 0x4;//enable EN_PLLLDO
		while((GTRY_SMU->SMU_SYS_STS&0x8) == 0x0);
	}
	GTRY_SMU->SMU_CLK_CFG = sysclk_src_cfg;

	if(wkup_int_callback != NULL)
	{
		wkup_int_callback(smu_get_wkup_irq_src());
	}
}


