/*
 * Copyright (c) 2016 
 * All Rights Reserved.
 *
 */
#include "kb_matrix.h"
#include "mfg_mode.h"
#include "ram_dma.h"
#include "ana_sw_mcu.h"
#include "ana_sw_phy.h"
#include "ana_hall2_phy.h"
#include "ana_sw_hal.h"
#include "ry_delay_drv.h"
#include "kb_manager.h"
#include "led_manager.h"
#include "pwm_hal_drv.h"
#include "ana_sw_app.h"
#include "mcu_slave_hal.h"
#include "usb_hid_config.h"
#include "global_data.h"
#if 1	//ANA_SW_HAL_ENABLE
extern uint8 led_mode;
extern uint8 current_seg_mode;
unsigned char crc_error_count = 0;
unsigned char ana_init_success = 0;
unsigned char ana_sample_first = 0;
unsigned short raw_data_backup[ANA_TOTAL_COUNT];
unsigned short bottom_baseline_adjust_cnt[ANA_TOTAL_COUNT];
unsigned char if_adjust_flag[ANA_TOTAL_COUNT];
unsigned short node[ANA_TOTAL_COUNT];
unsigned short node_backup[ANA_TOTAL_COUNT];
unsigned char hall_inverse_flag[ANA_TOTAL_COUNT];
unsigned short analog_node_backup[ANA_TOTAL_COUNT];

unsigned short raw_data_filter[ANA_TOTAL_COUNT];
unsigned int raw_data_filter_sum[ANA_TOTAL_COUNT];

unsigned char multi_cali_start_point[ANA_TOTAL_COUNT];

unsigned char hall_change_key_count = 0;
unsigned short hall_change_timer_cnt = 0;
unsigned short hall_change_key[ANA_TOTAL_COUNT] = { 0 };
#define length_filter_bit		10
#define FILTER_SKIP_VALUE		60

#define MAGNETIC_POLARITY_N		1
#define TOP_INIT_VALUE			5600
#define BOTTOM_INIT_VALUE		3500
#define AJUST_VALUE    0			//固定为0
#define FILTER_VALUE		24
#define TOP_BASELINE_NOISE		6
#define TOP_BASELINE_STEP_FAST	4
#define BOTTOM_DEADZONE_VALUE	500	//调整底部死区0.1~0.5mm:500/950/1350/1700/2000
#define TOP_DEADZONE_VALUE		35  	//调整顶部死区0.1~0.5mm:15/40/70/105/140
#define	TOP_RESET_VALUE			20  //根据死区对应修改0.1~0.5mm:10/25/50/75/110
#define TOP_DEADZONE_VALUE1		15 
#define	TOP_RESET_VALUE1		10

#define BOTTOM_BASELINE_LIMITATION			600

#define HALL_INVERSE_VALUE		8
#define HALL_INVERSE_VALUE2		20

unsigned char top_deadzone_value = TOP_DEADZONE_VALUE;
unsigned char top_reset_value = TOP_RESET_VALUE;
/****************************死区优化*****************************/
#define BOTTOM_STATUS_HANDLE_LINE 320//底部死区特殊处理分段值线
uint8_t Bottom_Status_Handle_Flag[ANA_TOTAL_COUNT] = {0}; //是否开启死区特殊处理标志位
uint8_t Slow_Raise_Flag[ANA_TOTAL_COUNT] = {0};	//行程机慢升标志位
uint16_t Bottom_Status_Segment_Backup[ANA_TOTAL_COUNT] = {0}; //行程机慢升后存储分段值
uint16_t Shake_Off_Count[ANA_TOTAL_COUNT] = {0};			  //晃动标志位和消抖次数
uint8_t Quick_Press_Flag[ANA_TOTAL_COUNT] = {0};			  //快速按下抬起标志位

//行程机慢升和按住数据存储数组
uint16_t DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[ANA_TOTAL_COUNT][BOTTOM_STATUS_PACKET_LIST_LENGTH];
uint16_t *DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[ANA_TOTAL_COUNT];
uint16_t *DMA_SPI_RX_Buffer_Bottom_Status_Packet_Old_Data_Point[ANA_TOTAL_COUNT];
uint8_t DMA_SPI_RX_Buffer_Bottom_Status_Init_Packet_count[ANA_TOTAL_COUNT];

//状态标志位
uint8_t DMA_SPI_RX_Buffer_Quick_Break_Packet[ANA_TOTAL_COUNT] = {0};

uint8_t DMA_SPI_RX_Buffer_Quick_Press_Packet[ANA_TOTAL_COUNT] = {0};

uint8_t DMA_SPI_RX_Buffer_Shake_Packet[ANA_TOTAL_COUNT] = {0};

uint8_t DMA_SPI_RX_Buffer_Hold_Packet[ANA_TOTAL_COUNT] = {0};

uint8_t DMA_SPI_RX_Buffer_Slow_Raise_Packet[ANA_TOTAL_COUNT] = {0};

uint8_t DMA_SPI_RX_Buffer_Press_Again_Packet[ANA_TOTAL_COUNT] = {0};

//记录上一包rawdata数据
uint16_t Original_Rawdata_Status_backup[ANA_TOTAL_COUNT] = {0};

uint16_t Filter_Rawdata_Status_backup[ANA_TOTAL_COUNT] = {0};

//每隔一定包数记录数据进数组里面
uint16_t DMA_SPI_RX_Buffer_Bottom_Status_Packet[ANA_TOTAL_COUNT] = {0};

uint8_t Key_Had_Make[ANA_TOTAL_COUNT] = {0};

//状态成立判断次数
uint8_t Count_Packet_A = 2;

uint8_t Count_Packet_B = 2;

uint8_t Count_Packet_C = 2;

uint8_t Count_Packet_D = 5;
//状态成立判断阈值
uint16_t RawDate_Quick_Press_Break_Range_Packet = 10;

uint16_t RawDate_Quick_Press_Break_Range = 35;

uint16_t RawDate_Shake_Range = 40;

uint16_t RawDate_Slow_Raise_Range = 5;
uint16_t RawDate_Slow_Raise_Range_Max = 10;

uint16_t RawDate_Has_Change_C = 10;
//判断数组长度和间隔包数
#define	BOTTOM_STATUS_PACKET_LIST_LENGTH	5
#define DMA_SPI_RX_Buffer_Bottom_Status_Packet_count 5

uint8_t Hold_Raise_Disable[ANA_TOTAL_COUNT] = {0};	  //判断是否可以进行行程机状态判断 1:disable
uint8_t Bottom_Status_Need_Reset_Flag[ANA_TOTAL_COUNT] = {0}; //判断是否需要清除死区处理Flag
uint16_t Bottom_Press_Status_Segment_Backup[ANA_TOTAL_COUNT] = {0};//用作按下时候的分段值记录
/****************************死区优化*****************************/
#if TRIGGLE_LEVEL_3
unsigned short trigger_level[3] = {
	3500,4500,5500,
};
unsigned char trigger_level_id=1;
#endif

void Hall_Power_Reset(void)
{
	Hall_VDD_Discharge();	//配置HALLPower_PIN & Discharge_PIN IO口，使hall VDD放电
	
	delay_ms(20);			//等待20ms
	
	Hall_VDD_PowerOn();		//配置HALLPower_PIN & Discharge_PIN IO口，使hall VDD上电
}

unsigned char ana_hal_hall2_reset(void)
{
	unsigned char result = 0;
	
	Hall_Power_Reset();		//reset hall VDD
	
	result = ana_phy_osc_calibrate(ANA_TOTAL_COUNT);			//频率校准

	ana_hall2_phy_set_mode(ANA_TOTAL_COUNT);
	
//	ana_hall5_phy_set_sps_mode(ANA_TOTAL_COUNT);
	
	ana_hall2_hal_get_gain(p_cur_gain_buff);		//从flash中读出校准后的增益放入buff，如果flash没有写入过增益值，则buff中填入初始默认值7
	ana_hall5_phy_set_gain(ANA_TOTAL_COUNT,p_cur_gain_buff);
	
	return result;
}

