/*******************************************************************************
 * @file    my_misc.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-03-16
 * @version V0.1
 * @note
*******************************************************************************/
#include "my_misc.h"
#include "my_debug.h"
#include "my_tick.h"
#include "my_electromagnet.h"
#include "my_protocol_mouse.h"

#if (defined(IRPT_V4))
const uint8_t nodeCaliTable[256] = 
{
  0 , 0 , 1 , 1 , 1 , 2 , 2 , 3 , 3 , 3 , 
  4 , 4 , 5 , 5 , 5 , 6 , 6 , 7 , 7 , 7 , 
  8 , 8 , 9 , 9 , 9 , 10 , 10 , 11 , 11 , 11 , 
  12 , 12 , 13 , 13 , 13 , 14 , 14 , 15 , 15 , 15 , 
  16 , 16 , 17 , 17 , 17 , 18 , 18 , 19 , 19 , 19 , 
  20 , 20 , 20 , 21 , 21 , 22 , 22 , 22 , 23 , 23 , 
  24 , 24 , 24 , 25 , 25 , 26 , 26 , 26 , 27 , 27 , 
  28 , 28 , 28 , 29 , 29 , 30 , 30 , 30 , 31 , 31 , 
  32 , 32 , 32 , 33 , 33 , 34 , 34 , 34 , 35 , 35 , 
  36 , 36 , 36 , 37 , 37 , 38 , 38 , 38 , 39 , 39 , 
  39 , 40 , 40 , 41 , 41 , 41 , 42 , 42 , 43 , 43 , 
  43 , 44 , 44 , 45 , 45 , 45 , 46 , 46 , 47 , 47 , 
  47 , 48 , 48 , 49 , 49 , 49 , 50 , 50 , 51 , 51 , 
  51 , 52 , 52 , 53 , 53 , 53 , 54 , 54 , 55 , 55 , 
  55 , 56 , 56 , 57 , 57 , 57 , 58 , 58 , 59 , 59 , 
  59 , 60 , 60 , 60 , 61 , 61 , 62 , 62 , 62 , 63 , 
  63 , 64 , 64 , 64 , 65 , 65 , 66 , 66 , 66 , 67 , 
  67 , 68 , 68 , 68 , 69 , 69 , 70 , 70 , 70 , 71 , 
  71 , 72 , 72 , 72 , 73 , 73 , 74 , 74 , 74 , 75 , 
  75 , 76 , 76 , 76 , 77 , 77 , 78 , 78 , 78 , 79 , 
  79 , 79 , 80 , 80 , 81 , 81 , 81 , 82 , 82 , 83 , 
  83 , 83 , 84 , 84 , 85 , 85 , 85 , 86 , 86 , 87 , 
  87 , 87 , 88 , 88 , 89 , 89 , 89 , 90 , 90 , 91 , 
  91 , 91 , 92 , 92 , 93 , 93 , 93 , 94 , 94 , 95 , 
  95 , 95 , 96 , 96 , 97 , 97 , 97 , 98 , 98 , 99 , 
  100, 100, 100, 100, 100, 100
};
#else
const uint8_t nodeCaliTable[256] = 
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 7, 8, 9, //15
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
#endif

uint16_t rt_press_value[ANA_TOTAL_COUNT] = {DEFAULT_RTP, DEFAULT_RTP};
uint16_t rt_release_value[ANA_TOTAL_COUNT] = {DEFAULT_RTR, DEFAULT_RTR};
uint16_t ap_value[ANA_TOTAL_COUNT] = {DEFAULT_AP, DEFAULT_AP};

uint8_t gbKeyState[ANA_TOTAL_COUNT] = {0};
uint16_t cali_init_cnt = 600;
extern const unsigned char bit_error[256];
uint16_t gwTopBaseline[ANA_TOTAL_COUNT] = {10000, 10000};
uint16_t gwBottomBaseline[ANA_TOTAL_COUNT] = {6000, 6000};


extern uint8_t hall_spi_rx[4];

uint8_t Hall_ConvertVKeyToArrayIndex(uint8_t virtualKey)
{
  uint8_t ret = 0;
  
  switch(virtualKey)
  {
    case VK_LBUTTON:
      ret = 0x01;
      break;
    case VK_RBUTTON:
      ret = 0x00;
      break;
    default:
      break;
  }
  return ret;

}



