/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

#include "gtry_uart_drv.h"
#include "gtry_gpio_drv.h"


#if ((!GTRY_UART0_EN) && (!GTRY_UART1_EN) && (!GTRY_UART2_EN))
	#error "UART not enabled in gtry_platform.h!"
#endif

// Trigger level definitions
// Can be user defined by C preprocessor
#ifndef UART0_TRIG_LVL
#define 	UART0_TRIG_LVL            UART_TRIG_LVL_1
#endif
#ifndef UART1_TRIG_LVL
#define 	UART1_TRIG_LVL            UART_TRIG_LVL_1
#endif
#ifndef UART2_TRIG_LVL
#define 	UART2_TRIG_LVL            UART_TRIG_LVL_1
#endif


unsigned int uart_io_multi_config(RYRV_UART_NO uart_no, uart_multi_io uart_gp, uint32 flow_enable)
{
	uint32_t offset_no = 0x0;
	uint32_t io_num = flow_enable? 4:2;//((uart_gp == UART_IO_GP0) && (uart_no == UART_NO0))?10:4;

	if(uart_no >= UART_NUM_MAXS || uart_gp >= UART_IO_GP_MAX)
	{
		return 1;
	}
	if(uart_no == UART_NO1 && flow_enable)
	{
		return 2;
	}
	switch(uart_no)
	{
		case UART_NO0:
			offset_no = (uart_gp == UART_IO_GP0)?1:(20 + (uart_gp-1)*12);
			break;
		case UART_NO1:
			offset_no = 12 + (uart_gp*12);
			break;
		case UART_NO2:
			offset_no = 16 + (uart_gp*12);
			break;
		default:
			return 3;
	}

	if((offset_no+io_num)>=64)
	{
		return 4;
	}

	for(uint32_t i = 0; i < io_num; i++)
	{
		GTRY_GPIO->GPIO_CFG[offset_no+i] &= ~(RY_MAX_REUSE << GPIO_CFG_REUSE_POS);
		GTRY_GPIO->GPIO_CFG[offset_no+i] |= ((RY_UART_REUSE << GPIO_CFG_REUSE_POS)|(RY_GPIO_INPUT_EN << GPIO_CFG_IN_EN_POS));
	}
	return 0;
}

// UART0
#if (GTRY_UART0_EN)
static RY_UART_INFO uart0_info = {0};

#if (GTRY_UART0_DMA_TX_EN == 1)
void uart0_dma_tx_event(uint32_t event);
static RY_UART_DMA uart0_dma_tx = {GTRY_UART0_DMA_TX_CH, GTRY_UART0_DMA_TX_REQID, uart0_dma_tx_event};
#endif
#if (GTRY_UART0_DMA_RX_EN == 1)
void uart0_dma_rx_event(uint32_t event);
static RY_UART_DMA uart0_dma_rx = {GTRY_UART0_DMA_RX_CH, GTRY_UART0_DMA_RX_REQID, uart0_dma_rx_event};
#endif


/*static const */RY_UART_RESOURCES uart0_resources = {
	{   // Capabilities
	//	1,  // supports UART (Asynchronous) mode
	//	0,  // supports Synchronous Master mode
	//	0,  // supports Synchronous Slave mode
	//	0,  // supports UART Single-wire mode
	//	0,  // supports UART IrDA mode
	//	0,  // supports UART Smart Card mode
	//	0,  // Smart Card Clock generator
		0,  // RTS Flow Control available
		0,  // CTS Flow Control available
		0,  // Transmit completed event: \ref RY_UART_EVENT_TX_COMPLETE
	#if ((GTRY_UART0_DMA_RX_EN == 1) || (UART0_TRIG_LVL == UART_TRIG_LVL_1))
		0,  // Signal receive character timeout event: \ref RY_UART_EVENT_RX_TIMEOUT
	#else
		1,  // Signal receive character timeout event: \ref RY_UART_EVENT_RX_TIMEOUT
	#endif
		0,  // RTS Line: 0=not available, 1=available
		0,  // CTS Line: 0=not available, 1=available
		0,  // DTR Line: 0=not available, 1=available
		0,  // DSR Line: 0=not available, 1=available
		0,  // DCD Line: 0=not available, 1=available
		0,  // RI Line: 0=not available, 1=available
		0,  // Signal CTS change event: \ref RY_UART_EVENT_CTS
		0,  // Signal DSR change event: \ref RY_UART_EVENT_DSR
		0,  // Signal DCD change event: \ref RY_UART_EVENT_DCD
		0,  // Signal RI change event: \ref RY_UART_EVENT_RI
	},
	GTRY_UART0,
	IRQ_UART0_SOURCE,
	UART0_TRIG_LVL,
#if (GTRY_UART0_DMA_TX_EN == 1)
	&uart0_dma_tx,
#else
	NULL,
#endif
#if (GTRY_UART0_DMA_RX_EN == 1)
	&uart0_dma_rx,
#else
	NULL,
#endif
	&uart0_info
};
#endif

// UART1
#if (GTRY_UART1_EN)
static RY_UART_INFO uart1_info = {0};

#if (GTRY_UART1_DMA_TX_EN == 1)
void uart1_dma_tx_event(uint32_t event);
static RY_UART_DMA uart1_dma_tx = {GTRY_UART1_DMA_TX_CH, GTRY_UART1_DMA_TX_REQID, uart1_dma_tx_event};
#endif
#if (GTRY_UART1_DMA_RX_EN == 1)
void uart1_dma_rx_event(uint32_t event);
static RY_UART_DMA uart1_dma_rx = {GTRY_UART1_DMA_RX_CH, GTRY_UART1_DMA_RX_REQID, uart1_dma_rx_event};
#endif

/*static const */RY_UART_RESOURCES uart1_resources = {
	{   // Capabilities
	//	1,  // supports UART (Asynchronous) mode
	//	0,  // supports Synchronous Master mode
	//	0,  // supports Synchronous Slave mode
	//	0,  // supports UART Single-wire mode
	//	0,  // supports UART IrDA mode
	//	0,  // supports UART Smart Card mode
	//	0,  // Smart Card Clock generator
		0,  // RTS Flow Control available
		0,  // CTS Flow Control available
		0,  // Transmit completed event: \ref RY_UART_EVENT_TX_COMPLETE
	#if ((GTRY_UART1_DMA_RX_EN == 1) || (UART1_TRIG_LVL == UART_TRIG_LVL_1))
		0,  // Signal receive character timeout event: \ref RY_UART_EVENT_RX_TIMEOUT
	#else
		1,  // Signal receive character timeout event: \ref RY_UART_EVENT_RX_TIMEOUT
	#endif
		0,  // RTS Line: 0=not available, 1=available
		0,  // CTS Line: 0=not available, 1=available
		0,  // DTR Line: 0=not available, 1=available
		0,  // DSR Line: 0=not available, 1=available
		0,  // DCD Line: 0=not available, 1=available
		0,  // RI Line: 0=not available, 1=available
		0,  // Signal CTS change event: \ref RY_UART_EVENT_CTS
		0,  // Signal DSR change event: \ref RY_UART_EVENT_DSR
		0,  // Signal DCD change event: \ref RY_UART_EVENT_DCD
		0,  // Signal RI change event: \ref RY_UART_EVENT_RI
	},
	GTRY_UART1,
	IRQ_UART1_SOURCE,
	UART1_TRIG_LVL,
#if (GTRY_UART1_DMA_TX_EN == 1)
	&uart1_dma_tx,
#else
	NULL,
#endif
#if (GTRY_UART1_DMA_RX_EN == 1)
	&uart1_dma_rx,
#else
	NULL,
#endif
	&uart1_info
};
#endif


