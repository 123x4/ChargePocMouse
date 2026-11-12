/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

#ifndef __GTRY_UART_DRV_H__
#define __GTRY_UART_DRV_H__


#include "gtry_platform.h"
#include "gtry_drv_common.h"
#include "gtry_dma_drv.h"

/****** UART Control Codes *****/

#define		RY_UART_CONTROL_Pos					0
#define		RY_UART_CONTROL_Msk					(0xFFUL << RY_UART_CONTROL_Pos)
//
///*----- UART Control Codes: Mode -----*/
#define		RY_UART_MODE_ASYNCHRONOUS			(0x01UL << RY_UART_CONTROL_Pos)   ///< UART (Asynchronous); arg = Baudrate
//#define	RY_UART_MODE_SYNCHRONOUS_MASTER		(0x02UL << RY_UART_CONTROL_Pos)   ///< Synchronous Master (generates clock signal); arg = Baudrate
//#define	RY_UART_MODE_SYNCHRONOUS_SLAVE		(0x03UL << RY_UART_CONTROL_Pos)   ///< Synchronous Slave (external clock signal)
//#define	RY_UART_MODE_SINGLE_WIRE			(0x04UL << RY_UART_CONTROL_Pos)   ///< UART Single-wire (half-duplex); arg = Baudrate
//#define	RY_UART_MODE_IRDA					(0x05UL << RY_UART_CONTROL_Pos)   ///< UART IrDA; arg = Baudrate
//#define	RY_UART_MODE_SMART_CARD				(0x06UL << RY_UART_CONTROL_Pos)   ///< UART Smart Card; arg = Baudrate

/*----- UART Control Codes: Mode Parameters: Data Bits -----*/
#define		RY_UART_DATA_BITS_Pos				8
#define		RY_UART_DATA_BITS_Msk				(7UL << RY_UART_DATA_BITS_Pos)
#define		RY_UART_DATA_BITS_5					(5UL << RY_UART_DATA_BITS_Pos)    ///< 5 Data bits
#define		RY_UART_DATA_BITS_6					(6UL << RY_UART_DATA_BITS_Pos)    ///< 6 Data bit
#define		RY_UART_DATA_BITS_7					(7UL << RY_UART_DATA_BITS_Pos)    ///< 7 Data bits
#define		RY_UART_DATA_BITS_8					(0UL << RY_UART_DATA_BITS_Pos)    ///< 8 Data bits (default)
#define		RY_UART_DATA_BITS_9					(1UL << RY_UART_DATA_BITS_Pos)    ///< 9 Data bits

/*----- UART Control Codes: Mode Parameters: Parity -----*/
#define		RY_UART_PARITY_Pos					12
#define		RY_UART_PARITY_Msk					(3UL << RY_UART_PARITY_Pos)
#define		RY_UART_PARITY_NONE					(0UL << RY_UART_PARITY_Pos)       ///< No Parity (default)
#define		RY_UART_PARITY_EVEN					(1UL << RY_UART_PARITY_Pos)       ///< Even Parity
#define		RY_UART_PARITY_ODD					(2UL << RY_UART_PARITY_Pos)       ///< Odd Parity

/*----- UART Control Codes: Mode Parameters: Stop Bits -----*/
#define		RY_UART_STOP_BITS_Pos				14
#define		RY_UART_STOP_BITS_Msk				(3UL << RY_UART_STOP_BITS_Pos)
#define		RY_UART_STOP_BITS_1					(0UL << RY_UART_STOP_BITS_Pos)    ///< 1 Stop bit (default)
#define		RY_UART_STOP_BITS_2					(1UL << RY_UART_STOP_BITS_Pos)    ///< 2 Stop bits
#define		RY_UART_STOP_BITS_1_5				(2UL << RY_UART_STOP_BITS_Pos)    ///< 1.5 Stop bits
#define		RY_UART_STOP_BITS_0_5				(3UL << RY_UART_STOP_BITS_Pos)    ///< 0.5 Stop bits

