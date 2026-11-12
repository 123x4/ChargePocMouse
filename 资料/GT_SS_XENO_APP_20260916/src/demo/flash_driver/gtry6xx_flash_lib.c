/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

#include "gtry_type.h"
#include "gtry6xx_flash_lib.h"

//#define FAST_W_R_OPER_ENBALE

volatile unsigned int core_sysclk = 0;//SMU_SYSCLK_200MHZ

#define CSR0_FLASH_BUSY_MASK			(0x1ul<<0)
#define CSR0_EP_EN_STS_MASK				(0x1ul<<1)
#define CSR0_BLK_PROT_MASK				(0x1Ful<<2)

#define CSR1_W_R_SPEED_MASK				(0x1ul<<1)
#define CSR1_EP_FAIL_MASK				(0x1ul<<2)
#define CSR1_PROT_CMPEN_MASK			(0x1ul<<6)

#define CSR2_PAGE512_EN_MASK			(0x1ul<<3)

//num = 1, then delay 10us under 200M
#define OPER_DELAY_10US(num)			do{for(int n = 0; n < (num<<(7-core_sysclk)); n++);}while(0)

#define FLASH_ERASE_CTRL(cmd, addr)		((cmd & 0xff)| \
										(((addr >> 16) & 0xff) << 8)| \
										(((addr >> 8) & 0xff) << 16)| \
										((addr & 0xff) << 24))

//#define GET_REG32(addr)					(*(volatile unsigned int *)(addr))
//#define SET_REG32(addr, value)			(*(volatile unsigned int *)(addr) = value)

#if 0
static __attribute__ ((section (".ram_ilm"))) unsigned int oper_waiting_end (void)
{
	unsigned int i/*, j*/;
	for (i = 1; i < 2000; i++)//max timeout = 20ms
	{
	  //for(j = 0; j < 150; j++);		//delay 10us under 200M
	  OPER_DELAY_10US(1);
	  if ((GET_REG32(0xF0B00034) & 0x1ul) == 0)
	  {
		  return 0;
	  }
	}
	return 1;
}
#else
#define oper_waiting_end()  ({                          \
    unsigned int ret = 1;                               \
    for (unsigned int x = 1; x < 2000; x++) {           \
        OPER_DELAY_10US(1);                             \
        if ((GET_REG32(0xF0B00034) & 0x1uL) == 0) {     \
            ret = 0;                                    \
            break;                                      \
        }                                               \
    }                                                   \
    ret;                                                \
})
#endif
#define CFG_OPER_FREQ_DIV(freq_div)		({ SET_REG32(0xF0B00040, freq_div); \
											OPER_DELAY_10US(1);})
#define CFG_OPER_EN_FSPEED()			({ SET_REG32(0xF0B00050, 0x5); \
											OPER_DELAY_10US(1);})

#define INIT_CFG_OPER()					({ SET_REG32(0xF0B00030, (GET_REG32(0xF0B00030)&(~0x18))|0x6); \
											SET_REG32(0xF0B00038, 0x0);})

#define INIT_DMA_OPER()					({ SET_REG32(0xF1400020, 0x1); \
											SET_REG32(0xF1400030, 0xffffffff); \
											SET_REG32(0xF1400040, 0x004a200e); \
											SET_REG32(0xF1400058, 0x0); \
											SET_REG32(0xF140005C, 0x0); \
											SET_REG32(0xF1400044, 0x0); \
											SET_REG32(0xF1400048, 0x00); \
											SET_REG32(0xF1400050, 0xF0B0002C); \
											SET_REG32(0xF1400060, 0x004a810e); \
											SET_REG32(0xF1400078, 0x0); \
											SET_REG32(0xF140007C, 0x0); \
											SET_REG32(0xF1400064, 0x0); \
											SET_REG32(0xF1400068, 0xF0B0002C); \
											SET_REG32(0xF1400070, 0x00);})

