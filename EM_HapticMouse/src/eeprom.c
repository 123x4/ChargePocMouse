#include "eeprom.h"
#include "test.h"
#include "device.h"
#include "gtech_flash.h"

#if USE_INSIDE_EEPROM

#else
void SPI_FLASH_Write_Enable(void)   
{
	UINT8 byte = 0;

	SPI_HARDWARE_DIRVE_OFF_FLASH_CS();   

	SPI_ReadWriteOneByte(MX25L_WriteEnable , &byte);

	SPI_HARDWARE_DIRVE_ON_FLASH_CS();   

} 

void SPI_FLASH_Write_Disable(void)   
{  
	UINT8 byte = 0;

	SPI_HARDWARE_DIRVE_OFF_FLASH_CS();   

	SPI_ReadWriteOneByte(MX25L_WriteDisable , &byte);

	SPI_HARDWARE_DIRVE_ON_FLASH_CS();   
}


UINT16 SPI_Flash_ReadID(void)
{
	UINT8 byte = 0;
	UINT16 Temp = 0;	  

	SPI_HARDWARE_DIRVE_OFF_FLASH_CS();   

	SPI_ReadWriteOneByte(MX25L_ManufactDeviceID,&byte);
	SPI_ReadWriteOneByte(0xFF,&byte);  //dummy   datasheet 12
	SPI_ReadWriteOneByte(0xFF,&byte);  //dummy
	SPI_ReadWriteOneByte(0x00,&byte);  //0x00

	SPI_ReadWriteOneByte(0xFF,&byte);	
	Temp |= byte<<8;
	SPI_ReadWriteOneByte(0xFF,&byte); 
	Temp |= byte;	 

	SPI_HARDWARE_DIRVE_ON_FLASH_CS();
	return Temp;
}

void FLASH_Communication_Check(void)
{
	UINT8 timer = 15 ;//ii;	
	UINT16 flashID;
	
	flashID = SPI_Flash_ReadID();
	
	if ((flashID != MX25L_PID) && (flashID != ZB25V_PID))  //read ID reset
	{
		flashID = SPI_Flash_ReadID();
		while ((flashID != MX25L_PID) && (flashID != ZB25V_PID) && timer)
		{
			timer--;
			TimerDelayMsec(10);
			flashID = SPI_Flash_ReadID();
//			M8C_ClearWDTAndSleep;
		}
	}
}

UINT8 SPI_Flash_ReadSR(void)
{
	UINT8 byte = 0;

	SPI_HARDWARE_DIRVE_OFF_FLASH_CS();

	SPI_ReadWriteOneByte(MX25L_ReadStatusReg , &byte);
	SPI_ReadWriteOneByte(0xFF , &byte);

	SPI_HARDWARE_DIRVE_ON_FLASH_CS();

	return byte;
}

void SPI_FLASH_Write_SR(UINT8 sr)   
{   
	UINT8 byte = 0;
	SPI_HARDWARE_DIRVE_OFF_FLASH_CS();   

	SPI_ReadWriteOneByte(MX25L_WriteStatusReg,&byte);    
	SPI_ReadWriteOneByte(sr,&byte);     

	SPI_HARDWARE_DIRVE_ON_FLASH_CS();
} 



void SPI_Flash_Read(UINT8* pBuffer,UINT32 ReadAddr,UINT16 NumByteToRead)   
{ 
	UINT16 i;  
	UINT8 byte = 0;

	SPI_SCLK_Speed(SPI_CLK_SPEED_12M);

	SPI_HARDWARE_DIRVE_OFF_FLASH_CS(); 

	SPI_ReadWriteOneByte(MX25L_ReadData , &byte);         
	SPI_ReadWriteOneByte((UINT8)((ReadAddr)>>16) , &byte); 
	SPI_ReadWriteOneByte((UINT8)((ReadAddr)>>8) , &byte); 
	SPI_ReadWriteOneByte((UINT8)ReadAddr , &byte); 

	for(i = 0; i < NumByteToRead; i++)
	{ 
		SPI_ReadWriteOneByte(0XFF, pBuffer + i);  
	}
	SPI_HARDWARE_DIRVE_ON_FLASH_CS();

	SPI_SCLK_Speed(SPI_CLK_SPEED_1_5M);
}  


void SPI_Flash_Wait_Busy(void)   
{   
	while((SPI_Flash_ReadSR()&0x01) == 0x01) 
	{
	
	}
//		M8C_ClearWDTAndSleep;    
} 

void SPI_Flash_Wait_Busy_when_erase(void)   
{   
	while((SPI_Flash_ReadSR()&0x01) == 0x01) 
	{
//		M8C_ClearWDTAndSleep; 

#ifdef OPTICAL_HANDLE_WHEN_MACRO

#endif 
	}	
} 

void SPI_Flash_Write_Page(UINT8* pBuffer,UINT32 WriteAddr,UINT16 NumByteToWrite)
{
	UINT16 i;  
	UINT8 byte = 0;

	SPI_FLASH_Write_Enable();

	SPI_HARDWARE_DIRVE_OFF_FLASH_CS(); 

	SPI_ReadWriteOneByte(MX25L_PageProgram , &byte);  
	SPI_ReadWriteOneByte((UINT8)((WriteAddr)>>16) , &byte); 
	SPI_ReadWriteOneByte((UINT8)((WriteAddr)>>8) , &byte); 
	SPI_ReadWriteOneByte((UINT8)WriteAddr , &byte);

	for(i = 0; i < NumByteToWrite; i++)
		SPI_ReadWriteOneByte(pBuffer[i], &byte); 

	SPI_HARDWARE_DIRVE_ON_FLASH_CS();      

	SPI_Flash_Wait_Busy();					   
} 



