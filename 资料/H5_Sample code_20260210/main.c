#include "test.h"
#include "device.h"
#include "keydef.h"
#include "stdlib.h"
#include "pc_communicate.h"
#include "wwdt.h"
#include "hal_spi.h"
#include "usbhw.h"
//#include ".\Led_Layer\light_effect_breathe.h"
//#include "operation_flash.h"

#define MAX_PACKET_SIZE_VENDOR_INPUT	64
#define CRC_ERROR	0xffff
#define TRANSFER_LENGTH 64
UINT8 receive_buff[TRANSFER_LENGTH];
UINT8 test_mode_start = 0;

typedef struct _ANA_FACTROY_UNIT
{				
	unsigned short checksum;
	unsigned char data[ANA_TOTAL_COUNT]; 
} ANA_FACTROY_UNIT;

typedef struct _ANA_FACTROY_UNIT2
{        
	unsigned short checksum;            
	unsigned char data_l[ANA_TOTAL_COUNT];
	unsigned char data_h[ANA_TOTAL_COUNT];
} ANA_FACTROY_UNIT2;

typedef struct _ANA_FACTROY_CALI_DATA_RAM
{
	ANA_FACTROY_UNIT2 ana_average;
	ANA_FACTROY_UNIT 	ana_nosie;

} ANA_FACTROY_CALI_DATA_RAM;


unsigned int sample_value_average[ANA_TOTAL_COUNT];
unsigned char sample_value_noise[ANA_TOTAL_COUNT];

unsigned char ana_hal_n_sample_average_noise(unsigned char count)
{
	unsigned char i,j;
	unsigned short sample_value_temp;
	unsigned short sample_value_max[ANA_TOTAL_COUNT];
	unsigned short sample_value_min[ANA_TOTAL_COUNT];
	unsigned short noise;
	for(i = 0; i < ANA_TOTAL_COUNT; i++)
	{
		sample_value_average[i] = 0;
		sample_value_max[i] = 0;
		sample_value_min[i] =0xffff;
	}
	
	for(j = 0; j < 10; j++)
	{
		ana_phy_sensor_convert(ANA_TOTAL_COUNT,ana_wr_buff,ana_rd_buff);			//调整增益后，前面几次读到的数据滤掉
		                                                                            
		ry_delay_ms(1);                                                             
	}                                                                               
	for(j=0;j< count;j++)															//读回一定包数的hall数据，计算平均值和noise
	{
		ana_phy_sensor_convert(ANA_TOTAL_COUNT,ana_wr_buff,ana_rd_buff); 
	
		for(i = 0; i < ANA_TOTAL_COUNT; i++)
		{									
			sample_value_temp = ana_phy_bit_error_checker2(ana_rd_buff[2*i],ana_rd_buff[2*i+1]);
			if((sample_value_temp!=CRC_ERROR) && (sample_value_temp!=0))
			{
				sample_value_average[i] += sample_value_temp;
				if(sample_value_temp>sample_value_max[i])
				{
						sample_value_max[i]=sample_value_temp;
				}
				if(sample_value_temp<sample_value_min[i])
				{
						sample_value_min[i]=sample_value_temp;
				}
			}
			else
			{
				return 0;
			}
		}
		
	}
	// average and noise
	for(i = 0; i < ANA_TOTAL_COUNT; i++)
	{
			sample_value_average[i] /= count;		
			noise = sample_value_max[i]-sample_value_min[i];
			if(noise > 255)
			{
				sample_value_noise[i] = 0xff;
			}
			else
			{
				sample_value_noise[i] = (unsigned char)noise;
			}
	}
	return 1;
}

unsigned char p_cur_gain_buff[ANA_TOTAL_COUNT];

void ana_hall2_hal_key_down_calibrate(unsigned char count)
{
	unsigned short i, key_down_times;
	unsigned char verify_start = 0;
	unsigned char cnt;

	for(i = 0; i < ANA_TOTAL_COUNT; i++)
    {
		p_cur_gain_buff[i] = ADC_REF_CALI_INIT_VALUE;
	}

	ana_hall5_phy_set_gain(ANA_TOTAL_COUNT,p_cur_gain_buff);
	
//	;Calibrate times is 30
	for(key_down_times = 0; key_down_times < 16; key_down_times++)
    {
		cnt = 50;
		wdt_hal_feed();		//清狗，避免watchdog reset
		ana_hall5_phy_set_gain(ANA_TOTAL_COUNT,p_cur_gain_buff);
		
		while((ana_hal_n_sample_average_noise(count)==0) && (cnt--))
		{
			wdt_hal_feed();
		}
		// calc adc ref
		verify_start = 0;
			
		for(i = 0; i < ANA_TOTAL_COUNT; i++)
		{
			if(sample_value_average[i] > KEY_DOWN_START_SAMPLE_VALUE)
			{
				if(sample_value_average[i] < KEY_DOWN_MAX_SAMPLE_VALUE)
				{
					if (sample_value_noise[i] > 60)
					{
						verify_start = 1;

						if (p_cur_gain_buff[i] > ADC_REF_MIN_VALUE)
						{
							p_cur_gain_buff[i]--;
						}
					}
				}
				else
				{
					verify_start = 1;

					if (p_cur_gain_buff[i] > ADC_REF_MIN_VALUE)
					{
						p_cur_gain_buff[i]--;
					}
				}
			}
		}

		if(verify_start == 0)
		{
			return;
		}
    }
}

