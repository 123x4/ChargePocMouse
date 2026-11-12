/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

#ifndef __GTRY_DMA_DRV_H__
#define __GTRY_DMA_DRV_H__

#include <stdint.h>
#include "gtry_platform.h"

// Number of DMA module
typedef enum{
	DMA0_QSPI = 0,
	DMA1_PERIPH = 1,
	DMA_NUMBER_MAX
}dma_number_types;

// Number of DMA channels
#define DMA_NUM_OF_CHLS           		 (8)

// GPDMA Events
#define DMA_EVENT_TC					 (1)
#define DMA_EVENT_ERROR                  (2)
#define DMA_EVENT_ABORT                  (3)

// Source burst size in Source and Destination definitions
#define DMA_BSIZE_1                      (0)    // Burst size = 1
#define DMA_BSIZE_2                      (1)    // Burst size = 2
#define DMA_BSIZE_4                      (2)    // Burst size = 4
#define DMA_BSIZE_8                      (3)    // Burst size = 8
#define DMA_BSIZE_16                     (4)    // Burst size = 16
#define DMA_BSIZE_32                     (5)    // Burst size = 32
#define DMA_BSIZE_64                     (6)    // Burst size = 64
#define DMA_BSIZE_128                    (7)    // Burst size = 128
#define DMA_BSIZE_256                    (8)    // Burst size = 256
#define DMA_BSIZE_512                    (9)    // Burst size = 512
#define DMA_BSIZE_1024                   (0xa)  // Burst size = 1024

// Width in Source transfer width and Destination transfer width definitions
#define DMA_WIDTH_BYTE                   (0)    // Width = 1 byte
#define DMA_WIDTH_HALFWORD               (1)    // Width = 2 bytes
#define DMA_WIDTH_WORD                   (2)    // Width = 4 bytes
#define DMA_WIDTH_DWORD                  (3)    // Width = 8 bytes
#define DMA_WIDTH_QWORD                  (4)    // Width = 16 bytes
#define DMA_WIDTH_EWORD                  (5)    // Width = 32 bytes

// Bus interface index
#define DMA_INF_IDX0                     (0)    // interface = 0
#define DMA_INF_IDX1                     (1)    // interface = 1

// Interrupt Status Register index
#define DMA_INT_STS_TC(ch)               (0x10000U << (ch))    // TC interrupt status
#define DMA_INT_STS_ABT(ch)              (0x100U << (ch))    // ABORT interrupt status
#define DMA_INT_STS_ERR(ch)              (0x1U << (ch))    // ERROR interrupt status