void Hall_UpdateParam(uint8_t virtualKey, const RapidTriggerConfig_ts *pRTcfg)
{
  uint8_t i = Hall_ConvertVKeyToArrayIndex(virtualKey);
  
  if((pRTcfg->bmEnableMask) & (0x01 << 7))
  {
    ap_value[i] = pRTcfg->wSegmentActuationPoint;
    
    if(pRTcfg->wSegmentRapidTriggerPress)
    {
      rt_press_value[i] = pRTcfg->wSegmentRapidTriggerPress;
    }
    else
    {
      rt_press_value[i] = 1;
    }
    
    if(pRTcfg->wSegmentRapidTriggerRelease)
    {
      rt_release_value[i] = pRTcfg->wSegmentRapidTriggerRelease;
    }
    else
    {
      rt_release_value[i] = 1;
    }
  }
  else
  {
    ap_value[i] = DEFAULT_AP;
    rt_press_value[i] = DEFAULT_RTP;
    rt_release_value[i] = DEFAULT_RTR;
  }
}

int32_t Hall_InitBaseline(uint16_t keyIndex, uint16_t rawData, uint16_t *const pTopBaseline, uint16_t *const pBottomBaseline)
{
  static uint16_t counter[ANA_TOTAL_COUNT] = {0};
  uint16_t upperLimit = *pTopBaseline + (MAGNETIC_NOISE);
  uint16_t lowerLimit = *pTopBaseline - (MAGNETIC_NOISE);
  
  if((rawData > upperLimit) || (rawData < lowerLimit))
  {
    *pTopBaseline = rawData;
    *pBottomBaseline = rawData;
    counter[keyIndex] = 0;
    return -1;
  }
  else
  {
    *pTopBaseline = (*pTopBaseline + rawData) / 2;
    *pBottomBaseline = *pTopBaseline;
    counter[keyIndex] += 1;
    if(counter[keyIndex] >= (100 * ONE_MS_COUNT))
    {
      counter[keyIndex] = 100;
      return 0;
    }
  }
  return -1;
}

void Hall_CalibrateTopBaseline(uint16_t keyIndex, uint8_t nowKeySta, uint16_t rawData, uint16_t *const pTopBaseline)
{
  static uint16_t counter1[ANA_TOTAL_COUNT] = {0};
  static uint16_t counter2[ANA_TOTAL_COUNT] = {0};
  static uint16_t counter3[ANA_TOTAL_COUNT] = {0};
  uint16_t topBaseline = *pTopBaseline;

  if(nowKeySta) //press
  {
    counter1[keyIndex] = 0;
    counter2[keyIndex] = 0;
    counter3[keyIndex] = 0;
    return;
  }
  
  if(rawData < (topBaseline - MAGNETIC_NOISE))
  {
    counter1[keyIndex] += 1;
    counter2[keyIndex] += 1;
    counter3[keyIndex] = 0;
    if(counter1[keyIndex] >= (300 * ONE_MS_COUNT))
    {
      counter1[keyIndex] = 0;
      counter2[keyIndex] = 0;
      topBaseline -= MAGNETIC_NOISE;
      *pTopBaseline = topBaseline;
    }
  }
  else if((rawData < topBaseline) /*&& (rawData >= (topBaseline - MAGNETIC_NOISE))*/)
  {
    counter2[keyIndex] += 1;
    counter1[keyIndex] = 0;
    counter3[keyIndex] = 0;
    if(counter2[keyIndex] >= (100 * ONE_MS_COUNT))
    {
      counter2[keyIndex] = 0;
      topBaseline -= 5;
      *pTopBaseline = topBaseline;
    }
  }
  else if((rawData <= (topBaseline + MAGNETIC_NOISE)) && (rawData > topBaseline))
  {
    counter3[keyIndex] += 1;
    counter1[keyIndex] = 0;
    counter2[keyIndex] = 0;
    if(counter3[keyIndex] >= (100 * ONE_MS_COUNT))
    {
      counter3[keyIndex] = 0;
      topBaseline += 2;
      *pTopBaseline = topBaseline;
    }
  }
  else /* if(rawData == topBaseline || rawData > (topBaseline + MAGNETIC_NOISE)) */
  {
    counter1[keyIndex] = 0;
    counter2[keyIndex] = 0;
    counter3[keyIndex] = 0;
  }
}