void mcu_slave_factory_cali_start_bottom(unsigned char count)
{
	ana_hall2_hal_key_down_calibrate(count);	
		
	unsigned char i, j, k = 0;
	
	for(j = 0;j<KB_ROW_ANA;j++)
	{
		Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));
		receive_buff[0] = 4;
		receive_buff[1] = 0x20;
		receive_buff[2] = 0x07;
		for (i = 0;i <KB_COL_ANA;i++)
		{
			if(key_code_normal_default[i][j].keycode_L != RES)
			{
				if(k<ANA_TOTAL_COUNT)
				{
					receive_buff[i * 4 + 3] = key_code_normal_default[i][j].keycode_L;
					receive_buff[i * 4 + 4] = sample_value_average[k] & 0x00ff;//LL
					receive_buff[i * 4 + 5] = (sample_value_average[k]>>8) & 0x00ff;//HH
					receive_buff[i * 4 + 6] = sample_value_noise[k];
				}
				k++;
			}
		}

		USB_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 
		TimerDelayMsec(2);
	}
}

void mcu_slave_factory_cali_end(unsigned char flash_addr)
{
		__align(4) ANA_FACTROY_CALI_DATA_RAM	cali_data;
			
	//将最终校准成功的数据保存到flash
		for (unsigned char k = 0;k <ANA_TOTAL_COUNT;k++)
		{	
				cali_data.ana_average.data_l[k] = sample_value_average[k] & 0x00ff;//LL
				cali_data.ana_average.data_h[k] = (sample_value_average[k]>>8) & 0x00ff;//HH
				cali_data.ana_nosie.data[k] = 	  sample_value_noise[k];
		}
		cali_data.ana_average.checksum = calc_factory_unit2_checksum(&cali_data.ana_average);
		cali_data.ana_nosie.checksum = calc_factory_unit_checksum(&cali_data.ana_nosie);

		factory_data_store(flash_addr,(unsigned char*)&cali_data.ana_average);
}

void mcu2_spi_test_command(unsigned char* buff)
{
	UINT8 data_type, test_type = buff[10];
	
	switch(test_type)
	{
		case SET_FACTORY_CALI_BOTTOM_START:		//0xD2,校准开始
			mcu_slave_factory_cali_start_bottom(buff[11]);
			break;
		
		case SET_FACTORY_CALI_BOTTOM_END:		//0xE2,校准结果
			if(buff[11])						//校准成功
			{// pass                            
				ana_hall2_hal_save_gain();			//将缓存在p_cur_gain_buff[]中的增益保存到flash

				mcu_slave_factory_cali_end(ANA_FACTROY_CALI_DATA_BOTTOM_ADDR);	//将校准的BOTTOM平均值和noise保存到flash
			}
			break;
		default:
			test_type = 0;
			break;
	}
}

void mcu2_spi_test_init(void)
{
	test_mode_start = 0;
	
	device_mode = MODE_TEST;
	
	all_key_release();
}

void set_report_format(UINT8 type)
{
	mcu2_data_type = type;
	
	test_mode_start = 1;
}



