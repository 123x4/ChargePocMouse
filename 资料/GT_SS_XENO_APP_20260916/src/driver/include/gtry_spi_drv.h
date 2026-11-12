/*
 * gtry_spi_drv.h
 *
 *  Created on: 2024.9.4
 *      Author: ziggy
 */

#ifndef __GTRY_SPI_DRV_H_
#define __GTRY_SPI_DRV_H_

#include "gtry_platform.h"
#include "gtry_drv_common.h"
#include "gtry_dma_drv.h"

/****** SPI Control Codes *****/

#define		RY_SPI_CONTROL_Pos				0
#define		RY_SPI_CONTROL_Msk				(0xFFUL << RY_SPI_CONTROL_Pos)

/*----- SPI Control Codes: Mode -----*/
#define		RY_SPI_MODE_INACTIVE           	(0x00UL << RY_SPI_CONTROL_Pos)     ///< SPI Inactive
#define 	RY_SPI_MODE_MASTER             	(0x01UL << RY_SPI_CONTROL_Pos)     ///< SPI Master (Output on MOSI, Input on MISO); arg = Bus Speed in bps
#define 	RY_SPI_MODE_SLAVE              	(0x02UL << RY_SPI_CONTROL_Pos)     ///< SPI Slave  (Output on MISO, Input on MOSI)
#define 	RY_SPI_MODE_MASTER_SIMPLEX     	(0x03UL << RY_SPI_CONTROL_Pos)     ///< SPI Master (Output/Input on MOSI); arg = Bus Speed in bps
#define 	RY_SPI_MODE_SLAVE_SIMPLEX      	(0x04UL << RY_SPI_CONTROL_Pos)     ///< SPI Slave  (Output/Input on MISO)

/*----- SPI Control Codes: Mode Parameters: Frame Format -----*/
#define 	RY_SPI_FRAME_FORMAT_Pos         8
#define 	RY_SPI_FRAME_FORMAT_Msk        	(7UL << RY_SPI_FRAME_FORMAT_Pos)
#define 	RY_SPI_CPOL0_CPHA0             	(0UL << RY_SPI_FRAME_FORMAT_Pos)   ///< Clock Polarity 0, Clock Phase 0 (default)
#define 	RY_SPI_CPOL0_CPHA1             	(1UL << RY_SPI_FRAME_FORMAT_Pos)   ///< Clock Polarity 0, Clock Phase 1
#define 	RY_SPI_CPOL1_CPHA0             	(2UL << RY_SPI_FRAME_FORMAT_Pos)   ///< Clock Polarity 1, Clock Phase 0
#define 	RY_SPI_CPOL1_CPHA1             	(3UL << RY_SPI_FRAME_FORMAT_Pos)   ///< Clock Polarity 1, Clock Phase 1
#define 	RY_SPI_TI_SSI                  	(4UL << RY_SPI_FRAME_FORMAT_Pos)   ///< Texas Instruments Frame Format
#define 	RY_SPI_MICROWIRE               	(5UL << RY_SPI_FRAME_FORMAT_Pos)   ///< National Microwire Frame Format

/*----- SPI Control Codes: Mode Parameters: Data Bits -----*/
#define 	RY_SPI_DATA_BITS_Pos            12
#define 	RY_SPI_DATA_BITS_Msk           	(0x3FUL << RY_SPI_DATA_BITS_Pos)
#define 	RY_SPI_DATA_BITS(n)            	(((n) & 0x3F) << RY_SPI_DATA_BITS_Pos) ///< Number of Data bits

/*----- SPI Control Codes: Mode Parameters: Bit Order -----*/
#define 	RY_SPI_BIT_ORDER_Pos            18
#define 	RY_SPI_BIT_ORDER_Msk           	(1UL << RY_SPI_BIT_ORDER_Pos)
#define 	RY_SPI_MSB_LSB                 	(0UL << RY_SPI_BIT_ORDER_Pos)      ///< SPI Bit order from MSB to LSB (default)
#define 	RY_SPI_LSB_MSB                 	(1UL << RY_SPI_BIT_ORDER_Pos)      ///< SPI Bit order from LSB to MSB