void ana_hal_init(void)
{
	mcu_slave_cmd.cmd.cali_init_cnt = ana_sample_cali_init_cnt();

	ana_init_success = 0;
	
	ana_init_success = ana_hal_hall2_reset();
/******************死区优化*******************************/
	Bottom_Status_Packet_List_Init();
/******************死区优化*******************************/
}

void ana_hal_esd_reset(void)
{
	ana_init_success = ana_hal_hall2_reset();
}

unsigned char ana_hal_check_reset(void)
{
	if(ana_phy_data_chain_check_esd(ANA_TOTAL_COUNT)<ANA_TOTAL_COUNT)
	{
		return 0;//failed
	}else
	{
		return 1;
	}
}

unsigned char ana_hal_esd_protect(void)
{
	unsigned char timeout=20;
	
	if(crc_error_count > 5)	// || ana_hal_check_reset()==0))			//出现5次以上异常数据或者自检失败，对sensor进行复位
	{
		crc_error_count=0;
		do
		{
			ana_hal_esd_reset();
			
			if(--timeout==0) break;
		}
		while (ana_init_success==0);				
		return 0;
	}
	return 1;
}

volatile  unsigned short ana_sample_error = 0;

unsigned char ana_trigger_point[ANA_TOTAL_COUNT]={0};



