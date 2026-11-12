/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_spi.h"
#include "fsl_dma.h"
#include "fsl_spi_dma.h"
#include "pin_mux.h"
#include "board.h"
#include "dma_config.h"
#include "usb_misc.h"
#include "hall_spi.h"
#include "led.h"
#include "hid_keyboard.h"
/*******************************************************************************
 * Lightweight SPI1 diagnostics switches
 ******************************************************************************/
#define SPI1_DIAG_ENABLE (1U)
/* Set to 1 only for short on-board probing. */
#define SPI1_BLOCKING_PROBE_ENABLE (0U)

#if SPI1_DIAG_ENABLE
volatile uint32_t g_spi1_diag_tag = 0;
volatile uint32_t g_spi1_diag_cfg = 0;
volatile uint32_t g_spi1_diag_stat = 0;
volatile uint32_t g_spi1_diag_fifocfg = 0;
volatile uint32_t g_spi1_diag_fifostat = 0;
volatile uint32_t g_spi1_diag_txrem = 0;
volatile uint32_t g_spi1_diag_rxrem = 0;
volatile uint32_t g_spi1_diag_dma_hwtrig = 0;
volatile uint32_t g_spi1_diag_probe_done = 0;
volatile uint32_t g_spi1_diag_probe_rx0 = 0;
volatile uint32_t g_spi1_diag_probe_rx1 = 0;

static void SPI1_DiagSnapshot(uint32_t tag)
{
//    g_spi1_diag_tag = tag;
//    g_spi1_diag_cfg = SPI1_MASTER->CFG;
//    g_spi1_diag_stat = SPI1_MASTER->STAT;
//    g_spi1_diag_fifocfg = SPI1_MASTER->FIFOCFG;
//    g_spi1_diag_fifostat = SPI1_MASTER->FIFOSTAT;
//    g_spi1_diag_txrem = DMA_GetRemainingBytes(spi1_masterHandle.txHandle->base, spi1_masterHandle.txHandle->channel);
//    g_spi1_diag_rxrem = DMA_GetRemainingBytes(spi1_masterHandle.rxHandle->base, spi1_masterHandle.rxHandle->channel);
//    g_spi1_diag_dma_hwtrig = SPI1_MASTER->FIFOTRIG;
}
#endif
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void SPI_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData);
static void EXAMPLE_MasterInit(void);
static void EXAMPLE_MasterDMASetup(void);
static void EXAMPLE_TransferDataCheck(void);

void DMA_CONFIG(void);
static void DMA_Variable_Init(void);
void DMA_StartTransfer_Once(void);


/*******************************************************************************
 * Variables
 ******************************************************************************/
