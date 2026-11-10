//--------------------------------------------------------------------------
//
// HALL  DRIVER PHY II Header file
//
//--------------------------------------------------------------------------
// Hall phy I layer - In general, user do not need modify these APIs
// Version 1.3
// Author River Chen
// 
//--------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
#ifndef _ANA_HALL2_PHY_H_
#define _ANA_HALL2_PHY_H_


#include "config_common.h"
//#define ANA_HALL2_PHY_ENABLE   1

typedef struct{
	uint8_t	LNA;
	uint8_t PGA;
}Gain;

#define GainNUM								16

#define  ADC_REF_INIT_VALUE   7
#define  ADC_REF_CALI_INIT_VALUE   15
#define  ADC_REF_MIN_VALUE    0
#define  ADC_REF_MAX_VALUE    15
//#define  KEY_DOWN_TARGET_SAMPLE_VALUE      2000
#define  KEY_DOWN_MAX_SAMPLE_VALUE   14000
#define  KEY_DOWN_MIN_SAMPLE_VALUE   13500
#define  KEY_DOWN_START_SAMPLE_VALUE   10000

//------------------------------------------------------------------------------------------------------------
#define LNA_8X_GAIN							0x0D
#define LNA_16X_GAIN						0x1D
#define LNA_32X_GAIN						0x25
#define LNA_64X_GAIN						0x35
#define LNA_128X_GAIN						0x41
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
#define PGA_13P2X_GAIN						0x00
#define PGA_14P4X_GAIN						0x01
#define PGA_15P6X_GAIN						0x02
#define PGA_16P8X_GAIN						0x03
#define PGA_18P0X_GAIN						0x04
#define PGA_19P2X_GAIN						0x05
#define PGA_20P4X_GAIN						0x06
#define PGA_21P6X_GAIN						0x07
#define PGA_22P8X_GAIN						0x08
#define PGA_24P0X_GAIN						0x09

#define REG_CNT	25

#define HALL_II_WRITE_REG13_HALL_FILTER	 (0x13|0x80) 
#define HALL_II_WRITE_REG14_HALL_LNA_GAIN (0x14|0x80) 
#define HALL_II_WRITE_REG16_HALL_PGA_GAIN (0x16|0x80)

#define HALL_II_WRITE_REG0A_SAMPLE_INTERVAL_FINE	 	(0x0A|0x80)
	#define SAMPLE_INTERVAL_FINE	0x0B				//124us
#define HALL_II_WRITE_REG0B_SAMPLE_INTERVAL_COARSE 		(0x0B|0x80)
	#define SAMPLE_INTERVAL_COARSE	0x02				//2ms
#define HALL_II_WRITE_REG0E_HALL_KEY_RELEASE_TIMEOUT 	(0x0E|0x80)
	#define	KEY_RELEASE_TIMEOUT		0x03				//3s
#define HALL_II_WRITE_REG0F_HALL_SMART_POWER_SAVE0		(0x0F|0x80) 
#define HALL_II_WRITE_REG10_HALL_SMART_POWER_SAVE0		(0x10|0x80)
	#define SPS_Polarity_Less		0x80
	#define SPS_Polarity_Greater	0x00
#define HALL_II_WRITE_REG18_HALL_MODE_CONFIG 			(0x18|0x80)
	#define SPS_MODE_CONFIG			0x8F			//led enable, continue conversion mode, sps mode enable

#define HALL_II_WRITE_REGD_HALL_MODE_CONFIG (0x18|0x80)  //0-15

#define HALL_II_WRITE_REG19_HALL_LED1_GAIN (0x19|0x80)  //0-15
#define HALL_II_WRITE_REG1A_HALL_LED2_GAIN (0x1A|0x80)  //0-15
#define HALL_II_WRITE_REG1B_HALL_LED3_GAIN (0x1B|0x80)  //0-15
#define HALL_II_WRITE_REG1C_HALL_LED4_GAIN (0x1C|0x80)  //0-15

#define HALL_II_WRITE_REG1D_HALL_LED1_2_MAX (0x1D|0x80)  //0-15
#define HALL_II_WRITE_REG1E_HALL_LED3_4_MAX (0x1E|0x80)  //0-15

#define HALL_II_WRITE_REG1F_LED_SDO_DRIVE (0x1F|0x80)  //0-3

#if ANA_HALL2_PHY_ENABLE
		void ana_hall2_phy_pwm3_duty(unsigned char length,unsigned short value);
		void ana_hall2_phy_pwm2_duty(unsigned char length,unsigned short value);
		void ana_hall2_phy_pwm1_duty(unsigned char length,unsigned short value);
		void ana_hall2_phy_pwm_period(unsigned char length,unsigned short value);
		unsigned char ana_hall2_phy_read_crc(unsigned char length);
		void ana_hall2_phy_led_current_max(unsigned char length,unsigned char gain, unsigned char mode);
		void ana_hall2_phy_led0_current(unsigned char length,unsigned char gain);
		void ana_hall2_phy_led1_current(unsigned char length,unsigned char gain);
		void ana_hall2_phy_led2_current(unsigned char length,unsigned char gain);
		void ana_hall2_phy_set_gain(unsigned char length,unsigned char value);
		void ana_hall2_phy_set_gain2(unsigned char length,unsigned char index,unsigned char value);
		void ana_hall2_phy_set_offset(unsigned char length,unsigned char value_h,unsigned char value_l);
		void ana_hall2_phy_set_offset2(unsigned char length,unsigned char index,unsigned char value_h,unsigned char value_l);
		void ana_hall2_phy_set_mode_14bit(unsigned char length);
		void ana_hall2_phy_set_mode_10bit(unsigned char length);
		void ana_hall5_phy_set_gain(unsigned char length, unsigned char* buff);
		void ana_hall2_phy_set_sdo_drive(unsigned char length,unsigned char drive);
		void ana_hall2_phy_key_down_calibrate(unsigned char length);
		void ana_hall2_phy_key_up_calibrate(unsigned char length);
#endif

#endif //_HALL_DRIVER_PHY_I_H_