// UART2
#if (GTRY_UART2_EN)
static RY_UART_INFO uart2_info = {0};

#if (GTRY_UART2_DMA_TX_EN == 1)
void uart2_dma_tx_event(uint32_t event);
static RY_UART_DMA uart2_dma_tx = {GTRY_UART2_DMA_TX_CH, GTRY_UART2_DMA_TX_REQID, uart2_dma_tx_event};
#endif
#if (GTRY_UART2_DMA_RX_EN == 1)
void uart2_dma_rx_event(uint32_t event);
static RY_UART_DMA uart2_dma_rx = {GTRY_UART2_DMA_RX_CH, GTRY_UART2_DMA_RX_REQID, uart2_dma_rx_event};
#endif

/*static const */RY_UART_RESOURCES uart2_resources = {
	{   // Capabilities
	//	1,  // supports UART (Asynchronous) mode
	//	0,  // supports Synchronous Master mode
	//	0,  // supports Synchronous Slave mode
	//	0,  // supports UART Single-wire mode
	//	0,  // supports UART IrDA mode
	//	0,  // supports UART Smart Card mode
	//	0,  // Smart Card Clock generator
		0,  // RTS Flow Control available
		0,  // CTS Flow Control available
		0,  // Transmit completed event: \ref RY_UART_EVENT_TX_COMPLETE
	#if ((GTRY_UART2_DMA_RX_EN == 1) || (UART2_TRIG_LVL == UART_TRIG_LVL_1))
		0,  // Signal receive character timeout event: \ref RY_UART_EVENT_RX_TIMEOUT
	#else
		1,  // Signal receive character timeout event: \ref RY_UART_EVENT_RX_TIMEOUT
	#endif
		0,  // RTS Line: 0=not available, 1=available
		0,  // CTS Line: 0=not available, 1=available
		0,  // DTR Line: 0=not available, 1=available
		0,  // DSR Line: 0=not available, 1=available
		0,  // DCD Line: 0=not available, 1=available
		0,  // RI Line: 0=not available, 1=available
		0,  // Signal CTS change event: \ref RY_UART_EVENT_CTS
		0,  // Signal DSR change event: \ref RY_UART_EVENT_DSR
		0,  // Signal DCD change event: \ref RY_UART_EVENT_DCD
		0,  // Signal RI change event: \ref RY_UART_EVENT_RI
	},
	GTRY_UART2,
	IRQ_UART2_SOURCE,
	UART2_TRIG_LVL,
#if (GTRY_UART2_DMA_TX_EN == 1)
	&uart2_dma_tx,
#else
	NULL,
#endif
#if (GTRY_UART2_DMA_RX_EN == 1)
	&uart2_dma_rx,
#else
	NULL,
#endif
	&uart2_info
};
#endif


// Local Function
/**
  \fn          int32_t uart_set_baudrate (uint32_t baudrate, RY_UART_RESOURCES *uart)
  \brief       Set baudrate dividers
  \param[in]   baudrate  Uart baudrate
  \param[in]   uart Pointer to UART resources)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t uart_set_baudrate(uint32_t baudrate, RY_UART_RESOURCES *uart)
{
	uint32_t div;
	uart->reg->UART_OSCR = 8;

	// Oversampling is set to 8, and round off the divider value:
	//   divider = ((g_hclk_freq>>2) / (8 * baudrate)) + 0.5;
	//
	// And below is a variant which doesn't use FP operation.
	div = (((g_hclk_freq>>2)+(uart->reg->UART_OSCR>>1)*baudrate)/(uart->reg->UART_OSCR*baudrate));

	uart->reg->UART_LCR = 0;
	uart->reg->UART_LCR = 3;
	uart->reg->UART_LCR |= UART_LCR_DLAB;
	uart->reg->UART_DLL  = (div >> 0) & 0xff;
	uart->reg->UART_DLM  = (div >> 8) & 0xff;
	uart->reg->UART_LCR &= (~UART_LCR_DLAB);

	uart->info->baudrate = baudrate;

	return 0;
}

/**
  \fn          uint32_t uart_rxline_irq_handler(RY_UART_RESOURCES *uart)
  \brief       Receive line interrupt handler
  \param[in]   uart     Pointer to UART resources
  \return      Rx Line event mask
*/
static uint32_t uart_rxline_irq_handler (RY_UART_RESOURCES *uart)
{
	uint32_t lsr, event;

	#define UART_LSR_LINE_INT      (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_BI)
	event = 0U;
	lsr   = uart->reg->UART_LSR  & UART_LSR_LINE_INT;

	// OverRun error
	if (lsr & UART_LSR_OE)
	{
		uart->info->rx_status.rx_overflow = 1U;
		event |= RY_UART_EVENT_RX_OVERFLOW;
	}

	// Parity error
	if (lsr & UART_LSR_PE)
	{
		uart->info->rx_status.rx_parity_error = 1U;
		event |= RY_UART_EVENT_RX_PARITY_ERROR;
	}

	// Break detected
	if (lsr & UART_LSR_BI)
	{
		uart->info->rx_status.rx_break = 1U;
		event |= RY_UART_EVENT_RX_BREAK;
	// Framing error
	}
	else
	{
		if (lsr & UART_LSR_FE)
		{
			uart->info->rx_status.rx_framing_error = 1U;
			event |= RY_UART_EVENT_RX_FRAMING_ERROR;
		}
	}

	return event;
}

// UART Driver functions

/**
  \fn          RY_UART_CAPABILITIES uart_get_capabilities(RY_UART_RESOURCES *uart)
  \brief       Get driver capabilities
  \param[in]   uart Pointer to UART resources
  \return      \ref RY_UART_CAPABILITIES
*/
static RY_UART_CAPABILITIES uart_get_capabilities(RY_UART_RESOURCES *uart)
{
	return uart->capabilities;
}

