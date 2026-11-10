/*******************************************************************************
 * @file    my_hallkey_func.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-04-14
 * @version V0.1
 * @note
*******************************************************************************/
#include "my_hallkey_func.h"

#define ANA_MODE_CONTINUE  0x80
#define ANA_MODE_ONCE      0x00

#define length_filter_bit		10
#define FILTER_SKIP_VALUE		60

#define TOP_INIT_VALUE      9000
#define BOTTOM_INIT_VALUE   8300
#define AJUST_VALUE    0			//固定为0

#define TOP_BASELINE_NOISE		6
#define TOP_BASELINE_STEP_FAST	4

//                      RightButton, LeftButton
uint8_t key_state[ANA_TOTAL_COUNT] = {0, 0};
uint16_t baseline_top[ANA_TOTAL_COUNT] = {9400, 9400};
uint16_t baseline_bottom[ANA_TOTAL_COUNT] = {8300, 8300};
uint16_t rt_press_value[ANA_TOTAL_COUNT] = {3, 3};
uint16_t rt_release_value[ANA_TOTAL_COUNT] = {3, 3};
uint16_t node[ANA_TOTAL_COUNT] = {0};
uint8_t crc_error_count = 0;
uint16_t cali_init_cnt = 600;
uint16_t cali_gain_cnt[ANA_TOTAL_COUNT] = {0};
unsigned char ana_mode=ANA_MODE_CONTINUE ;
extern const unsigned char bit_error[256];

unsigned short raw_data_filter[ANA_TOTAL_COUNT];
unsigned int raw_data_filter_sum[ANA_TOTAL_COUNT];

uint32_t baseline_step[ANA_TOTAL_COUNT] = {0};

unsigned short bottom_baseline_adjust_cnt[ANA_TOTAL_COUNT];
unsigned char if_adjust_flag[ANA_TOTAL_COUNT];

uint16_t top_timer_cnt[ANA_TOTAL_COUNT] = {0}; 
uint32_t sample_top_sum1[ANA_TOTAL_COUNT] = {0};
uint32_t sample_top_sum2[ANA_TOTAL_COUNT] = {0};
uint16_t top_update_cnt1[ANA_TOTAL_COUNT] = {0};
uint16_t top_update_cnt2[ANA_TOTAL_COUNT] = {0};
uint16_t top_up_cnt[ANA_TOTAL_COUNT] = {0};
uint16_t top_down_cnt[ANA_TOTAL_COUNT] = {0};



