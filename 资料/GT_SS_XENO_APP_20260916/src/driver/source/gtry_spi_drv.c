/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

#include "gtry_spi_drv.h"
#include "gtry_gpio_drv.h"

// SPI transfer operation
#define SPI_SEND                      0x0
#define SPI_RECEIVE                   0x1
#define SPI_TRANSFER                  0x2

unsigned int spi_io_multi_config(RYRV_SPI_NO spi_no, spi_multi_io spi_gp, uint32 qspi_enable)
{
	uint32_t offset_no = 0x0;
	uint32_t io_num = qspi_enable? 6:4;

	if(spi_no >= SPI_NUM_MAXS || spi_gp >= SPI_IO_GP_MAX)
	{
		return 1;
	}

	if(spi_gp == SPI_IO_GP1)
	{
		if(spi_no == SPI_NO3)
			return 2;
		else if(spi_no == SPI_NO0 && qspi_enable)
			return 3;
	}

	switch(spi_no)
	{
		case SPI_NO0:
			offset_no = 0 + (spi_gp*44);
			break;
		case SPI_NO1:
			offset_no = 6 + (spi_gp*42);
			break;
		case SPI_NO2:
			offset_no = 12 + (spi_gp*42);
			break;
		case SPI_NO3:
			offset_no = 18 + (spi_gp*42);
			break;
		case SPI_NO4:
			offset_no = 24;
			break;
		case SPI_NO5:
			offset_no = 32;
			break;
		case SPI_NO6:
			offset_no = 38;
			break;
		default:
			return 4;
	}

	if((offset_no+io_num)>=64)
	{
		return 5;
	}

	for(uint32_t i = 0; i < io_num; i++)
	{
		GTRY_GPIO->GPIO_CFG[offset_no+i] &= ~(RY_GPIO_DRIVE7 << GPIO_CFG_DRIVE_POS);
		GTRY_GPIO->GPIO_CFG[offset_no+i] &= ~(RY_MAX_REUSE << GPIO_CFG_REUSE_POS);
		GTRY_GPIO->GPIO_CFG[offset_no+i] |= ((RY_SPI_REUSE << GPIO_CFG_REUSE_POS)|(RY_GPIO_INPUT_EN << GPIO_CFG_IN_EN_POS));
	}
	return 0;
}


#if ((!GTRY_SPI0_EN) && (!GTRY_SPI1_EN) && (!GTRY_SPI2_EN) && (!GTRY_SPI3_EN) && (!GTRY_SPI4_EN) && (!GTRY_SPI5_EN) && (!GTRY_SPI6_EN))
	#error "SPI not enabled in gtry_platform.h!"
#endif

// SPI0
#if (GTRY_SPI0_EN)
static SPI_INFO spi0_info = {0};

#if (GTRY_SPI0_DMA_TX_EN == 1)
void spi0_dma_tx_event(uint32_t event);
static SPI_DMA spi0_dma_tx = {GTRY_SPI0_DMA_TX_CH, GTRY_SPI0_DMA_TX_REQID, spi0_dma_tx_event};
#endif

#if (GTRY_SPI0_DMA_RX_EN == 1)
void spi0_dma_rx_event(uint32_t event);
static SPI_DMA spi0_dma_rx = {GTRY_SPI0_DMA_RX_CH, GTRY_SPI0_DMA_RX_REQID, spi0_dma_rx_event};
#endif

static const SPI_RESOURCES spi0_resources = {
	{0},
	GTRY_SPI0,
	IRQ_SPI0_SOURCE,
#if (GTRY_SPI0_DMA_TX_EN == 1)
	&spi0_dma_tx,
#else
	NULL,
#endif
#if (GTRY_SPI0_DMA_RX_EN == 1)
	&spi0_dma_rx,
#else
	NULL,
#endif
	&spi0_info
};
#endif

// SPI1
#if (GTRY_SPI1_EN)
static SPI_INFO spi1_info = {0};

#if (GTRY_SPI1_DMA_TX_EN == 1)
void spi1_dma_tx_event(uint32_t event);
static SPI_DMA spi1_dma_tx = {GTRY_SPI1_DMA_TX_CH, GTRY_SPI1_DMA_TX_REQID, spi1_dma_tx_event};
#endif

#if (GTRY_SPI1_DMA_RX_EN == 1)
void spi1_dma_rx_event(uint32_t event);
static SPI_DMA spi1_dma_rx = {GTRY_SPI1_DMA_RX_CH, GTRY_SPI1_DMA_RX_REQID, spi1_dma_rx_event};
#endif

static const SPI_RESOURCES spi1_resources = {
	{0},
	GTRY_SPI1,
	IRQ_SPI1_SOURCE,
#if (GTRY_SPI1_DMA_TX_EN == 1)
	&spi1_dma_tx,
#else
	NULL,
#endif
#if (GTRY_SPI1_DMA_RX_EN == 1)
	&spi1_dma_rx,
#else
	NULL,
#endif
	&spi1_info
};
#endif

// SPI2
#if (GTRY_SPI2_EN)
static SPI_INFO spi2_info = {0};

#if (GTRY_SPI2_DMA_TX_EN == 1)
void spi2_dma_tx_event(uint32_t event);
static SPI_DMA spi2_dma_tx = {GTRY_SPI2_DMA_TX_CH, GTRY_SPI2_DMA_TX_REQID, spi2_dma_tx_event};
#endif

#if (GTRY_SPI2_DMA_RX_EN == 1)
void spi2_dma_rx_event(uint32_t event);
static SPI_DMA spi2_dma_rx = {GTRY_SPI2_DMA_RX_CH, GTRY_SPI2_DMA_RX_REQID, spi2_dma_rx_event};
#endif

static const SPI_RESOURCES spi2_resources = {
	{0},
	GTRY_SPI2,
	IRQ_SPI2_SOURCE,
#if (GTRY_SPI2_DMA_TX_EN == 1)
	&spi2_dma_tx,
#else
	NULL,
#endif
#if (GTRY_SPI2_DMA_RX_EN == 1)
	&spi2_dma_rx,
#else
	NULL,
#endif
	&spi2_info
};
#endif

// SPI3
#if (GTRY_SPI3_EN)
static SPI_INFO spi3_info = {0};

#if (GTRY_SPI3_DMA_TX_EN == 1)
void spi3_dma_tx_event(uint32_t event);
static SPI_DMA spi3_dma_tx = {GTRY_SPI3_DMA_TX_CH, GTRY_SPI3_DMA_TX_REQID, spi3_dma_tx_event};
#endif

#if (GTRY_SPI3_DMA_RX_EN == 1)
void spi3_dma_rx_event(uint32_t event);
static SPI_DMA spi3_dma_rx = {GTRY_SPI3_DMA_RX_CH, GTRY_SPI3_DMA_RX_REQID, spi3_dma_rx_event};
#endif

static const SPI_RESOURCES spi3_resources = {
	{0},
	GTRY_SPI3,
	IRQ_SPI3_SOURCE,
#if (GTRY_SPI3_DMA_TX_EN == 1)
	&spi3_dma_tx,
#else
	NULL,
#endif
#if (GTRY_SPI3_DMA_RX_EN == 1)
	&spi3_dma_rx,
#else
	NULL,
#endif
	&spi3_info
};
#endif

// SPI4
#if (GTRY_SPI4_EN)
static SPI_INFO spi4_info = {0};

#if (GTRY_SPI4_DMA_TX_EN == 1)
void spi4_dma_tx_event(uint32_t event);
static SPI_DMA spi4_dma_tx = {GTRY_SPI4_DMA_TX_CH, GTRY_SPI4_DMA_TX_REQID, spi4_dma_tx_event};
#endif

#if (GTRY_SPI4_DMA_RX_EN == 1)
void spi4_dma_rx_event(uint32_t event);
static SPI_DMA spi4_dma_rx = {GTRY_SPI4_DMA_RX_CH, GTRY_SPI4_DMA_RX_REQID, spi4_dma_rx_event};
#endif

