/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */

#ifndef __QSPI_FLASH_DRV_H__
#define __QSPI_FLASH_DRV_H__

#include "gtry_platform.h"
#include "gtry_spi_drv.h"
#include "gtry_dma_drv.h"


unsigned int QSPI_DMA_FLASH_init(unsigned int spi_div, p_dma_int_cb cb_event);
unsigned int QSPI_DMA_FLASH_check(void);
unsigned int QSPI_DMA_FLASH_read(unsigned int FlashAddr, unsigned char *DataBuf, unsigned int DataSize);
unsigned int QSPI_DMA_FLASH_dflag(void);
unsigned int QSPI_DMA_FLASH_fend(void);
unsigned int QSPI_DMA_FLASH_1read(unsigned int FlashAddr, unsigned char *DataBuf, unsigned int DataSize);
unsigned int QSPI_DMA_FLASH_2addr_read(unsigned int FlashAddr, unsigned char *DataBuf, unsigned int DataSize);
unsigned int QSPI_DMA_FLASH_4addr_read(unsigned int FlashAddr, unsigned char *DataBuf, unsigned int DataSize);
void QSPI_DMA_FLASH_disable_rxdma(void);
void QSPI_DMA_FLASH_prepare_suspend(void);
unsigned int QSPI_DMA_FLASH_enter_deep_power_down(void);
unsigned int QSPI_DMA_FLASH_prepare_resume(void);

extern unsigned int QSPI_EXT_FLASH_erase_sector(unsigned int sector_addr);
extern unsigned int QSPI_EXT_FLASH_erase_block_32k(unsigned int block_addr);
extern unsigned int QSPI_EXT_FLASH_erase_block_64k(unsigned int block_addr);
extern unsigned int QSPI_EXT_FLASH_erase_chip(void);
extern unsigned int QSPI_EXT_FLASH_write_page(unsigned int page_addr, const uint8_t *data, unsigned int size);
extern unsigned int QSPI_EXT_FLASH_write(unsigned int flash_addr, const uint8_t *data, unsigned int size);
extern unsigned int debug_read_external_flash(unsigned int flash_addr, uint8_t *buf, unsigned int size);
extern unsigned int QSPI_EXT_FLASH_read(unsigned int flash_addr, uint8_t *buf, unsigned int size);
extern unsigned int copy_internal_flash_to_external_flash(unsigned int external_flash_addr,unsigned int size);
extern uint8_t Test_extFlash (uint8_t cmd_buff);
#endif /* __QSPI_FLASH_DRV_H__ */