const unsigned char node_cali_table[256] = 
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 8, 9, //15
  10, 11, 12, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, //16
  20, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, //18
  30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, //19
  40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, //20

  50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 58, 59, 59, 59, //22
  
  60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, //24
  70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, //26
  
  80, 80, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 87, 88, 88, 88, 89, 89, 89, //28

  90, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 93, 94, 94, 94, 95, 95, 95, 96, 96, 96, 97, 97, 97, 98, 98, 98, 98, 99, 99, 99, 99, //32
  
  100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 //36
};

  
void key_rapid_trigger_handle(unsigned short segment, unsigned char key_index)
{
  static uint16_t current_node_update[ANA_TOTAL_COUNT] = {0};
	unsigned short release_dis, press_dis;
	unsigned short current_node_update_temp;
	
	press_dis = rt_press_value[key_index] * 10; //rapid trigger按下触发行程,rt_press_value(驱动设定最大10)*10对应最大100分段值
	release_dis = rt_release_value[key_index] * 10;
  
//	if(current_node_update[key_index] < 10)								//考虑初段hall值变化较小，为排除noise影响，对初段过程中的rapid trigger的触发值进行调整
//	{                                                                   
//		if(press_dis <= 20)press_dis = 20;                                
//		if(release_dis <= 20)release_dis = 20;                            
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
		if(segment >= 100)                                                 
		{                                                                       
			key_state[key_index] = 1;													//当检测到分段值为100时，按键状态一定为触发状态
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

void baseline_data_init(unsigned char index)
{
		top_timer_cnt[index] = 0;
		sample_top_sum1[index] = 0;
		sample_top_sum2[index] = 0;
		top_update_cnt1[index] = 0;
		top_update_cnt2[index] = 0;
		top_up_cnt[index] = 0;
		top_down_cnt[index] = 0;
}


void baseline_cali_handle(unsigned char key_index, unsigned short raw_data)
{
  
//	if((raw_data > 9000) || (raw_data < 128))							//异常数据跳过
//	{
//		crc_error_count++;
//		
//		return;
//	}
	
	if(raw_data > TOP_INIT_VALUE)
	{
		top_timer_cnt[key_index]++;
		
		if(cali_init_cnt || cali_gain_cnt[key_index])			//上电快速校准top baseline 600times,增益校准后
		{
			if(cali_gain_cnt[key_index])cali_gain_cnt[key_index]--;		//完成一次校准后，cnt--
			
			if(top_timer_cnt[key_index] >= 1)
			{
				top_timer_cnt[key_index] = 0;
					
				if(raw_data > (TOP_BASELINE_NOISE + baseline_top[key_index]))
				{
					baseline_top[key_index] += TOP_BASELINE_STEP_FAST;
				}
				else if(baseline_top[key_index] > (TOP_BASELINE_NOISE + raw_data))
				{ 
					baseline_top[key_index] -= TOP_BASELINE_STEP_FAST;
				}
			}
			
			if((cali_init_cnt <= 1) && (cali_gain_cnt[key_index] <= 1))		//最后一次再记录top baseline，并计算step
			{
				top_update_cnt1[key_index] = 0;
				sample_top_sum1[key_index] = 0;
				top_update_cnt2[key_index] = 0;
				
				baseline_step[key_index] = (unsigned int)((1 << 21) / (baseline_top[key_index] - (baseline_bottom[key_index] + AJUST_VALUE)));	//考虑底部晃动导致大的raw data差异，bottom baseline加上一个矫正值，参考值500
			}
		}
		else										//正常工作过程中校准top baseline
		{
			if(top_timer_cnt[key_index] >= 15)		//8K情况下，所有按键校准处理一次一个周期，15个周期高于6000的raw data记录一次
			{
				top_timer_cnt[key_index] = 0;
				sample_top_sum1[key_index] += raw_data;
				top_update_cnt1[key_index]++;			//raw data累加计数
				
				if(top_update_cnt1[key_index] == 8)			
				{				
					top_update_cnt1[key_index] = 0;
					
					sample_top_sum1[key_index] >>= 3;		//8次取平均值
					
					if(sample_top_sum1[key_index] > (TOP_BASELINE_NOISE + baseline_top[key_index]))
					{
						top_update_cnt2[key_index] = 0;
						
						top_down_cnt[key_index] = 0;
						top_up_cnt[key_index]++;
						if(top_up_cnt[key_index] >= 1)		//每次平均值超过当前top baseline 6后，更新top baseline
						{
							top_up_cnt[key_index] = 0;
							
							baseline_top[key_index] += 1;
						
							baseline_step[key_index] = (unsigned int)((1 << 21) / (baseline_top[key_index] - (baseline_bottom[key_index] + AJUST_VALUE)));	//考虑底部晃动导致大的raw data差异，bottom baseline加上一个矫正值，参考值500
						}
							
					}
					else if(baseline_top[key_index] > sample_top_sum1[key_index])
					{
						top_update_cnt2[key_index]++;				//平均值低于当前top baseline计数
						
						if(top_update_cnt2[key_index] == 16)		//累计16次平均值低于当前top baseline，确认一次向下数据
						{
							top_update_cnt2[key_index] = 0;

							top_down_cnt[key_index]++;
							top_up_cnt[key_index] = 0;
							if(top_down_cnt[key_index] >= 5)		//确认5次向下数据后，更新top baseline
							{
								top_down_cnt[key_index] = 0;
								
								baseline_top[key_index] -= 5;
								
								baseline_step[key_index] = (unsigned int)((1 << 21) / (baseline_top[key_index] - (baseline_bottom[key_index] + AJUST_VALUE)));	//考虑底部晃动导致大的raw data差异，bottom baseline加上一个矫正值，参考值500
							}
						}
					}
					else
					{
						top_down_cnt[key_index] = 0;
						top_up_cnt[key_index] = 0;
						top_update_cnt2[key_index] = 0;
					}
					
					sample_top_sum1[key_index] = 0;
				}
			}
		}
		
	}
	//else if(raw_data < 1000)									//读到低于250的raw data自动校准增益值
	//{
	//	if(p_cur_gain_buff[key_index] > ADC_REF_MIN_VALUE)p_cur_gain_buff[key_index]--;					//增益自减
	//	if_has_gain_adjust = 1;
	//	mcu_slave_cmd.cmd.cali_gain_cnt[key_index] = 600;
	//	
	//	baseline_bottom[key_index] = BOTTOM_INIT_VALUE;
	//	baseline_step[key_index] = (unsigned int)((1 << 21) / (baseline_top[key_index] - (baseline_bottom[key_index] + AJUST_VALUE)));
	//}
	else if(raw_data < BOTTOM_INIT_VALUE)
	{
		if(bottom_baseline_adjust_cnt[key_index] <= 1024)bottom_baseline_adjust_cnt[key_index]++;			//8K情况下，计数低于bottom baseline初值的前1024包raw data,共128ms 
		
		if(baseline_bottom[key_index]  > (1000 + raw_data))			//低于当前bottom baseline 1000以上，直接更新bottom baseline为raw data + 400	
		{
				baseline_bottom[key_index] = raw_data + 400;	
				baseline_step[key_index] = (unsigned int)((1 << 21) / (baseline_top[key_index] -(baseline_bottom[key_index] + AJUST_VALUE)));
		}
		else if(baseline_bottom[key_index]  > (50 + raw_data))			//raw data 低于当前bottom baseline 50以上时进行校准
		{			
				if(bottom_baseline_adjust_cnt[key_index] <= 1024)				//前1024包数据有效
				{
					if(if_adjust_flag[key_index] == 0)						//保证每次按下时，如果前1024包中有满足校准条件的数据，至少第一次会校准
					{
						if_adjust_flag[key_index] = 1;

						baseline_bottom[key_index] = (baseline_bottom[key_index] + raw_data) >> 1;			//更新成raw data和bottom baseline的平均值
						baseline_step[key_index] = (unsigned int)((1 << 21) / (baseline_top[key_index] - (baseline_bottom[key_index] + AJUST_VALUE)));
					}
					else if((bottom_baseline_adjust_cnt[key_index] & 0xff) == 0)			//每256包数据有效
					{
						baseline_bottom[key_index] = (baseline_bottom[key_index] + raw_data) >> 1;	
						baseline_step[key_index] = (unsigned int)((1 << 21) / (baseline_top[key_index] - (baseline_bottom[key_index] + AJUST_VALUE)));
					}
				}
		}
		
		baseline_data_init(key_index);
	}
	else
	{
		baseline_data_init(key_index);
	}
}


unsigned short ana_phy_bit_error_checker2(unsigned char sample_hi,unsigned char sample_lo)
{
	  unsigned char sample_hi_shade = sample_hi & 0x3f;//低6位
	
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

unsigned char ana_calc_node_data(unsigned char index, unsigned short raw_data)
{
  uint8_t ret = 0;
  uint16_t sample_noise = 0;
  uint32_t calc_node = 0;
  
	if(raw_data < (baseline_bottom[index]))
	{
		ret = 255;
	}
  else if(raw_data > baseline_top[index])
  {
    ret = 0;
  }
	else
	{
		sample_noise = baseline_top[index] - raw_data;

		calc_node = (unsigned int)((sample_noise * baseline_step[index] * 255) >> 21);
		if(calc_node > 255)calc_node = 255;
	}
  
  ret = node_cali_table[calc_node];
  
  return  ret;
}


void ProcessRecvSampleRawData(uint8_t* pData, uint16_t length)
{
  uint8_t *ana_rd_buff = pData;
  uint16_t temp[ANA_TOTAL_COUNT];
  
  
  if(cali_init_cnt)
  {
    cali_init_cnt--;
    for(uint8_t k = 0; k<ANA_TOTAL_COUNT; k++)
    {
      temp[k] = ana_phy_bit_error_checker2(ana_rd_buff[2*k] , ana_rd_buff[2*k + 1]);		//获取raw data

      if(temp[k] != CRC_ERROR)
      {
      #if MAGNETIC_POLARITY_N
        temp[k] = 15927 - temp[k];
      #endif
        
        if((raw_data_filter[k] > (temp[k] + FILTER_SKIP_VALUE)) || ((raw_data_filter[k] + FILTER_SKIP_VALUE) < temp[k]))
        {
          raw_data_filter[k] = temp[k];
          raw_data_filter_sum[k] = temp[k] << length_filter_bit;
        }
        else
        {
          raw_data_filter_sum[k] += temp[k];
          raw_data_filter_sum[k] -= raw_data_filter[k];
          raw_data_filter[k] = raw_data_filter_sum[k] >> length_filter_bit;
          temp[k] = raw_data_filter[k];
        }
        
        baseline_cali_handle(k, temp[k]);		//baseline初始化校准,一次所有按键
      }
      else
      {// error
        crc_error_count++;
      }
    }
  }
  else
  {
    for(uint8_t k = 0; k<ANA_TOTAL_COUNT; k++)
    {
      temp[k] = ana_phy_bit_error_checker2(ana_rd_buff[2 * k], ana_rd_buff[2 * k + 1]);		//获取raw data

      if (temp[k] != CRC_ERROR)
      {
#if MAGNETIC_POLARITY_N						//如果磁铁N极向sensor，调返hall值变化方向
        temp[k] = 15927 - temp[k];
#endif

//        if ((temp[k] > 9000) || (temp[k] < 128))							//异常数据跳过
//        {
//          crc_error_count++;
//          k++;
//          continue;
//        }

        if ((raw_data_filter[k] > (temp[k] + FILTER_SKIP_VALUE)) || ((raw_data_filter[k] + FILTER_SKIP_VALUE) < temp[k]))		//实际raw data相较滤波后的值变化超过40后，直接跟随
        {
          raw_data_filter[k] = temp[k];
          raw_data_filter_sum[k] = temp[k] << length_filter_bit;
        }
        else
        {
          raw_data_filter_sum[k] += temp[k];
          raw_data_filter_sum[k] -= raw_data_filter[k];
          raw_data_filter[k] = raw_data_filter_sum[k] >> length_filter_bit;
          temp[k] = raw_data_filter[k];
        }
        
        baseline_cali_handle(k, temp[k]);
        
        node[k] = ana_calc_node_data(k, temp[k]);
        
        key_rapid_trigger_handle(node[k], k);
      }
    }
  }
}