/*----- UART Control Codes: Mode Parameters: Flow Control -----*/
#define		RY_UART_FLOW_CONTROL_Pos			16
#define		RY_UART_FLOW_CONTROL_Msk			(3UL << RY_UART_FLOW_CONTROL_Pos)
#define		RY_UART_FLOW_CONTROL_NONE			(0UL << RY_UART_FLOW_CONTROL_Pos) ///< No Flow Control (default)
#define		RY_UART_FLOW_CONTROL_RTS			(1UL << RY_UART_FLOW_CONTROL_Pos) ///< RTS Flow Control
#define		RY_UART_FLOW_CONTROL_CTS			(2UL << RY_UART_FLOW_CONTROL_Pos) ///< CTS Flow Control
#define		RY_UART_FLOW_CONTROL_RTS_CTS		(3UL << RY_UART_FLOW_CONTROL_Pos) ///< RTS/CTS Flow Control

/*----- UART Control Codes: Mode Parameters: Clock Polarity (Synchronous mode) -----*/
#define		RY_UART_CPOL_Pos					18
#define		RY_UART_CPOL_Msk                  	(1UL << RY_UART_CPOL_Pos)
#define		RY_UART_CPOL0                     	(0UL << RY_UART_CPOL_Pos)         ///< CPOL = 0 (default)
#define		RY_UART_CPOL1                     	(1UL << RY_UART_CPOL_Pos)         ///< CPOL = 1

/*----- UART Control Codes: Mode Parameters: Clock Phase (Synchronous mode) -----*/
#define		RY_UART_CPHA_Pos                   	19
#define		RY_UART_CPHA_Msk                  	(1UL << RY_UART_CPHA_Pos)
#define		RY_UART_CPHA0                     	(0UL << RY_UART_CPHA_Pos)         ///< CPHA = 0 (default)
#define		RY_UART_CPHA1                    	(1UL << RY_UART_CPHA_Pos)         ///< CPHA = 1

/*----- UART Control Codes: Miscellaneous Controls  -----*/
#define		RY_UART_SET_DEFAULT_TX_VALUE      	(0x10UL << RY_UART_CONTROL_Pos)   ///< Set default Transmit value (Synchronous Receive only); arg = value
//#define	RY_UART_SET_IRDA_PULSE				(0x11UL << RY_UART_CONTROL_Pos)   ///< Set IrDA Pulse in ns; arg: 0=3/16 of bit period
//#define	RY_UART_SET_SMART_CARD_GUARD_TIME	(0x12UL << RY_UART_CONTROL_Pos)   ///< Set Smart Card Guard Time; arg = number of bit periods
//#define	RY_UART_SET_SMART_CARD_CLOCK		(0x13UL << RY_UART_CONTROL_Pos)   ///< Set Smart Card Clock in Hz; arg: 0=Clock not generated
//#define	RY_UART_CONTROL_SMART_CARD_NACK		(0x14UL << RY_UART_CONTROL_Pos)   ///< Smart Card NACK generation; arg: 0=disabled, 1=enabled
#define		RY_UART_CONTROL_TX                	(0x15UL << RY_UART_CONTROL_Pos)   ///< Transmitter; arg: 0=disabled, 1=enabled
#define		RY_UART_CONTROL_RX                	(0x16UL << RY_UART_CONTROL_Pos)   ///< Receiver; arg: 0=disabled, 1=enabled
#define 	RY_UART_CONTROL_BREAK             	(0x17UL << RY_UART_CONTROL_Pos)   ///< Continuous Break transmission; arg: 0=disabled, 1=enabled
#define 	RY_UART_ABORT_SEND                	(0x18UL << RY_UART_CONTROL_Pos)   ///< Abort \ref RY_UART_Send
#define 	RY_UART_ABORT_RECEIVE            	(0x19UL << RY_UART_CONTROL_Pos)   ///< Abort \ref RY_UART_Receive
#define 	RY_UART_ABORT_TRANSFER           	(0x1AUL << RY_UART_CONTROL_Pos)   ///< Abort \ref RY_UART_Transfer


