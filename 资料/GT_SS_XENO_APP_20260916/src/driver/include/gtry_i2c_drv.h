/*
 * gtry_i2c_drv.h
 *
 *  Created on: 2025.9.2
 *
 */

#ifndef __GTRY_I2C_DRV_H__
#define __GTRY_I2C_DRV_H__

#include "gtry_platform.h"
#include "gtry_drv_common.h"
#include "gtry_dma_drv.h"

/*----- I2C Bus Speed -----*/
#define I2C_BUS_SPEED_STANDARD      	(0x01)      ///< Standard Speed
#define I2C_BUS_SPEED_FAST          	(0x02)      ///< Fast Speed
#define I2C_BUS_SPEED_FAST_PLUS     	(0x03)      ///< Fast+ Speed

#define I2C_MAX_NUM						(0x3FFFFF)//0xFF
/**
 * \brief I2C工作模式
 */
typedef enum {
    I2C_SLAVE_MODE = 0,          		/**< 从机模式 */
    I2C_MASTER_MODE = 1,          		/**< 主机模式 */
} I2C_WORK_MODE;

typedef enum _I2C_CTRL_REG_ITEM_DIR
{
	I2C_MASTER_TX = 0x0,
	I2C_MASTER_RX = 0x1,
	I2C_SLAVE_TX = 0x1,
	I2C_SLAVE_RX = 0x0,
}I2C_CTRL_REG_ITEM_DIR;
/**
 * \brief I2C配置数据结构
 */
typedef struct {
	unsigned int	def_addr;         	/**< 从机地址 */
	unsigned char	dma_en;         	/**< DMA使能 */
	unsigned char	i2c_scl;        	/**< SCL引脚 */
	unsigned char	i2c_sda;        	/**< SDA引脚 */
}i2c_config_t;

// Interrupt Enable Register(RW)
#define IEN_ALL                     BITS(0,9)       // All Interrupts mask.
#define IEN_CMPL                    BIT(9)          // Completion Interrupt.
#define IEN_BYTE_RECV               BIT(8)          // Byte Receive Interrupt.
#define IEN_BYTE_TRANS              BIT(7)          // Byte Transmit Interrupt.
#define IEN_START                   BIT(6)          // START Condition Interrupt.
#define IEN_STOP                    BIT(5)          // STOP Condition Interrupt.
#define IEN_ARB_LOSE                BIT(4)          // Arbitration Lose Interrupt.
#define IEN_ADDR_HIT                BIT(3)          // Address Hit Interrupt.
#define IEN_FIFO_HALF               BIT(2)          // FIFO Half Interrupt.
#define IEN_FIFO_FULL               BIT(1)          // FIFO Full Interrupt.
#define IEN_FIFO_EMPTY              BIT(0)          // FIFO Empty Interrupt.

// Status Register(RW)
#define STATUS_W1C_ALL              BITS(3,9)       // All Interrupts status write 1 clear mask.
#define STATUS_LINE_SDA             BIT(14)         // current status of the SDA line on the bus.
#define STATUS_LINE_SCL             BIT(13)         // current status of the SCL line on the bus.
#define STATUS_GEN_CALL             BIT(12)         // the address of the current transaction is a general call address.
#define STATUS_BUS_BUSY             BIT(11)         // the bus is busy.
#define STATUS_ACK                  BIT(10)         // the type of the last received/transmitted acknowledgement bit.
#define STATUS_CMPL                 BIT(9)          // Transaction Completion
#define STATUS_BYTE_RECV            BIT(8)          // a byte of data has been received
#define STATUS_BYTE_TRANS           BIT(7)          // a byte of data has been transmitted.
#define STATUS_START                BIT(6)          // a START Condition or a repeated TART condition has been transmitted/received.
#define STATUS_STOP                 BIT(5)          // a STOP Condition has been transmitted/received.
#define STATUS_ARB_LOSE             BIT(4)          // the controller has lost the bus arbitration (master mode only).
#define STATUS_ADDR_HIT             BIT(3)          // Master: indicates that a slave has responded to the transaction, Slave: indicates that a transaction is targeting the controller (including the General Call).
#define STATUS_FIFO_HALF            BIT(2)          // Indicates that the FIFO is half-full or half-empty.
#define STATUS_FIFO_FULL            BIT(1)          // the FIFO is full.
#define STATUS_FIFO_EMPTY           BIT(0)          // the FIFO is empty.

// Address Register(RW)
#define SLAVE_ADDR_MSK              BITS(0,9)       // The slave address.

// Data Register(RW)
#define DATA_MSK                    BITS(0,7)       // Write this register to put one byte of data to the FIFO, Read this register to get one byte of data from the FIFO.