uint16_t DMA_SPI_RX_1_Buffer[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_2_Buffer[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_3_Buffer[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_4_Buffer[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_5_Buffer[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_6_Buffer[TRANSFER_SIZE] = {0};

uint16_t DMA_SPI_RX_1_Buffer_Store[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_2_Buffer_Store[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_3_Buffer_Store[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_4_Buffer_Store[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_5_Buffer_Store[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_6_Buffer_Store[TRANSFER_SIZE] = {0};

uint16_t DMA_SPI_RX_1_Buffer_BackUp[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_2_Buffer_BackUp[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_3_Buffer_BackUp[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_4_Buffer_BackUp[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_5_Buffer_BackUp[TRANSFER_SIZE] = {0};
uint16_t DMA_SPI_RX_6_Buffer_BackUp[TRANSFER_SIZE] = {0};

uint32_t DMA_SPI_TX_1_Buffer[TRANSFER_SIZE] = {0};
uint32_t DMA_SPI_TX_2_Buffer[TRANSFER_SIZE] = {0};
uint32_t DMA_SPI_TX_3_Buffer[TRANSFER_SIZE] = {0};
uint32_t DMA_SPI_TX_4_Buffer[TRANSFER_SIZE] = {0};
uint32_t DMA_SPI_TX_5_Buffer[TRANSFER_SIZE] = {0};
uint32_t DMA_SPI_TX_6_Buffer[TRANSFER_SIZE] = {0};

spi_transfer_t spi0_masterXfer;
spi_transfer_t spi1_masterXfer;
spi_transfer_t spi2_masterXfer;
spi_transfer_t spi3_masterXfer;
spi_transfer_t spi4_masterXfer;
spi_transfer_t spi5_masterXfer;
spi_transfer_t spi6_masterXfer;
spi_transfer_t spi7_masterXfer;

dma_handle_t spi0_masterTxHandle;
dma_handle_t spi0_masterRxHandle;

dma_handle_t spi1_masterTxHandle;
dma_handle_t spi1_masterRxHandle;

dma_handle_t spi2_masterTxHandle;
dma_handle_t spi2_masterRxHandle;

dma_handle_t spi3_masterTxHandle;
dma_handle_t spi3_masterRxHandle;

dma_handle_t spi4_masterTxHandle;
dma_handle_t spi4_masterRxHandle;

dma_handle_t spi5_masterTxHandle;
dma_handle_t spi5_masterRxHandle;

dma_handle_t spi6_masterTxHandle;
dma_handle_t spi6_masterRxHandle;

dma_handle_t spi7_masterTxHandle;
dma_handle_t spi7_masterRxHandle;

spi_dma_handle_t spi0_masterHandle;
spi_dma_handle_t spi1_masterHandle;
spi_dma_handle_t spi2_masterHandle;
spi_dma_handle_t spi3_masterHandle;
spi_dma_handle_t spi4_masterHandle;
spi_dma_handle_t spi5_masterHandle;
spi_dma_handle_t spi6_masterHandle;
spi_dma_handle_t spi7_masterHandle;

volatile bool isTransferCompleted_0 = false;
volatile bool isTransferCompleted_1 = false;
volatile bool isTransferCompleted_2 = false;
volatile bool isTransferCompleted_3 = false;
volatile bool isTransferCompleted_4 = false;
volatile bool isTransferCompleted_5 = false;
volatile bool isTransferCompleted_6 = false;
volatile bool isTransferCompleted_7 = false;



/*******************************************************************************
 * Code
 ******************************************************************************/


/*******************************************************************************
 * SPI callback function.
 * Note:Do not put any log in callback function,because it will cause the callback to take longer;
 ******************************************************************************/
static void SPI0_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        isTransferCompleted_0 = true;
        //usb_echo("SPI0 Success\r\n");
        //TEST_PIN_SET_LOW();
    }
}

static void SPI1_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        isTransferCompleted_1 = true;
        //usb_echo("SPI1 Success\r\n");
        //TEST_PIN_SET_LOW();
    }
}

static void SPI2_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        isTransferCompleted_2 = true;
        //usb_echo("SPI2 Success\r\n");
        //TEST_PIN_SET_LOW();
    }
}

static void SPI3_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        isTransferCompleted_3 = true;
        //usb_echo("SPI3 Success\r\n");
        //TEST_PIN_SET_LOW();
    }
}

static void SPI4_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        isTransferCompleted_4 = true;
        //usb_echo("SPI4 Success\r\n");
        //TEST_PIN_SET_LOW();
    }
}

static void SPI5_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        isTransferCompleted_5 = true;
        //usb_echo("SPI5 Success\r\n");

    }
}

static void SPI6_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        isTransferCompleted_6 = true;
        //usb_echo("SPI6 Success\r\n");
   }
}
//Note:Do not put any log in callback function;
static void SPI7_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        isTransferCompleted_7 = true;
        //usb_echo("SPI7 Success\r\n");
    }
}
//--------------------------------------------------------------------------------------------------------------------------------
/* DMA Init*/
void DMA_CONFIG(void)
{
	/* Initialize SPI master with configuration. */
	EXAMPLE_MasterInit();

	/* Set up DMA for SPI master TX and RX channel. */
	EXAMPLE_MasterDMASetup();

	///* Prepare DMA and start SPI master transfer in DMA way. */
	DMA_Variable_Init();

	/* De-initialzie the DMA instance. */
	//DMA_Deinit(EXAMPLE_DMA);

	/* De-initialize the SPI instance. */
	//SPI_Deinit(SPI0_MASTER);
	//SPI_Deinit(SPI1_MASTER);
	//SPI_Deinit(SPI2_MASTER);
	//SPI_Deinit(SPI3_MASTER);
	//SPI_Deinit(SPI4_MASTER);
}

/*!
 * DMA的参数设置,设置初始化TX/RX buffer,
 */
