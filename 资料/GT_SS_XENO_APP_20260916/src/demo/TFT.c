/*
 * Copyright (C) 2026 G.TECH TECHNOLOGY LTD. All rights reserved.
 * All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/
#include "TFT.h"
#include <stdio.h>
#include <string.h>
#include "gtry_gpio_drv.h"
#include "gtry_spi_drv.h"
#include "gtry_smu.h"
#include "gtry_regmap.h"
#include "gtry_drv_common.h"
#include "usb_descriptors.h"
#include "gtry_wdt_drv.h"
#include "gtry_pwm_drv.h"

#if LCD_MODULE_ENABLE
#define LCD_INIT_MAX_DATA_LEN 16U

typedef enum
{
	LCD_ASYNC_IDLE = 0,                    /* No asynchronous LCD fill is currently active. */
	LCD_ASYNC_SETUP,                       /* Program the LCD window and start the first DMA chunk. */
	LCD_ASYNC_WAIT_CHUNK                   /* Wait for the active DMA chunk without blocking the caller. */
} lcd_async_state_t;

typedef enum
{
	LCD_ASYNC_FRAME_NONE = 0,              /* No full-screen async frame is pending or active. */
	LCD_ASYNC_FRAME_COLOR,                 /* The async frame source is one repeated RGB565 color. */
	LCD_ASYNC_FRAME_IMAGE,                 /* The async frame source is a full-screen RGB565 image buffer. */
	LCD_ASYNC_FRAME_VERSION                /* The async frame generates black background and firmware text in each DMA chunk. */
} lcd_async_frame_type_t;

typedef struct
{
	lcd_async_state_t state;               /* Tracks which step LCD_Service() should execute next. */
	lcd_async_frame_type_t active_type;    /* Selects how the active frame's next DMA chunk is generated. */
	lcd_async_frame_type_t pending_type;   /* Selects how the newest pending frame request should be started. */
	uint8_t pending;                       /* A new fill request is waiting to replace or start a frame. */
	uint8_t transfer_active;               /* One SPI DMA chunk has been started and has not finished yet. */
	uint8_t retry;                         /* Counts full-frame retries after a lost SPI completion event. */
	uint8_t current_valid;                 /* current_color is meaningful only after one full frame finishes. */
	lcd_async_frame_type_t visible_type;   /* Last page type known to be completely visible on the panel. */
	uint16_t active_color;                 /* Color currently being transmitted by the async state machine. */
	uint16_t pending_color;                /* Most recent requested color while a frame is busy. */
	uint16_t current_color;                /* Last color known to have completed a full-screen fill. */
	const uint8_t *active_image;           /* Full-screen image buffer currently being transmitted. */
	const uint8_t *pending_image;          /* Newest requested full-screen image buffer. */
	uint32_t sent_bytes;                   /* Number of bytes already accepted by the LCD for this frame. */
	uint32_t active_chunk_len;             /* Length of the DMA chunk currently in flight. */
	unsigned long long chunk_start_ms;     /* Monotonic timestamp used for non-blocking transfer timeout. */
	uint8_t tx_buf[LCD_SPI_CHUNK_BYTES];   /* Stable DMA source buffer reused for solid-color chunks. */
} lcd_async_fill_t;

typedef enum {
	LCD_INIT_ACTION_COMMAND = 0,
	LCD_INIT_ACTION_RESET_LOW,
	LCD_INIT_ACTION_RESET_HIGH,
	LCD_INIT_ACTION_END,
} lcd_init_action_t;

typedef struct {
	lcd_init_action_t action;
	uint8_t command;
	uint8_t data_len;
	uint16_t delay_ms;
	uint8_t data[LCD_INIT_MAX_DATA_LEN];
} lcd_init_entry_t;

static const uint8_t font6x8[][6] =
{
    // '0' (0x30)
    { 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00 },
    // '1'
    { 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00 },
    // '2'
    { 0x42, 0x61, 0x51, 0x49, 0x46, 0x00 },
    // '3'
    { 0x21, 0x41, 0x45, 0x4B, 0x31, 0x00 },
    // '4'
    { 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00 },
    // '5'
    { 0x27, 0x45, 0x45, 0x45, 0x39, 0x00 },
    // '6'
    { 0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00 },
    // '7'
    { 0x01, 0x71, 0x09, 0x05, 0x03, 0x00 },
    // '8'
    { 0x36, 0x49, 0x49, 0x49, 0x36, 0x00 },
    // '9'
    { 0x06, 0x49, 0x49, 0x29, 0x1E, 0x00 },
    // '.' (index 10)
    { 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 },
};

volatile uint8_t spi_event_transfer_complete = 0;

static lcd_async_fill_t lcd_async_fill = { LCD_ASYNC_IDLE };

static void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
static void LCD_Fill_Color_Blocking(uint16_t color);
static void lcd_format_fw_version(uint32_t version, char text[LCD_VERSION_TEXT_LEN + 1U]);
static uint16_t lcd_version_pixel_color(uint16_t x, uint16_t y);

/*--------------------------------------------------------------------+
 * Public Function Prototypes
 *--------------------------------------------------------------------*/

/* LCD initialization entries are interpreted by one non-blocking state machine. */
#define LCD_INIT_COMMAND(command, delay_ms) \
	{ LCD_INIT_ACTION_COMMAND, (command), 0U, (delay_ms), { 0U } }

#define LCD_INIT_COMMAND_DATA(command, delay_ms, ...) \
	{ LCD_INIT_ACTION_COMMAND, (command), \
	  (uint8_t)sizeof((uint8_t[]){ __VA_ARGS__ }), (delay_ms), \
	  { __VA_ARGS__ } }

#define LCD_INIT_RESET_LOW(delay_ms) \
	{ LCD_INIT_ACTION_RESET_LOW, 0U, 0U, (delay_ms), { 0U } }

#define LCD_INIT_RESET_HIGH(delay_ms) \
	{ LCD_INIT_ACTION_RESET_HIGH, 0U, 0U, (delay_ms), { 0U } }

#define LCD_INIT_END() \
	{ LCD_INIT_ACTION_END, 0U, 0U, 0U, { 0U } }

#if LCD_PANEL_TYPE == LCD_PANEL_ORIGINAL
static const lcd_init_entry_t lcd_init_table_original[] = {
	LCD_INIT_COMMAND(ST7735S_SWRESET, 150U),
	LCD_INIT_COMMAND(ST7735S_SLPOUT, 120U),
	LCD_INIT_COMMAND_DATA(ST7735S_MADCTL, 0U, (ST7735S_MADCTL_MY | ST7735S_MADCTL_MX)),
	LCD_INIT_COMMAND_DATA(ST7735S_COLMOD, 0U, ST7735S_COLMOD_RGB565),
	LCD_INIT_COMMAND(ST7735S_DISPON, 100U),
	LCD_INIT_END(),
};

#define LCD_INIT_TABLE lcd_init_table_original
#elif LCD_PANEL_TYPE == LCD_PANEL_TRULY_ST7735P3
/* Initialization code for Truly ST7735P3 provided by Wisechip, parameter order remains unchanged. */
#define WISECHIP_TRULY_ST7735P3_VENDOR_F0_DATA  0x11U
#define WISECHIP_TRULY_ST7735P3_VENDOR_D6_DATA  0xCBU
#define WISECHIP_TRULY_ST7735P3_FRMCTR1_DATA    0x05U, 0x4CU, 0x4CU
#define WISECHIP_TRULY_ST7735P3_FRMCTR2_DATA    0x05U, 0x4CU, 0x4CU
#define WISECHIP_TRULY_ST7735P3_FRMCTR3_DATA    0x05U, 0x4CU, 0x4CU, 0x05U, 0x4CU, 0x4CU
#define WISECHIP_TRULY_ST7735P3_INVCTR_DATA     0x03U
#define WISECHIP_TRULY_ST7735P3_PWCTR1_DATA     0x62U, 0x02U
#define WISECHIP_TRULY_ST7735P3_PWCTR2_DATA     0x05U
#define WISECHIP_TRULY_ST7735P3_PWCTR3_DATA     0x0DU, 0x00U
#define WISECHIP_TRULY_ST7735P3_PWCTR4_DATA     0x8DU, 0x2AU
#define WISECHIP_TRULY_ST7735P3_PWCTR5_DATA     0x8DU, 0xEEU
#define WISECHIP_TRULY_ST7735P3_VMCTR1_DATA     0x18U
#define WISECHIP_TRULY_ST7735P3_GMCTRP1_DATA    0x0CU, 0x0BU, 0x08U, 0x08U, 0x17U, 0x0FU, 0x08U, 0x0DU, 0x0EU, 0x11U, 0x1CU, 0x38U, 0x00U, 0x08U, 0x03U, 0x3FU
#define WISECHIP_TRULY_ST7735P3_GMCTRN1_DATA    0x09U, 0x0DU, 0x05U, 0x0DU, 0x19U, 0x11U, 0x0BU, 0x10U, 0x11U, 0x16U, 0x1EU, 0x39U, 0x00U, 0x04U, 0x05U, 0x3FU