#define EN_WRITE_DMA_OPER(buf_addr, size)	({SET_REG32(0xF1400048, buf_addr); \
											  SET_REG32(0xF1400044, size); \
											  SET_REG32(0xF1400040, 0x004a200f); \
											  SET_REG32(0xF0B00030, GET_REG32(0xF0B00030) | 0x10ul);})

#define WRITE_FLASH_OPER(flash_addr, size)	({SET_REG32(0xF0B00028, flash_addr); \
											  SET_REG32(0xF0B00020, 0x61000000|((size -1)<<12)); \
											  SET_REG32(0xF0B00024, 0x2);})
#define FWRITE_FLASH_OPER(flash_addr, size)	({SET_REG32(0xF0B00028, flash_addr); \
											  SET_REG32(0xF0B00020, 0x61800000|((size -1)<<12)); \
											  SET_REG32(0xF0B00024, 0x32);})
#define WRITE_UICR_OPER(offset, size)		({SET_REG32(0xF0B00028, 0x3000|offset); \
											  SET_REG32(0xF0B00020, 0x61000000|((size -1)<<12)); \
											  SET_REG32(0xF0B00024, 0x42);})
#define READ_UICR_OPER(offset, size)		({SET_REG32(0xF0B00028, 0x3000|offset); \
											  SET_REG32(0xF0B00020, 0x69000000|(size -1)); \
											  SET_REG32(0xF0B00024, 0x48);})

#define DIS_WRITE_DMA_OPER(void)			({SET_REG32(0xF1400024, 0x1ul); \
											  SET_REG32(0xF1400040, GET_REG32(0xF1400040)&(~0x1ul)); \
											  SET_REG32(0xF1400030, 0xffffffff); \
											  SET_REG32(0xF0B00030, GET_REG32(0xF0B00030)&(~0x10ul));})
#define EN_READ_DMA_OPER(buf_addr, size)	({SET_REG32(0xF1400070, buf_addr); \
											  SET_REG32(0xF1400064, size); \
											  SET_REG32(0xF1400060, 0x004a810f); \
											  SET_REG32(0xF0B00030, GET_REG32(0xF0B00030) | 0x8ul);})
#define DIS_READ_DMA_OPER()					({SET_REG32(0xF1400024, 0x2ul); \
											  SET_REG32(0xF1400040, GET_REG32(0xF1400040)&(~0x1ul)); \
											  SET_REG32(0xF1400030, 0xffffffff); \
											  SET_REG32(0xF0B00030, GET_REG32(0xF0B00030)&(~0x8ul));})

#define CHECK_WRITE_DMA_DOWN()				(GET_REG32(0xF1400030)&0x10000ul)
#define CHECK_READ_DMA_DOWN()				(GET_REG32(0xF1400030)&0x20000ul)

#define RST_FLASH_OPER()				({ SET_REG32(0xF0B0002C, 0x66); \
											SET_REG32(0xF0B00020, 0x1000000); \
											SET_REG32(0xF0B00024, 0x0); \
											oper_waiting_end(); \
											SET_REG32(0xF0B0002C, 0x99); \
											SET_REG32(0xF0B00020, 0x1000000); \
											SET_REG32(0xF0B00024, 0x0); \
											oper_waiting_end(); \
											OPER_DELAY_10US(10);})
#define WRITE_EN_OPER()					({ SET_REG32(0xF0B0002C, 0x06); \
											SET_REG32(0xF0B00020, 0x1000000); \
											SET_REG32(0xF0B00024, 0x0); \
											oper_waiting_end();})
#define WRITE_CSR0_OPER(data)			({ SET_REG32(0xF0B0002C, 0x01|((data)<<8)); \
											SET_REG32(0xF0B00020, 0x1001000); \
											SET_REG32(0xF0B00024, 0x0); \
											oper_waiting_end();})
#define READ_CSR0_OPER()				({ SET_REG32(0xF0B0002C, 0x05); \
											SET_REG32(0xF0B00020, 0x3000000); \
											SET_REG32(0xF0B00024, 0x0); \
											oper_waiting_end(); \
											GET_REG32(0xF0B0002C);})