/****** UART specific error codes *****/
#define 	RY_UART_SUCCESS                		(RY_UART_DRIVER_RET_MASK | 0x0UL)	//< Specified Mode not supported
#define 	RY_UART_ERROR_MODE                	(RY_UART_DRIVER_RET_MASK | 0x1UL)	//< Specified Mode not supported
#define 	RY_UART_ERROR_BAUDRATE            	(RY_UART_DRIVER_RET_MASK | 0x2UL)	//< Specified baudrate not supported
#define 	RY_UART_ERROR_DATA_BITS           	(RY_UART_DRIVER_RET_MASK | 0x3UL)	//< Specified number of Data bits not supported
#define 	RY_UART_ERROR_PARITY              	(RY_UART_DRIVER_RET_MASK | 0x4UL)	//< Specified Parity not supported
#define 	RY_UART_ERROR_STOP_BITS           	(RY_UART_DRIVER_RET_MASK | 0x5UL)	//< Specified number of Stop bits not supported
#define 	RY_UART_ERROR_FLOW_CONTROL    		(RY_UART_DRIVER_RET_MASK | 0x6UL)	//< Specified Flow Control not supported
#define 	RY_UART_ERROR_BUSY       			(RY_UART_DRIVER_RET_MASK | 0x7UL)	//< Specified Mode not supported
#define 	RY_UART_ERROR_PARAM                	(RY_UART_DRIVER_RET_MASK | 0x8UL)	//< Specified Mode not supported
#define 	RY_UART_ERROR_CONFIG                (RY_UART_DRIVER_RET_MASK | 0x9UL)	//< Specified Mode not supported
#define 	RY_UART_ERROR_COMM                	(RY_UART_DRIVER_RET_MASK | 0xaUL)	//< Specified Mode not supported
#define 	RY_UART_ERROR_UNSUPPORTED      		(RY_UART_DRIVER_RET_MASK | 0xbUL)	//< Specified Mode not supported

/****** UART Event *****/
#define 	RY_UART_EVENT_SEND_COMPLETE       	(1UL << 0)		//< Send completed; however UART may still transmit data
#define 	RY_UART_EVENT_RECEIVE_COMPLETE   	(1UL << 1)		//< Receive completed
#define 	RY_UART_EVENT_TRANSFER_COMPLETE  	(1UL << 2)		//< Transfer completed
#define 	RY_UART_EVENT_TX_COMPLETE         	(1UL << 3)		//< Transmit completed (optional)
#define 	RY_UART_EVENT_TX_UNDERFLOW        	(1UL << 4)		//< Transmit data not available (Synchronous Slave)
#define 	RY_UART_EVENT_RX_OVERFLOW         	(1UL << 5)		//< Receive data overflow
#define 	RY_UART_EVENT_RX_TIMEOUT          	(1UL << 6)		//< Receive character timeout (optional)
#define 	RY_UART_EVENT_RX_BREAK            	(1UL << 7)		//< Break detected on receive
#define 	RY_UART_EVENT_RX_FRAMING_ERROR    	(1UL << 8)		//< Framing error detected on receive
#define 	RY_UART_EVENT_RX_PARITY_ERROR    	(1UL << 9)		//< Parity error detected on receive
#define 	RY_UART_EVENT_CTS                 	(1UL << 10)		//< CTS state changed (optional)
#define 	RY_UART_EVENT_DSR                	(1UL << 11)		//< DSR state changed (optional)
#define 	RY_UART_EVENT_DCD                 	(1UL << 12)		//< DCD state changed (optional)
#define 	RY_UART_EVENT_RI                  	(1UL << 13)		//< RI  state changed (optional)


typedef void (*p_uart_int_cb) (uint32_t event);  	//< Pointer to \ref p_uart_int_cb : UART Event.

/**
\brief UART Status
*/
typedef struct _RY_UART_STATUS {
  uint32_t tx_busy          : 1;        	//< Transmitter busy flag
  uint32_t rx_busy          : 1;        	//< Receiver busy flag
  uint32_t tx_underflow     : 1;        	//< Transmit data underflow detected (cleared on start of next send operation)
  uint32_t rx_overflow      : 1;       		//< Receive data overflow detected (cleared on start of next receive operation)
  uint32_t rx_break         : 1;        	//< Break detected on receive (cleared on start of next receive operation)
  uint32_t rx_framing_error : 1;			//< Framing error detected on receive (cleared on start of next receive operation)
  uint32_t rx_parity_error  : 1;			//< Parity error detected on receive (cleared on start of next receive operation)
} RY_UART_STATUS;