// Control Register(RW)
#define CTRL_PHASE_START            BIT(12)         // Enable this bit to send a START condition at the beginning of transaction, master mode only.
#define CTRL_PHASE_ADDR             BIT(11)         // Enable this bit to send the address after START condition, master mode only.
#define CTRL_PHASE_DATA             BIT(10)         // Enable this bit to send the data after Address phase, master mode only.
#define CTRL_PHASE_STOP             BIT(9)          // Enable this bit to send a STOP condition at the end of a transaction, master mode only.
#define CTRL_DIR                    BIT(8)          // Transaction direction
#define CTRL_DATA_COUNT             BITS(0,7)       // Data counts in bytes.

// Command Register(RW)
#define CMD_MSK                     BITS(0,2)       // action command mask
#define CMD_NO_ACT                  (0x0)           // no action
#define CMD_ISSUE_TRANSACTION       (0x1)           // issue a data transaction (Master only)
#define CMD_ACK                     (0x2)           // respond with an ACK to the received byte
#define CMD_NACK                    (0x3)           // respond with a NACK to the received byte
#define CMD_CLEAR_FIFO              (0x4)           // clear the FIFO
#define CMD_RESET_I2C               (0x5)           /* reset the I2C controller(abort current
                                                       ransaction, set the SDA and SCL line to the
                                                       open-drain mode, reset the Status Register and
                                                       the Interrupt Enable Register, and empty the
                                                       FIFO) */

// Setup Register(RW)
#define SETUP_T_SUDAT               BITS(24,28)     // T_SUDAT defines the data setup time before releasing the SCL.
#define SETUP_T_SP                  BITS(21,23)     // T_SP defines the pulse width of spikes that must be suppressed by the input filter.
#define SETUP_T_HDDAT               BITS(16,20)     // T_SUDAT defines the data hold time after SCL goes LOW.
#define SETUP_T_SCL_RATIO           BIT(13)         // The LOW period of the generated SCL clock is defined by the combination of T_SCLRatio and T_SCLHi values
#define SETUP_T_SCLHI               BITS(4,12)      // The HIGH period of generated SCL clock is defined by T_SCLHi.
#define SETUP_DMA_EN                BIT(3)          // Enable the direct memory access mode data transfer.
#define SETUP_MASTER                BIT(2)          // Configure this device as a master or a slave.
#define SETUP_ADDRESSING            BIT(1)          // I2C addressing mode: 10-bit or 7-bit addressing mode
#define SETUP_I2C_EN                BIT(0)          // Enable the I2C controller.

// I2C mode: standard-mode, fast-mode, fast-mode-plus, all based on APB-clk(60MHZ)
#define SETUP_T_SUDAT_STD           (0x9)
#define SETUP_T_SP_STD              (0x3)
#define SETUP_T_HDDAT_STD           (12)
#define SETUP_T_SCL_RATIO_STD       (0x0)
#define SETUP_T_SCLHI_STD           (287)

#define SETUP_T_SUDAT_FAST          (0x0)
#define SETUP_T_SP_FAST             (0x3)
#define SETUP_T_HDDAT_FAST          (12)
#define SETUP_T_SCL_RATIO_FAST      (0x1)
#define SETUP_T_SCLHI_FAST          (38)

#define SETUP_T_SUDAT_FAST_P        (0x0)
#define SETUP_T_SP_FAST_P           (0x3)
#define SETUP_T_HDDAT_FAST_P        (0x0)
#define SETUP_T_SCL_RATIO_FAST_P    (0x1)
#define SETUP_T_SCLHI_FAST_P        (13)

/****** I2C Control Codes *****/

#define RY_I2C_OWN_ADDRESS             (0x01)      ///< Set Own Slave Address; arg = address
#define RY_I2C_BUS_SPEED               (0x02)      ///< Set Bus Speed; arg = speed
#define RY_I2C_BUS_CLEAR               (0x03)      ///< Execute Bus clear: send nine clock pulses
#define RY_I2C_ABORT_TRANSFER          (0x04)      ///< Abort Master/Slave Transmit/Receive

/*----- I2C Bus Speed -----*/
#define RY_I2C_BUS_SPEED_STANDARD      (0x01)      ///< Standard Speed
#define RY_I2C_BUS_SPEED_FAST          (0x02)      ///< Fast Speed
#define RY_I2C_BUS_SPEED_FAST_PLUS     (0x03)      ///< Fast+ Speed
#define RY_I2C_BUS_SPEED_HIGH          (0x04)      ///< High Speed


/****** I2C Address Flags *****/

#define RY_I2C_ADDRESS_10BIT           0x0400      ///< 10-bit address flag
#define RY_I2C_ADDRESS_GC              0x8000      ///< General Call flag