/*----- SPI Control Codes: Mode Parameters: Slave Select Mode -----*/
#define 	RY_SPI_SS_MASTER_MODE_Pos       19
#define 	RY_SPI_SS_MASTER_MODE_Msk      	(3UL << RY_SPI_SS_MASTER_MODE_Pos)
#define 	RY_SPI_SS_MASTER_UNUSED        	(1UL << RY_SPI_SS_MASTER_MODE_Pos) ///< SPI Slave Select when Master: Not used
#define 	RY_SPI_SS_MASTER_SW            	(2UL << RY_SPI_SS_MASTER_MODE_Pos) ///< SPI Slave Select when Master: Software controlled
#define 	RY_SPI_SS_MASTER_HW_OUTPUT     	(0UL << RY_SPI_SS_MASTER_MODE_Pos) ///< SPI Slave Select when Master: Hardware controlled Output (default)
#define 	RY_SPI_SS_MASTER_HW_INPUT      	(3UL << RY_SPI_SS_MASTER_MODE_Pos) ///< SPI Slave Select when Master: Hardware monitored Input

#define 	RY_SPI_SS_SLAVE_MODE_Pos        21
#define 	RY_SPI_SS_SLAVE_MODE_Msk       	(1UL << RY_SPI_SS_SLAVE_MODE_Pos)
#define 	RY_SPI_SS_SLAVE_HW             	(0UL << RY_SPI_SS_SLAVE_MODE_Pos)  ///< SPI Slave Select when Slave: Hardware monitored (default)
#define 	RY_SPI_SS_SLAVE_SW             	(1UL << RY_SPI_SS_SLAVE_MODE_Pos)  ///< SPI Slave Select when Slave: Software controlled

/*----- SPI Control Codes: Mode Parameters: Transfer Format Phase -----*/
#define 	RY_SPI_TRANSFER_FORMAT_Pos      22
#define 	RY_SPI_TRANSFER_FORMAT_Msk     	(1UL << RY_SPI_TRANSFER_FORMAT_Pos)
#define 	RY_SPI_SLV_DATA_ONLY_TRANSFER  	(1UL << RY_SPI_TRANSFER_FORMAT_Pos) ///< (Slave mode only) SPI transaction only consists of data without command and address phase.

/*----- SPI Control Codes: Miscellaneous Controls  -----*/
#define 	RY_SPI_SET_BUS_SPEED           	(0x10UL << RY_SPI_CONTROL_Pos)     ///< Set Bus Speed in bps; arg = value
#define 	RY_SPI_GET_BUS_SPEED           	(0x11UL << RY_SPI_CONTROL_Pos)     ///< Get Bus Speed in bps
#define 	RY_SPI_SET_DEFAULT_TX_VALUE    	(0x12UL << RY_SPI_CONTROL_Pos)     ///< Set default Transmit value; arg = value
#define 	RY_SPI_CONTROL_SS              	(0x13UL << RY_SPI_CONTROL_Pos)     ///< Control Slave Select; arg: 0=inactive, 1=active
#define 	RY_SPI_ABORT_TRANSFER          	(0x14UL << RY_SPI_CONTROL_Pos)     ///< Abort current data transfer
#define 	RY_SPI_TX_HEADER_LENGTH			(0x20UL << RY_SPI_CONTROL_Pos)

/****** SPI Slave Select Signal definitions *****/
#define 	RY_SPI_SS_INACTIVE              0                                  ///< SPI Slave Select Signal Inactive
#define 	RY_SPI_SS_ACTIVE                1                                  ///< SPI Slave Select Signal Active