void PC_Communicate(void)
{
	Tool_Clear_ArrayData(receive_buff,0,63);

	if (!Usb_Read_Bytes(EP_4_CUSTION_IN,receive_buff,64))
	{
		return;
	}

	switch(receive_buff[0])
	{		
		case 0x04:
			switch(receive_buff[1])
			{
				case HtoD_ENTER_MFG_MODE:						//0x04,进入生产模式
					mcu2_spi_test_init();                       
					break;                                      
				                                                
				case HtoD_ENTER_NORMAL_MODE:					//0x05,进入正常工作模式
					Device_Set_Mode(MODE_NORMAL);               
					Custom_Load_ModeSetting();                  
					break;                                      
                                                                
				case HtoD_CALI_CMD2:							//0x10,生产校准
					mcu2_spi_test_command(receive_buff);        
					break;                                      
                                                                
				case HtoD_SET_REPORT_FORMAT:					//0x17,设置数据格式
					set_report_format(receive_buff[10]);        
					break;                                      
				case HtoD_GET_MES:								//0x19,获取设备保存的MES SN
					host_get_mes();                             
					break;                                      
				case HtoD_SET_MES:								//0x1a,将MES SN写入设备保存
					host_set_mes(receive_buff);					
					break;				

				default:
					break;
			}
			break;

		default:
			break;
	}	
	
	Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));
}
void host_set_mes(UINT8* buff)
{
	UINT8 i;
	
	mode_setting.mes_length = buff[10];
	
	for(i = 0; i < buff[10]; i++)
	{
		mode_setting.mes[i] = buff[11 + i];
	}
	
	OperateFlash_Store_ModeSetting(mode_setting);
}

void host_get_mes(void)
{
	UINT8 i;
	receive_buff[0] = 4;
	receive_buff[1] = 0x23;
	receive_buff[2] = mode_setting.mes_length;//19
	
	for(i = 0; i < buff[2]; i++)
	{
		receive_buff[3 + i] = mode_setting.mes[i];
	}
	
	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH);
	TimerDelayMsec(2);
}

void upload_hal_data_1_byte(UINT8 data_type)
{
	unsigned char i, j, k = 0;
	
	for(j = 0;j<KB_ROW_ANA;j++)
	{
		Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));
		receive_buff[0] = 4;
		receive_buff[1] = 0x20;
		receive_buff[2] = data_type;				
		for (i = 0;i <KB_COL_ANA;i++)
		{
			if(key_code_normal_default[i][j].keycode_L != RES)
			{
				if(k<ANA_TOTAL_COUNT)
				{
					receive_buff[i * 4 + 3] = key_code_normal_default[i][j].keycode_L;

					receive_buff[i * 2 + 4] = ana_rd_buf[MCU_CMD_LENGTH + k];
				}
				k++;
			}
		}

		USB_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 
		TimerDelayMsec(2);
	}
}

void upload_hal_data_2_byte(UINT8 data_type)
{
	unsigned char i, j, k = 0;
	
	for(j = 0;j<KB_ROW_ANA;j++)
	{
		Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));
		receive_buff[0] = 4;
		receive_buff[1] = 0x20;
		receive_buff[2] = data_type;
		for (i = 0;i <KB_COL_ANA;i++)
		{
			if(key_code_normal_default[i][j].keycode_L != RES)
			{
				if(k<ANA_TOTAL_COUNT)
				{
					receive_buff[i * 4 + 3] = key_code_normal_default[i][j].keycode_L;
					receive_buff[i * 3 + 4] = ana_rd_buf[MCU_CMD_LENGTH + 2 * k];					//raw data低8位
					receive_buff[i * 3 + 5] = ana_rd_buf[MCU_CMD_LENGTH + 2 * k + 1];				//raw data高8位
				}
				k++;
			}
		}

		USB_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 
		TimerDelayMsec(2);
	}
}

void upload_hal_data_3_byte(UINT8 data_type)
{
	unsigned char i, j, k = 0;
	
	for(j = 0;j<KB_ROW_ANA;j++)
	{
		Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));
		receive_buff[0] = 4;
		receive_buff[1] = 0x20;
		receive_buff[2] = data_type;
		for (i = 0;i <KB_COL_ANA;i++)
		{
			if(key_code_normal_default[i][j].keycode_L != RES)
			{
				if(k<ANA_TOTAL_COUNT)
				{
					receive_buff[i * 4 + 3] = key_code_normal_default[i][j].keycode_L;
					receive_buff[i * 4 + 4] = ana_rd_buf[MCU_CMD_LENGTH+3 * k];					//BOTTOM校准平均值低8位
					receive_buff[i * 4 + 5] = ana_rd_buf[MCU_CMD_LENGTH+3 * k + 1];				//BOTTOM校准平均值高8位
					receive_buff[i * 4 + 6] = ana_rd_buf[MCU_CMD_LENGTH+3 * k + 2];				//BOTTOM校准noise
				}
				k++;
			}
		}

		USB_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 
		TimerDelayMsec(2);
	}
}

