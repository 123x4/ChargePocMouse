/*
 * custom_original.h
 *
 *  Created on: 2023年7月30日
 *      Author: Hao.Luo
 */

#ifndef HALL_SPI_H_
#define HALL_SPI_H_

#include "usb.h"
#include "usb_device_config.h"

//It is defined by Hardware
#define BAD_IC_CHECK_MIN_VALUE    	0x50	//DEC:80
#define BAD_IC_CHECK_MAX_VALUE    	0xB0	//DEC:176
//------------------------------------------------------------------------------------------------------------
//It is the half of BAD_IC_CHECK_MAX_VALUE and BAD_IC_CHECK_MIN_VALUE;
#define TARGET_OSC_FREQ_CFG     	0x80
//------------------------------------------------------------------------------------------------------------
#define MAX_DELTA_OSC_FREQ_CFG   	(TARGET_OSC_FREQ_CFG + 4)
#define MIN_DELTA_OSC_FREQ_CFG   	(TARGET_OSC_FREQ_CFG - 4)
//------------------------------------------------------------------------------------------------------------
/* crc算法规格书里面漏了0x13这个寄存器，要记得加上这个寄存器来算 */
#define Hall_CRC_VALUE				0x3B
//------------------------------------------------------------------------------------------------------------
#define OSC_INIT_VALUE           	0xE0
//------------------------------------------------------------------------------------------------------------
#define Hall_sensor_Gain			0x0D
//------------------------------------------------------------------------------------------------------------
#define Hall_offset_L				0x00
//------------------------------------------------------------------------------------------------------------
#define Hall_offset_H				0x00
//------------------------------------------------------------------------------------------------------------
#if 0
#define  TD1_DELAY_US    							(11+306)      	//+-5%
#define  TD2_CONTIUNS_DELAY_US 	 					(2+50) 			//>2
#define  TD2_SIGNLE_DELAY_US    					(360+7815) 		//>360
#define  TD3_DELAY_US    							(17+490)		//(17+0)+-5%
#define  TD4_DELAY_US    							(26+750)		//(26+0)+-5%
#define  TD5_DELAY_US    							(10+315) 		//>10
#define  TD6_DELAY_US    							(39+1)      	//(39)+-5%
#define  TD7_DELAY_US    							(132+3815)		//(132)+-5%
#define  TD8_DELAY_US    							(392-4)    		//(392)+-5%
#define  TD9_DELAY_US    							(1000+30500)   	//+-5%
#define  TD10_DELAY_US   							(12+400)  		//>12
#define  TD11_DELAY_US    							(58+1670)     	//+-5%
#define  TD12_DELAY_US    							(3+306) 		//>2
#define  TD13_DELAY_US   							(12+406)  		//>12
#define  TD14_DELAY_US    							(1000+30500)   	//+-5%
#endif
#define  TD1_DELAY_US    							(5+122)      	//+-5%
#define  TD2_CONTIUNS_DELAY_US 	 					(2+50) 			//>2
#define  TD2_SIGNLE_DELAY_US    					(360+7815) 		//>360
#define  TD3_DELAY_US    							(9+240)			//(9)+-5%
#define  TD4_DELAY_US    							(24+670)		//(26+0)+-5%
#define  TD5_DELAY_US    							(10+315) 		//>10
#define  TD6_DELAY_US    							(39+1)      	//(39)+-5%
#define  TD7_DELAY_US    							(128+3700)		//(132)+-5%
#define  TD8_DELAY_US    							(392-4)    		//(392)+-5%
#define  TD9_DELAY_US    							(1000+30500)   	//+-5%
#define  TD10_DELAY_US   							(12+400)  		//>12
#define  TD11_DELAY_US    							(58+2)     		//+-5%
#define  TD12_DELAY_US    							(3+115) 		//>2
#define  TD13_DELAY_US   							(13+115)  		//>12
#define  TD14_DELAY_US    							(1000+30500)   	//+-5%

