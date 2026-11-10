//***********************************************************************
//*Writer						ADAM									*
//*Function						SPI										*
//***********************************************************************
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************
#include "../inc/LPC11Uxx.h"
#include "../inc/GPIO.h"
#include "../inc/OSC.h"
#include "../inc/device.h"
#include "../inc/SPI.h"
#include "../inc/optical_3389.h"
#include "my_board.h"
//***********************************************************************
//*								REGISTER								*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 SSP1_Receiver                   ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 SSP0_Receiver                   ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
volatile unsigned int SPI0InData[SSPRXBUFFER] ;
volatile unsigned int SPI1InData[SSPRXBUFFER] ;
volatile unsigned int SPI0Buffer =0;
//***********************************************************************
//*								DECLARE									*
//***********************************************************************
void InitSPI0Master(void) ;
void InitSPI1Master(void) ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Drv                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DrvSSP0Send(unsigned int DATA) ;
void DrvSSP0SendL(unsigned char *BufferPtr) ;
void DrvSSP0SendLong(unsigned char *BufferPtr , unsigned char Length) ;
unsigned int DrvSSP0Read(void) ;
void DrvSSPOReadBuffer(unsigned int Address , unsigned int Length) ;

void DrvSSP1Send(unsigned int DATA) ;
void DrvSSP1SendL(U16 *BufferPtr , unsigned char Length) ;
unsigned int DrvSSP1Read(void) ;
void DrvSSP1ReadBuffer(unsigned int Address , unsigned int Length) ;
//***********************************************************************
//*								INTERRUPT_FUNCTION						*
//***********************************************************************

//***********************************************************************
//*								SUB_FUNCTION							*
//***********************************************************************
/* statistics of all the interrupts */
volatile uint32_t interruptRxStat0 = 0;
volatile uint32_t interruptOverRunStat0 = 0;
volatile uint32_t interruptRxTimeoutStat0 = 0;

volatile uint32_t interruptRxStat1 = 0;
volatile uint32_t interruptOverRunStat1 = 0;
volatile uint32_t interruptRxTimeoutStat1 = 0;

//#define  SSP_USE_SSP0 0
//#define  SSP_USE_SSP1 1

//#define Optical_SPI SSP_USE_SSP1
//#define Finger_SPI  SSP_USE_SSP1

void SSP0_IRQHandler(void)
{
  uint32_t regValue;

  regValue = LPC_SSP0->MIS;
  if ( regValue & SSPMIS_RORMIS )	/* Receive overrun interrupt */
  {
	interruptOverRunStat0++;
	LPC_SSP0->ICR = SSPICR_RORIC;	/* clear interrupt */
  }
  if ( regValue & SSPMIS_RTMIS )	/* Receive timeout interrupt */
  {
	interruptRxTimeoutStat0++;
	LPC_SSP0->ICR = SSPICR_RTIC;	/* clear interrupt */
  }

  /* please be aware that, in main and ISR, CurrentRxIndex and CurrentTxIndex
  are shared as global variables. It may create some race condition that main
  and ISR manipulate these variables at the same time. SSPSR_BSY checking (polling)
  in both main and ISR could prevent this kind of race condition */
  if ( regValue & SSPMIS_RXMIS )	/* Rx at least half full */
  {
	interruptRxStat0++;		/* receive until it's empty */
  }
  return;
}

/*****************************************************************************
** Function name:		SSP1_IRQHandler
**
** Descriptions:		SSP port is used for SPI communication.
**						SSP interrupt handler
**						The algorithm is, if RXFIFO is at least half full,
**						start receive until it's empty; if TXFIFO is at least
**						half empty, start transmit until it's full.
**						This will maximize the use of both FIFOs and performance.
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void SSP1_IRQHandler(void)
{
  uint32_t regValue;

  regValue = LPC_SSP1->MIS;
  if ( regValue & SSPMIS_RORMIS )	/* Receive overrun interrupt */
  {
	interruptOverRunStat1++;
	LPC_SSP1->ICR = SSPICR_RORIC;	/* clear interrupt */
  }
  if ( regValue & SSPMIS_RTMIS )	/* Receive timeout interrupt */
  {
	interruptRxTimeoutStat1++;
	LPC_SSP1->ICR = SSPICR_RTIC;	/* clear interrupt */
  }

  /* please be aware that, in main and ISR, CurrentRxIndex and CurrentTxIndex
  are shared as global variables. It may create some race condition that main
  and ISR manipulate these variables at the same time. SSPSR_BSY checking (polling)
  in both main and ISR could prevent this kind of race condition */
  if ( regValue & SSPMIS_RXMIS )	/* Rx at least half full */
  {
	interruptRxStat1++;		/* receive until it's empty */
  }
  return;
}