void Hall_CalibrateBottomBaseline(uint16_t keyIndex, uint8_t nowKeySta, uint16_t rawData, uint16_t *const pBottomBaseline)
{
  static uint8_t pressTimeMs[ANA_TOTAL_COUNT] = {0};
  static uint8_t pressCnt[ANA_TOTAL_COUNT] = {0};
  static uint16_t counter1[ANA_TOTAL_COUNT] = {0};
  static uint16_t counter2[ANA_TOTAL_COUNT] = {0};
  uint16_t bottomBaseline = *pBottomBaseline;
  
/**********time calculate code begin*******************************************/
  if(nowKeySta) //press
  {
    pressCnt[keyIndex] += 1;
    if(pressCnt[keyIndex] >= ONE_MS_COUNT)
    {
      pressCnt[keyIndex] = 0;
      if(pressTimeMs[keyIndex] < 0xFF)
      {
        pressTimeMs[keyIndex] += 1;
      }
    }
  }
  else //release
  {
    pressCnt[keyIndex] = 0;
    pressTimeMs[keyIndex] = 0;
  }
/**********time calculate code end*********************************************/
  if(pressTimeMs[keyIndex] > 100) //100ms window in press
  {
    counter1[keyIndex] = 0;
    counter2[keyIndex] = 0;
    return;
  }
  
  if(rawData > (bottomBaseline + (MAGNETIC_NOISE * 4)))
  {
    
    counter1[keyIndex] += 1;
    counter2[keyIndex] += 1;
    if(counter1[keyIndex] >= (30 * ONE_MS_COUNT))
    {
      counter1[keyIndex] = 0;
      counter2[keyIndex] = 0;
      bottomBaseline += (MAGNETIC_NOISE * 3);
      *pBottomBaseline = bottomBaseline;
    }
    
  }
  else if(rawData > (bottomBaseline + MAGNETIC_NOISE))
  {
    counter2[keyIndex] += 1;
    counter1[keyIndex] = 0;
    if(counter2[keyIndex] >= (30 * ONE_MS_COUNT))
    {
      counter2[keyIndex] = 0;
      bottomBaseline += (MAGNETIC_NOISE / 2);
      *pBottomBaseline = bottomBaseline;
    }
  }
  else
  {
    counter1[keyIndex] = 0;
    counter2[keyIndex] = 0;
  }
}

uint16_t Hall_CalculateNode(uint16_t rawData, uint16_t bottomBaseline, uint16_t topBaseline)
{
  static uint16_t oldBottomBaseline = 0;
  static uint16_t oldTopBaseline = 0;
  static uint32_t step = 0;
  uint8_t tmp = 0;
  uint32_t diff = 0;

  
  if(topBaseline >= bottomBaseline)
  {
    return nodeCaliTable[0];
  }
  else if((bottomBaseline - topBaseline) < 100)
  {
    return nodeCaliTable[0];
  }

  if(rawData <= topBaseline)
  {
    tmp = 0;
  }
  else if(rawData >= bottomBaseline) 
  {
    tmp = 255;
  }
  else
  {
    if((oldBottomBaseline != bottomBaseline) || (oldTopBaseline != topBaseline))
    {
      oldBottomBaseline = bottomBaseline;
      oldTopBaseline = topBaseline;
      step = (uint32_t)((1 << 21) / (bottomBaseline - topBaseline)); // rawData != bottomBaseline
    }
    diff = rawData - topBaseline;
    tmp = (diff * step * 255) >> 21;
  }
  
  return nodeCaliTable[tmp];
}


