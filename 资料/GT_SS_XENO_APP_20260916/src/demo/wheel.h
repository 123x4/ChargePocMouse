/*
 * Copyright (C) 2026 G.TECH TECHNOLOGY LTD. All rights reserved.
 * All rights reserved.
 */

#ifndef SRC_DEMO_WHEEL_H_
#define SRC_DEMO_WHEEL_H_

/*--------------------------------------------------------------------+
 * Constants*/
#define L_encoder_BTN		PB9
#define L_encoder_A			PB10
#define L_encoder_B			PB11

#define R_encoder_A			PB8
#define R_encoder_B			PB7
#define R_encoder_BTN		PB6

#define LCD_COLOR_COUNT 	4

/*--------------------------------------------------------------------+
 * Extern Variables
 *--------------------------------------------------------------------*/
extern unsigned int LED_r[];
extern unsigned int LED_g[];
extern unsigned int LED_b[];

/*--------------------------------------------------------------------+
 * Type Definitions
 *--------------------------------------------------------------------*/
typedef struct
{
	uint8_t last_state[2];
	int8_t delta;
}WHEEL_LAST_DATA;


/*--------------------------------------------------------------------+
 * Public Function Prototypes
 *--------------------------------------------------------------------*/
extern void wheel_init(void);
extern void scan_wheel_A(void);
extern void scan_wheel_B(void);
extern void wheel_set_encoder_test_mode(uint8_t enable);
extern uint8_t wheel_scan_enabled(void);

#endif /* SRC_DEMO_WHEEL_H_ */
