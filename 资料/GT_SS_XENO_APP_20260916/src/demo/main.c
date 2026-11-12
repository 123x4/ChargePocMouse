/*
 * Copyright (c) 2019 Ruiyi Micro Corporation
 * All rights reserved.
 *
 */

#include "gtry_platform.h"
#include "gtry_smu.h"
#include "gtry_uart_drv.h"
#include "gtry_gpio_drv.h"
#include "gtry_timer_drv.h"
#include "gtry_wdt_drv.h"
#include "gtry_rtc_drv.h"
#include "gtry_ltimer_drv.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "musb_type.h"
#include "h6_hw_tab.h"
#include "h6_drv.h"
#include "hasp_drv.h"
#include "h6_hal.h"
#include "led_rgb.h"
#include "mfg_mode.h"
#include "app_config.h"

#if WHEEL_MODULE_ENABLE
#include "wheel.h"
#endif

#if LCD_MODULE_ENABLE
#include "TFT.h"
#endif

#include "keyboard.h"
#include "Puya_Flash.h"

#if 0
unsigned char usb_kb_Lreport[REPORT_SIZE] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
unsigned char usb_kb_report[REPORT_SIZE] = {0x0,0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

unsigned char *cur_report = &usb_kb_report[OFF_SET_KEY];
unsigned char *last_report = &usb_kb_Lreport[OFF_SET_KEY];
unsigned char *tmp_report = &usb_kb_Lreport[OFF_SET_KEY];
#endif

unsigned char usb_wakeup =0;

uint8_t kb_report_done = 0;
uint8_t data_report_done = 0;

unsigned int sys_mode = SYS_MODE_USB ;

volatile unsigned char my_sys_freq = 0;

uint8 stop_hall_sample_sof_single = 1;

static RY_DRIVER_UART *uart_drv = NULL;

void uart_print_init(RYRV_UART_NO uart_no, RY_UART_BD baud)
{
#if (GTRY_UART0_EN)
	if(uart_no == UART_NO0)
	{
		//PB8 -> TX
		GTRY_GPIO->GPB_CFG[8] |= 0x40000;
		uart_drv = &Driver_UART0;
	}
#endif
#if (GTRY_UART1_EN)
	if(uart_no == UART_NO1)
	{
		//PB12 -> TX
		GTRY_GPIO->GPB_CFG[12] |= 0x40000;
		uart_drv = &Driver_UART1;
	}
#endif
#if (GTRY_UART2_EN)
	if(uart_no == UART_NO2)
	{
		//PB4 -> TX
		GTRY_GPIO->GPB_CFG[4] |= 0x40000;
		uart_drv = &Driver_UART2;
	}
#endif
	uart_drv->Initialize(NULL/*uart_callback*/);
	uart_drv->PowerControl(RY_POWER_FULL);
	uart_drv->Control(RY_UART_MODE_ASYNCHRONOUS |
			RY_UART_DATA_BITS_8 | RY_UART_PARITY_NONE | RY_UART_STOP_BITS_1 |
			RY_UART_FLOW_CONTROL_NONE, baud);
    /* Enable Receiver and Transmitter lines */
	uart_drv->Control(RY_UART_CONTROL_TX, 1);
	uart_drv->Control(RY_UART_CONTROL_RX, 0);
}

unsigned char sys_mode_get(void)
{
	return	sys_mode;
}
void sys_mode_set(uint8_t mode)
{
	sys_mode = mode ;
}

//--------------------------------------------------------------------+
// Forward USB interrupt events to TinyUSB IRQ Handler
//--------------------------------------------------------------------+
void usb_irq_handler(void) {
#if CFG_TUH_ENABLED
  tuh_int_handler(0, true);
#endif

#if CFG_TUD_ENABLED
  tud_int_handler(0);
#endif
}

void H6_enter_sleep(void)
{
	hasp_drv_stop_work();
	hasp_drv_td_set(HASP_TD2 , H6_CMD_TIMES_TD7);
	GTRY_HASP->NSCK = 0;
	GTRY_HASP->START = 1;
	ry_delay_ms(1);
}

uint8 pwm_done = 0;
uint8 hall_timing_change = 1;
void switch_pwm_clk(void)
{
	#if PWM_ENABLE_MD
		if(pwm_send==1)
		{
			if(pwm_done==0)
			{
				pwm_done = 1;
				if(hall_timing_change == 0)
				{
					h6_hal_variable_time_inst(1);//sys_freq
					hall_timing_change =1;
				}
			}
			else
			{
				pwm_send = 0;
				pwm_done = 0;
			}

		}
		if(pwm_send==0)
		{
			if(hall_timing_change ==1)
			{
				h6_hal_variable_time_inst(0);
				hall_timing_change =0;
			}
		}
	#endif
}

void H6_reset(void);
void hall_sample_start(void)
{
	if(stop_hall_sample_sof_single)
	{
//		switch_pwm_clk();
		timer_start(TC_NUMBER0);
		hasp_drv_start_work_not_wait();
	}
//	check_kb_report_update();
}
/* Timer interrupt callback function */
static void timer_callback(uint32 event)
{

	if(stop_hall_sample_sof_single)
	{
		if(mfg_mode_cmd)
		{
			check_kb_report_update();
		}
		timer_trigger_clear(TC_NUMBER0);
	}
}
uint32_t ltimer_int_st_my[2] = {0};

//static void ltimer_irq_handler_my(uint32_t ltimer_no)
//{
//	ltimer_int_st_my[ltimer_no] = 0x0;
//	if(GTRY_LTIM0->LTIM_COMPARE[0] && (GTRY_LTIM0->LTIM_INT_EN&LTIMER_INT_CMP0_MASK))
//	{
//		GTRY_LTIM0->LTIM_COMPARE[0] = 1;
//		ltimer_int_st_my[ltimer_no] |= LTIMER_INT_CMP0_MASK;
//		GTRY_LTIM0->LTIM_CC[LTIMER_NUMBER0] =LTIMER_CC_TH;
//		GTRY_LTIM0->LTIM_CLEAR = 0x1;
//	}
//}
//
const unsigned char hasp_delay_us[3]=
{//     200M,	96M,	48M
		108, 	110,	107,  //116
};
const unsigned char hasp_delay_counts[3]=
{//     200M,	96M,	48M
		50,		47,		23,   //50
};

/*Timer initialization*/
void timer_init_my(void)
{
	//timer_compare_value is measured with a logic analyzer
	uint32_t timer_compare_value = (hasp_delay_us[my_sys_freq] * hasp_delay_counts[my_sys_freq]/10);

	timer_init(TC_NUMBER0, 20, RY_TIMER_TIMING, RY_TIMER_16BIT);
	timer_int_init(TC_NUMBER0, TIMER_INT_CC0_MASK, timer_callback);
	timer_set_ccn(TC_NUMBER0, TIMER_CC0, timer_compare_value);
	timer_set_shorts(TC_NUMBER0, TIMER_CC0, TIMER_SHORT_CC_STOP_MASK); //Single trigger mode
}

//  Ltimer interrupt callback function but please  ( ltimer_irq_handler )
void ltimer_wakeup_cb(uint32_t ltimer_no, uint32_t intst)
{

}

/*Ltimer init*/
void ltimer_init(void)
{
	ltimer_cfg(LTIMER_NUMBER0, 33, TICK_ENABLE);
	ltimer_set_ccn(LTIMER_NUMBER0, LTIMER_CC0, 260);
	ltimer_int_init(LTIMER_NUMBER0, LTIMER_INT_NULL_MASK, NULL);
	ltimer_int_set(LTIMER_NUMBER0, LTIMER_INT_CMP0_MASK);
}

/* HASP interrupt callback */
void hasp_int_callback_my(uint32_t int_state)
{
	if(int_state & (CRC_ERR_INT_MASK ))//| DATA_ERR_INT_MASK
	{
		GTRY_HASP->INTEN &= ~(CRC_ERR_INT ) ;//| DATA_ERR_INT
		stop_hall_sample_sof_single = 0;
	}

	if(int_state & KEY_BUSY_INT_MASK)
	{
//			check_kb_report_update();
	}
}

/*  Wireless mode working and sleeping switching  */
void wireless_work_deepsleep_switch(void)
{
	/*wireless example*/

	//Add wireless mode code from here
	//...
	//End

//	GTRY_HASP->INTST |= HASP_INT_ALL_MASK;
	smu_enter_dpsleep_quickly();
//	GTRY_HASP->START = 1;

	//Add wireless mode code from here
	//...
	//End
}

void board_init(void)
{
	smu_power_vddiob_config(POWER_VDDIOB_INTERNAL_OFF, POWER_VDDIOB_LDO_DEF);
	smu_vddiob_dischg();

	if(sys_mode_get() & SYS_MODE_USB)
	{
		smu_sysclk_init(SMU_SYSCLK_200MHZ);//SMU_SYSCLK_200MHZ
		my_sys_freq = SMU_SYSCLK_200MHZ;
		h6_hal_set_sys_clk(SMU_SYSCLK_200MHZ);

		smu_periph_diable(
							SMU_PCLK_SPI0_EN_MASK|
							SMU_PCLK_SPI2_EN_MASK|
							SMU_PCLK_SPI3_EN_MASK|
							SMU_PCLK_SPI4_EN_MASK|
							SMU_PCLK_SPI6_EN_MASK|
							SMU_PCLK_I2C0_EN_MASK|
							SMU_PCLK_I2C1_EN_MASK|

							SMU_PCLK_RGB_EN_MASK|
							SMU_PCLK_DVP_EN_MASK|
							SMU_PCLK_R8080_EN_MASK
							);
		smu_periph_enable(
							SMU_PCLK_SPI1_EN_MASK |
							SMU_PCLK_SPI5_EN_MASK |
							SMU_PCLK_UART0_EN_MASK|
							SMU_PCLK_UART1_EN_MASK|
							SMU_PCLK_UART2_EN_MASK|
							SMU_PCLK_TIMER_EN_MASK|
							SMU_PCLK_GPIO_EN_MASK|
							SMU_PCLK_HASP_EN_MASK|
							SMU_PCLK_USB_EN_MASK|
							SMU_PCLK_WDT_EN_MASK|
							SMU_PCLK_PWM_EN_MASK
							);
	}

	smu_32kclk_init(SMU_32K_ILO, SMU_XTAL32K_REUSE_NULL);
//	smu_usbphy_powerdown(1);
	smu_vddiob_dischg();

	#if 1 //base on board power design
		gpio_out_clear(HALL_POWER_IO);
		gpio_cfg_output(HALL_POWER_IO, RY_GPIO_DRIVE4);
		gpio_out_set(HALL_POWER_IO);
	#endif

	#if 1 //LED
		gpio_out_clear(LED_EN);
		gpio_cfg_output(LED_EN, RY_GPIO_DRIVE4);
		gpio_out_set(LED_EN);

		gpio_out_clear(LED_LCD_EN);
		gpio_cfg_output(LED_LCD_EN, RY_GPIO_DRIVE4);
		gpio_out_set(LED_LCD_EN);
	#endif

#if LCD_MODULE_ENABLE
	gpio_out_clear(LED_LCD_EN);
	gpio_cfg_output(LED_LCD_EN, RY_GPIO_DRIVE4);
	gpio_out_set(LED_LCD_EN);

	gpio_cfg_output(LCD_3V9, RY_GPIO_DRIVE4);
	gpio_cfg_output(LCD_3V3, RY_GPIO_DRIVE4);
	gpio_cfg_output(LCD_DC, RY_GPIO_DRIVE4);
	gpio_cfg_output(OLED_RES, RY_GPIO_DRIVE4);

	gpio_out_set(LCD_3V9);
	gpio_out_set(LCD_3V3);
	gpio_out_set(LCD_DC);
	gpio_out_set(OLED_RES);
#endif

#if WHEEL_MODULE_ENABLE
	gpio_cfg_input(L_encoder_BTN, RY_GPIO_PULLUP_5K);
	gpio_cfg_input(L_encoder_A, RY_GPIO_NOPULL);
	gpio_cfg_input(L_encoder_B, RY_GPIO_NOPULL);

	gpio_cfg_input(R_encoder_BTN, RY_GPIO_PULLUP_5K);
	gpio_cfg_input(R_encoder_A, RY_GPIO_NOPULL);
	gpio_cfg_input(R_encoder_B, RY_GPIO_NOPULL);
#endif

	gpio_out_clear(TEST_DEBUG_IO_0);
	gpio_cfg_output(TEST_DEBUG_IO_0, RY_GPIO_DRIVE4);

	gpio_out_clear(TEST_DEBUG_IO_1);
	gpio_cfg_output(TEST_DEBUG_IO_1, RY_GPIO_DRIVE4);

	gpio_out_clear(TEST_DEBUG_IO_2);
	gpio_cfg_output(TEST_DEBUG_IO_2, RY_GPIO_DRIVE4);

	gpio_out_clear(TEST_DEBUG_IO_3);
	gpio_cfg_output(TEST_DEBUG_IO_3, RY_GPIO_DRIVE4);

//	gpio_cfg_input(RESET_PIN, RY_GPIO_PULLUP_30K);

	//Use a logic analyzer to verify the accuracy of 200MHz through debugging the PIN (mass production code is masked)
	GTRY6xx_SHOW_DBG_H_0;
	ry_delay_ms(1);
	GTRY6xx_SHOW_DBG_L_0;

	GTRY6xx_SHOW_DBG_H_1;
	ry_delay_us(1000);
	GTRY6xx_SHOW_DBG_L_1;

	GTRY6xx_SHOW_DBG_H_2;
	ry_delay_us(1000);
	GTRY6xx_SHOW_DBG_L_2;

	GTRY6xx_SHOW_DBG_H_3;
	ry_delay_ms(1);
	GTRY6xx_SHOW_DBG_L_3;
}

/* USB initialization configuration */
uint32 wdt_reset_counter = 1000;
#if WDT_TEST_MODE
	void wdt_hal_cfg(void)
	{
		wdt_cfg(RY_WDT_EXPIRED_TIME_1s, RY_WDT_RST_TIME_500ms, WDT_RST_EN);
		wdt_enable();
	}
	void wdt_hal_feed_dog(void)
	{
		if(wdt_reset_counter -- ==0)
		{
			wdt_reset_timer();
			wdt_reset_counter = 1000;
		}
	}
#endif

void usb_init(void)
{
	#if USB_ENABLE
		smu_periph_diable(SMU_PCLK_USB_EN_MASK);
		ry_delay_ms(5);
		smu_periph_enable(SMU_PCLK_USB_EN_MASK);
		ry_delay_ms(10);

		tusb_rhport_init_t dev_init = {.role = TUSB_ROLE_DEVICE,
										.speed = TUSB_SPEED_AUTO
										};
		tusb_init(BOARD_TUD_RHPORT, &dev_init);

#if LCD_MODULE_ENABLE
		uint8_t lcd_init_done = 0;          /* Track LCD init while USB enumeration is still pending. */
		LCD_InitTickStart();                /* Start the LCD state machine once so command delays stay non-blocking. */
#endif

		while(tud_mounted() == false)
		{
			wdt_hal_feed_dog();
			tud_task();
			ry_delay_us(5);
		}

#if LCD_MODULE_ENABLE
		while(lcd_init_done == 0)
		{
			wdt_hal_feed_dog();             /* Keep WDT serviced if USB mounted before the LCD init sequence finished. */
			tud_task();                     /* Continue servicing USB while waiting for LCD command-delay deadlines. */
			lcd_init_done = LCD_InitTick(); /* Finish the LCD init before later code writes pixels to the panel. */
		}

#if LCD_RDDID_DEBUG_ENABLE
		LCD_ReadDisplayIdAndPrint();       /* Read the serial dummy clock plus ID1/ID2/ID3 and print diagnostics on UART1. */
#endif

#endif

	#endif
}

/* h6 soft reset */
void H6_reset(void)
{
	if(stop_hall_sample_sof_single == 0)
	{
		printf("H6_reset_sign\r\n");
		h6_hal_init(hasp_int_callback_my);
		wdt_reset_timer();
		stop_hall_sample_sof_single = 1;
	}
}

/* For temporary use, to be optimized later */
void low_32K_osc_calibration(void)
{
	/****************32K low-frequency clock calibration****************/
	GTRY_SMU->SMU_XTAL_ILO_CFG &= ~0x3f;	//Debug Ltimer cycle time    , MAX ->3F
	GTRY_SMU->SMU_XTAL_ILO_CFG |=  0x2D;	//The larger the value, the shorter the time
}

void h6_app_set_mode(void)
{
	unsigned int i;
	uint16_t hall_wrdata[HASP_KEY_NUM];

	for(unsigned int i = 0; i < g_hasp_key_num; i++)
	{
		if(i < KEY_BLUETOOTH_MODETONS_NUM)
		{
			hall_wrdata[i] = 0x1F;
		}
		else
		{
			hall_wrdata[i] = 0xFA;
		}
	}
	h6_drv_write_reg_cmd(CONGFIG_REGF, hall_wrdata, g_hasp_key_num);

	for(i = 0; i < g_hasp_key_num; i++)
	{
		if(i<KEY_BLUETOOTH_MODETONS_NUM)
		{
			hall_wrdata[i] = 0x14;
		}
		else
		{
			hall_wrdata[i] = 0x84;	// continuous
		}
	}
	h6_drv_write_reg_cmd(CONGFIG_REG33, hall_wrdata, g_hasp_key_num);
}

static void hall_hasp_switch_mode(uint8_t mode)
{
//	hasp_drv_stop_work();

	if(mode == MODE_SINGLE_ALL_CHAIN_SAMPLE)
	{
		h6_hal_start_sample(mode);// HASP config to single mode

		h6_hal_reg_multi_write(CONGFIG_REG33, 0x14, g_hasp_key_num);// hall 6 config to single mode
		h6_hal_single_time_inst(0);//hall single variable timing instruction
	}
	else if(mode == MODE_CONTINUOUS_ALL_CHAIN_SAMPLE)
	{
		h6_hal_reg_multi_write(CONGFIG_REG33, 0x84, g_hasp_key_num);// HASP config to continuous mode
		h6_hal_variable_time_inst(0);//hall continuous variable timing instruction

		h6_hal_start_sample(mode);// HASP config to single mode
	}
}

/*------------- MAIN -------------*/
int main(void)
{
	smu_periph_plic_reset();

	/****************Board operating environment/clock system initialization****************/
	board_init();

	/****************Initialize the console interface to implement serial command line debugging****************/
	uart_print_init(UART_PRINT_NO, UART_BD_115200);//

	/****************Total interrupt control related****************/
	smu_global_int_enable();

	/**************** WDT enable****************/
	wdt_hal_cfg();
	wdt_reset_timer();

	/**************** keyboard layout init****************/
	keyboard_config_init();

	printf("KB type=%d, Lang=%d, keys=%d, hasp_key=%d\r\n",
		 g_keyboard_type, g_language_type, g_key_board_num, g_hasp_key_num);

	/**************** External Flash QSPI init ****************/
	/*The external Flash QSPI must be configured before the Hall is initialised,
	 * as the Hall sensor initialisation requires reading data from the external device to make Configuration.*/
	Flash_id = w_data_to_external_flash();
//	info("Flash_id = %d\r\n",Flash_id);

//	QSPI_EXT_FLASH_erase_chip();

	/**************** Load per-key RT from Flash ****************/
	RT_data_init();

#if 1
/*******************external flash read write test start*********************************/
//	copy_internal_flash_to_external_flash(0,64);
//
//	debug_read_external_flash(0,buffer1,16);
//	for(uint8_t i=0;i<15;i++)
//	{
//		info("buffer1[i] = %d \r\n",buffer1[i]);
//	}
	/*******************external flash read write test end********************************/
#endif

	/**************** hall init****************/
	h6_hal_init(hasp_int_callback_my);
	wdt_reset_timer();
//	h6_app_set_mode();

	/**************** timer init****************/
	timer_init_my();

#if MFG_EXT_MODE_ENABLE
	report_timer1_init();                 /* Start the periodic report timer used by extended manufacturing reads. */
#endif

	/**************** Ltimer 0 and 1 init ****************/
	ltimer_init();

//	/**************** Generate eight random numbers ****************/
//	mfg_mode_init();

#if WHEEL_MODULE_ENABLE
	/**************** wheel init****************/
	wheel_init();
#endif

	/**************** USB init****************/
	if(sys_mode_get() & SYS_MODE_USB)
	{
		usb_init();
	}

#if LCD_MODULE_ENABLE
	/**************** LCD white****************/
	lcd_bl_pwm_init(LCD_BL_DEFAULT_BRIGHTNESS);
	LCD_Fill_White();
//	read_Lcdid();
#endif

	hasp_drv_start_work();
	h6_hal_variable_time_inst(0);//hall variable timing instruction
	h6_hal_start_sample(MODE_SINGLE_ALL_CHAIN_SAMPLE);//hall sampling configuration //MODE_CONTINUOUS_ALL_CHAIN_SAMPLE
	tud_sof_cb_enable(true);

	while(1)
	{
		wdt_hal_feed_dog();
		if(sys_mode_get() & SYS_MODE_USB)
		{
			if(tud_mounted() == false)
			{
				tud_task();
				ry_delay_us(5);
				continue;
			}
			tud_task();

#if MFG_EXT_MODE_ENABLE
			mfg_mode_check_usb_vendor_cmd();
			if (report_ready)
			{
				report_ready = 0;
				TestMode_Report_Data();
			}
#else
			#if (USB_DESCRIPTOR_VENDOR_REPORT_LENGTH == 1024)
				mfg_mode_check_usb_vendor_cmd();
			#endif
#endif
		}

#if WHEEL_MODULE_ENABLE
		if (wheel_scan_enabled())
		{
			scan_wheel_A();
			if (g_keyboard_type != KB_TYPE_65_PERCENT)
			{
				scan_wheel_B();
			}
		}
#endif

#if 0//WHEEL_MODULE_ENABLE || MFG_EXT_MODE_ENABLE
		//GTRY6xx_SHOW_DBG_L_0;
		if(color_update)
		{
			color_update = 0;
			Set_RGB(LED_red, LED_green, LED_blue);
			h6_drv_w_pwm_time_clr();
		}
//		GTRY6xx_SHOW_DBG_H_0;
#endif

#if PWM_ENABLE_MD
		if (mfg_mode_cmd != 0U)
		{
			rgb_gradient_polling();
		}
#endif

#if LCD_MODULE_ENABLE

		LCD_Service();                         /* Advance one non-blocking LCD DMA step without holding the main loop. */
#endif

		H6_reset();
	}
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+
// Invoked when device is mounted
void tud_mount_cb(void)
{
//	ry_delay_us(10);
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
//	ry_delay_us(30);
}

/*
 * Invoked when usb bus is suspended
 * remote_wakeup_en : if host allow us  to perform remote wakeup
 * Within 7ms, device must draw an average of current less than 2.5 mA from bus
*/
uint8_t wakeup_lowpower_flag = 0;

#if 1//CONTINUOUS_MODE
void tud_suspend_cb(bool remote_wakeup_en)
{
	/* 1 This function will affect flash programming  !!!!!! */
	/* 2 It is recommended to temporarily deactivate this code during the development phase !!!!!! */
	/* 3 If you want to test the tud_suspend_cb() , you need to execute this code  !!!!!! */
	/* 4 Then, add a delay of more than 3 seconds before this function to burn flash !!!!!! */
	/* 5 The flash programming issue will be resolved in the later upgrade  !!!!!! */

#if EXT_FLASH_DEEP_POWER_DOWN_ENABLE
	external_flash_prepare_suspend();  /* Make relevant configurations for external flashes during sleep mode to save power. */
//	QSPI_DMA_FLASH_dflag();
#endif

	//initial wake up from low power status
	if(wakeup_lowpower_flag == 0)
	{
		smu_enter_lowpower_init(NULL);
		wakeup_lowpower_flag = 1;
	}
	uint32_t three_samp_second_count = 18000;
	stop_hall_sample_sof_single = 0;

	tud_sof_cb_enable(false);
	hasp_drv_stop_work(); //stop auto trigger

	ry_delay_ms(1);

	timer_stop(TC_NUMBER0);
	timer_trigger_clear(TC_NUMBER0);
	wdt_reset_timer();

	hall_hasp_switch_mode(MODE_SINGLE_ALL_CHAIN_SAMPLE);

	ltimer_start(LTIMER_NUMBER0);

	#if PWM_ENABLE_MD
//		h6_hal_pwm_multi_write(H6_PWM_LED1_DUTY,0x0,g_hasp_key_num);
//		h6_hal_pwm_multi_write(H6_PWM_LED2_DUTY,0x0,g_hasp_key_num);
//		h6_hal_pwm_multi_write(H6_PWM_LED3_DUTY,0x0,g_hasp_key_num);
		stop_sampling_pwm_duty_adjust(0,0,0);
	#endif

	#if LCD_MODULE_ENABLE
		LCD_sleep();//suspend
		gpio_out_clear(LCD_3V3);
		gpio_out_clear(LCD_3V9);

		gpio_cfg_input(I2C2_SCL, RY_GPIO_PULLUP_30K);
		gpio_cfg_input(I2C2_SDA, RY_GPIO_PULLUP_30K);
		gpio_cfg_input(LCD_SCL, RY_GPIO_PULLUP_30K);
		gpio_cfg_input(LCD_SDA, RY_GPIO_PULLUP_30K);
	#endif

	ry_delay_ms(1);
	h6_drv_sleep_cmd(1);

	while(1)
	{
//		GTRY6xx_SHOW_DBG_H_0;
//		GTRY6xx_SHOW_DBG_H_2;
//		GTRY6xx_SHOW_DBG_H_3;
		smu_enter_lowpower_mode(SMU_LOWPOWER_DEEP_SLEEP, SMU_DPSLPWKUP_SRC_USB_MASK | SMU_DPSLPWKUP_SRC_LTIMER0_MASK);

		if(smu_get_wkup_irq_src() == IRQ_LTIMER0_SOURCE )
		{
//			GTRY6xx_SHOW_DBG_H_0;
//			GTRY6xx_SHOW_DBG_L_0;

			wdt_reset_timer();
			smu_clear_wkup_irq_src();
			ltimer_clear_event(LTIMER_NUMBER0, ltimer_get_event(LTIMER_NUMBER0));
			ltimer_clear(LTIMER_NUMBER0);

			if(remote_wakeup_en)//allow wakeup
			{
				h6_drv_sleep_cmd(0);
				h6_drv_interrupt_time_inst();
				hasp_drv_start_work();

				if(check_hall_sleep_update(KRY_DOWN_SLEEP_VALUE))
				{
					usb_wakeup = 0;
					tud_remote_wakeup();
					break;
				}
			}
			h6_drv_sleep_cmd(1);
			ltimer_start(LTIMER_NUMBER0);
		}
		else if (smu_get_wkup_irq_src() == IRQ_USB_SOURCE )
		{
			usb_wakeup = 1;
			h6_drv_sleep_cmd(0);
			smu_clear_wkup_irq_src();
			break;
		}
	}

//	smu_clear_wkup_irq_src();
	tud_task();
	if (usb_wakeup == 0)
	{
		//wake up
		//	1. Prevent sending only the DOWN button press without also sending the UP button press.
		//	2. Add a 2.5-second loop to wait for USB ACK IN to return to normal
		uint32 time = three_samp_second_count;

		while(time!=0)
		{
//			tud_task();
			wdt_hal_feed_dog();
			time--;
			hasp_drv_start_work();
			if(check_hall_sleep_update(KRY_UP_SLEEP_VALUE) == 0)
			{
				break;
			}
		}
	}

#if PWM_ENABLE_MD
//	h6_hal_pwm_multi_write(H6_PWM_LED1_DUTY,0xFF,g_hasp_key_num);
//	h6_hal_pwm_multi_write(H6_PWM_LED2_DUTY,0xFF,g_hasp_key_num);
//	h6_hal_pwm_multi_write(H6_PWM_LED3_DUTY,0xFF,g_hasp_key_num);
//	Set_RGB(0xFF,0xFF,0xFF);
	stop_sampling_pwm_duty_adjust(0xFF,0xFF,0xFF);
#endif

#if LCD_MODULE_ENABLE
	gpio_cfg_output(LCD_3V3, RY_GPIO_DRIVE4);
	gpio_cfg_output(LCD_3V9, RY_GPIO_DRIVE4);
	gpio_out_set(LCD_3V3);
	gpio_out_set(LCD_3V9);

	spi_init(LCD_SPI_DEFAULT_SPEED_HZ);

	LCD_wakeup();
	lcd_bl_set_brightness(LCD_PWM_CYCLE_MAX);
#endif

#if  EXT_FLASH_DEEP_POWER_DOWN_ENABLE
	external_flash_resume();  /* Restore SPI5/DMA0 and send 0xAB after the final USB or remote wake-up. */
#endif

	hall_hasp_switch_mode(MODE_CONTINUOUS_ALL_CHAIN_SAMPLE);
	ry_delay_ms(1);
	hasp_drv_start_work();

	stop_hall_sample_sof_single = 1;
	ltimer_stop(LTIMER_NUMBER0);
	tud_sof_cb_enable(true);

}
#else
//single, Not yet developed. Please do not refer to it.
void tud_suspend_cb(bool remote_wakeup_en)
{
	/* 1 This function will affect flash programming  !!!!!! */
	/* 2 It is recommended to temporarily deactivate this code during the development phase !!!!!! */
	/* 3 If you want to test the tud_suspend_cb() , you need to execute this code  !!!!!! */
	/* 4 Then, add a delay of more than 3 seconds before this function to burn flash !!!!!! */
	/* 5 The flash programming issue will be resolved in the later upgrade  !!!!!! */

	//initial wake up from low power status
	if(wakeup_lowpower_flag == 0)
	{
		smu_enter_lowpower_init(NULL);
		wakeup_lowpower_flag =1;
	}
	uint32_t three_samp_second_count = 18000;

//	tud_sof_cb_enable(false);
//	ry_delay_ms(1);

	timer_stop(TC_NUMBER0);
	timer_trigger_clear(TC_NUMBER0);
	wdt_reset_timer();

	stop_hall_sample_sof_single =0;
	ltimer_start(LTIMER_NUMBER0);

	#if PWM_ENABLE_MD
		h6_hal_pwm_multi_write(H6_PWM_LED1_DUTY,0x0,HASP_KEY_NUM);
		h6_hal_pwm_multi_write(H6_PWM_LED2_DUTY,0x0,HASP_KEY_NUM);
		h6_hal_pwm_multi_write(H6_PWM_LED3_DUTY,0x0,HASP_KEY_NUM);
	#endif
	ry_delay_ms(1);
	h6_drv_sleep_cmd(1);

	while(1)
	{
		smu_enter_lowpower_mode(SMU_LOWPOWER_DEEP_SLEEP, SMU_DPSLPWKUP_SRC_USB_MASK | SMU_DPSLPWKUP_SRC_LTIMER0_MASK);

		if(smu_get_wkup_irq_src() == IRQ_LTIMER0_SOURCE )
		{
			wdt_reset_timer();

			smu_clear_wkup_irq_src();
			ltimer_clear_event(LTIMER_NUMBER0, ltimer_get_event(LTIMER_NUMBER0));
			ltimer_clear(LTIMER_NUMBER0);

			if(remote_wakeup_en)
			{
				h6_drv_sleep_cmd(0);
				h6_drv_interrupt_time_inst();
				hasp_drv_start_work();
				if(check_hall_sleep_update(KRY_DOWN_SLEEP_VALUE))
				{
					usb_wakeup =0;
					keyboard_suppress_report_until_release(); /* Wake key should wake the host, not be reported as a held key. */
					tud_remote_wakeup();
					break;
				}
			}
			h6_drv_sleep_cmd(1);
			ltimer_start(LTIMER_NUMBER0);
		}
		else if (smu_get_wkup_irq_src() == IRQ_USB_SOURCE )
		{
			usb_wakeup =1;
			smu_clear_wkup_irq_src();
			break;
		}
	}

//	smu_clear_wkup_irq_src();
	tud_task();
	if (usb_wakeup == 0)
	{
		//wake up
		//	1. Prevent sending only the DOWN button press without also sending the UP button press.
		//	2. Add a 2.5-second loop to wait for USB ACK IN to return to normal
		uint32 time = three_samp_second_count;

		while(time!=0)
		{
//			tud_task();
			wdt_hal_feed_dog();
			time--;
			hasp_drv_start_work();
			if(check_hall_sleep_update(KRY_UP_SLEEP_VALUE) == 0)
			{
				break;
			}
		}
	}
	stop_hall_sample_sof_single = 1;
	ltimer_stop(LTIMER_NUMBER0);
//	tud_sof_cb_enable(true);
}
#endif

//CFG_TUD_SOF_CALLBACK
// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
//  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
	ry_delay_us(7);
}


//sof interrupt from dcd interrupt handle
void hidd_sof_isr(uint8_t rhport, uint32_t frame_count) {
  (void) rhport;
  (void) frame_count;
  hall_sample_start();
}

void tud_sof_cb(uint32_t frame_count)
{
#if PWM_ENABLE_MD
	rgb_gradient_sof_tick(frame_count);
#else
	(void)frame_count;
#endif
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
	(void) instance;
	(void) len;
	if(instance == 0)
	{
//		kb_report_done = 1;
	}
	else if(instance == 1)
	{
		data_report_done = 1;
	}
}
// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}
//
// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) instance;
    data_report_done = 1;

    uint8_t usbrddata[CFG_TUD_HID_EP_BUFSIZE] = {0};
	uint8_t updatedata[16] = {0x01,0x55,0xAA,0xFF,0x01,0x55,0xAA,0xFF,0x01,0x55,0xAA,0xFF,0x01,0x55,0xAA,0x00};
    if (report_type == HID_REPORT_TYPE_OUTPUT)
    {
            if (bufsize == mfg_usb_vendor_report_length())
            {
              	memcpy(vendor_buf_out, buffer, bufsize);
            }

		MEMCPY(usbrddata, buffer, CFG_TUD_HID_EP_BUFSIZE);

		if(((*(uint16 *)&usbrddata[4])) == 16 && memcmp(updatedata, &buffer[8], 16) == 0)//new boot
//		if(((*(uint32 *)&usbrddata[0])) == 0 && ((*(uint16 *)&usbrddata[4])) == 16)//old boot
		{
			tud_sof_cb_enable(false);		//Need to closs SOF
			((*(uint16 *)&usbrddata[4])) = 0x1;
			usbrddata[8] = 0x34;
			dcd_edpt_fast_data_in(0, EPNUM_IN_VENDOR, usbrddata, CFG_TUD_HID_EP_BUFSIZE);
			ry_delay_ms(20);
			info("reset to enterUpgrade!\n");
			smu_enter_upgrade();
		}
//		else
//		{
//			((*(uint16 *)&usbrddata[4])) = 0x1;
//			usbrddata[8] = 0x35;
//			dcd_edpt_fast_data_in(0, EPNUM_IN_VENDOR, usbrddata, CFG_TUD_HID_EP_BUFSIZE);
//		}
    }
}