void DMA_Variable_Init(void)
{
    uint32_t i = 0U;

    /* Set up the transfer data */
    for (i = 0U; i < TRANSFER_SIZE; i++)
    {
        /* SPI is configured for 8 bits transfer - set only lower 8 bits of buffers */
        DMA_SPI_RX_1_Buffer[i] = 0U;	//add
        DMA_SPI_RX_2_Buffer[i] = 0U;	//add
        DMA_SPI_RX_3_Buffer[i] = 0U;	//add
        DMA_SPI_RX_4_Buffer[i] = 0U;	//add

        DMA_SPI_TX_1_Buffer[i] = i+1 + 0x0F0F0000;
        DMA_SPI_TX_2_Buffer[i] = i+1 + 0x0F0F0000;	//add
        DMA_SPI_TX_3_Buffer[i] = i+1 + 0x0F0F0000;	//add
        DMA_SPI_TX_4_Buffer[i] = i+1 + 0x0F0F0000;	//add

    }
	DMA_SPI_TX_1_Buffer[SPI1_TRANSFER_SIZE-1] = 0x0F1F0021;
	DMA_SPI_TX_2_Buffer[SPI4_TRANSFER_SIZE-1] = 0x0F1F0021;
	DMA_SPI_TX_3_Buffer[SPI5_TRANSFER_SIZE-1] = 0x0F1F0021;
	DMA_SPI_TX_4_Buffer[SPI7_TRANSFER_SIZE-1] = 0x0F1F0021;

    /* Set up handle for spi master */
    /* 放一个读频率的时钟在这，防止这里没有时钟变化但是会给sensor发数据 */
    Hall_CLK_Command_SET(TD7_DELAY_US,TD10_DELAY_US);

    SPI_MasterTransferCreateHandleDMA_NoCallback(SPI1_MASTER, &spi1_masterHandle, NULL, NULL, &spi1_masterTxHandle,
                                      &spi1_masterRxHandle);

    SPI_MasterTransferCreateHandleDMA(SPI5_MASTER, &spi5_masterHandle, SPI5_MasterUserCallback, NULL, &spi5_masterTxHandle,
                                      &spi5_masterRxHandle);

    SPI_MasterTransferCreateHandleDMA_NoCallback(SPI4_MASTER, &spi4_masterHandle, NULL, NULL, &spi4_masterTxHandle,
                                          &spi4_masterRxHandle);

    SPI_MasterTransferCreateHandleDMA_NoCallback(SPI7_MASTER, &spi7_masterHandle, NULL, NULL, &spi7_masterTxHandle,
                                      &spi7_masterRxHandle);

    isTransferCompleted_1 = false;
    isTransferCompleted_4 = false;
    isTransferCompleted_5 = false;
    isTransferCompleted_7 = false;


    spi1_masterXfer.txData      = (uint8_t *)&DMA_SPI_TX_1_Buffer;
    spi1_masterXfer.rxData      = (uint8_t *)&DMA_SPI_RX_1_Buffer;
    spi1_masterXfer.dataSize    = SPI1_TRANSFER_SIZE;	//TRANSFER_SIZE;
    spi1_masterXfer.configFlags = kSPI_FrameAssert;
    SPI_MasterTransferDMA_First(SPI1_MASTER, &spi1_masterHandle, &spi1_masterXfer);

   //add
   spi4_masterXfer.txData      = (uint8_t *)&DMA_SPI_TX_2_Buffer;
   spi4_masterXfer.rxData      = (uint8_t *)&DMA_SPI_RX_2_Buffer;
   spi4_masterXfer.dataSize    = SPI4_TRANSFER_SIZE;	//TRANSFER_SIZE;
   spi4_masterXfer.configFlags = kSPI_FrameAssert;
   SPI_MasterTransferDMA_First(SPI4_MASTER, &spi4_masterHandle, &spi4_masterXfer);

   //add
    spi5_masterXfer.txData      = (uint8_t *)&DMA_SPI_TX_3_Buffer;
    spi5_masterXfer.rxData      = (uint8_t *)&DMA_SPI_RX_3_Buffer;
    spi5_masterXfer.dataSize    = SPI5_TRANSFER_SIZE;	//TRANSFER_SIZE;
    spi5_masterXfer.configFlags = kSPI_FrameAssert;
    SPI_MasterTransferDMA_First(SPI5_MASTER, &spi5_masterHandle, &spi5_masterXfer);

   //add
   spi7_masterXfer.txData      = (uint8_t *)&DMA_SPI_TX_4_Buffer;
   spi7_masterXfer.rxData      = (uint8_t *)&DMA_SPI_RX_4_Buffer;
   spi7_masterXfer.dataSize    = SPI7_TRANSFER_SIZE;	//TRANSFER_SIZE;
   spi7_masterXfer.configFlags = kSPI_FrameAssert;
   SPI_MasterTransferDMA_First(SPI7_MASTER, &spi7_masterHandle, &spi7_masterXfer);

#if SPI1_DIAG_ENABLE
//   SPI1_DiagSnapshot(0x101U);
#if SPI1_BLOCKING_PROBE_ENABLE
   {
	   uint8_t probeTx[4] = {0xA5, 0x5A, 0x3C, 0xC3};
	   uint8_t probeRx[4] = {0};
	   spi_transfer_t probeXfer;
	   probeXfer.txData = probeTx;
	   probeXfer.rxData = probeRx;
	   probeXfer.dataSize = sizeof(probeTx);
	   probeXfer.configFlags = kSPI_FrameAssert;
	   (void)SPI_MasterTransferBlocking(SPI1_MASTER, &probeXfer);
	   g_spi1_diag_probe_done = 1U;
	   g_spi1_diag_probe_rx0 = probeRx[0];
	   g_spi1_diag_probe_rx1 = probeRx[1];
	   SPI1_DiagSnapshot(0x102U);
   }
#endif
#endif
}

