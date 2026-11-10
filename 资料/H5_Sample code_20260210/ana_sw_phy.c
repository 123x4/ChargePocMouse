/*
 * Copyright (c) 2019  
 * All Rights Reserved.
 *
 */

#include "ram_dma.h"
#include "ana_sw_phy.h"
#include "ana_sw_mcu.h"

#if ANA_SW_PYH_ENABLE

unsigned char ana_mode=ANA_MODE_CONTINUE ;

unsigned char ana_phy_write_reg(unsigned char length, unsigned char *wr_buf, unsigned char *rd_buf)
{ 
    unsigned short len = length<<1;
    
    ana_mcu_spi_dma_master_block_transfer(len, wr_buf, rd_buf);
    
	ana_mcu_command_head_tail(TD4_DELAY_US,TD5_DELAY_US);
	
    return true;
}

unsigned char ana_phy_read_reg(unsigned char length, unsigned char *wr_buf, unsigned char *rd_buf)
{  

		unsigned short len = length<<1;
	
		//		ana_mcu_command_head_tail(TD3_DELAY_US,TD5_DELAY_US);
	
		ana_mcu_spi_dma_master_block_transfer(len, wr_buf, rd_buf);
		
		ana_mcu_command_head_tail(TD4_DELAY_US,TD5_DELAY_US);

		ana_mcu_spi_dma_master_block_transfer(len, wr_buf, rd_buf);
		
		return true;
}

unsigned short ana_phy_bit_error_checker(unsigned short sample)
{
	unsigned char i;
	unsigned short b14=0,b15=0;
	for (i=0;i<14;i++)
	{
		if(i&0x01)
		{
			b15 ^= (sample>>i) & 0x0001;
		}else
		{
			b14 ^= (sample>>i) & 0x0001;
		}
	}
	if((sample & 0xC000)==((b14<<14)|(b15<<15)))
	{//match
	    return sample & 0x3fff;
	}else
	{
		return CRC_ERROR;
	}
}

unsigned char ana_phy_read_instru_cnt(unsigned char length,unsigned char *wr_buf, unsigned char *rd_buf)
{
		unsigned short len = length<<1;
	
		ana_mcu_command_head_tail(TD7_DELAY_US,TD10_DELAY_US);
  
    ana_mcu_spi_dma_master_block_transfer(len, wr_buf, rd_buf);
    
    return true;
}

void ana_phy_osc_calibrate(unsigned char length)
{
    unsigned short i, times=0;
	unsigned short osc_freq_temp;
    unsigned char verify_start = 1;
	for(i = 0; i < length; i++)
    {
		ana_wr_buff[i*2+0] = 0x00 ; 
		ana_wr_buff[i*2+1] = ANA_INIT_VALUE;
    }
	
	while(times < 30)  //Calibrate times is 20 
    {
		ana_phy_read_instru_cnt(length,ana_wr_buff,ana_rd_buff);
       
		ry_delay_us(10);
        times += 1;
    }
	
	ana_phy_read_instru_cnt(length,ana_wr_buff,ana_rd_buff);

	for(i = 0; i < length; i++)
	{
		osc_freq_temp = ((ana_rd_buff[2*i]<<8) | ana_rd_buff[2*i + 1])&0xFFF;

		if(osc_freq_temp > MAX_DELTA_OSC_FREQ_CFG)
		{
			verify_start = 0;     
		}
		else if(osc_freq_temp < MIN_DELTA_OSC_FREQ_CFG)
		{
			verify_start = 0;
		}
	}

	return verify_start;
}

unsigned char ana_phy_get_mode(void)
{
		return ana_mode;
}

