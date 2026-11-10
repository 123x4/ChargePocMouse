//***********************************************************************
//*Writer						ADAM									*
//*Function						UART									*
//***********************************************************************
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************
#define _UART_C_
#include "../inc/LPC11Uxx.h"
//#include "../inc/device.h"
#include "../inc/OSC.h"
#include "../inc/UART.h"
#include "type.h"

#if 1
//***********************************************************************
//*								INTERRUPT_FUNCTION						*
//***********************************************************************
void UART_IRQHandler(void)
{
	unsigned char IIRValue ;
	unsigned char LSRValue ;
	unsigned char Dummy = 0 ;
    Dummy=Dummy;
	IIRValue = LPC_USART->IIR ;
    
	IIRValue >>= 1 ;												//skip pending bit in IIR
	IIRValue &= 0x07 ;												//check bit 1~3, interrupt identification

	if (IIRValue == IIR_RLS)										//Receive Line Status
	{
 		LSRValue = LPC_USART->LSR ;
// 		if (LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXFE | LSR_BI))
// 		{
// 			UARTStatus = LSRValue ;
// 			Dummy = LPC_USART->RBR ;								//Dummy read on RX to clear interrupt, then bail out
// 			return;
// 		}
// 		if (LSRValue & LSR_RDR)										//Receive Data Ready			
// 		{
// 			UARTRXBuffer[UARTCount++] = LPC_USART->RBR ;
// 			if (UARTCount == UARTBUFSIZE)
// 			{
// 				UARTCount = 0 ;
// 			}	
// 		}
	}
	else if (IIRValue == IIR_RDA)									//Receive Data Available
	{
		UARTRXBuffer[UARTCount] = LPC_USART->RBR ;
        UARTCharFg = 1 ;
		if(UARTRXBuffer[UARTCount] != 0x0A)
		{
			UARTCount++ ;
			if (UARTCount == UARTBUFSIZE)
 			{
 				UARTCount = 0 ;
 			}	
		}
		else
		{
			UARTReceiverFg = 1 ;
		}
	}
	else if (IIRValue == IIR_CTI)									//Character timeout indicator
	{
		UARTStatus |= 0x100 ;										//Bit 9 as the CTI error
	}
	else if (IIRValue == IIR_THRE)									//THRE, transmit holding register empty
	{
		LSRValue = LPC_USART->LSR ;									//Check status in the LSR to see if valid data in U0THR or not
		if (LSRValue & LSR_THRE)
		{
			UARTTxEmpty = 1 ;
		}
		else
		{
			UARTTxEmpty = 0 ;
		}
	}
	return ;
}
//***********************************************************************
//*								SUB_FUNCTION							*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Init							~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void InitUART(unsigned long Baudrate)
{
	unsigned long Fdiv ;
	unsigned long RegVal ;

	UARTTxEmpty = 1 ;
	UARTCount = 0 ;

	NVIC_DisableIRQ(UART_IRQn) ;									//Disable UART Interrupt
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	LPC_IOCON->PIO0_18 &= ~0x07 ;
	LPC_IOCON->PIO0_18 |= 0x01 ;									//UART RXD
	LPC_IOCON->PIO0_19 &= ~0x07 ;	
	LPC_IOCON->PIO0_19 |= 0x01 ;									//UART TXD

//	LPC_IOCON->PIO1_14 &= ~0x07 ;
//	LPC_IOCON->PIO1_14 |= 0x03 ;									//UART RXD
//	LPC_IOCON->PIO1_13 &= ~0x07 ;
//	LPC_IOCON->PIO1_13 |= 0x03 ;									//UART TXD

//  LPC_IOCON->PIO1_17 &= ~0x07 ;
//  LPC_IOCON->PIO1_17 |= 0x02 ;                                    //UART RXD
//  LPC_IOCON->PIO1_18 &= ~0x07 ;
//  LPC_IOCON->PIO1_18 |= 0x02 ;                                    //UART TXD
    
//	LPC_IOCON->PIO1_26 &= ~0x07 ;
//	LPC_IOCON->PIO1_26 |= 0x02 ;									//UART RXD
//	LPC_IOCON->PIO1_27 &= ~0x07 ;
//	LPC_IOCON->PIO1_27 |= 0x02 ;									//UART TXD
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//-----Enable UART clock
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12) ;							//Setup AHB Clock To The UART Domain
	LPC_SYSCON->UARTCLKDIV = 0x1 ;									//UARTCLK = MCU_OSC / UARTCLKDIV

	LPC_USART->LCR = 0x83 ;											//8 bits , No Parity , 1 Stop bit , DLAB = 1

	RegVal = LPC_SYSCON->UARTCLKDIV ;
	Fdiv = (((MCU_OSC/RegVal)/16)/Baudrate) ;						//Calculate Baudrate

	LPC_USART->DLM = Fdiv / 256 ;									//High 8 Bits							
	LPC_USART->DLL = Fdiv % 256 ;									//Low 8 Bits

	LPC_USART->LCR = 0x03 ;											//DLAB = 0
	LPC_USART->FCR = 0x07 ;											//Enable and reset TX and RX FIFO

	RegVal = LPC_USART->LSR ;										//Read to Clear The Line Status

	//Ensure a clean start, no data in either TX or RX FIFO
	while ( LPC_USART->LSR & (LSR_THRE|LSR_TEMT) != (LSR_THRE|LSR_TEMT) ) ;
	while ( LPC_USART->LSR & LSR_RDR )
	{
		RegVal = LPC_USART->RBR ;									//Dump data from RX FIFO
	}
    
	NVIC_EnableIRQ(UART_IRQn) ;										//Enable The UART Interrupt
    
	LPC_USART->IER = IER_RBR ;							            //Enable UART interrupt

	return ;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Drv								~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//-----DrvUARTSend-----