static const lcd_init_entry_t lcd_init_table_truly_st7735p3[] =
{
	LCD_INIT_RESET_HIGH(10U),
	LCD_INIT_RESET_LOW(10U),
	LCD_INIT_RESET_HIGH(120U),
	LCD_INIT_COMMAND(ST7735S_SLPOUT, 120U),
	LCD_INIT_COMMAND_DATA(ST7735S_VENDOR_CMD_F0, 0U, WISECHIP_TRULY_ST7735P3_VENDOR_F0_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_VENDOR_CMD_D6, 0U, WISECHIP_TRULY_ST7735P3_VENDOR_D6_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_FRMCTR1, 0U, WISECHIP_TRULY_ST7735P3_FRMCTR1_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_FRMCTR2, 0U, WISECHIP_TRULY_ST7735P3_FRMCTR2_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_FRMCTR3, 0U, WISECHIP_TRULY_ST7735P3_FRMCTR3_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_INVCTR, 0U, WISECHIP_TRULY_ST7735P3_INVCTR_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_PWCTR1, 0U, WISECHIP_TRULY_ST7735P3_PWCTR1_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_PWCTR2, 0U, WISECHIP_TRULY_ST7735P3_PWCTR2_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_PWCTR3, 0U, WISECHIP_TRULY_ST7735P3_PWCTR3_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_PWCTR4, 0U, WISECHIP_TRULY_ST7735P3_PWCTR4_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_PWCTR5, 0U, WISECHIP_TRULY_ST7735P3_PWCTR5_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_VMCTR1, 0U, WISECHIP_TRULY_ST7735P3_VMCTR1_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_GMCTRP1, 0U, WISECHIP_TRULY_ST7735P3_GMCTRP1_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_GMCTRN1, 0U, WISECHIP_TRULY_ST7735P3_GMCTRN1_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_TEON, 0U, ST7735S_TE_MODE_VBLANK),
	LCD_INIT_COMMAND_DATA(ST7735S_COLMOD, 0U, ST7735S_COLMOD_RGB565),
	LCD_INIT_COMMAND_DATA(ST7735S_MADCTL, 0U, (ST7735S_MADCTL_MY | ST7735S_MADCTL_MX | ST7735S_MADCTL_BGR)),
	LCD_INIT_COMMAND(ST7735S_DISPON, 0U),
	LCD_INIT_END(),
};

#define LCD_INIT_TABLE lcd_init_table_truly_st7735p3
#elif LCD_PANEL_TYPE == LCD_PANEL_BOE_ST7735SV
/* Initialization code for BOE ST7735SV provided by Wisechip. The parameter order is kept unchanged. */
#define WISECHIP_BOE_ST7735SV_FRMCTR1_DATA  0x05U, 0x3CU, 0x3CU
#define WISECHIP_BOE_ST7735SV_FRMCTR2_DATA  0x05U, 0x3CU, 0x3CU
#define WISECHIP_BOE_ST7735SV_FRMCTR3_DATA  0x05U, 0x3CU, 0x3CU, 0x05U, 0x3CU, 0x3CU
#define WISECHIP_BOE_ST7735SV_INVCTR_DATA   0x03U
#define WISECHIP_BOE_ST7735SV_PWCTR1_DATA   0xA4U, 0x04U
#define WISECHIP_BOE_ST7735SV_PWCTR2_DATA   0xC8U
#define WISECHIP_BOE_ST7735SV_PWCTR3_DATA   0x0DU, 0x00U
#define WISECHIP_BOE_ST7735SV_PWCTR4_DATA   0x8DU, 0x2AU
#define WISECHIP_BOE_ST7735SV_PWCTR5_DATA   0x8DU, 0xEEU
#define WISECHIP_BOE_ST7735SV_VMCTR1_DATA   0x1DU
#define WISECHIP_BOE_ST7735SV_GMCTRP1_DATA  0x0CU, 0x0AU, 0x06U, 0x00U, 0x1AU, 0x11U, 0x0BU, 0x0AU, 0x0BU, 0x0CU, 0x19U, 0x33U, 0x00U, 0x06U, 0x02U, 0x10U
#define WISECHIP_BOE_ST7735SV_GMCTRN1_DATA  0x0DU, 0x0FU, 0x03U, 0x00U, 0x11U, 0x0AU, 0x06U, 0x08U, 0x09U, 0x0DU, 0x1CU, 0x3AU, 0x00U, 0x09U, 0x07U, 0x10U

static const lcd_init_entry_t lcd_init_table_boe_st7735sv[] =
{
	LCD_INIT_RESET_HIGH(10U),
	LCD_INIT_RESET_LOW(10U),
	LCD_INIT_RESET_HIGH(120U),
	LCD_INIT_COMMAND(ST7735S_SLPOUT, 120U),
	LCD_INIT_COMMAND_DATA(ST7735S_FRMCTR1, 0U, WISECHIP_BOE_ST7735SV_FRMCTR1_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_FRMCTR2, 0U, WISECHIP_BOE_ST7735SV_FRMCTR2_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_FRMCTR3, 0U, WISECHIP_BOE_ST7735SV_FRMCTR3_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_INVCTR, 0U, WISECHIP_BOE_ST7735SV_INVCTR_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_PWCTR1, 0U, WISECHIP_BOE_ST7735SV_PWCTR1_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_PWCTR2, 0U, WISECHIP_BOE_ST7735SV_PWCTR2_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_PWCTR3, 0U, WISECHIP_BOE_ST7735SV_PWCTR3_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_PWCTR4, 0U, WISECHIP_BOE_ST7735SV_PWCTR4_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_PWCTR5, 0U, WISECHIP_BOE_ST7735SV_PWCTR5_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_VMCTR1, 0U, WISECHIP_BOE_ST7735SV_VMCTR1_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_GMCTRP1, 0U, WISECHIP_BOE_ST7735SV_GMCTRP1_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_GMCTRN1, 0U, WISECHIP_BOE_ST7735SV_GMCTRN1_DATA),
	LCD_INIT_COMMAND_DATA(ST7735S_TEON, 0U, ST7735S_TE_MODE_VBLANK),
	LCD_INIT_COMMAND_DATA(ST7735S_COLMOD, 0U, ST7735S_COLMOD_RGB565),
	LCD_INIT_COMMAND_DATA(ST7735S_MADCTL, 0U, (ST7735S_MADCTL_MY | ST7735S_MADCTL_MX | ST7735S_MADCTL_BGR)),
	LCD_INIT_COMMAND(ST7735S_DISPON, 0U),
	LCD_INIT_END(),
};
#define LCD_INIT_TABLE lcd_init_table_boe_st7735sv
#endif


/*---------- LCD Backlight PWM ----------*/
static uint32_t lcd_bl_pwm_cycle_ticks = 0;

void lcd_bl_pwm_init(uint8_t brightness)
{
	uint32_t duty_us = (uint32_t)LCD_BL_PWM_PERIOD_US * brightness / 100;

	pwm_single_config(LCD_BL_PWM_REUSE, LCD_BL_PWM_CH,
					  RY_PWM_UP_COUNT, LCD_BL_PWM_PERIOD_US, duty_us,
					  PWM_POLARITY_POS);

	uint32_t fclk_factor = g_sysclk_freq / 1000000;
	uint32_t scaler_val = ((GTRY_PWM->PWM_SCALER >> (PWM_GROUP1 << 3)) & 0xff) + 1;
	lcd_bl_pwm_cycle_ticks = LCD_BL_PWM_PERIOD_US * fclk_factor / scaler_val - 1;

	pwm_chl_enable(LCD_BL_PWM_CH_MASK);
}

void lcd_bl_set_brightness(uint8_t brightness)
{
	if (brightness > 100)
		brightness = 100;

	uint32_t duty_ticks = lcd_bl_pwm_cycle_ticks * brightness / 100;
	pwm_set_duty(LCD_BL_PWM_CH, duty_ticks);
	pwm_update_en(1);
}

void lcd_bl_pwm_off(void)
{
	pwm_single_disable(LCD_BL_PWM_REUSE, LCD_BL_PWM_CH);
}


void spi_callback(uint32_t event)
{
	switch (event) {
		case RY_SPI_EVENT_TRANSFER_COMPLETE:
			spi_event_transfer_complete = 1;
			break;
		case RY_SPI_EVENT_DATA_LOST:
			break;
	}
}

static RY_DRIVER_SPI *spi_drv = NULL;