/****** SPI specific error codes *****/
#define 	RY_SPI_ERROR_MODE              	(RY_DRIVER_ERROR_SPECIFIC - 1)     ///< Specified Mode not supported
#define 	RY_SPI_ERROR_FRAME_FORMAT      	(RY_DRIVER_ERROR_SPECIFIC - 2)     ///< Specified Frame Format not supported
#define 	RY_SPI_ERROR_DATA_BITS         	(RY_DRIVER_ERROR_SPECIFIC - 3)     ///< Specified number of Data bits not supported
#define 	RY_SPI_ERROR_BIT_ORDER         	(RY_DRIVER_ERROR_SPECIFIC - 4)     ///< Specified Bit order not supported
#define 	RY_SPI_ERROR_SS_MODE           	(RY_DRIVER_ERROR_SPECIFIC - 5)     ///< Specified Slave Select Mode not supported
#define 	RY_SPI_ERROR_HEADER_LEN			(RY_DRIVER_ERROR_SPECIFIC - 6)


/**
\brief SPI Status
*/
typedef struct _RY_SPI_STATUS {
  uint32_t busy       : 1;              ///< Transmitter/Receiver busy flag
  uint32_t data_lost  : 1;              ///< Data lost: Receive overflow / Transmit underflow (cleared on start of transfer operation)
  uint32_t mode_fault : 1;              ///< Mode fault detected; optional (cleared on start of transfer operation)
} RY_SPI_STATUS;


/****** SPI Event *****/
#define RY_SPI_EVENT_TRANSFER_COMPLETE (1UL << 0)  ///< Data Transfer completed
#define RY_SPI_EVENT_DATA_LOST         (1UL << 1)  ///< Data lost: Receive overflow / Transmit underflow
#define RY_SPI_EVENT_MODE_FAULT        (1UL << 2)  ///< Master Mode Fault (SS deactivated when Master)


// Function documentation
/**
  \fn          RY_DRIVER_VERSION RY_SPI_GetVersion (void)
  \brief       Get driver version.
  \return      \ref RY_DRIVER_VERSION

  \fn          RY_SPI_CAPABILITIES RY_SPI_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref RY_SPI_CAPABILITIES

  \fn          int32_t RY_SPI_Initialize (p_spi_int_cb cb_event)
  \brief       Initialize SPI Interface.
  \param[in]   cb_event  Pointer to \ref p_spi_int_cb
  \return      \ref execution_status

  \fn          int32_t RY_SPI_Uninitialize (void)
  \brief       De-initialize SPI Interface.
  \return      \ref execution_status

  \fn          int32_t RY_SPI_PowerControl (RY_POWER_STATE state)
  \brief       Control SPI Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status

  \fn          int32_t RY_SPI_Send (const void *data, uint32_t num)
  \brief       Start sending data to SPI transmitter.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter
  \param[in]   num   Number of data items to send
  \return      \ref execution_status

  \fn          int32_t RY_SPI_Receive (void *data, uint32_t num)
  \brief       Start receiving data from SPI receiver.
  \param[out]  data  Pointer to buffer for data to receive from SPI receiver
  \param[in]   num   Number of data items to receive
  \return      \ref execution_status

  \fn          int32_t RY_SPI_Transfer (const void *data_out,
                                               void *data_in,
                                         uint32_t    num)
  \brief       Start sending/receiving data to/from SPI transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   num       Number of data items to transfer
  \return      \ref execution_status

  \fn          uint32_t RY_SPI_GetDataCount (void)
  \brief       Get transferred data count.
  \return      number of data items transferred

  \fn          int32_t RY_SPI_Control (uint32_t control, uint32_t arg)
  \brief       Control SPI Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \return      common \ref execution_status and driver specific \ref spi_execution_status

  \fn          RY_SPI_STATUS RY_SPI_GetStatus (void)
  \brief       Get SPI status.
  \return      SPI status \ref RY_SPI_STATUS

  \fn          void p_spi_int_cb (uint32_t event)
  \brief       SPI Events.
  \param[in]   event \ref SPI_events notification mask
  \return      none
*/

