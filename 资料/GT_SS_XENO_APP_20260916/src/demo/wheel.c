/*
 * Copyright (C) 2026 G.TECH TECHNOLOGY LTD. All rights reserved.
 * All rights reserved.
 */


/* Includes ------------------------------------------------------------------*/
#include "app_config.h"

#if WHEEL_MODULE_ENABLE

#include <stdint.h>
#include <stdio.h>
#include <wheel.h>
#include "gtry_platform.h"
#include "gtry_gpio_drv.h"
#include <stddef.h>
#include <stdbool.h>
#include "gtry_drv_common.h"
#include "TFT.h"
#include "string.h"
#include "mfg_mode.h"
#include "led_rgb.h"
#include "h6_hw_tab.h"
#include "test_image.h"
#include "usb_descriptors.h"

extern uint8_t stop_hall_sample_sof_single;

/*--------------------------------------------------------------------+
 * Type Definitions
 *--------------------------------------------------------------------*/
WHEEL_LAST_DATA wheel_state;
WHEEL_LAST_DATA wheel_state_B;

uint8_t current_wheel_state;
uint8_t wheel_scand;
uint8_t wheel_debounce;
uint8_t wheel_Press = 0;

uint8_t current_wheel_state_B;
uint8_t wheel_scand_B;
uint8_t wheel_debounce_B;
uint8_t wheel_Press_B = 0;
// 0 = RED, 1 = GREEN, 2 = BLUE
static uint8_t wheel_color_index_A  = 3;
// 0 = RED, 1 = GREEN, 2 = BLUE, 3 = WHITE
static uint8_t lcd_color_index_A 	= 3;
static uint8_t encoder_test_mode_en = 0;

#define BTN_SCAN_PRESCALER   8
#define BTN_DEBOUNCE_TH      16

#define ENCODER_TEST_REPORT_ID                REPORT_ID_CONSUMER_CONTROL
#define ENCODER_TEST_WHEEL_A_LEFT_KEYCODE     0xEA
#define ENCODER_TEST_WHEEL_A_RIGHT_KEYCODE    0xE9
#define ENCODER_TEST_WHEEL_A_PRESS_KEYCODE    0xE2
#define ENCODER_TEST_WHEEL_B_PREV_KEYCODE     0xB6
#define ENCODER_TEST_WHEEL_B_NEXT_KEYCODE     0xB5
#define ENCODER_TEST_WHEEL_B_PRESS_KEYCODE    0xCD

uint8_t wheel_get_wheel_state_A(void);
uint8_t wheel_get_wheel_state_B(void);

static void wheel_set_all_led(uint8_t r, uint8_t g, uint8_t b)
{
	LED_red   = r;
	LED_green = g;
	LED_blue  = b;

	LED_space_r = r;
	LED_space_g = g;
	LED_space_b = b;

	color_update = 1;
}

/*
 * Reset the cached wheel states so mode changes do not create false
 * rotation events on the next scan.
 */
static void wheel_reset_runtime_state(void)
{
	uint8_t init_a = wheel_get_wheel_state_A();

	wheel_state.last_state[0] = init_a;
	wheel_state.last_state[1] = init_a;
	wheel_state.delta = 0;
	wheel_scand = 0;
	wheel_debounce = 0;
	wheel_Press = 0;

	if (g_keyboard_type != KB_TYPE_65_PERCENT)
	{
		uint8_t init_b = wheel_get_wheel_state_B();

		wheel_state_B.last_state[0] = init_b;
		wheel_state_B.last_state[1] = init_b;
	}
	else
	{
		wheel_state_B.last_state[0] = 0;
		wheel_state_B.last_state[1] = 0;
	}

	wheel_state_B.delta = 0;
	wheel_scand_B = 0;
	wheel_debounce_B = 0;
	wheel_Press_B = 0;
}

/*
 * Send one encoder-test response packet:
 *   Byte0 = 0x04
 *   Byte1 = 0x2A
 *   Byte2 = 0x05 (media key report ID)
 *   Byte3 = media keycode
 */
static void wheel_send_encoder_test_key(uint8_t keycode)
{
	if (!encoder_test_mode_en)
	{
		return;
	}

	memset(vendor_buf_in, 0, VENDOR_REPORT_SIZE);
	vendor_buf_in[0] = BYTE0_HEADER04;
	vendor_buf_in[1] = BYTE1_ENCODER_TEST;
	vendor_buf_in[2] = ENCODER_TEST_REPORT_ID;
	vendor_buf_in[3] = keycode;
	send_vendor();
}

