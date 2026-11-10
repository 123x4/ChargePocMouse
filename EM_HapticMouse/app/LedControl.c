#include "../inc/LPC11Uxx.h"
//#include "../inc/device.h"
#include "../inc/I2C.h"
#include "../inc/OSC.h"
#include "led1.h"
#include "../inc/LedControl.h"
#include "../inc/GPIO.h"
//i2C 400kbps = 2.5us/bit  1.4ms can write 72 byte


///because the LED I2C read is special so write here
void I2cMaster_Read(uint8_t slaveAdr,Point_Page page,uint8_t regAddr,uint8_t* dataArray ,uint32_t count)
{
	uint8_t databuff[2] , i;
	//uint8_t status = I2C_COM_ERROR;

	
	//  Modify by Matin 2017.11.08  
	databuff[0]= LED_DRIVER_CMD_REG;
	databuff[1]= (uint8_t)page;
	I2CTransBuffer[0] = databuff[0];
	I2CTransBuffer[1] = databuff[1];
	wnDrv_wI2C_Write(slaveAdr , LED_DRIVER_CMD_REG , 0x01 , 1);
	
	databuff[0] = regAddr;
	wnDrv_wI2C_Read(slaveAdr , databuff[0] , 0x0 , count );
	for(i=0;i<count;i++)
	{
		dataArray[i] = I2CReceiverBuffer[i];
	}


	
	#if 0
//	I2cMasterModule_I2CMasterWriteBuf(slaveAdr, databuff , 2,\
//		I2cMasterModule_I2C_MODE_NO_STOP);
//	/* Waits until master completes write transfer */
//	while (0u == (I2cMasterModule_I2CMasterStatus() & I2cMasterModule_I2C_MSTAT_WR_CMPLT))
//	{
//	}
//	(void) I2cMasterModule_I2CMasterClearStatus();


//	databuff[0] = regAddr;
//	I2cMasterModule_I2CMasterWriteBuf(slaveAdr, databuff , 1,\
//		I2cMasterModule_I2C_MODE_REPEAT_START);

//	/* Waits until master completes write transfer */
//	while (0u == (I2cMasterModule_I2CMasterStatus() & I2cMasterModule_I2C_MSTAT_WR_CMPLT))
//	{
//	}

//	I2cMasterModule_I2CMasterReadBuf(slaveAdr, dataArray, count ,\
//		I2cMasterModule_I2C_MODE_REPEAT_START);


//	/* Waits until master complete read transfer */
//	while (0u == (I2cMasterModule_I2CMasterStatus() & I2cMasterModule_I2C_MSTAT_RD_CMPLT))
//	{
//	}
//	(void) I2cMasterModule_I2CMasterClearStatus();
#endif

}

/*************************************************************************************/
///for 3731 led drive  1  use

uint8_t LedControl_Writebyte(uint8_t slaverAdr,uint8_t reg_addr,uint8_t data)
{
//    I2cMaster_Writebyte(slaverAdr,reg_addr,data);   // Mark By Martin 2017.11.08
		
		// Modify by Martin 2017.11.08
		I2CTransBuffer[0] = data;
		wnDrv_wI2C_Write(slaverAdr,reg_addr,0x00,1);

	return 0;
}

uint8_t LedControl_WriteArray(uint8_t slaverAdr,uint8_t* dataArray, uint32_t count)
{
//	I2cMaster_WriteArray(slaverAdr,dataArray,count); // Mark By Martin 2017.11.08
		uint32_t i;
		for(i=0;i<=count;i++)
		{
			I2CTransBuffer[i] = dataArray[i];
		}
		wnDrv_wI2C_Write(slaverAdr,I2CTransBuffer[0],0x01,count - 1);//I2C write 
    

	return 0;
}

void LedControl_Read(uint8_t slaverAdr,Point_Page page,uint8_t regAddr,uint8_t* dataArray ,uint32_t count)
{
    I2cMaster_Read(slaverAdr,page,regAddr,dataArray,count);
}

void LedControl_EnableHardwareShutdown(void)
{
//	Led_Reset_Write(0);  /* Hardware shutdown by pull the SDB to low */   // Mark By Martin 2017.11.08
	DrvGPIOOutput(0,9,0);	// Reset pin Lo
}

void LedControl_DisableHardwareShutdown(void)
{
//	Led_Reset_Write(1);  /* Disable Hardware shutdown by pull the SDB to high */  // Mark By Martin 2017.11.08
	DrvGPIOOutput(0,9,1);	// Reset pin Lo
}

void LedControl_Initial(void)
{
	/* disable the hardware shutdown */
	LedControl_DisableHardwareShutdown();

//	Led_INTB_Write(1);   /* set to high to wait for interrupt or pull low */

//	I2cMasterModule_Start();    /* Start the I2C block */
}

/*****************************************************************************************/














