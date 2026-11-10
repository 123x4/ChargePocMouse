#ifndef _EPPROM_H_
#define  _EPPROM_H_

#include "gloab.h"
#include "spi.h"
#include "timer.h"
//#include "port.h"
#include "IAP_CMD.h"


#if USE_INSIDE_EEPROM

#else

#define		OPTICAL_HANDLE_WHEN_MACRO

#define MX25L_WriteEnable		0x06
#define MX25L_WriteDisable		0x04
#define MX25L_ReadStatusReg		0x05
#define MX25L_WriteStatusReg	0x01
#define MX25L_ReadData			0x03
#define MX25L_FastReadData 		0x0B  
//#define MX25L_FastReadDual		0x3B
#define MX25L_PageProgram		0x02
#define MX25L_BlockErase		0xD8
#define MX25L_SectorErase		0x20
#define MX25L_ChipErase			0xC7
#define MX25L_PowerDown			0xB9

#define MX25L_ReleasePowerDown	0xAB
#define MX25L_DeviceID			0xAB
#define MX25L_ManufactDeviceID	0x90
#define MX25L_RDID				0x9F
#define ZB25V_PID				0x5E13
#define MX25L_PID				0xA113

void FLASH_Communication_Check(void);
void SPI_FLASH_Test(void);
void SPI_Flash_WAKEUP(void);
void SPI_Flash_PowerDown(void);
void SPI_Flash_Erase_Chip(void);
UINT16 SPI_Flash_ReadID(void);
void SPI_Flash_Erase_Sector(UINT8 Sector_No);
void SPI_Flash_Write_Byte(UINT8* pBuffer,UINT32 WriteAddr,UINT16 NumByteToWrite);
void SPI_Flash_Read(UINT8* pBuffer,UINT32 ReadAddr,UINT16 NumByteToRead)   ;

#endif /*#endif USE_INSIDE_EEPROM*/

#endif