/*
 * In manufacturing test mode, the normal wheel UI actions must stay disabled.
 * Only the dedicated encoder test command (0x04 0x2A) is allowed to emit
 * wheel events.
 */
static uint8_t wheel_normal_action_blocked(void)
{
	if ((mfg_mode_cmd == 0) && !encoder_test_mode_en)
	{
		wheel_reset_runtime_state();
		return 1;
	}

	return 0;
}

uint8_t wheel_get_wheel_state_A(void)
{
	uint8_t get_wheel_status;
	get_wheel_status = (gpio_in_read(L_encoder_A) | (gpio_in_read(L_encoder_B) << 1));
	return get_wheel_status;
}

uint8_t wheel_get_wheel_state_B(void)
{
	uint8_t get_wheel_status;
	get_wheel_status = (gpio_in_read(R_encoder_A) | (gpio_in_read(R_encoder_B) << 1));
	return get_wheel_status;
}

void wheel_init(void)
{
	wheel_reset_runtime_state();
}


void wheel_set_encoder_test_mode(uint8_t enable)
{
	encoder_test_mode_en = enable ? 1 : 0;
	wheel_reset_runtime_state();
}

uint8_t wheel_scan_enabled(void)
{
	if (mfg_mode_cmd != 0)
	{
		return 1;
	}

	return encoder_test_mode_en;
}