void spi_init(uint32_t spi_speed)
{
	spi_drv = &Driver_SPI1;

	spi_io_multi_config(SPI_NO1, SPI_IO_GP1, 0);

	spi_drv->Initialize(spi_callback);
	spi_drv->PowerControl(RY_POWER_FULL);

	spi_drv->Control(RY_SPI_MODE_MASTER    |
						RY_SPI_CPOL0_CPHA0 |
						RY_SPI_MSB_LSB     |
						RY_SPI_SS_MASTER_UNUSED |
						RY_SPI_DATA_BITS(8), spi_speed);

	/* spi_io_multi_config ?? PA32~PA35 ???? SPI ??,
	 * PA32(CS) ? PA35(DC) ?? GPIO ????, ????? GPIO ?? */
	gpio_cfg_output(OLED_CS, RY_GPIO_DRIVE4);
	gpio_out_set(OLED_CS);

	gpio_cfg_output(LCD_DC, RY_GPIO_DRIVE4);
	gpio_out_set(LCD_DC);
}

static void lcd_spi_recover(void)
{
	gpio_out_set(OLED_CS);                                      /* Release the LCD bus before resetting the SPI block. */
	spi_drv->Control(RY_SPI_ABORT_TRANSFER, 0);                 /* Clear a stuck SPI busy state after a missed completion event. */
	spi_event_transfer_complete = 1;                            /* Unblock any local wait path that observes the shared flag. */
	spi_init(LCD_SPI_DEFAULT_SPEED_HZ);                         /* Rebuild SPI and DMA configuration for the next LCD transfer. */
}

static uint8_t lcd_spi_wait_complete(void)
{
	unsigned long long start_ms = ry_get_mtime_ms();            /* Use a monotonic deadline so the wait cannot become infinite. */

	while (!spi_event_transfer_complete)
	{
		wdt_reset_timer();                                      /* Keep the watchdog alive during a long SPI/DMA wait. */
		if ((ry_get_mtime_ms() - start_ms) >= LCD_SPI_WAIT_TIMEOUT_MS)
		{
			lcd_spi_recover();                                  /* Recover the peripheral when the expected callback is missing. */
			return 0;                                           /* Tell the caller this chunk did not finish cleanly. */
		}
	}

	return 1;                                                   /* The SPI callback reported transfer completion. */
}

static uint8_t lcd_spi_send_wait(const uint8_t *data, uint32_t len)
{
	spi_event_transfer_complete = 0;                            /* Arm the completion flag before starting a new transfer. */
	if (spi_drv->Send(data, len) != RY_DRIVER_OK)
	{
		lcd_spi_recover();                                      /* Recover immediately if the driver refuses the transfer. */
		return 0;                                               /* Let the caller retry the whole LCD operation if needed. */
	}

	return lcd_spi_wait_complete();                             /* Wait with timeout instead of blocking forever. */
}

#if LCD_RDDID_DEBUG_ENABLE
static void lcd_gpio_half_period_delay(void)
{
	ry_delay_us(LCD_RDDID_HALF_PERIOD_US);
}

static void lcd_gpio_write_byte(uint8_t data)
{
	uint8_t mask;

	for (mask = 0x80U; mask != 0U; mask >>= 1U)
	{
		gpio_out_clear(LCD_SCL);
		if ((data & mask) != 0U)
		{
			gpio_out_set(LCD_SDA);
		}
		else
		{
			gpio_out_clear(LCD_SDA);
		}
		lcd_gpio_half_period_delay();
		gpio_out_set(LCD_SCL);
		lcd_gpio_half_period_delay();
	}
	/* Leave SCL high so SDA can be released before the final falling edge. */
}

static uint8_t lcd_gpio_read_bit(void)
{
	uint8_t value;

	lcd_gpio_half_period_delay();
	gpio_out_set(LCD_SCL);
	lcd_gpio_half_period_delay();
	value = (uint8_t)gpio_in_read(LCD_SDA);
	gpio_out_clear(LCD_SCL);

	return value;
}

static uint8_t lcd_gpio_read_byte(void)
{
	uint8_t data = 0U;
	uint8_t bit;

	for (bit = 0U; bit < 8U; bit++)
	{
		data = (uint8_t)((data << 1U) | lcd_gpio_read_bit());
	}

	return data;
}

static uint8_t lcd_gpio_begin_read(uint8_t command)
{
	uint8_t dummy_bit = 0U;
	uint8_t bit;

	/* Configure all four signals as GPIO before starting one CS-continuous read transaction. */
	gpio_out_set(OLED_CS);
	gpio_out_clear(LCD_SCL);
	gpio_out_clear(LCD_SDA);
	gpio_cfg_output(OLED_CS, RY_GPIO_DRIVE4);
	gpio_cfg_output(LCD_SCL, RY_GPIO_DRIVE4);
	gpio_cfg_output(LCD_SDA, RY_GPIO_DRIVE4);
	gpio_cfg_output(LCD_DC, RY_GPIO_DRIVE4);

	gpio_out_clear(LCD_DC);
	gpio_out_clear(OLED_CS);
	lcd_gpio_half_period_delay();
	lcd_gpio_write_byte(command);

	/* Release SDA before the command's last falling edge, then consume the serial dummy clock. */
	gpio_out_set(LCD_DC);
	gpio_cfg_input(LCD_SDA, RY_GPIO_NOPULL);
	gpio_out_clear(LCD_SCL);
	for (bit = 0U; bit < LCD_RDDID_DUMMY_BITS; bit++)
	{
		dummy_bit = lcd_gpio_read_bit();
	}

	return dummy_bit;
}

static uint8_t lcd_gpio_read_id_register(uint8_t command)
{
	uint8_t value;

	(void)lcd_gpio_begin_read(command);
	value = lcd_gpio_read_byte();
	gpio_out_set(OLED_CS);
	lcd_gpio_half_period_delay();

	return value;
}

static void lcd_gpio_test_sda(uint8_t *pull_down, uint8_t *pull_up)
{
	/* With CS high the panel must release SDA, so the weak internal pulls should control PB2. */
	gpio_out_set(OLED_CS);
	gpio_cfg_output(OLED_CS, RY_GPIO_DRIVE4);
	gpio_cfg_input(LCD_SDA, RY_GPIO_PULLDN_30K);
	lcd_gpio_half_period_delay();
	*pull_down = (uint8_t)gpio_in_read(LCD_SDA);
	gpio_cfg_input(LCD_SDA, RY_GPIO_PULLUP_30K);
	lcd_gpio_half_period_delay();
	*pull_up = (uint8_t)gpio_in_read(LCD_SDA);
	gpio_cfg_input(LCD_SDA, RY_GPIO_NOPULL);
}

void LCD_ReadDisplayIdAndPrint(void)
{
	uint8_t id[LCD_RDDID_ID_LEN] = { 0U };
	uint8_t id_register[LCD_RDDID_ID_LEN] = { 0U };
	uint8_t dummy_bit;
	uint8_t pull_down;
	uint8_t pull_up;
	uint8_t i;

	/* Stop the hardware SPI before temporarily using its clock/data pins as GPIO. */
	if (spi_drv != NULL)
	{
		spi_drv->Control(RY_SPI_ABORT_TRANSFER, 0U);
	}

	lcd_gpio_test_sda(&pull_down, &pull_up);
	dummy_bit = lcd_gpio_begin_read(ST7735S_RDDID);
	for (i = 0U; i < LCD_RDDID_ID_LEN; i++)
	{
		id[i] = lcd_gpio_read_byte();
	}
	gpio_out_set(OLED_CS);
	lcd_gpio_half_period_delay();

	/* DAh/DBh/DCh provide an independent check of the three bytes returned by 04h. */
	id_register[LCD_RDDID_ID1_INDEX] = lcd_gpio_read_id_register(ST7735S_RDID1);
	id_register[LCD_RDDID_ID2_INDEX] = lcd_gpio_read_id_register(ST7735S_RDID2);
	id_register[LCD_RDDID_ID3_INDEX] = lcd_gpio_read_id_register(ST7735S_RDID3);
	spi_init(LCD_SPI_DEFAULT_SPEED_HZ);

	printf("LCD SDA idle test: PD=%u PU=%u\r\n",
		   (unsigned int)pull_down,
		   (unsigned int)pull_up);
	printf("LCD RDDID(04h): DUMMY=%u ID=%02X %02X %02X\r\n",
		   (unsigned int)dummy_bit,
		   (unsigned int)id[LCD_RDDID_ID1_INDEX],
		   (unsigned int)id[LCD_RDDID_ID2_INDEX],
		   (unsigned int)id[LCD_RDDID_ID3_INDEX]);
	printf("LCD RDIDx: DA=%02X DB=%02X DC=%02X\r\n",
		   (unsigned int)id_register[LCD_RDDID_ID1_INDEX],
		   (unsigned int)id_register[LCD_RDDID_ID2_INDEX],
		   (unsigned int)id_register[LCD_RDDID_ID3_INDEX]);
	printf("LCD ID: MFR=0x%02X VER=0x%02X DRIVER=0x%02X\r\n",
		   (unsigned int)id[LCD_RDDID_ID1_INDEX],
		   (unsigned int)id[LCD_RDDID_ID2_INDEX],
		   (unsigned int)id[LCD_RDDID_ID3_INDEX]);
}
#endif

