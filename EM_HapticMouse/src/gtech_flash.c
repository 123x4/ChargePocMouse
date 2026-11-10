/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "../inc/LPC11Uxx.h"
#include "../inc/UART.h"
#include "gtech_flash.h"
#include "test.h"
#include "usb.h"
#include "OSC.h"
#include "eeprom.h"
#include "gpio.h"
#include "../inc/WWDT.h"
//#include "gloab.h"
#include "IAP_CMD.h"
#if 1
const UINT8 BootLoader_Flash[2]__attribute__((at(0X00003F00)))={0xAA,0xAA};				//for USB update .hex
//const UINT8 BootLoader_Flash1[2]__attribute__((at(0X0000FE00)))={0xAA,0xAA};			//.hex for merge .bin
UINT32 inside_Flash=0;

UINT8 flash_emptyData[H_Flash_Sector_Size];//H_Flash_Sector_Size

    //MCU Flash write and read action
typedef void (*IAP)(unsigned long [],unsigned long[]);
static const IAP iap_entry = (IAP) IAP_LOCATION;

const UINT8 MCU_inside_Flash[InsideFlaish_Size]__attribute__((at(0X0000F000))) ;
//UINT32 inside_Flash=0;
//UINT8 MCUflash_emptyData[NXP_Sector_Size];//NXP_Sector_Size
unsigned long IAP_STATUS = 0 ;
unsigned long IAP_Comman [5] = 
{
    0,0,0,0,0
} ;

unsigned long IAP_Result [4] = 
{
    0,0,0,0
} ;

unsigned long copyRAMtoFLASH(unsigned long Flash_dst ,  unsigned int* RAM_src , unsigned long copy_Size_of_Byte)
{
	IAP_Comman[0] = IAP_CMD_copyRAMtoFLASH ;                  
	IAP_Comman[1] = Flash_dst ;
	IAP_Comman[2] = ( unsigned int) RAM_src ;
	IAP_Comman[3] = copy_Size_of_Byte ;            
	IAP_Comman[4] = MCU_OSC/1000 ;
	
  iap_entry( IAP_Comman, IAP_Result) ;
	
	return IAP_Result[0];
}//OK

void Gtech_MCUFlash_ReadBytes(const UINT8 *flashPtr,UINT32 offAdderss,UINT8 *data,UINT32 count)
{
	UINT32 ii;

	for (ii = 0;ii < count;ii++)
	{
		data[ii] = *(flashPtr + offAdderss + ii);
	}
}

unsigned long prepareSector(unsigned char Start_sector_number, unsigned char End_sector_number)
{
	IAP_Comman[0] = IAP_CMD_prepareSector ;                  
	IAP_Comman[1] = Start_sector_number ;
	IAP_Comman[2] = End_sector_number ;
	IAP_Comman[3] = 0 ;            
	IAP_Comman[4] = 0 ;
	
  iap_entry( IAP_Comman, IAP_Result);
	
	return IAP_Result[0];
}//OK

unsigned long eraseSector(unsigned char Start_sector_number, unsigned char End_sector_number)
{
	IAP_Comman[0] = IAP_CMD_eraseSector ;                  
	IAP_Comman[1] = Start_sector_number ;
	IAP_Comman[2] = End_sector_number ;
	IAP_Comman[3] = MCU_OSC/1000 ;            
	IAP_Comman[4] = 0 ;
	
  iap_entry( IAP_Comman, IAP_Result) ;
	
	return IAP_Result[0];
}//OK

unsigned long erasePage(unsigned int Start_page_number, unsigned int End_page_number)
{
	IAP_Comman[0] = IAP_CMD_erasePage ;                  
	IAP_Comman[1] = Start_page_number ;
	IAP_Comman[2] = End_page_number ;
	IAP_Comman[3] = MCU_OSC/1000 ;            
	IAP_Comman[4] = 0 ;
	
  iap_entry( IAP_Comman, IAP_Result) ;
	
	return IAP_Result[0];
}//OK

void GtechFlash_ReadBlock(UINT32 blockIndex,UINT8 *data)
{
	UINT32 ii;
	UINT8 *base_adderss;

	base_adderss = (UINT8 *)(blockIndex*GTECH_BLOCK_SIZE);

	for (ii = 0;ii < GTECH_BLOCK_SIZE;ii++)
	{
		data[ii] = *(base_adderss + ii);
	}
}

