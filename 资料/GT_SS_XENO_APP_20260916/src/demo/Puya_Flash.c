/*
 * Copyright (C) 2026 G.TECH TECHNOLOGY LTD. All rights reserved.
 * All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "gtry_dma_drv.h"
#include "gtry_qspi_flash_drv.h"
#include "gtry_platform.h"
#include "gtry_smu.h"
#include "gtry_wdt_drv.h"
#include "gtry_r8080_drv.h"
#include <string.h>
#include "Puya_Flash.h"
#include "gtry_platform.h"
#include "gtry_spi_drv.h"
#include "gtry_dma_drv.h"
#include "gtry_gpio_drv.h"

unsigned int Flash_id;
uint8_t buffer1[16];

uint8_t array[64] = {0,  1,  2,  3,  4,  5,  6, 7,
					 8,  9,  10, 11, 12, 13, 14,
					 15, 16, 17, 18, 19, 20, 21,
					 22, 23, 24, 25, 26, 27, 28,
					 29, 30, 31, 32, 33, 34, 35,
					 36, 37, 38, 39, 40, 41, 42,
					 43, 44, 45, 46, 47, 48, 49,
					 50, 51, 52, 53, 54, 55, 56,
					 57, 58, 59, 60, 61, 62, 63};


/**
* @brief  Write data to the external Flash (automatically handles sector erasure and page writing)
* @param  flash_addr: Target address of the external Flash
* @param  data: Data buffer (internal Flash XIP address or RAM address)
* @param  size: Data size
* @retval 0 = Success, non-zero = Failure
 */
unsigned int w_data_to_external_flash(void)
{
	unsigned int flash_id;

#if (LCD_INT_TYPE_SEL == LCD_INT_IN_QSPI)

	if(spi_io_multi_config(QSPI_FLASH_NO, SPI_IO_GP1, 0) || QSPI_DMA_FLASH_init(PUYA_SCLK_DIV, NULL))
#elif (LCD_INT_TYPE_SEL == Puya_Flash)
	if(spi_io_multi_config(QSPI_FLASH_NO, SPI_IO_GP1, 1) || QSPI_DMA_FLASH_init(PUYA_SCLK_DIV, NULL))
#endif
	{
		return 0;
	}
	ry_delay_ms(10);
	flash_id = QSPI_DMA_FLASH_check();
	if (flash_id <= 0xFF)
	{
		return 0;
	}
	return flash_id;
}

void external_flash_prepare_suspend(void)
{
	if (Flash_id <= 0xFFU)
	{
		return;                                  /* Skip low-power commands when external-Flash initialization failed. */
	}

#if EXT_FLASH_DEEP_POWER_DOWN_ENABLE
	QSPI_DMA_FLASH_enter_deep_power_down(); /* Send 0xB9 while SPI5 pins and controller state are still active. */
#endif

//#if EXT_FLASH_SUSPEND_UNBIND_SPI_DMA
	QSPI_DMA_FLASH_prepare_suspend();        /* Step 1: detach SPI5 DMA requests, abort DMA0 channel 3, reset FIFOs, and clear pending interrupts. */
//#endif

}

void external_flash_resume(void)
{
	if (Flash_id <= 0xFFU)
	{
		return;                                  /* Do not restore an external-Flash interface that was never initialized. */
	}

	/* Restore the SPI5 pin multiplexer before accessing any SPI register or
	 * sending the external-Flash release command. */
	if (spi_io_multi_config(QSPI_FLASH_NO, SPI_IO_GP1, 0) != 0U)
	{
		return;
	}

	/* Restore the saved SPI5/DMA0 state. The driver sends 0xAB only after the
	 * controller, DMA channel, FIFO, and interrupt state are usable again. */
	QSPI_DMA_FLASH_prepare_resume();
}