//                        						(terget+modify) 	note
#define  TD1_DELAY_US_FreqDouble    				(5+135)      	//+-5%
#define  TD2_CONTIUNS_DELAY_US_FreqDouble 	 		(1+25) 			//>2
#define  TD2_SIGNLE_DELAY_US_FreqDouble    			(180+3907) 		//>360
#define  TD3_DELAY_US_FreqDouble    				(8+224)			//(8.5+0)+-5%
#define  TD4_DELAY_US_FreqDouble    				(13+360)		//(26+0)+-5%
#define  TD5_DELAY_US_FreqDouble    				(5+157) 		//>10
#define  TD6_DELAY_US_FreqDouble    				(20+1)      	//(39)+-5%
#define  TD7_DELAY_US_FreqDouble    				(66+1907)		//(132)+-5%
#define  TD8_DELAY_US_FreqDouble    				(196-2)    		//(392)+-5%
#define  TD9_DELAY_US_FreqDouble    				(500+14500)   	//+-5%
#define  TD10_DELAY_US_FreqDouble   				(6+200)  		//>12
#define  TD11_DELAY_US_FreqDouble    				(29+835)     	//+-5%
#define  TD12_DELAY_US_FreqDouble    				(2+153) 		//>2
#define  TD13_DELAY_US_FreqDouble   				(12+400)  		//>12

//Hall sensor  register address,refer to hall sensor datasheet.
//SPI Transfer by 16 bit,so register address is in high byte;
#define LED_MaxCur 				0x0800
#define LED_Current_0 			0x0900
#define LED_Current_1 			0x0A00
#define LED_Current_2 			0x0B00
#define VREF_Config 			0x0D00			//HALL GAIN
#define CRC_REG 				0x0E00
#define ADC_Config0 			0x0F00
#define ADC_Config1 			0x1000
#define HAL_Offset_L 			0x1100
#define HAL_Offset_H 			0x1200
#define HAL_Config1 			0x1300
#define HAL_Config2 			0x1800
#define HAL_Config4 			0x1900
#define SDO_Config 				0x1E00
//------------------------------------------------------------------------------------------------------------
#define HALL_SDO_DRIVER_1mA 	0x00
#define HALL_SDO_DRIVER_2mA 	0x01
#define HALL_SDO_DRIVER_4mA 	0x02
#define HALL_SDO_DRIVER_8mA 	0x03
//------------------------------------------------------------------------------------------------------------
#define LED_MAX_2p4mA   		0
#define LED_MAX_3p0mA   		1
#define LED_MAX_3p5mA   		2
#define LED_MAX_9p0mA   		3
//------------------------------------------------------------------------------------------------------------
#define LED_ENABLE   			1
#define LED_DASBLE   			0
//------------------------------------------------------------------------------------------------------------
#define LED0_Current_PWM    	150
#define LED1_Current_PWM    	150
#define LED2_Current_PWM    	150
//------------------------------------------------------------------------------------------------------------
#define Hall_sensor_Write 		0x8000
#define Hall_sensor_Read 		0x0000
//------------------------------------------------------------------------------------------------------------
/*  Some error define for Hall sensor   */
#define ROWDATA_CRC_SUCCESS		0x01
#define ROWDATA_CRC_ERROR		0xFF
//------------------------------------------------------------------------------------------------------------
#define SET_REPORT_FORMAT_RAW_DATA        				0
#define SET_REPORT_FORMAT_CALI_DATA        				1
#define SET_REPORT_FORMAT_BITMAP_DATA        			2
#define SET_REPORT_FORMAT_CALI_ANALOG_DATA        		3
#define SET_REPORT_FORMAT_MISC_DATA        				4
#define SET_REPORT_FORMAT_HALL_GAIN        				5
#define SET_REPORT_FORMAT_PCBA_AVE_NOISE        		6
#define SET_REPORT_FORMAT_BOTTOM_AVE_NOISE        		7
#define SET_REPORT_FORMAT_TOP_AVE_NOISE        			9
#define SET_REPORT_FORMAT_USER_BOTTOM_AVE_NOISE     	10
#define SET_REPORT_FORMAT_USER_TOP_AVE_NOISE       		11
#define SET_REPORT_FORMAT_HALL_SELF_CHECK        		12
#define SET_REPORT_FORMAT_TOP_BASELINE        			13
#define SET_REPORT_FORMAT_BOTTOM_BASELINE        		14
//------------------------------------------------------------------------------------------------------------
/*
Bit[15:14]: A1/A0
0x3：Write 14bit PWM Cycle
0x2：Write 14bit PWM2 Duty Cycle
0x1：Write 14bit PWM1 Duty Cycle
0x0：Write 14bit PWM0 Duty Cycle
2) Bit[13:0]: 14bit PWM data;
*/
//#define Hall_Write_PWM_Period 	0xC000
//#define Hall_Write_PWM_LED2 	0x8000
//#define Hall_Write_PWM_LED1  	0x4000
//#define Hall_Write_PWM_LED0  	0x0000