/* SPI Master Init*/
static void EXAMPLE_MasterInit(void)
{
	/* SPI init */
	uint32_t srcClock_Hz = 0U;
	spi_master_config_t masterConfig;
	srcClock_Hz = SPI1_MASTER_CLK_FREQ;

	SPI_MasterGetDefaultConfig(&masterConfig);
	masterConfig.sselNum = (spi_ssel_t)SPI1_SSEL;
	masterConfig.sselPol = (spi_spol_t)EXAMPLE_MASTER_SPI_SPOL;
	masterConfig.baudRate_Bps = 12000000;
	/* Use default CPHA=0 (Mode 0) for all channels - same transmission format. */
	//SPI_MasterInit(SPI5_MASTER, &masterConfig, srcClock_Hz);
//	SPI_MasterInit(SPI6_MASTER, &masterConfig, srcClock_Hz);
//	SPI_MasterInit(SPI4_MASTER, &masterConfig, srcClock_Hz);
	SPI_MasterInit(SPI7_MASTER, &masterConfig, srcClock_Hz);
	SPI_MasterInit(SPI5_MASTER, &masterConfig, srcClock_Hz);
	SPI_MasterInit(SPI4_MASTER, &masterConfig, srcClock_Hz);
	SPI_MasterInit(SPI1_MASTER, &masterConfig, srcClock_Hz);
}