const unsigned short node_cali_table[2560] = 														//segment
{
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,	29,
	30,															//31
	31,	31,	31,
	32,	32,
	33,	33,
	34,
	35,	35,
	36,
	37,
	38,
	39,
	40,
	41,	41,
	42,	42,
	43,
	44,	44,
	45,
	46,	46,
	47,	47,
	48,	48,
	49,	49,
	50,	50,														//63
	51,	51,
	52,	52,
	53,
	54,
	55,	55,
	56,
	57,	57,
	58,	58,	58,
	59,	59,	59,
	60,
	61,	61,
	62,	62,	62,
	63,	63,
	64,	64,
	65,
	66,
	67,	67,
	68,	68,	68,
	69,
	70,	70,
	71,	71,
	72,	72,	72,
	73,
	74,	74,
	75,	75,
	76,	76,
	77,	77,
	78,
	79,	79,	79,
	80,	80,
	81,	81,	81,
	82,	82,
	83,	83,
	84,
	85,	85,
	86,	86,	86,	86,
	87,	87,
	88,	88,
	89,	89,
	90,	90,
	91,	91,	91,
	92,
	93,
	94,	94,
	95,	95,
	96,	96,
	97,	97,
	98,	98,
	99,	99,	99,
	100,	100,
	101,	101,	101,
	102,	102,	102,	102,
	103,	103,
	104,	104,
	105,	105,	105,
	106,	106,
	107,	107,	107,
	108,	108,
	109,	109,	109,
	110,	110,
	111,	111,	111,
	112,	112,
	113,	113,
	114,	114,	114,	114,
	115,	115,
	116,	116,	116,
	117,	117,	117,
	118,	118,
	119,	119,	119,
	120,	120,	120,
	121,	121,	121,
	122,	122,	122,
	123,	123,
	124,	124,
	125,	125,	125,
	126,	126,	126,	126,
	127,	127,	127,
	128,	128,	128,
	129,	129,	129,
	130,	130,	130,	130,
	131,	131,	131,
	132,	132,	132,
	133,	133,	133,
	134,	134,	134,
	135,	135,	135,
	136,	136,	136,	136,
	137,	137,	137,
	138,	138,	138,
	139,	139,	139,
	140,	140,	140,	140,	140,
	141,	141,	141,
	142,	142,	142,
	143,	143,	143,
	144,	144,	144,
	145,	145,	145,	145,
	146,	146,	146,	146,
	147,	147,	147,	147,
	148,	148,	148,	148,
	149,	149,	149,	149,
	150,	150,	150,	150,
	151,	151,	151,	151,
	152,	152,	152,	152,
	153,	153,	153,	153,
	154,	154,	154,	154,
	155,	155,	155,	155,	155,	155,
	156,	156,	156,	156,
	157,	157,	157,	157,
	158,	158,	158,	158,
	159,	159,	159,	159,
	160,	160,	160,	160,
	161,	161,	161,	161,
	162,	162,	162,	162,
	163,	163,	163,	163,
	164,	164,	164,	164,
	165,	165,	165,	165,
	166,	166,	166,	166,
	167,	167,	167,	167,
	168,	168,	168,	168,
	169,	169,	169,	169,	169,
	170,	170,	170,	170,	170,
	171,	171,	171,	171,
	172,	172,	172,	172,
	173,	173,	173,	173,
	174,	174,	174,	174,
	175,	175,	175,	175,
	176,	176,	176,	176,
	177,	177,	177,	177,
	178,	178,	178,	178,
	179,	179,	179,	179,
	180,	180,	180,	180,
	181,	181,	181,	181,	181,	181,
	182,	182,	182,	182,	182,	182,
	183,	183,	183,	183,
	184,	184,	184,	184,
	185,	185,	185,	185,
	186,	186,	186,	186,
	187,	187,	187,	187,	187,
	188,	188,	188,	188,	188,
	189,	189,	189,	189,	189,
	190,	190,	190,	190,	190,
	191,	191,	191,	191,	191,
	192,	192,	192,	192,	192,
	193,	193,	193,	193,	193,
	194,	194,	194,	194,	194,
	195,	195,	195,	195,	195,
	196,	196,	196,	196,	196,	196,
	197,	197,	197,	197,	197,	197,
	198,	198,	198,	198,	198,	198,
	199,	199,	199,	199,	199,	199,
	200,	200,	200,	200,	200,	200,																										//537
	201,	201,	201,	201,	201,	201,
	202,	202,	202,	202,	202,	202,
	203,	203,	203,	203,	203,	203,
	204,	204,	204,	204,	204,	204,
	205,	205,	205,	205,	205,	205,
	206,	206,	206,	206,	206,	206,
	207,	207,	207,	207,	207,	207,
	208,	208,	208,	208,	208,	208,
	209,	209,	209,	209,	209,	209,
	210,	210,	210,	210,	210,	210,
	211,	211,	211,	211,	211,	211,
	212,	212,	212,	212,	212,	212,
	213,	213,	213,	213,	213,	213,	213,	213,
	214,	214,	214,	214,	214,	214,	214,
	215,	215,	215,	215,	215,	215,
	216,	216,	216,	216,	216,	216,	216,
	217,	217,	217,	217,	217,	217,
	218,	218,	218,	218,	218,	218,	218,
	219,	219,	219,	219,	219,	219,
	220,	220,	220,	220,	220,	220,
	221,	221,	221,	221,	221,	221,
	222,	222,	222,	222,	222,	222,
	223,	223,	223,	223,	223,	223,
	224,	224,	224,	224,	224,	224,	224,
	225,	225,	225,	225,	225,	225,	225,
	226,	226,	226,	226,	226,	226,	226,
	227,	227,	227,	227,	227,	227,	227,
	228,	228,	228,	228,	228,	228,	228,	228,	228,
	229,	229,	229,	229,	229,	229,	229,	229,	229,
	230,	230,	230,	230,	230,	230,	230,
	231,	231,	231,	231,	231,	231,	231,
	232,	232,	232,	232,	232,	232,	232,
	233,	233,	233,	233,	233,	233,	233,
	234,	234,	234,	234,	234,	234,	234,	234,
	235,	235,	235,	235,	235,	235,	235,
	236,	236,	236,	236,	236,	236,	236,	236,	236,	236,
	237,	237,	237,	237,	237,	237,	237,	237,
	238,	238,	238,	238,	238,	238,	238,	238,
	239,	239,	239,	239,	239,	239,	239,	239,	239,	239,
	240,	240,	240,	240,	240,	240,	240,	240,
	241,	241,	241,	241,	241,	241,	241,	241,	241,
	242,	242,	242,	242,	242,	242,	242,	242,	242,
	243,	243,	243,	243,	243,	243,	243,	243,	243,	243,
	244,	244,	244,	244,	244,	244,	244,	244,	244,
	245,	245,	245,	245,	245,	245,	245,	245,	245,
	246,	246,	246,	246,	246,	246,	246,	246,	246,	246,	246,
	247,	247,	247,	247,	247,	247,	247,	247,	247,
	248,	248,	248,	248,	248,	248,	248,	248,	248,
	249,	249,	249,	249,	249,	249,	249,	249,	249,
	250,	250,	250,	250,	250,	250,	250,	250,	250,
	251,	251,	251,	251,	251,	251,	251,	251,	251,
	252,	252,	252,	252,	252,	252,	252,	252,	252,	252,	252,
	253,	253,	253,	253,	253,	253,	253,	253,	253,
	254,	254,	254,	254,	254,	254,	254,	254,	254,
	255,	255,	255,	255,	255,	255,	255,	255,	255,
	256,	256,	256,	256,	256,	256,	256,	256,	256,	256,
	257,	257,	257,	257,	257,	257,	257,	257,	257,	257,
	258,	258,	258,	258,	258,	258,	258,	258,	258,
	259,	259,	259,	259,	259,	259,	259,	259,	259,
	260,	260,	260,	260,	260,	260,	260,	260,	260,
	261,	261,	261,	261,	261,	261,	261,	261,	261,
	262,	262,	262,	262,	262,	262,	262,	262,	262,
	263,	263,	263,	263,	263,	263,	263,	263,	263,
	264,	264,	264,	264,	264,	264,	264,	264,	264,
	265,	265,	265,	265,	265,	265,	265,	265,	265,	265,
	266,	266,	266,	266,	266,	266,	266,	266,	266,	266,
	267,	267,	267,	267,	267,	267,	267,	267,	267,	267,
	268,	268,	268,	268,	268,	268,	268,	268,	268,	268,
	269,	269,	269,	269,	269,	269,	269,	269,	269,	269,	269,
	270,	270,	270,	270,	270,	270,	270,	270,	270,	270,	270,
	271,	271,	271,	271,	271,	271,	271,	271,	271,	271,	271,
	272,	272,	272,	272,	272,	272,	272,	272,	272,	272,	272,
	273,	273,	273,	273,	273,	273,	273,	273,	273,	273,	273,
	274,	274,	274,	274,	274,	274,	274,	274,	274,	274,	274,
	275,	275,	275,	275,	275,	275,	275,	275,	275,	275,	275,
	276,	276,	276,	276,	276,	276,	276,	276,	276,	276,	276,
	277,	277,	277,	277,	277,	277,	277,	277,	277,	277,	277,
	278,	278,	278,	278,	278,	278,	278,	278,	278,	278,	278,
	279,	279,	279,	279,	279,	279,	279,	279,	279,	279,	279,
	280,	280,	280,	280,	280,	280,	280,	280,	280,	280,	280,
	281,	281,	281,	281,	281,	281,	281,	281,	281,	281,	281,
	282,	282,	282,	282,	282,	282,	282,	282,	282,	282,	282,
	283,	283,	283,	283,	283,	283,	283,	283,	283,	283,	283,
	284,	284,	284,	284,	284,	284,	284,	284,	284,	284,	284,
	285,	285,	285,	285,	285,	285,	285,	285,	285,	285,	285,	285,
	286,	286,	286,	286,	286,	286,	286,	286,	286,	286,	286,	286,
	287,	287,	287,	287,	287,	287,	287,	287,	287,	287,	287,	287,	287,	287,
	288,	288,	288,	288,	288,	288,	288,	288,	288,	288,	288,	288,	288,	288,
	289,	289,	289,	289,	289,	289,	289,	289,	289,	289,	289,	289,	289,	289,
	290,	290,	290,	290,	290,	290,	290,	290,	290,	290,	290,	290,
	291,	291,	291,	291,	291,	291,	291,	291,	291,	291,	291,	291,	291,	291,
	292,	292,	292,	292,	292,	292,	292,	292,	292,	292,	292,	292,	292,	292,
	293,	293,	293,	293,	293,	293,	293,	293,	293,	293,	293,	293,	293,	293,
	294,	294,	294,	294,	294,	294,	294,	294,	294,	294,	294,	294,	294,	294,
	295,	295,	295,	295,	295,	295,	295,	295,	295,	295,	295,	295,	295,	295,
	296,	296,	296,	296,	296,	296,	296,	296,	296,	296,	296,	296,	296,	296,	296,
	297,	297,	297,	297,	297,	297,	297,	297,	297,	297,	297,	297,	297,	297,	297,
	298,	298,	298,	298,	298,	298,	298,	298,	298,	298,	298,	298,	298,	298,	298,	298,
	299,	299,	299,	299,	299,	299,	299,	299,	299,	299,	299,	299,	299,	299,	299,	299,
	300,	300,	300,	300,	300,	300,	300,	300,	300,	300,	300,	300,	300,	300,	300,											//1468
	301,	301,	301,	301,	301,	301,	301,	301,	301,	301,	301,	301,	301,	301,	301,	301,
	302,	302,	302,	302,	302,	302,	302,	302,	302,	302,	302,	302,	302,	302,	302,	302,
	303,	303,	303,	303,	303,	303,	303,	303,	303,	303,	303,	303,	303,	303,	303,	303,	303,	303,
	304,	304,	304,	304,	304,	304,	304,	304,	304,	304,	304,	304,	304,	304,	304,	304,	304,
	305,	305,	305,	305,	305,	305,	305,	305,	305,	305,	305,	305,	305,	305,	305,	305,	305,	305,
	306,	306,	306,	306,	306,	306,	306,	306,	306,	306,	306,	306,	306,	306,	306,	306,	306,
	307,	307,	307,	307,	307,	307,	307,	307,	307,	307,	307,	307,	307,	307,	307,	307,	307,
	308,	308,	308,	308,	308,	308,	308,	308,	308,	308,	308,	308,	308,	308,	308,	308,	308,	308,
	309,	309,	309,	309,	309,	309,	309,	309,	309,	309,	309,	309,	309,	309,	309,	309,	309,	309,
	310,	310,	310,	310,	310,	310,	310,	310,	310,	310,	310,	310,	310,	310,	310,	310,	310,	310,	310,	310,
	311,	311,	311,	311,	311,	311,	311,	311,	311,	311,	311,	311,	311,	311,	311,	311,	311,	311,	311,	311,
	312,	312,	312,	312,	312,	312,	312,	312,	312,	312,	312,	312,	312,	312,	312,	312,	312,	312,	312,
	313,	313,	313,	313,	313,	313,	313,	313,	313,	313,	313,	313,	313,	313,	313,	313,	313,	313,	313,
	314,	314,	314,	314,	314,	314,	314,	314,	314,	314,	314,	314,	314,	314,	314,	314,	314,	314,	314,	314,
	315,	315,	315,	315,	315,	315,	315,	315,	315,	315,	315,	315,	315,	315,	315,	315,	315,	315,
	316,	316,	316,	316,	316,	316,	316,	316,	316,	316,	316,	316,	316,	316,	316,	316,	316,	316,
	317,	317,	317,	317,	317,	317,	317,	317,	317,	317,	317,	317,	317,	317,	317,	317,	317,	317,
	318,	318,	318,	318,	318,	318,	318,	318,	318,	318,	318,	318,	318,	318,	318,	318,	318,	318,	318,
	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,	319,
	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,	320,
	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,	321,
	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,	322,
	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,	323,
	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,	324,
	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,	325,
	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,	326,
	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,	327,
	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,	328,
	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,	329,
	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,	330,
	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,	331,
	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,	332,
	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,	333,
	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,	334,
	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,	335,
	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,	336,
	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,	337,
	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,	338,
	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,	339,
	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,	340,
	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,	341,
	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,	342,
	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,	343,
	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,	344,
	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,	345,
	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,	346,
	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,	347,
	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,	348,
	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,	349,
	350,	350,	350,	350,	350,																												//2555
};

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