static const SPI_RESOURCES spi4_resources = {
	{0},
	GTRY_SPI4,
	IRQ_SPI4_SOURCE,
#if (GTRY_SPI4_DMA_TX_EN == 1)
	&spi4_dma_tx,
#else
	NULL,
#endif
#if (GTRY_SPI4_DMA_RX_EN == 1)
	&spi4_dma_rx,
#else
	NULL,
#endif
	&spi4_info
};
#endif

// SPI5
#if (GTRY_SPI5_EN)
static SPI_INFO spi5_info = {0};

#if (GTRY_SPI5_DMA_TX_EN == 1)
void spi5_dma_tx_event(uint32_t event);
static SPI_DMA spi5_dma_tx = {GTRY_SPI5_DMA_TX_CH, GTRY_SPI5_DMA_TX_REQID, spi5_dma_tx_event};
#endif

#if (GTRY_SPI5_DMA_RX_EN == 1)
void spi5_dma_rx_event(uint32_t event);
static SPI_DMA spi5_dma_rx = {GTRY_SPI5_DMA_RX_CH, GTRY_SPI5_DMA_RX_REQID, spi5_dma_rx_event};
#endif

static const SPI_RESOURCES spi5_resources = {
	{0},
	GTRY_SPI5,
	IRQ_SPI5_SOURCE,
#if (GTRY_SPI5_DMA_TX_EN == 1)
	&spi5_dma_tx,
#else
	NULL,
#endif
#if (GTRY_SPI5_DMA_RX_EN == 1)
	&spi5_dma_rx,
#else
	NULL,
#endif
	&spi5_info
};
#endif

// SPI6
#if (GTRY_SPI6_EN)
static SPI_INFO spi6_info = {0};

#if (GTRY_SPI6_DMA_TX_EN == 1)
void spi6_dma_tx_event(uint32_t event);
static SPI_DMA spi6_dma_tx = {GTRY_SPI6_DMA_TX_CH, GTRY_SPI6_DMA_TX_REQID, spi6_dma_tx_event};
#endif

#if (GTRY_SPI6_DMA_RX_EN == 1)
void spi6_dma_rx_event(uint32_t event);
static SPI_DMA spi6_dma_rx = {GTRY_SPI6_DMA_RX_CH, GTRY_SPI6_DMA_RX_REQID, spi6_dma_rx_event};
#endif

static const SPI_RESOURCES spi6_resources = {
	{0},
	GTRY_SPI6,
	IRQ_SPI6_SOURCE,
#if (GTRY_SPI6_DMA_TX_EN == 1)
	&spi6_dma_tx,
#else
	NULL,
#endif
#if (GTRY_SPI6_DMA_RX_EN == 1)
	&spi6_dma_rx,
#else
	NULL,
#endif
	&spi6_info
};
#endif

static void spi_polling_spiactive(SPI_RESOURCES *spi)
{
	while((spi->reg->SPI_STATUS) & 0x1);
}

static RY_SPI_CAPABILITIES spi_get_capabilities(SPI_RESOURCES *spi)
{
	return spi->capabilities;
}

static int32_t spi_initialize(p_spi_int_cb cb_event, SPI_RESOURCES *spi)
{
	if (spi->info->flags & SPI_FLAG_INITIALIZED)
	{
		return RY_DRIVER_OK;
	}

	// initialize SPI run-time resources
	spi->info->cb_event          = cb_event;

	spi->info->status.busy       = 0U;
	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->xfer.rx_buf       = 0U;
	spi->info->xfer.tx_buf       = 0U;
	spi->info->xfer.tx_buf_limit = 0U;
	spi->info->xfer.rx_cnt       = 0U;
	spi->info->xfer.tx_cnt       = 0U;

	spi->info->mode              = 0U;
	spi->info->record_rx_buf     = 0U;
	spi->info->is_header         = 0U;
	spi->info->txfifo_size = 4;//words

	spi->info->flags = SPI_FLAG_INITIALIZED;  // SPI is initialized

	spi->info->xfer.dma_tx_complete = 0;
	spi->info->xfer.dma_rx_complete = 0;

	return RY_DRIVER_OK;
}

static int32_t spi_uninitialize(SPI_RESOURCES *spi)
{
	spi->info->flags = 0U;                    // SPI is uninitialized
	return RY_DRIVER_OK;
}

static int32_t spi_power_control(RY_POWER_STATE state, SPI_RESOURCES *spi)
{
	switch(state)
	{
		case RY_POWER_OFF:
			// Disable PLIC interrupt SPI0 source
			__nds__plic_disable_interrupt(spi->irq_num);

			// DMA uninitialize
			if (spi->dma_tx || spi->dma_rx)
			{
				dma_uninitialize(DMA0_QSPI);
				if (spi->dma_tx && (spi->info->status.busy != 0))
				{
					dma_channel_disable(DMA0_QSPI, spi->dma_tx->channel);
				}
				if (spi->dma_rx && (spi->info->status.busy != 0))
				{
					dma_channel_disable(DMA0_QSPI, spi->dma_rx->channel);
				}
			}

			// reset SPI and TX/RX FIFOs
			spi->reg->SPI_CTRL = (TXFIFORST | RXFIFORST | SPIRST);

			// clear SPI run-time resources
			spi->info->status.busy       = 0U;
			spi->info->status.data_lost  = 0U;
			spi->info->status.mode_fault = 0U;

			spi->info->xfer.rx_buf       = 0U;
			spi->info->xfer.tx_buf       = 0U;
			spi->info->xfer.rx_cnt       = 0U;
			spi->info->xfer.tx_cnt       = 0U;

			spi->info->mode              = 0U;

			spi->info->flags &= ~SPI_FLAG_POWERED;

			break;

		case RY_POWER_LOW:
			return RY_DRIVER_ERROR_UNSUPPORTED;

		case RY_POWER_FULL:
			if ((spi->info->flags & SPI_FLAG_INITIALIZED) == 0U)
			{
				return RY_DRIVER_ERROR;
			}
			if ((spi->info->flags & SPI_FLAG_POWERED) != 0U)
			{
				return RY_DRIVER_OK;
			}

			// DMA initialize
			if (spi->dma_tx || spi->dma_rx)
			{
				dma_initialize(DMA0_QSPI);
			}

			// reset SPI and TX/RX FIFOs
			spi->reg->SPI_CTRL = (TXFIFORST | RXFIFORST | SPIRST);

			// clear SPI run-time resources
			spi->info->status.busy       = 0U;
			spi->info->status.data_lost  = 0U;
			spi->info->status.mode_fault = 0U;

			spi->info->xfer.rx_buf       = 0U;
			spi->info->xfer.tx_buf       = 0U;
			spi->info->xfer.rx_cnt       = 0U;
			spi->info->xfer.tx_cnt       = 0U;

			spi->info->mode              = 0U;

			// Priority must be set > 0 to trigger the interrupt
			__nds__plic_set_priority(spi->irq_num, 1);

			// Enable PLIC interrupt SPI0 source
			__nds__plic_enable_interrupt(spi->irq_num);

			spi->info->flags |= SPI_FLAG_POWERED;   // SPI is powered

			break;

		default:
			return RY_DRIVER_ERROR_UNSUPPORTED;
	}
	return RY_DRIVER_OK;
}

