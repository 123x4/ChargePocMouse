//--------------------------------------------------------------------------
//
// OSD  DRIVER PHY Header file
//
//--------------------------------------------------------------------------
//
// 
//--------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
#ifndef _ANA_SW_PHY_H_
#define _ANA_SW_PHY_H_

#include "config_common.h"

//#define ANA_SW_PYH_ENABLE  1

#define ANA_MODE_CONTINUE  0x80
#define ANA_MODE_ONCE      0x00

#define  TARGET_OSC_FREQ_CFG      0x80 
#define  MAX_DELTA_OSC_FREQ_CFG   (TARGET_OSC_FREQ_CFG + 4)
#define  MIN_DELTA_OSC_FREQ_CFG   (TARGET_OSC_FREQ_CFG - 4)
#define  ANA_INIT_VALUE           0xE0

#define BAD_IC_CHECK_MIN_VALUE    80
#define BAD_IC_CHECK_MAX_VALUE    176

#define CALI_AFTER_IC_CHECK_MIN_VALUE    (MIN_DELTA_OSC_FREQ_CFG-2)
#define CALI_AFTER_IC_CHECK_MAX_VALUE    (MAX_DELTA_OSC_FREQ_CFG+2)


//                        			(terget+modify)   note
#define  TD1_DELAY_US    				5       			//+-5%

#define  TD2_CONTIUNS_DELAY_US  		3					//>2
#define  TD2_SIGNLE_DELAY_US    		90 					//>360

#define  TD3_DELAY_US    				9      				//+-5%
#define  TD4_DELAY_US    				24       			//+-5%
#define  TD5_DELAY_US    				5 					//>10

#define  TD7_DELAY_US    				128					//+-5%

#define  TD10_DELAY_US   				3  					//>6

#define  TD11_DELAY_US    				15     				//+-5%
#define  TD12_DELAY_US    				3 					//>2
#define  TD13_DELAY_US   				12  				//>12

	
#define CRC_ERROR 0xffff

#if ANA_SW_PYH_ENABLE		
 		extern unsigned char ana_mode ;
		void ana_phy_osc_calibrate(unsigned char length);
		unsigned char ana_phy_write_reg(unsigned char length, unsigned char *wr_buf, unsigned char *rd_buf);
		unsigned char ana_phy_read_reg(unsigned char length, unsigned char *wr_buf, unsigned char *rd_buf);		
		unsigned char ana_phy_get_mode(void);
		unsigned short ana_phy_bit_error_checker(unsigned short sample);
		unsigned short ana_phy_bit_error_checker2(unsigned char sample_hi,unsigned char sample_lo);			  	
		unsigned char ana_phy_write_pwm( unsigned char length, unsigned char *wr_buf, unsigned char *rd_buf);
		unsigned char ana_phy_write_pwm_and_convert(unsigned char length, unsigned char *wr_buf, unsigned char *rd_buf);		
		unsigned char ana_phy_sensor_convert(unsigned char length, unsigned char *wr_buf, unsigned char *rd_buf);		
		unsigned char ana_phy_data_chain_check(unsigned char length);
		unsigned char ana_phy_data_chain_check0(unsigned char length);
		unsigned char ana_phy_data_chain_check_esd(unsigned char length);
		void ana_phy_command_sleep(void);
		void ana_phy_command_wakeup(void);
#endif

#endif /*_OSD_HAL_H_*/