#define WRITE_CSR1_OPER(data)			({ SET_REG32(0xF0B0002C, 0x31|((data)<<8)); \
											SET_REG32(0xF0B00020, 0x1001000); \
											SET_REG32(0xF0B00024, 0x0); \
											oper_waiting_end();})
#define READ_CSR1_OPER()				({ SET_REG32(0xF0B0002C, 0x35); \
											SET_REG32(0xF0B00020, 0x3000000); \
											SET_REG32(0xF0B00024, 0x0); \
											oper_waiting_end(); \
											GET_REG32(0xF0B0002C);})
#define WRITE_CSR2_OPER(data)			({ SET_REG32(0xF0B0002C, 0x11|((data)<<8)); \
											SET_REG32(0xF0B00020, 0x1001000); \
											SET_REG32(0xF0B00024, 0x0); \
											oper_waiting_end();})
#define READ_CSR2_OPER()				({ SET_REG32(0xF0B0002C, 0x15); \
											SET_REG32(0xF0B00020, 0x3000000); \
											SET_REG32(0xF0B00024, 0x0); \
											oper_waiting_end(); \
											GET_REG32(0xF0B0002C);})

#define FLASH_ERASE_OPER(oper)			({ SET_REG32(0xF0B0002C, oper); \
											SET_REG32(0xF0B00020, 0x1003000); \
											SET_REG32(0xF0B00024, 0x0); \
											oper_waiting_end();})

#define UICR_ERASE_OPER()				({ SET_REG32(0xF0B0002C, 0x300044); \
											SET_REG32(0xF0B00020, 0x1003000); \
											SET_REG32(0xF0B00024, 0x0); \
											oper_waiting_end();})

unsigned int flash_signature[3] = {0x0, 0x0, 0x0};

__attribute__ ((section (".ram_ilm"))) unsigned int gtry_flash_ctrl_inital(unsigned int sysclk, freq_div_t freq_div)
{
	unsigned int timeout = 2000;//20ms
	unsigned int flash_csr = 0x0;

	if(sysclk < 7)
	{
		core_sysclk = sysclk;
	}

	if(oper_waiting_end())
	{
		return 1;
	}

	CFG_OPER_FREQ_DIV(freq_div);

	flash_csr = READ_CSR0_OPER();
	if(flash_csr&CSR0_FLASH_BUSY_MASK)//flash busy
	{
		return 2;
	}

	flash_csr = READ_CSR1_OPER();
	WRITE_EN_OPER();
	WRITE_CSR1_OPER(flash_csr|CSR1_W_R_SPEED_MASK);
	do{
		OPER_DELAY_10US(1);		//delay 10us under 200M
		flash_csr = READ_CSR1_OPER();
	}while((flash_csr&CSR1_W_R_SPEED_MASK) == 0x0 && --timeout);
	if(timeout == 0)
	{
		return 3;
	}
	CFG_OPER_EN_FSPEED();

	RST_FLASH_OPER();

	INIT_CFG_OPER();

	flash_csr = READ_CSR2_OPER();

	WRITE_EN_OPER();

	WRITE_CSR2_OPER(flash_csr|CSR2_PAGE512_EN_MASK);

	do{
		OPER_DELAY_10US(1);		//delay 10us under 200M
		flash_csr = READ_CSR2_OPER();
	}while((flash_csr&CSR2_PAGE512_EN_MASK) == 0x0 && --timeout);
	if(timeout == 0)
	{
		return 4;
	}

	INIT_DMA_OPER();

	return 0;
}

