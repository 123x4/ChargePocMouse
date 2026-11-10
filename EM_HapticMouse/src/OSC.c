//***********************************************************************
//*Writer						ADAM									*
//*Function						OSC										*
//***********************************************************************
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************
#include "../inc/LPC11Uxx.h"
//#include "../inc/device.h"
#include "../inc/OSC.h"
//***********************************************************************
//*								REGISTER								*
//***********************************************************************

//***********************************************************************
//*								DECLARE									*
//***********************************************************************
void InitSystemOSC(unsigned int SysOSC , unsigned char SysPSEL , unsigned char SysMSEL , unsigned char AHBDiv) ;
void InitUSBOSC(unsigned char USBSEL , unsigned char USBPSEL , unsigned char USBMSEL ,unsigned char USBDiv ) ;
void CLKOUT(unsigned long CLK);
//***********************************************************************
//*								INTERRUPT_FUNCTION						*
//***********************************************************************

//***********************************************************************
//*								SUB_FUNCTION							*
//***********************************************************************
//-----InitSystemOSC----
void InitSystemOSC(unsigned int SysOSC , unsigned char SysPSEL , unsigned char SysMSEL , unsigned char AHBDiv)
{
	unsigned short int OSCI ;

	//-----
	LPC_SYSCON->SYSOSCCTRL |= (OSC_Freqrange<<1) ;					//Frequency Range 
	LPC_SYSCON->PDRUNCFG &= ~(0x1<<5) ;								//Crystal oscillator powered
	for( OSCI = 0; OSCI < 0x100; OSCI++ ) ;
	//-----
	LPC_SYSCON->SYSPLLCLKSEL = SysOSC ;								//Select System PLL clock source
	LPC_SYSCON->SYSPLLCLKUEN = 0x01 ;
	LPC_SYSCON->SYSPLLCLKUEN = 0x00 ;
	LPC_SYSCON->SYSPLLCLKUEN = 0x01 ;
	while ( !(LPC_SYSCON->SYSPLLCLKUEN & 0x01) ) ;
	//-----
	LPC_SYSCON->SYSPLLCTRL = ((SysPSEL<<5) | SysMSEL) ;				//Feedback divider value
	LPC_SYSCON->PDRUNCFG &= ~(0x1<<7) ;								//System PLL powered
	while ( !(LPC_SYSCON->SYSPLLSTAT & 0x01) ) ;
	//-----
	LPC_SYSCON->MAINCLKSEL = 0x03 ;									//Select Clock source for main clock(0:IRC 1:PLL In 2:WDT 3:PLL Out)
	LPC_SYSCON->MAINCLKUEN = 0x01 ;
	LPC_SYSCON->MAINCLKUEN = 0x00 ;
	LPC_SYSCON->MAINCLKUEN = 0x01 ;
	while ( !(LPC_SYSCON->MAINCLKUEN & 0x01) ) ;
	//-----
	LPC_SYSCON->SYSAHBCLKDIV = AHBDiv ;								//System AHB clock divider values
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16) ;							//Setup AHB Clock To The IOCON Domain
}
//-----InitUSBOSC-----
void InitUSBOSC(unsigned char USBSEL , unsigned char USBPSEL , unsigned char USBMSEL ,unsigned char USBDiv )
{
	LPC_SYSCON->PDRUNCFG &= ~(1 << 10) ;							//Power On USB PHY
	LPC_SYSCON->PDRUNCFG &= ~(1 <<  8) ;							//Power On USB PLL
	//-----
	LPC_SYSCON->USBPLLCLKSEL = 0x01 ;								//Select USB PLL clock source(Only System oscillator)
	LPC_SYSCON->USBPLLCLKUEN = 0x01 ;
	LPC_SYSCON->USBPLLCLKUEN = 0x00 ;
	LPC_SYSCON->USBPLLCLKUEN = 0x01 ;
	while (!(LPC_SYSCON->USBPLLCLKUEN & 0x01)) ;
	//-----
	LPC_SYSCON->USBPLLCTRL = ((USBPSEL<<5) | USBMSEL) ;				//USB Feedback divider value
	while (!(LPC_SYSCON->USBPLLSTAT & 0x01)) ;
	//-----
	LPC_SYSCON->USBCLKSEL = 0x00 ;									//Select USB clock source(0 = USB PLL ; 1 = Main Clock)
	LPC_SYSCON->USBCLKDIV = USBDiv ;								//Set USB clock divider
}
//-----CLKOUT-----
void CLKOUT(unsigned long CLK) 
{
	LPC_SYSCON->CLKOUTSEL = CLK ;									//Select Clk Output (0 = IRC ; 1 = Crystal OSC ; 2 = LF OSC ; 3 = Main OSC)
	LPC_SYSCON->CLKOUTUEN = 0x01 ;
	LPC_SYSCON->CLKOUTUEN = 0x00 ;
	LPC_SYSCON->CLKOUTUEN = 0x01 ;
	while ( !(LPC_SYSCON->CLKOUTUEN & 0x01) ) ;
	LPC_SYSCON->CLKOUTDIV = 1 ;

	LPC_IOCON->PIO0_1 &= ~0x07 ;                                    //Set PIO0_1 To ClockOut
	LPC_IOCON->PIO0_1 |= 0x01 ;
	return ;
}
//***********************************************************************
//*								END										*
//***********************************************************************