void InitSPI0Master(void)
{
#if (defined(GT_LINA_T1A_H5))
    unsigned char I ;
    unsigned int DUMMY = 0 ;
    DUMMY=DUMMY;
    //-----SPI0-----
    LPC_SYSCON->PRESETCTRL |= (0x1<<0) ;                            //SPI0 Reset De-Asserted
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<11) ;                          //Setup AHB Clock To The SPI0 Domain 
    LPC_SYSCON->SSP0CLKDIV = 0x1 ;                                  //SPI0 Clock Divider 1(48MHz/1=48MHz)
	//-----GPIO TO SPI0-----
    LPC_IOCON->PIO0_8           &= ~0x07 ;                          //Select GPIO0_8 To MISO0
    LPC_IOCON->PIO0_8           |= 0x01 ;
    
    LPC_IOCON->PIO0_9           &= ~0x07 ;                          //Select GPIO0_9 To MOSI0
    LPC_IOCON->PIO0_9           |= 0x01 ;


	LPC_IOCON->SWCLK_PIO0_10	&= ~0x07 ;							//Select GPIO0_10 To CLK0
	LPC_IOCON->SWCLK_PIO0_10	|= 0x02 ;

		
	LPC_SSP0->CR0 = 0x01C7  ;                                        //CPOL = 1 CPHA = 1 8Bits
    LPC_SSP0->CPSR = 0x18 ;
    //SPI SPEED = ((MCU_OSC/SSP1CLKDIV)/(CPSR*[CR0 High Byte+1]))
    //((48MHz/1)/(24*[1+1])) = 1Mhz

    for ( I = 0; I < 8 ; I++ )
    {
        DUMMY = LPC_SSP0->DR ;
  	}
    LPC_SSP0->CR1 = SSPCR1_SSE ;                                    //Enable SSP0
    return ;
#else
    unsigned char I ;
    unsigned int DUMMY = 0 ;
    DUMMY=DUMMY;
    //-----SPI0-----
    LPC_SYSCON->PRESETCTRL |= (0x1<<0) ;                            //SPI0 Reset De-Asserted
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<11) ;                          //Setup AHB Clock To The SPI0 Domain 
    LPC_SYSCON->SSP0CLKDIV = 0x1 ;                                  //SPI0 Clock Divider 1(48MHz/1=48MHz)
	//-----GPIO TO SPI0-----
    LPC_IOCON->PIO0_8           &= ~0x07 ;                          //Select GPIO0_8 To MISO0
    LPC_IOCON->PIO0_8           |= 0x01 ;
    
    LPC_IOCON->PIO0_9           &= ~0x07 ;                          //Select GPIO0_9 To MOSI0
    LPC_IOCON->PIO0_9           |= 0x01 ;


	LPC_IOCON->SWCLK_PIO0_10	&= ~0x07 ;							//Select GPIO0_10 To CLK0
	LPC_IOCON->SWCLK_PIO0_10	|= 0x02 ;

		
	LPC_SSP0->CR0 = 0x0287 ;                                         //CPOL = 0 CPHA = 1 8Bits 
    LPC_SSP0->CPSR = 0x02;                   

    //SPI SPEED = ((MCU_OSC/SSP0CLKDIV)/(CPSR*[CR0 High Byte+1]))
    //((48MHz/1)/(2*[2+1])) = 8Mhz

    for ( I = 0; I < 8 ; I++ )
    {
        DUMMY = LPC_SSP0->DR ;
  	}
    LPC_SSP0->CR1 = SSPCR1_SSE ;                                    //Enable SSP0
    return ;