void upload_hal_data_version(UINT8 data_type)
{
	Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));
	receive_buff[0] = 4;
	receive_buff[1] = 0x20;
	receive_buff[2] = data_type;

	receive_buff[3] = ana_rd_buf[MCU_CMD_LENGTH];
	receive_buff[4] = ana_rd_buf[MCU_CMD_LENGTH + 1];
	receive_buff[5] = ana_rd_buf[MCU_CMD_LENGTH + 2];
	receive_buff[6] = ana_rd_buf[MCU_CMD_LENGTH + 3];

	USB_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 
	TimerDelayMsec(2);
}

void test_scan(void)
{
	if(!test_mode_start)return;
	
	if(ana_hal_sample_test_mode(1))	//根据不同的数据类型mcu2_data_type获取相应的数据，缓存在ana_rd_buf[]中
	{
		switch(mcu2_data_type)			//根据不同的数据类型mcu2_data_type选择不同的格式上报
		{
			case MCU_CMD_H2S_SET_REPORT_FORMAT_RAW_DATA:				//0x00, raw data
				upload_hal_data_2_byte(mcu2_data_type);                 
				break;                                                  
			case MCU_CMD_H2S_SET_REPORT_FORMAT_CALI_DATA:				//0x01, 矫正分段值
				upload_hal_data_1_byte(mcu2_data_type);                 
				break;                                                  
			case MCU_CMD_H2S_SET_REPORT_FORMAT_CALI_ANALOG_DATA:		//0x04,	analog分段值
				upload_hal_data_1_byte(mcu2_data_type);                 
				break;                                                  
			case MCU_CMD_H2S_SET_REPORT_FORMAT_SUNDRY_DATA:				//0x03, 杂项数据
				upload_hal_data_version(mcu2_data_type);                
				break;                                                  
			case MCU_CMD_H2S_SET_REPORT_FORMAT_BOTTOM_CORRECT_DATA:		//0x07, BOTTOM校准平均值&noise，此数据要从flash中读出
				upload_hal_data_3_byte(mcu2_data_type);                 
				break;                                                  
			case MCU_CMD_H2S_SET_REPORT_FORMAT_HALL_SELFCHECK_DATA:		//0x0c, hall自检值
				upload_hal_data_1_byte(mcu2_data_type);
				break;

			default:
				break;				
		}
	}
}

void device_test_scan(void)
{
	if (!one_ms_cycle)
	{
		one_ms_cycle = 10;//10=10ms
		
		test_scan();
	}
	

}

void key_rapid_trigger_handle(unsigned short segment, unsigned char key_index)
{
	unsigned short release_dis, press_dis;
	unsigned short current_node_update_temp;
	
	press_dis = trigger_value_temp[key_index].rt_press_value * 10;		//rapid trigger按下触发行程,rt_press_value(驱动设定最大40)*10对应最大400分段值
	release_dis = trigger_value_temp[key_index].rt_release_value * 10;	//rapid trigger抬起释放行程,rt_release_value(驱动设定最大40)*5对应最大400分段值
	                                                                    
//	if(current_node_update[key_index] < 80)								//考虑初段hall值变化较小，为排除noise影响，对初段过程中的rapid trigger的触发值进行调整
//	{                                                                   
//		if(press_dis <= 16)press_dis = 16;                                
//		if(release_dis <= 16)release_dis = 16;                            
//	}                                                                   
	                                                                    
	if(key_state[key_index])											//按键处于按触发状态
	{                                                                   
		if(segment == 0)                        
		{                                                               
			key_state[key_index] = 0;									//当检测到分段值0时，按键状态一定为释放状态
			current_node_update[key_index] = segment;					//记录释放状态下的最高点分段值
		}                                                               
		else                                                            
		{                                                               
			if(segment > current_node_update[key_index])				//当前的分段值大于触发状态下记录的最低点分段值
			{                                                           
				current_node_update[key_index] = segment;				//更新触发状态下最低点分段值
			}
			else
			{
				if(current_node_update[key_index] > (segment + release_dis))	//相对于记录的最低点分段值，当前分段值抬起超过rapid trigger抬起释放行程
				{                                                               
					key_state[key_index] = 0;									//切换按键为释放状态
					current_node_update[key_index] = segment;					//记录释放状态下的最高点分段值
				}                                                               
			}                                                                   
		}                                                                       
	}                                                                           
	else                                                                        
	{                                                                           
		if(segment >= 350)                                                 
		{                                                                       
			key_state[key_index] = 1;													//当检测到分段值为350时，按键状态一定为触发状态
			current_node_update[key_index] = segment;                                   //记录触发状态下的最低点分段值
		}                                                                       
		else                                                                    
		{                                                                       
			current_node_update_temp = current_node_update[key_index];          
			                                                                    
			if(segment < current_node_update_temp)										//当前的分段值小于释放状态下记录的最高点分段值
			{                                                                           
				current_node_update[key_index] = segment;                               //更新释放状态下最高点分段值
			}                                                                   
			else                                                                
			{                                                                   
				if(segment >= (current_node_update_temp + press_dis))			//相对于记录的最高点分段值，当前分段值按下不低于rapid trigger抬起触发行程
				{                                                               
					key_state[key_index] = 1;									//切换按键为按下状态
					current_node_update[key_index] = segment;					//记录触发状态下的最低点分段值
				}
			}
		}
	}
}