/**
  \fn          int32_t uart_initialize(p_uart_int_cb cb_event, RY_UART_RESOURCES *uart)
  \brief       Initialize UART Interface.
  \param[in]   cb_event  Pointer to \ref p_uart_int_cb
  \param[in]   uart Pointer to UART resources
  \return      \ref execution_status
*/
static int32_t uart_initialize (p_uart_int_cb cb_event, RY_UART_RESOURCES *uart)
{
	if (uart->info->flags & UART_FLAG_INITIALIZED)
	{
		// Driver is already initialized
		return RY_DRIVER_OK;
	}

	// Initialize UART Run-time Resources
	uart->info->cb_event = cb_event;

	uart->info->rx_status.rx_busy          = 0U;
	uart->info->rx_status.rx_overflow      = 0U;
	uart->info->rx_status.rx_break         = 0U;
	uart->info->rx_status.rx_framing_error = 0U;
	uart->info->rx_status.rx_parity_error  = 0U;

	uart->info->xfer.send_active           = 0U;
	uart->info->xfer.tx_def_val            = 0U;

	// DMA Initialize
	if (uart->dma_tx || uart->dma_rx)
	{
		dma_initialize(DMA1_PERIPH);
	}

	uart->info->flags = UART_FLAG_INITIALIZED;

	return RY_DRIVER_OK;
}

/**
  \fn          int32_t uart_uninitialize (RY_UART_RESOURCES *uart)
  \brief       De-initialize UART Interface.
  \param[in]   uart Pointer to UART resources
  \return      \ref execution_status
*/
static int32_t uart_uninitialize(RY_UART_RESOURCES *uart)
{
	// DMA Uninitialize
	if (uart->dma_tx || uart->dma_rx)
	{
		dma_uninitialize (DMA1_PERIPH);
	}

	// Reset UART status flags
	uart->info->flags = 0U;

	return RY_DRIVER_OK;
}

/**
  \fn          int32_t uart_power_control(RY_POWER_STATE state, RY_UART_RESOURCES *uart)
  \brief       Control UART Interface Power.
  \param[in]   state  Power state
  \param[in]   uart  Pointer to UART resources
  \return      \ref execution_status
*/
static int32_t uart_power_control (RY_POWER_STATE state, RY_UART_RESOURCES *uart)
{
	switch (state)
	{
		case RY_POWER_OFF:
			// Disable UART IRQ
			__nds__plic_disable_interrupt(uart->irq_num);

			// If DMA mode - disable TX DMA channel
			if ((uart->dma_tx) && (uart->info->xfer.send_active != 0U))
			{
				dma_channel_disable (DMA1_PERIPH, uart->dma_tx->channel);
			}

			// If DMA mode - disable DMA channel
			if ((uart->dma_rx) && (uart->info->rx_status.rx_busy))
			{
				dma_channel_disable (DMA1_PERIPH, uart->dma_rx->channel);
			}

			// Clear driver variables
			uart->info->rx_status.rx_busy          = 0U;
			uart->info->rx_status.rx_overflow      = 0U;
			uart->info->rx_status.rx_break         = 0U;
			uart->info->rx_status.rx_framing_error = 0U;
			uart->info->rx_status.rx_parity_error  = 0U;
			uart->info->xfer.send_active           = 0U;

			uart->info->flags &= ~UART_FLAG_POWERED;
			break;

		case RY_POWER_LOW:
			return RY_DRIVER_ERROR_UNSUPPORTED;

		case RY_POWER_FULL:
			if ((uart->info->flags & UART_FLAG_INITIALIZED) == 0U)
			{
				return RY_DRIVER_ERROR;
			}

			if ((uart->info->flags & UART_FLAG_POWERED) != 0U)
			{
				return RY_DRIVER_OK;
			}

			// Disable interrupts
			uart->reg->UART_IER = 0U;

			// Configure FIFO Control register
			uart->reg->UART_FCR |= (uart->trig_lvl & UART_FCR_RXFIFO_TRGL_MASK) | UART_FCR_RFIFO_RESET | UART_FCR_TFIFO_RESET | UART_FCR_FIFO_EN;
			if (uart->dma_rx || uart->dma_tx)
			{
				uart->reg->UART_FCR |= UART_FCR_DMA_EN;
			}

			if (uart->capabilities.cts || uart->capabilities.dcd || uart->capabilities.dsr || uart->capabilities.ri)
			{
				uart->reg->UART_IER |= UART_IER_MS;
			}

			// Clear driver variables
			uart->info->rx_status.rx_busy          = 0U;
			uart->info->rx_status.rx_overflow      = 0U;
			uart->info->rx_status.rx_break         = 0U;
			uart->info->rx_status.rx_framing_error = 0U;
			uart->info->rx_status.rx_parity_error  = 0U;

			uart->info->mode                       = 0U;
			uart->info->flags                      = 0U;
			uart->info->xfer.send_active           = 0U;

			uart->info->flags = UART_FLAG_POWERED | UART_FLAG_INITIALIZED;

			// Priority must be set > 0 to trigger the interrupt
			__nds__plic_set_priority(uart->irq_num, 1);

			// Enable PLIC interrupt PIT source
			__nds__plic_enable_interrupt(uart->irq_num);
		break;

		default:
			return RY_DRIVER_ERROR_UNSUPPORTED;
	}

	return RY_DRIVER_OK;
}

/**
  \fn          int32_t uart_send (const void *data, uint32_t num, RY_UART_RESOURCES *uart)
  \brief       Start sending data to UART transmitter.
  \param[in]   data  Pointer to buffer with data to send to UART transmitter
  \param[in]   num   Number of data items to send
  \param[in]   uart Pointer to UART resources
  \return      \ref execution_status
*/
static int32_t uart_send(const void	*data, uint32_t num, RY_UART_RESOURCES *uart)
{
	int32_t stat, val;

	if ((data == NULL) || (num == 0U))
	{
		// Invalid parameters
		return RY_DRIVER_ERROR_PARAMETER;
	}

	if ((uart->info->flags & UART_FLAG_CONFIGURED) == 0U)
	{
		// UART is not configured (mode not selected)
		return RY_DRIVER_ERROR;
	}

	if (uart->info->xfer.send_active != 0U)
	{
		// Send is not completed yet
		return RY_DRIVER_ERROR_BUSY;
	}

	// Set Send active flag
	uart->info->xfer.send_active = 1U;

	// Save transmit buffer info
	uart->info->xfer.tx_buf = (uint8_t *)data;
	uart->info->xfer.tx_num = num;
	uart->info->xfer.tx_cnt = 0U;

	// DMA mode
	if (uart->dma_tx) {

		// Configure DMA channel
		stat = dma_channel_configure (DMA1_PERIPH, uart->dma_tx->channel,
						(unsigned long)data|0xa0000000,
						(unsigned long)(&(uart->reg->UART_THR)),
						num,
						DMA_CH_CTRL_SBINF(DMA_INF_IDX1)                 |
						DMA_CH_CTRL_DBINF(DMA_INF_IDX1)                 |
						DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)			|
						DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)		|
						DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)		|
						DMA_CH_CTRL_DMODE_HANDSHAKE			|
						DMA_CH_CTRL_SRCADDR_INC				|
						DMA_CH_CTRL_DSTADDR_FIX				|
						DMA_CH_CTRL_DSTREQ(uart->dma_tx->reqsel)	|
						DMA_CH_CTRL_EN_INTABT				|
						DMA_CH_CTRL_EN_INTERR				|
						DMA_CH_CTRL_EN_INTTC				|
						DMA_CH_CTRL_ENABLE,
						uart->dma_tx->cb_event);
		if (stat == -1)
		{
			return RY_DRIVER_ERROR;
		}

	// Interrupt mode
	}
	else
	{
		// Fill TX FIFO
		if (uart->reg->UART_LSR & UART_LSR_THRE)
		{
			val = 16U;
			while ((val--) && (uart->info->xfer.tx_cnt != uart->info->xfer.tx_num))
			{
				uart->reg->UART_THR = uart->info->xfer.tx_buf[uart->info->xfer.tx_cnt++];
			}
		}

		// Enable transmit holding register empty interrupt
		uart->reg->UART_IER |= UART_IER_THRE;
	}

	return RY_DRIVER_OK;
}