void Hall_HandleKeyRapidTrigger(uint8_t keyIndex, uint16_t segment)
{
  static uint16_t current_node_update[ANA_TOTAL_COUNT] = {0};
	unsigned short release_dis, press_dis, ap_seg;
	unsigned short current_node_update_temp;
	
  ap_seg = ap_value[keyIndex];
	press_dis = rt_press_value[keyIndex]; //rapid trigger??????,rt_press_value(??????10)*10????100???
	release_dis = rt_release_value[keyIndex];
  
//	if(current_node_update[keyIndex] < 10)								//????hall?????,???noise??,???????rapid trigger????????
//	{                                                                   
//		if(press_dis <= 20)press_dis = 20;                                
//		if(release_dis <= 20)release_dis = 20;                            
//	}                                                                   
  
	if(gbKeyState[keyIndex])											//?????????
	{                                                                   
		if(segment == 0)                        
    //if((segment < ap_seg) || (!segment))
		{                                                               
			gbKeyState[keyIndex] = 0;									//???????0?,???????????
			current_node_update[keyIndex] = segment;					//??????????????
		}                                                               
		else                                                            
		{                                                               
			if(segment > current_node_update[keyIndex])				//??????????????????????
			{                                                           
				current_node_update[keyIndex] = segment;				//?????????????
			}
			else
			{
				if(current_node_update[keyIndex] > (segment + release_dis))	//????????????,?????????rapid trigger??????
				{                                                               
					gbKeyState[keyIndex] = 0;									//?????????
					current_node_update[keyIndex] = segment;					//??????????????
				}                                                               
			}                                                                   
		}                                                                       
	}                                                                           
	else                                                                        
	{                                                                           
		if(segment >= MAX_SEGMENT)                                                 
		{                                                                       
			gbKeyState[keyIndex] = 1;													//????????100?,???????????
			current_node_update[keyIndex] = segment;                                   //??????????????
		}                                                                       
		else                                                                    
		{                                                                       
			current_node_update_temp = current_node_update[keyIndex];          
			                                                                    
			if(segment < current_node_update_temp)										//??????????????????????
			{                                                                           
				current_node_update[keyIndex] = segment;                               //?????????????
			}                                                                   
			else                                                                
			{                                                                   
				//if(segment >= (current_node_update_temp + press_dis))			//????????????,??????????rapid trigger??????
				if((segment >= (current_node_update_temp + press_dis)) && (segment >= ap_seg))
        {                                                               
					gbKeyState[keyIndex] = 1;									//?????????
					current_node_update[keyIndex] = segment;					//??????????????
				}
			}
		}
	}
}

__attribute__((unused)) static unsigned short ana_phy_bit_error_checker2(unsigned char sample_hi,unsigned char sample_lo)
{
	  unsigned char sample_hi_shade = sample_hi & 0x3f;//?6?
	
		if((sample_hi & 0xC0)==(bit_error[sample_hi_shade]^bit_error[sample_lo]))
		{
//			if (ana_mode == ANA_MODE_CONTINUE)
//			{
				return (sample_hi_shade<<8) | sample_lo;
//			}
//			else
//			{
//				return (((sample_hi_shade<<8) | sample_lo) << 4);
//			}
		}else
    {
			return CRC_ERROR;
		}
}

