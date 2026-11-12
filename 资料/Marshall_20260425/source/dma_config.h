/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __DMA_CONFIG_H__
#define __DMA_CONFIG_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SPI7_MASTER            			SPI7
#define SPI7_MASTER_IRQ        			FLEXCOMM7_IRQn
#define SPI7_MASTER_CLK_SRC   			kCLOCK_Flexcomm7
#define SPI7_MASTER_CLK_FREQ   			CLOCK_GetFlexCommClkFreq(7U)
#define SPI7_SSEL              			0
#define EXAMPLE_DMA                   	DMA0
#define SPI7_MASTER_RX_CHANNEL 			18								//DMA channel had been defined in datasheet, Every flexcom has a fixed channel.
#define SPI7_MASTER_TX_CHANNEL 			19
#define EXAMPLE_MASTER_SPI_SPOL       	kSPI_SpolActiveAllLow

#define SPI6_MASTER            			SPI6
#define SPI6_MASTER_IRQ        			FLEXCOMM6_IRQn
#define SPI6_MASTER_CLK_SRC    			kCLOCK_Flexcomm6
#define SPI6_MASTER_CLK_FREQ   			CLOCK_GetFlexCommClkFreq(6U)
#define SPI6_SSEL              			0
#define EXAMPLE_DMA                   	DMA0
#define SPI6_MASTER_RX_CHANNEL 			16								//DMA channel had been defined in datasheet, Every flexcom has a fixed channel.
#define SPI6_MASTER_TX_CHANNEL 			17
#define EXAMPLE_MASTER_SPI_SPOL       	kSPI_SpolActiveAllLow

#define SPI5_MASTER            			SPI5
#define SPI5_MASTER_IRQ        			FLEXCOMM5_IRQn
#define SPI5_MASTER_CLK_SRC    			kCLOCK_Flexcomm5
#define SPI5_MASTER_CLK_FREQ   			CLOCK_GetFlexCommClkFreq(5U)
#define SPI5_SSEL              			0
#define EXAMPLE_DMA                   	DMA0
#define SPI5_MASTER_RX_CHANNEL 			14								//DMA channel had been defined in datasheet, Every flexcom has a fixed channel.
#define SPI5_MASTER_TX_CHANNEL 			15
#define EXAMPLE_MASTER_SPI_SPOL       	kSPI_SpolActiveAllLow

#define SPI4_MASTER           	 		SPI4
#define SPI4_MASTER_IRQ        			FLEXCOMM4_IRQn
#define SPI4_MASTER_CLK_SRC    			kCLOCK_Flexcomm4
#define SPI4_MASTER_CLK_FREQ   			CLOCK_GetFlexCommClkFreq(4U)
#define SPI4_SSEL             			0
#define EXAMPLE_DMA                   	DMA0
#define SPI4_MASTER_RX_CHANNEL 			12								//DMA channel had been defined in datasheet, Every flexcom has a fixed channel.
#define SPI4_MASTER_TX_CHANNEL 			13
#define EXAMPLE_MASTER_SPI_SPOL       	kSPI_SpolActiveAllLow

#define SPI3_MASTER            			SPI3
#define SPI3_MASTER_IRQ        			FLEXCOMM3_IRQn
#define SPI3_MASTER_CLK_SRC    			kCLOCK_Flexcomm3
#define SPI3_MASTER_CLK_FREQ   			CLOCK_GetFlexCommClkFreq(3U)
#define SPI3_SSEL              			0
#define EXAMPLE_DMA                   	DMA0
#define SPI3_MASTER_RX_CHANNEL 			8								//DMA channel had been defined in datasheet, Every flexcom has a fixed channel.
#define SPI3_MASTER_TX_CHANNEL 			9
#define EXAMPLE_MASTER_SPI_SPOL       	kSPI_SpolActiveAllLow

#define SPI2_MASTER            			SPI2
#define SPI2_MASTER_IRQ        			FLEXCOMM2_IRQn
#define SPI2_MASTER_CLK_SRC    			kCLOCK_Flexcomm2
#define SPI2_MASTER_CLK_FREQ   			CLOCK_GetFlexCommClkFreq(2U)
#define SPI2_SSEL              			0
#define EXAMPLE_DMA                   	DMA0
#define SPI2_MASTER_RX_CHANNEL 			10								//DMA channel had been defined in datasheet, Every flexcom has a fixed channel.
#define SPI2_MASTER_TX_CHANNEL 			11
#define EXAMPLE_MASTER_SPI_SPOL       	kSPI_SpolActiveAllLow