/**
  \fn          int32_t uart_receive (void *data, uint32_t num, RY_UART_RESOURCES *uart)
  \brief       Start receiving data from UART receiver.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver
  \param[in]   num   Number of data items to receive
  \param[in]   uart Pointer to UART resources
  \return      \ref execution_status
*/
static int32_t uart_receive(void *data, uint32_t num, RY_UART_RESOURCES *uart)
{
	int32_t stat;

	if ((data == NULL) || (num == 0U))
	{
		// Invalid parameters
		return RY_DRIVER_ERROR_PARAMETER;
	}

	if ((uart->info->flags & UART_FLAG_CONFIGURED) == 0U)
	{
		// UART is not configured (mode not selected)
		return RY_DRIVER_ERROR;
	}

	// Check if receiver is busy
	if (uart->info->rx_status.rx_busy == 1U)
	{
		return RY_DRIVER_ERROR_BUSY;
	}

	// Set RX busy flag
	uart->info->rx_status.rx_busy = 1U;

	// Save number of data to be received
	uart->info->xfer.rx_num = num;

	// Clear RX statuses
	uart->info->rx_status.rx_break          = 0U;
	uart->info->rx_status.rx_framing_error  = 0U;
	uart->info->rx_status.rx_overflow       = 0U;
	uart->info->rx_status.rx_parity_error   = 0U;

	// Save receive buffer info
	uart->info->xfer.rx_buf = (uint8_t *)data;
	uart->info->xfer.rx_cnt =  0U;

	// DMA mode
	if (uart->dma_rx) {

		stat = dma_channel_configure (DMA1_PERIPH, uart->dma_rx->channel,
						(unsigned long)&uart->reg->UART_RBR,
						(unsigned long)data|0xa0000000,
						num,
						DMA_CH_CTRL_SBINF(DMA_INF_IDX1)                 |
						DMA_CH_CTRL_DBINF(DMA_INF_IDX1)                 |
						DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)			|
						DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)		|
						DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)		|
						DMA_CH_CTRL_SMODE_HANDSHAKE			|
						DMA_CH_CTRL_SRCADDR_FIX				|
						DMA_CH_CTRL_DSTADDR_INC				|
						DMA_CH_CTRL_SRCREQ(uart->dma_rx->reqsel)	|
						DMA_CH_CTRL_EN_INTABT				|
						DMA_CH_CTRL_EN_INTERR				|
						DMA_CH_CTRL_EN_INTTC				|
						DMA_CH_CTRL_ENABLE,
						uart->dma_rx->cb_event);
		if (stat == -1)
		{
			return RY_UART_ERROR_CONFIG;
		}
	// Interrupt mode
	}
	else
	{
		// Enable receive data available interrupt
		uart->reg->UART_IER |= UART_IER_RDR;
	}

	return RY_DRIVER_OK;
}

/**
  \fn          int32_t uart_transfer (const void *data_out, void *data_in, uint32_t num, RY_UART_RESOURCES *uart)
  \brief       Start sending/receiving data to/from UART transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to UART transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from UART receiver
  \param[in]   num       Number of data items to transfer
  \param[in]   uart      Pointer to UART resources
  \return      \ref execution_status
*/
static int32_t uart_transfer(const void	*data_out, void	*data_in, uint32_t num, RY_UART_RESOURCES *uart)
{
	if ((data_out == NULL) || (data_in == NULL) || (num == 0U))
	{
		// Invalid parameters
		return RY_DRIVER_ERROR_PARAMETER;
	}

	if ((uart->info->flags & UART_FLAG_CONFIGURED) == 0U)
	{
		// UART is not configured
		return RY_DRIVER_ERROR;
	}

	// Only in synchronous mode
	return RY_DRIVER_ERROR;
}

/**
  \fn          uint32_t uart_get_txcount(RY_UART_RESOURCES *uart)
  \brief       Get transmitted data count.
  \param[in]   uart     Pointer to UART resources
  \return      number of data items transmitted
*/
static uint32_t uart_get_txcount(RY_UART_RESOURCES *uart)
{
	uint32_t cnt;

	if (uart->dma_tx)
	{
		cnt = dma_channel_get_count(DMA1_PERIPH, uart->dma_tx->channel);
	}
	else
	{
		cnt = uart->info->xfer.tx_cnt;
	}

	return cnt;
}

/**
  \fn          uint32_t uart_get_rxcount (RY_UART_RESOURCES *uart)
  \brief       Get received data count.
  \param[in]   uart     Pointer to UART resources
  \return      number of data items received
*/
static uint32_t uart_get_rxcount(RY_UART_RESOURCES *uart)
{
	uint32_t cnt;

	if (uart->dma_rx)
	{
		cnt = dma_channel_get_count(DMA1_PERIPH, uart->dma_rx->channel);
	}
	else
	{
		cnt = uart->info->xfer.rx_cnt;
	}

	return cnt;
}

