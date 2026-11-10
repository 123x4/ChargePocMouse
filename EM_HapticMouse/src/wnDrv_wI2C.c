//***********************************************************************
//*Writer						ADAM									*
//*Function						I2C										*
//***********************************************************************
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************
#include "wnDrv_wI2C.h"
#include "../inc/LPC11Uxx.h"
#include "../inc/Main.h"
#include "../inc/OSC.h"
#include "../inc/wnDrv_wI2C.h"

//***********************************************************************
//*								REGISTER								*
//***********************************************************************
unsigned char I2CMasterState ;    									//I2C Master State
unsigned char I2CBusState ;                                         //I2C BUS State

unsigned char I2CMasterWR ;                                         //I2C Master Write or Read Fg(0 = Read , 1 = Write)

unsigned char I2CAddress ;											//I2C Send Slave Address
unsigned char I2CRegister ;											//I2C Send Slave W/R Register
unsigned char I2CWriteLength ;
unsigned char I2CReadLength ;
unsigned char I2CWriteAddress ;
unsigned char I2CReadAddress ;
unsigned char I2CTransBuffer[I2CBUFFERSIZE]	;
unsigned char I2CReceiverBuffer[I2CBUFFERSIZE]	;

unsigned char I2CIntStatus ;
unsigned char I2CWriteCount ;
unsigned char I2CReadCount ;
unsigned long I2CTimeOut ;
//***********************************************************************
//*								DECLARE									*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Init							~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void wnDrv_wI2C_Init(void) ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Drv								~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unsigned char wnDrv_wI2C_Write(unsigned char Address , unsigned char Register , unsigned char DataAddress , unsigned char Length) ;
unsigned char wnDrv_wI2C_Read(unsigned char Address , unsigned char Register , unsigned char DataAddress , unsigned char Length) ;
//***********************************************************************
//*								INTERRUPT_FUNCTION						*
//***********************************************************************
//-----I2C_IRQHandler-----
void I2C_IRQHandler(void) 
{
	I2CIntStatus = LPC_I2C->STAT ;
	I2CTimeOut = 0 ;
	switch(I2CIntStatus)
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x08 :													//Master First Start(Start Status)
            LPC_I2C->DAT = I2CAddress ;
			LPC_I2C->CONCLR = 0x20 ;								//Clear Start Status
			I2CMasterState = I2C_STARTED ;
		break ;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x10 :													//Master Repeated Start(Start Status)
			I2CReadCount = 0 ;
			LPC_I2C->DAT = I2CAddress | 0x01 ;
			LPC_I2C->CONCLR = 0x20 ;								//Clear Start Status
			I2CMasterState = I2C_RESTARTED ;
		break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x18 :													//ACK After Write_Address(First ACK)
			I2CWriteCount = 0 ;
			LPC_I2C->DAT = I2CRegister ;
			I2CMasterState = I2C_ACK_ADDRESS ;
        break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x20 :													//NACK After Write_Address(First ACK)
            LPC_I2C->CONSET = 0x10 ;                                //Set Stop Status
            LPC_I2C->CONSET = 0x04 ;                                //Set ACK Status
            I2CBusState = I2C_BUS_IDLE ;
			I2CMasterState = I2C_NACK_ADDRESS ;
        break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x28 :													//ACK After Register(Other ACK)
           if(I2CMasterWR==0)
            {
                LPC_I2C->CONSET = 0x20 ;                            //Set Start Status
                I2CMasterState = I2C_ACK_REGISTER ;
            }
            if(I2CMasterWR==1)
            {
                if ( I2CWriteCount < I2CWriteLength )
                {
                    if(I2CWriteCount == 0 )
                    {
                        I2CMasterState = I2C_ACK_REGISTER ;                       
                    }
                    else
                    {
                        I2CMasterState = I2C_DATA_ACK ;                       
                    }
                    LPC_I2C->DAT = I2CTransBuffer[I2CWriteAddress+I2CWriteCount] ;
                    I2CWriteCount++ ;                    

                }
                else
                {
                    LPC_I2C->CONSET = 0x10 ;                        //Set Stop Status
                    LPC_I2C->CONSET = 0x04 ;                        //Set ACK Status
                    I2CBusState = I2C_BUS_IDLE ;
                    I2CMasterState = I2C_OK ;
                }
            }
		break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x30 :													//NACK After Register(Other ACK)
			if(I2CWriteCount==0)
			{
				I2CMasterState = I2C_NACK_REGISTER ;
			}
			else
			{
				I2CMasterState = I2C_NACK_DATA ;
			}
        break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x38 :
		break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x40 :													//ACK After Read_Address
            if(I2CReadLength==1)
            {
                LPC_I2C->CONCLR = 0x04 ;
            }
            else
            {
                LPC_I2C->CONSET = 0x04 ;							//Set ACK Status
            }
            I2CMasterState = I2C_ACK_READDRESS ;
        break ;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x48 :													//NACK After Read_Address
            LPC_I2C->CONSET = 0x10 ;                                //Set Stop Status
            LPC_I2C->CONSET = 0x04 ;                                //Set ACK Status
            I2CBusState = I2C_BUS_IDLE ;
            I2CMasterState = I2C_NACK_READDRESS ;
        break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x50 :													//Slave Send Data & Master Send ACK
			I2CReceiverBuffer[I2CReadAddress+I2CReadCount] = LPC_I2C->DAT ;
			I2CReadCount++ ;
			I2CMasterState = I2C_DATA_ACK ;
			if(I2CReadCount < (I2CReadLength-1))
			{
				LPC_I2C->CONSET = 0x04 ;							//Set ACK Status
			}
			else
			{
				LPC_I2C->CONCLR = 0x04 ;							//Clear ACK Status
			}
        break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case 0x58 :													//Slave Send Data & Master Send NACK
            I2CReceiverBuffer[I2CReadAddress+I2CReadCount] = LPC_I2C->DAT ;
            LPC_I2C->CONSET = 0x10 ;                                //Set Stop Status
            LPC_I2C->CONSET = 0x04 ;                                //Set ACK Status
            I2CBusState = I2C_BUS_IDLE ;
            I2CMasterState = I2C_OK ;
        break ;
//~~~~~SLAVE INTERRUPT~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x60 :													//Master Send Write Address & Slave Send ACK
			I2CReadCount = 0 ;
			LPC_I2C->CONSET = 0x04 ;								//Set ACK Status
//			I2CSlaveState = I2C_SLAVE_BUSY ;
        break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x68 :                                                 //Master Send Write Address & Slave Send NACK
			LPC_I2C->CONSET = 0x04 ;
        break ;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x70 :													//General Call & Slave Send ACK
			I2CReadCount = 0 ;
			LPC_I2C->CONSET = 0x04 ;
//			I2CSlaveState = I2C_SLAVE_BUSY ;
        break ;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x78 :													//General Call & Slave Send NACK
			LPC_I2C->CONSET = 0x04 ;
		break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x80 :													//Master Send Data & Slave Send ACK
			I2CReceiverBuffer[I2CReadCount] = LPC_I2C->DAT ;
			I2CReadCount++ ;
			LPC_I2C->CONSET = 0x04 ;
        break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case 0x88 :													//Master Send Data & Slave Send NACK
			I2CReceiverBuffer[I2CReadCount] = LPC_I2C->DAT ;
			I2CReadCount++ ;
			LPC_I2C->CONSET = 0x04 ;
		break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x90 :													//General Call Master Send Data & Slave Send ACK
			I2CReceiverBuffer[I2CReadCount] = LPC_I2C->DAT ;
			I2CReadCount++ ;
			LPC_I2C->CONSET = 0x04 ;
        break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x98 :													//General Call Master Send Data & Slave Send NACK
			I2CReceiverBuffer[I2CReadCount] = LPC_I2C->DAT ;
			I2CReadCount++ ;
			LPC_I2C->CONSET = 0x04 ;
		break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
		case 0xA0 :													//Repart Start Or Stop Condition Happen
			LPC_I2C->CONSET = 0x04 ;
		break ;
//
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0xA8 :													//Master Send Read Address & Slave Send ACK
			I2CWriteCount = 0 ;
			LPC_I2C->DAT = I2CTransBuffer[I2CWriteCount] ;
			I2CWriteCount++ ;
			LPC_I2C->CONSET = 0x04 ;
        break ;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0xB0 :													//Master Send Read Address & Slave Send NACK
			LPC_I2C->CONSET = 0x04 ;
        break ;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0xB8 :													//Master Send Clock & Slave Send Data & Master Send ACK
			LPC_I2C->DAT = I2CTransBuffer[I2CWriteCount] ;
			I2CWriteCount++ ;
			LPC_I2C->CONSET = 0x04 ;
        break ;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0xC0 :													//Master Send Clock & Slave Send Data & Masyer Send NACK
			LPC_I2C->CONSET = 0x04 ;
		break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0xC8 :                                                 //
			LPC_I2C->DAT = I2CTransBuffer[I2CWriteCount] ;
			I2CWriteCount++ ;
			LPC_I2C->CONSET = 0x04 ;
		break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		default :													//
		break ;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}
	LPC_I2C->CONCLR = 0x08 ;										//Clear Interrupt Status
	return ;
}
//***********************************************************************
//*								SUB_FUNCTION							*
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Init							~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void wnDrv_wI2C_Init(void)
{
	LPC_SYSCON->PRESETCTRL |= (0x1<<1) ;                            //I2C Reset De_asserted
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5) ;                           //Setup AHB Clock To The I2C Domain
	/*
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	LPC_IOCON->PIO0_4 &= ~0x07 ;                                    //I2C_SCL
	LPC_IOCON->PIO0_4 |= 0x01 ;
	LPC_IOCON->PIO0_5 &= ~0x07 ;                                    //I2C_SDA
	LPC_IOCON->PIO0_5 |= 0x01 ;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	LPC_IOCON->PIO0_4 &= ~0x0307 ;                                    //Nikita I2C_SCL, Fast-mode Plus I2C
	LPC_IOCON->PIO0_4 |= 0x0201 ;
	LPC_IOCON->PIO0_5 &= ~0x0307 ;                                    //Nikita I2C_SDA, Fast-mode Plus I2C
	LPC_IOCON->PIO0_5 |= 0x0201 ;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//Clear flags
    LPC_I2C->CONCLR = 0x6C ;
    //Speed Setup
    LPC_I2C->SCLL   = I2C_SCL_LOW ;                                 //Set I2C Clock low Timer
    LPC_I2C->SCLH   = I2C_SCL_HIGH ;                                //Set I2C Clock High Timer

    LPC_I2C->ADR0 = I2C_SLAVE_ADDRESS ;                             //Set MCU I2C_Slave Address

    //Enable the I2C Interrupt
    NVIC_EnableIRQ(I2C_IRQn) ;                                      //Set I2C Interrupt
    LPC_I2C->CONSET = 0x44 ;                                        //Setup I2C_EN & I2C_AA
    return ;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~					Drv								~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//-----wnDrv_wI2C_Write-----
unsigned char wnDrv_wI2C_Write(unsigned char Address , unsigned char Register , unsigned char DataAddress , unsigned char Length)
{

    I2CMasterWR = 1 ;
    
    I2CAddress = Address;
    I2CRegister = Register ;
    I2CWriteAddress = DataAddress ;
    I2CWriteLength = Length ;

    LPC_I2C->CONSET = 0x20 ;
    
    I2CMasterState = I2C_IDLE ;
    I2CBusState = I2C_BUS_BUSY ; 
    I2CTimeOut = 0 ;
    
    while ( I2CBusState == I2C_BUS_BUSY )
    {
        if ( I2CTimeOut >= 0x00FFFFFF )
        {
            I2CBusState = I2C_BUS_IDLE ;
            I2CMasterState = I2C_TIMEOUT;
        }
        I2CTimeOut++ ;
    }
    return I2CMasterState ;
}
//-----wnDrv_wI2C_Read-----
unsigned char wnDrv_wI2C_Read(unsigned char Address , unsigned char Register , unsigned char DataAddress , unsigned char Length)
{
    I2CMasterWR = 0 ;
    
    I2CAddress = Address;
    I2CRegister = Register ;
    I2CReadAddress = DataAddress ;
    I2CReadLength = Length ;
    
    LPC_I2C->CONSET = 0x20 ;
    
    I2CMasterState = I2C_IDLE ;
    I2CBusState = I2C_BUS_BUSY ; 
    I2CTimeOut = 0 ;
    
    while ( I2CBusState == I2C_BUS_BUSY )
    {
        if ( I2CTimeOut >= 0x00FFFFFF )
        {
            I2CBusState = I2C_BUS_IDLE ;
            I2CMasterState = I2C_TIMEOUT;
        }
        I2CTimeOut++ ;
    }
    return I2CMasterState ;
}
//***********************************************************************
//*								END										*
//***********************************************************************
