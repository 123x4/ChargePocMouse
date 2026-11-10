#ifndef __OSC_H__
#define __0SC_H__
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************

//***********************************************************************
//*								DEFINE									*
//***********************************************************************
#define OSC_Freqrange           0                                   //Frequency Range(0:1~20MHz 1:15~25Mhz)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					OSC SETUP						~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define OSC						12000000UL							//Setup OSC Frequency
#define MAIN_SEL				1									//Select System OSC (0:IRC_OSC 1:SYSTEM_OSC)
#define PSEL					1									//Post Divider Ratio (0~3)
#define MSEL					3									//Feedback Divider Value(0~31)
#define SYS_AHB_DIV_VAL			1									//System AHB Clock Divider Values (1~255)
#define MCU_OSC					( OSC * ( MSEL + 1 ) )
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					OSC SETUP						~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define USB_SEL					1									//Select System OSC (0:IRC_OSC 1:SYSTEM_OSC)
#define USB_PSEL				1									//Post Divider Ratio (0~3)
#define USB_MSEL				3									//Feedback Divider Value(0~31)
#define USB_DIV_VAL				1									//System AHB Clock Divider Values (1~255)
#define USB_OSC					( OSC * ( USB_MSEL + 1 ) )
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 CLKOUT                          ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define CLKOUTCLK_SRC_IRC_OSC		0
#define CLKOUTCLK_SRC_SYS_OSC		1
#define CLKOUTCLK_SRC_LF_OSC		2
#define CLKOUTCLK_SRC_MAIN_CLK		3
//***********************************************************************
//*								REGISTER								*
//***********************************************************************

//***********************************************************************
//*								DECLARE									*
//***********************************************************************
extern void InitSystemOSC(unsigned int SysOSC , unsigned char SysPSEL , unsigned char SysMSEL , unsigned char AHBDiv) ;
extern void InitUSBOSC(unsigned char USBSEL , unsigned char USBPSEL , unsigned char USBMSEL ,unsigned char USBDiv ) ;
extern void CLKOUT(unsigned long CLK) ;
//***********************************************************************
//*								END										*
//***********************************************************************
#endif