typedef void (*p_spi_int_cb)(uint32_t event);  ///< Pointer to \ref p_spi_int_cb : SPI Event.

/**
\brief SPI Driver Capabilities.
*/
typedef struct _RY_SPI_CAPABILITIES {
  uint32_t simplex          : 1;        ///< supports Simplex Mode (Master and Slave)
  uint32_t ti_ssi           : 1;        ///< supports TI Synchronous Serial Interface
  uint32_t microwire        : 1;        ///< supports Microwire Interface
  uint32_t event_mode_fault : 1;        ///< Signal Mode Fault event: \ref RY_SPI_EVENT_MODE_FAULT
} RY_SPI_CAPABILITIES;


/**
\brief Access structure of the SPI Driver.
*/
typedef struct _RY_DRIVER_SPI {
  RY_SPI_CAPABILITIES	(*GetCapabilities) (void);                             ///< Pointer to \ref RY_SPI_GetCapabilities : Get driver capabilities.
  int32_t              	(*Initialize)      (p_spi_int_cb cb_event);   ///< Pointer to \ref RY_SPI_Initialize : Initialize SPI Interface.
  int32_t              	(*Uninitialize)    (void);                             ///< Pointer to \ref RY_SPI_Uninitialize : De-initialize SPI Interface.
  int32_t              	(*PowerControl)    (RY_POWER_STATE state);            ///< Pointer to \ref RY_SPI_PowerControl : Control SPI Interface Power.
  int32_t              	(*Send)            (const void *data, uint32_t num);   ///< Pointer to \ref RY_SPI_Send : Start sending data to SPI Interface.
  int32_t              	(*Receive)         (void *data, uint32_t num);   ///< Pointer to \ref RY_SPI_Receive : Start receiving data from SPI Interface.
  int32_t              	(*Transfer)        (const void *data_out, void *data_in, uint32_t num);                  ///< Pointer to \ref RY_SPI_Transfer : Start sending/receiving data to/from SPI.
  uint32_t             	(*GetDataCount)    (void);                             ///< Pointer to \ref RY_SPI_GetDataCount : Get transferred data count.
  int32_t              	(*Control)         (uint32_t control, uint32_t arg);   ///< Pointer to \ref RY_SPI_Control : Control SPI Interface.
  RY_SPI_STATUS       	(*GetStatus)       (void);                             ///< Pointer to \ref RY_SPI_GetStatus : Get SPI status.
} const RY_DRIVER_SPI;


/* SPI ID revision register */
#define 	SPI_IDREV_MINOR_BIT           	(0)
#define 	SPI_IDREV_MINOR_MASK          	(0xf << 0)
#define 	SPI_IDREV_MAJOR_BIT           	(4)
#define 	SPI_IDREV_MAJOR_MASK          	(0xf << 4)

/* SPI transfer format register */
#define 	SPI_CPHA                      	(1UL << 0)
#define 	SPI_CPOL                      	(1UL << 1)
#define 	SPI_SLAVE                     	(1UL << 2)
#define 	SPI_LSB                       	(1UL << 3)
#define 	SPI_MERGE                     	(1UL << 7)
#define 	DATA_BITS_MSK                 	(0x1f << 8)
#define 	DATA_BITS(data_bits)          	((data_bits - 1) << 8)

/* SPI transfer control register */
#define 	SPI_TRANSCTRL_SLV_DATA_ONLY   	(0x1 << 31)
#define 	SPI_TRANSCTRL_CMDEN           	(0x1 << 30)
#define 	SPI_TRANSCTRL_ADDREN          	(0x1 << 29)
#define 	SPI_TRANSCTRL_ADDRFMT         	(0x1 << 28)

