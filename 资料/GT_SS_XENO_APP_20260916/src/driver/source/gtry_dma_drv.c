/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

#include "gtry_regmap.h"
#include "gtry_dma_drv.h"


#ifdef CFG_CACHE_ENABLE
#include "gtry_cache.h"
#endif

typedef struct {
	unsigned long      SrcAddr;
	unsigned long      DstAddr;
	uint32_t           Size;
	uint32_t           Cnt;
	p_dma_int_cb  cb_event;
} DMA_Channel_Info;

static uint32_t channel_active[DMA_NUMBER_MAX] = {0U};
static uint32_t init_cnt[DMA_NUMBER_MAX]       = {0U};

static DMA_Channel_Info channel_info[DMA_NUMBER_MAX][DMA_NUM_OF_CHLS];
#define GTRY_DMA(No)  		((DMA_RegDef *)((uint32_t)GTRY_DMA0+No*((uint32_t)GTRY_DMA1-(uint32_t)GTRY_DMA0)))
#define DMA_CHANNEL(No, n)  ((DMA_CHL_REG *)((uint32_t)&GTRY_DMA0->DMA_CHL[n]+No*((uint32_t)GTRY_DMA1-(uint32_t)GTRY_DMA0)))
#define DMA0_CHANNEL(n)  	GTRY_DMA0->DMA_CHL[n]
#define DMA1_CHANNEL(n)  	GTRY_DMA1->DMA_CHL[n]

#ifdef CFG_CACHE_ENABLE
#define DRAM_START   0x00000000U
#define DRAM_END     0x7FFFFFFFU
#define IS_ADDR_IN_RAM(addr) ((addr >= DRAM_START) && (addr <= DRAM_END))
#endif

#ifdef CFG_CACHE_ENABLE
#define DMA_DCACHE_WRITEBACK(start, size)        nds_dma_writeback_range(start, size)
#define DMA_DCACHE_INVALID(start, size)          nds_dma_invalidate_range(start, size)
#define DMA_DCACHE_INVALID_AFTER(start, size)    nds_dma_invalidate_range2(start, size)
#else
#define DMA_DCACHE_WRITEBACK(start, size)        NULL
#define DMA_DCACHE_INVALID(start, size)          NULL
#define DMA_DCACHE_INVALID_AFTER(start, size)    NULL
#endif

/**
  \fn          int32_t set_channel_active_flag (uint8_t ch)
  \brief       Protected set of channel active flag
  \param[in]   ch        Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
__inline static int32_t set_channel_active_flag (uint32_t dma_no, uint8_t ch) {

	uint8_t gie = (read_csr(NDS_MSTATUS) & (1 << 3));

	if (gie) {
		/* Disable global interrupt for core */
		clear_csr(NDS_MSTATUS, MSTATUS_MIE);
	}

	if (channel_active[dma_no] & (1U << ch)) {
		if (gie) {
			/* Enable interrupts in general. */
			set_csr(NDS_MSTATUS, MSTATUS_MIE);
		}
		return -1;
	}
	channel_active[dma_no] |= (1U << ch);

	if (gie) {
		/* Enable interrupts in general. */
		set_csr(NDS_MSTATUS, MSTATUS_MIE);
	}

	return 0;
}

/**
  \fn          void clear_channel_active_flag (uint8_t ch)
  \brief       Protected clear of channel active flag
  \param[in]   ch        Channel number (0..7)
*/
__inline static void clear_channel_active_flag (uint32_t dma_no, uint8_t ch) {

	uint8_t gie = (read_csr(NDS_MSTATUS) & (1 << 3));

	if (gie) {
		/* Disable global interrupt for core */
		clear_csr(NDS_MSTATUS, MSTATUS_MIE);
	}

  	channel_active[dma_no] &= ~(1U << ch);

	if (gie) {
		/* Enable interrupts in general. */
		set_csr(NDS_MSTATUS, MSTATUS_MIE);
	}
}

inline uint32_t dma_channel_get_intTC (uint32_t dma_no, uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUM_OF_CHLS) {
		return 0;
	}

	return GTRY_DMA(dma_no)->DMA_INT_STS&DMA_INT_STS_TC(ch);
}