void Gtech_MCUFlash_WriteBytes(const UINT8* flashPtr,UINT32 offSet_adder, UINT8 *data ,UINT32 sum_byte)
{
	UINT32 block_index;	//In which block began to write
	UINT32 startIndex_InBlock;//In the address of the block which began write 
	UINT32 free_inBlock;	//The current block can write space
	UINT32 written_index = 0;	//How many bytes to write
	UINT32 byte_count;	//How many byte will write in current block
	UINT32 remain_byte; //How many byte hadn't write
	UINT32 ii;
	UINT8 * writeAddress;
	UINT32 writeAdr;
	UINT32 sector;
    NVIC_DisableIRQ( GINT0_IRQn );
	NVIC_DisableIRQ( TIMER_16_0_IRQn );
	NVIC_DisableIRQ( TIMER_16_1_IRQn );
	NVIC_DisableIRQ(I2C_IRQn);
	NVIC_DisableIRQ(SSP0_IRQn);
	NVIC_DisableIRQ(SSP1_IRQn);
  NVIC_DisableIRQ(USB_IRQn);
	remain_byte = sum_byte;

	writeAddress = (UINT8 *)((flashPtr) + offSet_adder);
	
	block_index = (UINT32)writeAddress/GTECH_BLOCK_SIZE;

	startIndex_InBlock = (UINT32)writeAddress%GTECH_BLOCK_SIZE;

	free_inBlock = GTECH_BLOCK_SIZE - startIndex_InBlock;


	while (written_index < sum_byte)
	{
		byte_count = (remain_byte > free_inBlock ? free_inBlock :  remain_byte);

		GtechFlash_ReadBlock(block_index,flash_emptyData);

		for (ii = 0;ii < byte_count;ii++)
		{
			flash_emptyData[startIndex_InBlock+ii] = data[written_index];
			written_index++;
		}

		//Flash_WriteBlock(block_index,flash_emptyData);//!!!!

		sector = block_index / NXP_SectorBlock_Num;

		//-----Flash erase
//		IAP_STATUS = prepareSector(sector , sector) ;			//Prepare erase
		IAP_STATUS = prepareSector(sector , sector) ;			//Prepare erase
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;							//Wait prepare success flag
		IAP_STATUS = erasePage(block_index, block_index) ;				//Flash sector erase
//		TimerDelayMsec(1);
//		IAP_STATUS = eraseSector(sector , sector) ;	
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;							//Wait erase success flag
		//-----Read Flash data after earse

		//-----Flash write
//		IAP_STATUS = prepareSector(sector , sector) ;			//Prepare write
		IAP_STATUS = prepareSector(sector , sector) ;			//Prepare write
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;			 //Wait prepare success flag 

		writeAdr = block_index * GTECH_BLOCK_SIZE;

		IAP_STATUS = copyRAMtoFLASH(writeAdr , (unsigned int*)flash_emptyData , GTECH_BLOCK_SIZE) ;	//Write data(256byte) to flash
		
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;		

		block_index++;
		remain_byte = sum_byte - written_index;

		///written the first block,so remain data can write in next block 
		/// next block can write in first byte in block.
		free_inBlock = GTECH_BLOCK_SIZE;

		///will write next block so clear.
		startIndex_InBlock = 0;
	}
    NVIC_EnableIRQ(GINT0_IRQn);
	NVIC_EnableIRQ(USB_IRQn);
    NVIC_EnableIRQ(TIMER_16_1_IRQn);
	NVIC_EnableIRQ(TIMER_16_0_IRQn);
	NVIC_EnableIRQ(I2C_IRQn);
	NVIC_EnableIRQ(SSP0_IRQn);
	NVIC_EnableIRQ(SSP1_IRQn);
}

void OperateFlash_Write_value(UINT16 address,UINT8 *buffer,UINT16 len)
{
	UINT8 *writeAddress;

	writeAddress = (UINT8 *)(MCU_inside_Flash + address);

	if (address+len > InsideFlaish_Size)
	{
		return;
	}

	Gtech_MCUFlash_WriteBytes(writeAddress,0,buffer,len);	
}