static int32_t spi_send(const void *data, uint32_t num, SPI_RESOURCES *spi)
{
	int32_t stat;

	if ((data == NULL) || (num == 0U))
	{
		return RY_DRIVER_ERROR_PARAMETER;
	}
	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
	{
		return RY_DRIVER_ERROR;
	}
	if (spi->info->status.busy)
	{
		return RY_DRIVER_ERROR_BUSY;
	}

	// set busy flag
	spi->info->status.busy = 1U;

	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->xfer.tx_buf = (uint8_t *)data;
	spi->info->xfer.tx_cnt = 0U;

	spi->info->xfer.transfer_op = SPI_SEND;

	// wait prior transfer finish
	spi_polling_spiactive(spi);

	if ((spi->info->mode & RY_SPI_TRANSFER_FORMAT_Msk) != RY_SPI_SLV_DATA_ONLY_TRANSFER)
	{
		// set transfer mode to write only and transfer count for write data
		spi->reg->SPI_TRANS_CTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->SPI_TRANS_CTRL |= SPI_TRANSMODE_WRONLY;
	}
	else
	{
		// SlvDataOnly mode should set TransMode to 0
		spi->reg->SPI_TRANS_CTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->SPI_TRANS_CTRL |= SPI_TRANSCTRL_SLV_DATA_ONLY;
	}

	if(num <= (RD_TRANCNT_MSK+1))
	{
		spi->reg->SPI_TRANS_TXCNT = 0U;
		spi->reg->SPI_TRANS_CTRL |= WR_TRANCNT(num);
	}
	else
	{
		spi->reg->SPI_TRANS_TXCNT = (num <= (SPI_TRANS_MAX_CNT_MASK+1))?(num-1):(SPI_TRANS_MAX_CNT_MASK);
	}

	// set TX FIFO threshold to 2
	spi->reg->SPI_CTRL = TXTHRES(2);

	// prepare info that ISR needed
	spi->info->xfer.txfifo_refill = spi->info->txfifo_size - 2; // TX FIFO threshold = 2

	if(spi->info->data_bits <= 8)  		// data bits = 1....8
	{
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num));
		spi->info->src_width = 1;
	}
	else if(spi->info->data_bits <= 16) 	// data bits = 9....16
	{
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num * 2));
		spi->info->src_width = 2;
	}
	else  									// data bits = 17....32
	{
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num * 4));
		spi->info->src_width = 4;
	}

	if (spi->dma_tx)	// DMA mode
	{
		// enable TX DMA
		spi->reg->SPI_CTRL |= TXDMAEN;

		// configure DMA channel
		stat = dma_channel_configure(DMA0_QSPI, spi->dma_tx->channel,
					     (unsigned long)data|0xa0000000,
					     (unsigned long)(&(spi->reg->SPI_DATA)),
					     num,
					     DMA_CH_CTRL_SBINF(DMA_INF_IDX1)            |
					     DMA_CH_CTRL_DBINF(DMA_INF_IDX1)            |
					     DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)            |
					     //DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
					     DMA_CH_CTRL_SWIDTH(spi->info->src_width/2) |
					     DMA_CH_CTRL_DWIDTH(spi->info->src_width/2) |
					     DMA_CH_CTRL_DMODE_HANDSHAKE                |
					     DMA_CH_CTRL_SRCADDR_INC                    |
					     DMA_CH_CTRL_DSTADDR_FIX                    |
					     DMA_CH_CTRL_DSTREQ(spi->dma_tx->reqsel)    |
					     DMA_CH_CTRL_EN_INTABT                         |
					     DMA_CH_CTRL_EN_INTERR                         |
					     DMA_CH_CTRL_EN_INTTC                          |
					     DMA_CH_CTRL_ENABLE,
					     spi->dma_tx->cb_event);
		if (stat == -1)
		{
			return RY_DRIVER_ERROR;
		}

		// enable interrupts
		spi->reg->SPI_INT_EN = SPI_ENDINT;

	}
	else  	// interrupt mode
	{
		// enable interrupts
		spi->reg->SPI_INT_EN = (SPI_TXFIFOINT | SPI_ENDINT);

		// enable TX FIFO underrun interrupt when slave mode
		if ((spi->info->mode & RY_SPI_CONTROL_Msk) == RY_SPI_MODE_SLAVE)
		{
			spi->reg->SPI_INT_EN |= SPI_TXFIFOOURINT;
		}
	}

	// trigger transfer when SPI master mode
	if ((spi->info->mode & RY_SPI_CONTROL_Msk) == RY_SPI_MODE_MASTER)
	{
		spi->reg->SPI_CMD = 0U;
	}
	return RY_DRIVER_OK;
}

static int32_t spi_receive(void *data, uint32_t num, SPI_RESOURCES *spi)
{
	int32_t stat;

	if ((data == NULL) || (num == 0U))
	{
		return RY_DRIVER_ERROR_PARAMETER;
	}
	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
	{
		return RY_DRIVER_ERROR;
	}
	if (spi->info->status.busy)
	{
		return RY_DRIVER_ERROR_BUSY;
	}

	// set busy flag
	spi->info->status.busy = 1U;

	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->xfer.rx_buf = (uint8_t *)data;
	spi->info->xfer.rx_cnt = 0U;

	spi->info->xfer.transfer_op = SPI_RECEIVE;

	// wait prior transfer finish
	spi_polling_spiactive(spi);

	if ((spi->info->mode & RY_SPI_TRANSFER_FORMAT_Msk) != RY_SPI_SLV_DATA_ONLY_TRANSFER)
	{
		// set transfer mode to read only and transfer count for read data
		spi->reg->SPI_TRANS_CTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->SPI_TRANS_CTRL |= SPI_TRANSMODE_RDONLY;
	}
	else
	{
		// SlvDataOnly mode should set TransMode to 0
		spi->reg->SPI_TRANS_CTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->SPI_TRANS_CTRL |= SPI_TRANSCTRL_SLV_DATA_ONLY;
	}

	if(num <= (RD_TRANCNT_MSK+1))
	{
		spi->reg->SPI_TRANS_RXCNT = 0U;
		spi->reg->SPI_TRANS_CTRL |= RD_TRANCNT(num);
	}
	else
	{
		spi->reg->SPI_TRANS_RXCNT = (num <= (SPI_TRANS_MAX_CNT_MASK+1))?(num-1):SPI_TRANS_MAX_CNT_MASK;
	}

	// set RX FIFO threshold to 2
	spi->reg->SPI_CTRL = RXTHRES(1);

	if(spi->info->data_bits <= 8) 			// data bits = 1....8
	{
		spi->info->src_width = 1;
	}
	else if(spi->info->data_bits <= 16)  	// data bits = 9....16
	{
		spi->info->src_width = 2;
	}
	else	 								// data bits = 17....32
	{
		spi->info->src_width = 4;
	}


	if (spi->dma_rx) 	// DMA mode
	{
		// enable RX DMA
		spi->reg->SPI_CTRL |= RXDMAEN;

		// configure DMA channel
		stat = dma_channel_configure(DMA0_QSPI, spi->dma_rx->channel,
					     (unsigned long)(&(spi->reg->SPI_DATA)),
					     (unsigned long)data|0xa0000000,
					     num,
					     DMA_CH_CTRL_SBINF(DMA_INF_IDX0)            |
					     DMA_CH_CTRL_DBINF(DMA_INF_IDX0)            |
					     DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)            |
					     //DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
					     DMA_CH_CTRL_SWIDTH(spi->info->src_width/2) |
					     DMA_CH_CTRL_DWIDTH(spi->info->src_width/2) |
					     DMA_CH_CTRL_SMODE_HANDSHAKE                |
					     DMA_CH_CTRL_SRCADDR_FIX                    |
					     DMA_CH_CTRL_DSTADDR_INC                    |
					     DMA_CH_CTRL_SRCREQ(spi->dma_rx->reqsel)    |
					     DMA_CH_CTRL_EN_INTABT                         |
					     DMA_CH_CTRL_EN_INTERR                         |
					     DMA_CH_CTRL_EN_INTTC                          |
					     DMA_CH_CTRL_ENABLE,
					     spi->dma_rx->cb_event);

		if (stat == -1)
		{
			return RY_DRIVER_ERROR;
		}

		// enable interrupts
		spi->reg->SPI_INT_EN = SPI_ENDINT;

	}
	else  	// interrupt mode
	{
		// enable interrupts
		spi->reg->SPI_INT_EN = (SPI_RXFIFOINT | SPI_ENDINT);

		// enable RX FIFO overrun interrupt when slave mode
		if ((spi->info->mode & RY_SPI_CONTROL_Msk) == RY_SPI_MODE_SLAVE)
		{
			spi->reg->SPI_INT_EN |= SPI_RXFIFOOORINT;
		}
	}

	// trigger transfer when SPI master mode
	if ((spi->info->mode & RY_SPI_CONTROL_Msk) == RY_SPI_MODE_MASTER)
	{
		spi->reg->SPI_CMD = 0U;
	}

	return RY_DRIVER_OK;
}