static void lcd_format_fw_version(uint32_t version, char text[LCD_VERSION_TEXT_LEN + 1U])
{
	uint8_t parts[4] = {                                        /* Split the selected packed version into four decimal byte fields. */
		(uint8_t)((version >> 24) & 0xFF),
		(uint8_t)((version >> 16) & 0xFF),
		(uint8_t)((version >> 8) & 0xFF),
		(uint8_t)(version & 0xFF)
	};
	uint8_t out = 0;                                            /* Track the next write position in the fixed text buffer. */

	for (uint8_t i = 0; i < 4U; i++)
	{
		text[out++] = (char)('0' + (parts[i] / 10U));            /* Write the tens digit without calling snprintf in the LCD service path. */
		text[out++] = (char)('0' + (parts[i] % 10U));            /* Write the ones digit for the same byte field. */
		if (i != 3U)
		{
			text[out++] = '.';                                   /* Keep the existing "00.00.00.00" visual format. */
		}
	}
	text[out] = '\0';                                           /* Terminate the string for safe local indexing. */
}
static uint16_t lcd_version_pixel_color(uint16_t x, uint16_t y)
{
	char text[LCD_VERSION_TEXT_LEN + 1U];                       /* Small stack buffer holding one fixed firmware-version string. */
	uint16_t line_y = LCD_VERSION_TEXT_Y;                       /* First row prints FW_VERSION at the configured text origin. */
	uint32_t version = FW_VERSION;                              /* Default to the public firmware version for the first row. */
	uint16_t fg = WHITE;                                        /* Match the blocking version display: outer FW is white. */

	if (x < LCD_VERSION_TEXT_X)
	{
		return BLACK;                                           /* Pixels before the text origin are part of the black background. */
	}

	if (y >= (uint16_t)(LCD_VERSION_TEXT_Y + FONT6x8_HEIGHT + 2U))
	{
		line_y = (uint16_t)(LCD_VERSION_TEXT_Y + FONT6x8_HEIGHT + 2U); /* Second row starts below the first text row plus the same gap. */
		version = FW_VERSION_INNER;                             /* The second row prints the internal firmware version. */
		fg = YELLOW;                                            /* Match LCD_ShowFwVersionInner() so the inner version is easy to distinguish. */
	}
	else if (y < LCD_VERSION_TEXT_Y)
	{
		return BLACK;                                           /* Pixels above both version rows stay black. */
	}

	uint16_t local_x = (uint16_t)(x - LCD_VERSION_TEXT_X);       /* Convert panel X into text-local coordinates. */
	uint16_t local_y = (uint16_t)(y - line_y);                   /* Convert panel Y into the selected text row coordinates. */
	if (local_y >= FONT6x8_HEIGHT)
	{
		return BLACK;                                           /* Gap between rows and pixels below the second row stay black. */
	}

	uint16_t char_index = (uint16_t)(local_x / LCD_VERSION_CHAR_STEP); /* Select which character cell this pixel belongs to. */
	uint16_t char_col = (uint16_t)(local_x % LCD_VERSION_CHAR_STEP);   /* Select the column inside the character cell. */
	if ((char_index >= LCD_VERSION_TEXT_LEN) || (char_col >= FONT6x8_WIDTH))
	{
		return BLACK;                                           /* Inter-character spacing and text tail remain black. */
	}

	lcd_format_fw_version(version, text);                       /* Build the selected row string only when the pixel is inside the text box. */
	char ch = text[char_index];                                 /* Read the character that owns this pixel. */
	const uint8_t *bitmap = NULL;                               /* Font bitmap pointer; NULL means unsupported/background. */
	if ((ch >= '0') && (ch <= '9'))
	{
		bitmap = font6x8[ch - '0'];                             /* Digits use font entries 0..9. */
	}
	else if (ch == '.')
	{
		bitmap = font6x8[10];                                   /* The dot uses the final font entry. */
	}

	if ((bitmap != NULL) && (bitmap[char_col] & (1U << local_y)))
	{
		return fg;                                              /* Set foreground pixels to the row color. */
	}
	return BLACK;                                               /* Everything else is black background. */
}
static uint8_t lcd_async_prepare_chunk(void)
{
	uint32_t remaining = LCD_FULL_FRAME_BYTES - lcd_async_fill.sent_bytes; /* Compute the bytes still needed for this frame. */
	uint32_t chunk = (remaining > LCD_SPI_CHUNK_BYTES) ? LCD_SPI_CHUNK_BYTES : remaining; /* Clamp the final chunk to the frame tail. */

	if (lcd_async_fill.active_type == LCD_ASYNC_FRAME_COLOR)
	{
		for (uint32_t i = 0; i < chunk; i += 2U)
		{
			lcd_async_fill.tx_buf[i] = (uint8_t)(lcd_async_fill.active_color >> 8); /* Store the high RGB565 byte for a solid pixel. */
			lcd_async_fill.tx_buf[i + 1U] = (uint8_t)(lcd_async_fill.active_color & 0xFF); /* Store the low RGB565 byte beside it. */
		}
	}
	else if (lcd_async_fill.active_type == LCD_ASYNC_FRAME_VERSION)
	{
		for (uint32_t i = 0; i < chunk; i += 2U)
		{
			uint32_t byte_offset = lcd_async_fill.sent_bytes + i; /* Convert chunk-local byte index to frame byte offset. */
			uint32_t pixel_index = byte_offset >> 1;             /* Two bytes form one RGB565 pixel. */
			uint16_t x = (uint16_t)(pixel_index % LCD_WIDTH);    /* Convert the linear pixel index to panel X. */
			uint16_t y = (uint16_t)(pixel_index / LCD_WIDTH);    /* Convert the linear pixel index to panel Y. */
			uint16_t color = lcd_version_pixel_color(x, y);      /* Compose black background plus outer/inner version rows in software. */
			lcd_async_fill.tx_buf[i] = (uint8_t)(color >> 8);    /* Store the composed pixel high byte for DMA. */
			lcd_async_fill.tx_buf[i + 1U] = (uint8_t)(color & 0xFF); /* Store the composed pixel low byte for DMA. */
		}
	}
	else if (lcd_async_fill.active_type == LCD_ASYNC_FRAME_IMAGE)
	{
		if (lcd_async_fill.active_image == NULL)
		{
			return 0;                                           /* Refuse an image frame with no source buffer. */
		}
		memcpy(lcd_async_fill.tx_buf, &lcd_async_fill.active_image[lcd_async_fill.sent_bytes], chunk); /* Copy image bytes into a stable DMA buffer. */
	}
	else
	{
		return 0;                                               /* Unknown frame type: fail safely instead of sending garbage. */
	}

	lcd_async_fill.active_chunk_len = chunk;                    /* Remember the exact length that this DMA chunk will send. */
	return 1;                                                   /* The transmit buffer is ready for SPI DMA. */
}

static void lcd_async_fill_abort(void)
{
	if (lcd_async_fill.transfer_active && (spi_drv != NULL))
	{
		lcd_spi_recover();                                      /* Stop the in-flight DMA chunk before a synchronous draw takes over. */
	}

	gpio_out_set(OLED_CS);                                      /* Release the LCD chip-select so the next command starts cleanly. */
	lcd_async_fill.state = LCD_ASYNC_IDLE;                      /* Return the state machine to a known idle state. */
	lcd_async_fill.active_type = LCD_ASYNC_FRAME_NONE;           /* Clear the active frame kind after aborting the transfer. */
	lcd_async_fill.pending_type = LCD_ASYNC_FRAME_NONE;          /* Clear the pending frame kind with the pending request. */
	lcd_async_fill.pending = 0;                                  /* Drop stale async requests that would corrupt the following sync draw. */
	lcd_async_fill.transfer_active = 0;                          /* Mark that no async DMA chunk is owned by LCD_Service(). */
	lcd_async_fill.current_valid = 0;                            /* A synchronous caller may change pixels, so the solid-color cache is stale. */
	lcd_async_fill.visible_type = LCD_ASYNC_FRAME_NONE;          /* The panel content is no longer a tracked async page. */
}

static uint8_t lcd_async_fill_start_chunk(void)
{
	if (!lcd_async_prepare_chunk())
	{
		return 0;                                               /* Do not start SPI DMA if the source buffer is invalid. */
	}

	spi_event_transfer_complete = 0;                            /* Arm the shared completion flag before starting SPI DMA. */
	if (spi_drv->Send(lcd_async_fill.tx_buf, lcd_async_fill.active_chunk_len) != RY_DRIVER_OK)
	{
		lcd_spi_recover();                                      /* Recover immediately if the SPI driver rejects the DMA request. */
		lcd_async_fill.transfer_active = 0;                      /* Keep the async state consistent after recovery. */
		return 0;                                               /* Ask the state machine to retry the frame from pixel zero. */
	}

	lcd_async_fill.chunk_start_ms = ry_get_mtime_ms();           /* Record a timeout deadline without waiting here. */
	lcd_async_fill.transfer_active = 1;                          /* Tell LCD_Service() there is now a DMA chunk in flight. */
	return 1;                                                    /* The chunk was launched successfully. */
}