inline uint32_t dma_channel_get_intABT (uint32_t dma_no, uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUM_OF_CHLS) {
		return 0;
	}

	return GTRY_DMA(dma_no)->DMA_INT_STS&DMA_INT_STS_ABT(ch);
}

inline uint32_t dma_channel_get_intERR (uint32_t dma_no, uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUM_OF_CHLS) {
		return 0;
	}

	return GTRY_DMA(dma_no)->DMA_INT_STS&DMA_INT_STS_ERR(ch);
}

inline void dma_channel_clear_intsts (uint32_t dma_no, uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUM_OF_CHLS) {
		return;
	}

	GTRY_DMA(dma_no)->DMA_INT_STS = DMA_INT_STS_TC(ch)|DMA_INT_STS_ABT(ch)|DMA_INT_STS_ERR(ch);
}

/**
  \fn          uint32_t get_channel_active_flag (uint8_t ch)
  \brief       Check if DMA channel is enabled or disabled
  \param[in]   ch Channel number (0..7)
  \returns     Channel status
   - \b  1: channel enabled
   - \b  0: channel disabled
*/
uint32_t get_channel_active_flag (uint32_t dma_no, uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUM_OF_CHLS) {
		return 0U;
	};

	if (channel_active[dma_no] & (1 << ch)) {
		return 1U;
	} else {
		return 0U;
	}
}