void DrvUARTSend(unsigned char *BufferPtr)
{
	unsigned char UARTOut ;
	UARTOut = 1 ;
	while (UARTOut == 1 )
	{
		if(*BufferPtr==0x0A)
		{
			UARTOut = 0 ;
		}
		while ( !(LPC_USART->LSR & LSR_THRE) ) ;
		LPC_USART->THR = *BufferPtr ;
		while ( !(LPC_USART->LSR & LSR_THRE) ) ;
		BufferPtr++ ;
	}
	return ;
}
//-----DrvUARTSendWord-----
void DrvUARTSendWord(unsigned char UARTDATA)
{
	while ( !(LPC_USART->LSR & LSR_THRE) ) ;
	LPC_USART->THR = UARTDATA ;
	while ( !(LPC_USART->LSR & LSR_THRE) ) ;
	return ;
}
void DrvUARTSendData(unsigned char *BufferPtr,unsigned int UARTCOUNT)
{
	unsigned char UARTOut ;
	UARTOut = 1 ;
	while (UARTOut == 1 )
	{
		while ( !(LPC_USART->LSR & LSR_THRE) ) ;
		LPC_USART->THR = *BufferPtr ;
		while ( !(LPC_USART->LSR & LSR_THRE) ) ;
		BufferPtr++ ;
        UARTCOUNT-- ;
		if(UARTCOUNT==0)
		{
			UARTOut = 0 ;
		}
	}
	return ;    
}

void xputchar(unsigned char ucVal)
{
	while ( !(LPC_USART->LSR & LSR_THRE) ){}
	LPC_USART->THR = ucVal; // transfer to uart
}
void xprintf(unsigned char *pFmt,int wVal,...)
{
	unsigned char ucBff,ucDisp;
	unsigned char bHex = FALSE,u8Into = 0,bNotZero = FALSE;
	int wDivider = 10000000;

    while((ucBff =*(pFmt++)) > 0)
    {
        if(ucBff == '%') // check special case
        {
            u8Into++;

            switch(*(pFmt++)) // check next character
            {
			case 'x': // hexadecimal number
			case 'X':
			wDivider = 0x10000000;
			bHex = TRUE;
			case 'd': // decimal number
			case 'i':
      

			if(wVal)
			{
				while(wDivider)
				{
					ucDisp = wVal / wDivider;
					wVal = wVal % wDivider;

					if(ucDisp)
					bNotZero=TRUE;


					if(bNotZero)
					{
						if (ucDisp > 9)
						xputchar(ucDisp-0x0a+'A');
						else
						xputchar(ucDisp+'0');
					}


					if(bHex)
					wDivider /= 0x10;
					else
					wDivider /= 10;
				}
			}
                    else
      	                xputchar('0');
                    break;
            } // switch
        }
        else // general
            xputchar(ucBff); // put a character
    } // while
}

void PrintFloat(float value)
{
    unsigned char tmp,tmp1,tmp2;
    tmp = (unsigned char)value;
    tmp1=(unsigned char)((value-tmp)*10)%10;
    tmp2=(unsigned char)((value-tmp)*100)%10;
//    tmp1=(unsigned char)((value-tmp)*10);
//    tmp2=(unsigned char)(((value-tmp)*10-tmp1)*10);
    xprintf("value=%d.\r\n",tmp);
	  xprintf("µã1=%d\r\n",tmp1);
	  xprintf("µã2=%d\r\n",tmp2);
}

#undef _UART_C_
//***********************************************************************
//*								END										*
//***********************************************************************
#endif