static void lcd_async_fill_finish_frame(void)
{
	lcd_async_frame_type_t finished_type = lcd_async_fill.active_type; /* Save the frame kind before resetting state. */

	gpio_out_set(OLED_CS);                                      /* Close the RAMWR data stream after the final pixel byte. */
	if (finished_type == LCD_ASYNC_FRAME_COLOR)
	{
		lcd_async_fill.current_color = lcd_async_fill.active_color; /* Remember the completed solid color to suppress duplicates. */
		lcd_async_fill.current_valid = 1;                         /* Mark current_color as valid only for plain solid-color pages. */
		lcd_async_fill.visible_type = LCD_ASYNC_FRAME_COLOR;      /* The visible page is now a completed solid-color page. */
	}
	else
	{
		lcd_async_fill.current_valid = 0;                         /* Image/version pages are not equivalent to a completed solid color. */
		lcd_async_fill.visible_type = finished_type;              /* Remember the completed non-color page type. */
	}
	lcd_async_fill.state = LCD_ASYNC_IDLE;                       /* Make the async engine available for the next request. */
	lcd_async_fill.active_type = LCD_ASYNC_FRAME_NONE;           /* Clear the active frame kind after completion. */
	lcd_async_fill.transfer_active = 0;                          /* No DMA transfer remains active after the frame is done. */
}

static void lcd_async_fill_restart_or_drop(void)
{
	gpio_out_set(OLED_CS);                                      /* End the partial RAMWR stream before retrying or giving up. */
	lcd_async_fill.transfer_active = 0;                          /* The failed chunk is no longer considered active. */
	if (lcd_async_fill.retry < LCD_SPI_FRAME_RETRY_MAX)
	{
		lcd_async_fill.retry++;                                  /* Count this full-frame retry attempt. */
		lcd_async_fill.state = LCD_ASYNC_SETUP;                  /* Reprogram CASET/RASET/RAMWR so the LCD GRAM pointer restarts. */
	}
	else
	{
		lcd_async_fill.current_valid = 0;                        /* A failed frame may be partial, so do not cache it as a completed color. */
		lcd_async_fill.state = LCD_ASYNC_IDLE;                   /* Give up after bounded retries so the main loop never spins forever. */
		lcd_async_fill.active_type = LCD_ASYNC_FRAME_NONE;       /* Clear the failed frame kind before accepting later requests. */
		lcd_async_fill.visible_type = LCD_ASYNC_FRAME_NONE;      /* A failed frame leaves the visible page unknown. */
	}
}

void LCD_Service(void)
{
	if (spi_drv == NULL)
	{
		return;                                                  /* LCD SPI is not initialized yet, so there is no transfer to service. */
	}

	if (lcd_async_fill.state == LCD_ASYNC_IDLE)
	{
		if (!lcd_async_fill.pending)
		{
			return;                                              /* No pending fill request, so this service call is intentionally cheap. */
		}

		lcd_async_fill.active_type = lcd_async_fill.pending_type; /* Promote the newest requested frame kind to the active frame. */
		lcd_async_fill.active_color = lcd_async_fill.pending_color; /* Promote the newest requested color/background to the active frame. */
		lcd_async_fill.active_image = lcd_async_fill.pending_image; /* Promote the newest requested image pointer to the active frame. */
		lcd_async_fill.pending = 0;                              /* Consume the pending request before starting the frame. */
		lcd_async_fill.pending_type = LCD_ASYNC_FRAME_NONE;      /* Clear the pending kind now that it is active. */
		lcd_async_fill.retry = 0;                                /* A new frame starts with a fresh retry budget. */
		lcd_async_fill.state = LCD_ASYNC_SETUP;                  /* Continue below and launch the first chunk in this call. */
	}

	if (lcd_async_fill.state == LCD_ASYNC_SETUP)
	{
		LCD_SetWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);       /* Point ST7735S RAMWR at the full display area. */
		gpio_out_set(LCD_DC);                                    /* Data mode: following bytes are pixel data, not commands. */
		gpio_out_clear(OLED_CS);                                 /* Hold CS low across all chunks in this frame. */
		lcd_async_fill.sent_bytes = 0;                           /* Start counting from the first pixel byte in the frame. */
		if (lcd_async_fill_start_chunk())
		{
			lcd_async_fill.state = LCD_ASYNC_WAIT_CHUNK;          /* The first DMA chunk is running; future calls poll its completion. */
		}
		else
		{
			lcd_async_fill_restart_or_drop();                     /* A launch failure is handled like a lost chunk. */
		}
		return;
	}

	if (lcd_async_fill.state == LCD_ASYNC_WAIT_CHUNK)
	{
		if (!spi_event_transfer_complete)
		{
			if ((ry_get_mtime_ms() - lcd_async_fill.chunk_start_ms) < LCD_SPI_WAIT_TIMEOUT_MS)
			{
				return;                                          /* DMA is still within its deadline; return to the main loop. */
			}

			lcd_spi_recover();                                  /* The expected completion callback was missed, so reset SPI safely. */
			lcd_async_fill_restart_or_drop();                     /* Retry from a fresh full-screen window or give up after the limit. */
			return;
		}

		lcd_async_fill.transfer_active = 0;                      /* The callback completed the active DMA chunk. */
		if (lcd_async_fill.pending &&
			((lcd_async_fill.pending_type != lcd_async_fill.active_type) ||
			 (lcd_async_fill.pending_color != lcd_async_fill.active_color) ||
			 (lcd_async_fill.pending_image != lcd_async_fill.active_image)))
		{
			gpio_out_set(OLED_CS);                                /* End the stale partial frame before switching to the newest color. */
			lcd_async_fill.active_type = lcd_async_fill.pending_type; /* Promote the latest page request immediately after a safe chunk boundary. */
			lcd_async_fill.active_color = lcd_async_fill.pending_color; /* Promote the latest color/background for the replacement frame. */
			lcd_async_fill.active_image = lcd_async_fill.pending_image; /* Promote the latest image pointer for the replacement frame. */
			lcd_async_fill.pending = 0;                           /* Consume the replacement request so only one target frame remains. */
			lcd_async_fill.pending_type = LCD_ASYNC_FRAME_NONE;   /* Clear the pending kind after promotion. */
			lcd_async_fill.retry = 0;                             /* Give the replacement frame its own retry budget. */
			lcd_async_fill.state = LCD_ASYNC_SETUP;               /* Restart CASET/RASET/RAMWR so the new frame begins at pixel zero. */
			return;                                               /* Let the next LCD_Service() call launch the replacement frame. */
		}
		lcd_async_fill.sent_bytes += lcd_async_fill.active_chunk_len; /* Account for the bytes just accepted by the LCD. */
		if (lcd_async_fill.sent_bytes >= LCD_FULL_FRAME_BYTES)
		{
			lcd_async_fill_finish_frame();                        /* The full screen is now updated. */
			return;
		}

		if (!lcd_async_fill_start_chunk())
		{
			lcd_async_fill_restart_or_drop();                     /* Restart if the next chunk could not be launched. */
		}
	}
}

static void LCD_WriteCommand(uint8_t cmd)
{
	gpio_out_clear(LCD_DC);
	gpio_out_clear(OLED_CS);
	lcd_spi_send_wait(&cmd, 1);                                 /* Send one command byte without allowing an infinite wait. */

	gpio_out_set(OLED_CS);
}

static void LCD_WriteData(uint8_t data)
{
	gpio_out_set(LCD_DC);
	gpio_out_clear(OLED_CS);
	lcd_spi_send_wait(&data, 1);                                /* Send one data byte without allowing an infinite wait. */
	gpio_out_set(OLED_CS);
}

void OLED_Reset(void)
{
	gpio_out_set(OLED_RES);
	ry_delay_ms(10);
	gpio_out_clear(OLED_RES);
	ry_delay_ms(10);
	gpio_out_set(OLED_RES);

	wdt_reset_timer();
	ry_delay_ms(120);
}

void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	LCD_WriteCommand(ST7735S_CASET);
	LCD_WriteData((x0 + LCD_X_OFFSET) >> 8);
	LCD_WriteData((x0 + LCD_X_OFFSET) & 0xFF);
	LCD_WriteData((x1 + LCD_X_OFFSET) >> 8);
	LCD_WriteData((x1 + LCD_X_OFFSET) & 0xFF);

	LCD_WriteCommand(ST7735S_RASET);
	LCD_WriteData((y0 + LCD_Y_OFFSET) >> 8);
	LCD_WriteData((y0 + LCD_Y_OFFSET) & 0xFF);
	LCD_WriteData((y1 + LCD_Y_OFFSET) >> 8);
	LCD_WriteData((y1 + LCD_Y_OFFSET) & 0xFF);

	LCD_WriteCommand(ST7735S_RAMWR);
}