static void EXAMPLE_MasterDMASetup(void)
{
    /* DMA init */
    DMA_Init(EXAMPLE_DMA);
    /* Configure the DMA channel,priority and handle. */
    DMA_EnableChannel(EXAMPLE_DMA, SPI1_MASTER_TX_CHANNEL);
    DMA_EnableChannel(EXAMPLE_DMA, SPI1_MASTER_RX_CHANNEL);
    DMA_SetChannelPriority(EXAMPLE_DMA, SPI1_MASTER_TX_CHANNEL, kDMA_ChannelPriority3);
    DMA_SetChannelPriority(EXAMPLE_DMA, SPI1_MASTER_RX_CHANNEL, kDMA_ChannelPriority2);
    DMA_CreateHandle(&spi1_masterTxHandle, EXAMPLE_DMA, SPI1_MASTER_TX_CHANNEL);
    DMA_CreateHandle(&spi1_masterRxHandle, EXAMPLE_DMA, SPI1_MASTER_RX_CHANNEL);

    //add
    /* Configure the DMA channel,priority and handle. */
    DMA_EnableChannel(EXAMPLE_DMA, SPI4_MASTER_TX_CHANNEL);
    DMA_EnableChannel(EXAMPLE_DMA, SPI4_MASTER_RX_CHANNEL);
    DMA_SetChannelPriority(EXAMPLE_DMA, SPI4_MASTER_TX_CHANNEL, kDMA_ChannelPriority3);
    DMA_SetChannelPriority(EXAMPLE_DMA, SPI4_MASTER_RX_CHANNEL, kDMA_ChannelPriority2);
    DMA_CreateHandle(&spi4_masterTxHandle, EXAMPLE_DMA, SPI4_MASTER_TX_CHANNEL);
    DMA_CreateHandle(&spi4_masterRxHandle, EXAMPLE_DMA, SPI4_MASTER_RX_CHANNEL);

    //add
    /* Configure the DMA channel,priority and handle. */
    DMA_EnableChannel(EXAMPLE_DMA, SPI5_MASTER_TX_CHANNEL);
    DMA_EnableChannel(EXAMPLE_DMA, SPI5_MASTER_RX_CHANNEL);
    DMA_SetChannelPriority(EXAMPLE_DMA, SPI5_MASTER_TX_CHANNEL, kDMA_ChannelPriority3);
    DMA_SetChannelPriority(EXAMPLE_DMA, SPI5_MASTER_RX_CHANNEL, kDMA_ChannelPriority2);
    DMA_CreateHandle(&spi5_masterTxHandle, EXAMPLE_DMA, SPI5_MASTER_TX_CHANNEL);
    DMA_CreateHandle(&spi5_masterRxHandle, EXAMPLE_DMA, SPI5_MASTER_RX_CHANNEL);

    //add
    /* Configure the DMA channel,priority and handle. */
    DMA_EnableChannel(EXAMPLE_DMA, SPI7_MASTER_TX_CHANNEL);
    DMA_EnableChannel(EXAMPLE_DMA, SPI7_MASTER_RX_CHANNEL);
    DMA_SetChannelPriority(EXAMPLE_DMA, SPI7_MASTER_TX_CHANNEL, kDMA_ChannelPriority3);
    DMA_SetChannelPriority(EXAMPLE_DMA, SPI7_MASTER_RX_CHANNEL, kDMA_ChannelPriority2);
    DMA_CreateHandle(&spi7_masterTxHandle, EXAMPLE_DMA, SPI7_MASTER_TX_CHANNEL);
    DMA_CreateHandle(&spi7_masterRxHandle, EXAMPLE_DMA, SPI7_MASTER_RX_CHANNEL);
}

//#pragma OT(3)//用来优化成使用编译器优化等级3

/*!
 * DMA_StartTransfer_Once: Transfer data to slave once via DMA;
 */
void DMA_StartTransfer_Once(void)
{
	uint32_t time_out = 10000000;
	isTransferCompleted_1 = false;
	isTransferCompleted_4 = false;
	isTransferCompleted_5 = false;
	isTransferCompleted_7 = false;
	//------------------------------------------------------------------------------------------
#if SPI1_DIAG_ENABLE
//    SPI1_DiagSnapshot(0x201U);
#endif
    SPI_MasterTransferDMA_OPT(SPI1_MASTER, &spi1_masterHandle, &spi1_masterXfer);
    SPI_MasterTransferDMA_OPT(SPI4_MASTER, &spi4_masterHandle, &spi4_masterXfer);
    SPI_MasterTransferDMA_OPT(SPI5_MASTER, &spi5_masterHandle, &spi5_masterXfer);
    SPI_MasterTransferDMA_OPT(SPI7_MASTER, &spi7_masterHandle, &spi7_masterXfer);
    //------------------------------------------------------------------------------------------
#if SPI1_DIAG_ENABLE
//    SPI1_DiagSnapshot(0x202U);
#endif
    DMA_StartTransfer(spi1_masterHandle.txHandle);
    DMA_StartTransfer(spi4_masterHandle.txHandle);
    DMA_StartTransfer(spi5_masterHandle.txHandle);
    DMA_StartTransfer(spi7_masterHandle.txHandle);

#if SPI1_DIAG_ENABLE
//    SPI1_DiagSnapshot(0x203U);
#endif

    //------------------------------------------------------------------------------------------
    /* 只需要保证用来做退出DMA判断的callback在产生的这个callback的时候其他的SPI已经传完数据就行，不需要等全部的callback产生再退出.
     * 可以通过逻辑分析仪看哪个callback产生回调的时候全部的SPI已经传输完成. */
    while((isTransferCompleted_5 == false) && (time_out))
    {
		time_out--;
		if(!time_out)
		{
			return;
		}
		hall_mcu_delay_us(TD10_DELAY_US);
	}
    //------------------------------------------------------------------------------------------
}
/*!
 * FUNTION: DMA_StartTransfer_OnceFor_RawData(void)
 * Description:Normally,CLK pin should be pulled up and down for a while time, but 8K requires too much time,
 * so the duration of CLK pulling up and down is replaced by DMA FUNCTION.
 */