/**
\brief I2C Status
*/
typedef struct _RY_I2C_STATUS {
  uint32_t busy             : 1;        ///< Busy flag
  uint32_t mode             : 1;        ///< Mode: 0=Slave, 1=Master
  uint32_t direction        : 2;        ///< Direction: 0=non, 1=Receiver, 2=Transmitter
  uint32_t general_call     : 1;        ///< General Call indication (cleared on start of next Slave operation)
  uint32_t arbitration_lost : 1;        ///< Master lost arbitration (cleared on start of next Master operation)
  uint32_t bus_error        : 1;        ///< Bus error detected (cleared on start of next Master/Slave operation)
} RY_I2C_STATUS;


/****** I2C Event *****/
#define RY_I2C_EVENT_TRANSFER_DONE       (1UL << 0)  ///< Master/Slave Transmit/Receive finished
#define RY_I2C_EVENT_TRANSFER_INCOMPLETE (1UL << 1)  ///< Master/Slave Transmit/Receive incomplete transfer
#define RY_I2C_EVENT_SLAVE_TRANSMIT      (1UL << 2)  ///< Slave Transmit operation requested
#define RY_I2C_EVENT_SLAVE_RECEIVE       (1UL << 3)  ///< Slave Receive operation requested
#define RY_I2C_EVENT_ADDRESS_NACK        (1UL << 4)  ///< Address not acknowledged from Slave
#define RY_I2C_EVENT_GENERAL_CALL        (1UL << 5)  ///< General Call indication
#define RY_I2C_EVENT_ARBITRATION_LOST    (1UL << 6)  ///< Master lost arbitration
#define RY_I2C_EVENT_BUS_ERROR           (1UL << 7)  ///< Bus error detected (START/STOP at illegal position)
#define RY_I2C_EVENT_BUS_CLEAR           (1UL << 8)  ///< Bus clear finished

typedef void (*p_i2c_int_cb) (uint32_t event);  ///< Pointer to \ref p_i2c_int_cb : I2C Event.


// I2C driver running state
typedef enum _I2C_DRIVER_STATE
{
	I2C_DRV_NONE = 0x0,
	I2C_DRV_INIT = BIT(0),
	I2C_DRV_POWER = BIT(1),
	I2C_DRV_CFG_PARAM = BIT(2),
	I2C_DRV_MASTER_TX = BIT(3),
	I2C_DRV_MASTER_RX = BIT(4),
	I2C_DRV_SLAVE_TX = BIT(5),
	I2C_DRV_SLAVE_RX = BIT(6),
	I2C_DRV_MASTER_TX_CMPL = BIT(7),
	I2C_DRV_MASTER_RX_CMPL = BIT(8),
	I2C_DRV_SLAVE_TX_CMPL = BIT(9),
	I2C_DRV_SLAVE_RX_CMPL = BIT(10),
}I2C_DRIVER_STATE;

/**
\brief I2C Driver Capabilities.
*/
typedef struct _RY_I2C_CAPABILITIES {
  uint32_t address_10_bit : 1;          ///< supports 10-bit addressing
} RY_I2C_CAPABILITIES;

typedef struct _I2C_INFO
{
	p_i2c_int_cb           			Evt_CB;
	RY_I2C_CAPABILITIES             I2C_Cap;
	volatile I2C_DRIVER_STATE       Driver_State;
	RY_POWER_STATE                 Pwr_State;
	uint8_t*                        middleware_rx_buf;
	uint8_t*                        middleware_tx_buf;
	/* flags for flow control */
	// salve mode only
	uint32_t                        last_rx_data_count;
	// Xfer_Data_Rd_Buf[] for middleware read out, salve mode only
	uint32_t                        middleware_rx_ptr;
	// salve mode only
	uint8_t                         nack_assert;
	uint32_t                        FIFO_Depth;
	uint32_t                        Slave_Addr;
	uint32_t                        Xfer_Wt_Num;
	uint32_t                        Xfer_Rd_Num;
	// write pointer
	uint32_t                        Xfered_Data_Wt_Ptr;
	// read pointer
	uint32_t                        Xfered_Data_Rd_Ptr;
	// read pointer overwrite
	uint32_t                        Xfered_Data_Rd_Ptr_Ow;
	uint8_t                         *Xfer_Data_Rd_Buf;//[MAX_XFER_SZ];
	uint32_t                        Slave_Rx_Cmpl_Ctrl_Reg_Val;
	volatile RY_I2C_STATUS         	Status;
	uint32_t                        Xfer_Cmpl_Count;
}I2C_INFO;

// I2C DMA
typedef const struct _I2C_DMA
{
	uint8_t         channel;          	// DMA Channel
	uint8_t         reqsel;           	// DMA request selection
	p_dma_int_cb	cb_event;         	// DMA Event callback
}I2C_DMA;

