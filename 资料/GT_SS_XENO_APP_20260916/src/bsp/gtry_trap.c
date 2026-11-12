/*
 * Copyright (c) 2019
 * All rights reserved.
 *
 */

#include "gtry_regmap.h"
#include "gtry_core_v5.h"

typedef struct {
	union {
		struct {
			long x1;
			long x4;
			long x5;
			long x6;
			long x7;
			long x10;
			long x11;
			long x12;
			long x13;
			long x14;
			long x15;
			long x16;
			long x17;
			long x28;
			long x29;
			long x30;
			long x31;
		};
		long caller_regs[17];
	};
	long mepc;
	long mstatus;
	long mxstatus;
#ifdef __riscv_flen
#if __riscv_flen == 64
	long long fp_caller_regs[20];
#else
	long fp_caller_regs[20];
#endif
#endif
} SAVED_CONTEXT;

typedef void (*isr_func)(void);

void default_irq_handler(void)
{
	while(1);
}

void rtc_period_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//1
void rtc_alarm_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//2

void hasp_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//3 wr
void spi0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//5
void i2c0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//6
void gpio_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//7
void uart0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//8
void uart1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//9
void dma0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//10

void dma1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//12
void uart2_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//13
void i2c1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//14

void usb_dma_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//17
void usb_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//18
void spi1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//19
void spi2_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//20
void spi3_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//21

void wakeup_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//27

void spi4_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//32
void spi5_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//33
void spi6_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//34
void pwm_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//35
void timer_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//36
void trng_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//37
void ioam_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//38
void i2s_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//39
void r8080_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//40
void acmp0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//41
void acmp1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//42
void rgb_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//43
void adc_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//44
void tsc_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//45
void dvp_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//46
void aes_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//47
void Ltim0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//48
void Ltim1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//49
void sdio_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//50
void argb_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));//51

const isr_func irq_handler[] = {
	default_irq_handler,//0
	rtc_period_irq_handler,//1
	rtc_alarm_irq_handler,//2
	hasp_irq_handler,//3  wr
	default_irq_handler,//4
	spi0_irq_handler,//5
	i2c0_irq_handler,//6
	gpio_irq_handler,//7
	uart0_irq_handler,//8
	uart1_irq_handler,//9
	dma0_irq_handler,//10
	default_irq_handler,//11
	dma1_irq_handler,//12
	uart2_irq_handler,//13
	i2c1_irq_handler,//14
	default_irq_handler,//15
	default_irq_handler,//16
	usb_dma_irq_handler,//17
	usb_irq_handler,//18
	spi1_irq_handler,//19
	spi2_irq_handler,//20
	spi3_irq_handler,//21
	default_irq_handler,//22
	default_irq_handler,//23
	default_irq_handler,//24
	default_irq_handler,//25
	default_irq_handler,//26
	wakeup_irq_handler,//27
	default_irq_handler,//28
	default_irq_handler,//29
	default_irq_handler,//30
	default_irq_handler,//31
	spi4_irq_handler,//32
	spi5_irq_handler,//33
	spi6_irq_handler,//34
	pwm_irq_handler,//35
	timer_irq_handler,//36
	trng_irq_handler,//37
	ioam_irq_handler,//38
	i2s_irq_handler,//39
	r8080_irq_handler,//40
	acmp0_irq_handler,//41
	acmp1_irq_handler,//42
	rgb_irq_handler,//43
	adc_irq_handler,//44
	tsc_irq_handler,//45
	dvp_irq_handler,//46
	aes_irq_handler,//47
	Ltim0_irq_handler,//48
	Ltim1_irq_handler,//49
	sdio_irq_handler,//50
	argb_irq_handler,//51
	default_irq_handler//52
};

void mext_interrupt(unsigned int irq_source)
{
	/* Enable interrupts in general to allow nested */
	set_csr(NDS_MSTATUS, MSTATUS_MIE);

	/* Do interrupt handler */
	irq_handler[irq_source]();

	__nds__plic_complete_interrupt(irq_source);
}


__attribute__((weak)) void mtimer_handler(void)
{
	clear_csr(NDS_MIE, MIP_MTIP);
}

__attribute__((weak)) void mswi_handler(void)
{
	clear_csr(NDS_MIE, MIP_MSIP);
}

__attribute__((weak)) void syscall_handler(long n, long a0, long a1, long a2, long a3)
{
}

__attribute__((weak)) long except_handler(long cause, long epc, long *reg)
{
	/* Unhandled Trap */
	return 0;
}

void trap_handler(unsigned long mcause, SAVED_CONTEXT *context)
{
	/* Do your trap handling */
	if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_EXT)) {
		/* Machine-level interrupt from PLIC */
		mext_interrupt(__nds__plic_claim_interrupt());
	} else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER)) {
		/* Machine timer interrupt */
		mtimer_handler();
	} else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_SOFT)) {
		/* Machine SWI interrupt */
		mswi_handler();
	} else if (!(mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == TRAP_M_ECALL)) {
		/* Machine Syscal call */
		syscall_handler(context->x17, context->x10, context->x11, context->x12, context->x13);
		context->mepc += 4;
	}else {
		/* Unhandled Trap */
		context->mepc = except_handler(mcause, context->mepc, context->caller_regs);
	}
}