#define SPI1_MASTER            			SPI1
#define SPI1_MASTER_IRQ        			FLEXCOMM1_IRQn
#define SPI1_MASTER_CLK_SRC    			kCLOCK_Flexcomm1
#define SPI1_MASTER_CLK_FREQ   			CLOCK_GetFlexCommClkFreq(1U)
#define SPI1_SSEL              			0
#define EXAMPLE_DMA                   	DMA0
#define SPI1_MASTER_RX_CHANNEL 			6								//DMA channel had been defined in datasheet, Every flexcom has a fixed channel.
#define SPI1_MASTER_TX_CHANNEL 			7
#define EXAMPLE_MASTER_SPI_SPOL       	kSPI_SpolActiveAllLow

#define SPI0_MASTER            			SPI0
#define SPI0_MASTER_IRQ        			FLEXCOMM0_IRQn
#define SPI0_MASTER_CLK_SRC    			kCLOCK_Flexcomm0
#define SPI0_MASTER_CLK_FREQ   			CLOCK_GetFlexCommClkFreq(0U)
#define SPI0_SSEL              			0
#define EXAMPLE_DMA                   	DMA0
#define SPI0_MASTER_RX_CHANNEL 			4								//DMA channel had been defined in datasheet, Every flexcom has a fixed channel.
#define SPI0_MASTER_TX_CHANNEL 			5
#define EXAMPLE_MASTER_SPI_SPOL       	kSPI_SpolActiveAllLow

//TRANSFER_SIZE is the longest for DMA
#define TRANSFER_SIZE 21U /*! Transfer dataSize */
#define TRANSFER_Group 6U /*! Transfer dataSize */
//It is corresponded to the number of hall sensor;
#define SPI1_TRANSFER_SIZE 21U			//12U /*! Transfer dataSize */
#define SPI4_TRANSFER_SIZE 21U			//12U /*! Transfer dataSize */
#define SPI5_TRANSFER_SIZE 21U			//15U /*! Transfer dataSize */
#define SPI7_TRANSFER_SIZE 21U			//15U /*! Transfer dataSize */

#define SPI0_TRANSFER_SIZE 13U /*! Transfer dataSize */
#define SPI2_TRANSFER_SIZE 17U /*! Transfer dataSize */
#define SPI3_TRANSFER_SIZE 13U /*! Transfer dataSize */
#define SPI6_TRANSFER_SIZE 13U /*! Transfer dataSize */

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern volatile bool isTransferCompleted_0;
extern volatile bool isTransferCompleted_1;
extern volatile bool isTransferCompleted_2;
extern volatile bool isTransferCompleted_3;
extern volatile bool isTransferCompleted_4;
extern volatile bool isTransferCompleted_5;
extern volatile bool isTransferCompleted_6;
extern volatile bool isTransferCompleted_7;


extern uint32_t DMA_SPI_TX_1_Buffer[TRANSFER_SIZE];
extern uint32_t DMA_SPI_TX_2_Buffer[TRANSFER_SIZE];
extern uint32_t DMA_SPI_TX_3_Buffer[TRANSFER_SIZE];
extern uint32_t DMA_SPI_TX_4_Buffer[TRANSFER_SIZE];
extern uint32_t DMA_SPI_TX_5_Buffer[TRANSFER_SIZE];
extern uint32_t DMA_SPI_TX_6_Buffer[TRANSFER_SIZE];

extern uint16_t DMA_SPI_RX_1_Buffer[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_2_Buffer[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_3_Buffer[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_4_Buffer[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_5_Buffer[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_6_Buffer[TRANSFER_SIZE];

extern uint16_t DMA_SPI_RX_1_Buffer_BackUp[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_2_Buffer_BackUp[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_3_Buffer_BackUp[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_4_Buffer_BackUp[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_5_Buffer_BackUp[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_6_Buffer_BackUp[TRANSFER_SIZE];

extern uint16_t DMA_SPI_RX_1_Buffer_Store[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_2_Buffer_Store[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_3_Buffer_Store[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_4_Buffer_Store[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_5_Buffer_Store[TRANSFER_SIZE];
extern uint16_t DMA_SPI_RX_6_Buffer_Store[TRANSFER_SIZE];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

extern void DMA_CONFIG(void);
extern void DMA_StartTransfer_Once(void);
extern void DMA_StartTransfer_OnceFor_RawData(void);
extern void DMA_StartTransfer_Once_TESTAPP(void);
extern void DMA_StartTransfer_Suspend(void);


#endif /* __DMA_CONFIG_H__ */