// I2C Resource Configuration
typedef struct
{
	I2C_RegDef*		reg;               	// I2C register interface
	uint32_t		irq_num;      		// I2C IRQ number
	I2C_DMA*		dma_tx;
	I2C_DMA*		dma_rx;
	I2C_INFO*		info;              	// Run-Time control information
}const I2C_RESOURCES;

/**
\brief Access structure of the I2C Driver.
*/
typedef struct _RY_DRIVER_I2C {
  RY_I2C_CAPABILITIES	(*GetCapabilities)(void);                                      	///< Pointer to \ref RY_I2C_GetCapabilities : Get driver capabilities.
  int32_t				(*Initialize)     (p_i2c_int_cb cb_event);                     	///< Pointer to \ref RY_I2C_Initialize : Initialize I2C Interface.
  int32_t				(*Uninitialize)   (void);                                      	///< Pointer to \ref RY_I2C_Uninitialize : De-initialize I2C Interface.
  int32_t				(*PowerControl)   (RY_POWER_STATE state);                      	///< Pointer to \ref RY_I2C_PowerControl : Control I2C Interface Power.
  int32_t				(*MasterTransmit) (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending); ///< Pointer to \ref RY_I2C_MasterTransmit : Start transmitting data as I2C Master.
  int32_t				(*MasterReceive)  (uint32_t addr,       uint8_t *data, uint32_t num, bool xfer_pending); ///< Pointer to \ref RY_I2C_MasterReceive : Start receiving data as I2C Master.
  int32_t				(*SlaveTransmit)  (const uint8_t *data, uint32_t num);         	///< Pointer to \ref RY_I2C_SlaveTransmit : Start transmitting data as I2C Slave.
  int32_t				(*SlaveReceive)   (		uint8_t *data, uint32_t num);          	///< Pointer to \ref RY_I2C_SlaveReceive : Start receiving data as I2C Slave.
  int32_t				(*GetDataCount)   (void);                                     	///< Pointer to \ref RY_I2C_GetDataCount : Get transferred data count.
  int32_t				(*Control)        (uint32_t control, uint32_t arg);            	///< Pointer to \ref RY_I2C_Control : Control I2C Interface.
  RY_I2C_STATUS			(*GetStatus)      (void);                                       ///< Pointer to \ref RY_I2C_GetStatus : Get I2C status.
} const RY_DRIVER_I2C;


/****** I2C Event *****/
#define RY_I2C_EVENT_TRANSFER_DONE       (1UL << 0)  	///< Master/Slave Transmit/Receive finished
#define RY_I2C_EVENT_TRANSFER_INCOMPLETE (1UL << 1)  	///< Master/Slave Transmit/Receive incomplete transfer
#define RY_I2C_EVENT_SLAVE_TRANSMIT      (1UL << 2)  	///< Slave Transmit operation requested
#define RY_I2C_EVENT_SLAVE_RECEIVE       (1UL << 3)  	///< Slave Receive operation requested
#define RY_I2C_EVENT_ADDRESS_NACK        (1UL << 4)  	///< Address not acknowledged from Slave
#define RY_I2C_EVENT_GENERAL_CALL        (1UL << 5)  	///< General Call indication
#define RY_I2C_EVENT_ARBITRATION_LOST    (1UL << 6)  	///< Master lost arbitration
#define RY_I2C_EVENT_BUS_ERROR           (1UL << 7)  	///< Bus error detected (START/STOP at illegal position)
#define RY_I2C_EVENT_BUS_CLEAR           (1UL << 8)  	///< Bus clear finished


//I2C
typedef enum
{
	I2C_IO_GP0 = 0,
	I2C_IO_GP1 = 1,
	I2C_IO_GP2 = 2,
	I2C_IO_GP3 = 3,
	I2C_IO_GP4 = 4,
	I2C_IO_GP5 = 5,
	I2C_IO_GP6 = 6,
	I2C_IO_GP7 = 7,
	I2C_IO_GP8 = 8,
	I2C_IO_GP9 = 9,
	I2C_IO_GP10 = 10,
	I2C_IO_GP11 = 11,
	I2C_IO_GP12 = 12,
	I2C_IO_GP13 = 13,
	I2C_IO_GP14 = 14,
	I2C_IO_GP15 = 15,
	I2C_IO_GP_MAX,
}i2c_multi_io;

extern RY_DRIVER_I2C Driver_I2C0;
extern RY_DRIVER_I2C Driver_I2C1;

extern unsigned int i2c_io_multi_config(RYRV_I2C_NO i2c_no, i2c_multi_io i2c_gp);

#endif /* __GTRY_I2C_DRV_H__ */
