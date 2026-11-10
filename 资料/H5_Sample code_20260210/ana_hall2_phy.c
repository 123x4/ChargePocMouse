/*
 * Copyright (c) 2021
 * All Rights Reserved.
 *
 */


#include "ram_dma.h"
#include "ana_sw_phy.h"
#include "ana_hall2_phy.h"

#if ANA_HALL2_PHY_ENABLE

//unsigned char hall_gain_buff[HALL_TOTAL_COUNT]={4};
//unsigned char hall_offset_h_buff[HALL_TOTAL_COUNT]={0};
//unsigned char hall_offset_l_buff[HALL_TOTAL_COUNT]={0};
void ana_hall2_phy_pwm_period(unsigned char length,unsigned short value)
{
	int j;
//	value-=1;
// hall 2
	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = (0x04<<5)|(value>>8);
		ana_wr_buff[j*2+1] = value & 0x00ff;
	}
	ana_phy_write_pwm(length, ana_wr_buff, ana_rd_buff);
}

void ana_hall2_phy_pwm3_duty(unsigned char length,unsigned short value)
{
	int j;
	
	for(j= 0; j < length; j++)
	{
			ana_wr_buff[j*2+0] = (0x02<<5)|(value>>8);
			ana_wr_buff[j*2+1] = value & 0x00ff;
	}
	ana_phy_write_pwm(length, ana_wr_buff, ana_rd_buff);
	
}

void ana_hall2_phy_pwm2_duty(unsigned char length,unsigned short value)
{
	int j;
	
	for(j= 0; j < length; j++)
	{
			ana_wr_buff[j*2+0] = (0x01<<5)|(value>>8);
			ana_wr_buff[j*2+1] = value & 0x00ff;
	}
	ana_phy_write_pwm(length, ana_wr_buff, ana_rd_buff);
	
}

void ana_hall2_phy_pwm1_duty(unsigned char length,unsigned short value)
{
	int j;

	for(j= 0; j < length; j++)
	{
			ana_wr_buff[j*2+0] = (0x00<<5)|(value>>8);
			ana_wr_buff[j*2+1] = value & 0x00ff;
	}
	ana_phy_write_pwm(length, ana_wr_buff, ana_rd_buff);
}

const Gain Hall_Gain_Table[GainNUM] =
{
	//0								1								2								3								
	{LNA_8X_GAIN,PGA_13P2X_GAIN},	{LNA_8X_GAIN,PGA_15P6X_GAIN},	{LNA_8X_GAIN,PGA_19P2X_GAIN},	{LNA_8X_GAIN,PGA_22P8X_GAIN},
	//4								5								6								7								
	{LNA_16X_GAIN,PGA_13P2X_GAIN},	{LNA_16X_GAIN,PGA_15P6X_GAIN},	{LNA_16X_GAIN,PGA_19P2X_GAIN},	{LNA_16X_GAIN,PGA_21P6X_GAIN},
	//8								9								10								11							
	{LNA_32X_GAIN,PGA_13P2X_GAIN},	{LNA_32X_GAIN,PGA_15P6X_GAIN},	{LNA_32X_GAIN,PGA_19P2X_GAIN},	{LNA_32X_GAIN,PGA_21P6X_GAIN},
	//12							13								14								15
	{LNA_64X_GAIN,PGA_13P2X_GAIN},	{LNA_64X_GAIN,PGA_15P6X_GAIN},	{LNA_64X_GAIN,PGA_18P0X_GAIN},	{LNA_64X_GAIN,PGA_22P8X_GAIN},
};

#define SB  6
const unsigned char bit_crc_write_reg[256]=
{

	//0	  			1			2			3			4			5			6			7
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//8				9			10			11			12			13			14			15
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//16			17			18			19			20			21			22			23
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//24			25			26			27			28			29			30			31
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//32	`		33			34			35			36			37			38			39
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//40			41			42			43			44			45			46			47
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//48			49			50			51			52			53			54			55
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//56			57			58			59			60			61			62			63
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//64			65			66			67			68			69			70			71
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//72			73			74			75			76			77			78			79
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//80			81			82			83			84			85			86			87
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//88			89			90			91			92			93			94			95
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//96			97			98			99			100			101			102			103
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//104			105			106			107			108			109			110			111
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,


	//112			113			114			115			116			117			118			119
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,


	//120			121			122			123			124			125			126			127
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//128			129			130			131			132			133			134			135
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//136			137			138			139			140			141			142			143
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//144			145			146			147			148			149			150			151
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//152			153			154			155			156			157			158			159
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//160			161			162			163			164			165			166			167
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//168			169			170			171			172			173			174			175
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//176			177			178			179			180			181			182			183
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//184			185			186			187			188			189			190			191
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//192			193			194			195			196			197			198			199
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//200			201			202			203			204			205			206			207
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//208			209			210			211			212			213			214			215
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//216			217			218			219			220			221			222			223
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//224			225			226			227			228			229			230			231
		1<<SB,		0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

	//232			233			234			235			236			237			238			239
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//240			241			242			243			244			245			246			247
		0,			1<<SB,		1<<SB,		0,			1<<SB,		0,			0,			1<<SB,

	//248			249			250			251			252			253			254			255
	1<<SB,			0,			0,			1<<SB,		0,			1<<SB,		1<<SB,		0,

};