__attribute__ ((section (".flash_ep_ilm"))) unsigned int gtry_flash_normal_erase(unsigned int FlashAddr, erase_type_t EraseType)
{
	unsigned int timeout = 2000;//20ms
	unsigned int erase_cmd = 0x0;
	unsigned int flash_csr;

	if(FlashAddr >= FLASH_DATA_TOTALSIZE || FlashAddr < FLASH_USR_APP_ADDR)
	{
		return 1;
	}
	else if(EraseType == BLOCK_ERASE && (FlashAddr % FLASH_DATA_BLOCKSIZE) == 0x0)
	{
		erase_cmd = 0xD8;
	}
	else if(EraseType == HBLOCK_ERASE && (FlashAddr % FLASH_DATA_HBLOCKSIZE) == 0x0)
	{
		erase_cmd = 0x52;
	}
	else if(EraseType == SECTOR_ERASE && (FlashAddr % FLASH_DATA_SECTORSIZE) == 0x0)
	{
		erase_cmd = 0x20;
	}
	else if(EraseType == PAGE_ERASE && (FlashAddr % FLASH_DATA_PAGESIZE) == 0x0)
	{
		erase_cmd = 0x81;
	}
	else
	{
		return 2;
	}

	/*-- write enable --*/
	WRITE_EN_OPER();

	/*-- execute command --*/
	FLASH_ERASE_OPER(FLASH_ERASE_CTRL(erase_cmd, FlashAddr));

	/*-- get status --*/
	do{
		OPER_DELAY_10US(1);		//delay 10us under 200M
		flash_csr = READ_CSR0_OPER();
	}while((flash_csr&CSR0_FLASH_BUSY_MASK) && --timeout);//flash busy

	flash_csr = READ_CSR1_OPER();
	if(timeout == 0 || (flash_csr&CSR1_EP_FAIL_MASK))
	{
		return 4;
	}

	return 0;
}

__attribute__ ((section (".flash_ep_ilm"))) unsigned int gtry_flash_normal_write(unsigned int FlashAddr, unsigned char *WriteBuf, unsigned int WriteSize)
{
	unsigned int timeout = 2000;//20ms
	unsigned int tmp_addr;
	unsigned int flash_csr = 0;

	tmp_addr = (unsigned int)WriteBuf;

	if(FlashAddr >= FLASH_DATA_TOTALSIZE || (FlashAddr %4) || ((unsigned int )tmp_addr %4) || (WriteSize %4) || FlashAddr < FLASH_USR_APP_ADDR)
	{
		return 1;
	}
	else if(WriteSize > FLASH_DATA_PAGESIZE || ((FlashAddr%FLASH_DATA_PAGESIZE)+WriteSize) > FLASH_DATA_PAGESIZE)
	{
		return 2;
	}

	if(tmp_addr < 0x00210000)
	{
		tmp_addr |= 0xa0000000;
	}
	/*-- config dma --*/
	EN_WRITE_DMA_OPER(tmp_addr, WriteSize>>2);

	/*-- write enable --*/
	WRITE_EN_OPER();

#ifdef FAST_W_R_OPER_ENBALE
	FWRITE_FLASH_OPER(FlashAddr, WriteSize);
#else
	WRITE_FLASH_OPER(FlashAddr, WriteSize);
#endif

	do{
		OPER_DELAY_10US(1);		//delay 10us under 200M
	}while((oper_waiting_end() || CHECK_WRITE_DMA_DOWN() == 0) && --timeout);//dma busy

	DIS_WRITE_DMA_OPER();
	OPER_DELAY_10US(200);		//delay 10us under 200M

	flash_csr = READ_CSR1_OPER();
	if(timeout == 0 || (flash_csr&CSR1_EP_FAIL_MASK))
	{
		return 5;
	}

	return 0;
}

__attribute__ ((section (".flash_ep_ilm"))) unsigned int gtry_flash_normal_read(unsigned int FlashAddr, unsigned char *ReadBuf, unsigned int ReadSize)
{
	register unsigned long *src_ptr, *dst_ptr;
	register unsigned long i, size;

	if(FlashAddr >= FLASH_DATA_TOTALSIZE || (FlashAddr %4) || (((unsigned int )ReadBuf) %4) || (ReadSize %4))
	{
		return 1;
	}
	else if((FlashAddr+ReadSize) > FLASH_DATA_TOTALSIZE)
	{
		return 2;
	}

	size = ReadSize>>2;
	src_ptr = (unsigned long *)((unsigned long)FlashAddr|FLASH_ADDRESS);
	dst_ptr = (unsigned long *)ReadBuf;

	for (i = 0; i < size; i++)
		*dst_ptr++ = *src_ptr++;

	return 0;
}

