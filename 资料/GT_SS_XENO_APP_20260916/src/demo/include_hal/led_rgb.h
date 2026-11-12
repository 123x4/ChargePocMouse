/*
 * led_rgb.h
 *
 *  Created on:
 *      Author:
 */

#ifndef SRC_DEMO_LED_RGB_LED_RGB_H_
#define SRC_DEMO_LED_RGB_LED_RGB_H_

#include "gtry_type.h"

#define VOLUME_MAX 				11

typedef struct
{
	uint8_t led_bin;
	uint8_t r_cur;
	uint8_t g_cur;
	uint8_t b_cur;
} RGB_pwm;

extern RGB_pwm g_RGB_pwm;

#define LED_BIN 	g_RGB_pwm.led_bin
#define R_CUR 		g_RGB_pwm.r_cur
#define G_CUR 		g_RGB_pwm.g_cur
#define B_CUR 		g_RGB_pwm.b_cur

extern uint8_t LED_space_r, LED_space_g, LED_space_b;
extern uint8_t LED_red;
extern uint8_t LED_green;
extern uint8_t LED_blue;
extern uint8_t color_update;

extern const uint16_t LedVolumeTable[VOLUME_MAX];

extern unsigned char pwm_send;

extern void led_mode_polling(void);

extern void rgb_led_drive(void);

extern void Set_RGB(uint8_t r, uint8_t g, uint8_t b);

extern void rgb_gradient_sof_tick(uint32_t frame_count);

extern void rgb_gradient_polling(void);

#endif /* SRC_DEMO_LED_RGB_LED_RGB_H_ */
