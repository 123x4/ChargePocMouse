/*
 * Copyright (C) 2026 G.TECH TECHNOLOGY LTD. All rights reserved.
 * All rights reserved.
 */

#ifndef SRC_DEMO_TFT_H_
#define SRC_DEMO_TFT_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "app_config.h"
/*--------------------------------------------------------------------+
 * Constants
*/
#define FONT6x8_WIDTH   6
#define FONT6x8_HEIGHT  8

#define LCD_PANEL_ORIGINAL          0U
#define LCD_PANEL_TRULY_ST7735P3    1U
#define LCD_PANEL_BOE_ST7735SV      2U

#ifndef LCD_PANEL_TYPE
#define LCD_PANEL_TYPE              LCD_PANEL_TRULY_ST7735P3//LCD_PANEL_ORIGINAL//LCD_PANEL_BOE_ST7735SV//LCD_PANEL_TRULY_ST7735P3//LCD_PANEL_ORIGINAL
#endif

#if (LCD_PANEL_TYPE != LCD_PANEL_ORIGINAL) && \
	(LCD_PANEL_TYPE != LCD_PANEL_TRULY_ST7735P3) && \
	(LCD_PANEL_TYPE != LCD_PANEL_BOE_ST7735SV)
#error "Unsupported LCD_PANEL_TYPE"
#endif

#define LCD_SPI_DEFAULT_SPEED_HZ   20000000U//20000000U
#ifndef LCD_RDDID_DEBUG_ENABLE
#define LCD_RDDID_DEBUG_ENABLE     0U    /* Read and print the LCD ID once after panel initialization. */
#endif
#define LCD_RDDID_ID_LEN           3U    /* RDDID returns three ID bytes after the serial dummy clock. */
#define LCD_RDDID_DUMMY_BITS       1U    /* ST7735P3 4-line serial read uses one dummy clock, not one dummy byte. */
#define LCD_RDDID_HALF_PERIOD_US   1U    /* GPIO read clock: 1 us high + 1 us low, about 500 kHz. */
#define LCD_RDDID_ID1_INDEX        0U
#define LCD_RDDID_ID2_INDEX        1U
#define LCD_RDDID_ID3_INDEX        2U
#define LCD_SPI_WAIT_TIMEOUT_MS    20U   /* Bound each SPI/DMA chunk wait so a lost callback cannot freeze the keyboard. */
#define LCD_SPI_FRAME_RETRY_MAX    1U    /* Retry one full LCD window after a failed chunk to avoid shifted partial frames. */
#define LCD_SPI_CHUNK_BYTES        256U   /* Keep each DMA burst small so LCD_Service() returns to the main loop often. */
#define LCD_FULL_FRAME_BYTES       ((uint16_t)LCD_WIDTH * LCD_HEIGHT * 2U)
#define LCD_VERSION_TEXT_X         10U    /* Place the firmware string away from the panel edge. */
#define LCD_VERSION_TEXT_Y         60U    /* Keep the firmware string vertically centered on the 128x128 panel. */
#define LCD_VERSION_TEXT_LEN       11U    /* Firmware text format is "00.00.00.00". */
#define LCD_VERSION_CHAR_STEP      (FONT6x8_WIDTH + 1U)

#if LCD_MODULE_ENABLE
#define OLED_CS						PB0
#define LCD_SCL						PB1
#define LCD_SDA						PB2
#define OLED_RES					PB3
#define LCD_DET						PB4
#define LCD_DC						PB5
#define LCD_3V9						PB14

#define LCD_3V3						PA36
#define LCD_BL						PA40
#define I2C2_SCL					PA42//
#define I2C2_SDA					PA43//

#define LCD_BL_PWM_CH				PWM_G1_CH0_PA8_PA40
#define LCD_BL_PWM_CH_MASK			PWM_G1_CH0_PA8_PA40_MASK
#define LCD_BL_PWM_REUSE			RY_PWM_REUSE_P32_63
#define LCD_BL_PWM_PERIOD_US		100		/* 10kHz PWM frequency */
#define LCD_BL_DEFAULT_BRIGHTNESS	100		/* default brightness 0~100 */

#endif

#define LCD_WIDTH		128
#define LCD_HEIGHT		128
#define LCD_X_OFFSET	2
#define LCD_Y_OFFSET	3

#define LCD_PWM_CYCLE_MAX			100
#define LCD_PWM_CYCLE_MIN			0

#define WHITE			0xFFFF
#define BLACK			0x0000
#define BLUE			0x001F//0xF800
#define GREEN			0x07E0
#define RED				0xF800//0x001F
#define YELLOW			0xFFE0
#define CYAN			0x07FF
#define MAGENTA			0xF81F
#define ORANGE			0xFD20
#define PURPLE			0x780F
#define GRAY			0x8410