unsigned short ana_sample_cali_init_cnt(void)		//上电初始化baseline校准buff，返回上电校准次数
{
	unsigned char i;
	unsigned short cnt = 800;

	for(i = 0; i < ANA_TOTAL_COUNT; i++)
	{
		baseline_top[i] = TOP_INIT_VALUE;
		baseline_bottom[i] = BOTTOM_INIT_VALUE;
		
		baseline_step[i] = (unsigned int)((1 << 21) / (TOP_INIT_VALUE-BOTTOM_INIT_VALUE));
		
		baseline_data_init(i);
		
		if_adjust_gain_flag[i] = 0;

		node_backup[i] = 0;
		
		raw_data_filter[i] = 0;

		raw_data_backup[i] = 0;

		hall_inverse_flag[i] = 0;

		if (multi_cali_analog[i][1] > (31 << 1))multi_cali_start_point[i] = 0;		//0.3mm校准压缩处理起始analog值
		else multi_cali_start_point[i] = 31 - (multi_cali_analog[i][1] - 31);
	}

	return cnt;
}

unsigned char if_has_gain_adjust = 0;
extern unsigned char p_cur_gain_buff[ANA_TOTAL_COUNT];
void baseline_cali_handle(unsigned char key_index, unsigned short raw_data)
{
	if((raw_data > 9000) || (raw_data < 128))							//异常数据跳过
	{
		crc_error_count++;
		
		return;
	}
	
	if(raw_data > TOP_INIT_VALUE)
	{
		top_timer_cnt[key_index]++;
		
		if(mcu_slave_cmd.cmd.cali_init_cnt || mcu_slave_cmd.cmd.cali_gain_cnt[key_index])			//上电快速校准top baseline 600times,增益校准后
		{
			if(mcu_slave_cmd.cmd.cali_gain_cnt[key_index])mcu_slave_cmd.cmd.cali_gain_cnt[key_index]--;		//完成一次校准后，cnt--
			
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
			
			if((mcu_slave_cmd.cmd.cali_init_cnt <= 1) && (mcu_slave_cmd.cmd.cali_gain_cnt[key_index] <= 1))		//最后一次再记录top baseline，并计算step
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
		if(bottom_baseline_adjust_cnt[key_index] <= 5120)bottom_baseline_adjust_cnt[key_index]++;			//8K情况下，计数低于bottom baseline初值的前5120包raw data
		
		if(baseline_bottom[key_index]  > (1000 + raw_data))			//低于当前bottom baseline 1000以上，直接更新bottom baseline为raw data + 400	
		{
			baseline_bottom[key_index] = raw_data + 400;

			//以下multi_cali_raw[][6]均为3.7mm校准raw data值，需替换为ASUS变量
			if (multi_cali_raw[key_index][6] && (multi_cali_raw[key_index][6] != 0xffff))								//多点值非0，且不等于flash默认值，确认有做多点校准，具体以ASUS实际处理为准
			{
				if ((baseline_bottom[key_index] + BOTTOM_BASELINE_LIMITATION) <= multi_cali_raw[key_index][6])		//限制bottom baseline最低为多点校准值下600
				{
					baseline_bottom[key_index] = multi_cali_raw[key_index][6] - BOTTOM_BASELINE_LIMITATION;
				}
			}

			baseline_step[key_index] = (unsigned int)((1 << 21) / (baseline_top[key_index] -(baseline_bottom[key_index] + AJUST_VALUE)));
		}
		else if(baseline_bottom[key_index]  > (50 + raw_data))			//raw data 低于当前bottom baseline 50以上时进行校准
		{			
			if(bottom_baseline_adjust_cnt[key_index] <= 5120)				//前5120包数据有效
			{
				if(if_adjust_flag[key_index] == 0)						//保证每次按下时，如果前1024包中有满足校准条件的数据，至少第一次会校准
				{
					if_adjust_flag[key_index] = 1;

					baseline_bottom[key_index] = (baseline_bottom[key_index] + raw_data) >> 1;			//更新成raw data和bottom baseline的平均值

					if (multi_cali_raw[key_index][6] && (multi_cali_raw[key_index][6] != 0xffff))
					{
						if ((baseline_bottom[key_index] + BOTTOM_BASELINE_LIMITATION) <= multi_cali_raw[key_index][6])
						{
							baseline_bottom[key_index] = multi_cali_raw[key_index][6] - BOTTOM_BASELINE_LIMITATION;
						}
					}

					baseline_step[key_index] = (unsigned int)((1 << 21) / (baseline_top[key_index] - (baseline_bottom[key_index] + AJUST_VALUE)));
				}
				else if((bottom_baseline_adjust_cnt[key_index] & 0xff) == 0)			//每256包数据有效
				{
					baseline_bottom[key_index] = (baseline_bottom[key_index] + raw_data) >> 1;	

					if (multi_cali_raw[key_index][6] && (multi_cali_raw[key_index][6] != 0xffff))
					{
						if ((baseline_bottom[key_index] + BOTTOM_BASELINE_LIMITATION) <= multi_cali_raw[key_index][6])
						{
							baseline_bottom[key_index] = multi_cali_raw[key_index][6] - BOTTOM_BASELINE_LIMITATION;
						}
					}

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

unsigned char ana_calc_node_data(unsigned char index, unsigned short raw_data)
{
	unsigned short result;
	unsigned short sample_noise;
	unsigned int calc_node;
	
//	if((raw_data + FILTER_VALUE)> baseline_top[index])
//	{
//		return 0xff;			//如果触发值为1，读到分段值0xff才松开按键
//	}
//	else if((raw_data + TOP_DEADZONE_VALUE)> baseline_top[index])
//	{
//		return 0;
//	}
//	else 
	if(raw_data < (baseline_bottom[index] + BOTTOM_DEADZONE_VALUE))
	{
		calc_node = 2559;
	}
	else
	{
		sample_noise = baseline_top[index] - raw_data;

		calc_node = (unsigned int)((sample_noise * baseline_step[index] * 2559) >> 21);
		if(calc_node > 2559)calc_node = 2559;
	}
	
	//改动中出现的multi_cali_analog[][1]均为0.3mm校准analog值，需替换为ASUS变量，31对应的都是理论0.3mm analog值
	if(multi_cali_inverse_handle)										//是否启用0.3mm处校准压缩处理
	{ 
		if (calc_node > 31)
		{
			if (multi_cali_analog[index][1] > 31)
			{
				if (calc_node < multi_cali_analog[index][1])
				{
					if(multi_cali_analog[index][1] <= (31 << 1))
						calc_node = multi_cali_start_point[index] + ((calc_node - multi_cali_start_point[index]) >> 1);
					else
					{
						if (calc_node < (31 << 1))
						{
							calc_node >>= 1;
						}
						else calc_node = 31;
					}
				}
					
				else
				{
					calc_node -= (multi_cali_analog[index][1] - 31);						//实时analog超过校准analog值的部分需替换回ASUS原本0.3mm部分计算方式
				}
			}
		}
		else
		{
			if (multi_cali_analog[index][1] > 31)
			{
				if (calc_node >= multi_cali_start_point[index])
					calc_node = multi_cali_start_point[index] + ((calc_node - multi_cali_start_point[index]) >> 1);
			}
		}
	}
	
	analog_node_backup[index] = (unsigned short)calc_node;

	result = node_cali_table[(unsigned short)calc_node];

	return result;
}

unsigned char color_order=0;
void ana_hal_update_pwm_buff()
{
	unsigned char k=0;
	unsigned char temp_pwm=0;

	for(unsigned char i=0;i<LED_ROW_ANA;i++)
	{
		for(unsigned char j=0;j<LED_COL_ANA;j++)
		{				
			if(key_pos_table_ana[j][i]!=RES && k <ANA_TOTAL_COUNT )
			{									
				if(color_order==0)
				{
					temp_pwm = led_scan.pwm_duty_b[i][j];		
				}else if(color_order==1)
				{
					temp_pwm = led_scan.pwm_duty_g[i][j];	
				}else if(color_order==2)
				{
					temp_pwm = led_scan.pwm_duty_r[i][j];	
				}
				
				ana_wr_buff[k*2+0] = color_order<<5;
				ana_wr_buff[k*2+1] = temp_pwm;
				k++;
			}
		}
	}
	color_order++;
	if(color_order>=3)
	{
		color_order=0;
	}
}

// mode=1 : normal; mode=0 : suspend
unsigned char  ana_hal_sample(unsigned char mode)
{
	unsigned char i,j,k=0,ret=0;
	unsigned short temp[ANA_TOTAL_COUNT];
	static unsigned char cali_index = 0;
	
	if(ana_init_success==0) return 0;

	if(mode==1)
	{		
		ana_hal_update_pwm_buff();
		ana_phy_write_pwm_and_convert(ANA_TOTAL_COUNT, ana_wr_buff,ana_rd_buff );		
	}else
	{		
		ana_phy_sensor_convert(ANA_TOTAL_COUNT, ana_wr_buff,ana_rd_buff );	
		return 1;
	}
	
	if(mcu_slave_cmd.cmd.cali_init_cnt)mcu_slave_cmd.cmd.cali_init_cnt--;		// init 600
//	if(mcu_slave_cmd.cmd.cali_gain_cnt)mcu_slave_cmd.cmd.cali_gain_cnt--;		//自动校准增益后，需重新快速更新top baseline

	if(mcu_slave_cmd.cmd.cali_init_cnt)
	{
		for(j = 0;j<KB_ROW_ANA;j++)
		{				
			for (i = 0;i <KB_COL_ANA;i++)
			{
				if(key_pos_table_ana[i][j]!=RES)
				{
					if(k<ANA_TOTAL_COUNT)
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
			}
		}

		ret = 0;
	}
	else
	{
		test_pin_low();		//拉低IO测试hall数据处理时间

		for (j = 0;j < KB_ROW_ANA;j++)
		{
			for (i = 0;i < KB_COL_ANA;i++)
			{
				if (key_pos_table_ana[i][j] != RES)
				{
					if (k < ANA_TOTAL_COUNT)
					{
						temp[k] = ana_phy_bit_error_checker2(ana_rd_buff[2 * k], ana_rd_buff[2 * k + 1]);		//获取raw data

						if (temp[k] != CRC_ERROR)
						{
#if MAGNETIC_POLARITY_N						//如果磁铁N极向sensor，调返hall值变化方向
							temp[k] = 15927 - temp[k];
#endif

							if ((temp[k] > 9000) || (temp[k] < 128))							//异常数据跳过
							{
								crc_error_count++;
								ret = 1;
								k++;
								continue;
							}
/********************************************死区优化***********************/
							rawdata_original[k] = temp[k];
/********************************************死区优化***********************/
							if ((raw_data_filter[k] > (temp[k] + FILTER_SKIP_VALUE)) || ((raw_data_filter[k] + FILTER_SKIP_VALUE) < temp[k]))		//实际raw data相较滤波后的值变化超过40后，直接跟随
							{
								raw_data_filter[k] = temp[k];
								raw_data_filter_sum[k] = temp[k] << length_filter_bit;
							}
							else
							{
								if(top_deadzone_value == TOP_DEADZONE_VALUE)
								{
									if (temp[k] > (baseline_top[k] - TOP_RESET_VALUE))
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
								}
								else
								{
									raw_data_filter_sum[k] += temp[k];
									raw_data_filter_sum[k] -= raw_data_filter[k];
									raw_data_filter[k] = raw_data_filter_sum[k] >> length_filter_bit;
									temp[k] = raw_data_filter[k];
								}
							}

							if (mcu_slave_cmd.cmd.cali_gain_cnt[k])
							{
								baseline_cali_handle(k, temp[k]);							//确认漂移后，快速校准漂移的按键
							}
/*******************************************死区优化********************************************************/
							if(Bottom_Status_Handle_Flag[k] == 1)
							{
								node[k] = Bottom_Status_Segment_Handle(rawdata_original[k],temp[k],k);

								if(node[k] < BOTTOM_STATUS_HANDLE_LINE)		//分段小于特殊处理线退出特殊处理
								{
									Bottom_Status_Handle_Flag[k] = 0;
									
								}
								if(ana_calc_node_data(k, temp[k]) < BOTTOM_STATUS_HANDLE_LINE - 30)	   //防止卡死机制
								{
									Bottom_Status_Handle_Flag[k] = 0;
									
								}

								Bottom_Status_Need_Reset_Flag[k] = 1;


								k++;
								ret = 1;
								continue;
							}
							//抬到顶端清除标志位和数组
							if((node[k] == 0) && (Bottom_Status_Need_Reset_Flag[k] == 1))
							{
								Bottom_Status_Packet_List_Reset(k);
								Bottom_Status_Need_Reset_Flag[k] = 0;
							}
/*******************************************死区优化********************************************************/

							if (temp[k] >= (baseline_top[k] - top_reset_value))				//低于top baseline不超过10，分段值为0，方向向下，跳过
							{
								node[k] = 0;
								hall_inverse_flag[k] = 0;
								raw_data_backup[k] = temp[k];
								
								if ((top_reset_value == TOP_RESET_VALUE) && (temp[k] < (baseline_top[k] - 15)))
								{
									hall_change_key_num++;
									hall_change_key[k]++;
								}
								else
								{
									hall_change_key[k] = 0;
								}
								ret = 1;
								k++;
								continue;
							}
							else if (temp[k] >= (baseline_top[k] - top_deadzone_value))		//低于top baseline在10~25，直接跳过，分段值和方向保持之前状态
							{
								if (top_reset_value == TOP_RESET_VALUE)
								{
									hall_change_key_num++;
									hall_change_key[k]++;
								}
								else
								{
									hall_change_key[k] = 0;
								}
								ret = 1;
								k++;
								continue;
							}
							else if (hall_inverse_flag[k] == 0)									//hall向下变化
							{
								if (temp[k] >= (baseline_top[k] - 100))	//hall值相较baseline变化在35～100，则hall值算漂移
								{
									hall_change_key_num++;
									hall_change_key[k]++;
								}
								else
								{
									if (hall_change_key[k])hall_change_key[k]--;
								}

								if (analog_node_backup[k] < 1000)
								{
									if ((temp[k] >= raw_data_backup[k]) && (temp[k] < (raw_data_backup[k] + HALL_INVERSE_VALUE)))			//hall变大不超过10，跳过
									{
										ret = 1;
										k++;
										continue;
									}
								}
								else
								{
									if ((temp[k] >= raw_data_backup[k]) && (temp[k] < (raw_data_backup[k] + HALL_INVERSE_VALUE2)))			//hall变大不超过24，跳过
									{
										ret = 1;
										k++;
										continue;
									}
								}	
							}
							else																//hall向上变化
							{
								if (temp[k] >= (baseline_top[k] - 100))	//hall值相较baseline变化在35～100，则hall值算漂移
								{
									hall_change_key_num++;
									hall_change_key[k]++;
								}
								else
								{
									if (hall_change_key[k])hall_change_key[k]--;
								}

								if (analog_node_backup[k] < 1000)
								{
									if ((temp[k] > (raw_data_backup[k] - HALL_INVERSE_VALUE)) && (temp[k] <= raw_data_backup[k]))			//hall变小不超过8，跳过
									{
										ret = 1;
										k++;
										continue;
									}
								}
								else
								{
									if ((temp[k] > (raw_data_backup[k] - HALL_INVERSE_VALUE2)) && (temp[k] <= raw_data_backup[k]))			//hall变小不超过24，跳过
									{
										ret = 1;
										k++;
										continue;
									}
								}
							}

							if (temp[k] > raw_data_backup[k])								//hall值变大，方向为上
							{
								if (raw_data_backup[k])
								{
									hall_inverse_flag[k] = 1;
								}
								else														//如果备份raw data为初始值0， 方向固定为下
								{
									hall_inverse_flag[k] = 0;
								}
							}
							else															//hall值变小，方向为下
							{
								hall_inverse_flag[k] = 0;
							}

							if (temp[k] < BOTTOM_INIT_VALUE)
							{
								baseline_cali_handle(k, temp[k]);							//bottom baseline需要每次都进行校准
							}

							node[k] = ana_calc_node_data(k, temp[k]);

							raw_data_backup[k] = temp[k];

							if (node[k] < 100)
							{
								bottom_baseline_adjust_cnt[k] = 0;					//按键松开后，bottom baseline校准计数清零
								if_adjust_flag[k] = 0;

								if (node_backup[k] > 300)				//每次按键按到底后松开，bottom baseline + 5
								{
									node_backup[k] = 0;

									if (baseline_bottom[k] < BOTTOM_INIT_VALUE)
									{
										baseline_bottom[k] += 5;
										baseline_step[k] = (unsigned int)((1 << 21) / (baseline_top[k] - (baseline_bottom[k] + AJUST_VALUE)));
									}
								}
							}
							else if (node[k] > 300)
							{
								node_backup[k] = node[k];
							}
/***********************************死区优化*******************************************/
							if(node[k] >= BOTTOM_STATUS_HANDLE_LINE) //分段超过线开启死区优化标志位
							{
								Bottom_Status_Handle_Flag[k] = 1;

							}
/***********************************死区优化*******************************************/
						}
						else
						{// error
							crc_error_count++;
							ret = 1;
						}

					}
					else
					{
						return ret;
					}
					k++;
				}
			}

		}

		if (hall_change_key_num >= 2)			//所有按键一个周期有超过2键的hall值有漂移判定为一次
		{
			hall_change_timer_cnt++;

			if (hall_change_timer_cnt > 4000)	//连续4000次都有2键hall漂移，则快速校准baseline
			{
				hall_change_timer_cnt = 0;

				for (k = 0; k < ANA_TOTAL_COUNT; k++)
				{
					if (hall_change_key[k] > 4000)
					{
						mcu_slave_cmd.cmd.cali_gain_cnt[k] = 50;
					}
				}
			}
		}
		else
		{
			hall_change_timer_cnt = 0;
		}
		hall_change_key_num = 0;

		if (temp[cali_index] != CRC_ERROR)
		{
			baseline_cali_handle(cali_index, temp[cali_index]);		//baseline实时校准，每个周期校准一个按键

			cali_index++;
			if (cali_index >= ANA_TOTAL_COUNT)cali_index = 0;
		}

		if(if_has_gain_adjust)
		{
			if_has_gain_adjust = 0;
			
			ana_hall5_phy_set_gain(ANA_TOTAL_COUNT,p_cur_gain_buff);					//将更新后的增益值写入寄存器
		}
		
		test_pin_high();//拉高IO测试hall数据处理时间
		
		ret = 1;
	}
		
	return ret;
}

unsigned char  ana_hal_sample_test_mode(unsigned char mode)
{
	unsigned char i, j, k = 0, ret = 0;
	unsigned short temp[ANA_TOTAL_COUNT];
	static unsigned char cali_index = 0;

	if (ana_init_success == 0) return 0;

	if (mode == 1)
	{
		ana_hal_update_pwm_buff();
		ana_phy_write_pwm_and_convert(ANA_TOTAL_COUNT, ana_wr_buff, ana_rd_buff);
	}
	else
	{
		ana_phy_sensor_convert(ANA_TOTAL_COUNT, ana_wr_buff, ana_rd_buff);
		return 1;
	}

	test_pin_low();		//拉低IO测试hall数据处理时间

	for (j = 0;j < KB_ROW_ANA;j++)
	{
		for (i = 0;i < KB_COL_ANA;i++)
		{
			if (key_pos_table_ana[i][j] != RES)
			{
				if (k < ANA_TOTAL_COUNT)
				{
					temp[k] = ana_phy_bit_error_checker2(ana_rd_buff[2 * k], ana_rd_buff[2 * k + 1]);		//获取raw data

					if (temp[k] != CRC_ERROR)
					{
#if MAGNETIC_POLARITY_N						//如果磁铁N极向sensor，调返hall值变化方向
						temp[k] = 15927 - temp[k];
#endif

						if ((temp[k] > 9000) || (temp[k] < 128))							//异常数据跳过
						{
							crc_error_count++;
							ret = 1;
							k++;
							continue;
						}

						if ((raw_data_filter[k] > (temp[k] + FILTER_SKIP_VALUE)) || ((raw_data_filter[k] + FILTER_SKIP_VALUE) < temp[k]))		//实际raw data相较滤波后的值变化超过40后，直接跟随
						{
							raw_data_filter[k] = temp[k];
							raw_data_filter_sum[k] = temp[k] << length_filter_bit;
						}
						else
						{
							if (top_deadzone_value == TOP_DEADZONE_VALUE)
							{
								if (temp[k] > (baseline_top[k] - TOP_RESET_VALUE))
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
							}
							else
							{
								raw_data_filter_sum[k] += temp[k];
								raw_data_filter_sum[k] -= raw_data_filter[k];
								raw_data_filter[k] = raw_data_filter_sum[k] >> length_filter_bit;
								temp[k] = raw_data_filter[k];
							}
						}


						if (temp[k] >= (baseline_top[k] - top_reset_value))				//低于top baseline不超过10，分段值为0，方向向下，跳过
						{
							node[k] = 0;
							hall_inverse_flag[k] = 0;
							raw_data_backup[k] = temp[k];
							ret = 1;
							k++;
							continue;
						}
						else if (temp[k] >= (baseline_top[k] - top_deadzone_value))		//低于top baseline在10~25，直接跳过，分段值和方向保持之前状态
						{

							ret = 1;
							k++;
							continue;
						}
						else if (hall_inverse_flag[k] == 0)									//hall向下变化
						{
							if (analog_node_backup[k] < 1000)
							{
								if ((temp[k] >= raw_data_backup[k]) && (temp[k] < (raw_data_backup[k] + HALL_INVERSE_VALUE)))			//hall变大不超过10，跳过
								{
									ret = 1;
									k++;
									continue;
								}
							}
							else
							{
								if ((temp[k] >= raw_data_backup[k]) && (temp[k] < (raw_data_backup[k] + HALL_INVERSE_VALUE2)))			//hall变大不超过24，跳过
								{
									ret = 1;
									k++;
									continue;
								}
							}
						}
						else																//hall向上变化
						{
							if (analog_node_backup[k] < 1000)
							{
								if ((temp[k] > (raw_data_backup[k] - HALL_INVERSE_VALUE)) && (temp[k] <= raw_data_backup[k]))			//hall变小不超过8，跳过
								{
									ret = 1;
									k++;
									continue;
								}
							}
							else
							{
								if ((temp[k] > (raw_data_backup[k] - HALL_INVERSE_VALUE2)) && (temp[k] <= raw_data_backup[k]))			//hall变小不超过24，跳过
								{
									ret = 1;
									k++;
									continue;
								}
							}
						}

						if (temp[k] > raw_data_backup[k])								//hall值变大，方向为上
						{
							if (raw_data_backup[k])
							{
								hall_inverse_flag[k] = 1;
							}
							else														//如果备份raw data为初始值0， 方向固定为下
							{
								hall_inverse_flag[k] = 0;
							}
						}
						else															//hall值变小，方向为下
						{
							hall_inverse_flag[k] = 0;
						}

						if (temp[k] < BOTTOM_INIT_VALUE)
						{
							baseline_cali_handle(k, temp[k]);							//bottom baseline需要每次都进行校准
						}

						node[k] = ana_calc_node_data(k, temp[k]);

						raw_data_backup[k] = temp[k];
					}
					else
					{// error
						crc_error_count++;
						ret = 1;
					}

				}
				else
				{
					return ret;
				}
				k++;
			}
		}

	}

	test_pin_high();//拉高IO测试hall数据处理时间

	return ret;
}
void ana_hal_sample_mfg_mode(void)
{
	if(ana_init_success)
	{
		ana_hal_sample(1);
	}
	else
	{
		//ana_data_chain_check_and_report();
	}
}

void mfg_mode_poll(void)
{	 
	 ana_hal_sample_mfg_mode();
}


#define SHAKE_OFF_COUNT 1000	
uint16_t handle_abs(uint16_t first_data,uint16_t second_data)
{
	uint16_t result = 0;
	result = (first_data > second_data) ? (first_data - second_data):(second_data - first_data);
	return result;
}
void Bottom_Status_Packet_List_Reset(unsigned char Keyindex)
{
	uint8_t i;

	DMA_SPI_RX_Buffer_Bottom_Status_Packet[Keyindex] = DMA_SPI_RX_Buffer_Bottom_Status_Packet_count;
	DMA_SPI_RX_Buffer_Bottom_Status_Init_Packet_count[Keyindex] = BOTTOM_STATUS_PACKET_LIST_LENGTH;

	DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex] = &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][0];
	DMA_SPI_RX_Buffer_Bottom_Status_Packet_Old_Data_Point[Keyindex] = &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][0];
	Shake_Off_Count[Keyindex] = 0;


	Slow_Raise_Flag[Keyindex] = 0;
	Quick_Press_Flag[Keyindex] = 0;
	Hold_Raise_Disable[Keyindex] = 0;

	Bottom_Press_Status_Segment_Backup[Keyindex] = 0;

	for(i = 0; i < BOTTOM_STATUS_PACKET_LIST_LENGTH; i++)
	{
		DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][i] = 0;
	}

}

void Bottom_Status_Packet_List_Init(void)
{
	uint8_t i,j;
	for(j = 0; j < ANA_TOTAL_COUNT; j++)
	{
		DMA_SPI_RX_Buffer_Bottom_Status_Packet[j] = DMA_SPI_RX_Buffer_Bottom_Status_Packet_count;
		DMA_SPI_RX_Buffer_Bottom_Status_Init_Packet_count[j] = BOTTOM_STATUS_PACKET_LIST_LENGTH;

		DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[j] = &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[j][0];
		DMA_SPI_RX_Buffer_Bottom_Status_Packet_Old_Data_Point[j] = &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[j][0];
		Shake_Off_Count[j] = 0;

		Bottom_Status_Handle_Flag[j] = 0;
		Slow_Raise_Flag[j] = 0;
		Quick_Press_Flag[j] = 0;
		Hold_Raise_Disable[j] = 0;

		Bottom_Press_Status_Segment_Backup[j] = 0;

		for(i = 0; i < BOTTOM_STATUS_PACKET_LIST_LENGTH; i++)
		{
			DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[j][i] = 0;
		}
	}

}


uint16_t Bottom_Status_Segment_Handle(uint16_t Original_Rawdata,uint16_t Filter_Rawdata,uint8_t Keyindex)
{
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	//没有进最终判断的中间包数的分段值判断
	uint16_t segment_result = ana_calc_node_data(Keyindex, Original_Rawdata);
	//按下的分段值只升不降
	if(segment_result > Bottom_Press_Status_Segment_Backup[Keyindex])
	{
		Bottom_Press_Status_Segment_Backup[Keyindex] = segment_result;
	}
	else
	{
		segment_result = Bottom_Press_Status_Segment_Backup[Keyindex];
	}

	if(Slow_Raise_Flag[Keyindex] == 1)
	{
		segment_result = Bottom_Status_Segment_Backup[Keyindex];
	}

	{
		//不用滤波的快速按下,快速抬起,和晃动判断

		//rawdata变化大于35记录为快速抬起
		if(Original_Rawdata > Original_Rawdata_Status_backup[Keyindex] + RawDate_Quick_Press_Break_Range)
		{
			DMA_SPI_RX_Buffer_Quick_Break_Packet[Keyindex]++;// 记录快速抬起次数
			//清空其他记录
			DMA_SPI_RX_Buffer_Quick_Press_Packet[Keyindex] = 0;
			DMA_SPI_RX_Buffer_Shake_Packet[Keyindex] = 0;

		}
		//rawdata变化大于35记录为快速按下
		else if(Original_Rawdata_Status_backup[Keyindex] > Original_Rawdata + RawDate_Quick_Press_Break_Range)
		{
			DMA_SPI_RX_Buffer_Quick_Press_Packet[Keyindex]++;//记录快速按下次数
			//清空其他记录
			DMA_SPI_RX_Buffer_Quick_Break_Packet[Keyindex] = 0;
			DMA_SPI_RX_Buffer_Shake_Packet[Keyindex] = 0;

		}
		//rawdata变化大于10小于35记录为晃动
		//晃动
		else if(((handle_abs(Original_Rawdata,Original_Rawdata_Status_backup[Keyindex])) > RawDate_Has_Change_C) && ((handle_abs(Original_Rawdata,Original_Rawdata_Status_backup[Keyindex])) < RawDate_Quick_Press_Break_Range))
		{
			DMA_SPI_RX_Buffer_Shake_Packet[Keyindex]++;
			//清空其他记录
			DMA_SPI_RX_Buffer_Quick_Break_Packet[Keyindex] = 0;
			DMA_SPI_RX_Buffer_Quick_Press_Packet[Keyindex] = 0;

		}

		//产生了快速按下就不能去行程机慢升判断防止晃动过程中识别到行程机慢升导致分段变化抬起断触
		if(Hold_Raise_Disable[Keyindex] == 0)
		{
					//需要滤波的行程机慢升和按住情况
					//rawdata变化小于10,抬起方向
					if(((Filter_Rawdata - Filter_Rawdata_Status_backup[Keyindex]) < RawDate_Has_Change_C) && (Filter_Rawdata > Filter_Rawdata_Status_backup[Keyindex]))
					{

						if(Shake_Off_Count[Keyindex])//识别到晃动就加防止与按住行程机慢升混淆导致抬起断触
						{

							Shake_Off_Count[Keyindex]--;

							segment_result = Bottom_Press_Status_Segment_Backup[Keyindex];
							
							return segment_result;
						}


						//每隔5包记录一次,数组长度为5
						if(!DMA_SPI_RX_Buffer_Bottom_Status_Packet[Keyindex])//每DMA_SPI_RX_Buffer_Bottom_Status_Packet_count包记录
						{

							{
								//每隔包记录一个值,初始化记录包
								if(DMA_SPI_RX_Buffer_Bottom_Status_Init_Packet_count[Keyindex] > 0)
								{
									*DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex] = Filter_Rawdata;
									//New_Data_Point指针初始化完指向末尾
									if(DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex] != &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][BOTTOM_STATUS_PACKET_LIST_LENGTH - 1])
									{
										DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex]++;
									}

									DMA_SPI_RX_Buffer_Bottom_Status_Init_Packet_count[Keyindex]--;

								
								}
								//初始化数组完成
								else
								{


									if(*DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex] > *DMA_SPI_RX_Buffer_Bottom_Status_Packet_Old_Data_Point[Keyindex] + RawDate_Slow_Raise_Range)
									{

										DMA_SPI_RX_Buffer_Slow_Raise_Packet[Keyindex]++;
										DMA_SPI_RX_Buffer_Hold_Packet[Keyindex] = 0;
									




									}
									else
									{
										DMA_SPI_RX_Buffer_Slow_Raise_Packet[Keyindex] = 0;
										DMA_SPI_RX_Buffer_Hold_Packet[Keyindex]++;
										

									



									}

									//新数据指针指向最后一个元素就指回头元素
									if(DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex] == &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][BOTTOM_STATUS_PACKET_LIST_LENGTH - 1])
									{
										DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex] = &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][0];

									}
									else//否则则移动
									{
										DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex]++;

									}
									//存储新数据
									*DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex] = Filter_Rawdata;

									//旧数据指针移动
									///旧数据指针指向最后一个元素就指回头元素
									if(DMA_SPI_RX_Buffer_Bottom_Status_Packet_Old_Data_Point[Keyindex] == &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][BOTTOM_STATUS_PACKET_LIST_LENGTH - 1])
									{
										DMA_SPI_RX_Buffer_Bottom_Status_Packet_Old_Data_Point[Keyindex] = &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][0];

									}
									else//否则则移动
									{
										DMA_SPI_RX_Buffer_Bottom_Status_Packet_Old_Data_Point[Keyindex]++;

									}
								}

								DMA_SPI_RX_Buffer_Bottom_Status_Packet[Keyindex] = DMA_SPI_RX_Buffer_Bottom_Status_Packet_count;
							}



						}
						DMA_SPI_RX_Buffer_Bottom_Status_Packet[Keyindex]--;


					}
		}

		//特殊处理行程机慢升后再次按下
		//按下方向
		if(((Filter_Rawdata_Status_backup[Keyindex] - Filter_Rawdata) < RawDate_Has_Change_C) && (Filter_Rawdata < Filter_Rawdata_Status_backup[Keyindex]) && (Slow_Raise_Flag[Keyindex] == 1) && (Hold_Raise_Disable[Keyindex] == 0))
		{
			if(!DMA_SPI_RX_Buffer_Bottom_Status_Packet[Keyindex])
			{


				if(*DMA_SPI_RX_Buffer_Bottom_Status_Packet_Old_Data_Point[Keyindex] > *DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex] + RawDate_Slow_Raise_Range)
				{
					DMA_SPI_RX_Buffer_Press_Again_Packet[Keyindex]++;

				}

				//新数据指针指向最后一个元素就指回头元素
				if(DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex] == &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][BOTTOM_STATUS_PACKET_LIST_LENGTH - 1])
				{
					DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex] = &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][0];

				}
				else//否则则移动
				{
					DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex]++;

				}
				//存储新数据
				*DMA_SPI_RX_Buffer_Bottom_Status_Packet_New_Data_Point[Keyindex] = Filter_Rawdata;

				//旧数据指针移动
				///旧数据指针指向最后一个元素就指回头元素
				if(DMA_SPI_RX_Buffer_Bottom_Status_Packet_Old_Data_Point[Keyindex] == &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][BOTTOM_STATUS_PACKET_LIST_LENGTH - 1])
				{
					DMA_SPI_RX_Buffer_Bottom_Status_Packet_Old_Data_Point[Keyindex] = &DMA_SPI_RX_Buffer_Bottom_Status_Packet_List[Keyindex][0];

				}
				else//否则则移动
				{
					DMA_SPI_RX_Buffer_Bottom_Status_Packet_Old_Data_Point[Keyindex]++;

				}



				DMA_SPI_RX_Buffer_Bottom_Status_Packet[Keyindex] = DMA_SPI_RX_Buffer_Bottom_Status_Packet_count;
			}
			DMA_SPI_RX_Buffer_Bottom_Status_Packet[Keyindex]--;
		}



	}

	//更新记录rawdata对比值
	Original_Rawdata_Status_backup[Keyindex] = Original_Rawdata;
	Filter_Rawdata_Status_backup[Keyindex] = Filter_Rawdata;