const unsigned char sensor_reg_init_array[REG_CNT][2] = {
	{0x0A, 0x0B},		//SampleInterval_ Fine, 11*4+80=124us
	{0x0B, 0x00},		//SampleInterval_ Coarse, 0ms
	{0x0C, 0xB5},		//fix
	{0x0D, 0x02},		//fix
	{0x0E, 0x1F},		//KeyReleased_ Timeout, 
	{0x0F, 0x00},		//SmartPower Save0
	{0x10, 0x10},		//SmartPower Save1
	{0x11, 0x00},		//fix
	{0x12, 0x47},		//fix
	{0x13, 0x01},		//Filter_Config, for continue conversion mode
	{0x14, 0x25},		//LNA_Config0, 32x gain
	{0x15, 0x44},		//fix
	{0x16, 0x00},		//PGA_Config0, 13.2x gain
	{0x17, 0x7C},		//fix
	{0x18, 0x8E},		//ModeConfig0, led enable, continue conversion mode, smart power-saving mode disabled
	//led恒流源档位设置
	{0x19, 0xFF},		//LED_Current_1
	{0x1A, 0xFF},		//LED_Current_2
	{0x1B, 0xFF},		//LED_Current_3
	{0x1C, 0xFF},		//LED_Current_4
	//led恒流源设置
	{0x1D, 0x88},		//LED_MaxCur1_2
	{0x1E, 0x88},		//LED_MaxCur3_4
	{0x1F, 0x07},		//LED_SDO_ Driver, LED_Multiplier = 0, SDO IO Driver current 12mA
	{0x20, 0x00},		//DeepSleepEn, disable
	{0x21, 0x88},		//fix
	{0x27, 0x00},		//Temp Compensation
};

unsigned char sps_mode_interval_fine[ANA_TOTAL_COUNT] = {SAMPLE_INTERVAL_FINE};
unsigned char sps_mode_interval_Coarse[ANA_TOTAL_COUNT] = {SAMPLE_INTERVAL_COARSE};
unsigned char sps_mode_release_timeout[ANA_TOTAL_COUNT] = {KEY_RELEASE_TIMEOUT};

void ana_hall2_phy_mode_config(unsigned char length,unsigned char gain,unsigned char mode)
{
	unsigned char  j;

	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REGD_HALL_MODE_CONFIG|bit_crc_write_reg[gain];
		ana_wr_buff[j*2+1] = gain;//|0x80;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
}

void ana_hall2_phy_led1_2_current_max(unsigned char length,unsigned char gain)
{
	unsigned char  j;

	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG1D_HALL_LED1_2_MAX|bit_crc_write_reg[gain];
		ana_wr_buff[j*2+1] = gain;//|0x80;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
}

void ana_hall2_phy_led3_4_current_max(unsigned char length,unsigned char gain)
{
	unsigned char  j;

	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG1E_HALL_LED3_4_MAX|bit_crc_write_reg[gain];
		ana_wr_buff[j*2+1] = gain;//|0x80;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
}

void ana_hall2_phy_led1_current(unsigned char length,unsigned char gain)
{
	unsigned char  j;

	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG19_HALL_LED1_GAIN|bit_crc_write_reg[gain];
		ana_wr_buff[j*2+1] = gain;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
}

void ana_hall2_phy_led2_current(unsigned char length,unsigned char gain)
{
	unsigned char  j;

	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG1A_HALL_LED2_GAIN|bit_crc_write_reg[gain];
		ana_wr_buff[j*2+1] = gain;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
}

void ana_hall2_phy_led3_current(unsigned char length,unsigned char gain)
{
	unsigned char  j;

	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG1B_HALL_LED3_GAIN|bit_crc_write_reg[gain];
		ana_wr_buff[j*2+1] = gain;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
}