#endif
}
//-----
void InitSPI1Master(void)
{
#if (defined(GT_LINA_T1A_H5))
    unsigned char I ;
    unsigned int DUMMY = 0;
    DUMMY=DUMMY;
    //-----SPI1-----
    LPC_SYSCON->PRESETCTRL |= (0x1<<2) ;                            //SPI1 Reset De-Asserted
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<18) ;                          //Setup AHB Clock To The SPI1 Domain
    LPC_SYSCON->SSP1CLKDIV = 0x01 ;                                 //SPI1 Clock Divider 1(48MHz/1=48MHz)
	//-----GPIO TO SPI1-----
    
     LPC_IOCON->PIO0_21			&= ~0x07 ;							//Select GPIO0_21 To MOSI1
     LPC_IOCON->PIO0_21			|= 0x02 ;
//    LPC_IOCON->PIO1_22			&= ~0x07 ;							//Select GPIO1_22 To MOSI1
//    LPC_IOCON->PIO1_22			|= 0x02 ;

     LPC_IOCON->PIO0_22			&= ~0x07 ;							//Select GPIO0_22 To MISO1
     LPC_IOCON->PIO0_22			|= 0x03 ;
//    LPC_IOCON->PIO1_21			&= ~0x07 ;							//Select GPIO1_21 To MISO1
//    LPC_IOCON->PIO1_21			|= 0x02 ;


     LPC_IOCON->PIO1_15			&= ~0x07 ;							//Select GPIO1_15 To SCK1
     LPC_IOCON->PIO1_15			|= 0x03 ;
//   LPC_IOCON->PIO1_20			&= ~0x07 ;							//Select GPIO1_20 To SCK1
//   LPC_IOCON->PIO1_20			|= 0x02 ;

 //  LPC_IOCON->PIO1_23			&= ~0x07 ;							//Select GPIO1_23 To SSEL1  //  USE for segment
 //  LPC_IOCON->PIO1_23			|= 0x02 ;
 //  LPC_IOCON->PIO1_19			&= ~0x07 ;							//Select GPIO1_19 To SSEL1
 //  LPC_IOCON->PIO1_19			|= 0x02 ;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LPC_SSP1->CR0 = 0x0287  ;                                        //CPOL = 0 CPHA = 1 8Bits 
    LPC_SSP1->CPSR = 0x08 ;
    //SPI SPEED = ((MCU_OSC/SSP0CLKDIV)/(CPSR*[CR0 High Byte+1]))
    //((48MHz/1)/(2*[2+1])) = 8Mhz

    for ( I = 0; I < 8; I++ )
    {
        DUMMY = LPC_SSP1->DR ;
    }
    LPC_SSP1->CR1 |= SSPCR1_SSE ;                               //Enable SSP1
    return ;  
#else
    unsigned char I ;
    unsigned int DUMMY = 0;
    DUMMY=DUMMY;
    //-----SPI1-----
    LPC_SYSCON->PRESETCTRL |= (0x1<<2) ;                            //SPI1 Reset De-Asserted
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<18) ;                          //Setup AHB Clock To The SPI1 Domain
    LPC_SYSCON->SSP1CLKDIV = 0x01 ;                                 //SPI1 Clock Divider 1(48MHz/1=48MHz)
	//-----GPIO TO SPI1-----
    
     LPC_IOCON->PIO0_21			&= ~0x07 ;							//Select GPIO0_21 To MOSI1
     LPC_IOCON->PIO0_21			|= 0x02 ;
//    LPC_IOCON->PIO1_22			&= ~0x07 ;							//Select GPIO1_22 To MOSI1
//    LPC_IOCON->PIO1_22			|= 0x02 ;

     LPC_IOCON->PIO0_22			&= ~0x07 ;							//Select GPIO0_22 To MISO1
     LPC_IOCON->PIO0_22			|= 0x03 ;
//    LPC_IOCON->PIO1_21			&= ~0x07 ;							//Select GPIO1_21 To MISO1
//    LPC_IOCON->PIO1_21			|= 0x02 ;


     LPC_IOCON->PIO1_15			&= ~0x07 ;							//Select GPIO1_15 To SCK1
     LPC_IOCON->PIO1_15			|= 0x03 ;