void LCD_Fill_Color(uint16_t color)
{
	if ((lcd_async_fill.state != LCD_ASYNC_IDLE) &&
		(lcd_async_fill.active_type == LCD_ASYNC_FRAME_COLOR) &&
		(lcd_async_fill.active_color == color))
	{
		lcd_async_fill.pending = 0;                              /* The requested color is already being sent, so drop older requests. */
		lcd_async_fill.pending_type = LCD_ASYNC_FRAME_NONE;      /* Keep the pending slot empty after suppressing a duplicate color. */
		return;                                                  /* Return immediately instead of restarting the same full frame. */
	}

	if ((lcd_async_fill.state == LCD_ASYNC_IDLE) &&
		lcd_async_fill.current_valid &&
		(lcd_async_fill.current_color == color))
	{
		return;                                                  /* The screen already contains this completed solid color. */
	}

	lcd_async_fill.pending_color = color;                        /* Store only the newest requested color for the async service loop. */
	lcd_async_fill.pending_image = NULL;                         /* A solid-color frame does not need an image source buffer. */
	lcd_async_fill.pending_type = LCD_ASYNC_FRAME_COLOR;         /* Mark this request as a solid-color full-screen frame. */
	lcd_async_fill.pending = 1;                                  /* Let LCD_Service() start or replace the fill when the bus is ready. */
}

static void LCD_Fill_Color_Blocking(uint16_t color)
{
	uint8_t buf[256];
	uint32_t total = (uint32_t)LCD_WIDTH * LCD_HEIGHT * 2;
	uint32_t i;
	uint8_t retry;

	lcd_async_fill_abort();                                      /* Keep synchronous drawing from colliding with an async frame. */

	for(i = 0; i < 256; i += 2)
	{
		buf[i]   = color >> 8;
		buf[i+1] = color & 0xFF;
	}

	for (retry = 0; retry <= LCD_SPI_FRAME_RETRY_MAX; retry++)
	{
		uint8_t failed = 0;                                     /* Restart the full frame if any chunk times out. */

		LCD_SetWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

		gpio_out_set(LCD_DC);
		gpio_out_clear(OLED_CS);

		for(i = 0; i < total; i += 256)
		{
			uint32_t chunk = (total - i) > 256 ? 256 : (total - i);
			if (!lcd_spi_send_wait(buf, chunk))
			{
				failed = 1;                                     /* A timeout may have advanced the LCD GRAM pointer partially. */
				break;                                          /* Leave CS high and retry from a fresh full-screen window. */
			}
		}

		gpio_out_set(OLED_CS);
		if (!failed)
		{
			lcd_async_fill.current_color = color;                 /* Track the color completed by this blocking fill too. */
			lcd_async_fill.current_valid = 1;                     /* Duplicate async requests can now be suppressed. */
			return;                                             /* The whole screen was sent successfully. */
		}
	}
}

void LCD_DrawImage(const uint8_t *image)
{
	if (image == NULL)
	{
		return;                                                  /* Ignore an invalid image pointer instead of starting a bad DMA frame. */
	}

	lcd_async_fill.pending_color = 0;                            /* Image frames do not use the solid-color source value. */
	lcd_async_fill.pending_image = image;                        /* Store the caller's persistent full-screen RGB565 image buffer. */
	lcd_async_fill.pending_type = LCD_ASYNC_FRAME_IMAGE;         /* Mark this request as an image-backed full-screen frame. */
	lcd_async_fill.pending = 1;                                  /* LCD_Service() will stream the image in DMA chunks. */
}

void LCD_ShowVersionPage(void)
{
	if ((lcd_async_fill.visible_type == LCD_ASYNC_FRAME_VERSION) &&
		(lcd_async_fill.state == LCD_ASYNC_IDLE) &&
		(lcd_async_fill.pending == 0))
	{
		return;                                                  /* The complete version page is already visible. */
	}

	if (((lcd_async_fill.state != LCD_ASYNC_IDLE) &&
		 (lcd_async_fill.active_type == LCD_ASYNC_FRAME_VERSION)) ||
		(lcd_async_fill.pending &&
		 (lcd_async_fill.pending_type == LCD_ASYNC_FRAME_VERSION)))
	{
		return;                                                  /* The version page is already being prepared. */
	}

	lcd_async_fill.pending_color = BLACK;                        /* Clear the page to black before writing white version text. */
	lcd_async_fill.pending_image = NULL;                         /* The version page background is generated as a solid color. */
	lcd_async_fill.pending_type = LCD_ASYNC_FRAME_VERSION;       /* Ask the service loop to generate the complete version page asynchronously. */
	lcd_async_fill.pending = 1;                                  /* Queue the version page without blocking the encoder scan. */
}

void LCD_DrawChar(uint16_t x, uint16_t y, char ch, uint16_t fg, uint16_t bg)
{
    const uint8_t *bitmap = NULL;
    uint8_t  buf[FONT6x8_WIDTH * FONT6x8_HEIGHT * 2];
    uint32_t idx = 0;
    uint8_t  supported = 0;

    if (x >= LCD_WIDTH || y >= LCD_HEIGHT)
        return;
    if (x + FONT6x8_WIDTH > LCD_WIDTH || y + FONT6x8_HEIGHT > LCD_HEIGHT)
        return;

    if (ch >= '0' && ch <= '9')
    {
        bitmap    = font6x8[ch - '0'];
        supported = 1;
    }
    else if (ch == '.')
    {
        bitmap    = font6x8[10];
        supported = 1;
    }
    else
    {
        supported = 0;
    }

    if (supported)
    {
        for (uint8_t row = 0; row < FONT6x8_HEIGHT; row++)
        {
            for (uint8_t col = 0; col < FONT6x8_WIDTH; col++)
            {
                uint8_t bits = bitmap[col];
                uint16_t color = (bits & (1 << row)) ? fg : bg;

                buf[idx++] = color >> 8;
                buf[idx++] = color & 0xFF;
            }
        }
    }
    else
    {
        for (uint8_t row = 0; row < FONT6x8_HEIGHT; row++)
        {
            for (uint8_t col = 0; col < FONT6x8_WIDTH; col++)
            {
                buf[idx++] = bg >> 8;
                buf[idx++] = bg & 0xFF;
            }
        }
    }

    lcd_async_fill_abort();                                      /* Text drawing is synchronous and must not overlap an async fill chunk. */

    LCD_SetWindow(x, y, x + FONT6x8_WIDTH - 1, y + FONT6x8_HEIGHT - 1);

    gpio_out_set(LCD_DC);
    gpio_out_clear(OLED_CS);

    lcd_spi_send_wait(buf, sizeof(buf));                         /* Use the bounded SPI wait helper instead of an infinite spin. */

    gpio_out_set(OLED_CS);
}

void LCD_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg)
{
    uint16_t cursor_x = x;

    while (*str)
    {
        LCD_DrawChar(cursor_x, y, *str, fg, bg);
        cursor_x += FONT6x8_WIDTH + 1;
        str++;
    }
}

void LCD_ShowFwVersionInner(uint16_t x, uint16_t y)
{
    char buf[16];

    uint32_t v = FW_VERSION_INNER;
    uint8_t v3 = (v >> 24) & 0xFF;
    uint8_t v2 = (v >> 16) & 0xFF;
    uint8_t v1 = (v >> 8)  & 0xFF;
    uint8_t v0 = (v >> 0)  & 0xFF;

    snprintf(buf, sizeof(buf), "%02u.%02u.%02u.%02u", v3, v2, v1, v0);

    LCD_DrawString(x, y, buf, YELLOW, BLACK);
}

void LCD_ShowFwVersion(uint16_t x, uint16_t y)
{
    char buf[16];

    uint32_t v = FW_VERSION;
    uint8_t v3 = (v >> 24) & 0xFF;
    uint8_t v2 = (v >> 16) & 0xFF;
    uint8_t v1 = (v >> 8)  & 0xFF;
    uint8_t v0 = (v >> 0)  & 0xFF;

    snprintf(buf, sizeof(buf), "%02u.%02u.%02u.%02u", v3, v2, v1, v0);

    LCD_DrawString(x, y, buf, WHITE, BLACK);
    LCD_ShowFwVersionInner(x, y + FONT6x8_HEIGHT + 2);
}


static void bitbang_write_cmd(uint8_t cmd)
{
	int8_t bit;

	gpio_out_clear(PB1);
	gpio_cfg_output(PB1, RY_GPIO_DRIVE4);
	gpio_cfg_output(PB2, RY_GPIO_DRIVE4);

	gpio_out_clear(LCD_DC);
	gpio_out_clear(OLED_CS);

	for (bit = 7; bit >= 0; bit--)
	{
		if (cmd & (1 << bit))
			gpio_out_set(PB2);
		else
			gpio_out_clear(PB2);

		gpio_out_set(PB1);
		gpio_out_clear(PB1);
	}
}

static void bitbang_read_bytes(uint8_t *buf, uint8_t len)
{
	int8_t bit;

	gpio_cfg_input(PB2, RY_GPIO_NOPULL);
	gpio_out_set(LCD_DC);

	for (uint8_t i = 0; i < len; i++)
	{
		uint8_t byte = 0;
		for (bit = 0; bit < 8; bit++)
		{
			gpio_out_set(PB1);
			gpio_out_clear(PB1);
			byte = (byte << 1) | (gpio_in_read(PB2) & 0x01);
		}
		buf[i] = byte;
	}

	gpio_out_set(OLED_CS);
}