void ana_hall2_phy_led4_current(unsigned char length,unsigned char gain)
{
	unsigned char  j;

	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG1C_HALL_LED4_GAIN|bit_crc_write_reg[gain];
		ana_wr_buff[j*2+1] = gain;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
}

void ana_hall5_phy_set_gain(unsigned char length, unsigned char* buff)
{
	unsigned char j, gain;

	for(j= 0; j < length; j++)
	{
		if(buff[j] >= GainNUM)buff[j] = ADC_REF_INIT_VALUE;
		gain = Hall_Gain_Table[buff[j]].LNA;
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG14_HALL_LNA_GAIN|bit_crc_write_reg[gain];
		ana_wr_buff[j*2+1] = gain ;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);

	for(j= 0; j < length; j++)
	{
		gain = Hall_Gain_Table[buff[j]].PGA;
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG16_HALL_PGA_GAIN|bit_crc_write_reg[gain];
		ana_wr_buff[j*2+1] = gain ;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
}

void ana_hall5_phy_set_sps_mode(unsigned char length)
{
	unsigned char j, value;
	unsigned short temp

	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG0A_SAMPLE_INTERVAL_FINE|bit_crc_write_reg[sps_mode_interval_fine[j]];
		ana_wr_buff[j*2+1] = sps_mode_interval_fine[j] ;				//actual sample interval 124us	
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);

	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG0B_SAMPLE_INTERVAL_COARSE|bit_crc_write_reg[sps_mode_interval_Coarse[j]];
		ana_wr_buff[j*2+1] = sps_mode_interval_Coarse[j] ;			//release sample interval 124us+5ms
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
	
	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG0E_HALL_KEY_RELEASE_TIMEOUT|bit_crc_write_reg[sps_mode_release_timeout[j]];
		ana_wr_buff[j*2+1] = sps_mode_release_timeout[j] ;				//hall值低于阈值3s进入power save mode
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
	
	for(j= 0; j < length; j++)
	{
	#if MAGNETIC_POLARITY_N	
		temp = 15927 - TOP_INIT_VALUE;
	#else
		temp = TOP_INIT_VALUE;
	#eendif
		value = (uint8)temp;
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG0F_HALL_SMART_POWER_SAVE0|bit_crc_write_reg[value];
		ana_wr_buff[j*2+1] = value ;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
	
	for(j= 0; j < length; j++)
	{
	#if MAGNETIC_POLARITY_N		
		temp = 15927 - TOP_INIT_VALUE;
		value = SPS_Polarity_Less | ((uint8)(temp >> 8) & 0x3f);		//hall值低于TOP_INIT_VALUE时进入power save mode
	#else
		temp = TOP_INIT_VALUE;
		value = SPS_Polarity_Greater | ((uint8)(temp >> 8) & 0x3f);		//hall值高于TOP_INIT_VALUE时进入power save mode
	#endif
		
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG10_HALL_SMART_POWER_SAVE0|bit_crc_write_reg[value];
		ana_wr_buff[j*2+1] = value ;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
	
	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG18_HALL_MODE_CONFIG|bit_crc_write_reg[SPS_MODE_CONFIG];
		ana_wr_buff[j*2+1] = SPS_MODE_CONFIG ;
	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
}

void ana_hall2_phy_set_sdo_drive(unsigned char length,unsigned char drive)
{
	unsigned char j;

	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG1E_SDO_DRIVE;
		ana_wr_buff[j*2+1] = drive|0x04 ;

	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
}

void ana_hall2_phy_set_mode(unsigned char length)
{
	unsigned char i, j;

	for(i = 0; i < REG_CNT; i++)
	{
		for(j= 0; j < length; j++)
		{
			ana_wr_buff[j*2+0] = sensor_reg_init_array[i][0]|0x80|bit_crc_write_reg[sensor_reg_init_array[i][1]];
			ana_wr_buff[j*2+1] = sensor_reg_init_array[i][1];

		}
		ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
	}
}

void ana_hall2_phy_set_sdo_drive(unsigned char length,unsigned char drive)
{
	unsigned char j;

	for(j= 0; j < length; j++)
	{
		ana_wr_buff[j*2+0] = HALL_II_WRITE_REG1F_LED_SDO_DRIVE|bit_crc_write_reg[drive];
		ana_wr_buff[j*2+1] = drive;

	}
	ana_phy_write_reg(length, ana_wr_buff, ana_rd_buff);
}

#endif
/* end of file */

