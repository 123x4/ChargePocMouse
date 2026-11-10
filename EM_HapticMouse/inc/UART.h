#ifndef __UART_H__
#define __UART_H__


#ifdef _UART_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************

//***********************************************************************
//*								DEFINE									*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					UART							~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define UART_BAUDRATE			115200								//Set UART baudrate
#define UARTBUFSIZE				0x100								//64 bytes

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					UART_Register					~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define IER_RBR					0x01
#define IER_THRE				0x02
#define IER_RLS					0x04

#define IIR_PEND				0x01
#define IIR_RLS					0x03
#define IIR_RDA					0x02
#define IIR_CTI					0x06
#define IIR_THRE				0x01

#define LSR_RDR					0x01
#define LSR_OE					0x02
#define LSR_PE					0x04
#define LSR_FE					0x08
#define LSR_BI					0x10
#define LSR_THRE				0x20
#define LSR_TEMT				0x40
#define LSR_RXFE				0x80
//***********************************************************************
//*								REGISTER								*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					UART							~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
INTERFACE unsigned char UARTTxEmpty ;
INTERFACE unsigned long UARTCount ;
INTERFACE unsigned char UARTRXBuffer[UARTBUFSIZE] ;
INTERFACE unsigned char UARTReceiverFg ;
INTERFACE unsigned char UARTCharFg ;
INTERFACE unsigned char UARTStatus ;
//***********************************************************************
//*								DECLARE									*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Init							~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
INTERFACE void InitUART(unsigned long Baudrate) ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Drv								~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
INTERFACE void DrvUARTSend(unsigned char *BufferPtr) ;
INTERFACE void DrvUARTSendWord(unsigned char UARTDATA) ;
INTERFACE void DrvUARTSendData(unsigned char *BufferPtr,unsigned int UARTCOUNT) ;
INTERFACE void xprintf(unsigned char *pFmt,int wVal,...);
INTERFACE void PrintFloat(float value);
#undef INTERFACE
//***********************************************************************
//*								END										*
//***********************************************************************
#endif