static void bitbang_restore_spi(void)
{
	spi_io_multi_config(SPI_NO1, SPI_IO_GP1, 0);
	gpio_cfg_output(OLED_CS, RY_GPIO_DRIVE4);
	gpio_out_set(OLED_CS);
	gpio_cfg_output(LCD_DC, RY_GPIO_DRIVE4);
	gpio_out_set(LCD_DC);
}

void read_Lcdid(void)
{
	uint8_t rddid[4] = {0};
	uint8_t rdid1[2] = {0};
	uint8_t rdid2[2] = {0};
	uint8_t rdid3[2] = {0};

	/* RDDID (0x04): 1 dummy + 3 ID = 4 bytes */
	bitbang_write_cmd(0x04);
	bitbang_read_bytes(rddid, 4);
	bitbang_restore_spi();

	/* RDID1 (0xDA): 1 dummy + 1 byte */
	bitbang_write_cmd(0xDA);
	bitbang_read_bytes(rdid1, 2);
	bitbang_restore_spi();

	/* RDID2 (0xDB): 1 dummy + 1 byte */
	bitbang_write_cmd(0xDB);
	bitbang_read_bytes(rdid2, 2);
	bitbang_restore_spi();

	/* RDID3 (0xDC): 1 dummy + 1 byte */
	bitbang_write_cmd(0xDC);
	bitbang_read_bytes(rdid3, 2);
	bitbang_restore_spi();

	info("=== RDDID(0x04) raw 4 bytes ===\r\n");
	info("  [0]=0x%02X [1]=0x%02X [2]=0x%02X [3]=0x%02X\r\n",
		 rddid[0], rddid[1], rddid[2], rddid[3]);

	info("=== RDID1/2/3 (0xDA/DB/DC) each 2 bytes ===\r\n");
	info("  RDID1: dummy=0x%02X val=0x%02X\r\n", rdid1[0], rdid1[1]);
	info("  RDID2: dummy=0x%02X val=0x%02X\r\n", rdid2[0], rdid2[1]);
	info("  RDID3: dummy=0x%02X val=0x%02X\r\n", rdid3[0], rdid3[1]);
}


void LCD_Fill_White(void)
{
	LCD_Fill_Color(WHITE);
}

void LCD_power_off(void)
{
	lcd_async_fill_abort();                                      /* Stop any pending LCD DMA before removing panel power. */
    gpio_out_clear(LCD_BL);
    gpio_out_clear(LCD_3V3);
    gpio_out_clear(LCD_3V9);
}

void LCD_power_on(void)
{
	gpio_out_set(LCD_3V3);
	gpio_out_set(LCD_3V9);
	ry_delay_us(5000);
	ST7735S_Init();
	LCD_Fill_White();
	gpio_out_set(LCD_BL);
}

void LCD_sleep(void)
{
	lcd_async_fill_abort();                                      /* Ensure no RAMWR data burst is active before sleep commands. */
	lcd_bl_set_brightness(LCD_PWM_CYCLE_MIN);
	ry_delay_ms(1);
	LCD_WriteCommand(ST7735S_DISPOFF);
	LCD_WriteCommand(ST7735S_SLPIN);
	gpio_out_clear(LCD_BL);
}

void LCD_wakeup(void)
{
	lcd_async_fill_abort();                                      /* Start wake commands from a clean LCD/SPI bus state. */
	LCD_WriteCommand(ST7735S_SLPOUT);
	ry_delay_ms(120);
	LCD_WriteCommand(ST7735S_DISPON);
	gpio_out_set(LCD_BL);
}



static uint16_t lcd_init_index = 0U;                 /* Next selected-table entry to execute. */
static unsigned long long lcd_init_deadline_ms = 0U; /* Absolute deadline for the current non-blocking wait. */
static uint8_t lcd_init_done = 0U;                   /* Latch the terminal state across repeated tick calls. */

void LCD_InitTickStart(void)
{
	spi_init(LCD_SPI_DEFAULT_SPEED_HZ);     /* Configure SPI once before the table executor sends LCD commands. */
	LCD_InitTickReset();                    /* Restart the selected sequence without blocking for command delays. */
}

void LCD_InitTickReset(void)
{
	lcd_init_index = 0U;
	lcd_init_deadline_ms = 0U;
	lcd_init_done = 0U;
}

uint8_t LCD_InitTick(void)
{
	unsigned long long now_ms = ry_get_mtime_ms();

	if (lcd_init_done != 0U)
	{
		return 1U;
	}

	if ((lcd_init_deadline_ms != 0U) && (now_ms < lcd_init_deadline_ms))
	{
		return 0U;
	}
	lcd_init_deadline_ms = 0U;

	while (lcd_init_done == 0U)
	{
		const lcd_init_entry_t *entry = &LCD_INIT_TABLE[lcd_init_index++];
		uint8_t i;

		switch (entry->action)
		{
			case LCD_INIT_ACTION_COMMAND:
				LCD_WriteCommand(entry->command);
				for (i = 0U; i < entry->data_len; i++)
				{
					LCD_WriteData(entry->data[i]);
				}
				break;

			case LCD_INIT_ACTION_RESET_LOW:
				gpio_out_clear(OLED_RES);
				break;

			case LCD_INIT_ACTION_RESET_HIGH:
				gpio_out_set(OLED_RES);
				break;

			case LCD_INIT_ACTION_END:
			default:
				lcd_init_done = 1U;
				return 1U;
		}

		if (entry->delay_ms != 0U)
		{
			lcd_init_deadline_ms = now_ms + entry->delay_ms;
			return 0U;
		}
	}

	return 1U;
}

//uint8_t ST7735S_Init(void)
//{
//	spi_init(LCD_SPI_DEFAULT_SPEED_HZ);//1/ 66 ns = 15.15 Mhz
//
//	OLED_Reset();
//
//	LCD_WriteCommand(ST7735S_SWRESET);
//	ry_delay_ms(150);
//
//	LCD_WriteCommand(ST7735S_SLPOUT);
//	ry_delay_ms(200);
//
//	LCD_WriteCommand(ST7735S_FRMCTR1);
//	LCD_WriteData(0x01);
//	LCD_WriteData(0x2C);
//	LCD_WriteData(0x2D);
//
//	LCD_WriteCommand(ST7735S_FRMCTR2);
//	LCD_WriteData(0x01);
//	LCD_WriteData(0x2C);
//	LCD_WriteData(0x2D);
//
//	LCD_WriteCommand(ST7735S_FRMCTR3);
//	LCD_WriteData(0x01);
//	LCD_WriteData(0x2C);
//	LCD_WriteData(0x2D);
//	LCD_WriteData(0x01);
//	LCD_WriteData(0x2C);
//	LCD_WriteData(0x2D);
//
//	LCD_WriteCommand(ST7735S_INVCTR);
//	LCD_WriteData(0x07);
//
//	LCD_WriteCommand(ST7735S_PWCTR1);
//	LCD_WriteData(0xA2);
//	LCD_WriteData(0x02);
//	LCD_WriteData(0x84);
//
//	LCD_WriteCommand(ST7735S_PWCTR2);
//	LCD_WriteData(0xC5);
//
//	LCD_WriteCommand(ST7735S_PWCTR3);
//	LCD_WriteData(0x0A);
//	LCD_WriteData(0x00);
//
//	LCD_WriteCommand(ST7735S_PWCTR4);
//	LCD_WriteData(0x8A);
//	LCD_WriteData(0x2A);
//
//	LCD_WriteCommand(ST7735S_PWCTR5);
//	LCD_WriteData(0x8A);
//	LCD_WriteData(0xEE);
//
//	LCD_WriteCommand(ST7735S_VMCTR1);
//	LCD_WriteData(0x0E);
//
//	LCD_WriteCommand(ST7735S_INVOFF);
//
//	LCD_WriteCommand(ST7735S_MADCTL);
//	LCD_WriteData(0xC0);
//
//	LCD_WriteCommand(ST7735S_COLMOD);
//	LCD_WriteData(0x05);
//
//	LCD_WriteCommand(ST7735S_GMCTRP1);
//	LCD_WriteData(0x0F);
//	LCD_WriteData(0x1A);
//	LCD_WriteData(0x0F);
//	LCD_WriteData(0x18);
//	LCD_WriteData(0x2F);
//	LCD_WriteData(0x28);
//	LCD_WriteData(0x20);
//	LCD_WriteData(0x22);
//	LCD_WriteData(0x1F);
//	LCD_WriteData(0x1B);
//	LCD_WriteData(0x23);
//	LCD_WriteData(0x37);
//	LCD_WriteData(0x00);
//	LCD_WriteData(0x07);
//	LCD_WriteData(0x02);
//	LCD_WriteData(0x10);
//
//	LCD_WriteCommand(ST7735S_GMCTRN1);
//	LCD_WriteData(0x0F);
//	LCD_WriteData(0x1B);
//	LCD_WriteData(0x0F);
//	LCD_WriteData(0x17);
//	LCD_WriteData(0x33);
//	LCD_WriteData(0x2C);
//	LCD_WriteData(0x29);
//	LCD_WriteData(0x2E);
//	LCD_WriteData(0x30);
//	LCD_WriteData(0x30);
//	LCD_WriteData(0x39);
//	LCD_WriteData(0x3F);
//	LCD_WriteData(0x00);
//	LCD_WriteData(0x07);
//	LCD_WriteData(0x03);
//	LCD_WriteData(0x10);
//
//	LCD_WriteCommand(ST7735S_NORON);
//	ry_delay_ms(10);
//
//	LCD_WriteCommand(ST7735S_DISPON);
//	ry_delay_ms(100);
//
//	return 0;
//}

