#ifndef __GPIO_H__
#define __GPIO_H__
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************

//***********************************************************************
//*								DEFINE									*
//***********************************************************************

//***********************************************************************
//*								REGISTER								*
//***********************************************************************

//***********************************************************************
//*								DECLARE									*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					GOIO							~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern void InitGPIO(void) ;
extern void DrvGPIOInput(unsigned char Port ,unsigned char Set) ;
extern void DrvGPIOOutput(unsigned char Port ,unsigned char Set , unsigned char HL) ;
extern unsigned char DrvGPIORead(unsigned char Port ,unsigned char Set) ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					FLEX							~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern void InitGPIOFlexInterrupt(unsigned char ChannelNum , unsigned char PortNum , unsigned char BitPosi , unsigned char Trig , unsigned char Event ) ;
extern void GPIOFlexIntEnable(unsigned char ChannelNum , unsigned char Event ) ;
extern void GPIOFlexIntDisable(unsigned char ChannelNum , unsigned char Event ) ;
extern unsigned char GPIOFlexIntStatus(unsigned char ChannelNum ) ;
extern void GPIOFlexIntClear(unsigned char ChannelNum ) ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					GROUP							~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern void InitGPIOGroupedInterrupt(unsigned char GroupNum , unsigned int *bitPattern, unsigned char Logic , unsigned char Trig , unsigned int *eventPattern ) ;
//***********************************************************************
//*								END										*
//***********************************************************************
#endif