// DMA Channel Control register definition
#define DMA_CH_CTRL_SBINFIDX_POS         (        31)
#define DMA_CH_CTRL_SBINF(n)             ((n)  << DMA_CH_CTRL_SBINFIDX_POS) //SrcBusINFIDX
#define DMA_CH_CTRL_SBINF_MASK           (   1 << DMA_CH_CTRL_SBINFIDX_POS) //SrcBusINFIDX
#define DMA_CH_CTRL_DBINFIDX_POS         (        30)
#define DMA_CH_CTRL_DBINF(n)             ((n)  << DMA_CH_CTRL_DBINFIDX_POS) //DstBusINFIDX
#define DMA_CH_CTRL_DBINF_MASK           (   1 << DMA_CH_CTRL_DBINFIDX_POS) //SrcBusINFIDX
#define DMA_CH_CTRL_PRIORITY_HIGH        (   1 << 29)
#define DMA_CH_CTRL_SBSIZE_POS           (        24)
#define DMA_CH_CTRL_SBSIZE_MASK          (0x0f << DMA_CH_CTRL_SBSIZE_POS)
#define DMA_CH_CTRL_SBSIZE(n)            (((n) << DMA_CH_CTRL_SBSIZE_POS) & DMA_CH_CTRL_SBSIZE_MASK)
#define DMA_CH_CTRL_SWIDTH_POS           (        21)
#define DMA_CH_CTRL_SWIDTH_MASK          (0x07 << DMA_CH_CTRL_SWIDTH_POS)
#define DMA_CH_CTRL_SWIDTH(n)            (((n) << DMA_CH_CTRL_SWIDTH_POS) & DMA_CH_CTRL_SWIDTH_MASK)
#define DMA_CH_CTRL_DWIDTH_POS           (        18)
#define DMA_CH_CTRL_DWIDTH_MASK          (0x07 << DMA_CH_CTRL_DWIDTH_POS)
#define DMA_CH_CTRL_DWIDTH(n)            (((n) << DMA_CH_CTRL_DWIDTH_POS) & DMA_CH_CTRL_DWIDTH_MASK)
#define DMA_CH_CTRL_SMODE_HANDSHAKE      (   1 << 17)
#define DMA_CH_CTRL_DMODE_HANDSHAKE      (   1 << 16)
#define DMA_CH_CTRL_SRCADDRCTRL_POS      (        14)
#define DMA_CH_CTRL_SRCADDRCTRL_MASK     (0x03 << DMA_CH_CTRL_SRCADDRCTRL_POS)
#define DMA_CH_CTRL_SRCADDR_INC          (   0 << DMA_CH_CTRL_SRCADDRCTRL_POS)
#define DMA_CH_CTRL_SRCADDR_DEC          (   1 << DMA_CH_CTRL_SRCADDRCTRL_POS)
#define DMA_CH_CTRL_SRCADDR_FIX          (   2 << DMA_CH_CTRL_SRCADDRCTRL_POS)
#define DMA_CH_CTRL_DSTADDRCTRL_POS      (        12)
#define DMA_CH_CTRL_DSTADDRCTRL_MASK     (0x03 << DMA_CH_CTRL_DSTADDRCTRL_POS)
#define DMA_CH_CTRL_DSTADDR_INC          (   0 << DMA_CH_CTRL_DSTADDRCTRL_POS)
#define DMA_CH_CTRL_DSTADDR_DEC          (   1 << DMA_CH_CTRL_DSTADDRCTRL_POS)
#define DMA_CH_CTRL_DSTADDR_FIX          (   2 << DMA_CH_CTRL_DSTADDRCTRL_POS)
#define DMA_CH_CTRL_SRCREQ_POS           (        8)
#define DMA_CH_CTRL_SRCREQ_MASK          (0x0F << DMA_CH_CTRL_SRCREQ_POS)
#define DMA_CH_CTRL_SRCREQ(n)            (((n) << DMA_CH_CTRL_SRCREQ_POS) & DMA_CH_CTRL_SRCREQ_MASK)
#define DMA_CH_CTRL_DSTREQ_POS           (        4)
#define DMA_CH_CTRL_DSTREQ_MASK          (0x0F << DMA_CH_CTRL_DSTREQ_POS)
#define DMA_CH_CTRL_DSTREQ(n)            (((n) << DMA_CH_CTRL_DSTREQ_POS) & DMA_CH_CTRL_DSTREQ_MASK)
//#define DMA_CH_CTRL_INTABT               (   0 << 3)
//#define DMA_CH_CTRL_INTERR               (   0 << 2)
//#define DMA_CH_CTRL_INTTC                (   1 << 1)//0->1 Disable the terminal count interrupt
#define DMA_CH_CTRL_EN_INTABT             	(   0 << 3)//0 Enable the terminal abort interrupt
#define DMA_CH_CTRL_EN_INTERR             	(   0 << 2)//0 Enable the terminal error interrupt
#define DMA_CH_CTRL_EN_INTTC              	(   0 << 1)//0 Enable the terminal count interrupt
#define DMA_CH_CTRL_DIS_INTABT            	(   1 << 3)//1 Disable the terminal abort interrupt
#define DMA_CH_CTRL_DIS_INTERR            	(   1 << 2)//1 Disable the terminal error interrupt
#define DMA_CH_CTRL_DIS_INTTC				(   1 << 1)//1 Disable the terminal count interrupt
#define DMA_CH_CTRL_ENABLE					(   1 << 0)


