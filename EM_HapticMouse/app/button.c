#include "button.h"
#include "GPIO.h"
#include "SPI.h"
#include "pc_communicate.h"
#include "my_electromagnet.h"
#include "Timer.h"
#include "my_misc.h"
#include "my_hallkey_func.h"
#include "my_board.h"
#include "my_adc.h"

UINT8 hall_spi_tx[4] = {0};
UINT8 hall_spi_rx[4] = {0};

UINT16 top_baseline[2] = {8000, 8000};
uint16_t bottom_baseline[2] = {6800, 6800};
UINT16 cali_cnt = 600;

BUTTON_SCAN_DATA buttonScan_stru;

#define L_HALL_INVERSR	1
#define R_HALL_INVERSR	1
#define SB  6
const unsigned char bit_crc_write_reg[256]=
{

	//0	  			1				2				3				4				5				6				7
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//8				9				10				11				12				13				14				15
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//16			17				18				19				20				21				22				23
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//24			25				26				27				28				29				30				31
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//32	`		33				34				35				36				37				38				39
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//40			41				42				43				44				45				46				47
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//48			49				50				51				52				53				54				55
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//56			57				58				59				60				61				62				63
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//64			65				66				67				68				69				70				71
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//72			73				74				75				76				77				78				79
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//80			81				82				83				84				85				86				87
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//88			89				90				91				92				93				94				95
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//96			97				98				99				100				101				102				103
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//104			105				106				107				108				109				110				111
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//112			113				114				115				116				117				118				119
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//120			121				122				123				124				125				126				127
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//128			129				130				131				132				133				134				135
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//136			137				138				139				140				141				142				143
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//144			145				146				147				148				149				150				151
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//152			153				154				155				156				157				58				159
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//160			161				162				163				164				165				166				167
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//168			169				170				171				172				173				174				175
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//176			177				178				179				180				181				182				183
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//184			185				186				187				188				189				190				191
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//192			193				194				195				196				197				198				199
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//200			201				202				203				204				205				206				207
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//208			209				210				211				212				213				214				215
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//216			217				218				219				220				221				222				223
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//224			225				226				227				228				229				230				231
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

	//232			233				234				235				236				237				238				239
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//240			241				242				243				244				245				246				247
	0,				1<<SB,			1<<SB,			0,				1<<SB,			0,				0,				1<<SB,

	//248			249				250				251				252				253				254				255
	1<<SB,			0,				0,				1<<SB,			0,				1<<SB,			1<<SB,			0,

};