/**
  \fn          int32_t dma_initialize (void)
  \brief       Initialize DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_initialize (uint32_t dma_no) {

	uint32_t ch_num;

	init_cnt[dma_no]++;

	// Check if already initialized
	if (init_cnt[dma_no] > 1U) {
		return 0;
	}

	// Reset DMA
	GTRY_DMA(dma_no)->DMA_CTRL = 1U;

	// Reset all DMA channels
	for (ch_num = 0U; ch_num < DMA_NUM_OF_CHLS; ch_num++) {
		DMA_CHANNEL(dma_no, ch_num)->CHL_CTRL     = 0U;
		channel_info[dma_no][ch_num].SrcAddr  = 0UL;
		channel_info[dma_no][ch_num].DstAddr  = 0UL;
		channel_info[dma_no][ch_num].Size     = 0U;
		channel_info[dma_no][ch_num].Cnt      = 0U;
	}

	// Clear all DMA interrupt flags
	GTRY_DMA(dma_no)->DMA_INT_STS = 0xFFFFFF;

	// Priority must be set > 0 to trigger the interrupt
	__nds__plic_set_priority(IRQ_DMA0_SOURCE+(dma_no<<1), 1);

	// Enable PLIC interrupt DMA source
	__nds__plic_enable_interrupt(IRQ_DMA0_SOURCE+(dma_no<<1));

	return 0;
}

/**
  \fn          int32_t dma_uninitialize (void)
  \brief       De-initialize DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_uninitialize (uint32_t dma_no) {

	// Check if DMA is initialized
	if (init_cnt[dma_no] == 0U) {
		return -1;
	}

	init_cnt[dma_no]--;
	if (init_cnt[dma_no] != 0U) {
		return 0;
	}

	// Disable and Clear DMA IRQ
	__nds__plic_disable_interrupt(IRQ_DMA0_SOURCE+(dma_no<<1));

	return 0;
}


/**
  \fn          int32_t dma_channel_configure (uint8_t              ch,
                                              unsigned long        src_addr,
                                              unsigned long        dst_addr,
                                              uint32_t             size,
                                              uint32_t             control,
                                              uint32_t             config,
                                              p_dma_int_cb    cb_event)
  \brief       Configure DMA channel for next transfer
  \param[in]   ch        Channel number (0..7)
  \param[in]   src_addr  Source address
  \param[in]   dst_addr  Destination address
  \param[in]   size      Amount of data to transfer
  \param[in]   control   Channel control
  \param[in]   config    Channel configuration
  \param[in]   cb_event  Channel callback pointer
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_channel_configure (uint32_t dma_no,	uint8_t            ch,
				unsigned long           src_addr,
				unsigned long           dst_addr,
				uint32_t           size,
				uint32_t           control,
				p_dma_int_cb  cb_event) {

	DMA_CHL_REG* dma_ch;

	// Check if channel is valid
	if (ch >= DMA_NUM_OF_CHLS) {
		return -1;
	}

	// Set Channel active flag
	if (set_channel_active_flag (dma_no, ch) == -1) {
		return -1;
	}

#ifdef CFG_CACHE_ENABLE
	if ((control & DMA_CH_CTRL_DMODE_HANDSHAKE) && IS_ADDR_IN_RAM(src_addr)) {
		DMA_DCACHE_WRITEBACK(src_addr, size);
	}
	if ((control & DMA_CH_CTRL_SMODE_HANDSHAKE) && IS_ADDR_IN_RAM(dst_addr)) {
		DMA_DCACHE_INVALID(dst_addr, size);
	}
#endif

	// Save callback pointer
	channel_info[dma_no][ch].cb_event = cb_event;

	dma_ch = DMA_CHANNEL(dma_no, ch);

	// Reset DMA Channel configuration
	dma_ch->CHL_CTRL = 0U;

	// Clear DMA interrupts status
	GTRY_DMA(dma_no)->DMA_INT_STS = (1U << (16+ch));
	GTRY_DMA(dma_no)->DMA_INT_STS = (1U << (8+ch));
	GTRY_DMA(dma_no)->DMA_INT_STS = (1U << ch);

	// Link list not supported
	dma_ch->CHL_LL_PL = 0U;
	dma_ch->CHL_LL_PH = 0U;

	channel_info[dma_no][ch].Size = size;
	if (size > 0x3FFFFFU) {
		// Max DMA transfer size = 4M
		size = 0x3FFFFFU;
	}

	dma_ch->CHL_TRAN_SIZE = size;
	// Set Source and Destination address
	dma_ch->CHL_SRC_ADDRL = src_addr & 0xFFFFFFFFU;
	dma_ch->CHL_DST_ADDRL = dst_addr & 0xFFFFFFFFU;
#if __riscv_xlen == 32
	dma_ch->CHL_SRC_ADDRH = 0U;
	dma_ch->CHL_DST_ADDRH = 0U;
#else
	dma_ch->CHL_SRC_ADDRH = (src_addr >> 32);
	dma_ch->CHL_DST_ADDRH = (dst_addr >> 32);
#endif


	if (!(control & DMA_CH_CTRL_SRCADDR_FIX)) {
		if (control & DMA_CH_CTRL_SRCADDR_DEC) {
			// Source address decrement
			src_addr -= (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
		} else {
			// Source address increment
			src_addr += (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
		}
	}

	if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
		if (control & DMA_CH_CTRL_DSTADDR_DEC) {
			// Source address decrement
			dst_addr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		} else {
			// Destination address increment
			dst_addr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
	}

	// Save channel information
	channel_info[dma_no][ch].SrcAddr = src_addr;
	channel_info[dma_no][ch].DstAddr = dst_addr;
	channel_info[dma_no][ch].Cnt     = size;

	// Compiler barrier to ensure channel_info[ch] is completed before setup DMA CTRL register.
	asm volatile("" ::: "memory");

	dma_ch->CHL_CTRL = control;

	if ((control & DMA_CH_CTRL_ENABLE) == 0U) {
		// Clear Channel active flag
		clear_channel_active_flag (dma_no, ch);
	}

	return 0;
}

int32_t dma_channel_reconfig (uint32_t dma_no,	uint8_t            ch,
				unsigned long           src_addr,
				unsigned long           dst_addr,
				uint32_t           size) {

	DMA_CHL_REG* dma_ch;

	// Check if channel is valid
	if (ch >= DMA_NUM_OF_CHLS) {
		return -1;
	}

	// Set Channel active flag
	if (set_channel_active_flag (dma_no, ch) == -1) {
		return -1;
	}

	dma_ch = DMA_CHANNEL(dma_no, ch);

	if (size > 0x3FFFFFU) {
		// Max DMA transfer size = 4M
		size = 0x3FFFFFU;
	}

	dma_ch->CHL_TRAN_SIZE = size;
	// Set Source and Destination address
	dma_ch->CHL_SRC_ADDRL = src_addr;
	dma_ch->CHL_DST_ADDRL = dst_addr;

	dma_ch->CHL_CTRL |= DMA_CH_CTRL_ENABLE;
	return 0;
}

uint32_t dma_spichl_rx_reconfig (uint32_t spi_chl, unsigned long dst_addr, uint32_t size)
{
	DMA0_CHANNEL(spi_chl).CHL_DST_ADDRL = dst_addr;

	DMA0_CHANNEL(spi_chl).CHL_TRAN_SIZE = size;
	// Set Source and Destination address

	DMA0_CHANNEL(spi_chl).CHL_CTRL |= DMA_CH_CTRL_ENABLE;
	return 0;
}
#if 0
uint32_t dma_i2schl_tx_reconfig (unsigned long src_addr, uint32_t size)
{
	DMA1_CHANNEL(GTRY_I2S_DMA_TX_CH).SRCADDRL = src_addr;
	//DMA1_CHANNEL[ch]->DSTADDRL = dst_addr;

	DMA1_CHANNEL(GTRY_I2S_DMA_TX_CH).TRANSIZE = size;
	// Set Source and Destination address

	DMA1_CHANNEL(GTRY_I2S_DMA_TX_CH).CTRL |= DMA_CH_CTRL_ENABLE;
	return 0;
}

uint32_t dma_i2schl_rx_reconfig (unsigned long dst_addr, uint32_t size)
{
	DMA1_CHANNEL(GTRY_I2S_DMA_RX_CH).DSTADDRL = dst_addr;
	//DMA1_CHANNEL[ch]->DSTADDRL = dst_addr;

	DMA1_CHANNEL(GTRY_I2S_DMA_RX_CH).TRANSIZE = size;
	// Set Source and Destination address

	DMA1_CHANNEL(GTRY_I2S_DMA_RX_CH).CTRL |= DMA_CH_CTRL_ENABLE;
	return 0;
}
#endif

/**
  \fn          int32_t dma_channel_enable (uint8_t ch)
  \brief       Enable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_channel_enable (uint32_t dma_no, uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUM_OF_CHLS) {
		return -1;
	}

	// Set Channel active flag
	if (set_channel_active_flag (dma_no, ch) == -1) {
		return -1;
	}

	DMA_CHANNEL(dma_no, ch)->CHL_CTRL |= DMA_CH_CTRL_ENABLE;
	return 0;
}

/**
  \fn          int32_t dma_channel_disable (uint8_t ch)
  \brief       Disable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_channel_disable (uint32_t dma_no, uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUM_OF_CHLS) {
		return -1;
	}

#ifdef CFG_CACHE_ENABLE
	uint32_t dst_addr = channel_info[dma_no][ch].DstAddr;
	uint32_t cnt = channel_info[dma_no][ch].Cnt;
	uint32_t size = channel_info[dma_no][ch].Size;
	uint32_t control = DMA_CHANNEL(dma_no, ch)->CTRL;

	if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
		if (control & DMA_CH_CTRL_DSTADDR_DEC) {
			// Source address increment to complemetal
			dst_addr += (cnt << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		} else {
			// Destination address decrement to complemetal
			dst_addr -= (cnt << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
	}

	if ((control & DMA_CH_CTRL_SMODE_HANDSHAKE) && IS_ADDR_IN_RAM(dst_addr)) {
		DMA_DCACHE_INVALID_AFTER(dst_addr, size);
	}
#endif

	// Clear Channel active flag
	clear_channel_active_flag (dma_no, ch);

	DMA_CHANNEL(dma_no, ch)->CHL_CTRL &= ~DMA_CH_CTRL_ENABLE;

	return 0;
}

/**
  \fn          int32_t dma_channel_abort (uint8_t ch)
  \brief       Disable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t dma_channel_abort (uint32_t dma_no, uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUM_OF_CHLS) {
		return -1;
	}

#ifdef CFG_CACHE_ENABLE
	uint32_t dst_addr = channel_info[ch].DstAddr;
	uint32_t cnt = channel_info[ch].Cnt;
	uint32_t size = channel_info[ch].Size;
	uint32_t control = DMA_CHANNEL(dma_no, ch)->CTRL;

	if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
		if (control & DMA_CH_CTRL_DSTADDR_DEC) {
			// Source address increment to complemetal
			dst_addr += (cnt << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		} else {
			// Destination address decrement to complemetal
			dst_addr -= (cnt << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
		}
	}

	if ((control & DMA_CH_CTRL_SMODE_HANDSHAKE) && IS_ADDR_IN_RAM(dst_addr)) {
		DMA_DCACHE_INVALID_AFTER(dst_addr, size);
	}
#endif

	// Clear Channel active flag
	clear_channel_active_flag (dma_no, ch);

	// abort DMA ch transfer
	GTRY_DMA(dma_no)->DMA_CHL_ABT = (1U << ch);

	DMA_CHANNEL(dma_no, ch)->CHL_CTRL     = 0U;
	channel_info[dma_no][ch].SrcAddr  = 0UL;
	channel_info[dma_no][ch].DstAddr  = 0UL;
	channel_info[dma_no][ch].Size     = 0U;
	channel_info[dma_no][ch].Cnt      = 0U;

	// Clear DMA interrupts status
	GTRY_DMA(dma_no)->DMA_INT_STS = (1U << (16+ch));
	GTRY_DMA(dma_no)->DMA_INT_STS = (1U << (8+ch));
	GTRY_DMA(dma_no)->DMA_INT_STS = (1U << ch);

	return 0;
}

uint32_t dma_channel_get_txaddr (uint32_t dma_no, uint8_t ch)
{
	return DMA_CHANNEL(dma_no, ch)->CHL_SRC_ADDRL;//&(~0xa0000000);
}

uint32_t dma_channel_get_rxaddr (uint32_t dma_no, uint8_t ch)
{
	return DMA_CHANNEL(dma_no, ch)->CHL_DST_ADDRL;//&(~0xa0000000);
}

/**
  \fn          uint32_t dma_channel_get_count (uint8_t ch)
  \brief       Get number of transferred data
  \param[in]   ch Channel number (0..7)
  \returns     Number of transferred data
*/
uint32_t dma_channel_get_count (uint32_t dma_no, uint8_t ch) {

	// Check if channel is valid
	if (ch >= DMA_NUM_OF_CHLS) {
		return 0;
	}

	return (channel_info[dma_no][ch].Cnt - (DMA_CHANNEL(dma_no, ch)->CHL_TRAN_SIZE & 0x3FFFFF));
}