void OperateFlash_Read_value(UINT16 address,UINT8 *buffer,UINT16 len)
{
	UINT8 *writeAddress;

	writeAddress = (UINT8 *)(MCU_inside_Flash + address);

	Gtech_MCUFlash_ReadBytes(writeAddress,0,buffer,len);	
}

/* Reinvoke ISP */
UINT8 Chip_IAP_ReinvokeISP(void)
{
	wwdt_close();
	NVIC_DisableIRQ( TIMER_16_1_IRQn );
	NVIC_DisableIRQ(I2C_IRQn);
	NVIC_DisableIRQ(SSP0_IRQn);
	NVIC_DisableIRQ(SSP1_IRQn);
  NVIC_DisableIRQ(USB_IRQn);
	NVIC_DisableIRQ(UART_IRQn);
	NVIC_DisableIRQ(GINT0_IRQn);
	NVIC_DisableIRQ(GINT1_IRQn);
	NVIC_DisableIRQ(WDT_IRQn);
	NVIC_DisableIRQ(USBWakeup_IRQn);
	NVIC_DisableIRQ(SysTick_IRQn);
//	uint32_t command[5], result[5];
// InitSystemOSC( 0 , PSEL , MSEL , SYS_AHB_DIV_VAL ) ;
//	command[0] = IAP_REINVOKE_ISP_CMD;
	IAP_Comman[0] = IAP_REINVOKE_ISP_CMD;
//	iap_entry(command, result);
  iap_entry( IAP_Comman, IAP_Result) ;
	
	NVIC_EnableIRQ( TIMER_16_1_IRQn );
	NVIC_EnableIRQ(I2C_IRQn);
	NVIC_EnableIRQ(SSP0_IRQn);
	NVIC_EnableIRQ(SSP1_IRQn);
  NVIC_EnableIRQ(USB_IRQn);
	NVIC_EnableIRQ(UART_IRQn);
	NVIC_EnableIRQ(GINT0_IRQn);
	NVIC_EnableIRQ(GINT1_IRQn);
	NVIC_EnableIRQ(WDT_IRQn);
	NVIC_EnableIRQ(USBWakeup_IRQn);
	NVIC_EnableIRQ(SysTick_IRQn);
	return IAP_Result[0];
}

void Write_MCUFLASH_Flag(void)
{
  uint8_t buffer[2]={0xAA,0xAA};
	OperateFlash_Write_value(0,buffer,2);
}

void write_crp_lv(UINT8* buffer)
{
	UINT8 crp_data[4] = {0xff, 0xff, 0xff, 0xff};
	
	Gtech_MCUFlash_WriteBytes(0, 0x2fc, crp_data, 4);	
	
	buffer[1] = 0x01;
	
	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer,TRANSFER_LENGTH);
}


void GtechFlash_ReadBytes( UINT32 flashPtr, UINT32 offAdderss,UINT8 *data, UINT16 count)
{
//	UINT32 ii;
  UINT32 ReadAddr=0;
	ReadAddr = flashPtr + offAdderss;
	
#if USE_INSIDE_EEPROM
	Chip_EEPROM_Read(ReadAddr, data, count);
#else
	SPI_Flash_Read(data, ReadAddr, count); 
#endif
}