static int32_t spi_transfer(const void *data_out, void *data_in, uint32_t num, SPI_RESOURCES *spi)
{
	uint32_t dma_rx_num = 0;
	int32_t stat;

	if ((data_out == NULL) || (data_in == NULL) || (num == 0U))
	{
		return RY_DRIVER_ERROR_PARAMETER;
	}
	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
	{
		return RY_DRIVER_ERROR;
	}
	if (spi->info->status.busy)
	{
		return RY_DRIVER_ERROR_BUSY;
	}
	if (spi->info->tx_header_len > num)
	{
		return RY_SPI_ERROR_HEADER_LEN;
	}

	// set busy flag
	spi->info->status.busy       = 1U;

	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->xfer.rx_buf = (uint8_t *)data_in;
	spi->info->xfer.tx_buf = (uint8_t *)data_out;
	spi->info->xfer.rx_cnt = 0U;
	spi->info->xfer.tx_cnt = 0U;

	spi->info->xfer.transfer_op = SPI_TRANSFER;

	// wait prior transfer finish
	spi_polling_spiactive(spi);

	if ((spi->info->mode & RY_SPI_TRANSFER_FORMAT_Msk) != RY_SPI_SLV_DATA_ONLY_TRANSFER)
	{
		// set transfer mode to write and read at the same time and transfer count for write/read data
		spi->reg->SPI_TRANS_CTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->SPI_TRANS_CTRL |= SPI_TRANSMODE_WRnRD;
	}
	else
	{
		// SlvDataOnly mode should set TransMode to 0
		spi->reg->SPI_TRANS_CTRL &= ~(SPI_TRANSMODE_MSK | RD_TRANCNT_MSK | WR_TRANCNT_MSK);
		spi->reg->SPI_TRANS_CTRL |= SPI_TRANSCTRL_SLV_DATA_ONLY;
	}

	if(num <= (RD_TRANCNT_MSK+1))
	{
		spi->reg->SPI_TRANS_TXCNT = 0U;
		spi->reg->SPI_TRANS_RXCNT = 0U;
		spi->reg->SPI_TRANS_CTRL |= (RD_TRANCNT(num) | WR_TRANCNT(num));
	}
	else
	{
		spi->reg->SPI_TRANS_TXCNT = (num <= (SPI_TRANS_MAX_CNT_MASK+1))?(num-1):SPI_TRANS_MAX_CNT_MASK;
		spi->reg->SPI_TRANS_RXCNT = (num <= (SPI_TRANS_MAX_CNT_MASK+1))?(num-1):SPI_TRANS_MAX_CNT_MASK;
	}

	if(spi->info->data_bits <= 8) 		 	// data bits = 1....8)
	{
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num));
		spi->info->src_width = 1;
	}
	else if(spi->info->data_bits <= 16)  	// data bits = 9....16
	{
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num * 2));
		spi->info->src_width = 2;
	}
	else									// data bits = 17....32
	{
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + num * 4));
		spi->info->src_width = 4;
	}

	if ((spi->info->tx_header_len > 0) && (spi->info->mode & RY_SPI_CONTROL_Msk))
	{
		// If there is tx header, only setting dma to receive the header at first time 
		dma_rx_num = spi->info->tx_header_len;
		spi->info->record_rx_buf = (uint8_t *)(spi->info->xfer.rx_buf);
		spi->info->is_header = 1;
		spi->info->data_num = num - spi->info->tx_header_len;
	}
	else
	{
		dma_rx_num = num;
	}

	spi->reg->SPI_CTRL &= 0x0;

	// set TX FIFO threshold and RX FIFO threshold to 2
	spi->reg->SPI_CTRL |= (TXTHRES(2) | RXTHRES(1));

	// prepare info that ISR needed
	spi->info->xfer.txfifo_refill = spi->info->txfifo_size - 2; // TX FIFO threshold = 2


	if (spi->dma_tx || spi->dma_rx)		// DMA mode
	{
		if(spi->dma_tx)
		{
			// enable TX DMA
			spi->reg->SPI_CTRL |= TXDMAEN;

			// configure DMA channel
			stat = dma_channel_configure(DMA0_QSPI, spi->dma_tx->channel,
						      (unsigned long)data_out|0xa0000000,
						      (unsigned long)(&(spi->reg->SPI_DATA)),
						      num,
						      DMA_CH_CTRL_SBINF(DMA_INF_IDX1)            |
						      DMA_CH_CTRL_DBINF(DMA_INF_IDX1)            |
						      DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)            |
						      //DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
						      DMA_CH_CTRL_SWIDTH(spi->info->src_width/2) |
						      DMA_CH_CTRL_DWIDTH(spi->info->src_width/2) |
						      DMA_CH_CTRL_DMODE_HANDSHAKE                |
						      DMA_CH_CTRL_SRCADDR_INC                    |
						      DMA_CH_CTRL_DSTADDR_FIX                    |
						      DMA_CH_CTRL_DSTREQ(spi->dma_tx->reqsel)    |
						      DMA_CH_CTRL_EN_INTABT                         |
						      DMA_CH_CTRL_EN_INTERR                         |
						      DMA_CH_CTRL_EN_INTTC                          |
						      DMA_CH_CTRL_ENABLE,
						      spi->dma_tx->cb_event);
			if (stat == -1)
			{
				return RY_DRIVER_ERROR;
			}
		}

		if(spi->dma_rx)
		{
			// enable RX DMA
			spi->reg->SPI_CTRL |= RXDMAEN;

			// configure DMA channel
			stat = dma_channel_configure(DMA0_QSPI, spi->dma_rx->channel,
						     (unsigned long)(&(spi->reg->SPI_DATA)),
						     (unsigned long)data_in|0xa0000000,
						     dma_rx_num,
						     DMA_CH_CTRL_SBINF(DMA_INF_IDX0)            |
						     DMA_CH_CTRL_DBINF(DMA_INF_IDX0)            |
						     DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)            |
						     //DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
						     DMA_CH_CTRL_SWIDTH(spi->info->src_width/2) |
						     DMA_CH_CTRL_DWIDTH(spi->info->src_width/2) |
						     DMA_CH_CTRL_SMODE_HANDSHAKE                |
						     DMA_CH_CTRL_SRCADDR_FIX                    |
						     DMA_CH_CTRL_DSTADDR_INC                    |
						     DMA_CH_CTRL_SRCREQ(spi->dma_rx->reqsel)    |
						     DMA_CH_CTRL_EN_INTABT                         |
						     DMA_CH_CTRL_EN_INTERR                         |
						     DMA_CH_CTRL_EN_INTTC                          |
						     DMA_CH_CTRL_ENABLE,
						     spi->dma_rx->cb_event);
			if (stat == -1)
			{
				return RY_DRIVER_ERROR;
			}
		}

		// enable interrupts
		spi->reg->SPI_INT_EN = SPI_ENDINT;
	}
	else 	// interrupt mode
	{

		// enable interrupts
		spi->reg->SPI_INT_EN = (SPI_TXFIFOINT | SPI_RXFIFOINT | SPI_ENDINT);

		// enable TX FIFO underrun and RX FIFO overrun interrupt when slave mode
		if ((spi->info->mode & RY_SPI_CONTROL_Msk) == RY_SPI_MODE_SLAVE)
			spi->reg->SPI_INT_EN |= (SPI_TXFIFOOURINT | SPI_RXFIFOOORINT);
	}

	// trigger transfer when SPI master mode
	if ((spi->info->mode & RY_SPI_CONTROL_Msk) == RY_SPI_MODE_MASTER)
	{
		spi->reg->SPI_CMD = 0;
	}

	return RY_DRIVER_OK;
}