#define Hall_Write_PWM_Period 	0x8000
#define Hall_Write_PWM_led3 	0x6000  	//only for GTRY154X
#define Hall_Write_PWM_LED2 	0x4000
#define Hall_Write_PWM_LED1  	0x2000
#define Hall_Write_PWM_LED0  	0x0000

//------------------------------------------------------------------------------------------------------------
#define Led_PWM_Period_Value 	0x00FF
//------------------------------------------------------------------------------------------------------------
#define BaslineTopDefault 		5600
#define BaslineBottomDefault 	3500
//------------------------------------------------------------------------------------------------------------
#define RawDate_Has_Change 		2000
#define RawDate_Has_Change_ALTL 50
#define Adjust_Value			500
#define Interval_Values			1279//256*5-1
//------------------------------------------------------------------------------------------------------------
#define HALL_5S_ESDTIME			5000000//125*8*1000*5
/**************************************************************************************************************
 * Variables Define
 **************************************************************************************************************/
extern const uint8_t BIT_Error[256];
extern const uint8_t Node_Cali_Table[1280];
extern uint16_t Hall_PowerON_BaselineCal;
extern uint8_t Hall_Data_Report_Type;
extern uint8_t Flag_RawData_CRC_Check;
extern uint32_t Tick_5s_ESDCheck;
extern uint8_t Flag_has_gain_adjust;
extern uint8_t Flag_Need_adjust_Gain;
extern uint8_t Tick_SETGain_Need_Wait;
//------------------------------------------------------------------------------------------------------------
extern uint16_t Baseline_Top_SPI0[TRANSFER_SIZE];
extern uint16_t Baseline_Top_SPI1[TRANSFER_SIZE];
extern uint16_t Baseline_Top_SPI2[TRANSFER_SIZE];
extern uint16_t Baseline_Top_SPI3[TRANSFER_SIZE];
extern uint16_t Baseline_Top_SPI4[TRANSFER_SIZE];
extern uint16_t Baseline_Top_SPI5[TRANSFER_SIZE];
extern uint16_t Baseline_Top_SPI6[TRANSFER_SIZE];
extern uint16_t Baseline_Top_SPI7[TRANSFER_SIZE];
//------------------------------------------------------------------------------------------------------------
extern uint16_t Baseline_Bottom_SPI0[TRANSFER_SIZE];
extern uint16_t Baseline_Bottom_SPI1[TRANSFER_SIZE];
extern uint16_t Baseline_Bottom_SPI2[TRANSFER_SIZE];
extern uint16_t Baseline_Bottom_SPI3[TRANSFER_SIZE];
extern uint16_t Baseline_Bottom_SPI4[TRANSFER_SIZE];
extern uint16_t Baseline_Bottom_SPI5[TRANSFER_SIZE];
extern uint16_t Baseline_Bottom_SPI6[TRANSFER_SIZE];
extern uint16_t Baseline_Bottom_SPI7[TRANSFER_SIZE];
//------------------------------------------------------------------------------------------------------------
extern uint16_t Baseline_Step_SPI0[TRANSFER_SIZE];
extern uint16_t Baseline_Step_SPI1[TRANSFER_SIZE];
extern uint16_t Baseline_Step_SPI2[TRANSFER_SIZE];
extern uint16_t Baseline_Step_SPI3[TRANSFER_SIZE];
extern uint16_t Baseline_Step_SPI4[TRANSFER_SIZE];
extern uint16_t Baseline_Step_SPI5[TRANSFER_SIZE];
extern uint16_t Baseline_Step_SPI6[TRANSFER_SIZE];
extern uint16_t Baseline_Step_SPI7[TRANSFER_SIZE];
//------------------------------------------------------------------------------------------------------------
extern uint16_t Hall_Node_SPI0[TRANSFER_SIZE];
extern uint16_t Hall_Node_SPI1[TRANSFER_SIZE];
extern uint16_t Hall_Node_SPI2[TRANSFER_SIZE];
extern uint16_t Hall_Node_SPI3[TRANSFER_SIZE];
extern uint16_t Hall_Node_SPI4[TRANSFER_SIZE];
extern uint16_t Hall_Node_SPI5[TRANSFER_SIZE];
extern uint16_t Hall_Node_SPI6[TRANSFER_SIZE];
extern uint16_t Hall_Node_SPI7[TRANSFER_SIZE];
/**************************************************************************************************************
 * Function Define
 **************************************************************************************************************/