/**
\brief UART Modem Control
*/
typedef enum _RY_UART_MODEM_CONTROL {
	RY_UART_RTS_CLEAR,				//< Deactivate RTS
	RY_UART_RTS_SET,				//< Activate RTS
	RY_UART_DTR_CLEAR,				//< Deactivate DTR
	RY_UART_DTR_SET					//< Activate DTR
} RY_UART_MODEM_CONTROL;

/**
\brief UART Modem Status
*/
typedef struct _RY_UART_MODEM_STATUS {
  uint32_t cts : 1;					//< CTS state: 1=Active, 0=Inactive
  uint32_t dsr : 1;					//< DSR state: 1=Active, 0=Inactive
  uint32_t dcd : 1;					//< DCD state: 1=Active, 0=Inactive
  uint32_t ri  : 1;					//< RI  state: 1=Active, 0=Inactive
} RY_UART_MODEM_STATUS;

/**
\brief UART Device Driver Capabilities.
*/
typedef struct _RY_UART_CAPABILITIES {
//  uint32_t asynchronous       : 1;			//< supports UART (Asynchronous) mode
//  uint32_t synchronous_master : 1;			//< supports Synchronous Master mode
//  uint32_t synchronous_slave  : 1;			//< supports Synchronous Slave mode
//  uint32_t single_wire        : 1;			//< supports UART Single-wire mode
//  uint32_t irda               : 1;			//< supports UART IrDA mode
//  uint32_t smart_card         : 1;			//< supports UART Smart Card mode
//  uint32_t smart_card_clock   : 1;			//< Smart Card Clock generator available
  uint32_t flow_control_rts   : 1;				//< RTS Flow Control available
  uint32_t flow_control_cts   : 1;				//< CTS Flow Control available
  uint32_t event_tx_complete  : 1;				//< Transmit completed event: \ref RY_UART_EVENT_TX_COMPLETE
  uint32_t event_rx_timeout   : 1;				//< Signal receive character timeout event: \ref RY_UART_EVENT_RX_TIMEOUT
  uint32_t rts                : 1;				//< RTS Line: 0=not available, 1=available
  uint32_t cts                : 1;				//< CTS Line: 0=not available, 1=available
  uint32_t dtr                : 1;				//< DTR Line: 0=not available, 1=available
  uint32_t dsr                : 1;				//< DSR Line: 0=not available, 1=available
  uint32_t dcd                : 1;				//< DCD Line: 0=not available, 1=available
  uint32_t ri                 : 1;				//< RI Line: 0=not available, 1=available
  uint32_t event_cts          : 1;				//< Signal CTS change event: \ref RY_UART_EVENT_CTS
  uint32_t event_dsr          : 1;				//< Signal DSR change event: \ref RY_UART_EVENT_DSR
  uint32_t event_dcd          : 1;				//< Signal DCD change event: \ref RY_UART_EVENT_DCD
  uint32_t event_ri           : 1;				//< Signal RI change event: \ref RY_UART_EVENT_RI
} RY_UART_CAPABILITIES;

// UART Transfer Information (Run-Time)
typedef struct _UART_TRANSFER_INFO {
	uint32_t                rx_num;        		// Total number of data to be received
	uint32_t                tx_num;        		// Total number of data to be send
	uint8_t                *rx_buf;        		// Pointer to in data buffer
	uint8_t                *tx_buf;        		// Pointer to out data buffer
	uint32_t                rx_cnt;        		// Number of data received
	uint32_t                tx_cnt;        		// Number of data sent
	uint8_t                 tx_def_val;    		// Transmit default value (used in UART_SYNC_MASTER_MODE_RX)
	uint8_t                 send_active;   		// Send active flag
} RY_UART_TRANSFER_INFO;

typedef struct _UART_RX_STATUS {
	uint8_t rx_busy;                       		// Receiver busy flag
	uint8_t rx_overflow;                		// Receive data overflow detected (cleared on start of next receive operation)
	uint8_t rx_break;                     		// Break detected on receive (cleared on start of next receive operation)
	uint8_t rx_framing_error;        			// Framing error detected on receive (cleared on start of next receive operation)
	uint8_t rx_parity_error;           			// Parity error detected on receive (cleared on start of next receive operation)
} RY_UART_RX_STATUS;

