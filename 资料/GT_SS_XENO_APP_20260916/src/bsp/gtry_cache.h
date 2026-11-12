/*
 * Copyright (c) 2019
 * All rights reserved.
 *
 */

#ifndef __GTRY_CACHE_H__
#define __GTRY_CACHE_H__

#include "gtry_core_v5.h"


/* Cache ops */
extern void nds_icache_invalidate_range(unsigned long start, unsigned long size);
extern void nds_icache_invalidate_all(void);
extern void nds_dcache_writeback_range(unsigned long start, unsigned long size);
extern void nds_dcache_invalidate_range(unsigned long start, unsigned long size);
extern void nds_dcache_flush_range(unsigned long start, unsigned long size);
extern void nds_dcache_flush_all(void);

/* DMA-specific ops */
extern void nds_dma_writeback_range(unsigned long start, unsigned long size);
extern void nds_dma_invalidate_range(unsigned long start, unsigned long size);
extern void nds_dma_invalidate_range2(unsigned long start, unsigned long size);


#endif /* __GTRY_CACHE_H__ */