void DMA_StartTransfer_OnceFor_RawData(void)
{

	uint32_t Tick_time_out = 10000000;
	uint8_t Flag_DoOnce_InFun = 1;

	Hall_CLK_Command_SET(TD1_DELAY_US_FreqDouble,TD2_CONTIUNS_DELAY_US_FreqDouble);

	isTransferCompleted_1 = false;
	isTransferCompleted_4 = false;
	isTransferCompleted_5 = false;
	isTransferCompleted_7 = false;
	//------------------------------------------------------------------------------------------
	SPI_MasterTransferDMA_OPT(SPI1_MASTER, &spi1_masterHandle, &spi1_masterXfer);

    SPI_MasterTransferDMA_OPT(SPI4_MASTER, &spi4_masterHandle, &spi4_masterXfer);

    SPI_MasterTransferDMA_OPT(SPI5_MASTER, &spi5_masterHandle, &spi5_masterXfer);

    SPI_MasterTransferDMA_OPT(SPI7_MASTER, &spi7_masterHandle, &spi7_masterXfer);
    //------------------------------------------------------------------------------------------
	DMA_StartTransfer(spi1_masterHandle.txHandle);
	DMA_StartTransfer(spi4_masterHandle.txHandle);
	DMA_StartTransfer(spi5_masterHandle.txHandle);
	DMA_StartTransfer(spi7_masterHandle.txHandle);

    //------------------------------------------------------------------------------------------
    /* 只需要保证用来做退出DMA判断的callback在产生的这个callback的时候其他的SPI已经传完数据就行，不需要等全部的callback产生再退出.
     * 可以通过逻辑分析仪看哪个callback产生回调的时候全部的SPI已经传输完成. */
    while((isTransferCompleted_5 == false) && (Tick_time_out))
    {
       	Tick_time_out--;
		//------------------------------------------------------------------------------------------
		/* 每次循环只做一次，不然没法算时间 */
		/* 为了利用时间，RGB的颜色数据处理在等DMA期间做，做不完下次进来再从上一次的数据开始 */
		if(Flag_DoOnce_InFun)
		{
			Flag_DoOnce_InFun = 0;

			LEDMode_RGBdataProcess();

			//Hall_Baseline_Cal_Process();

			//TGL_TIMECNT_Process();
		}
		//------------------------------------------------------------------------------------------
    }
}

/*!
 * FUNTION: DMA_StartTransfer_OnceFor_RawData(void)
 * Description:Normally,CLK pin should be pulled up and down for a while time, but 8K requires too much time,
 * so the duration of CLK pulling up and down is replaced by DMA FUNCTION.
 */
void DMA_StartTransfer_Once_TESTAPP(void)
{
	uint32_t Tick_time_out = 10000000;
	uint8_t Keyindex = 0;
	uint8_t Flag_DoOnce_InFun = 1;

	Hall_CLK_Command_SET(TD1_DELAY_US_FreqDouble,TD2_CONTIUNS_DELAY_US_FreqDouble);

	isTransferCompleted_1 = false;
	isTransferCompleted_4 = false;
	isTransferCompleted_5 = false;
	isTransferCompleted_7 = false;
	//------------------------------------------------------------------------------------------
    SPI_MasterTransferDMA_OPT(SPI1_MASTER, &spi1_masterHandle, &spi1_masterXfer);

    SPI_MasterTransferDMA_OPT(SPI4_MASTER, &spi4_masterHandle, &spi4_masterXfer);

	SPI_MasterTransferDMA_OPT(SPI5_MASTER, &spi5_masterHandle, &spi5_masterXfer);

    SPI_MasterTransferDMA_OPT(SPI7_MASTER, &spi7_masterHandle, &spi7_masterXfer);
    //------------------------------------------------------------------------------------------
    DMA_StartTransfer(spi1_masterHandle.txHandle);
    DMA_StartTransfer(spi4_masterHandle.txHandle);
    DMA_StartTransfer(spi5_masterHandle.txHandle);
    DMA_StartTransfer(spi7_masterHandle.txHandle);
    //------------------------------------------------------------------------------------------
    /* 只需要保证用来做退出DMA判断的callback在产生的这个callback的时候其他的SPI已经传完数据就行，不需要等全部的callback产生再退出.
     * 可以通过逻辑分析仪看哪个callback产生回调的时候全部的SPI已经传输完成. */
    while((isTransferCompleted_5 == false) && (Tick_time_out))
	{
    	Tick_time_out--;
    	//------------------------------------------------------------------------------------------
    	/* 每次循环只做一次，不然没法算时间 */
    	/* 为了利用时间，RGB的颜色数据处理在等DMA期间做，做不完下次进来再从上一次的数据开始 */
		/* 测试模式也计算Baseline */
		if(Flag_DoOnce_InFun)
		{
			Flag_DoOnce_InFun = 0;
			Hall_Baseline_Cal_Process();
		}
    	//------------------------------------------------------------------------------------------
	}
    //------------------------------------------------------------------------------------------
}