//------------------------------------------------------------------------------------------------------------
extern void Hall_CLK_Command_SET(unsigned short high_delay, unsigned short low_delay);
extern void Hall_Cali_Init(void);
extern void HALL_Read_Instru_cnt(void);
extern void Set_Register(uint16_t Address, uint16_t Data);
extern uint8_t HALLChain_Check_FREQ_DETECH(void);
extern uint8_t HALLChain_Check_FREQ_DETECH_Doubling(void);
extern void Hall_write_register(void);
extern uint8_t Hall_read_register(void);
extern void Hall_write_osc_reg(void);
extern void Hall_write_osc_reg_FrequencyDoubling(void);
extern void Hall_write_PWM(void);
extern void Hall_osc_calibrate(void);
extern void Hall_osc_calibrate_FrequencyDoubling(void);
extern void Hall_OSC_init(void);
extern void Hall_osc_config_init(void);
extern void HallSensor_INIT(void);
extern void Hall_set_mode_14bit(void);
extern void Hall_set_gain(void);
extern void Hall_set_offset(uint8_t Offset_L,uint8_t Offset_H);
extern void Hall_set_SDO_Driver(uint8_t SDO_Driver_Level);
extern void Hall_set_led_current_max(void);
extern void Hall_set_led0_current(uint8_t Led0_Current_PWM);
extern void Hall_set_led1_current(uint8_t Led1_Current_PWM);
extern void Hall_set_led2_current(uint8_t Led2_Current_PWM);
extern void Hall_set_pwm_period(uint16_t Led_PWM_Period);
extern uint8_t Hall_read_CRC(void);
extern void Hall_read_REG(void);
extern void HALL_ESD_CheckAndReset(void);
//------------------------------------------------------------------------------------------------------------
extern void Hall_Baseline_Cal_Process(void);
extern void Hall_Sample_Rawdata(void);
extern void CRCErr_Hall_INIT_Again(void);
//------------------------------------------------------------------------------------------------------------
extern uint8_t Hall_Check_Rawdata_CRC_SPI0(uint8_t Keyindex);
extern uint8_t Hall_Check_Rawdata_CRC_SPI1(uint8_t Keyindex);
extern uint8_t Hall_Check_Rawdata_CRC_SPI2(uint8_t Keyindex);
extern uint8_t Hall_Check_Rawdata_CRC_SPI3(uint8_t Keyindex);
extern uint8_t Hall_Check_Rawdata_CRC_SPI4(uint8_t Keyindex);
extern uint8_t Hall_Check_Rawdata_CRC_SPI6(uint8_t Keyindex);
//------------------------------------------------------------------------------------------------------------
extern void Group1_Baseline_Calibration(uint8_t Keyindex);
extern void Group2_Baseline_Calibration(uint8_t Keyindex);
extern void Group3_Baseline_Calibration(uint8_t Keyindex);
extern void Group4_Baseline_Calibration(uint8_t Keyindex);
extern void Group5_Baseline_Calibration(uint8_t Keyindex);
//------------------------------------------------------------------------------------------------------------
extern uint16_t Hall_Cal_Node_Data_SPI0(uint8_t Keyindex);
extern uint16_t Hall_Cal_Node_Data_SPI1(uint8_t Keyindex);
extern uint16_t Hall_Cal_Node_Data_SPI2(uint8_t Keyindex);
extern uint16_t Hall_Cal_Node_Data_SPI3(uint8_t Keyindex);
extern uint16_t Hall_Cal_Node_Data_SPI4(uint8_t Keyindex);
extern uint16_t Hall_Cal_Node_Data_SPI6(uint8_t Keyindex);
extern uint16_t Hall_Cal_Node_Data_SPI7(uint8_t Keyindex);
//------------------------------------------------------------------------------------------------------------
#endif /* RESUME_H_ */