static int32_t spi_get_data_count(SPI_RESOURCES *spi)
{
	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
	{
		return 0U;
	}

	switch (spi->info->xfer.transfer_op)
	{
		case SPI_SEND:
		case SPI_TRANSFER:
			if (spi->dma_tx)
			{
				return (dma_channel_get_count(DMA0_QSPI, spi->dma_tx->channel));
			}
			else
			{
				return (spi->info->xfer.tx_cnt);
			}
		case SPI_RECEIVE:
			if (spi->dma_rx)
			{
				return (dma_channel_get_count(DMA0_QSPI, spi->dma_rx->channel));
			}
			else
			{
				return (spi->info->xfer.rx_cnt);
			}
		default:
			return RY_DRIVER_OK;
	}
}

static int32_t spi_control(uint32_t control, uint32_t arg, SPI_RESOURCES *spi)
{
	uint32_t sclk_div;

	if (!(spi->info->flags & SPI_FLAG_POWERED))
		return RY_DRIVER_ERROR;

	if ((control & RY_SPI_CONTROL_Msk) == RY_SPI_ABORT_TRANSFER)   // abort SPI transfer
	{
		// disable SPI interrupts
		spi->reg->SPI_INT_EN = 0;

		// clear SPI run-time resources
		spi->info->status.busy = 0U;

		spi->info->xfer.rx_buf = 0U;
		spi->info->xfer.tx_buf = 0U;
		spi->info->xfer.rx_cnt = 0U;
		spi->info->xfer.tx_cnt = 0U;

		spi->info->mode        = 0U;

		return RY_DRIVER_OK;
	}

	if (spi->info->status.busy)
	{
		return RY_DRIVER_ERROR_BUSY;
	}

	switch (control & RY_SPI_CONTROL_Msk)
	{
		default:
			return RY_DRIVER_ERROR_UNSUPPORTED;

		case RY_SPI_MODE_MASTER_SIMPLEX:  // SPI master (output/input on MOSI); arg = bus speed in bps
		case RY_SPI_MODE_SLAVE_SIMPLEX:   // SPI slave (output/input on MISO
		case RY_SPI_SET_DEFAULT_TX_VALUE: // set default Transmit value; arg = value
		case RY_SPI_CONTROL_SS:           // control slave select; arg = 0:inactive, 1:active
			return RY_SPI_ERROR_MODE;

		case RY_SPI_MODE_INACTIVE: // SPI inactive
			spi->info->mode &= ~RY_SPI_CONTROL_Msk;
			spi->info->mode |= RY_SPI_MODE_INACTIVE;

			spi->info->flags &= ~SPI_FLAG_CONFIGURED;

			return RY_DRIVER_OK;

		case RY_SPI_MODE_MASTER: // SPI master (output on MOSI, input on MISO); arg = bus speed in bps
			// set master mode and disable data merge mode
			spi->reg->SPI_TRANS_FMT &= ~(SPI_MERGE | SPI_SLAVE);

			spi->info->mode &= ~RY_SPI_CONTROL_Msk;
			spi->info->mode |= RY_SPI_MODE_MASTER;

			spi->info->flags |= SPI_FLAG_CONFIGURED;

			goto set_speed;

		case RY_SPI_MODE_SLAVE: // SPI slave (output on MISO, input on MOSI)
			// set slave mode and disable data merge mode
			spi->reg->SPI_TRANS_FMT &= ~SPI_MERGE;
			spi->reg->SPI_TRANS_FMT |= SPI_SLAVE;

			spi->info->mode &= ~RY_SPI_CONTROL_Msk;
			spi->info->mode |= RY_SPI_MODE_SLAVE;

			spi->info->flags |= SPI_FLAG_CONFIGURED;

			break;

		case RY_SPI_SET_BUS_SPEED: // set bus speed in bps; arg = value
			set_speed:
				if (arg == 0U)
				{
					return RY_DRIVER_ERROR;
				}

				sclk_div = (g_sysclk_freq / (2 * arg)) - 1;

				spi->reg->SPI_TIMING &= ~0xff;
				spi->reg->SPI_TIMING |= sclk_div;

				if ((control & RY_SPI_CONTROL_Msk) == RY_SPI_SET_BUS_SPEED)
				{
					return RY_DRIVER_OK;
				}
			break;

		case RY_SPI_GET_BUS_SPEED: // get bus speed in bps
			sclk_div = spi->reg->SPI_TIMING & 0xff;
			return (g_sysclk_freq / ((sclk_div + 1) * 2));

		case RY_SPI_TX_HEADER_LENGTH:
			spi->info->tx_header_len = (uint8_t)arg;
			return RY_DRIVER_OK;
	}

	// SPI slave select mode for master
	if((spi->info->mode & RY_SPI_CONTROL_Msk) ==  RY_SPI_MODE_MASTER)
	{
		switch(control & RY_SPI_SS_MASTER_MODE_Msk)
		{
			case RY_SPI_SS_MASTER_HW_OUTPUT:
				break;
			case RY_SPI_SS_MASTER_UNUSED:
			case RY_SPI_SS_MASTER_SW:
			case RY_SPI_SS_MASTER_HW_INPUT:
				return RY_SPI_ERROR_SS_MODE;
			default:
				break;
		}
	}

	// SPI slave select mode for slave
	if((spi->info->mode & RY_SPI_CONTROL_Msk) == RY_SPI_MODE_SLAVE)
	{
		switch(control & RY_SPI_SS_SLAVE_MODE_Msk)
		{
			case RY_SPI_SS_SLAVE_HW:
				break;
			case RY_SPI_SS_SLAVE_SW:
				return RY_SPI_ERROR_SS_MODE;
			default:
				break;
		}
	}

	// SPI data-only transfer mode for slave
	if((spi->info->mode & RY_SPI_CONTROL_Msk) == RY_SPI_MODE_SLAVE)
	{
		if((control & RY_SPI_TRANSFER_FORMAT_Msk) == RY_SPI_SLV_DATA_ONLY_TRANSFER)
		{
			spi->info->mode &= ~RY_SPI_TRANSFER_FORMAT_Msk;
			spi->info->mode |= RY_SPI_SLV_DATA_ONLY_TRANSFER;
		}
	}

	// set SPI frame format
	switch (control & RY_SPI_FRAME_FORMAT_Msk)
	{
		case RY_SPI_CPOL0_CPHA0:
			spi->reg->SPI_TRANS_FMT &= ~(SPI_CPOL | SPI_CPHA);
			break;
		case RY_SPI_CPOL0_CPHA1:
			spi->reg->SPI_TRANS_FMT &= ~SPI_CPOL;
			spi->reg->SPI_TRANS_FMT |= SPI_CPHA;
			break;
		case RY_SPI_CPOL1_CPHA0:
			spi->reg->SPI_TRANS_FMT |= SPI_CPOL;
			spi->reg->SPI_TRANS_FMT &= ~SPI_CPHA;
			break;
		case RY_SPI_CPOL1_CPHA1:
			spi->reg->SPI_TRANS_FMT |= SPI_CPOL;
			spi->reg->SPI_TRANS_FMT |= SPI_CPHA;
			break;
		case RY_SPI_TI_SSI:
		case RY_SPI_MICROWIRE:
			default:
			return RY_SPI_ERROR_FRAME_FORMAT;
	}

	// set number of data bits
	spi->info->data_bits = ((control & RY_SPI_DATA_BITS_Msk) >> RY_SPI_DATA_BITS_Pos);

	if ((spi->info->data_bits < 1U) || (spi->info->data_bits > 32U))
	{
		return RY_SPI_ERROR_DATA_BITS;
	}
	else
	{
		spi->reg->SPI_TRANS_FMT &= ~(DATA_BITS_MSK);
		spi->reg->SPI_TRANS_FMT |= DATA_BITS(spi->info->data_bits);
	}

	// set SPI bit order
	if ((control & RY_SPI_BIT_ORDER_Msk) == RY_SPI_LSB_MSB)
	{
		spi->reg->SPI_TRANS_FMT |= SPI_LSB;
	}
	else
	{
		spi->reg->SPI_TRANS_FMT &= ~SPI_LSB;
	}

	return RY_DRIVER_OK;
}