/**
  \fn          int32_t uart_control(uint32_t control, uint32_t arg, RY_UART_RESOURCES  *uart)
  \brief       Control UART Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \param[in]   uart    Pointer to UART resources
  \return      common \ref execution_status and driver specific \ref uart_execution_status
*/
static int32_t uart_control(uint32_t control, uint32_t arg, RY_UART_RESOURCES *uart)
{
	uint32_t val, mode;
	uint32_t lcr, mcr;

	if ((uart->info->flags & UART_FLAG_POWERED) == 0U)
	{
		// UART not powered
		return RY_DRIVER_ERROR;
	}

	lcr = 0U;
	mcr = 0U;

	switch (control & RY_UART_CONTROL_Msk)
	{
		// Control TX
		case RY_UART_CONTROL_TX:
			// Check if TX line available
			if(arg)
			{
				uart->info->flags |= UART_FLAG_TX_ENABLED;
			}
			else
			{
				uart->info->flags &= ~UART_FLAG_TX_ENABLED;
			}
			return RY_DRIVER_OK;

		// Control RX
		case RY_UART_CONTROL_RX:
			// RX Line interrupt enable (overrun, framing, parity error, break)
			if(arg)
			{
				uart->info->flags |= UART_FLAG_RX_ENABLED;
				uart->reg->UART_IER |= UART_IER_RLS;
			}
			else
			{
				uart->info->flags &= ~UART_FLAG_RX_ENABLED;
				uart->reg->UART_IER &= ~UART_IER_RLS;
			}
			return RY_DRIVER_OK;

		// Control break
		case RY_UART_CONTROL_BREAK:
			if(arg)
			{
				if (uart->info->xfer.send_active != 0U)
				{
					return RY_DRIVER_ERROR_BUSY;
				}

				uart->reg->UART_LCR |= UART_LCR_SETBREAK;

				// Set Send active flag
				uart->info->xfer.send_active = 1U;
			}
			else
			{
				uart->reg->UART_LCR &= ~UART_LCR_SETBREAK;

				// Clear Send active flag
				uart->info->xfer.send_active = 0U;
			}
			return RY_DRIVER_OK;

		// Abort Send
		case RY_UART_ABORT_SEND:
			// Disable transmit holding register empty interrupt
			uart->reg->UART_IER &= ~UART_IER_THRE;

			// Set trigger level
			val  = (uart->trig_lvl & UART_FCR_RXFIFO_TRGL_MASK)|UART_FCR_FIFO_EN;
			if (uart->dma_rx || uart->dma_tx)
			{
				val |= UART_FCR_DMA_EN;
			}

			// Transmit FIFO reset
			val |= UART_FCR_TFIFO_RESET;
			uart->reg->UART_FCR = val;

			// If DMA mode - disable DMA channel
			if((uart->dma_tx) && (uart->info->xfer.send_active != 0U))
			{
				dma_channel_disable (DMA1_PERIPH, uart->dma_tx->channel);
			}

			// Clear Send active flag
			uart->info->xfer.send_active = 0U;
			return RY_DRIVER_OK;

			// Abort receive
			case RY_UART_ABORT_RECEIVE:
			// Disable receive data available interrupt
			uart->reg->UART_IER &= ~UART_IER_RDR;

			// Set trigger level
			val  = (uart->trig_lvl & UART_FCR_RXFIFO_TRGL_MASK)|UART_FCR_FIFO_EN;
			if (uart->dma_rx || uart->dma_tx)
			{
				val |= UART_FCR_DMA_EN;
			}

			// Receive FIFO reset
			val |= UART_FCR_RFIFO_RESET;
			uart->reg->UART_FCR = val;

			// If DMA mode - disable DMA channel
			if ((uart->dma_rx) && (uart->info->rx_status.rx_busy))
			{
				dma_channel_disable (DMA1_PERIPH, uart->dma_rx->channel);
			}

			// Clear RX busy status
			uart->info->rx_status.rx_busy = 0U;
			return RY_DRIVER_OK;

		// Abort transfer
		case RY_UART_ABORT_TRANSFER:
			// Disable transmit holding register empty and
			// receive data available interrupts
			uart->reg->UART_IER &= ~(UART_IER_THRE | UART_IER_RDR);

			// If DMA mode - disable DMA channel
			if((uart->dma_tx) && (uart->info->xfer.send_active != 0U))
			{
				dma_channel_disable (DMA1_PERIPH, uart->dma_tx->channel);
			}

			if((uart->dma_rx) && (uart->info->rx_status.rx_busy))
			{
				dma_channel_disable (DMA1_PERIPH, uart->dma_rx->channel);
			}

			// Set trigger level
			val = (uart->trig_lvl & UART_FCR_RXFIFO_TRGL_MASK) | UART_FCR_FIFO_EN;
			if(uart->dma_rx || uart->dma_tx)
			{
				val |= UART_FCR_DMA_EN;
			}

			// Transmit and receive FIFO reset
			val |= (UART_FCR_TFIFO_RESET | UART_FCR_RFIFO_RESET);
			uart->reg->UART_FCR = val;

			// Clear busy statuses
			uart->info->rx_status.rx_busy = 0U;
			uart->info->xfer.send_active  = 0U;
			return RY_DRIVER_OK;

		default:
			break;
	}

	switch (control & RY_UART_CONTROL_Msk)
	{
		case RY_UART_MODE_ASYNCHRONOUS:
			mode = RY_UART_MODE_ASYNCHRONOUS;
			break;
//		case RY_UART_MODE_SYNCHRONOUS_MASTER:
//		case RY_UART_MODE_SYNCHRONOUS_SLAVE:
//		case RY_UART_MODE_IRDA:
//		case RY_UART_MODE_SMART_CARD:
//		case RY_UART_MODE_SINGLE_WIRE:
//			return RY_UART_ERROR_MODE;

		// Default TX value
		case RY_UART_SET_DEFAULT_TX_VALUE:
			uart->info->xfer.tx_def_val = arg;
			return RY_DRIVER_OK;

		// Unsupported command
		default:
			return RY_DRIVER_ERROR_UNSUPPORTED;
	}

	// Check if Receiver/Transmitter is busy
	if (uart->info->rx_status.rx_busy || (uart->info->xfer.send_active != 0U))
	{
		return RY_DRIVER_ERROR_BUSY;
	}

	// UART Data bits
	switch (control & RY_UART_DATA_BITS_Msk)
	{
		case RY_UART_DATA_BITS_5:
			lcr |= UART_LCR_BITS5;
			break;
		case RY_UART_DATA_BITS_6:
			lcr |= UART_LCR_BITS6;
			break;
		case RY_UART_DATA_BITS_7:
			lcr |= UART_LCR_BITS7;
			break;
		case RY_UART_DATA_BITS_8:
			lcr |= UART_LCR_BITS8;
			break;
		default:
			return RY_UART_ERROR_DATA_BITS;
	}

	// UART Parity
	switch (control & RY_UART_PARITY_Msk)
	{
		case RY_UART_PARITY_NONE:
			break;
		case RY_UART_PARITY_EVEN:
			lcr |= UART_LCR_PARITY_EVEN;
			break;
		case RY_UART_PARITY_ODD:
			lcr |= UART_LCR_PARITY_ODD;
			break;
		default:
			return RY_UART_ERROR_PARITY;
	}

	// UART Stop bits
	switch (control & RY_UART_STOP_BITS_Msk)
	{
		case RY_UART_STOP_BITS_1:
			break;
		case RY_UART_STOP_BITS_2:
			lcr |= UART_LCR_STOP2;
			break;
		default:
			return RY_UART_ERROR_STOP_BITS;
	}

	mcr = uart->reg->UART_MCR & ~UART_MCR_AFE;

	switch (control & RY_UART_FLOW_CONTROL_Msk)
	{
		case RY_UART_FLOW_CONTROL_NONE:
			break;
		case RY_UART_FLOW_CONTROL_RTS:
			if(uart->capabilities.flow_control_rts)
			{
				mcr |= UART_MCR_AFE;
			}
			else
			{
				return RY_UART_ERROR_FLOW_CONTROL;
			}
			break;
		case RY_UART_FLOW_CONTROL_CTS:
			if(uart->capabilities.flow_control_cts)
			{
				mcr |= UART_MCR_AFE;
			}
			else
			{
				return RY_UART_ERROR_FLOW_CONTROL;
			}
			break;
		case RY_UART_FLOW_CONTROL_RTS_CTS:
			if (uart->capabilities.flow_control_rts && uart->capabilities.flow_control_cts)
			{
				mcr |= UART_MCR_AFE;
			}
			else
			{
				return RY_UART_ERROR_FLOW_CONTROL;
			}
			break;
		default:
			return RY_UART_ERROR_FLOW_CONTROL;
	}

	// UART Baudrate
	if (uart_set_baudrate(arg, uart) == -1)
	{
		return RY_UART_ERROR_BAUDRATE;
	}

	// Configuration is OK - Mode is valid
	uart->info->mode = mode;

	// Configure MCR register
	uart->reg->UART_MCR = (uart->reg->UART_MCR & ~UART_MCR_AFE) | mcr;

	// Configure Line control register
	uart->reg->UART_LCR = ((uart->reg->UART_LCR & (UART_LCR_SETBREAK | UART_LCR_DLAB)) | lcr);

	// Set configured flag
	uart->info->flags |= UART_FLAG_CONFIGURED;

	return RY_DRIVER_OK;
}