uint8_t ProcessRecvSampleRawData(uint8_t* pData, uint16_t length)
{
#if (defined(IRPT_V4))
  #define length_filter_bit		4
  #define FILTER_SKIP_VALUE		10
#else
  #define length_filter_bit		6
  #define FILTER_SKIP_VALUE		60
#endif
	static uint16_t raw_data_filter[ANA_TOTAL_COUNT] = {0};
	static uint32_t raw_data_filter_sum[ANA_TOTAL_COUNT] = {0};
	
  uint8_t *ana_rd_buff = pData;
  uint16_t temp[ANA_TOTAL_COUNT];
  uint16_t node[ANA_TOTAL_COUNT];
	uint8_t ret = 0;
  #if (!USE_HALL_VALUE_DURING_VIBRATION)
  static uint8_t previousKeySta[ANA_TOTAL_COUNT] = {0};
  #if MYDEBUG_ENABLE
  static uint16_t previousTemp[ANA_TOTAL_COUNT] = {0};
  #endif
  #endif

  if(cali_init_cnt)
  {
    cali_init_cnt--;
    for(uint8_t k = 0; k < ANA_TOTAL_COUNT; k++)
    {
      #if (defined(IRPT_V4))
        temp[k] = ana_rd_buff[2*k];
        temp[k] <<= 8;
        temp[k] |= ana_rd_buff[2*k + 1];
        #if PRESS_KEY_AND_VALUE_DECREASE
          temp[k] = MAX_VALUE_10BIT - temp[k];
        #endif
        if(0 == Hall_InitBaseline(k, temp[k], &(gwTopBaseline[k]), &(gwBottomBaseline[k])))
        {
          cali_init_cnt = 0;
        }
      #else
      temp[k] = ana_phy_bit_error_checker2(ana_rd_buff[2*k] , ana_rd_buff[2*k + 1]);		//??raw data

      if(temp[k] != CRC_ERROR)
      {
        #if PRESS_KEY_AND_VALUE_DECREASE
        temp[k] = MAX_VALUE_14BIT - temp[k];
        #endif
        
        #if 0
        if(temp[k] > (gwTopBaseline[k] + 30))
        {
          gwTopBaseline[k] += 5;
        }
        else if(temp[k] < (gwTopBaseline[k] - 30))
        {
          gwTopBaseline[k] -= 5;
        }
        #else
        if(0 == Hall_InitBaseline(k, temp[k], &(gwTopBaseline[k]), &(gwBottomBaseline[k])))
        {
          cali_init_cnt = 0;
        }
        #endif
      }
      #endif
    }
  }
  else
  {
    for(uint8_t k = 0; k < ANA_TOTAL_COUNT; k++)
    {
      #if (!USE_HALL_VALUE_DURING_VIBRATION)
      if(Ele_IsVibrating(ELE_POS_LEFT))
      {
        if(k == 1) // left button
        {
          gbKeyState[k] = previousKeySta[k];
          #if MYDEBUG_ENABLE
          temp[k] = previousTemp[k];
          #endif
          continue;
        }
      }
      
      if(Ele_IsVibrating(ELE_POS_RIGHT))
      {
        if(k == 0) // right button
        {
          gbKeyState[k] = previousKeySta[k];
          #if MYDEBUG_ENABLE
          temp[k] = previousTemp[k];
          #endif
          continue;
        }
      }
      #endif
      
      #if (defined(IRPT_V4))
        temp[k] = ana_rd_buff[2*k];
        temp[k] <<= 8;
        temp[k] |= ana_rd_buff[2*k + 1];
      #else
        temp[k] = ana_phy_bit_error_checker2(ana_rd_buff[2 * k], ana_rd_buff[2 * k + 1]);		//??raw data
        if (temp[k] != CRC_ERROR)
      #endif
      {
        
        #if (defined(IRPT_V4))
          #if PRESS_KEY_AND_VALUE_DECREASE
            temp[k] = MAX_VALUE_10BIT - temp[k];
          #endif
        #else
          #if PRESS_KEY_AND_VALUE_DECREASE
            temp[k] = MAX_VALUE_14BIT - temp[k];
          #endif
        #endif
          if((raw_data_filter[k] > (temp[k] + FILTER_SKIP_VALUE)) || ((raw_data_filter[k] + FILTER_SKIP_VALUE) < temp[k]))		//??raw data???????????40?,????
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
				
        node[k] = Hall_CalculateNode(temp[k], gwBottomBaseline[k], gwTopBaseline[k]);
        Hall_HandleKeyRapidTrigger(k, node[k]);
        Hall_CalibrateTopBaseline(k, gbKeyState[k], temp[k], &(gwTopBaseline[k]));
        Hall_CalibrateBottomBaseline(k, gbKeyState[k], temp[k], &(gwBottomBaseline[k]));
        
        if(0 == k) // right
        {
          Ele_UpadteSegment(VK_RBUTTON, node[0]);
        }
        else //if(1 == k) //left
        {
          Ele_UpadteSegment(VK_LBUTTON, node[1]);
        }
        #if (!USE_HALL_VALUE_DURING_VIBRATION)
        previousKeySta[k] = gbKeyState[k];
        #if MYDEBUG_ENABLE
        previousTemp[k] = temp[k];
        #endif
        #endif
        
        #if PUSH_VIBRATING_HALL_VALUE_TO_GUI
        if(1 == k) //left
        {
          if(0xFF == gbLeftVibratingFlag)
          {
            gbLeftVibratingFlag = 0;
            
            HallRawInfo_ts xRawInfo;
            HallSegmentInfo_ts xSegInfo;
            xRawInfo.wBaselineBottom = gwBottomBaseline[k];
            xRawInfo.wBaselineTop = gwTopBaseline[k];
            xRawInfo.wCurrentRaw = temp[k];
            xSegInfo.wCurrentSegmentValue = node[k];
            xSegInfo.wMaxSegmentValue = 100;
            Hall_SyncParam(0, &xRawInfo, &xSegInfo); // push to RightBox of GUI,
          }
          else
          {
            HallRawInfo_ts xRawInfo;
            HallSegmentInfo_ts xSegInfo;
            xRawInfo.wBaselineBottom = gwBottomBaseline[k];
            xRawInfo.wBaselineTop = gwTopBaseline[k];
            xRawInfo.wCurrentRaw = temp[k];
            xSegInfo.wCurrentSegmentValue = node[k];
            xSegInfo.wMaxSegmentValue = 100;
            Hall_SyncParam(k, &xRawInfo, &xSegInfo); // push to LeftBox of GUI,
          }
        }
        else /* if (0 == k) */ // right
        {
          ;
        }
        #else
        HallRawInfo_ts xRawInfo;
        HallSegmentInfo_ts xSegInfo;
        xRawInfo.wBaselineBottom = gwBottomBaseline[k];
        xRawInfo.wBaselineTop = gwTopBaseline[k];
        xRawInfo.wCurrentRaw = temp[k];
        xSegInfo.wCurrentSegmentValue = node[k];
        xSegInfo.wMaxSegmentValue = 100;
        Hall_SyncParam(k, &xRawInfo, &xSegInfo);
        #endif

      }
    }

    /********debug code begin*********************/
    #if MYDEBUG_ENABLE
      static uint16_t minVal[2] = {0xFFFF, 0xFFFF}, maxVal[2] = {0, 0};
      static uint32_t tickStart = 0;
      
      if(temp[0] != CRC_ERROR) // R
      {
        if(temp[0] > maxVal[0]) {maxVal[0] = temp[0];}
        if(temp[0] < minVal[0]) {minVal[0] = temp[0];}
      }
      if(temp[1] != CRC_ERROR)// L
      {
        if(temp[1] > maxVal[1]) {maxVal[1] = temp[1];}
        if(temp[1] < minVal[1]) {minVal[1] = temp[1];}
      }
      if(MyTick_GetTickMs() - tickStart > 300)
      {
        tickStart = MyTick_GetTickMs();
        InfoLog("POS: sta--seg--now--min--max--dif--top--bottom");
        InfoLog("L: %u--%u--%u--%u--%u--%u--%u--%u", gbKeyState[1], node[1], \
                temp[1], minVal[1], maxVal[1], maxVal[1] - minVal[1], gwTopBaseline[1], gwBottomBaseline[1]);
        InfoLog("R: %u--%u--%u--%u--%u--%u--%u--%u", gbKeyState[0], node[0], \
                temp[0], minVal[0], maxVal[0], maxVal[0] - minVal[0], gwTopBaseline[0], gwBottomBaseline[0]);
      }
      
      #if(USE_HALL_VALUE_DURING_VIBRATION)
      if(Ele_IsVibrating(ELE_POS_LEFT))
      {
        //InfoLog("POS: sta--seg--now--min--max--dif--top--bottom");
        InfoLog("[%u]L: %u--%u--%u--%u--%u--%u--%u--%u", MyTick_GetTickMs(), gbKeyState[1], node[1], \
                temp[1], minVal[1], maxVal[1], maxVal[1] - minVal[1], gwTopBaseline[1], gwBottomBaseline[1]);
      }
      if(Ele_IsVibrating(ELE_POS_RIGHT))
      {
        //InfoLog("POS: sta--seg--now--min--max--dif--top--bottom");
        InfoLog("[%u]R: %u--%u--%u--%u--%u--%u--%u--%u", MyTick_GetTickMs(), gbKeyState[0], node[0], \
                temp[0], minVal[0], maxVal[0], maxVal[0] - minVal[0], gwTopBaseline[0], gwBottomBaseline[0]);
      }
      #endif
    #endif
    /********debug code end*********************/
  }
  
  #if (!USE_HALL_VALUE_DURING_VIBRATION)
  if(gbKeyState[0])
  {
    ret |= (0x01 << 1);
  }
  
  if(gbKeyState[1])
  {
    ret |= (0x01 << 0);
  }
  #endif
  return ret;
}