const unsigned char bit_error[256]=
{

		//0	1		2			3			4		5		6			7	
		0,	64,	128,	192,	64,	0,	192,	128,	

		//8		9			10	11	12		13		14	15	
		128,	192,	0,	64,	192,	128,	64,	0,	

		//16	17	18	19	20	21	22	23	
		64,	0,	192,	128,	0,	64,	128,	192,	

		//24	25	26	27	28	29	30	31	
		192,	128,	64,	0,	128,	192,	0,	64,	

		//32	33	34	35	36	37	38	39	
		128,	192,	0,	64,	192,	128,	64,	0,	

		//40	41	42	43	44	45	46	47	
		0,	64,	128,	192,	64,	0,	192,	128,	

		//48	49	50	51	52	53	54	55	
		192,	128,	64,	0,	128,	192,	0,	64,	

		//56	57	58	59	60	61	62	63	
		64,	0,	192,	128,	0,	64,	128,	192,	

		//64	65	66	67	68	69	70	71	
		64,	0,	192,	128,	0,	64,	128,	192,	

		//72	73	74	75	76	77	78	79	
		192,	128,	64,	0,	128,	192,	0,	64,	

		//80	81	82	83	84	85	86	87	
		0,	64,	128,	192,	64,	0,	192,	128,	

		//88	89	90	91	92	93	94	95	
		128,	192,	0,	64,	192,	128,	64,	0,	

		//96	97	98	99	100	101	102	103	
		192,	128,	64,	0,	128,	192,	0,	64,	

		//104	105	106	107	108	109	110	111	
		64,	0,	192,	128,	0,	64,	128,	192,	

		//112	113	114	115	116	117	118	119	
		128,	192,	0,	64,	192,	128,	64,	0,	

		//120	121	122	123	124	125	126	127	
		0,	64,	128,	192,	64,	0,	192,	128,	

		//128	129	130	131	132	133	134	135	
		128,	192,	0,	64,	192,	128,	64,	0,	

		//136	137	138	139	140	141	142	143	
		0,	64,	128,	192,	64,	0,	192,	128,	

		//144	145	146	147	148	149	150	151	
		192,	128,	64,	0,	128,	192,	0,	64,	

		//152	153	154	155	156	157	158	159	
		64,	0,	192,	128,	0,	64,	128,	192,	

		//160	161	162	163	164	165	166	167	
		0,	64,	128,	192,	64,	0,	192,	128,	

		//168	169	170	171	172	173	174	175	
		128,	192,	0,	64,	192,	128,	64,	0,	

		//176	177	178	179	180	181	182	183	
		64,	0,	192,	128,	0,	64,	128,	192,	

		//184	185	186	187	188	189	190	191	
		192,	128,	64,	0,	128,	192,	0,	64,	

		//192	193	194	195	196	197	198	199	
		192,	128,	64,	0,	128,	192,	0,	64,	

		//200	201	202	203	204	205	206	207	
		64,	0,	192,	128,	0,	64,	128,	192,	

		//208	209	210	211	212	213	214	215	
		128,	192,	0,	64,	192,	128,	64,	0,	

		//216	217	218	219	220	221	222	223	
		0,	64,	128,	192,	64,	0,	192,	128,	

		//224	225	226	227	228	229	230	231	
		64,	0,	192,	128,	0,	64,	128,	192,	

		//232	233	234	235	236	237	238	239	
		192,	128,	64,	0,	128,	192,	0,	64,	

		//240	241	242	243	244	245	246	247	
		0,	64,	128,	192,	64,	0,	192,	128,	

		//248	249	250	251	252	253	254	255	
		128,	192,	0,	64,	192,	128,	64,	0,	

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
#if (defined(HAPTIC_V4))
#if (defined(HAPTIC_V4_PWM))
  {0x14, 0x35},		//LNA_Config0, 64x gain
  {0x15, 0x44},		//fix
  {0x16, 0x04},		//PGA_Config0, 18x gain
#else
  //{0x14, 0x1D},		//LNA_Config0, 16x gain
  //{0x15, 0x44},		//fix
  //{0x16, 0x00},		//PGA_Config0, 13.2x gain
  {0x14, 0x35},		//LNA_Config0, 64x gain
  {0x15, 0x44},		//fix
  {0x16, 0x04},		//PGA_Config0, 18x gain
#endif
#elif (defined(P723_POC))
  {0x14, 0x1D},		//LNA_Config0, 16x gain
  {0x15, 0x44},		//fix
  {0x16, 0x00},		//PGA_Config0, 13.2x gain
#elif (defined(V4_HALL_HPTIC_V4))
  #if(defined(V4_HALL_HPTIC_V4_G2_0923))
  {0x14, 0x35},		//LNA_Config0, 64x gain
  {0x15, 0x44},		//fix
  {0x16, 0x04},		//PGA_Config0, 18x gain
  #else
  {0x14, 0x1D},		//LNA_Config0, 16x gain
  {0x15, 0x44},		//fix
  {0x16, 0x00},		//PGA_Config0, 13.2x gain
  #endif
#elif (defined(GT_LINA_T1A_H5))
  {0x14, 0x35},		//LNA_Config0, 64x gain
  {0x15, 0x44},		//fix
  {0x16, 0x04},		//PGA_Config0, 18x gain
#else
  {0x14, 0x35},		//LNA_Config0, 64x gain
  {0x15, 0x44},		//fix
  {0x16, 0x04},		//PGA_Config0, 18x gain
#endif
	{0x17, 0x7C},		//fix
	{0x18, 0x8E},		//ModeConfig0, led enable, continue conversion mode, smart power-saving mode disabled
	//led恒流源档位设置
	//{0x18, 0x0A}, 	//disable led
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

UINT32 Button_Location_Data(UINT32 port,UINT32 mask,UINT32 position,UINT32 reportPosition)
{
	UINT32 reportMask;
	UINT32 offSet;

	reportMask = port & mask;

	if (reportPosition > position)
	{
		offSet = reportPosition - position;
		reportMask <<= offSet;

	}else
	{
		offSet = position - reportPosition;
		reportMask >>= offSet;
	}

	return reportMask;
}

UINT32 LR_GetState(void)
{
#if 0
	static uint8_t nowStateL = 0;
	static uint8_t nowStateR = 0;
	uint16_t threshold = 0;
  	uint16_t sample[2];
#endif
	UINT32 result = 0; //init value is 0.

  #if PUSH_VIBRATING_HALL_VALUE_TO_GUI
  if(Ele_IsTureVibrating(ELE_POS_LEFT))
  {
    gbLeftVibratingFlag = 0x0F;
    CT16B1_DelayUs(130);
  }
  #endif
  
  #if (defined(IRPT_V4))
  int32_t ret = -1;
  uint16_t leftVal, rightVal;
  
  ret = MyADC_GetValue(pLeftAdcHandle, &leftVal);
  ret |= MyADC_GetValue(pRightAdcHandle, &rightVal);
  if(ret)
  {
    return result;
  }
  
  hall_spi_rx[0] = (uint8_t)(rightVal >> 8);
  hall_spi_rx[1] = (uint8_t)(rightVal);
  hall_spi_rx[2] = (uint8_t)(leftVal >> 8);
  hall_spi_rx[3] = (uint8_t)(leftVal);

  #else
	hall_get_sample();
  #endif
  
  #if PUSH_VIBRATING_HALL_VALUE_TO_GUI
  if(Ele_IsTureVibrating(ELE_POS_LEFT))
  {
    if(0x0F == gbLeftVibratingFlag)
    {
      gbLeftVibratingFlag = 0xFF;
    }
  }
  else
  {
    gbLeftVibratingFlag = 0;
  }
  #endif
  
  result = ProcessRecvSampleRawData((uint8_t *)(&hall_spi_rx), 4);
	return result;
#if 0
	if(cali_cnt)
	{
		cali_cnt--;
		if((hall_spi_rx[0] & 0xC0) == (bit_error[hall_spi_rx[0] & 0x3f] ^ bit_error[hall_spi_rx[1]]))
		{
			sample[0] = (hall_spi_rx[0] & 0x3f) << 8 | hall_spi_rx[1];
		}else
		{
			sample[0] = CRC_ERROR;
		}
		
		if(sample[0] != CRC_ERROR)
		{
			if(sample[0] > (top_baseline[0] + 30))
			{
				top_baseline[0] += 5;
			}
			else if((sample[0] + 30) < top_baseline[0])
			{
				top_baseline[0] -= 5;
			}
		}
		
		if((hall_spi_rx[2] & 0xC0) == (bit_error[hall_spi_rx[2] & 0x3f] ^ bit_error[hall_spi_rx[3]]))
		{
			sample[1] = (hall_spi_rx[2] & 0x3f) << 8 | hall_spi_rx[3];
		}else
		{
			sample[1] = CRC_ERROR;
		}
		
		if(sample[1] != CRC_ERROR)
		{
			if(sample[1] > (top_baseline[1] + 30))
			{
				top_baseline[1] += 5;
			}
			else if((sample[1] + 30) < top_baseline[1])
			{
				top_baseline[1] -= 5;
			}
		}
		if(!cali_cnt)
		{
			USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, (UINT8*)top_baseline, 4);
      /********debug code begin*********************/
      #if MYDEBUG_ENABLE
      InfoLog("l_top_b: %u; r_top_b: %u", top_baseline[1], top_baseline[0]);
      uint8_t leftData = 0, rightData = 0;
      Hall_ReadReg(0x14, &leftData, &rightData);
      InfoLog("Reg0x14: (L)0x%X, (R)0x%X", leftData, rightData);
      Hall_ReadReg(0x16, &leftData, &rightData);
      InfoLog("Reg0x16: (L)0x%X, (R)0x%X", leftData, rightData);
      #endif
      /********debug code end***********************/
			
		}
		
	}
	else
	{//right button
		if((hall_spi_rx[0] & 0xC0) == (bit_error[hall_spi_rx[0] & 0x3f] ^ bit_error[hall_spi_rx[1]]))
		{
			sample[0] = (hall_spi_rx[0] & 0x3f) << 8 | hall_spi_rx[1];
		}else
		{
			sample[0] = CRC_ERROR;
		}
		
		if(sample[0] != CRC_ERROR)
		{
      Hall_CalibrateTopBaseline(0, sample[0], &(top_baseline[0]));
			if(nowStateR)//press state
			{
				threshold = 300;
			}
			else//release state
			{
				threshold = 400;
			}
			#if R_HALL_INVERSR
			//if(sample[0] > (top_baseline[0] + 200))
			if(sample[0] > (top_baseline[0] + threshold))
			#else
			//if(sample[0] < (top_baseline[0] - 200))
			if(sample[0] < (top_baseline[0] - threshold))
			#endif
			{
        if(!Ele_IsReleaseVibrating(ELE_POS_RIGHT))
        {
          result |= 0x02;
          nowStateR = 1;
        }
			}
			else
			{
				result &= ~0x02;
				nowStateR = 0;
			}
		}
		//left button
		if((hall_spi_rx[2] & 0xC0) == (bit_error[hall_spi_rx[2] & 0x3f] ^ bit_error[hall_spi_rx[3]]))
		{
			sample[1] = (hall_spi_rx[2] & 0x3f) << 8 | hall_spi_rx[3];
		}else
		{
			sample[1] = CRC_ERROR;
		}
		
		if(sample[1] != CRC_ERROR)
		{
      Hall_CalibrateTopBaseline(1, sample[1],  &(top_baseline[1]));
			if(nowStateL)//press state
			{
				threshold = 300;
			}
			else//release state
			{
				threshold = 400;
			}
			#if L_HALL_INVERSR
			//if(sample[1] > (top_baseline[1] + 200))
			if(sample[1] > (top_baseline[1] + threshold))
			#else
			//if(sample[1] < (top_baseline[1] - 200))
			if(sample[1] < (top_baseline[1] - threshold))
			#endif
			{
        if(!Ele_IsReleaseVibrating(ELE_POS_LEFT))
        {
          result |= 0x01;
          nowStateL = 1;
        }
			}
			else
			{
				result &= ~0x01;
				nowStateL = 0;
			}
		}
		/********debug code begin*********************/
    #if MYDEBUG_ENABLE
		static uint16_t minVal[2] = {0xFFFF, 0xFFFF}, maxVal[2] = {0, 0};
		static uint32_t tickStart = 0;
    static uint32_t tickStart2 = 0;
    
		if(sample[0] != CRC_ERROR)
		{
			if(sample[0] > maxVal[0]) {maxVal[0] = sample[0];}
			if(sample[0] < minVal[0]) {minVal[0] = sample[0];}
		}
		if(sample[1] != CRC_ERROR)
		{
			if(sample[1] > maxVal[1]) {maxVal[1] = sample[1];}
			if(sample[1] < minVal[1]) {minVal[1] = sample[1];}
		}
		if(MyTick_GetTickMs() - tickStart > 300)
		{
			tickStart = MyTick_GetTickMs();
			InfoLog("POS: now--min--max--dif--top");
			InfoLog("L: %u--%u--%u--%u--%u", sample[1], minVal[1], maxVal[1], maxVal[1] - minVal[1], top_baseline[1]);
			InfoLog("R: %u--%u--%u--%u--%u", sample[0], minVal[0], maxVal[0], maxVal[0] - minVal[0], top_baseline[0]);
		}
    if(MyTick_GetTickMs() - tickStart2 > 30001)
    {
      tickStart2 = MyTick_GetTickMs();
      InfoLog("mySfVer: 0x%x-%u", MY_MAIN_VERSION, DEBUG_VER);
      InfoLog("l_top_b: %u; r_top_b: %u", top_baseline[1], top_baseline[0]);
      uint8_t leftData = 0, rightData = 0;
      Hall_ReadReg(0x14, &leftData, &rightData);
      InfoLog("Reg0x14: (L)0x%X, (R)0x%X", leftData, rightData);
      Hall_ReadReg(0x16, &leftData, &rightData);
      InfoLog("Reg0x16: (L)0x%X, (R)0x%X", leftData, rightData);
    }
    #endif
		/********debug code end*********************/
		
		if(test_mode)
		{
			receive_buff[0] = 0x04;
			receive_buff[1] = 0x20;
			receive_buff[4] = sample[1];
			receive_buff[5] = sample[1] >> 8;
			receive_buff[6] = top_baseline[1];
			receive_buff[7] = top_baseline[1] >> 8;
			receive_buff[8] = sample[0];
			receive_buff[9] = sample[0] >> 8;
			receive_buff[10] = top_baseline[0];
			receive_buff[11] = top_baseline[0] >> 8;
			USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, 64);
		}

	}

	return result;