// UART Information (Run-Time)
typedef struct _UART_INFO {
	p_uart_int_cb cb_event;      				// Event callback
	RY_UART_RX_STATUS		rx_status;   	 	// Receive status flags
	RY_UART_TRANSFER_INFO	xfer;				// Transfer information
	uint8_t                 mode;          		// UART mode
	uint8_t                 flags;         		// UART driver flags
	uint32_t                baudrate;      		// Baudrate
} RY_UART_INFO;

// UART DMA
typedef const struct _UART_DMA {
	uint8_t  	channel;       					// DMA Channel
	uint8_t		reqsel;        					// DMA request selection
	p_dma_int_cb	cb_event;				// DMA Event callback
} RY_UART_DMA;

// UART Resources definitions
typedef struct _UART_RESOURCES{
	RY_UART_CAPABILITIES  capabilities;  		// Capabilities
	UART_RegDef		*reg;          				// Pointer to UART peripheral
	uint32_t		irq_num;       				// UART IRQ Number
	uint32_t		trig_lvl;      				// FIFO Trigger level
	RY_UART_DMA		*dma_tx;
	RY_UART_DMA		*dma_rx;
	RY_UART_INFO	*info;          			// Run-Time Information
}RY_UART_RESOURCES;

/**
\brief Access structure of the UART Driver.
*/
typedef struct _NDS_DRIVER_UART {
  RY_UART_CAPABILITIES	(*GetCapabilities) 	(void);                       		///< Pointer to \ref NDS_UART_GetCapabilities : Get driver capabilities.
  int32_t              	(*Initialize)    	(p_uart_int_cb cb_event);  			///< Pointer to \ref NDS_UART_Initialize : Initialize UART Interface.
  int32_t              	(*Uninitialize)  	(void);                            	///< Pointer to \ref NDS_UART_Uninitialize : De-initialize UART Interface.
  int32_t              	(*PowerControl)   	(RY_POWER_STATE state);           ///< Pointer to \ref NDS_UART_PowerControl : Control UART Interface Power.
  int32_t              	(*Send)           	(const void *data, uint32_t num);   ///< Pointer to \ref NDS_UART_Send : Start sending data to UART transmitter.
  int32_t              	(*Receive)         	(void *data, uint32_t num);    		///< Pointer to \ref NDS_UART_Receive : Start receiving data from UART receiver.
  int32_t              	(*Transfer)        	(const void *data_out,
		  	  	  	  	  	  	  	  	  	  	  void *data_in,
												  uint32_t num);          		///< Pointer to \ref NDS_UART_Transfer : Start sending/receiving data to/from UART.
  uint32_t             	(*GetTxCount)      	(void);                      		///< Pointer to \ref NDS_UART_GetTxCount : Get transmitted data count.
  uint32_t             	(*GetRxCount)      	(void);                       		///< Pointer to \ref NDS_UART_GetRxCount : Get received data count.
  int32_t              	(*Control)         	(uint32_t control, uint32_t arg);   ///< Pointer to \ref NDS_UART_Control : Control UART Interface.
  RY_UART_STATUS       	(*GetStatus)       	(void);                             ///< Pointer to \ref NDS_UART_GetStatus : Get UART status.
  int32_t              	(*SetModemControl) 	(RY_UART_MODEM_CONTROL control);   	///< Pointer to \ref NDS_UART_SetModemControl : Set UART Modem Control line state.
  RY_UART_MODEM_STATUS 	(*GetModemStatus)  	(void);                             ///< Pointer to \ref NDS_UART_GetModemStatus : Get UART Modem Status lines state.
} const RY_DRIVER_UART;

//UART
typedef enum
{
	UART_IO_GP0 = 0,
	UART_IO_GP1,
	UART_IO_GP2,
	UART_IO_GP3,
	UART_IO_GP4,
	UART_IO_GP_MAX,
}uart_multi_io;

extern unsigned int uart_io_multi_config(RYRV_UART_NO uart_no, uart_multi_io uart_gp, uint32 flow_enable);