uint8_t ST7735S_Init(void)
{
	LCD_InitTickStart();                    /* Use the same one-shot start path as non-blocking callers. */
	while (!LCD_InitTick())
	{
		wdt_reset_timer();                  /* Feed WDT while this compatibility wrapper waits for init completion. */
	}
	return 0;                               /* Preserve the existing success return convention. */
}


static void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT)
        return;

    LCD_SetWindow(x, y, x, y);
    LCD_WriteData(color >> 8);
    LCD_WriteData(color & 0xFF);
}

void LCD_FillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	uint8_t buf[256];
	uint32_t w = x1 - x0 + 1;
	uint32_t h = y1 - y0 + 1;
	uint32_t total = w * h * 2;
	uint32_t i;
	uint8_t retry;

	lcd_async_fill_abort();                                      /* Rectangles are synchronous overlays, so stop async fill first. */

	for (i = 0; i < 256; i += 2)
	{
		buf[i]   = color >> 8;
		buf[i+1] = color & 0xFF;
	}

	for (retry = 0; retry <= LCD_SPI_FRAME_RETRY_MAX; retry++)
	{
		uint8_t failed = 0;                                     /* Retry the rectangle from its first pixel on transfer timeout. */

		LCD_SetWindow(x0, y0, x1, y1);

		gpio_out_set(LCD_DC);
		gpio_out_clear(OLED_CS);

		for (i = 0; i < total; i += 256)
		{
			uint32_t chunk = (total - i) > 256 ? 256 : (total - i);
			if (!lcd_spi_send_wait(buf, chunk))
			{
				failed = 1;                                     /* The current rectangle may be partially written. */
				break;                                          /* Retry with a new CASET/RASET/RAMWR window. */
			}
		}

		gpio_out_set(OLED_CS);
		if (!failed)
		{
			return;                                             /* The rectangle completed normally. */
		}
	}
}

/*
 * Draw a full-screen border frame with configurable colors.
 *
 * border_width:
 *   Outer border thickness in pixels. Values of 0 are clamped to 1.
 *
 * outer_color:
 *   RGB565 color used for the outermost border.
 *
 * inner_color:
 *   RGB565 color used to fill the inside area enclosed by the border.
 */
void LCD_DrawBorderFrameEx(uint16_t border_width, uint16_t outer_color, uint16_t inner_color)
{
	uint16_t max_border_w = (LCD_WIDTH < LCD_HEIGHT) ? (LCD_WIDTH / 2U) : (LCD_HEIGHT / 2U);

	if (border_width == 0)
	{
		border_width = 1;
	}

	if (border_width > max_border_w)
	{
		border_width = max_border_w;
	}

	/* Fill the whole screen first, then paint only the outermost border area. */
	LCD_Fill_Color_Blocking(inner_color);                       /* Keep border composition ordered: background must finish before edges. */

	LCD_FillRect(0, 0, LCD_WIDTH - 1, border_width - 1, outer_color);
	LCD_FillRect(0, LCD_HEIGHT - border_width, LCD_WIDTH - 1, LCD_HEIGHT - 1, outer_color);
	LCD_FillRect(0, border_width, border_width - 1, LCD_HEIGHT - border_width - 1, outer_color);
	LCD_FillRect(LCD_WIDTH - border_width, border_width, LCD_WIDTH - 1, LCD_HEIGHT - border_width - 1, outer_color);
}


void LCD_DrawBorderFrame(void)
{
	/* Default production/test frame: white 1-pixel border with black center. */
	LCD_DrawBorderFrameEx(1, WHITE, BLACK);
}

/*
 * Draw a display test grid over the full panel area.
 *
 * This function is meant for TFT production / inspection patterns.
 * The goal is to expose panel issues such as:
 *   - uneven spacing or scaling,
 *   - weak contrast,
 *   - line jitter / missing columns or rows,
 *   - color path / channel problems.
 *
 * Pattern definition:
 *   rgb_mode == 0
 *     White background with black grid lines.
 *     This gives a high-contrast monochrome pattern that is useful for
 *     checking geometry, aliasing, edge sharpness, and obvious pixel defects.
 *
 *   rgb_mode != 0
 *     Black background with grid lines cycling through RED / GREEN / BLUE.
 *     This makes it easier to verify the three primary color channels and
 *     quickly spot if one color path is weak, shifted, or missing.
 *
 * Current grid parameters:
 *   - line spacing: 16 pixels
 *   - line width  : 1 pixel
 *
 */
void LCD_DrawTestGrid(uint8_t rgb_mode)
{
	static const uint16_t rgb_grid_colors[3] = { RED, GREEN, BLUE };
	const uint16_t background = rgb_mode ? BLACK : WHITE;
	const uint16_t mono_grid = BLACK;
	const uint16_t grid_step = 16;
	const uint16_t line_width = 1;
	uint16_t x;
	uint16_t y;

	LCD_Fill_Color_Blocking(background);                         /* Grid lines depend on the background being fully drawn first. */

	for (x = 0; x < LCD_WIDTH; x += grid_step)
	{
		uint16_t line_color = rgb_mode ? rgb_grid_colors[(x / grid_step) % 3] : mono_grid;
		uint16_t x1 = x + line_width - 1;
		if (x1 >= LCD_WIDTH)
		{
			x1 = LCD_WIDTH - 1;
		}
		LCD_FillRect(x, 0, x1, LCD_HEIGHT - 1, line_color);
	}

	for (y = 0; y < LCD_HEIGHT; y += grid_step)
	{
		uint16_t line_color = rgb_mode ? rgb_grid_colors[(y / grid_step) % 3] : mono_grid;
		uint16_t y1 = y + line_width - 1;
		if (y1 >= LCD_HEIGHT)
		{
			y1 = LCD_HEIGHT - 1;
		}
		LCD_FillRect(0, y, LCD_WIDTH - 1, y1, line_color);
	}
}

/*
 *	The rendering shows 9 solid colour blocks
 */
void LCD_DrawGrid9(void)
{
	LCD_DrawTestGrid(1);
}

#else
uint8_t ST7735S_Init(void) { return 0; }
void LCD_InitTickStart(void) {}
void LCD_InitTickReset(void) {}
uint8_t LCD_InitTick(void) { return 1; }
void LCD_power_off(void) {}
void LCD_power_on(void) {}
void LCD_sleep(void) {}
void LCD_wakeup(void) {}
void LCD_Fill_Color(uint16_t color) { (void)color; }
void LCD_Fill_White(void) {}
void LCD_Service(void) {}
void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	(void)x0;
	(void)y0;
	(void)x1;
	(void)y1;
}
void LCD_DrawImage(const uint8_t *image) { (void)image; }
void LCD_ShowVersionPage(void) {}
void LCD_DrawChar(uint16_t x, uint16_t y, char ch, uint16_t fg, uint16_t bg)
{
	(void)x;
	(void)y;
	(void)ch;
	(void)fg;
	(void)bg;
}
void LCD_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg)
{
	(void)x;
	(void)y;
	(void)str;
	(void)fg;
	(void)bg;
}
void LCD_ShowFwVersion(uint16_t x, uint16_t y)
{
	(void)x;
	(void)y;
}
void LCD_ShowFwVersionInner(uint16_t x, uint16_t y)
{
	(void)x;
	(void)y;
}
void LCD_FillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	(void)x0;
	(void)y0;
	(void)x1;
	(void)y1;
	(void)color;
}
void LCD_DrawBorderFrameEx(uint16_t border_width, uint16_t outer_color, uint16_t inner_color)
{
	(void)border_width;
	(void)outer_color;
	(void)inner_color;
}
void LCD_DrawBorderFrame(void) {}
void LCD_DrawTestGrid(uint8_t rgb_mode) { (void)rgb_mode; }
void LCD_DrawGrid9(void) {}
void lcd_bl_pwm_init(uint8_t brightness) { (void)brightness; }
void lcd_bl_set_brightness(uint8_t brightness) { (void)brightness; }
void lcd_bl_pwm_off(void) {}

#endif /* LCD_MODULE_ENABLE */