#endif
}

UINT32 Button_GetState(void)
{
	UINT32 button_port_state = 0;
	UINT32 button_port0_state=0;
  
  #if (defined(HAPTIC_V2) || defined (HAPTIC_V3) || defined (HAPTIC_V4) || defined (IRPT_V4) || defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))
  button_port0_state = BUTTON_PORT0;
  button_port_state |= Button_Location_Data(button_port0_state,MIDDLE_BUTTON_MASK,MIDDLE_BUTTON_POSITION,MIDDLE_BUTTON_REPORT_BIT);
  button_port_state |= Button_Location_Data(button_port0_state,DPI_CYCLE_BUTTON_MASK,DPI_CYCLE_BUTTON_POSITION,DPI_CYCLE_BUTTON_REPORT_BIT);
  button_port_state |= Button_Location_Data(button_port0_state,K4_BUTTON_MASK,K4_BUTTON_POSITION,K4_BUTTON_REPORT_BIT);
  button_port_state |= Button_Location_Data(button_port0_state,K5_BUTTON_MASK,K5_BUTTON_POSITION,K5_BUTTON_REPORT_BIT);
  
  #elif (defined(P723_POC))
  UINT32 button_port1_state=0;
  button_port0_state = BUTTON_PORT0;
  button_port1_state = BUTTON_PORT1;
  button_port_state |= Button_Location_Data(button_port0_state,MIDDLE_BUTTON_MASK,MIDDLE_BUTTON_POSITION,MIDDLE_BUTTON_REPORT_BIT);
  button_port_state |= Button_Location_Data(button_port0_state,DPI_CYCLE_BUTTON_MASK,DPI_CYCLE_BUTTON_POSITION,DPI_CYCLE_BUTTON_REPORT_BIT);
  button_port_state |= Button_Location_Data(button_port1_state,K4_BUTTON_MASK,K4_BUTTON_POSITION,K4_BUTTON_REPORT_BIT);
  button_port_state |= Button_Location_Data(button_port0_state,K5_BUTTON_MASK,K5_BUTTON_POSITION,K5_BUTTON_REPORT_BIT);
  
  #elif (defined(GEN_2_H_V1))
  UINT32 button_port1_state=0;
  button_port0_state = BUTTON_PORT0;
  button_port1_state = BUTTON_PORT1;
  button_port_state |= Button_Location_Data(button_port0_state,MIDDLE_BUTTON_MASK,MIDDLE_BUTTON_POSITION,MIDDLE_BUTTON_REPORT_BIT);
  button_port_state |= Button_Location_Data(button_port0_state,DPI_CYCLE_BUTTON_MASK,DPI_CYCLE_BUTTON_POSITION,DPI_CYCLE_BUTTON_REPORT_BIT);
  button_port_state |= Button_Location_Data(button_port0_state,K4_BUTTON_MASK,K4_BUTTON_POSITION,K4_BUTTON_REPORT_BIT);
  button_port_state |= Button_Location_Data(button_port1_state,K5_BUTTON_MASK,K5_BUTTON_POSITION,K5_BUTTON_REPORT_BIT);
  
  #else
    #error "Hardware information is undefined."
  #endif
  
	button_port_state |= LR_GetState();

	
	return button_port_state;
}


