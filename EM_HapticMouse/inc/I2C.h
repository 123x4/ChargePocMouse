#ifndef __I2C_H__
#define __I2C_H__
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************

//***********************************************************************
//*								DEFINE									*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 I2C                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define I2C_SLAVE_ADDRESS		0xA0								//SLAVE Address
#define I2C_SPEED				400000	//		1000000 //				//400K
#define I2C_SCL_HIGH			( MCU_OSC / I2C_SPEED ) / 2
#define I2C_SCL_LOW				( MCU_OSC / I2C_SPEED) / 2

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
extern volatile unsigned char I2CMasterState ;    				            //I2C Master State
extern volatile unsigned char I2CBusState ;                                  //I2C BUS State

extern volatile unsigned char I2CMasterWR ;                                  //I2C Master Write or Read Fg(0 = Read , 1 = Write)

extern volatile unsigned char I2CAddress ;									//I2C Send Slave Address
extern volatile unsigned char I2CRegister ;									//I2C Send Slave W/R Register
extern volatile unsigned char I2CWriteLength ;
extern volatile unsigned char I2CReadLength ;
extern unsigned char I2CWriteAddress ;
extern unsigned char I2CReadAddress ;
extern volatile unsigned char I2CTransBuffer[I2CBUFFERSIZE]	;
extern volatile unsigned char I2CReceiverBuffer[I2CBUFFERSIZE]	;

extern volatile unsigned char I2CIntStatus ;
extern unsigned char I2CWriteCount ;
extern unsigned char I2CReadCount ;
extern volatile unsigned long I2CTimeOut ;
//***********************************************************************
//*								DECLARE									*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Init							~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern void InitI2C(void) ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Drv								~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern unsigned char DrvI2CWrite(unsigned char Address , unsigned char Register , unsigned char DataAddress , unsigned char Length) ;
extern unsigned char DrvI2CRead(unsigned char Address , unsigned char Register , unsigned char DataAddress , unsigned char Length) ;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Drv								~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern unsigned char wnDrv_wI2C_Write(unsigned char Address , unsigned char Register , unsigned char DataAddress , unsigned char Length) ;
extern unsigned char wnDrv_wI2C_Read(unsigned char Address , unsigned char Register , unsigned char DataAddress , unsigned char Length) ;

//***********************************************************************
//*								END										*
//***********************************************************************
#endif