/**
  \fn          void dma_irq_handler (void)
  \brief       DMA interrupt handler
*/
void dma0_irq_handler (void) {

	uint32_t ch, size;
	DMA_CHL_REG * dma_ch;

	for (ch = 0; ch < DMA_NUM_OF_CHLS; ch++)
	{
//		if ((GTRY_DMA0->DMA_INT_STS & (1U << ch)) == 0)
		{
			dma_ch = DMA_CHANNEL(0, ch);

			// Terminal count request interrupt
			if (GTRY_DMA0->DMA_INT_STS & (1U << (16 + ch)))
			{
				// Clear interrupt flag
				GTRY_DMA0->DMA_INT_STS = (1U << (16 + ch));

				if (channel_info[0][ch].Cnt != channel_info[0][ch].Size)
				{
					// Data waiting to transfer
					uint32_t control;

					size = channel_info[0][ch].Size - channel_info[0][ch].Cnt;
					// Max DMA transfer size = 4M
					if (size > 0x3FFFFFU) {
						size = 0x3FFFFFU;
					}

					channel_info[0][ch].Cnt += size;
					control = dma_ch->CHL_CTRL;

					if (!(control & DMA_CH_CTRL_SRCADDR_FIX)) {
						dma_ch->CHL_SRC_ADDRL = channel_info[0][ch].SrcAddr & 0xFFFFFFFFU;
#if __riscv_xlen == 32
						dma_ch->CHL_SRC_ADDRH = 0U;
#else
						dma_ch->CHL_SRC_ADDRH = (channel_info[0][ch].SrcAddr >> 32);
#endif
						if (control & DMA_CH_CTRL_SRCADDR_DEC) {
							// Source address decrement
							channel_info[0][ch].SrcAddr -= (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
						} else {
							// Source address increment
							channel_info[0][ch].SrcAddr += (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
						}
					}
					if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
						dma_ch->CHL_DST_ADDRL = channel_info[0][ch].DstAddr & 0xFFFFFFFFU;
#if __riscv_xlen == 32
						dma_ch->CHL_DST_ADDRH = 0U;
#else
						dma_ch->CHL_DST_ADDRH = (channel_info[ch].DstAddr >> 32);
#endif
						if (control & DMA_CH_CTRL_DSTADDR_DEC) {
							// Source address decrement
							channel_info[0][ch].DstAddr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						} else {
							// Destination address increment
							channel_info[0][ch].DstAddr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						}
					}

					// Set transfer size
					dma_ch->CHL_TRAN_SIZE = size;
					// Enable DMA Channel
					dma_ch->CHL_CTRL |= DMA_CH_CTRL_ENABLE;
				}
				else
				{
					// All Data has been transferred

#ifdef CFG_CACHE_ENABLE
					uint32_t dst_addr = channel_info[0][ch].DstAddr;
					uint32_t control = dma_ch->CHL_CTRL;
					uint32_t size = channel_info[0][ch].Size;

					if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
						if (control & DMA_CH_CTRL_DSTADDR_DEC) {
							// Source address increment to complemetal
							dst_addr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						} else {
							// Destination address decrement to complemetal
							dst_addr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						}
					}
					if ((control & DMA_CH_CTRL_SMODE_HANDSHAKE) && IS_ADDR_IN_RAM(dst_addr)) {
						DMA_DCACHE_INVALID_AFTER(dst_addr, size);
					}
#endif
					// Clear Channel active flag
					clear_channel_active_flag (0, ch);

					// Signal Event
					if (channel_info[0][ch].cb_event) {
						channel_info[0][ch].cb_event(DMA_EVENT_TC);
					}
				}
			}
			// DMA error interrupt
			else if (GTRY_DMA0->DMA_INT_STS & (1U << ch))
			{
				dma_ch->CHL_CTRL = 0U;
				// Clear Channel active flag
				clear_channel_active_flag (0, ch);

				// Clear interrupt flag
				GTRY_DMA0->DMA_INT_STS = (1U << ch);

				// Signal Event
				if (channel_info[0][ch].cb_event) {
					channel_info[0][ch].cb_event(DMA_EVENT_ERROR);
				}

			}
			// DMA abort interrupt
			else if (GTRY_DMA0->DMA_INT_STS & (1U << (8 + ch)))
			{
				dma_ch->CHL_CTRL = 0U;
				// Clear Channel active flag
				clear_channel_active_flag (0, ch);

				// Clear interrupt flag
				GTRY_DMA0->DMA_INT_STS = (1U << (8 + ch));

				// Signal Event
				if (channel_info[0][ch].cb_event) {
					channel_info[0][ch].cb_event(DMA_EVENT_ABORT);
				}
			}
		}
	}
}