//   LPC_IOCON->PIO1_20			&= ~0x07 ;							//Select GPIO1_20 To SCK1
//   LPC_IOCON->PIO1_20			|= 0x02 ;

 //  LPC_IOCON->PIO1_23			&= ~0x07 ;							//Select GPIO1_23 To SSEL1  //  USE for segment
 //  LPC_IOCON->PIO1_23			|= 0x02 ;
 //  LPC_IOCON->PIO1_19			&= ~0x07 ;							//Select GPIO1_19 To SSEL1
 //  LPC_IOCON->PIO1_19			|= 0x02 ;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LPC_SSP1->CR0 = 0x01C7  ;                                        //CPOL = 1 CPHA = 1 8Bits
    LPC_SSP1->CPSR = 0x18 ;//0x06
    //SPI SPEED = ((MCU_OSC/SSP1CLKDIV)/(CPSR*[CR0 High Byte+1]))
    //((48MHz/1)/(24*[1+1])) = 1Mhz

    for ( I = 0; I < 8; I++ )
    {
        DUMMY = LPC_SSP1->DR ;
    }
    LPC_SSP1->CR1 |= SSPCR1_SSE ;                               //Enable SSP1
    return ;    
#endif
}
#if 0
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Drv                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//-----DrvSSP0Send-----
void DrvSSP0Send(unsigned int DATA)
{
    uint32_t DUMMY = 0 ;
    DUMMY=DUMMY;
    while ( (LPC_SSP0->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF ) ;
    LPC_SSP0->DR = DATA ;
    while( LPC_SSP0->SR & SSPSR_BSY ) ;
    while ( (LPC_SSP0->SR & SSPSR_RNE) != SSPSR_RNE ) ;
    DUMMY = LPC_SSP0->DR ;
    return ;
}
//-----DrvSSP0SendL-----
void DrvSSP0SendL(unsigned char *BufferPtr )
{
    unsigned int DUMMY = 0 ;
    unsigned char SPIOK = 1 ;
    DUMMY = DUMMY ;
    while (SPIOK == 1 )
    {
		if(*BufferPtr==0x0A)
		{
			SPIOK = 0 ;
		}
        while ( (LPC_SSP0->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF ) ;
        LPC_SSP0->DR = *BufferPtr ;
        while( LPC_SSP0->SR & SSPSR_BSY ) ;
        while ( (LPC_SSP0->SR & SSPSR_RNE) != SSPSR_RNE ) ;
        DUMMY = LPC_SSP0->DR ;
        BufferPtr++ ;
    }
    return ;    
}
void DrvSSP0SendLong(unsigned char *BufferPtr , unsigned char Length)
{
    uint32_t DUMMY = 0 ;
    unsigned char FORI ;
    DUMMY=DUMMY;
    
    for(FORI = 0 ; FORI < Length ; FORI++)
    {
        while ( (LPC_SSP0->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF ) ;
        LPC_SSP0->DR = *BufferPtr ;
        while( LPC_SSP0->SR & SSPSR_BSY ) ;
        while ( (LPC_SSP0->SR & SSPSR_RNE) != SSPSR_RNE ) ;
        DUMMY = LPC_SSP0->DR ;
        BufferPtr++ ;
    }
    return ;    
}
//-----DrvSSP0Read-----
unsigned int DrvSSP0Read(void)
{
    unsigned int ReadData = 0 ;
    ReadData=ReadData;
    while ( (LPC_SSP0->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF ) ;
    LPC_SSP0->DR = 0xFF ;
    while( LPC_SSP0->SR & SSPSR_BSY ) ;
    while ( (LPC_SSP0->SR & SSPSR_RNE) != SSPSR_RNE ) ;
    ReadData = LPC_SSP0->DR ;
    return ReadData ;
}
//-----DrvSSPOReadBuffer-----
void DrvSSPOReadBuffer(unsigned int Address , unsigned int Length)
{
    unsigned char FORI ;
    for(FORI = 0 ; FORI < Length ; FORI++)
    {
        //LPC_GPIO0->DATA &= ~0x004 ;
        while ( (LPC_SSP0->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF ) ;
        LPC_SSP0->DR = 0xFF ;
        while( LPC_SSP0->SR & SSPSR_BSY ) ;
        while ( (LPC_SSP0->SR & SSPSR_RNE) != SSPSR_RNE ) ;
        SPI0InData[(Address + FORI)] = LPC_SSP0->DR ;
        //LPC_GPIO0->DATA |= 0x004 ;
    }
}
//-----
//-----DrvSSP1SendL-----
void DrvSSP1SendL(U16 *BufferPtr , unsigned char Length)
{
    uint32_t DUMMY = 0 ;
    unsigned char FORI ;
    DUMMY=DUMMY;

    for(FORI = 0 ; FORI < Length ; FORI++)
    {
        while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF ) ;
        LPC_SSP1->DR = *BufferPtr ;
        while( LPC_SSP1->SR & SSPSR_BSY ) ;
        while ( (LPC_SSP1->SR & SSPSR_RNE) != SSPSR_RNE ) ;
        DUMMY = LPC_SSP1->DR ;
        BufferPtr++ ;
    }
    return ;    
}
//-----DrvSSP01Read-----
unsigned int DrvSSP1Read(void)
{
    unsigned int ReadData = 0 ;
    ReadData=ReadData;
    while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF ) ;
    LPC_SSP0->DR = 0xFF ;
    while( LPC_SSP1->SR & SSPSR_BSY ) ;
    while ( (LPC_SSP1->SR & SSPSR_RNE) != SSPSR_RNE ) ;
    ReadData = LPC_SSP0->DR ;
    return ReadData ;
}
//-----DrvSSP1ReadBuffer-----
void DrvSSP1ReadBuffer(unsigned int Address , unsigned int Length)
{
    unsigned char FORI ;
    for(FORI = 0 ; FORI < Length ; FORI++)
    {
        //LPC_GPIO0->DATA &= ~0x004 ;
        while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF ) ;
        LPC_SSP0->DR = 0xFF ;
        while( LPC_SSP1->SR & SSPSR_BSY ) ;
        while ( (LPC_SSP1->SR & SSPSR_RNE) != SSPSR_RNE ) ;
        SPI1InData[(Address + FORI)] = LPC_SSP1->DR ;
        //LPC_GPIO0->DATA |= 0x004 ;
    }
}
#endif

//-----DrvSSP1Send-----
void DrvSSP1Send(unsigned int DATA)
{
    uint32_t DUMMY = 0 ;
    DUMMY=DUMMY;
    while ( (LPC_SSP1->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF ) ;
    LPC_SSP1->DR = DATA ;
    while( LPC_SSP1->SR & SSPSR_BSY ) ;
    while ( (LPC_SSP1->SR & SSPSR_RNE) != SSPSR_RNE ) ;
    DUMMY = LPC_SSP1->DR ;
    return ;
}

void SSP_Send_Receive(U8 *buf_wr, U8 *buf_rd, U16 length)
{
#if (defined(GT_LINA_T1A_H5))
	while(length--)
	{
		while((LPC_SSP1->SR & (SSP_SR_TNF_NOTFULL | SSP_SR_BSY_BUSY)) != SSP_SR_TNF_NOTFULL){}
		LPC_SSP1->DR = *buf_wr;

		while((LPC_SSP1->SR & (SSP_SR_BSY_BUSY|SSP_SR_RNE_NOTEMPTY)) != SSP_SR_RNE_NOTEMPTY){}
		/* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO
		on MISO. Otherwise, when sspReceive is called, previous data byte is left in the FIFO. */
		*buf_rd = LPC_SSP1->DR;
			
		buf_wr++;
		buf_rd++;
	}
#else
	while(length--)
	{
		while((LPC_SSP0->SR & (SSP_SR_TNF_NOTFULL | SSP_SR_BSY_BUSY)) != SSP_SR_TNF_NOTFULL){}
		LPC_SSP0->DR = *buf_wr;

		while((LPC_SSP0->SR & (SSP_SR_BSY_BUSY|SSP_SR_RNE_NOTEMPTY)) != SSP_SR_RNE_NOTEMPTY){}
		/* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO
		on MISO. Otherwise, when sspReceive is called, previous data byte is left in the FIFO. */
		*buf_rd = LPC_SSP0->DR;
			
		buf_wr++;
		buf_rd++;
	}
#endif
}

void SSP_Send(U8 Ssp_Nums,U8 *buf, U16 length)
{
    U8 Dummy = Dummy;
    /* Move on only if NOT busy and TX FIFO not full. */

    if(Ssp_Nums == SSP_USE_SSP0)
    {
        while((LPC_SSP0->SR & (SSP_SR_TNF_NOTFULL | SSP_SR_BSY_BUSY)) != SSP_SR_TNF_NOTFULL){}
        LPC_SSP0->DR = *buf;

        while((LPC_SSP0->SR & (SSP_SR_BSY_BUSY|SSP_SR_RNE_NOTEMPTY)) != SSP_SR_RNE_NOTEMPTY){}
        /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO
        on MISO. Otherwise, when sspReceive is called, previous data byte is left in the FIFO. */
        Dummy = LPC_SSP0->DR;
    }
   
    if(Ssp_Nums == SSP_USE_SSP1)
    {
        while((LPC_SSP1->SR & (SSP_SR_TNF_NOTFULL | SSP_SR_BSY_BUSY)) != SSP_SR_TNF_NOTFULL){}
        LPC_SSP1->DR = *buf;

        while((LPC_SSP1->SR & (SSP_SR_BSY_BUSY|SSP_SR_RNE_NOTEMPTY)) != SSP_SR_RNE_NOTEMPTY){}
        /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO
        on MISO. Otherwise, when sspReceive is called, previous data byte is left in the FIFO. */
        Dummy = LPC_SSP1->DR;
    }

}
void SSP_Receive(U8 Ssp_Nums,U8 *buf, U16 length)
{
		
    if(Ssp_Nums == SSP_USE_SSP0)
    {
        /* As long as the receive FIFO is not empty, data can be received. */
        LPC_SSP0->DR = EMPTY;
        /* Wait until the Busy bit is cleared */
        while((LPC_SSP0->SR & (SSP_SR_BSY_BUSY|SSP_SR_RNE_NOTEMPTY)) != SSP_SR_RNE_NOTEMPTY ){}
        *buf = LPC_SSP0->DR;
    }
 
    if(Ssp_Nums == SSP_USE_SSP1)
    {
        /* As long as the receive FIFO is not empty, data can be received. */
        //LPC_SSP1->DR =  (EMPTY) | (temp[0]&0xFF00) ;
			  LPC_SSP1->DR =  EMPTY ;
        /* Wait until the Busy bit is cleared */
        while((LPC_SSP1->SR & (SSP_SR_BSY_BUSY|SSP_SR_RNE_NOTEMPTY)) != SSP_SR_RNE_NOTEMPTY ){}
        *buf = LPC_SSP1->DR;
    }
}

void SPI_Write(U8 addr,U8 data )
{
	U8 Buff_temp[2]={0};
	NCS_OFF();
	//DrvGPIOOutput(0,20,0);
	nsAPI_SSP_Delay1us(1);
	Buff_temp[0] = addr|(1<<7);
	nsAPI_SSP_Delay1us(2);
	SSP_Send(Optical_SPI,&Buff_temp[0],1);
	Buff_temp[1] = data;
	SSP_Send(Optical_SPI,&Buff_temp[1],1);
	nsAPI_SSP_Delay1us(35);
//Delayus(35);
//	SPI_HARDWARE_DIRVE_ON_FLASH_CS();
	nsAPI_SSP_Delay1us(45);
	NCS_ON();
//	DrvGPIOOutput(0,20,1);
	//Delayus(20);
}

U8 SPI_Read(unsigned int addr)
{
	U8 ret = addr | (0<<7);
	NCS_OFF();
//	Delayus(1);
	nsAPI_SSP_Delay1us(1);
//	DrvGPIOOutput(0,20,0);
//	Delayus(1);
	SSP_Send(Optical_SPI,&ret,1);
	nsAPI_SSP_Delay1us(35);
//	Delayus(213);
	SSP_Receive(Optical_SPI,&ret,1);
	nsAPI_SSP_Delay1us(1);
//	Delayus(1);
	DrvGPIOOutput(0,20,1);
	nsAPI_SSP_Delay1us(20);
//	Delayus(20);
	return ret ;
}

#if 0
void SPI_Burst_Write(unsigned char Reg,const UINT8 *srom_add,unsigned int len)
{
//	U16 Buff_temp[2]={0};
//	Buff_temp[0] = Reg;
//	Buff_temp[1] = srom_add[0];
//	DrvSSP1SendL(&Buff_temp[0],len);
	uint16_t i;
	U8 Buff_temp[1]={0};
	Buff_temp[0] = Reg|(1<<7);
//	Buff_temp[1] = data;
	Delayus(140);
	DrvGPIOOutput(0,20,0);
	Delayus(5);
	SSP_Send(Optical_SPI,&Buff_temp[0],1);

	for( i=0;i<len;i++ ){
		Buff_temp[0] = *srom_add ++;
		SSP_Send(Optical_SPI,&Buff_temp[0],1);
		Delayus(30);
	}
	DrvGPIOOutput(0,20,1);
}

void SPI_Burst_Read( U8 Reg,U8 *addr,U8 len )
{
	uint16_t i;
	U8 Buff_temp[1]={0};
	U8 Buff_temp1[16]={0};
	Buff_temp[0] = Reg | 1<<7 ;
	SPI_Write(Reg,0x11);
	DrvGPIOOutput(0,20,0);        // Lower NCS 
	nsAPI_SSP_Delay1us(1);
//	Buff_temp[1] = data;  
//	Write_Sensor(Reg,0x11);       // Write any value to Motion_brust register
//	DrvGPIOOutput(0,20,0);        // Lower NCS     
	Buff_temp[0] = Reg | 0<<7 ;
	SSP_Send(Optical_SPI,&Buff_temp[0],1);  // Send Motion_Brust address 0x50 
	nsAPI_SSP_Delay1us(45);
//  Delayus(45);            // Delay Tsrad_motbr >=35us
	for( i=0;i<16;i++ )  //12
	{	        // Read
		//SSP_Send(1,&Buff_temp[0],1);  // Send Motion_Brust address 0x50 
		SSP_Receive(Optical_SPI,&Buff_temp1[0],1);
		*addr ++ = Buff_temp1[0];
//		Delayus(10);    
	}
	nsAPI_SSP_Delay1us(1);
//	addr[2] = Buff_temp1[2];
//	addr[3] = Buff_temp1[3];
//	addr[4] = Buff_temp1[4];
//	addr[5] = Buff_temp1[5];
	DrvGPIOOutput(0,20,1);
}



void SPI_Write(U8 addr,U8 data )
{
	U8 Buff_temp[2]={0};
	Buff_temp[0] = addr|(1<<7);
	Delayus(195);
	DrvGPIOOutput(0,20,0);
	Delayus(1);
	SSP_Send(Optical_SPI,&Buff_temp[0],1);
	Buff_temp[1] = data;
	SSP_Send(Optical_SPI,&Buff_temp[1],1);
	Delayus(35);
	DrvGPIOOutput(0,20,1);
	//Delayus(20);
}

U8 SPI_Read(unsigned int addr)
{
	U8 ret = addr | (0<<7);
	Delayus(26);
	DrvGPIOOutput(0,20,0);
	Delayus(1);
	SSP_Send(Optical_SPI,&ret,1);
	Delayus(213);
	SSP_Receive(Optical_SPI,&ret,1);
	Delayus(1);
	DrvGPIOOutput(0,20,1);
//	Delayus(20);
	return ret ;
}
#endif
void SPI_SCLK_Speed(UINT8 freq)
{
	LPC_SSP1->CR0 = 0x01C7 ;                                        //CPOL = 1 CPHA = 1 8Bits 
  LPC_SSP1->CPSR = freq;                   //  Modify this config by Martin 2017.12.1
}

void NCS_OFF(void)
{
 DrvGPIOOutput(NCS_PORT,NCS_PIN, 0);
}

void NCS_ON(void)
{
DrvGPIOOutput(NCS_PORT,NCS_PIN, 1);
}

//void SPI_HARDWARE_DIRVE_OFF_FLASH_CS (void) 
//{
//	DrvGPIOOutput(1,19, 0);//DrvGPIOOutput(1,21, 1);
//}

//void SPI_HARDWARE_DIRVE_ON_FLASH_CS(void)
//{
//  DrvGPIOOutput(1,19, 1);//DrvGPIOOutput(1,21, 0);
//}

  void SPI_ReadWriteOneByte( U8 addr , U8 *buffer)
 {
	U8 ret=0;
  ret = addr | (0<<7); 
//	TimerDelayMsec(2);
//	Delayus(160);
	if(ret!=0xff)
	{
  SSP_Send(HangingFlash_SPI,&addr,1);
	}
	else
	{
//	Delayus(10);
	SSP_Receive(HangingFlash_SPI,&ret,1);
//	Delayus(10); 
	buffer[0]=ret;
	}
 }

#if 0
void SPI_Write_Finger(U8 addr,U8 data )
{
	U8 Buff_temp[2]={0};
	Buff_temp[0] = addr|(1<<7);
	DrvGPIOOutput(1,19,0);
	Delayus(20);
	SSP_Send(Finger_SPI,&Buff_temp[0],1);
//	Delayus(100);
	Buff_temp[1] = data;
	if(DrvGPIORead(0,16)==0)
	{
	SSP_Send(Finger_SPI,&Buff_temp[1],1);
	Delayus(20);
	}
	DrvGPIOOutput(1,19,1);
}

U8 SPI_Read_Finger(unsigned int addr)
{
		
	U8 ret = addr | (0<<7);
	DrvGPIOOutput(1,19,0);
	Delayus(20);
	SSP_Send(Finger_SPI,&ret,1);
	Delayus(160);
	if(DrvGPIORead(0,16)==0)
	{
	SSP_Receive(Finger_SPI,&ret,1);
	}
	DrvGPIOOutput(1,19,1);
	Delayus(20);
	return ret ;
}
//#endif

void SPI_Burst_Write(unsigned char Reg,const UINT8 *srom_add,unsigned int len)
{
//	U16 Buff_temp[2]={0};
//	Buff_temp[0] = Reg;
//	Buff_temp[1] = srom_add[0];
//	DrvSSP1SendL(&Buff_temp[0],len);
	uint16_t i;
	U8 Buff_temp[1]={0};
	Buff_temp[0] = Reg|(1<<7);
//	Buff_temp[1] = data;
	Delayus(140);
	DrvGPIOOutput(0,20,0);
	Delayus(5);
	SSP_Send(Optical_SPI,&Buff_temp[0],1);

	for( i=0;i<len;i++ ){
		Buff_temp[0] = *srom_add ++;
		SSP_Send(Optical_SPI,&Buff_temp[0],1);
		Delayus(30);
	}
	DrvGPIOOutput(0,20,1);
}

void SPI_Burst_Read( U8 Reg,U8 *addr,U8 len )
{
	uint16_t i;
	U8 Buff_temp[1]={0};
	U8 Buff_temp1[6]={0};
	Buff_temp[0] = Reg | 1<<7 ;
//	Buff_temp[1] = data;  
	Write_Sensor(Reg,0x11);       // Write any value to Motion_brust register
	DrvGPIOOutput(0,20,0);        // Lower NCS     
	Buff_temp[0] = Reg | 0<<7 ;
	SSP_Send(Optical_SPI,&Buff_temp[0],1);  // Send Motion_Brust address 0x50 
	nsAPI_SSP_Delay1us(35);
//  Delayus(45);            // Delay Tsrad_motbr >=35us
	for( i=0;i<6;i++ )  //12
	{	        // Read
		//SSP_Send(1,&Buff_temp[0],1);  // Send Motion_Brust address 0x50 
		SSP_Receive(Optical_SPI,&Buff_temp1[0],1);
		*addr ++ = Buff_temp1[0];
//		Delayus(10);    
	}
//	addr[2] = Buff_temp1[2];
//	addr[3] = Buff_temp1[3];
//	addr[4] = Buff_temp1[4];
//	addr[5] = Buff_temp1[5];
	DrvGPIOOutput(0,20,1);
}
#endif
void SpiModule_Start(void)
{
//	InitSPI0Master();
//	InitSPI1Master();
}	



U8 Optical_MOT_Read(void)
{
	U8 MOT_temp;
//	MOT_temp = DrvGPIORead(1,29);
	MOT_temp = DrvGPIORead(0,2);
	return MOT_temp;
}

//***********************************************************************
//*								END										*
//***********************************************************************