void GtechFlash_WriteBytes( UINT32 flashPtr,UINT32 offSet_adder, UINT8 *data ,UINT16 sum_byte)
{
	
#if USE_INSIDE_EEPROM
	Chip_EEPROM_Write(flashPtr + offSet_adder, data, sum_byte);
#else
	UINT32 sector_index;	//In which block began to write
	UINT32 startIndex_sector;//In the address of the block which began write 
	UINT32 free_inBlock;	//The current block can write space
	UINT32 written_index = 0;	//How many bytes to write
	UINT32 byte_count;	//How many byte will write in current block
	UINT32 remain_byte; //How many byte hadn't write
	UINT32 ii;
	UINT32 writeAddress;
//UINT8 * writeAddress;
	UINT32 writeAdr;
	UINT32 sector;
//  UINT8 flash_emptyData[H_Flash_Sector_Size];
	remain_byte = sum_byte;

	writeAddress = flashPtr + offSet_adder;
	
//	writeAddress = (UINT8 *)((flashPtr) + offSet_adder);
	
	sector_index = (UINT32)writeAddress/H_Flash_Sector_Size;

	startIndex_sector = (UINT32)writeAddress%H_Flash_Sector_Size;

	free_inBlock = H_Flash_Sector_Size - startIndex_sector;


	while (written_index < sum_byte)
	{
		wwdt_feed();
		byte_count = (remain_byte > free_inBlock ? free_inBlock :  remain_byte);
		
    GtechFlash_ReadBytes(sector_index*H_Flash_Sector_Size,0,flash_emptyData,H_Flash_Sector_Size);
		
		for (ii = 0;ii < byte_count;ii++)
		{
			flash_emptyData[startIndex_sector+ii] = data[written_index];
			written_index++;
		}

		//Flash_WriteBlock(block_index,flash_emptyData);//!!!!
		sector = sector_index; // / NXP_SectorBlock_Num;

		SPI_Flash_Erase_Sector(sector);
		
		writeAdr= sector_index*H_Flash_Sector_Size;
		
		SPI_Flash_Write_Byte(flash_emptyData, (UINT32)writeAdr, H_Flash_Sector_Size);
		
		#if 0
		//-----Flash erase
//		IAP_STATUS = prepareSector(sector , sector) ;			//Prepare erase
		IAP_STATUS = prepareSector(sector , sector) ;			//Prepare erase
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;							//Wait prepare success flag
		IAP_STATUS = erasePage(block_index, block_index) ;				//Flash sector erase
//		TimerDelayMsec(1);
//		IAP_STATUS = eraseSector(sector , sector) ;	
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;							//Wait erase success flag
		//-----Read Flash data after earse

		//-----Flash write
//		IAP_STATUS = prepareSector(sector , sector) ;			//Prepare write
		IAP_STATUS = prepareSector(sector , sector) ;			//Prepare write
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;			 //Wait prepare success flag 

		writeAdr = block_index * GTECH_BLOCK_SIZE;

		IAP_STATUS = copyRAMtoFLASH(writeAdr , (unsigned int*)flash_emptyData , GTECH_BLOCK_SIZE) ;	//Write data(256byte) to flash
		
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;		
    #endif
		
		sector_index++;
		
		remain_byte = sum_byte - written_index;
		///written the first block,so remain data can write in next block 
		/// next block can write in first byte in block.
		free_inBlock = H_Flash_Sector_Size;

		///will write next block so clear.
		startIndex_sector = 0;
			
	}
#endif
}


#endif

#if 0
///25k
const UINT8 BootLoader_Flash[2]__attribute__((at(0X00007F00)))={0xAA,0xAA};

const UINT8 inside_Flash[InsideFlaish_Size]__attribute__((at(0X0000A000))); //CY_SECTION(".cy_checksum_exclude") CY_ALIGN(CY_FLASH_SIZEOF_ROW)= {0}; 
////UINT8 
UINT8 flash_emptyData[GTECH_BLOCK_SIZE];
extern BOOL SaveDPIflag;
unsigned long IAP_STATUS = 0 ;
typedef void (*IAP)(unsigned long [],unsigned long[]);
static const IAP iap_entry = (IAP) IAP_LOCATION;

//UINT32 inside_Flash=0;
//UINT8 flash_emptyData[H_Flash_Sector_Size];//H_Flash_Sector_Size


unsigned long IAP_Comman [5] = 
{
    0,0,0,0,0
} ;

unsigned long IAP_Result [4] = 
{
    0,0,0,0
} ;

unsigned long copyRAMtoFLASH(unsigned long Flash_dst ,  unsigned int* RAM_src , unsigned long copy_Size_of_Byte)
{
	IAP_Comman[0] = IAP_CMD_copyRAMtoFLASH ;                  
	IAP_Comman[1] = Flash_dst ;
	IAP_Comman[2] = ( unsigned int) RAM_src ;
	IAP_Comman[3] = copy_Size_of_Byte ;            
	IAP_Comman[4] = MCU_OSC/1000 ;
	
  iap_entry( IAP_Comman, IAP_Result) ;
	
	return IAP_Result[0];
}//OK