static RY_SPI_STATUS spi_get_status(SPI_RESOURCES *spi)
{
	RY_SPI_STATUS status;

	status.busy       = spi->info->status.busy;
	status.data_lost  = spi->info->status.data_lost;
	status.mode_fault = spi->info->status.mode_fault;

	return (status);
}

static void spi_irq_handler(SPI_RESOURCES *spi)
{
	uint32_t i, j, status;
	uint32_t data = 0;
	uint32_t rx_num = 0;
	uint32_t event = 0;

	// read status register
	status = spi->reg->SPI_INT_ST;

	if ((status & SPI_RXFIFOOORINT) || (status & SPI_TXFIFOOURINT))
	{
		// TX FIFO underrun or RX FIFO overrun interrupt status
		spi->info->status.data_lost = 1U;

		event |= RY_SPI_EVENT_DATA_LOST;
	}

	if (status & SPI_TXFIFOINT)
	{
		for (i = 0; i < spi->info->xfer.txfifo_refill; i++)
		{
			data = 0;
			if (spi->info->xfer.tx_buf < spi->info->xfer.tx_buf_limit)
			{
				// handle the data frame format
				if (spi->info->data_bits <= 8) 			// data bits = 1....8
				{
					data = *spi->info->xfer.tx_buf;
					spi->info->xfer.tx_buf++;
				}
				else if(spi->info->data_bits <= 16) 	// data bits = 9....16
				{
					for (j = 0; j < 2; j++)
					{
						data |= *spi->info->xfer.tx_buf << j * 8;
						spi->info->xfer.tx_buf++;
					}
				}
				else 	// data bits = 17....32
				{
					for (j = 0; j < 4; j++)
					{
						data |= *spi->info->xfer.tx_buf << j * 8;
						spi->info->xfer.tx_buf++;
					}
				}
				spi->reg->SPI_DATA = data;
				spi->info->xfer.tx_cnt++;
			}
			else
			{
				spi->reg->SPI_INT_EN &= ~SPI_TXFIFOINT;
			}
		}
	}

	if (status & SPI_RXFIFOINT)
	{
		// get number of valid entries in the RX FIFO
		rx_num = (((spi->reg->SPI_STATUS >> 8) & 0x2f) | ((spi->reg->SPI_STATUS & (0x3 << 24)) >> 18));

		for (i = rx_num; i > 0; i--)
		{
			data = spi->reg->SPI_DATA;
			spi->info->xfer.rx_cnt++;

			// handle the data frame format
			if (spi->info->data_bits <= 8)
			{
				*spi->info->xfer.rx_buf = data & 0xff;
				spi->info->xfer.rx_buf++;
			}
			else if(spi->info->data_bits <= 16)
			{
				for (j = 0; j < 2; j++)
				{
					*spi->info->xfer.rx_buf = (data >> j * 8) & 0xff;
					spi->info->xfer.rx_buf++;
				}
			}
			else
			{
				for (j = 0; j < 4; j++)
				{
					*spi->info->xfer.rx_buf = (data >> j * 8) & 0xff;
					spi->info->xfer.rx_buf++;
				}
			}

			//If there is a tx header on spi_transfer, drop the dummy data from slave when master sending header.
			if ((spi->info->is_header == 1) && (spi->info->xfer.rx_cnt == spi->info->tx_header_len))
			{
				spi->info->is_header = 0;
				spi->info->xfer.rx_buf = spi->info->record_rx_buf;
				spi->info->xfer.rx_cnt = 0;
			}
		}
	}

	if (status & SPI_ENDINT)
	{
		// disable SPI interrupts
		spi->reg->SPI_INT_EN = 0;

		if (spi->dma_tx && ((spi->info->xfer.transfer_op == SPI_SEND) || (spi->info->xfer.transfer_op == SPI_TRANSFER)))
		{
			if (!spi->info->xfer.dma_tx_complete)
			{
				dma_channel_disable(DMA0_QSPI, spi->dma_tx->channel);
			}
			spi->info->xfer.dma_tx_complete = 0;
		}

		if (spi->dma_rx && ((spi->info->xfer.transfer_op == SPI_RECEIVE) || (spi->info->xfer.transfer_op == SPI_TRANSFER)))
		{
			if (!spi->info->xfer.dma_rx_complete)
			{
				dma_channel_disable(DMA0_QSPI, spi->dma_rx->channel);
			}
			spi->info->xfer.dma_rx_complete = 0;
		}

		// clear TX/RX FIFOs
		spi->reg->SPI_CTRL = (TXFIFORST | RXFIFORST);

		spi->info->status.busy = 0U;

		event |= RY_SPI_EVENT_TRANSFER_COMPLETE;
	}

	// clear interrupt status
	spi->reg->SPI_INT_ST = status;
	// make sure "write 1 clear" take effect before iret
	spi->reg->SPI_INT_ST;

	if((spi->info->cb_event != NULL) && (event != 0))
	{
		spi->info->cb_event(event);
	}
}

#if (((GTRY_SPI0_EN) && (GTRY_SPI0_DMA_TX_EN == 1)) || \
     ((GTRY_SPI1_EN) && (GTRY_SPI1_DMA_TX_EN == 1)) || \
     ((GTRY_SPI2_EN) && (GTRY_SPI2_DMA_TX_EN == 1)) || \
     ((GTRY_SPI3_EN) && (GTRY_SPI3_DMA_TX_EN == 1)) || \
     ((GTRY_SPI4_EN) && (GTRY_SPI4_DMA_TX_EN == 1)) || \
     ((GTRY_SPI5_EN) && (GTRY_SPI5_DMA_TX_EN == 1)) || \
     ((GTRY_SPI6_EN) && (GTRY_SPI6_DMA_TX_EN == 1)))
static void spi_dma_tx_event (uint32_t event, SPI_RESOURCES * spi)
{
	switch(event)
	{
		case DMA_EVENT_TC:
			spi->info->xfer.dma_tx_complete = 1;
			break;
		case DMA_EVENT_ERROR:
		default:
			break;
	}
}
#endif

#if (((GTRY_SPI0_EN) && (GTRY_SPI0_DMA_RX_EN == 1)) || \
     ((GTRY_SPI1_EN) && (GTRY_SPI1_DMA_RX_EN == 1)) || \
     ((GTRY_SPI2_EN) && (GTRY_SPI2_DMA_RX_EN == 1)) || \
     ((GTRY_SPI3_EN) && (GTRY_SPI3_DMA_RX_EN == 1)) || \
     ((GTRY_SPI4_EN) && (GTRY_SPI4_DMA_RX_EN == 1)) || \
     ((GTRY_SPI5_EN) && (GTRY_SPI5_DMA_RX_EN == 1)) || \
     ((GTRY_SPI6_EN) && (GTRY_SPI6_DMA_RX_EN == 1)))
static void spi_dma_rx_event (uint32_t event, SPI_RESOURCES * spi)
{
	switch(event)
	{
		case DMA_EVENT_TC:
			if (spi->info->is_header == 0)
			{
				spi->info->xfer.dma_rx_complete = 1;
			}
			else
			{
				//Setting another dma transfer to cover the dummy data from slave when master is sending header.
				// configure DMA channel
				dma_channel_configure(DMA0_QSPI, spi->dma_rx->channel,
						       (unsigned long)(&(spi->reg->SPI_DATA)),
						       (unsigned long)spi->info->xfer.rx_buf,
						       spi->info->data_num,
						       DMA_CH_CTRL_SBINF(DMA_INF_IDX0)            |
						       DMA_CH_CTRL_DBINF(DMA_INF_IDX0)            |
						       DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)            |
						       //DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
						       DMA_CH_CTRL_SWIDTH(spi->info->src_width/2) |
						       DMA_CH_CTRL_DWIDTH(spi->info->src_width/2) |
						       DMA_CH_CTRL_SMODE_HANDSHAKE                |
						       DMA_CH_CTRL_SRCADDR_FIX                    |
						       DMA_CH_CTRL_DSTADDR_INC                    |
						       DMA_CH_CTRL_SRCREQ(spi->dma_rx->reqsel)    |
						       DMA_CH_CTRL_EN_INTABT                         |
						       DMA_CH_CTRL_EN_INTERR                         |
						       DMA_CH_CTRL_EN_INTTC                          |
						       DMA_CH_CTRL_ENABLE,
						       spi->dma_rx->cb_event);
				spi->info->is_header = 0;
			}
			break;
		case DMA_EVENT_ERROR:
		default:
			break;
	}
}
#endif