// baud rate setting
typedef enum
{
    UART_BD_4800       = 4800,
    UART_BD_7200       = 7200,
    UART_BD_9600       = 9600,
    UART_BD_19200      = 19200,
    UART_BD_38400      = 38400,
    UART_BD_57600      = 57600,
    UART_BD_115200     = 115200,
    UART_BD_230400     = 230400,
    UART_BD_460800     = 460800,
	UART_BD_921600     = 921600,
    UART_BD_1500000    = 1500000,//HCLK >= 48M
	UART_BD_3000000    = 3000000,//HCLK >= 96M
	UART_BD_6250000    = 6250000,//HCLK >= 200M
} RY_UART_BD;

/* IER Register (+0x24) */
#define 	UART_IER_RDR						0x01 	/* Data Ready Enable */
#define 	UART_IER_THRE						0x02 	/* THR Empty Enable */
#define 	UART_IER_RLS						0x04 	/* Receive Line Status Enable */
#define 	UART_IER_MS							0x08 	/* Modem Status Enable */

/* IIR Register (+0x28) */
#define 	UART_IIR_NONE						0x01 	/* No interrupt pending */
#define 	UART_IIR_RLS						0x06 	/* Receive Line Status */
#define	 	UART_IIR_RDA						0x04 	/* Receive Data Available */
#define 	UART_IIR_RTO						0x0c 	/* Receive Time Out */
#define 	UART_IIR_THRE						0x02 	/* THR Empty */
#define 	UART_IIR_MODEM						0x00 	/* Modem Status */
#define 	UART_IIR_INT_MASK					0x0f 	/* Interrupt Status Bits Mask */

#define 	UART_IIR_TFIFO_FULL					0x10 	/* TX FIFO full */
#define 	UART_IIR_FIFO_EN					0xc0 	/* FIFO mode is enabled, set when FCR[0] is 1 */

/* FCR Register (+0x28) */
#define 	UART_FCR_FIFO_EN               		0x01 	/* FIFO Enable */
#define 	UART_FCR_RFIFO_RESET           		0x02 	/* Rx FIFO Reset */
#define 	UART_FCR_TFIFO_RESET           		0x04 	/* Tx FIFO Reset */
#define 	UART_FCR_DMA_EN                		0x08 	/* Select UART DMA mode */

#define 	UART_FCR_TFIFO16_TRGL1         		0x00 	/* TX 16-byte FIFO int trigger level - 1 char */
#define 	UART_FCR_TFIFO16_TRGL3         		0x10 	/* TX 16-byte FIFO int trigger level - 3 char */
#define 	UART_FCR_TFIFO16_TRGL9         		0x20 	/* TX 16-byte FIFO int trigger level - 9 char */
#define 	UART_FCR_TFIFO16_TRGL13        		0x30 	/* TX 16-byte FIFO int trigger level - 13 char */

#define 	UART_FCR_RFIFO16_TRGL1         		0x00 	/* RX 16-byte FIFO int trigger level - 1 char */
#define 	UART_FCR_RFIFO16_TRGL4         		0x40 	/* RX 16-byte FIFO int trigger level - 4 char */
#define 	UART_FCR_RFIFO16_TRGL8         		0x80 	/* RX 16-byte FIFO int trigger level - 8 char */
#define 	UART_FCR_RFIFO16_TRGL14        		0xc0 	/* RX 16-byte FIFO int trigger level - 14 char */

#define 	UART_FCR_FIFO_EN_MASK				0x01 	/* FIFO Enable */
#define 	UART_FCR_FIFO_EN_BIT				0
#define 	UART_FCR_RFIFO_RESET_MASK			0x02 	/* Rx FIFO Reset */
#define		UART_FCR_RFIFO_RESET_BIT			1
#define 	UART_FCR_TFIFO_RESET_MASK			0x04 	/* Tx FIFO Reset */
#define 	UART_FCR_TFIFO_RESET_BIT			2
#define 	UART_FCR_DMA_EN_MASK				0x08 	/* Select UART DMA mode */
#define 	UART_FCR_DMA_EN_BIT					3