BOOL Button_Scan(void)
{
	UINT8 i;
	UINT32 button;
   
	buttonScan_stru.button_changed = FALSE;

	button = Button_GetState();

  for(i = 0; i <= 1; i++) // 0: left; 1: right
  {
		if((button ^ buttonScan_stru.scan_result_cur) & (0x01 << i))
		{
			if(button & (0x01 << i))
			{
				buttonScan_stru.scan_result_cur |= (0x01 << i);
				//buttonScan_stru.debounce[i] = 5;
			}
			else
			{
				buttonScan_stru.scan_result_cur &= ~(0x01 << i);
				//buttonScan_stru.debounce[i] = 8;
			}
		}
    
    if((buttonScan_stru.scan_result_pre ^ buttonScan_stru.scan_result_cur) & (0x01 << i))
    {
      if(buttonScan_stru.scan_result_cur & (0x01 << i))
      {
        buttonScan_stru.scan_result_pre |= (0x01 << i);
        buttonScan_stru.button_changed = TRUE;
        //****************code press begin****************/
        if(0 == i)// left button
        {
          Ele_LeftKeyPressProcess();
        }
        else //if(1 == i)//right button
        {
          Ele_RightKeyPressProcess();
          
        }
        //****************code press end****************/
      }
      else
      {
        buttonScan_stru.scan_result_pre &= ~(0x01 << i);
        buttonScan_stru.button_changed = TRUE;
        //****************code release begin****************/
        if(0 == i)// left button
        {
          Ele_LeftKeyReleaseProcess();
        }
        else //if(1 == i)//right button
        {
          Ele_RightKeyReleaseProcess();
        }
        //****************code release end****************/
      }	
    }
  }
  
	for(i = 2; i <= 2; i++) //middle
	{
		if((button ^ buttonScan_stru.scan_result_cur) & (0x01 << i))
		{
			if(button & (0x01 << i))
			{
				buttonScan_stru.scan_result_cur |= (0x01 << i);
				buttonScan_stru.debounce[i] = 5;
			}
			else
			{
				buttonScan_stru.scan_result_cur &= ~(0x01 << i);
				buttonScan_stru.debounce[i] = 8;
			}
		}
		else
		{
			if(buttonScan_stru.debounce[i])
			{
				buttonScan_stru.debounce[i]--;
				if(buttonScan_stru.debounce[i] == 0)
				{
					if((buttonScan_stru.scan_result_pre ^ buttonScan_stru.scan_result_cur) & (0x01 << i))
					{
						if(buttonScan_stru.scan_result_cur & (0x01 << i))
						{
							buttonScan_stru.scan_result_pre |= (0x01 << i);
							buttonScan_stru.button_changed = TRUE;
						}
						else
						{
							buttonScan_stru.scan_result_pre &= ~(0x01 << i);
							buttonScan_stru.button_changed = TRUE;
						}	
					}
				}
			}
		}
	}
#if (defined(DISABLE_K4_K5_KEY))
#else
	for(i = 5; i <= 6; i++)
	{
		if((button ^ buttonScan_stru.scan_result_cur) & (0x01 << i))
		{
			if(button & (0x01 << i))
			{
				buttonScan_stru.scan_result_cur |= (0x01 << i);
				buttonScan_stru.debounce[i - 2] = 5;
			}
			else
			{
				buttonScan_stru.scan_result_cur &= ~(0x01 << i);
				buttonScan_stru.debounce[i - 2] = 8;
			}
		}
		else
		{
			if(buttonScan_stru.debounce[i - 2])
			{
				buttonScan_stru.debounce[i - 2]--;
				if(buttonScan_stru.debounce[i - 2] == 0)
				{
					if((buttonScan_stru.scan_result_pre ^ buttonScan_stru.scan_result_cur) & (0x01 << i))
					{
						if(buttonScan_stru.scan_result_cur & (0x01 << i))
						{
							buttonScan_stru.scan_result_pre |= (0x01 << i);
							buttonScan_stru.button_changed = TRUE;
							//****************code press begin****************/
							if(5 == i)// K4 button, front
							{
								Ele_K4KeyPressProcess();
							}
							else if(6 == i)//k5 button back
							{
								Ele_K5KeyPressProcess();
							}
							//****************code press end****************/
						}
						else
						{
							buttonScan_stru.scan_result_pre &= ~(0x01 << i);
							buttonScan_stru.button_changed = TRUE;
						}			
					}
				}
			}
		}
	}
#endif
	for(i = 9; i <= 9; i++)
	{
		if((button ^ buttonScan_stru.scan_result_cur) & (0x01 << i))
		{
			if(button & (0x01 << i))
			{
				buttonScan_stru.scan_result_cur |= (0x01 << i);
				buttonScan_stru.debounce[i - 4] = 5;
			}
			else
			{
				buttonScan_stru.scan_result_cur &= ~(0x01 << i);
				buttonScan_stru.debounce[i - 4] = 8;
			}
		}
		else
		{
			if(buttonScan_stru.debounce[i - 4])
			{
				buttonScan_stru.debounce[i - 4]--;
				if(buttonScan_stru.debounce[i - 4] == 0)
				{
					if((buttonScan_stru.scan_result_pre ^ buttonScan_stru.scan_result_cur) & (0x01 << i))
					{
						if(buttonScan_stru.scan_result_cur & (0x01 << i))
						{
							buttonScan_stru.scan_result_pre |= (0x01 << i);
							buttonScan_stru.button_changed = TRUE;
						}
						else
						{
							buttonScan_stru.scan_result_pre &= ~(0x01 << i);
							buttonScan_stru.button_changed = TRUE;
						}
					}
				}
			}
		}
	}

	return buttonScan_stru.button_changed;
}