extern uint32_t dma_channel_get_intTC (uint32_t dma_no, uint8_t ch);
extern uint32_t dma_channel_get_intABT (uint32_t dma_no, uint8_t ch);
extern uint32_t dma_channel_get_intERR (uint32_t dma_no, uint8_t ch);
extern void dma_channel_clear_intsts (uint32_t dma_no, uint8_t ch);

/**
  \fn          void p_dma_int_cb (uint32_t event)
  \brief       DMA Events.
  \param[in]   event  DMA Event mask
  \return      none
*/
typedef void (*p_dma_int_cb) (uint32_t event);

/**
  \fn          int32_t dma_initialize (void)
  \brief       Initialize DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t dma_initialize (uint32_t dma_no);

/**
  \fn          int32_t dma_uninitialize (void)
  \brief       De-initialize DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t dma_uninitialize (uint32_t dma_no);

/**
  \fn          int32_t dma_channel_configure (uint8_t            ch,
                                              unsigned long      src_addr,
                                              unsigned long      dst_addr,
                                              uint32_t           size,
                                              uint32_t           control,
                                              uint32_t           config,
                                              p_dma_int_cb  cb_event)
  \brief       Configure DMA channel for next transfer
  \param[in]   ch        Channel number (0..7)
  \param[in]   src_addr  Source address
  \param[in]   dest_addr Destination address
  \param[in]   size      Amount of data to transfer
  \param[in]   control   Channel control
  \param[in]   config    Channel configuration
  \param[in]   cb_event  Channel callback pointer
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t dma_channel_configure (uint32_t dma_no, uint8_t ch,
										unsigned long src_addr,
										unsigned long dst_addr,
										uint32_t size,
										uint32_t control,
										p_dma_int_cb cb_event);
extern int32_t dma_channel_reconfig (uint32_t dma_no, uint8_t ch, unsigned long src_addr, unsigned long dst_addr, uint32_t size);
extern uint32_t dma_spichl_rx_reconfig (uint32_t spi_chl, unsigned long dst_addr, uint32_t size);
//extern uint32_t dma_i2schl_tx_reconfig (unsigned long src_addr, uint32_t size);
//extern uint32_t dma_i2schl_rx_reconfig (unsigned long dst_addr, uint32_t size);

/**
  \fn          int32_t dma_channel_enable (uint8_t ch)
  \brief       Enable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/

extern int32_t dma_channel_enable (uint32_t dma_no, uint8_t ch);

/**
  \fn          int32_t dma_channel_disable (uint8_t ch)
  \brief       Disable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t dma_channel_disable (uint32_t dma_no, uint8_t ch);

/**
  \fn          uint32_t get_channel_active_flag (uint8_t ch)
  \brief       Check if DMA channel is enabled or disabled
  \param[in]   ch Channel number (0..7)
  \returns     Channel status
   - \b  1: channel enabled
   - \b  0: channel disabled
*/
extern uint32_t get_channel_active_flag (uint32_t dma_no, uint8_t ch);

extern uint32_t dma_channel_get_txaddr (uint32_t dma_no, uint8_t ch);
extern uint32_t dma_channel_get_rxaddr (uint32_t dma_no, uint8_t ch);

/**
  \fn          uint32_t dma_channel_get_count (uint8_t ch)
  \brief       Get number of transferred data
  \param[in]   ch Channel number (0..7)
  \returns     Number of transferred data
*/
extern uint32_t dma_channel_get_count (uint32_t dma_no, uint8_t ch);

/**
  \fn          uint32_t dma_channel_abort (uint8_t ch)
  \brief       Abort ch transfer
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t dma_channel_abort (uint32_t dma_no, uint8_t ch);

#endif /* __GTRY_DMA_DRV_H__ */