void scan_wheel_A(void)
{
	if (wheel_normal_action_blocked())
	{
		return;
	}

	if (g_keyboard_type == KB_TYPE_65_PERCENT)
	{
		uint8_t state;
		uint8_t last_state;
		uint8_t prev_last_state;
		uint8_t encoder_event_keycode = 0;

		state = wheel_get_wheel_state_A();
		last_state = wheel_state.last_state[1];

		if(++wheel_scand > BTN_SCAN_PRESCALER)
		{
			wheel_scand = 0;
			if(!gpio_in_read(L_encoder_BTN))
			{
				wheel_debounce++;
				if(wheel_debounce > BTN_DEBOUNCE_TH)
				{
					wheel_debounce = 0;
					if(wheel_Press == 0)
					{
						wheel_Press = 1;
						if (encoder_test_mode_en)
						{
							wheel_send_encoder_test_key(ENCODER_TEST_WHEEL_A_PRESS_KEYCODE);
						}
						else
						{
							wheel_set_all_led(0xFF, 0xFF, 0xFF);
						}
					}
				}
			}
			else
			{
				wheel_debounce = 0;
				if(wheel_Press)
				{
					wheel_Press = 0;
				}
			}
		}

		if( state == last_state )
		{
			return ;
		}
		else
		{
			prev_last_state = wheel_state.last_state[0];

			wheel_state.last_state[0] = last_state;
			wheel_state.last_state[1] = state;

			if((prev_last_state == 0) && (state == 3))
			{
				if( last_state == 1 )
				{
					++wheel_state.delta;
					if (encoder_test_mode_en)
					{
						encoder_event_keycode = ENCODER_TEST_WHEEL_A_RIGHT_KEYCODE;
					}
					else
					{
						wheel_color_index_A = (wheel_color_index_A + 1) % 3;
					}
				}
				else if( last_state == 2 )
				{
					--wheel_state.delta;
					if (encoder_test_mode_en)
					{
						encoder_event_keycode = ENCODER_TEST_WHEEL_A_LEFT_KEYCODE;
					}
					else
					{
						wheel_color_index_A = (wheel_color_index_A + 2) % 3;
					}
				}
			}
			else if((prev_last_state == 3) && (state == 0))
			{
				if(last_state == 1)
				{
					--wheel_state.delta;
					if (encoder_test_mode_en)
					{
						encoder_event_keycode = ENCODER_TEST_WHEEL_A_LEFT_KEYCODE;
					}
					else
					{
						wheel_color_index_A = (wheel_color_index_A + 2) % 3;
					}
				}
				else if(last_state == 2)
				{
					++wheel_state.delta;
					if (encoder_test_mode_en)
					{
						encoder_event_keycode = ENCODER_TEST_WHEEL_A_RIGHT_KEYCODE;
					}
					else
					{
						wheel_color_index_A = (wheel_color_index_A + 1) % 3;
					}
				}
			}

			if (encoder_event_keycode != 0)
			{
				wheel_send_encoder_test_key(encoder_event_keycode);
				return;
			}

			if (encoder_test_mode_en)
			{
				return;
			}

			{
				switch (wheel_color_index_A)
				{
					case 0:
						wheel_set_all_led(0xFF, 0x00, 0x00);
						break;
					case 1:
						wheel_set_all_led(0x00, 0xFF, 0x00);
						break;
					case 2:
						wheel_set_all_led(0x00, 0x00, 0xFF);
						break;
					default:
						break;
				}
			}
		}
	}
	else//g_keyboard_type == KB_TYPE_TKL / KB_TYPE_96_PERCENT / KB_TYPE_FULL_SIZE
	{
		uint8_t state;
		uint8_t last_state;
		uint8_t prev_last_state;
		uint8_t encoder_event_keycode = 0;

		state = wheel_get_wheel_state_A();
		last_state = wheel_state.last_state[1];

		if(++wheel_scand > BTN_SCAN_PRESCALER)
		{
			wheel_scand = 0;
			if(!gpio_in_read(L_encoder_BTN))
			{
				wheel_debounce++;
				if(wheel_debounce > BTN_DEBOUNCE_TH)
				{
					wheel_debounce = 0;
					if(wheel_Press == 0)
					{
						wheel_Press = 1;
						if (encoder_test_mode_en)
						{
							wheel_send_encoder_test_key(ENCODER_TEST_WHEEL_A_PRESS_KEYCODE);
						}
						else
						{
							// LCD_Fill_Color(BLACK);
							LCD_DrawImage(gImage_test_image);
						}
					}
				}
			}
			else
			{
				wheel_debounce = 0;
				if(wheel_Press)
				{
					wheel_Press = 0;
	//				LCD_Fill_Color(WHITE);
				}
			}
		}

		if( state == last_state )
		{
			return ;
		}
		else
		{
			prev_last_state = wheel_state.last_state[0];

			wheel_state.last_state[0] = last_state;
			wheel_state.last_state[1] = state;

			if((prev_last_state == 0) && (state == 3))
			{
				if( last_state == 1 )
				{
					++wheel_state.delta;
					if (encoder_test_mode_en)
					{
						encoder_event_keycode = ENCODER_TEST_WHEEL_A_RIGHT_KEYCODE;
					}
					else
					{
						/* Only redraw after a confirmed quadrature step changes the
						 * selected LCD page/color. Half-step edges must not repaint. */
						lcd_color_index_A = (lcd_color_index_A + 1) % LCD_COLOR_COUNT;
					}
				}
				else if( last_state == 2 )
				{
					--wheel_state.delta;
					if (encoder_test_mode_en)
					{
						encoder_event_keycode = ENCODER_TEST_WHEEL_A_LEFT_KEYCODE;
					}
					else
					{
						/* Only redraw after a confirmed quadrature step changes the
						 * selected LCD page/color. Half-step edges must not repaint. */
						lcd_color_index_A = (lcd_color_index_A - 1 + LCD_COLOR_COUNT) % LCD_COLOR_COUNT;
					}
				}
			}
			else if((prev_last_state == 3) && (state == 0))
			{
				if(last_state == 1)
				{
					--wheel_state.delta;
					if (encoder_test_mode_en)
					{
						encoder_event_keycode = ENCODER_TEST_WHEEL_A_LEFT_KEYCODE;
					}
					else
					{
						/* Only redraw after a confirmed quadrature step changes the
						 * selected LCD page/color. Half-step edges must not repaint. */
						lcd_color_index_A = (lcd_color_index_A - 1 + LCD_COLOR_COUNT) % LCD_COLOR_COUNT;
					}
				}
				else if(last_state == 2)
				{
					++wheel_state.delta;
					if (encoder_test_mode_en)
					{
						encoder_event_keycode = ENCODER_TEST_WHEEL_A_RIGHT_KEYCODE;
					}
					else
					{
						/* Only redraw after a confirmed quadrature step changes the
						 * selected LCD page/color. Half-step edges must not repaint. */
						lcd_color_index_A = (lcd_color_index_A + 1) % LCD_COLOR_COUNT;
					}
				}
			}

			if (encoder_event_keycode != 0)
			{
				wheel_send_encoder_test_key(encoder_event_keycode);
				return;
			}

			if (encoder_test_mode_en)
			{
				return;
			}

			switch (lcd_color_index_A)
			{
				case 0:
					LCD_Fill_Color(RED);
					break;
				case 1:
					LCD_Fill_Color(GREEN);
					break;
				case 2:
					LCD_Fill_Color(BLUE);
					break;
				case 3:
					LCD_ShowVersionPage(); /* Queue a fully async version page so encoder scanning is not blocked by text drawing. */
					break;
				default:
					break;
			}
		}
	}
}