__attribute__ ((section (".flash_ep_ilm"))) unsigned int gtry_flash_uicr_erase(void)
{
	unsigned int timeout = 2000;//20ms
	unsigned int flash_csr;

	WRITE_EN_OPER();

	UICR_ERASE_OPER();

	do{
		OPER_DELAY_10US(1);		//delay 10us under 200M
		flash_csr = READ_CSR0_OPER();
	}while((flash_csr&CSR0_FLASH_BUSY_MASK) && --timeout);//flash busy

	flash_csr = READ_CSR1_OPER();
	if(timeout == 0 || (flash_csr&CSR1_EP_FAIL_MASK))
	{
		return 1;
	}

	return 0;
}

__attribute__ ((section (".flash_ep_ilm"))) unsigned int gtry_flash_uicr_write(unsigned int Offset, unsigned char *WriteBuf, unsigned int WriteSize)
{
	unsigned int /*j, */timeout = 2000;//20ms
	unsigned int tmp_addr;
	unsigned int flash_csr = 0;

	tmp_addr = (unsigned int)WriteBuf;
	if(Offset >= FLASH_DATA_PAGESIZE || (Offset %4) || ((unsigned int )tmp_addr %4) || (WriteSize %4))
	{
		return 1;
	}
	else if(WriteSize > FLASH_DATA_PAGESIZE || (Offset+WriteSize) > FLASH_DATA_PAGESIZE)
	{
		return 2;
	}

	if(tmp_addr < 0x00210000)
	{
		tmp_addr |= 0xa0000000;
	}
	/*-- config dma --*/
	EN_WRITE_DMA_OPER(tmp_addr, WriteSize>>2);

	/*-- write enable --*/
	WRITE_EN_OPER();

	/*-- execute command --*/
	WRITE_UICR_OPER(Offset, WriteSize);

	do{
		OPER_DELAY_10US(1);		//delay 10us under 200M
	}while((oper_waiting_end() || CHECK_WRITE_DMA_DOWN() == 0) && --timeout);//dma busy

	DIS_WRITE_DMA_OPER();
	OPER_DELAY_10US(200);		//delay 10us under 200M

	flash_csr = READ_CSR1_OPER();
	if(timeout == 0 || (flash_csr&CSR1_EP_FAIL_MASK))
	{
		return 3;
	}

	return 0;
}

__attribute__ ((section (".ram_ilm"))) unsigned int gtry_flash_uicr_read(unsigned int Offset, unsigned char *ReadBuf, unsigned int ReadSize)
{
	unsigned int timeout = 2000;//20ms
	unsigned int tmp_addr;

	tmp_addr = (unsigned int)ReadBuf;
	if(Offset >= FLASH_DATA_PAGESIZE || (Offset %4) || ((unsigned int )tmp_addr %4) || (ReadSize %4))
	{
		return 1;
	}
	else if(ReadSize > FLASH_DATA_PAGESIZE || (Offset+ReadSize) > FLASH_DATA_PAGESIZE)
	{
		return 2;
	}

	if(tmp_addr < 0x00210000)
	{
		tmp_addr |= 0xa0000000;
	}
	/*-- config dma --*/
	EN_READ_DMA_OPER(tmp_addr, ReadSize>>2);

	/*-- execute command --*/
	READ_UICR_OPER(Offset, ReadSize);

	do{
		OPER_DELAY_10US(1);		//delay 10us under 200M
	}while((oper_waiting_end() || CHECK_READ_DMA_DOWN() == 0) && --timeout);//dma busy

	DIS_READ_DMA_OPER();
	OPER_DELAY_10US(200);		//delay 10us under 200M

	if(timeout == 0)
	{
		return 3;
	}

	return 0;
}

/*********************************************END OF FILE**********************/