// SPI transfer mode
#define 	SPI_TRANSMODE_MSK             	(0xf << 24)
#define 	SPI_TRANSMODE_WRnRD           	(0x0 << 24)
#define 	SPI_TRANSMODE_WRONLY          	(0x1 << 24)
#define 	SPI_TRANSMODE_RDONLY          	(0x2 << 24)
#define 	SPI_TRANSMODE_WR_RD           	(0x3 << 24)
#define 	SPI_TRANSMODE_RD_WR           	(0x4 << 24)
#define 	SPI_TRANSMODE_WR_DMY_RD       	(0x5 << 24)
#define 	SPI_TRANSMODE_RD_DMY_WR       	(0x6 << 24)
#define 	SPI_TRANSMODE_NONEDATA        	(0x7 << 24)
#define 	SPI_TRANSMODE_DMY_WR          	(0x8 << 24)
#define 	SPI_TRANSMODE_DMY_RD          	(0x9 << 24)

// RD/WR transfer count
#define 	RD_TRANCNT_MSK                	(0x1ff << 0)
#define 	RD_TRANCNT(num)               	((num - 1) << 0)
#define 	WR_TRANCNT_MSK                	(0x1ff << 12)
#define 	WR_TRANCNT(num)               	((num - 1) << 12)

/* SPI Master Data Count Register */
/* SPI Slave Data Count Register */
#define 	SPI_TRANS_MAX_CNT_MASK          (0x3fffff)

// SPI Dummy Count
#define 	SPI_DUMMY_COUNT_1     	  	  	(0x0 << 9)
#define 	SPI_DUMMY_COUNT_2     	  	  	(0x1 << 9)
#define 	SPI_DUMMY_COUNT_3     	  	  	(0x2 << 9)
#define 	SPI_DUMMY_COUNT_4     	  	  	(0x3 << 9)

/* SPI control register */
#define 	SPIRST                        	(1UL << 0)
#define 	RXFIFORST                     	(1UL << 1)
#define 	TXFIFORST                     	(1UL << 2)
#define 	RXDMAEN                       	(1UL << 3)
#define 	TXDMAEN                       	(1UL << 4)
#define 	RXTHRES(num)                  	(num << 8)
#define 	TXTHRES(num)                  	(num << 16)

#define 	THRES_MASK_FIFO_16            	(0x1f)
#define 	THRES_MASK_FIFO_128           	(0xff)
#define 	RXTHRES_OFFSET                	(8)
#define 	TXTHRES_OFFSET                	(16)

/* SPI interrupt status register */
/* SPI interrupt enable register */
#define 	SPI_RXFIFOOORINT              	(1UL << 0)
#define 	SPI_TXFIFOOURINT              	(1UL << 1)
#define 	SPI_RXFIFOINT                 	(1UL << 2)
#define 	SPI_TXFIFOINT                 	(1UL << 3)
#define 	SPI_ENDINT                    	(1UL << 4)
#define 	SPI_SLVCMD                    	(1UL << 5)


// SPI flags
#define 	SPI_FLAG_INITIALIZED          	(1UL << 0)     // SPI initialized
#define 	SPI_FLAG_POWERED              	(1UL << 1)     // SPI powered on
#define 	SPI_FLAG_CONFIGURED           	(1UL << 2)     // SPI configured
#define 	SPI_FLAG_DATA_LOST            	(1UL << 3)     // SPI data lost occurred
#define 	SPI_FLAG_MODE_FAULT           	(1UL << 4)     // SPI mode fault occurred

// SPI return values
#define 	SPI_RET_SUCCESS          		RY_SPI_DRIVER_RET_MASK|(0x0UL)     // SPI operate success
#define 	SPI_RET_PARAM_ERROR          	RY_SPI_DRIVER_RET_MASK|(0x1UL)     // SPI operate success
#define 	SPI_RET_CONFIG_ERROR          	RY_SPI_DRIVER_RET_MASK|(0x2UL)     // SPI operate success
#define 	SPI_RET_COMM_ERROR          	RY_SPI_DRIVER_RET_MASK|(0x3UL)     // SPI operate success