#if (GTRY_SPI0_EN)
// SPI0 driver wrapper functions
static RY_SPI_CAPABILITIES spi0_get_capabilities(void)
{
	return spi_get_capabilities(&spi0_resources);
}
static int32_t spi0_initialize(p_spi_int_cb cb_event)
{
	return spi_initialize(cb_event, &spi0_resources);
}
static int32_t spi0_uninitialize (void)
{
	return spi_uninitialize(&spi0_resources);
}
static int32_t spi0_power_control(RY_POWER_STATE state)
{
	return spi_power_control(state, &spi0_resources);
}
static int32_t spi0_send(const void *data, uint32_t num)
{
	return spi_send(data, num, &spi0_resources);
}
static int32_t spi0_receive(void *data, uint32_t num)
{
	return spi_receive(data, num, &spi0_resources);
}
static int32_t spi0_transfer(const void *data_out, void *data_in, uint32_t num)
{
	return spi_transfer(data_out, data_in, num, &spi0_resources);
}
static uint32_t spi0_get_data_count(void)
{
	return spi_get_data_count(&spi0_resources);
}
static int32_t spi0_control(uint32_t control, uint32_t arg)
{
	return spi_control(control, arg, &spi0_resources);
}
static RY_SPI_STATUS spi0_get_status(void)
{
	return spi_get_status(&spi0_resources);
}

/*-----------*/
void spi0_irq_handler(void)
{
	spi_irq_handler(&spi0_resources);
}

#if (GTRY_SPI0_DMA_TX_EN == 1)
void spi0_dma_tx_event(uint32_t event)
{
	spi_dma_tx_event(event, &spi0_resources);
}
#endif

#if (GTRY_SPI0_DMA_RX_EN == 1)
void spi0_dma_rx_event(uint32_t event)
{
	spi_dma_rx_event(event, &spi0_resources);
}
#endif

// SPI0 driver control block
RY_DRIVER_SPI Driver_SPI0 = {
	spi0_get_capabilities,
	spi0_initialize,
	spi0_uninitialize,
	spi0_power_control,
	spi0_send,
	spi0_receive,
	spi0_transfer,
	spi0_get_data_count,
	spi0_control,
	spi0_get_status
};
#endif

#if (GTRY_SPI1_EN)
// SPI1 driver wrapper functions
static RY_SPI_CAPABILITIES spi1_get_capabilities(void)
{
	return spi_get_capabilities(&spi1_resources);
}
static int32_t spi1_initialize(p_spi_int_cb cb_event)
{
	return spi_initialize(cb_event, &spi1_resources);
}
static int32_t spi1_uninitialize (void)
{
	return spi_uninitialize(&spi1_resources);
}
static int32_t spi1_power_control(RY_POWER_STATE state)
{
	return spi_power_control(state, &spi1_resources);
}
static int32_t spi1_send(const void *data, uint32_t num)
{
	return spi_send(data, num, &spi1_resources);
}
static int32_t spi1_receive(void *data, uint32_t num)
{
	return spi_receive(data, num, &spi1_resources);
}
static int32_t spi1_transfer(const void *data_out, void *data_in, uint32_t num)
{
	return spi_transfer(data_out, data_in, num, &spi1_resources);
}
static uint32_t spi1_get_data_count(void)
{
	return spi_get_data_count(&spi1_resources);
}
static int32_t spi1_control(uint32_t control, uint32_t arg)
{
	return spi_control(control, arg, &spi1_resources);
}
static RY_SPI_STATUS spi1_get_status(void)
{
	return spi_get_status(&spi1_resources);
}

/*----------*/
void spi1_irq_handler(void)
{
	spi_irq_handler(&spi1_resources);
}

#if (GTRY_SPI1_DMA_TX_EN == 1)
void spi1_dma_tx_event(uint32_t event)
{
	spi_dma_tx_event(event, &spi1_resources);
}
#endif

#if (GTRY_SPI1_DMA_RX_EN == 1)
void spi1_dma_rx_event(uint32_t event)
{
	spi_dma_rx_event(event, &spi1_resources);
}
#endif

// SPI1 driver control block
RY_DRIVER_SPI Driver_SPI1 = {
	spi1_get_capabilities,
	spi1_initialize,
	spi1_uninitialize,
	spi1_power_control,
	spi1_send,
	spi1_receive,
	spi1_transfer,
	spi1_get_data_count,
	spi1_control,
	spi1_get_status
};
#endif

#if (GTRY_SPI2_EN)
// SPI2 driver wrapper functions
static RY_SPI_CAPABILITIES spi2_get_capabilities(void)
{
	return spi_get_capabilities(&spi2_resources);
}
static int32_t spi2_initialize(p_spi_int_cb cb_event)
{
	return spi_initialize(cb_event, &spi2_resources);
}
static int32_t spi2_uninitialize(void)
{
	return spi_uninitialize(&spi2_resources);
}
static int32_t spi2_power_control(RY_POWER_STATE state)
{
	return spi_power_control(state, &spi2_resources);
}
static int32_t spi2_send(const void *data, uint32_t num)
{
	return spi_send(data, num, &spi2_resources);
}
static int32_t spi2_receive(void *data, uint32_t num)
{
	return spi_receive(data, num, &spi2_resources);
}
static int32_t spi2_transfer(const void *data_out, void *data_in, uint32_t num)
{
	return spi_transfer(data_out, data_in, num, &spi2_resources);
}
static uint32_t spi2_get_data_count(void)
{
	return spi_get_data_count(&spi2_resources);
}
static int32_t spi2_control(uint32_t control, uint32_t arg)
{
	return spi_control(control, arg, &spi2_resources);
}
static RY_SPI_STATUS spi2_get_status(void)
{
	return spi_get_status(&spi2_resources);
}

/*----------*/
void spi2_irq_handler(void)
{
	spi_irq_handler(&spi2_resources);
}

#if (GTRY_SPI2_DMA_TX_EN == 1)
void spi2_dma_tx_event(uint32_t event)
{
	spi_dma_tx_event(event, &spi2_resources);
}
#endif

#if (GTRY_SPI2_DMA_RX_EN == 1)
void spi2_dma_rx_event(uint32_t event)
{
	spi_dma_rx_event(event, &spi2_resources);
}
#endif

// SPI2 driver control block
RY_DRIVER_SPI Driver_SPI2 = {
	spi2_get_capabilities,
	spi2_initialize,
	spi2_uninitialize,
	spi2_power_control,
	spi2_send,
	spi2_receive,
	spi2_transfer,
	spi2_get_data_count,
	spi2_control,
	spi2_get_status
};
#endif

#if (GTRY_SPI3_EN)
// SPI3 driver wrapper functions
static RY_SPI_CAPABILITIES spi3_get_capabilities(void)
{
	return spi_get_capabilities(&spi3_resources);
}
static int32_t spi3_initialize(p_spi_int_cb cb_event)
{
	return spi_initialize(cb_event, &spi3_resources);
}
static int32_t spi3_uninitialize(void)
{
	return spi_uninitialize(&spi3_resources);
}
static int32_t spi3_power_control(RY_POWER_STATE state)
{
	return spi_power_control(state, &spi3_resources);
}
static int32_t spi3_send(const void *data, uint32_t num)
{
	return spi_send(data, num, &spi3_resources);
}
static int32_t spi3_receive(void *data, uint32_t num)
{
	return spi_receive(data, num, &spi3_resources);
}
static int32_t spi3_transfer(const void *data_out, void *data_in, uint32_t num)
{
	return spi_transfer(data_out, data_in, num, &spi3_resources);
}
static uint32_t spi3_get_data_count(void)
{
	return spi_get_data_count(&spi3_resources);
}
static int32_t spi3_control(uint32_t control, uint32_t arg)
{
	return spi_control(control, arg, &spi3_resources);
}
static RY_SPI_STATUS spi3_get_status(void)
{
	return spi_get_status(&spi3_resources);
}