void SPI_Flash_Write_Byte(UINT8* pBuffer,UINT32 WriteAddr,UINT16 NumByteToWrite)   
{ 			 		 
	UINT16 pageremain;	 

	SPI_SCLK_Speed(SPI_CLK_SPEED_12M);

	pageremain = 256 - WriteAddr%256; 

	if(NumByteToWrite <= pageremain) pageremain = NumByteToWrite;

	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite == pageremain) break;
		else 
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;			  
			if(NumByteToWrite > 256) pageremain = 256; 
			else pageremain = NumByteToWrite; 	 
		}
	} 

	SPI_SCLK_Speed(SPI_CLK_SPEED_1_5M);
} 


void SPI_Flash_Erase_Sector(UINT8 Sector_No)   
{          
	UINT8 byte = 0;
	UINT32 Dst_Addr;

	Dst_Addr = (4096 * (UINT32)Sector_No);

	//Dst_Addr = (4096 * (UINT32)Sector_No);

	SPI_Flash_Wait_Busy_when_erase();

	SPI_FLASH_Write_Enable();                  //SET WEL 
	SPI_Flash_Wait_Busy();  

	SPI_HARDWARE_DIRVE_OFF_FLASH_CS(); 

	SPI_ReadWriteOneByte(MX25L_SectorErase , &byte); 
	SPI_ReadWriteOneByte((UINT8)((Dst_Addr)>>16) , &byte); 
	SPI_ReadWriteOneByte((UINT8)((Dst_Addr)>>8) , &byte); 
	SPI_ReadWriteOneByte((UINT8)Dst_Addr , &byte);

	SPI_HARDWARE_DIRVE_ON_FLASH_CS();

	SPI_Flash_Wait_Busy_when_erase();

			   
}  

void SPI_Flash_Erase_Chip(void)   
{        
	UINT8 byte = 0;

	//SPI_Flash_Wait_Busy_when_erase();

	SPI_SCLK_Speed(SPI_CLK_SPEED_12M);  //20180818 

	SPI_FLASH_Write_Enable();                   
	SPI_Flash_Wait_Busy();   

	SPI_HARDWARE_DIRVE_OFF_FLASH_CS(); 

	SPI_ReadWriteOneByte(MX25L_ChipErase , &byte);   

	SPI_HARDWARE_DIRVE_ON_FLASH_CS(); 	

	SPI_Flash_Wait_Busy_when_erase();

	SPI_SCLK_Speed(SPI_CLK_SPEED_1_5M);		

}  

void SPI_Flash_PowerDown(void)   
{ 
	UINT8 byte = 0;

	SPI_HARDWARE_DIRVE_OFF_FLASH_CS();

	SPI_ReadWriteOneByte(MX25L_PowerDown , &byte); 

	SPI_HARDWARE_DIRVE_ON_FLASH_CS();

//	TimerDelay10usec();
  Delayus(10);
}   

void SPI_Flash_WAKEUP(void)   
{  
	UINT8 byte = 0;

	SPI_HARDWARE_DIRVE_OFF_FLASH_CS();

	SPI_ReadWriteOneByte(MX25L_ReleasePowerDown , &byte); 

	SPI_HARDWARE_DIRVE_ON_FLASH_CS(); 

//	TimerDelay10usec();
  Delayus(10);
}  
#if 0
void SPI_FLASH_Test(void)
{
	UINT8 testDat[300];
	UINT16 ii,i;

	for (ii = 0;ii < 300;ii++)
	{
		testDat[ii] = ii;
		if(ii>=256)
		{
		 testDat[ii] = ii-256;
		}
	}
//		for (ii = 256;ii < 300;ii++)
//	{
//		testDat[ii] = ii-256;
//	}
//	SPI_Flash_Erase_Chip();

	GtechFlash_WriteBytes(0,0,testDat,300);
//	SPI_Flash_Write_Byte(testDat,0,300);

	for (ii = 0;ii < 300;ii++)
	{
		testDat[ii] = 0;
	}

	GtechFlash_ReadBytes(0,0,testDat,300);
//	SPI_Flash_Read(testDat,0,300);
	
	for(i=0;i<300;i++)
  {
 // xprintf("d=%x",testDat[i]);
	}
	
//	DrvUARTSend(testDat) ;
//	Tool_USB_ArrayData(testDat,15);

//	Tool_USB_ArrayData(testDat,15);
}
#endif 
#if 1
void SPI_FLASH_Test(void)
{
	UINT8 testDat[15],ii;

	for (ii = 0;ii < 15;ii++)
	{
		testDat[ii] = ii;
	}

	SPI_Flash_Erase_Chip();

//	GtechFlash_WriteBytes(0,0,testDat,15);
	
	SPI_Flash_Write_Byte(testDat,0,15);

	for (ii = 0;ii < 15;ii++)
	{
		testDat[ii] = 0;
	}
	
//  GtechFlash_ReadBytes(0,0,testDat,15);
	
	SPI_Flash_Read(testDat,0,15);

	Tool_USB_ArrayData(testDat,15);

	Tool_USB_ArrayData(testDat,15);

}
#endif

#endif /*#endif USE_INSIDE_EEPROM*/