void Hall_CLK_Command_SET(unsigned short high_delay, unsigned short low_delay)
{
#if (defined(GT_LINA_T1A_H5))
  	uint32_t reg;
	
	NVIC_DisableIRQ(USB_IRQn);
//	NVIC_DisableIRQ(TIMER_16_0_IRQn);
	reg = LPC_IOCON->PIO1_15;//	&= ~0x07 ;							//Select GPIO0_10 To CLK0

	reg &= ~0x07;
	reg |= 0x00;
	LPC_IOCON->PIO1_15 = reg;	
	DrvGPIOOutput(1,15,1) ;

	//nsAPI_SSP_Delay1us(high_delay);
	CT16B1_DelayUs(high_delay);

	DrvGPIOOutput(1,15,0) ;

	//nsAPI_SSP_Delay1us(low_delay);
	CT16B1_DelayUs(low_delay);
  reg = LPC_IOCON->PIO1_15;//	&= ~0x07 ;							//Select GPIO0_10 To CLK0
	reg &= ~0x07;
	reg |= 0x03;
	LPC_IOCON->PIO1_15 = reg;
	
	NVIC_EnableIRQ(USB_IRQn);
//	NVIC_DisableIRQ(TIMER_16_0_IRQn);
#else
	uint32_t reg;
	
	NVIC_DisableIRQ(USB_IRQn);
//	NVIC_DisableIRQ(TIMER_16_0_IRQn);
	reg = LPC_IOCON->SWCLK_PIO0_10;//	&= ~0x07 ;							//Select GPIO0_10 To CLK0

	reg &= ~0x07;
	reg |= 0x01;
	LPC_IOCON->SWCLK_PIO0_10 = reg;	
	DrvGPIOOutput(0,10,1) ;

	//nsAPI_SSP_Delay1us(high_delay);
	CT16B1_DelayUs(high_delay);

	DrvGPIOOutput(0,10,0) ;
	reg = LPC_IOCON->SWCLK_PIO0_10;//	&= ~0x07 ;							//Select GPIO0_10 To CLK0
	reg &= ~0x07;
	reg |= 0x02;
	LPC_IOCON->SWCLK_PIO0_10 = reg;

	//nsAPI_SSP_Delay1us(low_delay);
	CT16B1_DelayUs(low_delay);
	
	NVIC_EnableIRQ(USB_IRQn);
//	NVIC_DisableIRQ(TIMER_16_0_IRQn);
#endif
}