#define 	UART_FCR_TXFIFO_TRGL_MASK			0x30 	/* TX FIFO int trigger level */
#define 	UART_FCR_TXFIFO_TRGL_SHIFT			4
#define 	UART_FCR_RXFIFO_TRGL_MASK			0xc0 	/* RX FIFO int trigger level */
#define 	UART_FCR_RXFIFO_TRGL_SHIFT			6

/* LCR Register (+0x2c) */
#define 	UART_LCR_BITS5                 		0x00
#define 	UART_LCR_BITS6                 		0x01
#define 	UART_LCR_BITS7                 		0x02
#define 	UART_LCR_BITS8                 		0x03

#define 	UART_LCR_STOP1                 		0x00
#define 	UART_LCR_STOP2                 		0x04

#define 	UART_LCR_PARITY_NONE         		0x00 	/* No Parity Check */
#define 	UART_LCR_PARITY_ODD           		0x08 	/* Odd Parity */
#define 	UART_LCR_PARITY_EVEN          		0x18 	/* Even Parity */
#define 	UART_LCR_PARITY_1              		0x28 	/* 1 Parity Bit */
#define 	UART_LCR_PARITY_0              		0x38 	/* 0 Parity Bit */

#define 	UART_LCR_SETBREAK              		0x40 	/* Set Break condition */
#define 	UART_LCR_DLAB                  		0x80 	/* Divisor Latch Access Bit */

/* MCR Register (+0x30) */
#define 	UART_MCR_DTR                   		0x01 	/* Data Terminal Ready */
#define 	UART_MCR_RTS                   		0x02 	/* Request to Send */
#define 	UART_MCR_OUT1                  		0x04 	/* output1 */
#define 	UART_MCR_OUT2                  		0x08 	/* output2 or global interrupt enable */
#define 	UART_MCR_LPBK                  		0x10 	/* loopback mode */
#define 	UART_MCR_AFE                   		0x20 	/* Auto flow control */

/* LSR Register (+0x34) */
#define 	UART_LSR_RDR                   		0x1 	/* Data Ready */
#define 	UART_LSR_OE                    		0x2 	/* Overrun Error */
#define 	UART_LSR_PE                    		0x4 	/* Parity Error */
#define 	UART_LSR_FE                    		0x8 	/* Framing Error */
#define 	UART_LSR_BI                    		0x10 	/* Break Interrupt */
#define 	UART_LSR_THRE                  		0x20 	/* THR/FIFO Empty */
#define 	UART_LSR_TEMT                  		0x40 	/* THR/FIFO and TFR Empty */
#define 	UART_LSR_ERRF                  		0x80 	/* FIFO Data Error */

/* MSR Register (+0x38) */
#define 	UART_MSR_DCTS                  		0x1 	/* Delta CTS */
#define 	UART_MSR_DDSR                  		0x2 	/* Delta DSR */
#define 	UART_MSR_TERI                  		0x4 	/* Trailing Edge RI */
#define 	UART_MSR_DDCD                  		0x8 	/* Delta CD */
#define 	UART_MSR_CTS                   		0x10 	/* Clear To Send */
#define 	UART_MSR_DSR                  		0x20 	/* Data Set Ready */
#define 	UART_MSR_RI                    		0x40 	/* Ring Indicator */
#define 	UART_MSR_DCD                   		0x80 	/* Data Carrier Detect */

// UART flags
#define 	UART_FLAG_INITIALIZED          		(1UL<<0)
#define 	UART_FLAG_POWERED              		(1UL<<1)
#define 	UART_FLAG_CONFIGURED           		(1UL<<2)
#define 	UART_FLAG_TX_ENABLED           		(1UL<<3)
#define 	UART_FLAG_RX_ENABLED           		(1UL<<4)
#define 	UART_FLAG_SEND_ACTIVE         		(1UL<<5)

// UART TX FIFO trigger level
#define 	UART_TRIG_LVL_1               	 	(0x00U)
#define 	UART_TRIG_LVL_4                		(0x40U)
#define 	UART_TRIG_LVL_8                		(0x80U)
#define 	UART_TRIG_LVL_14               		(0xc0U)

extern RY_DRIVER_UART Driver_UART0;
extern RY_DRIVER_UART Driver_UART1;
extern RY_DRIVER_UART Driver_UART2;

#endif /* __GTRY_UART_DRV_H__ */