void GtechFlash_ReadBytes(const UINT8 *flashPtr,UINT32 offAdderss,UINT8 *data,UINT32 count)
{
	UINT32 ii;

	for (ii = 0;ii < count;ii++)
	{
		data[ii] = *(flashPtr + offAdderss + ii);
	}
}

unsigned long prepareSector(unsigned char Start_sector_number, unsigned char End_sector_number)
{
	IAP_Comman[0] = IAP_CMD_prepareSector ;                  
	IAP_Comman[1] = Start_sector_number ;
	IAP_Comman[2] = End_sector_number ;
	IAP_Comman[3] = 0 ;            
	IAP_Comman[4] = 0 ;
	
  iap_entry( IAP_Comman, IAP_Result);
	
	return IAP_Result[0];
}//OK

unsigned long eraseSector(unsigned char Start_sector_number, unsigned char End_sector_number)
{
	IAP_Comman[0] = IAP_CMD_eraseSector ;                  
	IAP_Comman[1] = Start_sector_number ;
	IAP_Comman[2] = End_sector_number ;
	IAP_Comman[3] = MCU_OSC/1000 ;            
	IAP_Comman[4] = 0 ;
	
  iap_entry( IAP_Comman, IAP_Result) ;
	
	return IAP_Result[0];
}//OK

unsigned long erasePage(unsigned int Start_page_number, unsigned int End_page_number)
{
	IAP_Comman[0] = IAP_CMD_erasePage ;                  
	IAP_Comman[1] = Start_page_number ;
	IAP_Comman[2] = End_page_number ;
	IAP_Comman[3] = MCU_OSC/1000 ;            
	IAP_Comman[4] = 0 ;
	
  iap_entry( IAP_Comman, IAP_Result) ;
	
	return IAP_Result[0];
}//OK

void GtechFlash_ReadBlock(UINT32 blockIndex,UINT8 *data)
{
	UINT32 ii;
	UINT8 *base_adderss;

	base_adderss = (UINT8 *)(blockIndex*GTECH_BLOCK_SIZE);

	for (ii = 0;ii < GTECH_BLOCK_SIZE;ii++)
	{
		data[ii] = *(base_adderss + ii);
	}
}

void GtechFlash_WriteBytes(const UINT8* flashPtr,UINT32 offSet_adder, UINT8 *data ,UINT32 sum_byte)
{
	UINT32 block_index;	//In which block began to write
	UINT32 startIndex_InBlock;//In the address of the block which began write 
	UINT32 free_inBlock;	//The current block can write space
	UINT32 written_index = 0;	//How many bytes to write
	UINT32 byte_count;	//How many byte will write in current block
	UINT32 remain_byte; //How many byte hadn't write
	UINT32 ii;
	UINT8 * writeAddress;
	UINT32 writeAdr;
	UINT32 sector;
	NVIC_DisableIRQ( TIMER_16_1_IRQn );
	NVIC_DisableIRQ(I2C_IRQn);
	NVIC_DisableIRQ(SSP0_IRQn);
	NVIC_DisableIRQ(SSP1_IRQn);
  NVIC_DisableIRQ(USB_IRQn);
//	if(SaveDPIflag)
//	{
//	 sum_byte=1;
//	}
	remain_byte = sum_byte;

	writeAddress = (UINT8 *)((flashPtr) + offSet_adder);
	
	block_index = (UINT32)writeAddress/GTECH_BLOCK_SIZE;

	startIndex_InBlock = (UINT32)writeAddress%GTECH_BLOCK_SIZE;

	free_inBlock = GTECH_BLOCK_SIZE - startIndex_InBlock;


	while (written_index < sum_byte)
	{
		byte_count = (remain_byte > free_inBlock ? free_inBlock :  remain_byte);

		GtechFlash_ReadBlock(block_index,flash_emptyData);

		for (ii = 0;ii < byte_count;ii++)
		{
			flash_emptyData[startIndex_InBlock+ii] = data[written_index];
			written_index++;
		}

		//Flash_WriteBlock(block_index,flash_emptyData);//!!!!
    
		sector = block_index / NXP_SectorBlock_Num;
//     DrvGPIOOutput(0,23,0); 
		//-----Flash erase
//		IAP_STATUS = prepareSector(sector , sector) ;			//Prepare erase
		IAP_STATUS = prepareSector(sector , sector) ;			//Prepare erase
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;							//Wait prepare success flag
		
		IAP_STATUS = erasePage(block_index, block_index) ;				//Flash sector erase
//		TimerDelayMsec(1);
//		IAP_STATUS = eraseSector(sector , sector) ;	
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;							//Wait erase success flag
		//-----Read Flash data after earse
		
		//-----Flash write
//	IAP_STATUS = prepareSector(sector , sector) ;			//Prepare write
		IAP_STATUS = prepareSector(sector , sector) ;			//Prepare write
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;			 //Wait prepare success flag 

		writeAdr = block_index * GTECH_BLOCK_SIZE;

		IAP_STATUS = copyRAMtoFLASH(writeAdr , (unsigned int*)flash_emptyData , GTECH_BLOCK_SIZE) ;	//Write data(256byte) to flash
		
		while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;		

		block_index++;
		remain_byte = sum_byte - written_index;

		///written the first block,so remain data can write in next block 
		/// next block can write in first byte in block.
		free_inBlock = GTECH_BLOCK_SIZE;

		///will write next block so clear.
		startIndex_InBlock = 0;
//		DrvGPIOOutput(0,23,1); 
	}
	NVIC_EnableIRQ(USB_IRQn);
  NVIC_EnableIRQ(TIMER_16_1_IRQn);
	NVIC_EnableIRQ(I2C_IRQn);
	NVIC_EnableIRQ(SSP0_IRQn);
	NVIC_EnableIRQ(SSP1_IRQn);
}