/**
  \fn          RY_UART_STATUS uart_get_status(RY_UART_RESOURCES *uart)
  \brief       Get UART status.
  \param[in]   uart     Pointer to UART resources
  \return      UART status \ref RY_UART_STATUS
*/
static RY_UART_STATUS uart_get_status(RY_UART_RESOURCES *uart)
{
	RY_UART_STATUS stat;

	stat.tx_busy          = (uart->reg->UART_LSR & UART_LSR_TEMT ? (0U) : (1U));
	stat.rx_busy          = uart->info->rx_status.rx_busy;
	stat.tx_underflow     = 0U;
	stat.rx_overflow      = uart->info->rx_status.rx_overflow;
	stat.rx_break         = uart->info->rx_status.rx_break;
	stat.rx_framing_error = uart->info->rx_status.rx_framing_error;
	stat.rx_parity_error  = uart->info->rx_status.rx_parity_error;

	return stat;
}

/**
  \fn          int32_t uart_set_modem_control(RY_UART_MODEM_CONTROL control, RY_UART_RESOURCES *uart)
  \brief       Set UART Modem Control line state.
  \param[in]   control   \ref RY_UART_MODEM_CONTROL
  \param[in]   uart     Pointer to UART resources
  \return      \ref execution_status
*/
static int32_t uart_set_modem_control(RY_UART_MODEM_CONTROL control, RY_UART_RESOURCES *uart)
{
	if ((uart->info->flags & UART_FLAG_CONFIGURED) == 0U)
	{
		// UART is not configured
		return RY_DRIVER_ERROR;
	}

	if(control == RY_UART_RTS_CLEAR)
	{
		if(uart->capabilities.rts)
		{
			uart->reg->UART_MCR &= ~UART_MCR_RTS;
		}
		else
		{
			return RY_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	if(control == RY_UART_RTS_SET)
	{
		if(uart->capabilities.rts)
		{
			uart->reg->UART_MCR |=  UART_MCR_RTS;
		}
		else
		{
			return RY_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	if(control == RY_UART_DTR_CLEAR)
	{
		if(uart->capabilities.dtr)
		{
			uart->reg->UART_MCR &= ~UART_MCR_DTR;
		}
		else
		{
			return RY_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	if(control == RY_UART_DTR_SET)
	{
		if(uart->capabilities.dtr)
		{
			uart->reg->UART_MCR |=  UART_MCR_DTR;
		}
		else
		{
			return RY_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	return RY_DRIVER_OK;
}

/**
  \fn          RY_UART_MODEM_STATUS uart_get_modem_status(RY_UART_RESOURCES *uart)
  \brief       Get UART Modem Status lines state.
  \param[in]   uart     Pointer to UART resources
  \return      modem status \ref RY_UART_MODEM_STATUS
*/
static RY_UART_MODEM_STATUS uart_get_modem_status(RY_UART_RESOURCES *uart)
{
	RY_UART_MODEM_STATUS modem_status;
	uint32_t msr;

	if(uart->info->flags & UART_FLAG_CONFIGURED)
	{
		msr = uart->reg->UART_MSR;

		if(uart->capabilities.cts)
		{
			modem_status.cts = (msr & UART_MSR_CTS ? (1U) : (0U));
		}
		else
		{
			modem_status.cts = 0U;
		}

		if(uart->capabilities.dsr)
		{
			modem_status.dsr = (msr & UART_MSR_DSR ? (1U) : (0U));
		}
		else
		{
			modem_status.dsr = 0U;
		}

		if(uart->capabilities.ri)
		{
			modem_status.ri  = (msr & UART_MSR_RI ? (1U) : (0U));
		}
		else
		{
			modem_status.ri  = 0U;
		}

		if (uart->capabilities.dcd)
		{
			modem_status.dcd = (msr & UART_MSR_DCD ? (1U) : (0U));
		}
		else
		{
			modem_status.dcd = 0U;
		}

	}
	else
	{
		modem_status.cts = 0U;
		modem_status.dsr = 0U;
		modem_status.ri  = 0U;
		modem_status.dcd = 0U;
	}

	return modem_status;
}

/**
  \fn          void uart_irq_handler(RY_UART_RESOURCES *uart)
  \brief       UART Interrupt handler.
  \param[in]   uart Pointer to UART resources
*/
static void uart_irq_handler(RY_UART_RESOURCES *uart)
{
	uint32_t iir, event, val, i;

	event = 0U;
	iir = uart->reg->UART_IIR;

	if((iir & UART_IIR_NONE) == 0U)
	{
		// Transmit holding register empty
		if((iir & UART_IIR_INT_MASK) == UART_IIR_THRE)
		{
			val = 16U;
			while((val --) && (uart->info->xfer.tx_num != uart->info->xfer.tx_cnt))
			{
				// Write data to Tx FIFO
				uart->reg->UART_THR = uart->info->xfer.tx_buf[uart->info->xfer.tx_cnt];
				uart->info->xfer.tx_cnt++;
			}

			// Check if all data is transmitted
			if (uart->info->xfer.tx_num == uart->info->xfer.tx_cnt)
			{
				// Disable THRE interrupt
				uart->reg->UART_IER &= ~UART_IER_THRE;

				// Clear TX busy flag
				uart->info->xfer.send_active = 0U;

				// Set send complete event
				event |= RY_UART_EVENT_SEND_COMPLETE;
			}
		}

		// Receive line status
		if((iir & UART_IIR_INT_MASK) == UART_IIR_RLS)
		{
			event |= uart_rxline_irq_handler(uart);
		}

		// Receive data available and Character time-out indicator interrupt
		if (((iir & UART_IIR_INT_MASK) == UART_IIR_RDA)||((iir & UART_IIR_INT_MASK) == UART_IIR_RTO))
		{
			switch(uart->trig_lvl)
			{
				case UART_TRIG_LVL_1:
					i = 1U;
					break;
				case UART_TRIG_LVL_4:
					i = 3U;
					break;
				case UART_TRIG_LVL_8:
					i = 7U;
					break;
				case UART_TRIG_LVL_14:
					i = 13U;
					break;
				default:
					i = 1U;
			}

			// Get available data from RX FIFO
			while((uart->reg->UART_LSR & UART_LSR_RDR) && (i--))
			{
				// Check RX line interrupt for errors
				event |= uart_rxline_irq_handler(uart);

				// Read data from RX FIFO into receive buffer
				uart->info->xfer.rx_buf[uart->info->xfer.rx_cnt] = uart->reg->UART_RBR;

				uart->info->xfer.rx_cnt++;

				// Check if requested amount of data is received
				if(uart->info->xfer.rx_cnt == uart->info->xfer.rx_num)
				{
					// Disable RDA interrupt
					uart->reg->UART_IER &= ~UART_IER_RDR;

					// Clear RX busy flag and set receive transfer complete event
					uart->info->rx_status.rx_busy = 0U;
					event |= RY_UART_EVENT_RECEIVE_COMPLETE;

					break;
				}
			}
		}

		// Character time-out indicator
		if((iir & UART_IIR_INT_MASK) == UART_IIR_RTO)
		{
			// Signal RX Time-out event, if not all requested data received
			if(uart->info->xfer.rx_cnt != uart->info->xfer.rx_num)
			{
				event |= RY_UART_EVENT_RX_TIMEOUT;
			}
		}

		// Modem interrupt
		if((iir & UART_IIR_INT_MASK) == UART_IIR_MODEM)
		{
			// Save modem status register
			val = uart->reg->UART_MSR;

			// CTS state changed
			if((uart->capabilities.cts) && (val & UART_MSR_DCTS))
			{
				event |= RY_UART_EVENT_CTS;
			}

			// DSR state changed
			if((uart->capabilities.dsr) && (val & UART_MSR_DDSR))
			{
				event |= RY_UART_EVENT_DSR;
			}

			// Ring indicator
			if((uart->capabilities.ri)  && (val & UART_MSR_TERI))
			{
				event |= RY_UART_EVENT_RI;
			}

			// DCD state changed
			if((uart->capabilities.dcd) && (val & UART_MSR_DDCD))
			{
				event |= RY_UART_EVENT_DCD;
			}

		}
	}

	if((uart->info->cb_event != NULL) && (event != 0U))
	{
		uart->info->cb_event(event);
	}
}

//static uint8_t uart_getc(RY_UART_RESOURCES *uart)
//{
//	while((uart->reg->LSR & UART_LSR_RDR) == 0);
//	return uart->reg->RBR;
//}

static void uart_putc(uint8_t data, RY_UART_RESOURCES *uart)
{
	while((uart->reg->UART_LSR & UART_LSR_THRE) == 0);
	uart->reg->UART_THR = data;
}

static void uart_puts(const char *s, uint8_t size, RY_UART_RESOURCES *uart)
{
	while(size--)
	{
		uart_putc(*s++, uart);
	}
}

#if (((GTRY_UART0_EN) && (GTRY_UART0_DMA_TX_EN == 1)) || \
     ((GTRY_UART1_EN) && (GTRY_UART1_DMA_TX_EN == 1)) || \
     ((GTRY_UART2_EN) && (GTRY_UART2_DMA_TX_EN == 1)))
/**
  \fn          void uart_dma_tx_event (uint32_t event, RY_UART_RESOURCES *uart)
  \brief       UART Interrupt handler.
  \param[in]   uart Pointer to UART resources
  \param[in]   event DMA_EVENT_TC / DMA_EVENT_ERROR
*/
static void uart_dma_tx_event(uint32_t event, RY_UART_RESOURCES *uart)
{
	switch(event)
	{
		case DMA_EVENT_TC:
			uart->info->xfer.tx_cnt = uart->info->xfer.tx_num;
			// Clear TX busy flag
			uart->info->xfer.send_active = 0U;

			// Set Send Complete event for asynchronous transfers
			if(uart->info->cb_event)
			{
				uart->info->cb_event(RY_UART_EVENT_SEND_COMPLETE);
			}
			break;
		case DMA_EVENT_ERROR:
		default:
			break;
	}
}
#endif

#if (((GTRY_UART0_EN) && (GTRY_UART0_DMA_RX_EN == 1)) || \
     ((GTRY_UART1_EN) && (GTRY_UART1_DMA_RX_EN == 1)) || \
     ((GTRY_UART2_EN) && (GTRY_UART2_DMA_RX_EN == 1)))
/**
  \fn          void uart_dma_rx_event (uint32_t event, RY_UART_RESOURCES *uart)
  \brief       UART Interrupt handler.
  \param[in]   event DMA_EVENT_TC / DMA_EVENT_ERROR
  \param[in]   uart  Pointer to UART resources
*/
static void uart_dma_rx_event(uint32_t event, RY_UART_RESOURCES *uart)
{
	switch(event)
	{
		case DMA_EVENT_TC:
			uart->info->xfer.rx_cnt = uart->info->xfer.rx_num;
			uart->info->rx_status.rx_busy = 0U;

			if(uart->info->cb_event)
			{
				uart->info->cb_event(RY_UART_EVENT_RECEIVE_COMPLETE);
			}
			break;
		case DMA_EVENT_ERROR:
		default:
			break;
	}
}
#endif

#if (GTRY_UART0_EN)
// uart0 Driver Wrapper functions
static RY_UART_CAPABILITIES uart0_get_capabilities(void)
{
	return uart_get_capabilities(&uart0_resources);
}
static int32_t uart0_initialize(p_uart_int_cb cb_event)
{
	return uart_initialize(cb_event, &uart0_resources);
}
static int32_t uart0_uninitialize(void)
{
	return uart_uninitialize(&uart0_resources);
}
static int32_t uart0_power_control(RY_POWER_STATE state)
{
	return uart_power_control(state, &uart0_resources);
}
static int32_t uart0_send(const void *data, uint32_t num)
{
	return uart_send(data, num, &uart0_resources);
}
static int32_t uart0_receive(void *data, uint32_t num)
{
	return uart_receive(data, num, &uart0_resources);
}
static int32_t uart0_transfer(const void *data_out, void *data_in, uint32_t num)
{
	return uart_transfer(data_out, data_in, num, &uart0_resources);
}
static uint32_t uart0_get_txcount(void)
{
	return uart_get_txcount(&uart0_resources);
}
static uint32_t uart0_get_rxcount(void)
{
	return uart_get_rxcount(&uart0_resources);
}
static int32_t uart0_control(uint32_t control, uint32_t arg)
{
	return uart_control(control, arg, &uart0_resources);
}
static RY_UART_STATUS uart0_get_status(void)
{
	return uart_get_status(&uart0_resources);
}
static int32_t uart0_set_modem_control(RY_UART_MODEM_CONTROL control)
{
	return uart_set_modem_control(control, &uart0_resources);
}
static RY_UART_MODEM_STATUS uart0_get_modem_status(void)
{
	return uart_get_modem_status(&uart0_resources);
}

/*--------*/
void uart0_irq_handler(void)
{
	uart_irq_handler(&uart0_resources);
}

#if (GTRY_UART0_DMA_TX_EN == 1)
void uart0_dma_tx_event (uint32_t event)
{
	uart_dma_tx_event(event, &uart0_resources);
}
#endif

#if (GTRY_UART0_DMA_RX_EN == 1)
void uart0_dma_rx_event(uint32_t event)
{
	uart_dma_rx_event(event, &uart0_resources);
}
#endif

// UART0 Driver Control Block
RY_DRIVER_UART Driver_UART0 = {
	uart0_get_capabilities,
	uart0_initialize,
	uart0_uninitialize,
	uart0_power_control,
	uart0_send,
	uart0_receive,
	uart0_transfer,
	uart0_get_txcount,
	uart0_get_rxcount,
	uart0_control,
	uart0_get_status,
	uart0_set_modem_control,
	uart0_get_modem_status
};
#endif

#if (GTRY_UART1_EN)
// UART1 Driver Wrapper functions
static RY_UART_CAPABILITIES uart1_get_capabilities(void)
{
	return uart_get_capabilities(&uart1_resources);
}
static int32_t uart1_initialize(p_uart_int_cb cb_event)
{
	return uart_initialize(cb_event, &uart1_resources);
}
static int32_t uart1_uninitialize(void)
{
	return uart_uninitialize(&uart1_resources);
}
static int32_t uart1_power_control(RY_POWER_STATE state)
{
	return uart_power_control (state, &uart1_resources);
}
static int32_t uart1_send(const void *data, uint32_t num)
{
	return uart_send (data, num, &uart1_resources);
}
static int32_t uart1_receive(void *data, uint32_t num)
{
	return uart_receive(data, num, &uart1_resources);
}
static int32_t uart1_transfer(const void *data_out, void *data_in, uint32_t	num)
{
	return uart_transfer(data_out, data_in, num, &uart1_resources);
}
static uint32_t uart1_get_txcount(void)
{
	return uart_get_txcount(&uart1_resources);
}
static uint32_t uart1_get_rxcount(void)
{
	return uart_get_rxcount(&uart1_resources);
}
static int32_t uart1_control(uint32_t control, uint32_t arg)
{
	return uart_control(control, arg, &uart1_resources);
}
static RY_UART_STATUS uart1_get_status(void)
{
	return uart_get_status(&uart1_resources);
}
static int32_t uart1_set_modem_control(RY_UART_MODEM_CONTROL control)
{
	return uart_set_modem_control(control, &uart1_resources);
}
static RY_UART_MODEM_STATUS uart1_get_modem_status(void)
{
	return uart_get_modem_status(&uart1_resources);
}

/*--------*/
void uart1_irq_handler(void)
{
	uart_irq_handler(&uart1_resources);
}

#if (GTRY_UART1_DMA_TX_EN == 1)
void uart1_dma_tx_event(uint32_t event)
{
	uart_dma_tx_event(event, &uart1_resources);
}
#endif

#if (GTRY_UART1_DMA_RX_EN == 1)
void uart1_dma_rx_event(uint32_t event)
{
	uart_dma_rx_event(event, &uart1_resources);
}
#endif

// UART1 Driver Control Block
RY_DRIVER_UART Driver_UART1 = {
	uart1_get_capabilities,
	uart1_initialize,
	uart1_uninitialize,
	uart1_power_control,
	uart1_send,
	uart1_receive,
	uart1_transfer,
	uart1_get_txcount,
	uart1_get_rxcount,
	uart1_control,
	uart1_get_status,
	uart1_set_modem_control,
	uart1_get_modem_status
};
#endif

#if (GTRY_UART2_EN)
// UART2 Driver Wrapper functions
static RY_UART_CAPABILITIES uart2_get_capabilities(void)
{
	return uart_get_capabilities (&uart2_resources);
}
static int32_t uart2_initialize(p_uart_int_cb cb_event)
{
	return uart_initialize(cb_event, &uart2_resources);
}
static int32_t uart2_uninitialize(void)
{
	return uart_uninitialize(&uart2_resources);
}
static int32_t uart2_power_control(RY_POWER_STATE state)
{
	return uart_power_control(state, &uart2_resources);
}
static int32_t uart2_send(const void *data, uint32_t num)
{
	return uart_send(data, num, &uart2_resources);
}
static int32_t uart2_receive(void *data, uint32_t num)
{
	return uart_receive(data, num, &uart2_resources);
}
static int32_t uart2_transfer(const void *data_out, void *data_in, uint32_t	num)
{
	return uart_transfer(data_out, data_in, num, &uart2_resources);
}
static uint32_t uart2_get_txcount(void)
{
	return uart_get_txcount(&uart2_resources);
}
static uint32_t uart2_get_rxcount(void)
{
	return uart_get_rxcount(&uart2_resources);
}
static int32_t uart2_control(uint32_t control, uint32_t arg)
{
	return uart_control(control, arg, &uart2_resources);
}
static RY_UART_STATUS uart2_get_status(void)
{
	return uart_get_status(&uart2_resources);
}
static int32_t uart2_set_modem_control(RY_UART_MODEM_CONTROL control)
{
	return uart_set_modem_control(control, &uart2_resources);
}
static RY_UART_MODEM_STATUS uart2_get_modem_status(void)
{
	return uart_get_modem_status(&uart2_resources);
}

/*--------*/
void uart2_irq_handler(void)
{
	uart_irq_handler(&uart2_resources);
}

#if (GTRY_UART2_DMA_TX_EN == 1)
void uart2_dma_tx_event (uint32_t event)
{
	uart_dma_tx_event(event, &uart2_resources);
}
#endif

#if (GTRY_UART2_DMA_RX_EN == 1)
void uart2_dma_rx_event (uint32_t event)
{
	uart_dma_rx_event(event, &uart2_resources);
}
#endif

// UART2 Driver Control Block
RY_DRIVER_UART Driver_UART2 = {
	uart2_get_capabilities,
	uart2_initialize,
	uart2_uninitialize,
	uart2_power_control,
	uart2_send,
	uart2_receive,
	uart2_transfer,
	uart2_get_txcount,
	uart2_get_rxcount,
	uart2_control,
	uart2_get_status,
	uart2_set_modem_control,
	uart2_get_modem_status
};
#endif

int _write(int fd, char *pBuffer, int size)
{
#if (GTRY_UART0_EN && (UART_PRINT_NO == 0))
	uart_puts(pBuffer, (uint8_t)size, &uart0_resources);
#elif (GTRY_UART1_EN && UART_PRINT_NO == 1)
	uart_puts(pBuffer, (uint8_t)size, &uart1_resources);
#elif (GTRY_UART2_EN && (UART_PRINT_NO == 2))
	uart_puts(pBuffer, (uint8_t)size, &uart2_resources);
#endif
    return size;
}

