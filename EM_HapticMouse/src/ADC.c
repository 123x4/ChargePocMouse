//***********************************************************************
//*Writer						ADAM									*
//*Function						ADC                                     *
//***********************************************************************
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************
#include "../inc/LPC11Uxx.h"
//#include "../inc/device.h"
#include "../inc/OSC.h"
#include "../inc/ADC.h"
//***********************************************************************
//*                             REGISTER                                *
//***********************************************************************

//***********************************************************************
//*                             DECLARE                                 *
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Init                            ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void InitADC(unsigned int ADCClk) ;
unsigned int ADCRead(unsigned char Channel) ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Drv                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//***********************************************************************
//*                             INTERRUPT_FUNCTION                      *
//***********************************************************************

//***********************************************************************
//*                             SUB_FUNCTION                            *
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Init                            ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if 0
void InitADC(unsigned int ADCClk)
{
	//Powered to the ADC block ( 0 = Powered ; 1 = Powered Down)
	LPC_SYSCON->PDRUNCFG &= ~( 0x1 << 4 ) ;
	//Enable AHB clock to the ADC
	LPC_SYSCON->SYSAHBCLKCTRL |= ( 1 << 13 ) ;
    //-----GPIO To ADC Input-----
//     LPC_IOCON->TDI_PIO0_11 &= ~0x9F ;
//     LPC_IOCON->TDI_PIO0_11 |= 0x02 ;                                //AD0
//     LPC_IOCON->TMS_PIO0_12 &= ~0x9F ;
//     LPC_IOCON->TMS_PIO0_12 |= 0x02 ;                                //AD1
//     LPC_IOCON->TDO_PIO0_13 &= ~0x9F ;
//     LPC_IOCON->TDO_PIO0_13 |= 0x02 ;                                //AD2
//     LPC_IOCON->TRST_PIO0_14 &= ~0x9F ;
//     LPC_IOCON->TRST_PIO0_14 |= 0x02 ;                               //AD3
//     LPC_IOCON->SWDIO_PIO0_15 &= ~0x9F ;
//     LPC_IOCON->SWDIO_PIO0_15 |= 0x02 ;                              //AD4
//     LPC_IOCON->PIO0_16 &= ~0x9F ;
//     LPC_IOCON->PIO0_16 |= 0x02 ;                                    //AD5
    LPC_IOCON->PIO0_22 &= ~0x9F ;
    LPC_IOCON->PIO0_22 |= 0x01 ;                                    //AD6
    LPC_IOCON->PIO0_23 &= ~0x9F ;
    LPC_IOCON->PIO0_23 |= 0x01 ;                                    //AD7    
	//-----
    LPC_ADC->CR =	( 0x01 << 0 ) |									//SEL=1,select channel 0~7 on ADC0 (8bit)
					( ( MCU_OSC / ADCClk - 1 ) << 8 ) |				//The APB clock (PCLK) is divided by CLKDIV +1 to produce the clock for the ADC(8bit)
					( 0 << 16 ) |									//BURST MODE ( 0 = Software-controlled mode ; 1 = Hardware scan mode) (1bit)
					( 0 << 17 ) |									//ADC Resolution & Conversion Time (11 clocks/10 bits) (3bit)
					( 0 << 24 ) |									//No start (3bit)
					( 0 << 27 ) ;									//EDGE = 0
                    
//	NVIC_EnableIRQ(ADC_IRQn) ;										//
//	LPC_ADC->INTEN = 0x1FF ;										//Enable all interrupts
	return ;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Drv                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unsigned int ADCRead(unsigned char Channel)
{
	unsigned int RegVal ;
	unsigned int ADC_Data ;

	//Channel number Over MCU ADC
	if ( Channel >= 8 )
	{
		Channel = 0 ;												//Reset Channel Number to 0
	}
	LPC_ADC->CR &= 0xFFFFFF00 ;
	LPC_ADC->CR |= (1 << 24) | (1 << Channel) ;						//設定 Channel 及開始抓取	

	while ( 1 )
	{
		//Read Channel Data
		RegVal = *(volatile unsigned long *)(LPC_ADC_BASE + ADC_OFFSET + ADC_INDEX * Channel) ;
		if ( RegVal & ADC_DONE )
		{
			break;
		}
	}	
	LPC_ADC->CR &= 0xF8FFFFFF ;										//stop ADC now
	if ( RegVal & ADC_OVERRUN )										//save data when it's not overrun, otherwise, return zero
	{
		return ( 0 );
	}
	ADC_Data = ( RegVal >> 6 ) & 0x3FF;
	return ( ADC_Data ) ;
}
#endif
//***********************************************************************
//*                             END                                     *
//***********************************************************************