void dma1_irq_handler (void) {

	uint32_t ch, size;
	DMA_CHL_REG * dma_ch;

    for (ch = 0; ch < DMA_NUM_OF_CHLS; ch++)
    {
//		if ((GTRY_DMA1->DMA_INT_STS & (1U << ch)) == 0)
		{
			dma_ch = DMA_CHANNEL(1, ch);

			// Terminal count request interrupt
			if (GTRY_DMA1->DMA_INT_STS & (1U << (16 + ch)))
			{
				// Clear interrupt flag
				GTRY_DMA1->DMA_INT_STS = (1U << (16 + ch));

				if (channel_info[1][ch].Cnt != channel_info[1][ch].Size) {
					// Data waiting to transfer
					uint32_t control;

					size = channel_info[1][ch].Size - channel_info[1][ch].Cnt;
					// Max DMA transfer size = 4M
					if (size > 0x3FFFFFU) {
						size = 0x3FFFFFU;
					}

					channel_info[1][ch].Cnt += size;
					control = dma_ch->CHL_CTRL;

					if (!(control & DMA_CH_CTRL_SRCADDR_FIX)) {
						dma_ch->CHL_SRC_ADDRL = channel_info[1][ch].SrcAddr & 0xFFFFFFFFU;
#if __riscv_xlen == 32
						dma_ch->CHL_SRC_ADDRH = 0U;
#else
						dma_ch->CHL_SRC_ADDRH = (channel_info[1][ch].SrcAddr >> 32);
#endif
						if (control & DMA_CH_CTRL_SRCADDR_DEC) {
							// Source address decrement
							channel_info[1][ch].SrcAddr -= (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
						} else {
							// Source address increment
							channel_info[1][ch].SrcAddr += (size << ((control & DMA_CH_CTRL_SWIDTH_MASK) >> DMA_CH_CTRL_SWIDTH_POS));
						}
					}
					if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
						dma_ch->CHL_DST_ADDRL = channel_info[1][ch].DstAddr & 0xFFFFFFFFU;
#if __riscv_xlen == 32
						dma_ch->CHL_DST_ADDRH = 0U;
#else
						dma_ch->CHL_DST_ADDRH = (channel_info[ch].DstAddr >> 32);
#endif
						if (control & DMA_CH_CTRL_DSTADDR_DEC) {
							// Source address decrement
							channel_info[1][ch].DstAddr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						} else {
							// Destination address increment
							channel_info[1][ch].DstAddr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						}
					}

					// Set transfer size
					dma_ch->CHL_TRAN_SIZE = size;
					// Enable DMA Channel
					dma_ch->CHL_CTRL |= DMA_CH_CTRL_ENABLE;
				}
				else
				{
					// All Data has been transferred
#ifdef CFG_CACHE_ENABLE
					uint32_t dst_addr = channel_info[ch].DstAddr;
					uint32_t control = dma_ch->CHL_CTRL;
					uint32_t size = channel_info[1][ch].Size;

					if (!(control & DMA_CH_CTRL_DSTADDR_FIX)) {
						if (control & DMA_CH_CTRL_DSTADDR_DEC) {
							// Source address increment to complemetal
							dst_addr += (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						} else {
							// Destination address decrement to complemetal
							dst_addr -= (size << ((control & DMA_CH_CTRL_DWIDTH_MASK) >> DMA_CH_CTRL_DWIDTH_POS));
						}
					}
					if ((control & DMA_CH_CTRL_SMODE_HANDSHAKE) && IS_ADDR_IN_RAM(dst_addr)) {
						DMA_DCACHE_INVALID_AFTER(dst_addr, size);
					}
#endif
					// Clear Channel active flag
					clear_channel_active_flag (1, ch);

					// Signal Event
					if (channel_info[1][ch].cb_event) {
						channel_info[1][ch].cb_event(DMA_EVENT_TC);
					}
				}
			}
			// DMA error interrupt
			else if (GTRY_DMA1->DMA_INT_STS & (1U << ch))
			{
				dma_ch->CHL_CTRL = 0U;
				// Clear Channel active flag
				clear_channel_active_flag (1, ch);

				// Clear interrupt flag
				GTRY_DMA1->DMA_INT_STS = (1U << ch);

				// Signal Event
				if (channel_info[1][ch].cb_event) {
					channel_info[1][ch].cb_event(DMA_EVENT_ERROR);
				}

			}
			// DMA abort interrupt
			else if (GTRY_DMA1->DMA_INT_STS & (1U << (8 + ch)))
			{
				dma_ch->CHL_CTRL = 0U;
				// Clear Channel active flag
				clear_channel_active_flag (1, ch);

				// Clear interrupt flag
				GTRY_DMA1->DMA_INT_STS = (1U << (8 + ch));

				// Signal Event
				if (channel_info[1][ch].cb_event) {
					channel_info[1][ch].cb_event(DMA_EVENT_ABORT);
				}
			}
		}
	}
}