/*!
 * FUNTION: DMA_StartTransfer_OnceFor_RawData(void)
 * Description:Normally,CLK pin should be pulled up and down for a while time, but 8K requires too much time,
 * so the duration of CLK pulling up and down is replaced by DMA FUNCTION.
 */
void DMA_StartTransfer_Suspend(void)
{
	uint32_t Tick_time_out = 1000000;
	uint8_t Keyindex = 0;
	uint8_t Flag_DoOnce_InFun = 1;
	//------------------------------------------------------------------------------------------
	/* Disable global interrupt */
	//uint32_t regPrimask = DisableGlobalIRQ();
	//------------------------------------------------------------------------------------------
	/* Set all hall sensor SPI clock as GPIO */
	SET_ALLCLKPins_AS_GPIO();
	//SET_ALLCLKPins_AS_OUTPUT();
	//------------------------------------------------------------------------------------------
	/* Set all hall sensor SPI clock as high level */
	SET_ALLCLKPins_HIGH();
	uint32_t regPrimask = DisableGlobalIRQ();
	hall_mcu_delay_us(16);
	//------------------------------------------------------------------------------------------
	isTransferCompleted_1 = false;
	isTransferCompleted_4 = false;
	isTransferCompleted_5 = false;
	isTransferCompleted_7 = false;
	//------------------------------------------------------------------------------------------
	/* Set all hall sensor SPI clock as low level */
    SET_ALLCLKPins_LOW();
	//------------------------------------------------------------------------------------------
    SPI_MasterTransferDMA_OPT(SPI1_MASTER, &spi1_masterHandle, &spi1_masterXfer);

    SPI_MasterTransferDMA_OPT(SPI4_MASTER, &spi4_masterHandle, &spi4_masterXfer);

	SPI_MasterTransferDMA_OPT(SPI5_MASTER, &spi5_masterHandle, &spi5_masterXfer);

    SPI_MasterTransferDMA_OPT(SPI7_MASTER, &spi7_masterHandle, &spi7_masterXfer);
	//------------------------------------------------------------------------------------------
	/* Set all hall sensor SPI clock as SPI clock */
	SET_ALLCLKPins_AS_CLK();
	//------------------------------------------------------------------------------------------
	/* Enable global interrupt */
	EnableGlobalIRQ(regPrimask);
	//------------------------------------------------------------------------------------------
    DMA_StartTransfer(spi1_masterHandle.txHandle);
    DMA_StartTransfer(spi4_masterHandle.txHandle);
    DMA_StartTransfer(spi5_masterHandle.txHandle);
    DMA_StartTransfer(spi7_masterHandle.txHandle);
    //------------------------------------------------------------------------------------------
    /* 只需要保证用来做退出DMA判断的callback在产生的这个callback的时候其他的SPI已经传完数据就行，不需要等全部的callback产生再退出.
     * 可以通过逻辑分析仪看哪个callback产生回调的时候全部的SPI已经传输完成. */
    while((isTransferCompleted_5 == false) && (Tick_time_out))
	{
    	Tick_time_out--;
	}
    //------------------------------------------------------------------------------------------
}