const unsigned char bit_error[256]=
{

		//0		1		2		3		4		5		6		7	
		0,		64,		128,	192,	64,		0,		192,	128,	

		//8		9		10		11		12		13		14		15	
		128,	192,	0,		64,		192,	128,	64,		0,	

		//16	17		18		19		20		21		22		23	
		64,		0,		192,	128,	0,		64,	128,	192,	

		//24	25		26		27	28	29	30	31	
		192,	128,	64,		0,	128,	192,	0,	64,	

		//32	33		34		35	36	37	38	39	
		128,	192,	0,		64,	192,	128,	64,	0,	

		//40	41		42		43	44	45	46	47	
		0,		64,		128,	192,	64,	0,	192,	128,	

		//48	49		50		51	52	53	54	55	
		192,	128,	64,		0,	128,	192,	0,	64,	

		//56	57		58		59	60	61	62	63	
		64,		0,		192,	128,	0,	64,	128,	192,	

		//64	65		66		67	68	69	70	71	
		64,		0,		192,	128,	0,	64,	128,	192,	

		//72	73		74		75	76	77	78	79	
		192,	128,	64,		0,	128,	192,	0,	64,	

		//80	81		82		83	84	85	86	87	
		0,		64,		128,	192,	64,	0,	192,	128,	

		//88	89		90		91	92	93	94	95	
		128,	192,	0,		64,	192,	128,	64,	0,	

		//96	97		98		99	100	101	102	103	
		192,	128,	64,		0,	128,	192,	0,	64,	

		//104	105		106		107	108	109	110	111	
		64,		0,		192,	128,	0,	64,	128,	192,	

		//112	113		114		115	116	117	118	119	
		128,	192,	0,		64,	192,	128,	64,	0,	

		//120	121		122		123	124	125	126	127	
		0,		64,		128,	192,	64,	0,	192,	128,	

		//128	129		130		131	132	133	134	135	
		128,	192,	0,		64,	192,	128,	64,	0,	

		//136	137		138		139	140	141	142	143	
		0,		64,		128,	192,	64,	0,	192,	128,	

		//144	145		146		147	148	149	150	151	
		192,	128,	64,		0,	128,	192,	0,	64,	

		//152	153		154		155	156	157	158	159	
		64,		0,		192,	128,	0,	64,	128,	192,	

		//160	161		162		163	164	165	166	167	
		0,		64,		128,	192,	64,	0,	192,	128,	

		//168	169		170		171	172	173	174	175	
		128,	192,	0,		64,	192,	128,	64,	0,	

		//176	177		178		179	180	181	182	183	
		64,		0,		192,	128,	0,	64,	128,	192,	

		//184	185		186		187	188	189	190	191	
		192,	128,	64,		0,	128,	192,	0,	64,	

		//192	193		194		195	196	197	198	199	
		192,	128,	64,		0,	128,	192,	0,	64,	

		//200	201		202		203	204	205	206	207	
		64,		0,		192,	128,	0,	64,	128,	192,	

		//208	209		210		211	212	213	214	215	
		128,	192,	0,		64,	192,	128,	64,	0,	

		//216	217		218		219	220	221	222	223	
		0,		64,		128,	192,	64,	0,	192,	128,	

		//224	225		226		227	228	229	230	231	
		64,		0,		192,	128,	0,	64,	128,	192,	

		//232	233		234		235	236	237	238	239	
		192,	128,	64,		0,	128,	192,	0,	64,	

		//240	241		242		243	244	245	246	247	
		0,		64,		128,	192,	64,	0,	192,	128,	

		//248	249		250		251	252	253	254	255	
		128,	192,	0,		64,	192,	128,	64,	0,	

};


unsigned short ana_phy_bit_error_checker2(unsigned char sample_hi,unsigned char sample_lo)
{
	  unsigned char sample_hi_shade = sample_hi & 0x3f;//ตอ6ฮป
	
		if((sample_hi & 0xC0)==(bit_error[sample_hi_shade]^bit_error[sample_lo]))
		{
			if (ana_mode == ANA_MODE_CONTINUE)
			{
				return (sample_hi_shade<<8) | sample_lo;
			}
			else
			{
				return (((sample_hi_shade<<8) | sample_lo) << 4);
			}
		}else
    {
			return CRC_ERROR;
		}
}
unsigned char ana_phy_write_pwm( unsigned char length, unsigned char *wr_buf, unsigned char *rd_buf)
{
		
		unsigned short len = length<<1;
		
//		ana_mcu_command_head_tail(TD3_DELAY_US,TD5_DELAY_US);
    
    ana_mcu_spi_dma_master_block_transfer(len, wr_buf, rd_buf);
	
		ana_mcu_command_head_tail(TD3_DELAY_US,TD5_DELAY_US);
    return true;
}
unsigned char ana_phy_sensor_convert(unsigned char length, unsigned char *wr_buf, unsigned char *rd_buf)
{ 
	  unsigned short len;
    unsigned short td2_delay;
	
	  if (ana_mode == ANA_MODE_CONTINUE)
		{
			len = length<<1;
			td2_delay = TD2_CONTIUNS_DELAY_US;
		}else
		{
			len = length<<1;
			td2_delay = TD2_SIGNLE_DELAY_US;
		}
    
		ana_mcu_command_head_tail(TD1_DELAY_US,td2_delay);
  
    ana_mcu_spi_dma_master_block_transfer(len, wr_buf, rd_buf);
		
    return true;
}
unsigned char ana_phy_write_pwm_and_convert(unsigned char length, unsigned char *wr_buf, unsigned char *rd_buf)
{ 
	  
		ana_phy_sensor_convert(length, wr_buf, rd_buf);
		ana_mcu_command_head_tail(TD3_DELAY_US,TD5_DELAY_US);
		
    return true;
}

unsigned char ana_phy_data_chain_check_esd(unsigned char length)
{
	unsigned char i,ret=0xff;
	ana_phy_read_instru_cnt(length,ana_wr_buff,ana_rd_buff);

	for(i = 0; i < length; i++)
	{
		if( (ana_rd_buff[2*i + 1]<CALI_AFTER_IC_CHECK_MIN_VALUE) || (ana_rd_buff[2*i + 1]>CALI_AFTER_IC_CHECK_MAX_VALUE ))
		{//bad hall
			if(ret==0xff)
			{// record first bad hall
				ret = i;
				ana_rd_buff[2*i + 1] = 44;//means bad hall
				break;
			}
		}
	}
	return ret;
}

unsigned char sleep_flag=0;
void ana_phy_command_sleep(void)
{

	if( sleep_flag==0)
	{
			sleep_flag=1;
			ana_mcu_command_head_tail(TD11_DELAY_US,TD12_DELAY_US);
//			ry_delay_ms(3);		//must be wait 3ms above
	}
}
void ana_phy_command_wakeup(void)
{

	if( sleep_flag==1)
	{
			sleep_flag=0;
			ana_mcu_command_head_tail(TD12_DELAY_US,TD13_DELAY_US);
			ry_delay_ms(2);
	}
}
#endif
/* end of file */