#if 1
	//连续一定包判断快速松开
	if(DMA_SPI_RX_Buffer_Quick_Break_Packet[Keyindex] >= Count_Packet_D)
	{
	
		//清空其他记录
		DMA_SPI_RX_Buffer_Quick_Break_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Quick_Press_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Shake_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Hold_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Slow_Raise_Packet[Keyindex] = 0;

		segment_result = ana_calc_node_data(Keyindex, Original_Rawdata);

		if(Quick_Press_Flag[Keyindex] == 1)
		{
			Hold_Raise_Disable[Keyindex] = 1;
		}

	}
	//连续一定包判断快速按下
	else if(DMA_SPI_RX_Buffer_Quick_Press_Packet[Keyindex] >= Count_Packet_A)
	{
		
		//清空其他记录
		DMA_SPI_RX_Buffer_Quick_Break_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Quick_Press_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Shake_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Hold_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Slow_Raise_Packet[Keyindex] = 0;

		segment_result = Bottom_Press_Status_Segment_Backup[Keyindex];

		Quick_Press_Flag[Keyindex] = 1;
		Hold_Raise_Disable[Keyindex] = 1;

	}
	//连续一定包判断晃动
	else if(DMA_SPI_RX_Buffer_Shake_Packet[Keyindex] >= Count_Packet_A)
	{
		
		//清空其他记录
		DMA_SPI_RX_Buffer_Quick_Break_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Quick_Press_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Shake_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Hold_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Slow_Raise_Packet[Keyindex] = 0;
	
		segment_result = Bottom_Press_Status_Segment_Backup[Keyindex];

		Shake_Off_Count[Keyindex] = SHAKE_OFF_COUNT;
		if(Shake_Off_Count[Keyindex])//抖动过还抖动
		{
			Quick_Press_Flag[Keyindex] = 1;
		}

	}

	//连续一定包判断行程机按住
	else if(DMA_SPI_RX_Buffer_Hold_Packet[Keyindex] >= Count_Packet_B)
	{
		
		//清空其他记录
		DMA_SPI_RX_Buffer_Quick_Break_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Quick_Press_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Shake_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Hold_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Slow_Raise_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Press_Again_Packet[Keyindex] = 0;		

		segment_result = Bottom_Press_Status_Segment_Backup[Keyindex];

		if(Slow_Raise_Flag[Keyindex] == 1)//行程机慢抬后就不输出350防止又按下
		{
			segment_result = Bottom_Status_Segment_Backup[Keyindex];

		}


	}
	//连续一定包判断行程机慢升
	else if(DMA_SPI_RX_Buffer_Slow_Raise_Packet[Keyindex] >= Count_Packet_B)
	{

	
		//清空其他记录
		DMA_SPI_RX_Buffer_Quick_Break_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Quick_Press_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Shake_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Hold_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Slow_Raise_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Press_Again_Packet[Keyindex] = 0;
		

		segment_result = ana_calc_node_data(Keyindex, Filter_Rawdata);

		{

			Bottom_Status_Segment_Backup[Keyindex] = segment_result;
			Slow_Raise_Flag[Keyindex] = 1;
		}


	}
	//行程机慢升0.03mm后再次按下
	else if(DMA_SPI_RX_Buffer_Press_Again_Packet[Keyindex] >= Count_Packet_B)
	{
		

		DMA_SPI_RX_Buffer_Quick_Break_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Quick_Press_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Shake_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Hold_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Slow_Raise_Packet[Keyindex] = 0;
		DMA_SPI_RX_Buffer_Press_Again_Packet[Keyindex] = 0;

		segment_result = Bottom_Press_Status_Segment_Backup[Keyindex];
		Slow_Raise_Flag[Keyindex] = 0;

	}
#endif

	return segment_result;


}
/****************************************死区优化处理*****************************************************/
#endif
/* end of file */