// SPI transfer information (Run-time)
typedef struct _SPI_TRANSFER_INFO {
	uint8_t			transfer_op;   		// transfer operation: send/recv/transfer
	uint8_t			*rx_buf;       		// pointer to in data buffer
	uint8_t			*tx_buf;       		// pointer to out data buffer
	uint8_t			*tx_buf_limit; 		// pointer to the end of in data buffer
	uint32_t		rx_cnt;        		// number of data received
	uint32_t		tx_cnt;        		// number of data sent
	uint8_t			txfifo_refill; 		// The size of data SPI TX ISR refills one time.
	uint8_t			dma_tx_complete; 	// DMA driver have handled it's completion, or SPI driver needs to help to close DMA channel.
	uint8_t			dma_rx_complete; 	// DMA driver have handled it's completion, or SPI driver needs to help to close DMA channel.
} SPI_TRANSFER_INFO;

// SPI status
typedef struct _SPI_STATUS {
	uint8_t			busy;         		// transmitter/receiver busy flag
	uint8_t			data_lost;    		// data lost: receiver overflow/transmit underflow (cleared on start of transfer operation)
	uint8_t			mode_fault;   		// Mode fault detected; optional (cleared on start of transfer operation)
} SPI_STATUS;

// SPI information (Run-time)
typedef struct _SPI_INFO {
	p_spi_int_cb	cb_event;			// event callback
	SPI_STATUS		status;        		// SPI status flags
	SPI_TRANSFER_INFO	xfer;          	// SPI transfer information
	uint8_t			flags;         		// SPI driver flags
	uint32_t		mode;          		// SPI mode
	uint8_t			is_header;     		// Tx header flag
	uint8_t			*record_rx_buf;		// Record the original rx buf (Only used when there is tx header)
	uint8_t			txfifo_size;   		// SPI HW TXFIFO size
	uint8_t			data_bits;     		// the size of one unit SPI data (1 ~ 32, defaults: 8 bits)
	uint8_t			src_width;     		// SPI transfer width(align 8 bits)
	uint32_t		data_num;      		// num of the pure transfer data(Does not include header)
	uint8_t			tx_header_len; 		// SPI header(Usually include CMD, ADDRESS and DUMMY)
} SPI_INFO;

// SPI DMA
typedef const struct _SPI_DMA {
	uint8_t			channel;     		// DMA channel
	uint8_t			reqsel;      		// DMA request selection
	p_dma_int_cb	cb_event;    	// DMA event callback
} SPI_DMA;

// SPI resources definitions
typedef struct {
	RY_SPI_CAPABILITIES	capabilities; 	// capabilities
	SPI_RegDef		*reg;         		// pointer to SPI peripheral
	uint32_t		irq_num;      		// SPI IRQ number
	SPI_DMA			*dma_tx;      		// SPI TX DMA
	SPI_DMA			*dma_rx;      		// SPI RX DMA
	SPI_INFO		*info;        		// SPI run-time information
} const SPI_RESOURCES;



//SPI
typedef enum
{
	SPI_IO_GP0 = 0,
	SPI_IO_GP1,
	SPI_IO_GP_MAX,
}spi_multi_io;

extern RY_DRIVER_SPI Driver_SPI0;
extern RY_DRIVER_SPI Driver_SPI1;
extern RY_DRIVER_SPI Driver_SPI2;
extern RY_DRIVER_SPI Driver_SPI3;
extern RY_DRIVER_SPI Driver_SPI4;
extern RY_DRIVER_SPI Driver_SPI5;
extern RY_DRIVER_SPI Driver_SPI6;

extern unsigned int spi_io_multi_config(RYRV_SPI_NO spi_no, spi_multi_io spi_gp, uint32 qspi_enable);

#endif	/* __GTRY_SPI_DRV_H_ */
