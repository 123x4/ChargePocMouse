/*
 * Copyright (C) 2026 G.TECH TECHNOLOGY LTD. All rights reserved.
 * All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "gtry_r8080_drv.h"
//#include "gtry_hasp_drv.h"


#define  LCD_INT_IN_QSPI 0
#define  Puya_Flash		 1

#define  LCD_INT_TYPE_SEL    	  Puya_Flash

#define EXT_FLASH_USE_QSPI        (LCD_INT_TYPE_SEL == LCD_INT_IN_QSPI)
#define EXT_FLASH_USE_SINGLE_SPI  (LCD_INT_TYPE_SEL == Puya_Flash)

#define EXT_FLASH_DEEP_POWER_DOWN_ENABLE      1  /* Send 0xB9 before sleep and 0xAB after the final wake-up; set to 0 while isolating the seven cleanup steps. */
#define EXT_FLASH_SUSPEND_STEP3_DISABLE_DMA   1  /* clear the DMA0 channel 3 enable bit. */

/*  P25Q80SH SCLK divider setting (SYSCLK=200MHz)
 *  SCLK = SYSCLK / (PUYA_SCLK_DIV + 1)
 *  0 -> 100MHz (violates tCLH/tCLL min 3.7ns)
 *  1 -> 50MHz (OK, tCLH~5ns)
 */
#define PUYA_SCLK_DIV		 1

extern uint8_t array[64];
extern unsigned int Flash_id;
extern uint8_t buffer1[16];

extern unsigned int w_data_to_external_flash(void);
extern void external_flash_prepare_suspend(void);  /* Runs the selected external-Flash low-power debug action before suspend. */
extern void external_flash_resume(void);           /* Restores SPI5/DMA state and releases the external Flash from deep power-down. */
