#ifndef __SPI_H__
#define __SPI_H__


#include "../inc/led.h"
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************

//***********************************************************************
//*								DEFINE									*
//***********************************************************************
#define  SSP_USE_SSP0 0
#define  SSP_USE_SSP1 1
#define Optical_SPI SSP_USE_SSP1
//#define Finger_SPI  SSP_USE_SSP1
#define HangingFlash_SPI  SSP_USE_SSP1

#define SPI_CLK_SPEED_1_5M	6
#define SPI_CLK_SPEED_12M	2
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 SSP                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SSPRXBUFFER             0xFF                                //SSP Receiver Buffer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 SSP_Register                    ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SSP0_SEL		        (1 << 2)
//SSP Status register
#define SSPSR_TFE		        (1 << 0)
#define SSPSR_TNF		        (1 << 1) 
#define SSPSR_RNE		        (1 << 2)
#define SSPSR_RFF		        (1 << 3) 
#define SSPSR_BSY		        (1 << 4)
//SSP CR0 register
#define SSPCR0_DSS		        (1 << 0)
#define SSPCR0_FRF		        (1 << 4)
#define SSPCR0_SPO		        (1 << 6)
#define SSPCR0_SPH		        (1 << 7)
#define SSPCR0_SCR		        (1 << 8)
//SSP CR1 register
#define SSPCR1_LBM		        (1 << 0)
#define SSPCR1_SSE		        (1 << 1)
#define SSPCR1_MS		        (1 << 2)
#define SSPCR1_SOD		        (1 << 3)
//SSP Interrupt Mask Set/Clear register
#define SSPIMSC_RORIM	        (1 << 0)
#define SSPIMSC_RTIM	        (1 << 1)
#define SSPIMSC_RXIM	        (1 << 2)
#define SSPIMSC_TXIM	        (1 << 3)
//SSP Interrupt Status register
#define SSPRIS_RORRIS	        (1 << 0)
#define SSPRIS_RTRIS	        (1 << 1)
#define SSPRIS_RXRIS	        (1 << 2)
#define SSPRIS_TXRIS	        (1 << 3)
//SSP Masked Interrupt register
#define SSPMIS_RORMIS	        (1 << 0)
#define SSPMIS_RTMIS	        (1 << 1)
#define SSPMIS_RXMIS	        (1 << 2)
#define SSPMIS_TXMIS	        (1 << 3)
//SSP Interrupt clear register
#define SSPICR_RORIC	        (1 << 0)
#define SSPICR_RTIC		        (1 << 1)


/*  Current status of the SSP controller.  */
#define SSP_SR_TFE_MASK (0x00000001) // Transmit FIFO empty
#define SSP_SR_TFE_EMPTY (0x00000001)
#define SSP_SR_TFE_NOTEMPTY (0x00000000)
#define SSP_SR_TNF_MASK (0x00000002) // Transmit FIFO not full
#define SSP_SR_TNF_NOTFULL (0x00000002)
#define SSP_SR_TNF_FULL (0x00000000)
#define SSP_SR_RNE_MASK (0x00000004) // Receive FIFO not empty
#define SSP_SR_RNE_NOTEMPTY (0x00000004)
#define SSP_SR_RNE_EMPTY (0x00000000)
#define SSP_SR_RFF_MASK (0x00000008) // Receive FIFO full
#define SSP_SR_RFF_FULL (0x00000008)
#define SSP_SR_RFF_NOTFULL (0x00000000)
#define SSP_SR_BSY_MASK (0x00000010) // Busy Flag
#define SSP_SR_BSY_IDLE (0x00000000)
#define SSP_SR_BSY_BUSY (0x00000010)

//***********************************************************************
//*								REGISTER								*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 SSP1_Receiver                   ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 SSP0_Receiver                   ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern volatile unsigned int SPI0InData[SSPRXBUFFER] ;
extern volatile unsigned int SPI1InData[SSPRXBUFFER] ;
extern volatile unsigned int SPI0Buffer ;
//***********************************************************************
//*								DECLARE									*
//***********************************************************************
extern void InitSPI0Master(void) ;
extern void InitSPI1Master(void) ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Drv                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern void DrvSSP0Send(unsigned int DATA) ;
extern void DrvSSP0SendL(unsigned char *BufferPtr ) ;
extern void DrvSSP0SendLong(unsigned char *BufferPtr , unsigned char Length) ;
extern unsigned int DrvSSP0Read(void) ;
extern void DrvSSPOReadBuffer(unsigned int Address , unsigned int Length) ;

extern void DrvSSP1Send(unsigned int DATA) ;
extern void DrvSSP1SendL(U16 *BufferPtr , unsigned char Length) ;
extern unsigned int DrvSSP1Read(void) ;
extern void DrvSSP1ReadBuffer(unsigned int Address , unsigned int Length) ;



extern void SPI_Write(U8 addr,U8 data );
extern U8 SPI_Read(unsigned int addr);

extern void SPI_Burst_Write(unsigned char Reg,const UINT8 *srom_addr,unsigned int len);
extern void SPI_Burst_Read( U8 Reg,U8 *addr,U8 len );
extern U8 Optical_MOT_Read(void);
extern void SpiModule_Start(void);

extern void SSP_Send_Receive(U8 *buf_wr, U8 *buf_rd, U16 length);
extern void SSP_Receive(U8 Ssp_Nums,U8 *buf, U16 length);
extern void SSP_Send(U8 Ssp_Nums,U8 *buf, U16 length);

extern void SPI_Write_Finger(U8 addr,U8 data );
extern U8 SPI_Read_Finger(unsigned int addr);
void SPI_Sleep(void);

void SPI_SCLK_Speed(UINT8 freq);
//void SPI_HARDWARE_DIRVE_OFF_FLASH_CS (void) ;
//void SPI_HARDWARE_DIRVE_ON_FLASH_CS(void);
void SPI_ReadWriteOneByte( U8 data , U8 *len);
void NCS_OFF(void);
void NCS_ON(void);
//***********************************************************************
//*								END										*
//***********************************************************************
#endif