void hall_get_sample(void)
{
	Hall_CLK_Command_SET(TD1_DELAY_US, TD2_CONTIUNS_DELAY_US);

	SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);
}

void HALL_Read_Instru_cnt(void)
{
	Hall_CLK_Command_SET(TD7_DELAY_US, TD10_DELAY_US);

	SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);
}

void Hall_osc_calibrate(void)
{
	//uint16_t i, j,times=0;
	uint16_t i, times=0;
//    uint8_t verify_start = 0;

	for(i = 0; i < 2; i++)
	{
		hall_spi_tx[i * 2 + 1] = 0x55;
	}
	
    while(times < 30)  //Calibrate times is 20
    {
    	HALL_Read_Instru_cnt();
		
        times++;
		nsAPI_SSP_Delay1us(100);
    }
	
//	for(i = 0; i < 2; i++)
//	{
//		if(hall_spi_rx[i * 2 + 1] > 0x84)
//		{
//			verify_start = 1;
//		}
//		else if(hall_spi_rx[i * 2 + 1] < 0x7C)
//		{
//			verify_start = 1;
//		}
//	}

//	if(verify_start == 0)
//	{
//		return;
//	}
}
void Hall_read_register(void)
{
	SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);

	Hall_CLK_Command_SET(TD4_DELAY_US,TD5_DELAY_US);
	
	SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);
}

void Hall_ReadReg(uint8_t reg, uint8_t *leftValue, uint8_t *rightValue)
{
	hall_spi_tx[0] =  reg & (~(0x01 << 7));
	hall_spi_rx[1] = 0x55;//any value
	hall_spi_tx[2] =  reg & (~(0x01 << 7));
	hall_spi_rx[3] = 0x55;//any value
	
	Hall_read_register();
	
	*rightValue = hall_spi_rx[1];
	*leftValue = hall_spi_rx[3];
}

void Hall_write_register(void)
{
	SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);

	Hall_CLK_Command_SET(TD4_DELAY_US,TD5_DELAY_US);
}

void ana_hall2_phy_set_mode(unsigned char length)
{
	unsigned char i, j;

	for(i = 0; i < REG_CNT; i++)
	{
		for(j= 0; j < length; j++)
		{
			hall_spi_tx[j*2+0] = sensor_reg_init_array[i][0]|0x80|bit_crc_write_reg[sensor_reg_init_array[i][1]];
			hall_spi_tx[j*2+1] = sensor_reg_init_array[i][1];

		}
		Hall_write_register();
	}
}

void HallSensor_INIT(void)
{
	Hall_osc_calibrate();

	ana_hall2_phy_set_mode(2);
}
