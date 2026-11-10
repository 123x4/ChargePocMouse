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
#ifndef GTECH_FLASH_H		
#define GTECH_FLASH_H

#include "gloab.h"    
//#include "project.h"
#if 1
extern UINT32 inside_Flash;
#define  H_Flash_Sector_Size 4096//4096
#define  Temp_Store_Size 2048
#define  Last_Sector 127
//#endif
//extern UINT32 inside_Flash;
//#define  H_Flash_Sector_Size 4096
//#define  Temp_Store_Size 2048
//#define  Last_Sector 127
//#if 1
#define InsideFlaish_Size	256//15360//30720//30K
extern const UINT8 MCU_inside_Flash[InsideFlaish_Size]__attribute__((at(0X0000F000))) ;

//#define  GTECH_BLOCK_SIZE  0x00000100u // CY_FLASH_SIZEOF_ROW  
#define  GTECH_BLOCK_SIZE 256 
#define  NXP_Sector_Size 4096
#define  NXP_SectorBlock_Num  (NXP_Sector_Size/GTECH_BLOCK_SIZE)
#endif

#if 0
#define InsideFlaish_Size	 15360//30720//30K
extern const UINT8 inside_Flash[InsideFlaish_Size]__attribute__((at(0X0000A000))) ;

//#define  GTECH_BLOCK_SIZE  0x00000100u // CY_FLASH_SIZEOF_ROW  
#define  GTECH_BLOCK_SIZE 256 
#define  NXP_Sector_Size 4096
#define  NXP_SectorBlock_Num  (NXP_Sector_Size/GTECH_BLOCK_SIZE)
#endif

#if 1 //ndef __Flash_H__
//#define __Flash_H__
//===========================================================================
//                      				Declare                          	 		 	  
//===========================================================================
 unsigned long readUID(unsigned long * UID) ;
 unsigned long prepareSector(unsigned char Start_sector_number, unsigned char End_sector_number) ;
 unsigned long eraseSector(unsigned char Start_sector_number, unsigned char End_sector_number) ;
 unsigned long erasePage(unsigned int Start_page_number, unsigned int End_page_number) ;
 unsigned long copyRAMtoFLASH(unsigned long Flash_dst , unsigned int* RAM_src , unsigned long copy_Size_of_Byte) ;
 unsigned long writeEEPROM(unsigned long EEPROM_dst , unsigned int* RAM_src , unsigned long copy_Size_of_Byte) ;
 unsigned long readEEPROM(unsigned long EEPROM_dst , unsigned int* RAM_src , unsigned long copy_Size_of_Byte) ;
 void testFlashControl(void) ;
 uint8_t Chip_IAP_ReinvokeISP(void);
//===========================================================================
//                      				Parameter                          	 		 	  
//===========================================================================
//IAP Command
#define IAP_CMD_prepareSector   50                                  //Select Sector
#define IAP_CMD_copyRAMtoFLASH  51                                  //Copy Data From Ram to Flash
#define IAP_CMD_eraseSector     52                                  //Erase Sector
#define IAP_CMD_readUID					58									//Read UID
#define IAP_CMD_erasePage				59									//Erase Page
#define IAP_CMD_writeEEPROM			61									
#define IAP_CMD_eraseEEPROM			62									
#define IAP_REINVOKE_ISP_CMD    57

//IAP Status Codes
#define IAP_STA_CMD_SUCCESS 					0
#define IAP_STA_INVALID_COMMAND 			1
#define IAP_STA_SRC_ADDR_ERROR 				2
#define IAP_STA_DST_ADDR_ERROR 				3
#define IAP_STA_SRC_ADDR_NOT_MAPPED 	4
#define IAP_STA_DST_ADDR_NOT_MAPPED 	5
#define IAP_STA_COUNT_ERROR 					6
#define IAP_STA_INVALID_SECTOR 				7
#define IAP_STA_SECTOR_NOT_BLANK			8
#define IAP_STA_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION 	9
#define IAP_STA_COMPARE_ERROR 				10
#define IAP_STA_BUSY 									11

#define IAP_LOCATION 0x1FFF1FF1

//every sector is 4K Byte
#define SECTOR_0				0
#define SECTOR_1				1
#define SECTOR_2        2
#define SECTOR_3        3
#define SECTOR_4        4
#define SECTOR_5        5
#define SECTOR_6        6
#define SECTOR_7        7
#define SECTOR_8        8
#define SECTOR_9        9
#define SECTOR_10				10
#define SECTOR_11				11	//LPC11U34/4xx only 48 K Byte
#define SECTOR_12				12
#define SECTOR_13				13
#define SECTOR_14				14
#define SECTOR_15				15

//every page is 256 Byte

//every sector address
#define SECTOR_0_START		0x00000000
#define SECTOR_1_START		0x00001000
#define SECTOR_2_START		0x00002000
#define SECTOR_3_START		0x00003000
#define SECTOR_4_START		0x00004000
#define SECTOR_5_START		0x00005000
#define SECTOR_6_START		0x00006000
#define SECTOR_7_START		0x00007000
#define SECTOR_8_START		0x00008000
#define SECTOR_9_START		0x00009000
#define SECTOR_10_START		0x0000A000
#define SECTOR_11_START		0x0000B000	//LPC11U34/4xx only 48 K Byte
#define SECTOR_12_START		0x0000C000
#define SECTOR_13_START		0x0000D000
#define SECTOR_14_START		0x0000E000
#define SECTOR_15_START		0x0000F000

//#define NXP_Flash   (const UINT8*)0x0000A000
	
	
extern unsigned long UID_buffer ;


//===========================================================================
//                      				End of File                          	 		 	  
//===========================================================================
#endif


#define  Flash_WriteBlock(black_index,array_data) CySysFlashWriteRow(black_index, array_data)  

    
#define Flash_Write(x,y);

#define Flash_Read(x,y);
#if 1
void GtechFlash_ReadBytes( UINT32 flashPtr, UINT32 offAdderss,UINT8 *data,UINT16 count);
void GtechFlash_WriteBytes( UINT32 flashPtr, UINT32 offSet_adder, UINT8 *data ,UINT16 sum_byte);

void Gtech_MCUFlash_ReadBytes(const UINT8 *flashPtr,UINT32 offAdderss,UINT8 *data,UINT32 count);
void Gtech_MCUFlash_WriteBytes(const UINT8* flashPtr,UINT32 offSet_adder, UINT8 *data ,UINT32 sum_byte);
void OperateFlash_Write_value(UINT16 address,UINT8 *buffer,UINT16 len);
void OperateFlash_Read_value(UINT16 address,UINT8 *buffer,UINT16 len);
#endif

#if 0
void GtechFlash_ReadBytes(const UINT8 *flashPtr,UINT32 offAdderss,UINT8 *data,UINT32 count);
void GtechFlash_WriteBytes(const UINT8* flashPtr,UINT32 offSet_adder, UINT8 *data ,UINT32 sum_byte);
#endif
void Flash_Check(void);
UINT8 Chip_IAP_ReinvokeISP(void);
void Write_MCUFLASH_Flag(void);
void write_crp_lv(UINT8* buffer);
    
#endif /* #ifndef GLOAB_H */