/* Reinvoke ISP */
uint8_t Chip_IAP_ReinvokeISP(void)
{
	wwdt_close();
	NVIC_DisableIRQ( TIMER_16_1_IRQn );
	NVIC_DisableIRQ(I2C_IRQn);
	NVIC_DisableIRQ(SSP0_IRQn);
	NVIC_DisableIRQ(SSP1_IRQn);
  NVIC_DisableIRQ(USB_IRQn);
	NVIC_DisableIRQ(UART_IRQn);
	NVIC_DisableIRQ(GINT0_IRQn);
	NVIC_DisableIRQ(GINT1_IRQn);
	NVIC_DisableIRQ(WDT_IRQn);
	NVIC_DisableIRQ(USBWakeup_IRQn);
	NVIC_DisableIRQ(SysTick_IRQn);
//	uint32_t command[5], result[5];
// InitSystemOSC( 0 , PSEL , MSEL , SYS_AHB_DIV_VAL ) ;
//	command[0] = IAP_REINVOKE_ISP_CMD;
	IAP_Comman[0] = IAP_REINVOKE_ISP_CMD;
//	iap_entry(command, result);
  iap_entry( IAP_Comman, IAP_Result) ;
	return IAP_Result[0];
}
#endif

#if 0
UINT8 test_write_data[8] = 
{
//			0x03020100,0x07060504,0xB0A09080,0x13121110,0x17161514,0x1B1A1918,0x1F1E1D1C,0x23222120
			0x8,0x9,10,11,12,13,14,15,
//			16,17,18,19,20,21,22,23,
//			24,25,26,27,28,29,30,31,
} ;

unsigned char Flash_test_to_UART[32];
//unsigned char Flash_test_to_UART[300] =
//{
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//	
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//	
//	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//} ;

