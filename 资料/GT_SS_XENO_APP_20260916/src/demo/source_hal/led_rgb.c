/*
 * led_rgb.c
 *
 *  Created on:
 *      Author:
 */


#include "h6_hw_tab.h"
#include "h6_drv.h"
#include "hasp_drv.h"
#include "led_rgb.h"
#include "h6_hal.h"
#include "app_config.h"

uint8 pwm_send = 0;
uint8 color_index  = 0;
uint8 color_update = 0;

RGB_pwm g_RGB_pwm;

unsigned int LED_r[HASP_KEY_NUM];
unsigned int LED_g[HASP_KEY_NUM];
unsigned int LED_b[HASP_KEY_NUM];

uint8_t LED_space_r, LED_space_g, LED_space_b;
uint8_t LED_red;
uint8_t LED_green;
uint8_t LED_blue;

uint32 timer=0;
uint8 sw=0;

#define RGB_GRADIENT_UPDATE_INTERVAL_MS  10U
#define RGB_GRADIENT_PHASE_COUNT          6U

static volatile uint8_t rgb_gradient_step_pending = 0U;
static uint8_t rgb_gradient_phase = 0U;
static uint8_t rgb_gradient_level = 0U;

const uint16_t LedVolumeTable[VOLUME_MAX] =
{
	0,	2550,	1275,	850,	637,	500,	455,	410,	365,	310,	257
};

void Set_RGB(uint8_t r, uint8_t g, uint8_t b)
{
	hasp_drv_stop_work();
	h6_hal_pwm_multi_write(H6_PWM_LED1_DUTY, r, g_hasp_key_num);

	hasp_drv_stop_work();
	h6_hal_pwm_multi_write(H6_PWM_LED2_DUTY, g, g_hasp_key_num);

	hasp_drv_stop_work();
	h6_hal_pwm_multi_write(H6_PWM_LED3_DUTY, b, g_hasp_key_num);

	hasp_drv_stop_work();
	h6_w_pwm_space_LR(r, g, b);
}

static void rgb_gradient_next_color(void)
{
	switch (rgb_gradient_phase)
	{
		case 0U:
			LED_red = 0xFFU;
			LED_green = rgb_gradient_level;
			LED_blue = 0U;
			break;

		case 1U:
			LED_red = (uint8_t)(0xFFU - rgb_gradient_level);
			LED_green = 0xFFU;
			LED_blue = 0U;
			break;

		case 2U:
			LED_red = 0U;
			LED_green = 0xFFU;
			LED_blue = rgb_gradient_level;
			break;

		case 3U:
			LED_red = 0U;
			LED_green = (uint8_t)(0xFFU - rgb_gradient_level);
			LED_blue = 0xFFU;
			break;

		case 4U:
			LED_red = rgb_gradient_level;
			LED_green = 0U;
			LED_blue = 0xFFU;
			break;

		case 5U:
			LED_red = 0xFFU;
			LED_green = 0U;
			LED_blue = (uint8_t)(0xFFU - rgb_gradient_level);
			break;

		default:
			rgb_gradient_phase = 0U;
			rgb_gradient_level = 0U;
			LED_red = 0xFFU;
			LED_green = 0U;
			LED_blue = 0U;
			break;
	}

	if (rgb_gradient_level == 0xFFU)
	{
		rgb_gradient_level = 0U;
		rgb_gradient_phase++;
		if (rgb_gradient_phase >= RGB_GRADIENT_PHASE_COUNT)
		{
			rgb_gradient_phase = 0U;
		}
	}
	else
	{
		rgb_gradient_level++;
	}
}

void rgb_gradient_sof_tick(uint32_t frame_count)
{
	if ((frame_count % RGB_GRADIENT_UPDATE_INTERVAL_MS) == 0U)
	{
		rgb_gradient_step_pending = 1U;
	}
}

void rgb_gradient_polling(void)
{
	if (rgb_gradient_step_pending == 0U)
	{
		return;
	}

	rgb_gradient_step_pending = 0U;
	rgb_gradient_next_color();
	Set_RGB(LED_red, LED_green, LED_blue);
	h6_drv_w_pwm_time_clr();
}


void rgb_led_drive(void)
{
    if(color_update == 1)
    {
        switch(color_index)
        {
            case 0:
            	if(pwm_send==0 )
            	{
//            		GTRY6xx_SHOW_DBG_H;
					for(unsigned int i = 0; i < HASP_KEY_NUM; i++)
					{
						if(chain_miss_buff_led[i] == 0)
						{

							GTRY_HASP->TXDATA[i] = 0x5555;
						}
						else
						{
							GTRY_HASP->TXDATA[i] = H6_PWM_LED3_DUTY | LED_r[i];
						}
					}
					color_index = 1;
					pwm_send = 1;
//					GTRY6xx_SHOW_DBG_L;
            	}
                break;

            case 1:
            	if(pwm_send==0)
            	{
            		for(unsigned int i = 0; i < HASP_KEY_NUM; i++)
					{
                    	if(chain_miss_buff_led[i] == 0)
                    	{
                    		GTRY_HASP->TXDATA[i] = 0x5555;
                    	}
                    	else
                    	{
                    		GTRY_HASP->TXDATA[i] = H6_PWM_LED2_DUTY | LED_g[i];
                    	}
					}
					color_index = 2;
					pwm_send = 1;
            	}
                break;

            case 2:
              	if(pwm_send==0)
                {
					for(unsigned int i = 0; i < HASP_KEY_NUM; i++)
					{
	                	if(chain_miss_buff_led[i] == 0)
	                	{
	                		GTRY_HASP->TXDATA[i] = 0x5555;
	                	}
	                	else
	                	{
	                		GTRY_HASP->TXDATA[i] = H6_PWM_LED1_DUTY | LED_b[i];
	                	}
					}
					color_index = 3;
	                pwm_send =1;
                }
                break;

			default:
				break;
        }
        if(color_index==3)
        {
        	color_index=0;
        	color_update=0;
        }
    }
}


void led_mode_polling(void)
{
	uint32_t timer_us ;
	timer_us = 2500 * 50;

	if(timer >timer_us)
    {
        timer = 0;
        switch(sw)
		{
			case 0:
				for(unsigned int i = 0; i < HASP_KEY_NUM; i++)
				{
					LED_r[i] = 128;
					LED_g[i] = 0;
					LED_b[i] = 0;
				}
				sw = 1;
				break;

			case 1:
				for(unsigned int i = 0; i < HASP_KEY_NUM; i++)
				{
					LED_r[i] = 0;
					LED_g[i] = 128;
					LED_b[i] = 0;
				}
				sw = 2;
				break;

			case 2:
				for(unsigned int i = 0; i < HASP_KEY_NUM; i++)
				{
					LED_r[i] = 0;
					LED_g[i] = 0;
					LED_b[i] = 128;
				}
				sw = 0;
				break;
		}
        color_update = 1;
    }
    else
    {
        timer++;
    }
}

void led_mode_turn_OFF(void)
{
	color_update = 1;
	for(unsigned int i = 0; i < HASP_KEY_NUM; i++)
	{
		LED_r[i] = 0;
		LED_g[i] = 0;
		LED_b[i] = 0;
	}
}
