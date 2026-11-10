#ifndef __I2C_H__
#define __I2C_H__
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************
//#include "wnBoard.h"
//#include "wnDrv_System_Lpc11uxx.h"


//***********************************************************************
//*								DEFINE									*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 I2C                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define I2C_SLAVE_ADDRESS		0xA0								//SLAVE Address
#define I2C_SPEED				1000000								//400K, 200000, 1000000(1MHz)
#define I2C_SCL_HIGH			( wnDrv_System_Lpc11uxx_GetCoreClock() / I2C_SPEED ) / 2
#define I2C_SCL_LOW				( wnDrv_System_Lpc11uxx_GetCoreClock() / I2C_SPEED ) / 2

#define I2C_MAXTIMEOUT			0xFFFFFF00
#define I2CBUFFERSIZE			0x40
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					I2C_Status						~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define I2C_BUS_IDLE            0
#define I2C_BUS_BUSY            1

#define I2C_OK					0
#define I2C_IDLE				1
#define I2C_STARTED				2
#define I2C_NSTARTED			3
#define I2C_RESTARTED			4
#define I2C_ACK_ADDRESS			5
#define I2C_NACK_ADDRESS		6
#define I2C_ACK_REGISTER        7
#define I2C_NACK_REGISTER		8
#define I2C_DATA_ACK			9
#define I2C_NACK_DATA			10
#define I2C_ACK_READDRESS		11
#define I2C_NACK_READDRESS		12
#define I2C_TIMEOUT				13

//***********************************************************************
//*								REGISTER								*
//***********************************************************************
extern unsigned char I2CMasterState ;    				            //I2C Master State
extern unsigned char I2CBusState ;                                  //I2C BUS State

extern unsigned char I2CMasterWR ;                                  //I2C Master Write or Read Fg(0 = Read , 1 = Write)

extern unsigned char I2CAddress ;									//I2C Send Slave Address
extern unsigned char I2CRegister ;									//I2C Send Slave W/R Register
extern unsigned char I2CWriteLength ;
extern unsigned char I2CReadLength ;
extern unsigned char I2CWriteAddress ;
extern unsigned char I2CReadAddress ;
extern unsigned char I2CTransBuffer[I2CBUFFERSIZE]	;
extern unsigned char I2CReceiverBuffer[I2CBUFFERSIZE]	;

extern unsigned char I2CIntStatus ;
extern unsigned char I2CWriteCount ;
extern unsigned char I2CReadCount ;
extern unsigned long I2CTimeOut ;
//***********************************************************************
//*								DECLARE									*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Init							~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern void wnDrv_wI2C_Init(void) ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Drv								~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern unsigned char wnDrv_wI2C_Write(unsigned char Address , unsigned char Register , unsigned char DataAddress , unsigned char Length) ;
extern unsigned char wnDrv_wI2C_Read(unsigned char Address , unsigned char Register , unsigned char DataAddress , unsigned char Length) ;
//***********************************************************************
//*								END										*
//***********************************************************************
#endif