void Flash_Check(void)
{
	unsigned int i;
//	UINT8 testData[300] = {
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
//		
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,21,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,22,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,23,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,24,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,25,

//  1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,26,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,27,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,28,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,29,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,30,
//  };
	
//  1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
//	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,	
		
		
		
		
		
		
		
		
		
		
/*
	for(i=0 ; i<8 ; i++)																		//read 0x0000A000 32 Byte
	{
		Flash_test_to_UART[4*i] = (LPC_FLASH_USE->SECTOR10[i] & 0xff) ;
		Flash_test_to_UART[4*i+1] = ((LPC_FLASH_USE->SECTOR10[i] >> 8) & 0xff) ;
		Flash_test_to_UART[4*i+2] = ((LPC_FLASH_USE->SECTOR10[i] >> 16)& 0xff) ;
		Flash_test_to_UART[4*i+3] = ((LPC_FLASH_USE->SECTOR10[i] >> 24) & 0xff) ;
	}
	InitUART(115200) ;
	DrvUARTSendData(Flash_test_to_UART, 32) ;
	

	//-----Flash earse
	IAP_STATUS = prepareSector(SECTOR_10 , SECTOR_10) ;			//Prepare erase
	while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;							//Wait prepare success flag
	IAP_STATUS = eraseSector(SECTOR_10 , SECTOR_10) ;				//Flash sector erase
	while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;							//Wait erase success flag
	//-----Read Flash data after earse
	for(i=0 ; i<8 ; i++)																		//read 0x0000A000 32 Byte
	{
		Flash_test_to_UART[4*i] = (LPC_FLASH_USE->SECTOR10[i] & 0xff) ;
		Flash_test_to_UART[4*i+1] = ((LPC_FLASH_USE->SECTOR10[i] >> 8) & 0xff) ;
		Flash_test_to_UART[4*i+2] = ((LPC_FLASH_USE->SECTOR10[i] >> 16)& 0xff) ;
		Flash_test_to_UART[4*i+3] = ((LPC_FLASH_USE->SECTOR10[i] >> 24) & 0xff) ;
	}
	InitUART(115200) ;
	DrvUARTSendData(Flash_test_to_UART, 32) ;
	
	//-----Flash write
	IAP_STATUS = prepareSector(SECTOR_10 , SECTOR_10) ;			//Prepare write
	while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;							//Wait prepare success flag 
	IAP_STATUS = copyRAMtoFLASH(SECTOR_10_START , (unsigned int*)testData , 256) ;	//Write data(256byte) to flash
	while(IAP_STATUS != IAP_STA_CMD_SUCCESS) ;							//Wait write success flag
	//-----Read Flash data after write
	for(i=0 ; i<8 ; i++)																		//read 0x0000A000 32 Byte
	{
		Flash_test_to_UART[4*i] = (LPC_FLASH_USE->SECTOR10[i] & 0xff) ;
		Flash_test_to_UART[4*i+1] = ((LPC_FLASH_USE->SECTOR10[i] >> 8) & 0xff) ;
		Flash_test_to_UART[4*i+2] = ((LPC_FLASH_USE->SECTOR10[i] >> 16)& 0xff) ;
		Flash_test_to_UART[4*i+3] = ((LPC_FLASH_USE->SECTOR10[i] >> 24) & 0xff) ;
	}
	InitUART(115200) ;
	DrvUARTSendData(Flash_test_to_UART, 32) ;
*/	

//	GtechFlash_WriteBytes(inside_Flash,0,testData,300);
//	

//	
//		for(i=0 ; i<80 ; i++)																		//read 0x0000A000 32 Byte
//	{
//		Flash_test_to_UART[4*i] =   (LPC_FLASH_USE->SECTOR10[i] & 0xff) ;
//		Flash_test_to_UART[4*i+1] = ((LPC_FLASH_USE->SECTOR10[i] >> 8) & 0xff) ;
//		Flash_test_to_UART[4*i+2] = ((LPC_FLASH_USE->SECTOR10[i] >> 16)& 0xff) ;
//		Flash_test_to_UART[4*i+3] = ((LPC_FLASH_USE->SECTOR10[i] >> 24) & 0xff) ;
//		
//	}
//	
//	DrvUARTSendData(Flash_test_to_UART, 300) ;
//	

		GtechFlash_WriteBytes(inside_Flash,0,test_write_data,32);
//	
	  for(i=0 ; i<8 ; i++)																		//read 0x0000A000 32 Byte
	{
		Flash_test_to_UART[4*i] =   (LPC_FLASH_USE->SECTOR16[i] & 0xff) ;
		Flash_test_to_UART[4*i+1] = ((LPC_FLASH_USE->SECTOR16[i] >> 8) & 0xff) ;
		Flash_test_to_UART[4*i+2] = ((LPC_FLASH_USE->SECTOR16[i] >> 16)& 0xff) ;
		Flash_test_to_UART[4*i+3] = ((LPC_FLASH_USE->SECTOR16[i] >> 24) & 0xff) ;
		
	}
	
	DrvUARTSendData(Flash_test_to_UART, 32) ;

//	Tool_Clear_ArrayData(testData,0,9);

//	GtechFlash_ReadBytes(inside_Flash,0,testData,10);

}
#endif
