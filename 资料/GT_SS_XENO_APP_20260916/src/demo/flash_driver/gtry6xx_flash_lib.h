/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

#ifndef __GTRY6xx_FLASH_LIB_H__
#define __GTRY6xx_FLASH_LIB_H__


extern unsigned int flash_signature[3];
#define FLASH_SIGNATURE_0  0x5a5a5a5a
#define FLASH_SIGNATURE_1  0xa5a5a5a5
#define FLASH_SIGNATURE_2  0x5555aaaa

#define FLASH_DATA_TOTALSIZE	0x100000//1MB
#define FLASH_DATA_BLOCKSIZE	0x10000//64KB one block size
#define FLASH_DATA_HBLOCKSIZE	0x8000//32KB one half block size
#define FLASH_DATA_SECTORSIZE	0x1000//4KB one sector size
#define FLASH_DATA_PAGESIZE		0x200//512B one page size

#define FLASH_ADDRESS			0x80000000//flash address in chip memory
#define FLASH_BOOT_DATA_SPACE	0x50000//192KB+128KB//FLASH_BOOT_SPACE+FLASH_ROMRUN_SPACE
#define FLASH_USR_APP_ADDR		FLASH_BOOT_DATA_SPACE//other use to application

#define FLASH_USE_DATA_ADDR		0x80000//512KB~1MB use to application

typedef enum{
	FREQ_DIV2 = 0,	//= sysclk/2
	FREQ_DIV4,		//= sysclk/4
	FREQ_DIV6,		//= sysclk/6
	FREQ_DIV8,		//= sysclk/8
	FREQ_DIV10,		//= sysclk/10
	FREQ_DIVMAX
}freq_div_t;

typedef enum{
	BLOCK_ERASE = 0,	//64KB
	HBLOCK_ERASE,		//32KB
	SECTOR_ERASE,		//4KB
	PAGE_ERASE,			//512B
}erase_type_t;
extern unsigned int gtry_flash_ctrl_inital(unsigned int sysclk, freq_div_t freq_div);
extern unsigned int gtry_flash_normal_erase(unsigned int FlashAddr, erase_type_t EraseType);
extern unsigned int gtry_flash_normal_write(unsigned int FlashAddr, unsigned char *WriteBuf, unsigned int WriteSize);
extern unsigned int gtry_flash_normal_read(unsigned int FlashAddr, unsigned char *ReadBuf, unsigned int ReadSize);
extern unsigned int gtry_flash_uicr_erase(void);
extern unsigned int gtry_flash_uicr_write(unsigned int Offset, unsigned char *WriteBuf, unsigned int ReadSize);
extern unsigned int gtry_flash_uicr_read(unsigned int Offset, unsigned char *ReadBuf, unsigned int ReadSize);


#endif /* __GTRY6xx_FLASH_LIB_H__ */

