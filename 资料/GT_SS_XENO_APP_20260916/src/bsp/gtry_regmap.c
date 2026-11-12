/*
 * Copyright (c) 2019
 * All rights reserved.
 *
 */
#include "gtry_type.h"
#include "gtry_build_cfg.h"
#include "gtry_core_v5.h"
#include "gtry_regmap.h"


/* This must be a leaf function, no child function */
void __platform_init (void) __attribute__((naked));
void __platform_init(void)
{
	/* Do your platform low-level initial */
	__asm("ret");
}

void c_startup(void)
{
	/* Data section initialization */
	extern char  _edata, _end;
	unsigned int size;

#ifdef CFG_XIP
	unsigned int i;
	unsigned int *src_ptr, *dst_ptr;
	extern unsigned int __data_lmastart, __data_start;
	extern unsigned int __ramrun_lmastart, __ramrun_lmaend, __ramrun_start;
	extern unsigned int __sram_lmastart, __sram_lmaend, __sram_start;

	/* Copy data section from LMA to VMA */
	size = ((unsigned int)&_edata - (unsigned int)&__data_start + (sizeof(int) - 1)) / sizeof(int);
	src_ptr = (unsigned int *)&__data_lmastart;
	dst_ptr = (unsigned int *)&__data_start;
	for (i = 0; i < size; i++)
		*dst_ptr++ = *src_ptr++;

	/* Copy .ramrun* from LMA to VMA */
	size = ((unsigned int)&__ramrun_lmaend - (unsigned int)&__ramrun_lmastart + (sizeof(int) - 1)) / sizeof(int);
	src_ptr = (unsigned int *)&__ramrun_lmastart;
	dst_ptr = (unsigned int *)&__ramrun_start;
	for (i = 0; i < size; i++)
		*dst_ptr++ = *src_ptr++;

	/* Copy sram section from LMA to VMA */
	size = ((unsigned int)&__sram_lmaend - (unsigned int)&__sram_lmastart + (sizeof(int) - 1)) / sizeof(int);
	src_ptr = (unsigned int *)&__sram_lmastart;
	dst_ptr = (unsigned int *)&__sram_start;
	for (i = 0; i < size; i++)
		*dst_ptr++ = *src_ptr++;
#elif defined(CFG_LOAD)
	unsigned int i;
	unsigned int *src_ptr, *dst_ptr;
	extern unsigned int __data_lmastart, __data_start;
	extern unsigned int __sram_lmastart, __sram_lmaend, __sram_start;

	/* Copy data section from LMA to VMA */
	size = ((unsigned int)&_edata - (unsigned int)&__data_start + (sizeof(int) - 1)) / sizeof(int);
	src_ptr = (unsigned int *)(GTRY_SMU->SMU_PCS_SCRATCH+(unsigned int)&__data_lmastart);
	dst_ptr = (unsigned int *)&__data_start;
	for (i = 0; i < size; i++)
		*dst_ptr++ = *src_ptr++;

	/* Copy sram section from LMA to VMA */
	size = ((unsigned int)&__sram_lmaend - (unsigned int)&__sram_lmastart + (sizeof(int) - 1)) / sizeof(int);
	src_ptr = (unsigned int *)(GTRY_SMU->SMU_PCS_SCRATCH+(unsigned int)&__sram_lmastart);
	dst_ptr = (unsigned int *)&__sram_start;
	for (i = 0; i < size; i++)
		*dst_ptr++ = *src_ptr++;
#endif

	/* Clear bss section */
	size = &_end - &_edata;
	MEMSET(&_edata, 0, size);
}

void system_init(void)
{
	/*
	 * Do your system reset handling here
	 */
	/* Reset the CPU reset vector for this program. */
	GTRY_SMU->SMU_RST_VEC_LP = FLASH_BASE;

	/* Enable PLIC features */
	if (read_csr(NDS_MMISC_CTL) & (1 << 1)) {
		/* External PLIC interrupt is vectored */
		__nds__plic_set_feature(NDS_PLIC_FEATURE_PREEMPT | NDS_PLIC_FEATURE_VECTORED);
	} else {
		/* External PLIC interrupt is NOT vectored */
		__nds__plic_set_feature(NDS_PLIC_FEATURE_PREEMPT);
	}

	/* Enable misaligned access and non-blocking load. */
	set_csr(NDS_MMISC_CTL, (1 << 8) | (1 << 6));

#ifdef CFG_CACHE_ENABLE
	/*
	 * Enable I/D cache with HW prefetcher,
	 * D-cache write-around (threshold: 4 cache lines),
	 * and CM (Coherence Manager).
	 */
	clear_csr(NDS_MCACHE_CTL, (3 << 13));
	set_csr(NDS_MCACHE_CTL, (1 << 19) | (1 << 13) | (1 << 10) | (1 << 9) | (1 << 1) | (1 << 0));

	/* Check if CPU support CM or not. */
	if (read_csr(NDS_MCACHE_CTL) & (1 << 19)) {
		/* Wait for cache coherence enabling completed */
		while((read_csr(NDS_MCACHE_CTL) & (1 << 20)) == 0);
	}

	/* Enable L2 cache if HW supports */
	if (GTRY_SMU->SMU_SYS_CFG & (1 << 8)) {
		/* Configure L2 related functions. */
		GTRY_L2C->L2C_CTRL |= ((3 << 5) | (3 << 3));
		/* For GEN1, L2 cache is disabled by default and needs to be configured twice when enabling L2 cache*/
		if (((GTRY_L2C->L2C_CFG & (0xFF << 24)) >> 24) < 16) {
			GTRY_L2C->L2C_CTRL |= (1 << 0);
		}
	}
#else
	/* Disable L2 cache. For GEN2, L2 cache is enabled by default and needs to be disbaled. */
	if (GTRY_SMU->SMU_SYS_CFG & (1 << 8)) {
		GTRY_L2C->L2C_CTRL &= ~(1 << 0);
	}
#endif
}