/*----------*/
void spi3_irq_handler(void)
{
	spi_irq_handler(&spi3_resources);
}

#if (GTRY_SPI3_DMA_TX_EN == 1)
void spi3_dma_tx_event(uint32_t event)
{
	spi_dma_tx_event(event, &spi3_resources);
}
#endif

#if (GTRY_SPI3_DMA_RX_EN == 1)
void spi3_dma_rx_event(uint32_t event)
{
	spi_dma_rx_event(event, &spi3_resources);
}
#endif

// SPI3 driver control block
RY_DRIVER_SPI Driver_SPI3 = {
	spi3_get_capabilities,
	spi3_initialize,
	spi3_uninitialize,
	spi3_power_control,
	spi3_send,
	spi3_receive,
	spi3_transfer,
	spi3_get_data_count,
	spi3_control,
	spi3_get_status
};
#endif

#if (GTRY_SPI4_EN)
// SPI4 driver wrapper functions
static RY_SPI_CAPABILITIES spi4_get_capabilities(void)
{
	return spi_get_capabilities(&spi4_resources);
}
static int32_t spi4_initialize(p_spi_int_cb cb_event)
{
	return spi_initialize(cb_event, &spi4_resources);
}
static int32_t spi4_uninitialize(void)
{
	return spi_uninitialize(&spi4_resources);
}
static int32_t spi4_power_control(RY_POWER_STATE state)
{
	return spi_power_control(state, &spi4_resources);
}
static int32_t spi4_send(const void *data, uint32_t num)
{
	return spi_send(data, num, &spi4_resources);
}
static int32_t spi4_receive(void *data, uint32_t num)
{
	return spi_receive(data, num, &spi4_resources);
}
static int32_t spi4_transfer(const void *data_out, void *data_in, uint32_t num)
{
	return spi_transfer(data_out, data_in, num, &spi4_resources);
}
static uint32_t spi4_get_data_count(void)
{
	return spi_get_data_count(&spi4_resources);
}
static int32_t spi4_control(uint32_t control, uint32_t arg)
{
	return spi_control(control, arg, &spi4_resources);
}
static RY_SPI_STATUS spi4_get_status(void)
{
	return spi_get_status(&spi4_resources);
}

/*----------*/
void spi4_irq_handler(void)
{
	spi_irq_handler(&spi4_resources);
}

#if (GTRY_SPI4_DMA_TX_EN == 1)
void spi4_dma_tx_event(uint32_t event)
{
	spi_dma_tx_event(event, &spi4_resources);
}
#endif

#if (GTRY_SPI4_DMA_RX_EN == 1)
void spi4_dma_rx_event(uint32_t event)
{
	spi_dma_rx_event(event, &spi4_resources);
}
#endif

// SPI4 driver control block
RY_DRIVER_SPI Driver_SPI4 = {
	spi4_get_capabilities,
	spi4_initialize,
	spi4_uninitialize,
	spi4_power_control,
	spi4_send,
	spi4_receive,
	spi4_transfer,
	spi4_get_data_count,
	spi4_control,
	spi4_get_status
};
#endif

#if (GTRY_SPI5_EN)
// SPI4 driver wrapper functions
static RY_SPI_CAPABILITIES spi5_get_capabilities(void)
{
	return spi_get_capabilities(&spi5_resources);
}
static int32_t spi5_initialize(p_spi_int_cb cb_event)
{
	return spi_initialize(cb_event, &spi5_resources);
}
static int32_t spi5_uninitialize(void)
{
	return spi_uninitialize(&spi5_resources);
}
static int32_t spi5_power_control(RY_POWER_STATE state)
{
	return spi_power_control(state, &spi5_resources);
}
static int32_t spi5_send(const void *data, uint32_t num)
{
	return spi_send(data, num, &spi5_resources);
}
static int32_t spi5_receive(void *data, uint32_t num)
{
	return spi_receive(data, num, &spi5_resources);
}
static int32_t spi5_transfer(const void *data_out, void *data_in, uint32_t num)
{
	return spi_transfer(data_out, data_in, num, &spi5_resources);
}
static uint32_t spi5_get_data_count(void)
{
	return spi_get_data_count(&spi5_resources);
}
static int32_t spi5_control(uint32_t control, uint32_t arg)
{
	return spi_control(control, arg, &spi5_resources);
}
static RY_SPI_STATUS spi5_get_status(void)
{
	return spi_get_status(&spi5_resources);
}

/*----------*/
void spi5_irq_handler(void)
{
	spi_irq_handler(&spi5_resources);
}

#if (GTRY_SPI5_DMA_TX_EN == 1)
void spi5_dma_tx_event(uint32_t event)
{
	spi_dma_tx_event(event, &spi5_resources);
}
#endif

#if (GTRY_SPI5_DMA_RX_EN == 1)
void spi5_dma_rx_event(uint32_t event)
{
	spi_dma_rx_event(event, &spi5_resources);
}
#endif

// SPI5 driver control block
RY_DRIVER_SPI Driver_SPI5 = {
	spi5_get_capabilities,
	spi5_initialize,
	spi5_uninitialize,
	spi5_power_control,
	spi5_send,
	spi5_receive,
	spi5_transfer,
	spi5_get_data_count,
	spi5_control,
	spi5_get_status
};
#endif

#if (GTRY_SPI6_EN)
// SPI6 driver wrapper functions
static RY_SPI_CAPABILITIES spi6_get_capabilities(void)
{
	return spi_get_capabilities(&spi6_resources);
}
static int32_t spi6_initialize(p_spi_int_cb cb_event)
{
	return spi_initialize(cb_event, &spi6_resources);
}
static int32_t spi6_uninitialize(void)
{
	return spi_uninitialize(&spi6_resources);
}
static int32_t spi6_power_control(RY_POWER_STATE state)
{
	return spi_power_control(state, &spi6_resources);
}
static int32_t spi6_send(const void *data, uint32_t num)
{
	return spi_send(data, num, &spi6_resources);
}
static int32_t spi6_receive(void *data, uint32_t num)
{
	return spi_receive(data, num, &spi6_resources);
}
static int32_t spi6_transfer(const void *data_out, void *data_in, uint32_t num)
{
	return spi_transfer(data_out, data_in, num, &spi6_resources);
}
static uint32_t spi6_get_data_count(void)
{
	return spi_get_data_count(&spi6_resources);
}
static int32_t spi6_control(uint32_t control, uint32_t arg)
{
	return spi_control(control, arg, &spi6_resources);
}
static RY_SPI_STATUS spi6_get_status(void)
{
	return spi_get_status(&spi6_resources);
}

/*----------*/
void spi6_irq_handler(void)
{
	spi_irq_handler(&spi6_resources);
}

#if (GTRY_SPI6_DMA_TX_EN == 1)
void spi6_dma_tx_event(uint32_t event)
{
	spi_dma_tx_event(event, &spi6_resources);
}
#endif

#if (GTRY_SPI6_DMA_RX_EN == 1)
void spi6_dma_rx_event(uint32_t event)
{
	spi_dma_rx_event(event, &spi6_resources);
}
#endif

// SPI6 driver control block
RY_DRIVER_SPI Driver_SPI6 = {
	spi6_get_capabilities,
	spi6_initialize,
	spi6_uninitialize,
	spi6_power_control,
	spi6_send,
	spi6_receive,
	spi6_transfer,
	spi6_get_data_count,
	spi6_control,
	spi6_get_status
};
#endif