void scan_wheel_B(void)
{
	if (wheel_normal_action_blocked())
	{
		return;
	}

	uint8_t state;
	uint8_t last_state;
	uint8_t prev_last_state;
	uint8_t encoder_event_keycode = 0;

	state = wheel_get_wheel_state_B();
	last_state = wheel_state_B.last_state[1];

	if(++wheel_scand_B > BTN_SCAN_PRESCALER)
	{
		wheel_scand_B = 0;
		if(!gpio_in_read(R_encoder_BTN))
		{
			wheel_debounce_B++;
			if(wheel_debounce_B > BTN_DEBOUNCE_TH)
			{
				wheel_debounce_B = 0;
				if(wheel_Press_B == 0)
				{
					wheel_Press_B = 1;
					if (encoder_test_mode_en)
					{
						wheel_send_encoder_test_key(ENCODER_TEST_WHEEL_B_PRESS_KEYCODE);
					}
					else
					{
						wheel_set_all_led(0xFF, 0xFF, 0xFF);
					}
				}
			}
		}
		else
		{
			wheel_debounce_B = 0;
			if(wheel_Press_B)
			{
				wheel_Press_B = 0;
			}
		}
	}

	if( state == last_state )
	{
		return ;
	}
	else
	{
		prev_last_state = wheel_state_B.last_state[0];

		wheel_state_B.last_state[0] = last_state;
		wheel_state_B.last_state[1] = state;

		if((prev_last_state == 0) && (state == 3))
        {
            if( last_state == 1 )
            {
                ++wheel_state.delta;
                if (encoder_test_mode_en)
                {
                	encoder_event_keycode = ENCODER_TEST_WHEEL_B_NEXT_KEYCODE;
                }
                else
                {
                	wheel_color_index_A = (wheel_color_index_A + 1) % 3;
                }
            }
            else if( last_state == 2 )
            {
                --wheel_state.delta;
                if (encoder_test_mode_en)
                {
                	encoder_event_keycode = ENCODER_TEST_WHEEL_B_PREV_KEYCODE;
                }
                else
                {
                	wheel_color_index_A = (wheel_color_index_A + 2) % 3;
                }
            }
        }
        else if((prev_last_state == 3) && (state == 0))
        {
            if(last_state == 1)
            {
                --wheel_state.delta;
                if (encoder_test_mode_en)
                {
                	encoder_event_keycode = ENCODER_TEST_WHEEL_B_PREV_KEYCODE;
                }
                else
                {
                	wheel_color_index_A = (wheel_color_index_A + 2) % 3;
                }
            }
            else if(last_state == 2)
            {
                ++wheel_state.delta;
                if (encoder_test_mode_en)
                {
                	encoder_event_keycode = ENCODER_TEST_WHEEL_B_NEXT_KEYCODE;
                }
                else
                {
                	wheel_color_index_A = (wheel_color_index_A + 1) % 3;
                }
            }
        }

		if (encoder_event_keycode != 0)
		{
			wheel_send_encoder_test_key(encoder_event_keycode);
			return;
		}

		if (encoder_test_mode_en)
		{
			return;
		}

		switch (wheel_color_index_A)
		{
			case 0:
				wheel_set_all_led(0xFF, 0x00, 0x00);
				break;
			case 1:
				wheel_set_all_led(0x00, 0xFF, 0x00);
				break;
			case 2:
				wheel_set_all_led(0x00, 0x00, 0xFF);
				break;
			case 3:
				break;
			default:
				break;
		}
	}
}

#else

#include <stdint.h>
#include <wheel.h>

void wheel_init(void)
{
	/* Keep the module linkable while the wheel feature is compiled out. */
}

void scan_wheel_A(void)
{
	/* Keep callers harmless when WHEEL_MODULE_ENABLE is disabled. */
}

void scan_wheel_B(void)
{
	/* Keep callers harmless when WHEEL_MODULE_ENABLE is disabled. */
}

void wheel_set_encoder_test_mode(uint8_t enable)
{
	(void)enable;                         /* The encoder test command has no effect without wheel hardware. */
}

uint8_t wheel_scan_enabled(void)
{
	return 0;                             /* Disabled modules must never request periodic wheel scanning. */
}

#endif /* WHEEL_MODULE_ENABLE */






