/* ST7735S Command Definitions */
#define ST7735S_SWRESET  0x01  /* Software Reset */
#define ST7735S_RDDID    0x04U /* Read Display ID: one serial dummy clock + ID1 + ID2 + ID3 */
#define ST7735S_SLPIN    0x10  /* Sleep In: stop DC/DC, oscillator, scanning; GRAM retained */
#define ST7735S_SLPOUT   0x11  /* Sleep Out: restart DC/DC & oscillator, wait 120ms after */
#define ST7735S_NORON    0x13  /* Normal Display Mode On */
#define ST7735S_INVOFF   0x20  /* Display Inversion Off */
#define ST7735S_CASET    0x2A  /* Column Address Set */
#define ST7735S_RASET    0x2B  /* Row Address Set */
#define ST7735S_RAMWR    0x2C  /* Memory Write */
#define ST7735S_DISPOFF  0x28  /* Display Off: output disabled, GRAM retained */
#define ST7735S_DISPON   0x29  /* Display On: output enabled */
#define ST7735S_TEON     0x35U /* Tearing Effect Line On */
#define ST7735S_MADCTL   0x36  /* Memory Data Access Control (scan direction, RGB/BGR) */
#define ST7735S_COLMOD   0x3A  /* Interface Pixel Format */
#define ST7735S_FRMCTR1  0xB1  /* Frame Rate Control (normal mode) */
#define ST7735S_FRMCTR2  0xB2  /* Frame Rate Control (idle mode) */
#define ST7735S_FRMCTR3  0xB3  /* Frame Rate Control (partial mode) */
#define ST7735S_INVCTR   0xB4  /* Display Inversion Control */
#define ST7735S_PWCTR1   0xC0  /* Power Control 1 */
#define ST7735S_PWCTR2   0xC1  /* Power Control 2 */
#define ST7735S_PWCTR3   0xC2  /* Power Control 3 (normal mode) */
#define ST7735S_PWCTR4   0xC3  /* Power Control 4 (idle mode) */
#define ST7735S_PWCTR5   0xC4  /* Power Control 5 (partial mode) */
#define ST7735S_VMCTR1   0xC5  /* VCOM Control 1 */
#define ST7735S_VENDOR_CMD_D6  0xD6U /* Wisechip vendor initialization command */
#define ST7735S_RDID1    0xDAU /* Read manufacturer ID: one serial dummy clock + ID1 */
#define ST7735S_RDID2    0xDBU /* Read module/driver version: one serial dummy clock + ID2 */
#define ST7735S_RDID3    0xDCU /* Read module/driver ID: one serial dummy clock + ID3 */
#define ST7735S_GMCTRP1  0xE0  /* Gamma (+polarity) Correction Characteristics */
#define ST7735S_GMCTRN1  0xE1  /* Gamma (-polarity) Correction Characteristics */
#define ST7735S_VENDOR_CMD_F0  0xF0U /* Wisechip vendor initialization command */

/* ST7735S command parameter definitions */
#define ST7735S_TE_MODE_VBLANK  0x00U /* Tearing output only during vertical blanking */
#define ST7735S_COLMOD_RGB565   0x05U /* 16-bit/pixel RGB565 interface format */
#define ST7735S_MADCTL_MY       0x80U /* Row address order */
#define ST7735S_MADCTL_MX       0x40U /* Column address order */
#define ST7735S_MADCTL_MV       0x20U /* Row/column exchange */
#define ST7735S_MADCTL_ML       0x10U /* Vertical refresh order */
#define ST7735S_MADCTL_BGR      0x08U /* BGR color order */
#define ST7735S_MADCTL_MH       0x04U /* Horizontal refresh order */

/*--------------------------------------------------------------------+
 * Public Function Prototypes
 *--------------------------------------------------------------------*/
uint8_t ST7735S_Init(void);
void lcd_bl_pwm_init(uint8_t brightness);
void lcd_bl_set_brightness(uint8_t brightness);
void lcd_bl_pwm_off(void);
/* Configure LCD SPI once and restart the timed LCD init state machine. */
void LCD_InitTickStart(void);
/* Reset the non-blocking LCD init state machine to its first command. */
void LCD_InitTickReset(void);
/* Advance LCD init when its command delay has expired; returns 1 when done. */
uint8_t LCD_InitTick(void);
#if LCD_RDDID_DEBUG_ENABLE
void LCD_ReadDisplayIdAndPrint(void);
#endif

extern void spi_init(uint32_t spi_speed);
extern void LCD_power_off(void);
extern void LCD_power_on(void);
extern void LCD_sleep(void);
extern void LCD_wakeup(void);
extern void LCD_Fill_Color(uint16_t color);
extern void LCD_Fill_White(void);
extern void read_Lcdid(void);

void LCD_Service(void);
extern void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_DrawImage(const uint8_t *image);
void LCD_ShowVersionPage(void);
void LCD_DrawChar(uint16_t x, uint16_t y, char ch, uint16_t fg, uint16_t bg);
void LCD_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg);
void LCD_ShowFwVersion(uint16_t x, uint16_t y);
void LCD_ShowFwVersionInner(uint16_t x, uint16_t y);
void LCD_FillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void LCD_DrawBorderFrameEx(uint16_t border_width, uint16_t outer_color, uint16_t inner_color);
/* Default border style: 1-pixel white outer border with black inner fill. */
void LCD_DrawBorderFrame(void);
/*
 * Draw a full-screen display test grid.
 *
 * rgb_mode = 0:
 *   Draw a black/white inspection grid intended for geometry, edge sharpness,
 *   line uniformity, dead-pixel visibility, and contrast checking.
 *
 * rgb_mode != 0:
 *   Draw a colored inspection grid where grid lines cycle through red, green,
 *   and blue. This is intended for quick subpixel/color-path validation and
 *   for checking whether primary colors render cleanly across the panel.
 */
void LCD_DrawTestGrid(uint8_t rgb_mode);
void LCD_DrawGrid9(void);
#endif