void device_normal_scan(void)
{
// normal mode	 

	if(!tick_ana_esd_protect)
	{				
		if(ana_hal_esd_protect() == 0)
		{
			one_ms_cycle= 5;		//5ms 初始化完等待5ms才开始读数据
		}
		tick_ana_esd_protect = 5000;		//每5s做一次hall自检
	}
	
	if (!one_ms_cycle)
	{
		one_ms_cycle = 1;//1=1ms
		
		if(ana_hal_sample(1))
		{
			for(unsigned char i = 0; i < ANA_TOTAL_COUNT; i++)
			{
				key_rapid_trigger_handle(node[i], i);
			}				
		}	
	}

}

unsigned char  ana_hal_data_handle_suspend(void)
{
	unsigned char k=0,ret=0;
	unsigned short temp;
	for (k = 0;k <ANA_TOTAL_COUNT;k++)
	{
//																					HH								LL					
			temp = ana_phy_bit_error_checker2(ana_rd_buff[2*k],ana_rd_buff[2*k + 1]);

			if(temp!=CRC_ERROR)
			{	
			#if MAGNETIC_POLARITY_N
			temp = 15927 - temp;
				#endif			
	
				if(temp<TOP_INIT_VALUE)
				{
					ret = 1;	
				}
			}else
			{// error
				
				ret = 0;
				break;
			}
				
	 }

	return ret;
}


void check_usb_suspend(void)
{

	unsigned char key_changed=0;
	if (get_usb_dev_state() == SUSPENDED)
	{// start enter suspended

	#if SYSTICK_ENALBE 
		systick_disable();
	#endif

		ry_delay_ms(1);


		ana_hal_led_enable(0);
		ana_phy_command_sleep();
	
		rtc1_hal_start();
		rtc1_hal_tick0_enable(RTC_300ms);

		ry_delay_ms(1);

		while(get_usb_dev_state() == SUSPENDED)
		{
			wdt_hal_feed();
			
			pmu_enter_sleep(SLPMODE_DEEP_SLEEP);//SLPMODE_SLEEP
			ry_delay_ms(1);
	
			if(get_usb_bm_state() & USB_BM_STATE_ALLOW_REMOTE_WAKEUP)
			{									
				if(rtc1_hal_tick0_timeout())
				{
					rtc1_hal_tick0_reset();
						ana_phy_command_wakeup();	
						
						if(ana_hal_sample(0))
						{
							key_changed = ana_hal_data_handle_suspend();
							if(key_changed)
							{
								mcu_slave_cmd.cmd.sleep=0;

													
								usb_manual_wkup_host();			
								//some pc shutdown but still  power on, and also send set_feature allow remote wakeup. this is bad host.				
								while((get_usb_dev_state() == SUSPENDED)|| (usb_report_ok()==0))
								{
									wdt_hal_feed();
								}
								break;
							
							}	
						}						
					ana_phy_command_sleep();
				}
			
					
			}else  
			{//   
				ry_delay_ms(1);
			}
									
		}	

		rtc1_hal_tick0_disable();
		rtc1_hal_stop();
		ana_phy_command_wakeup();
		ana_hal_led_enable(1);
		
		if(mcu_slave_cmd.cmd.cali_init_cnt < 300)mcu_slave_cmd.cmd.cali_init_cnt = 300;//resume 后做300次快速校准

		ry_delay_ms(100);//500
	#if SYSTICK_ENALBE 
		systick_enable();
	#endif
	}
	
}

void Device_Active(void)
{
	while (1)
	{
		switch(device_mode)
		{
			case MODE_NORMAL:
				device_normal_scan();
				break;
			
			case MODE_TEST:
				device_test_scan();
				break;
			
			default:
				break;
		}
			
		wwdt_feed();
	
		PC_Communicate();
	
		LED_Scan();
			
		check_usb_suspend();
	
	}
}



int main (void)
{
	
	ana_hal_init();	
	
	Device_Active();
		

	return 0;
}