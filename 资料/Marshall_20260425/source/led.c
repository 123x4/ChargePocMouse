/*
 * custom_original.c
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <dma_config.h>
#include <hall_spi.h>
#include <led.h>
#include <Flash_iap.h>
#include <composite.h>
#include <pin_mux.h>
#include <hid_keyboard.h>
#include <fntab_mode.h>
#include <driverapp_communication.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
RGB_STRU node_num_depend(uint8_t n,uint16_t node0,uint16_t node1,uint16_t node2,uint16_t node3,uint16_t node4);
void LED_6_mode_keyAreaOn(uint8_t i, uint8_t j,RGB_STRU h_rgb);
uint8_t Current_key_state_LED[TRANSFER_Group][TRANSFER_SIZE]= {0};
/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t Flag_LedMode_mode = 1;
uint8_t Flag_LedMode_ColorIndex = 0;
uint8_t Flag_LedMode_ModeInit = 0;
uint8_t Flag_LedMode_RGBUpdate = 0;
uint8_t Flag_LedMode_ColorChange = 0;
uint8_t Flag_RGB_index = 0;
uint8_t Flag_Light_Has_Change = 0;
uint8_t sys_led_status_backup = 0;
//------------------------------------------------------------------------------------------
/* RGB速度参数：以125us一次为基础调速度 */
uint8_t Flag_LedMode_speed = 1;
uint16_t Tick_RGBSpeed_cnt = 0;
uint16_t RGB_SpeedLevel_cnt = 0;
//------------------------------------------------------------------------------------------
/* 呼吸颜色参数 */
uint8_t Breath_color_num = 0;
uint8_t Breath_color_flag = 0;
uint8_t Breath_colorful = 1;
//------------------------------------------------------------------------------------------
/* mode5位移参数 */
uint8_t led_key_mark[LED_ROW_MAX][LED_COL_MAX];
uint8_t led_right_move[LED_ROW_MAX][LED_COL_MAX];//[ID_MAX];
//------------------------------------------------------------------------------------------
/* mode6 */
uint8_t Current_key_state_buf[TRANSFER_Group][TRANSFER_SIZE]= {0};
//------------------------------------------------------------------------------------------
DrivetData_LED   	g_DrivetData_LED;
//------------------------------------------------------------------------------------------
/* mode7 */
//#define led_mode7_loop_len(n)			LED_MODE7_LOOP##n##_LEN
#define led_mode7_loop_len(n)			LED_MODE7_LOOP##n##_LEN
#define LED_MODE7_LOOP1_LEN           6
#define LED_MODE7_LOOP2_LEN           3
#define LED_MODE7_LOOP3_LEN           5
#define LED_MODE7_LOOP4_LEN           6
#define LED_MODE7_LOOP5_LEN           3
#define LED_MODE7_LOOP6_LEN           3
#define LED_MODE7_LOOP7_LEN           7
#define LED_MODE7_LOOP8_LEN           8
#define LED_MODE7_LOOP9_LEN           5
#define LED_MODE7_LOOP10_LEN          8
#define LED_MODE7_LOOP11_LEN          5
#define LED_MODE7_LOOP12_LEN          2
#define LED_MODE7_LOOP13_LEN          7

#define LOOP_LINE_MAX		13

const Position loop1[LED_MODE7_LOOP1_LEN]=
{
//    G      F      D      X      Z    SHIFT_L
	{2,5}, {2,4}, {2,3}, {3,2}, {3,1}, {3,0},
};

const Position loop2[LED_MODE7_LOOP2_LEN]=
{
//   CAPS    A      S
	{2,0}, {2,1}, {2,2}
};

const Position loop3[LED_MODE7_LOOP3_LEN]=
{
//    R      E      W      Q     TAB
	{1,4}, {1,3}, {1,2}, {1,1}, {1,0},
};

const Position loop4[LED_MODE7_LOOP4_LEN]=
{
//    T      4      3      2     1      ESC
	{1,5}, {0,4}, {0,3}, {0,2}, {0,1}, {0,0}
};

const Position loop5[LED_MODE7_LOOP5_LEN]=
{
//    Y      6      5
    {1,6}, {0,6}, {0,5}
};

const Position loop6[LED_MODE7_LOOP6_LEN]=
{
//    U      7      8
	{1,7}, {0,7}, {0,8}
};

const Position loop7[LED_MODE7_LOOP7_LEN]=
{
//    I      9      0      -_       =+      BS     Ins
	{1,8}, {0,9}, {0,10}, {0,11}, {0,12}, {0,13}, {0,14}
};
const Position loop8[LED_MODE7_LOOP8_LEN]=
{
//    J      K       O       P       [{       ]}      \|     DEL
	{2,7}, {2,8},  {1,9},  {1,10},  {1,11}, {1,12}, {1,13}, {1,14}
};

const Position loop9[LED_MODE7_LOOP9_LEN]=
{
//    L     ;:     '"     	ENTER   PGUP
	{2,9}, {2,10}, {2,11},  {1,9},  {1,10}
};

const Position loop10[LED_MODE7_LOOP10_LEN]=
{
//    M     ,<     ALT_R     FN     CTR_R   Left    Down    Right
	{3,7}, {3,8},  {4,4},  {4,5},  {4,6},   {4,7},  {4,8},  {4,9}
};

const Position loop11[LED_MODE7_LOOP11_LEN]=
{
//    .>     /?      Shift_R   UP      PGDN
	{3,9}, {3,10},  {3,11},  {3,12},  {3,13}
};

const Position loop12[LED_MODE7_LOOP12_LEN]=
{
//    N    SPACE
	{3,6}, {4,3}
};

const Position loop13[LED_MODE7_LOOP13_LEN]=
{
//    H     B        V       C     ALT_L    WIN_L   CTRL_L
	{2,6}, {3,5},  {3,4},  {3,3},  {4,2},   {4,1},  {4,0}
};
//------------------------------------------------------------------------------------------
/* mode9矩阵 */
#define led_mode9_loop_len(n)		LED_MODE9_LOOP##n##_LEN
#define LED_MODE9_LOOP1_LEN           1
#define LED_MODE9_LOOP2_LEN           7
#define LED_MODE9_LOOP3_LEN           12
#define LED_MODE9_LOOP4_LEN           8
#define LED_MODE9_LOOP5_LEN           13
#define LED_MODE9_LOOP6_LEN           13
#define LED_MODE9_LOOP7_LEN           11

const Position led_mode9_loop1[LED_MODE9_LOOP1_LEN]=
{
//    H
	{2,6},
};

const Position led_mode9_loop2[LED_MODE9_LOOP2_LEN]=
{
//    N        B        G        T        Y        U        J
	{3,6},   {3,5},   {2,5},   {1,5},   {1,6},   {1,7},   {2,7},
};

const Position led_mode9_loop3[LED_MODE9_LOOP3_LEN]=
{
//  SPACE      V        F        R        4        5        6        7        8        I        K        M
	{4,3},   {3,4},   {2,4},   {1,4},   {0,4},   {0,5},   {0,6},   {0,7},   {0,8},   {1,8},   {2,8},   {3,7},
};

const Position led_mode9_loop4[LED_MODE9_LOOP4_LEN]=
{
//    C        D        E        3        9        O        L       ,<
	{3,3},   {2,3},   {1,3},   {0,3},   {0,9},   {1,9},   {2,9},   {3,8},
};

const Position led_mode9_loop5[LED_MODE9_LOOP5_LEN]=
{
//   X        S        W        2        0         -_        P        [{        ;:       '"        /?      Alt_R     FN
   {3,2},   {2,2},   {1,2},   {0,2},   {0,10},   {0,11},   {1,10},   {1,11},   {2,8},   {2,9},   {3,10},   {4,4},   {4,5},
};

const Position led_mode9_loop6[LED_MODE9_LOOP6_LEN]=
{
//  Alt_L     Z        A        Q        1        =+        BS        ]}        \|      ENTER    SHIFT_R   CTRL_R   LEFT
   {4,2},   {3,1},   {2,1},   {1,1},   {0,1},   {0,12},   {0,13},   {1,12},   {1,13},   {1,9},   {3,11},   {4,6},   {4,7},
};

const Position led_mode9_loop7[LED_MODE9_LOOP7_LEN]=
{
//  Win_L    Ctrl_L   Shift_L   CAPS     TAB      ESC       INS       DEL       PGUP     PGDN      RIGHT
	{4,1},   {4,0},   {3,0},   {2,0},   {1,0},   {0,0},    {0,14},   {1,14},   {1,10},   {3,13},   {4,9},
};

//------------------------------------------------------------------------------------------
uint16_t LED_Buffer_I[TRANSFER_Group][TRANSFER_SIZE];
uint16_t LED_Buffer_R[TRANSFER_Group][TRANSFER_SIZE];
uint16_t LED_Buffer_G[TRANSFER_Group][TRANSFER_SIZE];
uint16_t LED_Buffer_B[TRANSFER_Group][TRANSFER_SIZE];
//------------------------------------------------------------------------------------------
uint16_t led_ColorIndex[TRANSFER_Group][TRANSFER_SIZE];
uint16_t led_UserConfig[TRANSFER_Group][TRANSFER_SIZE];

uint16_t LED_R_TAG[TRANSFER_Group][TRANSFER_SIZE];
uint16_t LED_G_TAG[TRANSFER_Group][TRANSFER_SIZE];
uint16_t LED_B_TAG[TRANSFER_Group][TRANSFER_SIZE];
uint16_t LED_R_LUM[TRANSFER_Group][TRANSFER_SIZE];
uint16_t LED_G_LUM[TRANSFER_Group][TRANSFER_SIZE];
uint16_t LED_B_LUM[TRANSFER_Group][TRANSFER_SIZE];
//------------------------------------------------------------------------------------------
const Position led_position_to_coord[LED_ROW_MAX][LED_COL_MAX]={
//		0		 1       2       3       4       5       6       7       8       9       10       11       12       13       14       15       16       17       18       19       20
//R0	ESC		 1		 2		 3		 4		 5		 6		 7		 8		 9		 0		  -_	   =+	    Backsp	 Ins	  NC	   NC	    NC	     NC	      NC       NC
		{{4,0},	 {4,1},  {4,2},  {4,3},  {4,4},  {4,5},  {4,6},  {4,7},  {4,8},  {4,9},  {4,10},  {4,11},  {4,12},  {4,13},  {4,14},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R1	TAB		 Q		 W		 E		 R		 T		 Y		 U		 I		 O		 P		  [{	   ]}	    \|	      DEL	  NC       NC	    NC	     NC	      NC	   NC
		{{2,0},  {2,1},  {2,2},  {2,3},  {2,4},  {2,5},  {2,6},  {2,7},  {2,8},  {2,9},  {2,10},  {2,11},  {2,12},  {2,13},  {2,14},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R2	Caps	 A		 S		 D		 F		 G		 H		 J		 K		 L		 ;:	  	  '"	   ENTER	PgUp	 NC		  NC	   NC	    NC	     NC	      NC	   NC
		{{3,0},  {3,1},  {3,2},  {3,3},  {3,4},  {3,5},  {3,6},  {3,7},  {3,8},  {3,9},  {3,10},  {3,11},  {3,12},  {3,13},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R3	Shift	 Z		 X		 C		 V		 B		 N		 M		 ,<		 .>		 /?	  	  ShiftR   UP		PgDn	 NC		  NC	   NC	 	NC	     NC	      NC	   NC
		{{0,0},  {0,1},  {0,2},  {0,3},  {0,4},  {0,5},  {0,6},  {0,7},  {0,8},  {0,9},   {0,10}, {0,11},  {0,12},  {0,13},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R4	CtrlL	 Win_L	 Alt_L	 SPACE	 Alt_R	 FN		 CtrlR	 Left	 Down	 Right	 NC		  NC	   NC	    NC	     NC	      NC	   NC	    NC       NC	      NC	   NC
		{{1,0},  {1,1},  {1,2},  {1,3}, {1,4},   {1,5},  {1,6},  {1,7},  {1,8},  {1,9},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},
};
//这里面的{4,0}对应灯的位置
const Position LED_MDOE6_Position[5][21]=
{
//		0		 1       2       3       4       5       6       7       8       9       10       11       12       13       14       15       16       17       18       19       20
//R0	ESC		 1		 2		 3		 4		 5		 6		 7		 8		 9		 0		  -_	   =+	    Backsp	 Ins
		{{0,0},	 {0,1},  {0,2},  {0,3},  {0,4},  {0,5},  {0,6},  {0,7},  {0,8},  {0,9},  {0,10},  {0,11},  {0,12},  {0,13},  {0,14},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R1	TAB		 Q		 W		 E		 R		 T		 Y		 U		 I		 O		 P		  [{	   ]}	    \|	     DEL
		{{1,0},  {1,1},  {1,2},  {1,3},  {1,4},  {1,5},  {1,6},  {1,7},  {1,8},  {1,9},  {1,10},  {1,11},  {1,12},  {1,13},  {1,14},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R2	Caps	 A		 S		 D		 F		 G		 H		 J		 K		 L		 ;:	  	  '"	   ENTER	PgUp
		{{2,0},  {2,1},  {2,2},  {2,3},  {2,4},  {2,5},  {2,6},  {2,7},  {2,8},  {2,9},  {2,10},  {2,11},  {2,12},  {2,13},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R3	Shift	 Z		 X		 C		 V		 B		 N		 M		 ,<		 .>		 /?	  	  ShiftR   UP		PgDn
		{{3,0},  {3,1},  {3,2},  {3,3},  {3,4},  {3,5},  {3,6},  {3,7},  {3,8},  {3,9},  {3,10},  {3,11},  {3,12},  {3,13},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R4	CtrlL	 Win_L	 Alt_L	 SPACE	 Alt_R	 FN		 CtrlR	 Left	 Down	 Right
		{{4,0},  {4,1},  {4,2},  {4,5},  {4,8},  {4,9},  {4,10}, {4,11}, {4,12}, {4,13}, NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},
};
//这里面的{4,0}对应hall的SPI位置
const Position LED_Layout_Position[5][21]=
{
//		 0		  1       2       3       4        5        6       7       8       9       10       11       12       13       14       15       16       17       18       19       20
//R0	 ESC	  1		  2		  3		  4		   5		6		7		8		9		0		 -_	      =+	   Backsp	Ins
		{{4,0},	 {4,1},  {4,2},  {4,3},  {4,4},   {4,5},   {4,6},  {4,7},  {4,8},  {4,9},  {4,10},  {4,11},  {4,12},  {4,13},  {4,14},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R1	 TAB	  Q		  W		  E		  R		   T	    Y		U		I		O		P		 [{	      ]}	   \|	    DEL
		{{2,0},  {2,1},  {2,2},  {2,3},  {2,4},   {2,5},   {2,6},  {2,7},  {2,8},  {2,9},  {2,10},  {2,11},  {2,12},  {2,13},  {2,14},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R2	 Caps	  A		  S		  D		  F		   G		H		J		K		L		;:	  	 '"	      ENTER	   PgUp
		{{3,0},  {3,1},  {3,2},  {3,3},  {3,4},   {3,5},   {3,6},  {3,7},  {3,8},  {3,9},  {3,10},  {3,11},  {3,12},  {3,13},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R3	 Shift	  Z		  X		  C		  V		   B		N		M		,<		.>		/?	  	 ShiftR   UP	   PgDn
		{{0,0},  {0,1},  {0,2},  {0,3},  {0,4},   {0,5},   {0,6},  {0,7},  {0,8},  {0,9},  {0,10},  {0,11},  {0,12},  {0,13},  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},

//R4	 CtrlL	  Win_L	  Alt_L	                   SPACE                    Alt_R	FN		CtrlR	Left	Down	 Right
		{{1,0},  {1,1},  {1,2},  NULL_F, NULL_F,  {1,3},   NULL_F, NULL_F, {1,4},  {1,5},  {1,6},   {1,7},  {1,8},   {1,9},    NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,  NULL_F,},
};

const Position HorseTable[68] =
{
//  ESC		 1		  2		   3		 4		 5		  6		   7		8		 9		   0		 -_	      =+	   Backsp	 Ins
	{0,0},	 {0,1},   {0,2},   {0,3},    {0,4},  {0,5},   {0,6},   {0,7},   {0,8},   {0,9},    {0,10},   {0,11},  {0,12},  {0,13},   {0,14},	//15
//	DEL		 PgUp	  PgDn	   Right
	{1,14},	 {2,13},  {3,13},  {4,9},																											//4
//  Down	 Left	  CtrlR	   FN		Alt_R	 SPACE	  Alt_L	   Win_L	CtrlL
	{4,8},	 {4,7},	  {4,6},   {4,5},	{4,4},	 {4,3},	  {4,2},   {4,1},	{4,0},																//9
//  Shift	 Caps	  TAB
	{3,0},	 {2,0},	  {1,0},																													//3
//	Q		 W		  E		   R	    T		 Y		  U		   I		O		 P		   [{	   	]}	     \|
	{1,1},  {1,2},    {1,3},   {1,4},   {1,5},   {1,6},   {1,7},   {1,8},   {1,9},   {1,10},   {1,11},  {1,12},  {1,13},						//13
//	ENTER	 UP
	{2,12},	 {3,12},																															//2
//	ShiftR	 /?		  .>	   ,<		M		 N		  B		   V		C	 	 X		   Z
	{3,11},	 {3,10},  {3,9},   {3,8},   {3,7},	 {3,6},	  {3,5},   {3,4},	{3,3},	 {3,2},	   {3,1},											//11
//	A		 S		  D		   F		G		 H		  J		   K		L		 ;:	  	   '"
	{2,1},   {2,2},   {2,3},   {2,4},   {2,5},   {2,6},   {2,7},   {2,8},   {2,9},   {2,10},   {2,11},											//11
};

const uint8_t Gamma_steps[color_max] =
{
	0,	1,	2,	3,	4,	5,	6,	7,	8,
	10,	12,	14,	16,	18,	20,	22,	24,	26,
	29,	32,	35,	38,	41,	44,	47,	50,	53,
	57,	61,	65,	69,	73,	77,	81,	85,	89,
	94,	99,	104,109,114,119,124,129,134,
	140,146,152,158,164,170,176,182,188,
	195,202,209,216,223,230,237,244,251,255
};

const uint8_t LightingColorTable[Lighting_ColorMAX][3]=
{
	{ 0xFF , 0xFF , 0xFF },	//0 white
	{ 0xFF , 0x00 , 0x00 },	//1 Red
	{ 0x00 , 0xFF , 0x00 },	//2 Green
	{ 0x00 , 0x00 , 0xFF },	//3 Blue
	{ 0xFF , 0xFF , 0x00 },	//4 黄
	{ 0xFF , 0x00 , 0xFF },	//5 粉
	{ 0x00 , 0xFF , 0xFF },	//6 青
};

#if 0
const uint16_t HALLVolumeTable[100]=
{
		//1														10%
		25500,12750, 8500, 6375, 5100, 4250, 3642, 3187, 2833, 2550,
		//11%													20%
		2318, 2125,  1961, 1821, 1700, 1593, 1500, 1416, 1342, 1275,
		//21%													30%
		1214, 1159,  1108, 1062, 1020, 980,  944,  910,  879,  850,
		//31%													40%
		822,  796,   772,  750,  728,  708,  689,  671,  653,  637,
		//41%													50%
		500,  490,   481,  472,  463,  455,  447,  439,  432,  510,
		//51%													60%
		418,  411,   404,  398,  392,  386,  380,  375,  369,  425,
		//61%													70%
		359,  354,   349,  344,  340,  335,  331,  326,  322,  364,
		//71%													80%
		314,  310,   307,  303,  300,  296,  293,  289,  286,  318,
		//81%													90%
		314,  310,   307,  303,  300,  296,  293,  289,  286,  283,
		//91%													100%
		280,  277,   274,  271,  268,  265,  262,  260,  257,  255,
};
#endif

const uint16_t LedVolumeTable[VOLUME_MAX]=
{
//		10%		20%		30%		40%		50%		60%		70%		80%		90%		100%
		2550,	1275, 	850, 	637, 	510, 	425, 	364, 	318, 	283,	255,
};

const uint8_t Specturn_colors[ SPECTURN_COLOR_NUM ][3] =
{
	{ 0xFF , 0x00 , 0x00  },
	{ 0xFF , 0x06 , 0x00  },
	{ 0xFF , 0x12 , 0x00  },
	{ 0xFF , 0x29 , 0x00  },
	{ 0xFF , 0x49 , 0x00  },
	{ 0xFF , 0x72 , 0x00  },
	{ 0xFF , 0xA4 , 0x00  },
	{ 0xFF , 0xDF , 0x00  },
	{ 0xFF , 0xFF , 0x00  },

	{ 0xFF , 0xFF , 0x00  },
	{ 0xDF , 0xFF , 0x00  },
	{ 0xA4 , 0xFF , 0x00  },
	{ 0x72 , 0xFF , 0x00  },
	{ 0x49 , 0xFF , 0x00  },
	{ 0x29 , 0xFF , 0x00  },
	{ 0x12 , 0xFF , 0x00  },
	{ 0x06 , 0xFF , 0x00  },
	{ 0x00 , 0xFF , 0x00  },

	{ 0x00 , 0xFF , 0x00  },
	{ 0x00 , 0xFF , 0x06  },
	{ 0x00 , 0xFF , 0x12  },
	{ 0x00 , 0xFF , 0x29  },
	{ 0x00 , 0xFF , 0x49  },
	{ 0x00 , 0xFF , 0x72  },
	{ 0x00 , 0xFF , 0xA4  },
	{ 0x00 , 0xFF , 0xDF  },
	{ 0x00 , 0xFF , 0xFF  },

	{ 0x00 , 0xFF , 0xFF  },
	{ 0x00 , 0xDF , 0xFF  },
	{ 0x00 , 0xA4 , 0xFF  },
	{ 0x00 , 0x72 , 0xFF  },
	{ 0x00 , 0x49 , 0xFF  },
	{ 0x00 , 0x29 , 0xFF  },
	{ 0x00 , 0x12 , 0xFF  },
	{ 0x00 , 0x06 , 0xFF  },
	{ 0x00 , 0x00 , 0xFF  },

	{ 0x00 , 0x00 , 0xFF  },
	{ 0x06 , 0x00 , 0xFF  },
	{ 0x12 , 0x00 , 0xFF  },
	{ 0x29 , 0x00 , 0xFF  },
	{ 0x49 , 0x00 , 0xFF  },
	{ 0x72 , 0x00 , 0xFF  },
	{ 0xA4 , 0x00 , 0xFF  },
	{ 0xDF , 0x00 , 0xFF  },
	{ 0xFF , 0x00 , 0xFF  },

	{ 0xFF , 0x00 , 0xFF  },
	{ 0xFF , 0x00 , 0xDF  },
	{ 0xFF , 0x00 , 0xA4  },
	{ 0xFF , 0x00 , 0x72  },
	{ 0xFF , 0x00 , 0x49  },
	{ 0xFF , 0x00 , 0x29  },
	{ 0xFF , 0x00 , 0x12  },
	{ 0xFF , 0x00 , 0x06  },
	{ 0xFF , 0x00 , 0x00  }
};
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : Hall_osc_calibrate
 * Description   : Config hall sensor OSC.
 *
 * END ****************************************************************************************************************/
void LED_Variable_Init(void)
{
	uint8_t i;
	uint8_t j;

	switch(g_variableData.LED_MODE)
	{
		case LED_1_mode:
			LED_1_mode_RGBdataInit();
			break;
		case LED_2_mode:
			LED_2_mode_RGBdataInit();
			break;
		case LED_3_mode:
			LED_3_mode_RGBdataInit();
			break;
		case LED_4_mode:
			LED_4_mode_RGBdataInit();
			break;
		case LED_5_mode:
			LED_5_mode_RGBdataInit();
			break;
		case LED_6_mode:
			LED_6_mode_RGBdataInit();
			break;
		case LED_7_mode:
			LED_7_mode_RGBdataInit();
			break;
		case LED_8_mode:
			LED_8_mode_RGBdataInit();
			break;
		case LED_9_mode:
			LED_9_mode_RGBdataInit();
			break;
		case LED_10_mode:
			LED_10_mode_RGBdataInit();
			break;
		case LED_20_mode:
			LED_20_mode_RGBdataInit();
			break;
		default:
			break;
	}
	Flag_LedMode_ModeInit = 1;
	//------------------------------------------------------------------------------------------
	//Flag_LedMode_RGBUpdate |= LED_Update_IRGB;
	//------------------------------------------------------------------------------------------
	Flag_LedMode_ColorChange = 0;
	Tick_10ms_LEDUpdatePWM = 0xFF;
	Flag_RGB_index = 0;
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : Hall_osc_calibrate
 * Description   : Config hall sensor OSC.
 *
 * END ****************************************************************************************************************/
void Hall_Update_PWM(void)
{
	uint8_t j;
	static uint8_t RGB_Cycle = 0;
    //------------------------------------------------------------------------------------------
	/* write LED_MaxCur register. */
	if(RGB_Cycle<4)
	{
		switch(RGB_Cycle)
		{
			case 0:
				for(j= 0; j < TRANSFER_SIZE; j++)
				{
					DMA_SPI_TX_1_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_1_Buffer[j] |= ((Hall_Write_PWM_LED2)|(LED_Buffer_B[SPI0_GROUP][j]));
					//usb_echo("DMA_SPI_TX_1_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_1_Buffer[j]);

					DMA_SPI_TX_2_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_2_Buffer[j] |= ((Hall_Write_PWM_LED2)|(LED_Buffer_B[SPI1_GROUP][j]));
					//usb_echo("DMA_SPI_TX_2_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_2_Buffer[j]);

					DMA_SPI_TX_3_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_3_Buffer[j] |= ((Hall_Write_PWM_LED2)|(LED_Buffer_B[SPI2_GROUP][j]));
					//usb_echo("DMA_SPI_TX_3_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_3_Buffer[j]);

					DMA_SPI_TX_4_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_4_Buffer[j] |= ((Hall_Write_PWM_LED2)|(LED_Buffer_B[SPI3_GROUP][j]));
					//usb_echo("DMA_SPI_TX_4_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_4_Buffer[j]);

					DMA_SPI_TX_5_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_5_Buffer[j] |= ((Hall_Write_PWM_LED2)|(LED_Buffer_B[SPI4_GROUP][j]));
					//usb_echo("DMA_SPI_TX_5_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_5_Buffer[j]);

					DMA_SPI_TX_6_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_6_Buffer[j] |= ((Hall_Write_PWM_LED2)|(LED_Buffer_B[SPI6_GROUP][j]));
					//usb_echo("DMA_SPI_TX_6_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_6_Buffer[j]);
				}
				break;
			case 1:
				for(j= 0; j < TRANSFER_SIZE; j++)
				{
					DMA_SPI_TX_1_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_1_Buffer[j] |= ((Hall_Write_PWM_LED1)|(LED_Buffer_G[SPI0_GROUP][j]));
					//usb_echo("DMA_SPI_TX_1_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_1_Buffer[j]);

					DMA_SPI_TX_2_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_2_Buffer[j] |= ((Hall_Write_PWM_LED1)|(LED_Buffer_G[SPI1_GROUP][j]));
					//usb_echo("DMA_SPI_TX_2_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_2_Buffer[j]);

					DMA_SPI_TX_3_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_3_Buffer[j] |= ((Hall_Write_PWM_LED1)|(LED_Buffer_G[SPI2_GROUP][j]));
					//usb_echo("DMA_SPI_TX_3_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_3_Buffer[j]);

					DMA_SPI_TX_4_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_4_Buffer[j] |= ((Hall_Write_PWM_LED1)|(LED_Buffer_G[SPI3_GROUP][j]));
					//usb_echo("DMA_SPI_TX_4_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_4_Buffer[j]);

					DMA_SPI_TX_5_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_5_Buffer[j] |= ((Hall_Write_PWM_LED1)|(LED_Buffer_G[SPI4_GROUP][j]));

					DMA_SPI_TX_6_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_6_Buffer[j] |= ((Hall_Write_PWM_LED1)|(LED_Buffer_G[SPI6_GROUP][j]));
					//usb_echo("DMA_SPI_TX_5_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_5_Buffer[j]);
				}
				break;
			case 2:
				for(j= 0; j < TRANSFER_SIZE; j++)
				{
					DMA_SPI_TX_1_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_1_Buffer[j] |= ((Hall_Write_PWM_LED0)|(LED_Buffer_R[SPI0_GROUP][j]));
					//usb_echo("DMA_SPI_TX_1_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_1_Buffer[j]);

					DMA_SPI_TX_2_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_2_Buffer[j] |= ((Hall_Write_PWM_LED0)|(LED_Buffer_R[SPI1_GROUP][j]));
					//usb_echo("DMA_SPI_TX_2_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_2_Buffer[j]);

					DMA_SPI_TX_3_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_3_Buffer[j] |= ((Hall_Write_PWM_LED0)|(LED_Buffer_R[SPI2_GROUP][j]));
					//usb_echo("DMA_SPI_TX_3_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_3_Buffer[j]);

					DMA_SPI_TX_4_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_4_Buffer[j] |= ((Hall_Write_PWM_LED0)|(LED_Buffer_R[SPI3_GROUP][j]));
					//usb_echo("DMA_SPI_TX_4_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_4_Buffer[j]);

					DMA_SPI_TX_5_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_5_Buffer[j] |= ((Hall_Write_PWM_LED0)|(LED_Buffer_R[SPI4_GROUP][j]));

					DMA_SPI_TX_6_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_6_Buffer[j] |= ((Hall_Write_PWM_LED0)|(LED_Buffer_R[SPI6_GROUP][j]));
					//usb_echo("DMA_SPI_TX_5_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_5_Buffer[j]);
				}
				break;
			case 3:
				for(j= 0; j < TRANSFER_SIZE; j++)
				{
					DMA_SPI_TX_1_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_1_Buffer[j] |= ((Hall_Write_PWM_Period)|(LED_Buffer_I[SPI0_GROUP][j]));
					//usb_echo("DMA_SPI_TX_1_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_1_Buffer[j]);

					DMA_SPI_TX_2_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_2_Buffer[j] |= ((Hall_Write_PWM_Period)|(LED_Buffer_I[SPI1_GROUP][j]));
					//usb_echo("DMA_SPI_TX_2_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_2_Buffer[j]);

					DMA_SPI_TX_3_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_3_Buffer[j] |= ((Hall_Write_PWM_Period)|(LED_Buffer_I[SPI2_GROUP][j]));
					//usb_echo("DMA_SPI_TX_3_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_3_Buffer[j]);

					DMA_SPI_TX_4_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_4_Buffer[j] |= ((Hall_Write_PWM_Period)|(LED_Buffer_I[SPI3_GROUP][j]));
					//usb_echo("DMA_SPI_TX_4_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_4_Buffer[j]);

					DMA_SPI_TX_5_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_5_Buffer[j] |= ((Hall_Write_PWM_Period)|(LED_Buffer_I[SPI4_GROUP][j]));

					DMA_SPI_TX_6_Buffer[j] &= 0xFFFF0000;
					DMA_SPI_TX_6_Buffer[j] |= ((Hall_Write_PWM_Period)|(LED_Buffer_I[SPI6_GROUP][j]));
					//usb_echo("DMA_SPI_TX_5_Buffer[%d] = 0x%x\n",j,DMA_SPI_TX_5_Buffer[j]);
				}
				break;
			default:
				break;
		}
		//------------------------------------------------------------------------------------------
		#if 0
			for(uint8_t i=0;i<TRANSFER_SIZE;i++)
			{
				usb_echo("DMA_SPI_TX_1_Buffer[%d]:%x\r\n",i,DMA_SPI_TX_1_Buffer[i]);
				usb_echo("DMA_SPI_TX_2_Buffer[%d]:%x\r\n",i,DMA_SPI_TX_2_Buffer[i]);
				usb_echo("DMA_SPI_TX_3_Buffer[%d]:%x\r\n",i,DMA_SPI_TX_3_Buffer[i]);
				usb_echo("DMA_SPI_TX_4_Buffer[%d]:%x\r\n",i,DMA_SPI_TX_4_Buffer[i]);
				usb_echo("DMA_SPI_TX_5_Buffer[%d]:%x\r\n",i,DMA_SPI_TX_5_Buffer[i]);
			}
			usb_echo("\r\n");
		#endif
		#if 0
			for(uint8_t i=0;i<SPI1_TRANSFER_SIZE;i++)
			{
				usb_echo("LED_Buffer_I1[%d]:%x\r\n",i,LED_Buffer_I[1][i]);
				usb_echo("LED_Buffer_R1[%d]:%x\r\n",i,LED_Buffer_R[1][i]);
				usb_echo("LED_Buffer_G1[%d]:%x\r\n",i,LED_Buffer_G[1][i]);
				usb_echo("LED_Buffer_B1[%d]:%x\r\n",i,LED_Buffer_B[1][i]);
			}
			usb_echo("\r\n");
		#endif
		#if 0
			for(uint8_t i=0;i<SPI3_TRANSFER_SIZE;i++)
			{
				usb_echo("LED_Buffer_I2[%d]:%x\r\n",i,LED_Buffer_I[2][i]);
				usb_echo("LED_Buffer_R2[%d]:%x\r\n",i,LED_Buffer_R[2][i]);
				usb_echo("LED_Buffer_G2[%d]:%x\r\n",i,LED_Buffer_G[2][i]);
				usb_echo("LED_Buffer_B2[%d]:%x\r\n",i,LED_Buffer_B[2][i]);
			}
			usb_echo("\r\n");
		#endif
		#if 0
			for(uint8_t i=0;i<SPI4_TRANSFER_SIZE;i++)
			{
				usb_echo("LED_Buffer_I3[%d]:%x\r\n",i,LED_Buffer_I[3][i]);
				usb_echo("LED_Buffer_R3[%d]:%x\r\n",i,LED_Buffer_R[3][i]);
				usb_echo("LED_Buffer_G3[%d]:%x\r\n",i,LED_Buffer_G[3][i]);
				usb_echo("LED_Buffer_B3[%d]:%x\r\n",i,LED_Buffer_B[3][i]);
			}
			usb_echo("\r\n");
		#endif
		#if 0
			for(uint8_t i=0;i<SPI7_TRANSFER_SIZE;i++)
			{
				usb_echo("LED_Buffer_I4[%d]:%x\r\n",i,LED_Buffer_I[4][i]);
				usb_echo("LED_Buffer_R4[%d]:%x\r\n",i,LED_Buffer_R[4][i]);
				usb_echo("LED_Buffer_G4[%d]:%x\r\n",i,LED_Buffer_G[4][i]);
				usb_echo("LED_Buffer_B4[%d]:%x\r\n",i,LED_Buffer_B[4][i]);
			}
			usb_echo("\r\n");
		#endif
		//------------------------------------------------------------------------------------------

		RGB_Cycle++;
		if(RGB_Cycle>=4)
		{
			RGB_Cycle = 0;
			Tick_10ms_LEDUpdatePWM = 0;
			Tick_SETGain_Need_Wait = 0;
			//------------------------------------------------------------------------------------------
			Flag_LedMode_RGBUpdate &= (~LED_Update_IRGB);
			//Flag_LedMode_RGBUpdate |= (LED_Update_IRGB);
			//------------------------------------------------------------------------------------------
		}
	}
	else
	{
		RGB_Cycle = 0;
		Tick_10ms_LEDUpdatePWM = 0;
		Tick_SETGain_Need_Wait = 0;
		Flag_LedMode_RGBUpdate &= (~LED_Update_IRGB);
		//Flag_LedMode_RGBUpdate |= (LED_Update_IRGB);
	}
	Hall_write_PWM();
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : Hall_PWMOff_Suspend
 * Description   : 睡眠的时候把RGB和亮度写0，关灯。
 *
 * END ****************************************************************************************************************/
void Hall_PWMOff_Suspend(void)
{
	uint8_t j;
    //------------------------------------------------------------------------------------------
	/* write LED_MaxCur register. */
	for(j= 0; j < TRANSFER_SIZE; j++)
	{
		DMA_SPI_TX_1_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_1_Buffer[j] |= ((Hall_Write_PWM_LED2)|(0));

		DMA_SPI_TX_2_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_2_Buffer[j] |= ((Hall_Write_PWM_LED2)|(0));

		DMA_SPI_TX_3_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_3_Buffer[j] |= ((Hall_Write_PWM_LED2)|(0));

		DMA_SPI_TX_4_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_4_Buffer[j] |= ((Hall_Write_PWM_LED2)|(0));

		DMA_SPI_TX_5_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_5_Buffer[j] |= ((Hall_Write_PWM_LED2)|(0));
	}
	Hall_write_PWM();
	for(j= 0; j < TRANSFER_SIZE; j++)
	{
		DMA_SPI_TX_1_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_1_Buffer[j] |= ((Hall_Write_PWM_LED1)|(0));

		DMA_SPI_TX_2_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_2_Buffer[j] |= ((Hall_Write_PWM_LED1)|(0));

		DMA_SPI_TX_3_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_3_Buffer[j] |= ((Hall_Write_PWM_LED1)|(0));

		DMA_SPI_TX_4_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_4_Buffer[j] |= ((Hall_Write_PWM_LED1)|(0));

		DMA_SPI_TX_5_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_5_Buffer[j] |= ((Hall_Write_PWM_LED1)|(0));
	}
	Hall_write_PWM();
	for(j= 0; j < TRANSFER_SIZE; j++)
	{
		DMA_SPI_TX_1_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_1_Buffer[j] |= ((Hall_Write_PWM_LED0)|(0));

		DMA_SPI_TX_2_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_2_Buffer[j] |= ((Hall_Write_PWM_LED0)|(0));

		DMA_SPI_TX_3_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_3_Buffer[j] |= ((Hall_Write_PWM_LED0)|(0));

		DMA_SPI_TX_4_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_4_Buffer[j] |= ((Hall_Write_PWM_LED0)|(0));

		DMA_SPI_TX_5_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_5_Buffer[j] |= ((Hall_Write_PWM_LED0)|(0));
	}
	Hall_write_PWM();
	for(j= 0; j < TRANSFER_SIZE; j++)
	{
		DMA_SPI_TX_1_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_1_Buffer[j] |= ((Hall_Write_PWM_Period)|(0));

		DMA_SPI_TX_2_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_2_Buffer[j] |= ((Hall_Write_PWM_Period)|(0));

		DMA_SPI_TX_3_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_3_Buffer[j] |= ((Hall_Write_PWM_Period)|(0));

		DMA_SPI_TX_4_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_4_Buffer[j] |= ((Hall_Write_PWM_Period)|(0));

		DMA_SPI_TX_5_Buffer[j] &= 0xFFFF0000;
		DMA_SPI_TX_5_Buffer[j] |= ((Hall_Write_PWM_Period)|(0));
	}
	Hall_write_PWM();
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LedVolume_Inc
 * Description   :
 *
 * END ****************************************************************************************************************/
void LedVolume_Inc(void)
{
    //------------------------------------------------------------------------------------------
	/* write LED_MaxCur register. */
	if(g_variableData.LED_BRIGHTNESS < 9)
	{
		g_variableData.LED_BRIGHTNESS++;
	}
	//------------------------------------------------------------------------------------------
	Flag_LedMode_ModeInit = 1;
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LedVolume_Dec
 * Description   :
 *
 * END ****************************************************************************************************************/
void LedVolume_Dec(void)
{
    //------------------------------------------------------------------------------------------
	/* write LED_MaxCur register. */
	if(g_variableData.LED_BRIGHTNESS > 0)
	{
		g_variableData.LED_BRIGHTNESS--;
	}
	//------------------------------------------------------------------------------------------
	Flag_LedMode_ModeInit = 1;
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LedSpeed_Dec
 * Description   :
 *
 * END ****************************************************************************************************************/
void LedSpeed_Dec(void)
{
    //------------------------------------------------------------------------------------------
	/* write LED_MaxCur register. */
	if(g_variableData.LED_SPEED > 0)
	{
		g_variableData.LED_SPEED--;
	}
	//------------------------------------------------------------------------------------------
	Flag_LedMode_ModeInit = 1;
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LedSpeed_Inc
 * Description   :
 *
 * END ****************************************************************************************************************/
void LedSpeed_Inc(void)
{
    //------------------------------------------------------------------------------------------
	if(g_variableData.LED_SPEED < 3)
	{
		g_variableData.LED_SPEED++;
	}
	//------------------------------------------------------------------------------------------
	Flag_LedMode_ModeInit = 1;
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LedColorIndex_Dec
 * Description   :
 *
 * END ****************************************************************************************************************/
void LedColorIndex_Inc(void)
{
    //------------------------------------------------------------------------------------------
	/* write LED_MaxCur register. */
	if(g_variableData.LedColor_Index < Lighting_ColorMAX-1)
	{
		g_variableData.LedColor_Index++;
	}
	else
	{
		g_variableData.LedColor_Index = 0;
	}
	//------------------------------------------------------------------------------------------
	Flag_LedMode_ModeInit = 1;
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LedMode_Change
 * Description   :
 *
 * END ****************************************************************************************************************/
void LedMode_Change(void)
{
    //------------------------------------------------------------------------------------------
	/* write LED_MaxCur register. */
	if(!Flag_FN_SPACE_Status)
	{
		if(g_variableData.LED_MODE < LED_20_mode)
		{
			g_variableData.LED_MODE++;
		}
		else
		{
			g_variableData.LED_MODE = LED_1_mode;
		}
		//------------------------------------------------------------------------------------------
		Flag_LedMode_ModeInit = 1;
		//------------------------------------------------------------------------------------------
	}
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LedMode_Forward_Change
 * Description   : 灯光向前
 *
 * END ****************************************************************************************************************/
void LedMode_Forward_Change(void)
{
    //------------------------------------------------------------------------------------------
	/* write LED_MaxCur register. */
	if(!Flag_FN_SPACE_Status)
	{
		if(g_variableData.LED_MODE < LED_20_mode)
		{
			g_variableData.LED_MODE++;
		}
		//------------------------------------------------------------------------------------------
		Flag_LedMode_ModeInit = 1;
		//------------------------------------------------------------------------------------------
	}
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LedMode_Backward_Change
 * Description   : 灯光向后
 *
 * END ****************************************************************************************************************/
void LedMode_Backward_Change(void)
{
    //------------------------------------------------------------------------------------------
	/* write LED_MaxCur register. */
	if(!Flag_FN_SPACE_Status)
	{
		if(g_variableData.LED_MODE > 0)
		{
			g_variableData.LED_MODE--;
		}
		//------------------------------------------------------------------------------------------
		Flag_LedMode_ModeInit = 1;
		//------------------------------------------------------------------------------------------
	}
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : Ledoff
 * Description   : 关灯，只写RGB=0
 *
 * END ****************************************************************************************************************/
void Ledoff(void)
{
	Hall_PWMOff_Suspend();
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LEDMode_RGBdataProcess
 * Description   :
 *
 * END ****************************************************************************************************************/
void LEDMode_RGBdataProcess(void)
{
	//------------------------------------------------------------------------------------------
	/* hall写完RGB后，再做下一次RGB数据 */
	if(!(Flag_LedMode_RGBUpdate & 0x07))
	{
		if(Flag_LedMode_ModeInit)
		{
			if(Flag_FN_SPACE_Status)
			{
				LED_FN_SPACE_mode_RGBdataInit();
				//------------------------------------------------------------------------------------------
				Flag_LedMode_RGBUpdate |= (LED_Update_SET_Highlight);
				//------------------------------------------------------------------------------------------
			}
			else
			{
				switch(g_variableData.LED_MODE)
				{
					case LED_1_mode:
						LED_1_mode_RGBdataInit();
						break;
					case LED_2_mode:
						LED_2_mode_RGBdataInit();
						break;
					case LED_3_mode:
						LED_3_mode_RGBdataInit();
						break;
					case LED_4_mode:
						LED_4_mode_RGBdataInit();
						break;
					case LED_5_mode:
						LED_5_mode_RGBdataInit();
						break;
					case LED_6_mode:
						LED_6_mode_RGBdataInit();
						break;
					case LED_7_mode:
						LED_7_mode_RGBdataInit();
						break;
					case LED_8_mode:
						LED_8_mode_RGBdataInit();
						break;
					case LED_9_mode:
						LED_9_mode_RGBdataInit();
						break;
					case LED_10_mode:
						LED_10_mode_RGBdataInit();
						break;
					case LED_20_mode:
						LED_20_mode_RGBdataInit();
						break;
					default:
						break;
				}
			}
			Flag_LedMode_ModeInit = 0;
			//------------------------------------------------------------------------------------------
			Flag_LedMode_RGBUpdate |= LED_Update_IRGB;
			//------------------------------------------------------------------------------------------
			Flag_LedMode_ColorChange = 0;
			Tick_RGBSpeed_cnt = 0;
			Flag_RGB_index = 0;
		}
		else
		{
			/* 每一次更新完颜色后，等颜色写到hall寄存器后再进行下一次更新 */
			if(!(Flag_LedMode_RGBUpdate & 0x07))
			{
				if(!Flag_FN_SPACE_Status)
				{
					if(g_variableData.LED_ONOFF)
					{
						if(++Tick_RGBSpeed_cnt > RGB_SpeedLevel_cnt)
						{
							//----------------------------------------------------------------------------------------------
							switch(g_variableData.LED_MODE)
							{
								case LED_1_mode:
									LED_1_mode_RGBdataProcess(Flag_RGB_index);
									break;
								case LED_2_mode:
									LED_2_mode_RGBdataProcess(Flag_RGB_index);
									break;
								case LED_3_mode:
									LED_3_mode_RGBdataProcess(Flag_RGB_index);
									break;
								case LED_4_mode:
									LED_4_mode_RGBdataProcess(Flag_RGB_index);
									break;
								case LED_5_mode:
									LED_5_mode_RGBdataProcess(Flag_RGB_index);
									break;
								case LED_6_mode:
									LED_6_mode_RGBdataProcess(Flag_RGB_index);
									break;
								case LED_7_mode:
									LED_7_mode_RGBdataProcess(Flag_RGB_index);
									break;
								case LED_8_mode:
									LED_8_mode_RGBdataProcess(Flag_RGB_index);
									break;
								case LED_9_mode:
									LED_9_mode_RGBdataProcess(Flag_RGB_index);
									break;
								case LED_10_mode:
									LED_10_mode_RGBdataProcess(Flag_RGB_index);
									break;
								case LED_20_mode:
									LED_20_mode_RGBdataProcess(Flag_RGB_index);
									break;
								default:
									break;
							}
							Flag_RGB_index++;
							if(Flag_RGB_index > TRANSFER_SIZE-1)
							{
								Flag_RGB_index = 0;
								/* 更新完数据后，等hall写到RGB寄存器后再更新下次数据 */
								//------------------------------------------------------------------------------------------
								if(g_variableData.LED_MODE != LED_2_mode)
								{
									Flag_LedMode_RGBUpdate |= LED_Update_IRGB;
								}
								//------------------------------------------------------------------------------------------
								Flag_LedMode_ColorChange = 0;
								//----------------------------------------------------------------------------------------------
								/* 改一次全部颜色要花的时间:125us*RGB_SpeedLevel_cnt+125*21 */
								Tick_RGBSpeed_cnt = 0;
								//----------------------------------------------------------------------------------------------
							}
							//----------------------------------------------------------------------------------------------
						}
					}
					else
					{
						Flag_RGB_index = 0;
						Tick_RGBSpeed_cnt = 0;
					}
				}
			}
		}
	}
	//------------------------------------------------------------------------------------------
}

void led_mode_rgb_set(uint8_t row, uint8_t col, uint8_t n,uint8_t RGB_index)
{
	if((led_position_to_coord[row][col].row != 0xff) && (led_position_to_coord[row][col].col != 0xff))
	{
		LED_Buffer_R[ led_position_to_coord[row][col].row ][ led_position_to_coord[row][col].col ] = Specturn_colors[ led_ColorIndex[0][n - 1] ][0];
		LED_Buffer_G[ led_position_to_coord[row][col].row ][ led_position_to_coord[row][col].col ] = Specturn_colors[ led_ColorIndex[0][n - 1] ][1];
		LED_Buffer_B[ led_position_to_coord[row][col].row ][ led_position_to_coord[row][col].col ] = Specturn_colors[ led_ColorIndex[0][n - 1] ][2];
	}
}
void led_mode_10_rgb_set(uint8_t row, uint8_t col)
{
	if((led_position_to_coord[row][col].row != 0xff) && (led_position_to_coord[row][col].col != 0xff))
	{
		LED_Buffer_R[ led_position_to_coord[row][col].row ][ led_position_to_coord[row][col].col ] = LED_R_TAG[1][0];
		LED_Buffer_G[ led_position_to_coord[row][col].row ][ led_position_to_coord[row][col].col ] = LED_G_TAG[1][0];
		LED_Buffer_B[ led_position_to_coord[row][col].row ][ led_position_to_coord[row][col].col ] = LED_B_TAG[1][0];
	}
}

void led_mode_10_rgb_clear(uint8_t row, uint8_t col)
{
	if((led_position_to_coord[row][col].row != 0xff) && (led_position_to_coord[row][col].col != 0xff))
	{
		LED_Buffer_R[ led_position_to_coord[row][col].row ][ led_position_to_coord[row][col].col ] = 0x00;
		LED_Buffer_G[ led_position_to_coord[row][col].row ][ led_position_to_coord[row][col].col ] = 0x00;
		LED_Buffer_B[ led_position_to_coord[row][col].row ][ led_position_to_coord[row][col].col ] = 0x00;
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LED_RGBbufClear
 * Description   :
 *
 * END ****************************************************************************************************************/
void LED_RGB_buf_clear(void)
{
	uint8_t i, j;
	for(i=0;i<TRANSFER_Group;i++)
	{
		for(j=0;j<TRANSFER_SIZE;j++)
		{
			LED_Buffer_R[i][j] = 0x00;
			LED_Buffer_G[i][j] = 0x00;
			LED_Buffer_B[i][j] = 0x00;
		}
	}
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LED_1_mode_RGBdataInit
 * Description   : 光谱循环
 *
 * END ****************************************************************************************************************/
void LED_1_mode_RGBdataInit(void)
{
	uint8_t i, j;
	/* 颜色初始化 */
	//--------------------------------------------------------------------------------------------------
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			//------------------------------------------------------------------------------------------
			/* 亮度初始化 */
			LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
			//------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
			{
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
			}
			else
			{
				/* 光谱循环:不支持颜色设置，颜色初始化 */
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
			}
		}
	}
	//------------------------------------------------------------------------------------------
	if(g_variableData.LED_ONOFF)
	{
		//------------------------------------------------------------------------------------------
		LED_Buffer_R[SPI0_GROUP][0] = 0xFF;
		//------------------------------------------------------------------------------------------
		LED_Buffer_G[SPI0_GROUP][0] = 0x00;
		//------------------------------------------------------------------------------------------
		LED_Buffer_B[SPI0_GROUP][0] = 0x00;
		//------------------------------------------------------------------------------------------
	}
	else
	{
		//------------------------------------------------------------------------------------------
		LED_Buffer_R[SPI0_GROUP][0] = 0x00;
		//------------------------------------------------------------------------------------------
		LED_Buffer_G[SPI0_GROUP][0] = 0x00;
		//------------------------------------------------------------------------------------------
		LED_Buffer_B[SPI0_GROUP][0] = 0x00;
		//------------------------------------------------------------------------------------------
	}
	//------------------------------------------------------------------------------------------
	switch(g_variableData.LED_SPEED)
	{
		case 0:
			RGB_SpeedLevel_cnt = 500;
			break;
		case 1:
			RGB_SpeedLevel_cnt = 250;
			break;
		case 2:
			RGB_SpeedLevel_cnt = 180;
			break;
		case 3:
			RGB_SpeedLevel_cnt = 80;
			break;
		default:
			break;
	}
	//------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LED_1_mode_RGBdataProcess
 * Description   : 光谱循环
 *
 * END ****************************************************************************************************************/
void LED_1_mode_RGBdataProcess(uint8_t RGB_index)
{
	/* 等于0是一次循环更新完全部RGB灯的值后的下一次开始 */
	if(RGB_index == 0)
	{
		/* 开始先做一次颜色变化，然后等21个RGB_index过后同步了所有的led颜色后再做下次颜色变化 */
		//------------------------------------------------------------------------------------------
		if((LED_Buffer_R[SPI0_GROUP][0] == 0xFF) && (LED_Buffer_G[SPI0_GROUP][0] < 0xFF) && (LED_Buffer_B[SPI0_GROUP][0] == 0))
		{
			if(LED_Buffer_G[SPI0_GROUP][0] > (0xFF - 3))
			{
				LED_Buffer_G[SPI0_GROUP][0] = 0xFF;
			}
			else
			{
				LED_Buffer_G[SPI0_GROUP][0] += 3;
			}
		}
		else if((LED_Buffer_R[SPI0_GROUP][0] > 0) && (LED_Buffer_G[SPI0_GROUP][0] == 0xFF) && (LED_Buffer_B[SPI0_GROUP][0] == 0))
		{
			if(LED_Buffer_R[SPI0_GROUP][0] < 3)
			{
				LED_Buffer_R[SPI0_GROUP][0] = 0;
			}
			else
			{
				LED_Buffer_R[SPI0_GROUP][0] -= 3;
			}
		}
		else if((LED_Buffer_R[SPI0_GROUP][0] == 0) && (LED_Buffer_G[SPI0_GROUP][0] == 0xFF) && (LED_Buffer_B[SPI0_GROUP][0] < 0xFF))
		{
			if(LED_Buffer_B[SPI0_GROUP][0] > (0xFF - 3))
			{
				LED_Buffer_B[SPI0_GROUP][0] = 0xFF;
			}
			else
			{
				LED_Buffer_B[SPI0_GROUP][0] += 3;
			}
		}
		else if((LED_Buffer_R[SPI0_GROUP][0] == 0) && (LED_Buffer_G[SPI0_GROUP][0] > 0) && (LED_Buffer_B[SPI0_GROUP][0] == 0xFF))
		{
			if(LED_Buffer_G[SPI0_GROUP][0] < 3)
			{
				LED_Buffer_G[SPI0_GROUP][0] = 0;
			}
			else
			{
				LED_Buffer_G[SPI0_GROUP][0] -= 3;
			}
		}
		else if((LED_Buffer_R[SPI0_GROUP][0] < 0xFF) && (LED_Buffer_G[SPI0_GROUP][0] == 0) && (LED_Buffer_B[SPI0_GROUP][0] == 0xFF))
		{
			if(LED_Buffer_R[SPI0_GROUP][0] > (0xFF - 3))
			{
				LED_Buffer_R[SPI0_GROUP][0] = 0xFF;
			}
			else
			{
				LED_Buffer_R[SPI0_GROUP][0] += 3;
			}
		}
		else if((LED_Buffer_B[SPI0_GROUP][0] != 0))
		{
			if(LED_Buffer_B[SPI0_GROUP][0] < 3)
			{
				LED_Buffer_B[SPI0_GROUP][0] = 0;
			}
			else
			{
				LED_Buffer_B[SPI0_GROUP][0] -= 3;
			}
		}
	}
	//else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI0_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI1_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI1_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI1_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI1_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI3_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI3_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI3_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI3_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI4_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI4_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI4_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI4_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI6_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI6_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI6_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI6_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LED_2_mode_RGBdataInit
 * Description   :常亮
 *
 * END ****************************************************************************************************************/
void LED_2_mode_RGBdataInit(void)
{
	uint8_t i;
	uint8_t j;
	//------------------------------------------------------------------------------------------
	if(g_variableData.LED_ONOFF)
	{
		if(g_variableData.LED_RGB_MODE)
		{
			/* 颜色初始化 */
			//--------------------------------------------------------------------------------------------------
			for(i = 0; i < TRANSFER_Group; i++)
			{
				for(j = 0; j < TRANSFER_SIZE; j++)
				{
					//------------------------------------------------------------------------------------------
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
					//------------------------------------------------------------------------------------------
					/* 高亮的时候要常亮白灯 */
					if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
					{
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
					}
					else
					{
						/* 颜色初始化 */
						LED_Buffer_R[i][j] = g_variableData.LED_COLOR[0];
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = g_variableData.LED_COLOR[1];
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = g_variableData.LED_COLOR[2];
						//------------------------------------------------------------------------------------------
					}
				}
			}
			//--------------------------------------------------------------------------------------------------
		}
		else
		{
			//--------------------------------------------------------------------------------------------------
			for(i = 0; i < TRANSFER_Group; i++)
			{
				for(j = 0; j < TRANSFER_SIZE; j++)
				{
					//------------------------------------------------------------------------------------------
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
					//------------------------------------------------------------------------------------------
					/* 高亮的时候要常亮白灯 */
					if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
					{
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
					}
					else
					{
						/* 光谱循环:不支持颜色设置，颜色初始化 */
						/* 颜色初始化 */
						LED_Buffer_R[i][j] = LightingColorTable[g_variableData.LedColor_Index][RGB_R];
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = LightingColorTable[g_variableData.LedColor_Index][RGB_G];
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = LightingColorTable[g_variableData.LedColor_Index][RGB_B];
						//------------------------------------------------------------------------------------------
					}
				}
			}
			//------------------------------------------------------------------------------------------
		}
	}
	else
	{
		//--------------------------------------------------------------------------------------------------
		for(i = 0; i < TRANSFER_Group; i++)
		{
			for(j = 0; j < TRANSFER_SIZE; j++)
			{
				//------------------------------------------------------------------------------------------
				/* 亮度初始化 */
				LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
				//------------------------------------------------------------------------------------------
				/* 高亮的时候要常亮白灯 */
				if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
				{
					//------------------------------------------------------------------------------------------
					LED_Buffer_R[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
				}
				else
				{
					/* 颜色初始化 */
					LED_Buffer_R[i][j] = 0x00;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[i][j] = 0x00;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[i][j] = 0x00;
					//------------------------------------------------------------------------------------------
				}
				//------------------------------------------------------------------------------------------
			}
		}
		//------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name : LED_LightingMode_RGBdataProcess
 * Description   :
 *
 * END ****************************************************************************************************************/
void LED_2_mode_RGBdataProcess(uint8_t RGB_index)
{
	//------------------------------------------------------------------------------------------
	/* 亮度初始化 */
	LED_Buffer_I[SPI0_GROUP][RGB_index] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
	LED_Buffer_I[SPI1_GROUP][RGB_index] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
	LED_Buffer_I[SPI3_GROUP][RGB_index] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
	LED_Buffer_I[SPI4_GROUP][RGB_index] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
	LED_Buffer_I[SPI6_GROUP][RGB_index] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
	//else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI0_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI1_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI1_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI1_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI1_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI3_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI3_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI3_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI3_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI4_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI4_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI4_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI4_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI6_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI6_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI6_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI6_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/******************************************************************************************************************
 *Function Name : LED_3_mode_RGBdataInit
 *Description   : 左右冲浪
 *****************************************************************************************************************/
void LED_3_mode_RGBdataInit(void)
{
	uint8_t i;
	uint8_t j;
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			//------------------------------------------------------------------------------------------
			/* 亮度初始化 */
			LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
			//------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
			{
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
			}
			else
			{
				/* 光谱循环:不支持颜色设置，颜色初始化 */
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
			}
			//------------------------------------------------------------------------------------------
			led_ColorIndex[i][j] = 0x00;
			//------------------------------------------------------------------------------------------
			led_UserConfig[i][j] = 0x00;
		}
	}
	//------------------------------------------------------------------------------------------
	switch(g_variableData.LED_SPEED)
	{
		case 0:
			RGB_SpeedLevel_cnt = 600;
			break;
		case 1:
			RGB_SpeedLevel_cnt = 360;
			break;
		case 2:
			RGB_SpeedLevel_cnt = 200;
			break;
		case 3:
			RGB_SpeedLevel_cnt = 120;
			break;
		default:
			break;
	}
	//------------------------------------------------------------------------------------------

}
/******************************************************************************************************************
 *Function Name : LED_3_mode_RGBdataInit
 *Description   :
 *****************************************************************************************************************/
void LED_3_mode_RGBdataProcess(uint8_t RGB_index)
{
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t index = 0;
	//------------------------------------------------------------------------------------------
	/* 21次写灯才是一次循环的开始 */
	if(RGB_index == 0)
	{
		if(led_UserConfig[0][0] < LED_COL_MAX)
		{
			led_UserConfig[0][0]++;
		}
		//------------------------------------------------------------------------------------------
		for(i = 0; i < led_UserConfig[0][0]; i++)
		{
			led_ColorIndex[0][i]++;
			if(led_ColorIndex[0][i] >= SPECTURN_COLOR_NUM)
			{
				led_ColorIndex[0][i] = 0;
			}
		}
	}
	//------------------------------------------------------------------------------------------
	for(i=0; i < LED_ROW_MAX; i++)
	{
		if( RGB_index < led_UserConfig[0][0])
		{
			if((led_position_to_coord[i][RGB_index].row != 0xff) && (led_position_to_coord[i][RGB_index].col != 0xff))
			{
				//if(slow_motion_result[index] == SPEED_LOW)continue;
				LED_Buffer_R[ led_position_to_coord[i][RGB_index].row ][ led_position_to_coord[i][RGB_index].col ] = Specturn_colors[ led_ColorIndex[0][RGB_index] ][0];
				LED_Buffer_G[ led_position_to_coord[i][RGB_index].row ][ led_position_to_coord[i][RGB_index].col ] = Specturn_colors[ led_ColorIndex[0][RGB_index] ][1];
				LED_Buffer_B[ led_position_to_coord[i][RGB_index].row ][ led_position_to_coord[i][RGB_index].col ] = Specturn_colors[ led_ColorIndex[0][RGB_index] ][2];
			}
		}
	}
	//else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI0_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI1_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI3_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI4_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI6_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/************************************************************************************************************************************************************************
 *Function Name : LED_4_mode_RGBdataInit
 *Description   : 呼吸
 ***********************************************************************************************************************************************************************/
void LED_4_mode_RGBdataInit(void)
{
	uint8_t i;
	uint8_t j;
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			//------------------------------------------------------------------------------------------
			/* 亮度初始化 */
			LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
			//------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
			{
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
			}
			else
			{
				/* 光谱循环:不支持颜色设置，颜色初始化 */
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
			}
			//------------------------------------------------------------------------------------------
			led_ColorIndex[i][j] = 0x00;
			//------------------------------------------------------------------------------------------
			led_UserConfig[i][j] = 0x00;
		}
	}
	//------------------------------------------------------------------------------------------
	Breath_color_num = 0;
	Breath_color_flag = 0;
	Breath_colorful = 0;
	//------------------------------------------------------------------------------------------
	switch(g_variableData.LED_SPEED)
	{
		case 0:
			RGB_SpeedLevel_cnt = 500;
			break;
		case 1:
			RGB_SpeedLevel_cnt = 300;
			break;
		case 2:
			RGB_SpeedLevel_cnt = 180;
			break;
		case 3:
			RGB_SpeedLevel_cnt = 100;
			break;
		default:
			break;
	}
	//------------------------------------------------------------------------------------------
}
/************************************************************************************************************************************************************************
 *Function Name : LED_4_mode_RGBdataInit
 *Description   :
 ***********************************************************************************************************************************************************************/
void LED_4_mode_RGBdataProcess(uint8_t RGB_index)
{
	uint8_t rgb_red, rgb_green, rgb_blue;
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	/* 21次写灯才是一次循环的开始 */
	if(RGB_index == 0)
	{
		if(g_variableData.LED_RGB_MODE)
		{
			rgb_red = LightingColorTable[Breath_colorful][0];
			rgb_green = LightingColorTable[Breath_colorful][1];
			rgb_blue = LightingColorTable[Breath_colorful][2];
		}
		else
		{
			rgb_red = g_variableData.LED_COLOR[0];
			rgb_green = g_variableData.LED_COLOR[1];
			rgb_blue = g_variableData.LED_COLOR[2];
		}

		LED_Buffer_R[SPI0_GROUP][0] = (uint8_t)(((uint16_t)(Gamma_steps[Breath_color_num] * rgb_red)) >> 8);
		LED_Buffer_G[SPI0_GROUP][0] = (uint8_t)(((uint16_t)(Gamma_steps[Breath_color_num] * rgb_green)) >> 8);
		LED_Buffer_B[SPI0_GROUP][0] = (uint8_t)(((uint16_t)(Gamma_steps[Breath_color_num] * rgb_blue)) >> 8);

		if(Breath_color_flag == 0)
		{
			Breath_color_num++;
			if(Breath_color_num == color_max-1)
			{
				Breath_color_flag = 1;
			}
		}
		else
		{
			Breath_color_num--;
			if(Breath_color_num == 0)
			{
				Breath_color_flag = 0;
				Breath_colorful++;
				if(Breath_colorful >= 7)
				{
					Breath_colorful = 0;
				}
			}
		}
	}

	{
		if(g_variableData.LED_RGB_MODE)
		{
			rgb_red = LightingColorTable[Breath_colorful][0];
			rgb_green = LightingColorTable[Breath_colorful][1];
			rgb_blue = LightingColorTable[Breath_colorful][2];
		}
		else
		{
			rgb_red = g_variableData.LED_COLOR[0];
			rgb_green = g_variableData.LED_COLOR[1];
			rgb_blue = g_variableData.LED_COLOR[2];
		}

		LED_Buffer_R[SPI0_GROUP][0] = (uint8_t)(((uint16_t)(Gamma_steps[Breath_color_num] * rgb_red)) >> 8);
		LED_Buffer_G[SPI0_GROUP][0] = (uint8_t)(((uint16_t)(Gamma_steps[Breath_color_num] * rgb_green)) >> 8);
		LED_Buffer_B[SPI0_GROUP][0] = (uint8_t)(((uint16_t)(Gamma_steps[Breath_color_num] * rgb_blue)) >> 8);
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI0_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI1_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI1_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI1_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI1_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI3_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI3_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI3_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI3_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI4_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI4_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI4_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI4_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI6_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		else
		{
			LED_Buffer_R[SPI6_GROUP][RGB_index] = LED_Buffer_R[SPI0_GROUP][0];
			LED_Buffer_G[SPI6_GROUP][RGB_index] = LED_Buffer_G[SPI0_GROUP][0];
			LED_Buffer_B[SPI6_GROUP][RGB_index] = LED_Buffer_B[SPI0_GROUP][0];
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/******************************************************************************************************************
 *Function Name : LED_5_mode_RGBdataInit
 *Description   : 往中间蛇形跑灯
 *****************************************************************************************************************/
void LED_5_mode_RGBdataInit(void)
{
	uint8_t i;
	uint8_t j;
	uint8_t m;
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			//------------------------------------------------------------------------------------------
			/* 亮度初始化 */
			LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
			//------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
			{
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
			}
			else
			{
				/* 光谱循环:不支持颜色设置，颜色初始化 */
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
			}
			//------------------------------------------------------------------------------------------
			led_ColorIndex[i][j] = 0x00;
			//------------------------------------------------------------------------------------------
			led_UserConfig[i][j] = 0x00;
			//------------------------------------------------------------------------------------------
			led_right_move[0][j] = 0x00;
			led_right_move[1][j] = 0x00;
			led_right_move[2][j] = 0x00;
			led_right_move[3][j] = 0x00;
			led_right_move[4][j] = 0x00;
			//------------------------------------------------------------------------------------------
			led_key_mark[0][j] = 0x00;
			led_key_mark[1][j] = 0x00;
			led_key_mark[2][j] = 0x00;
			led_key_mark[3][j] = 0x00;
			led_key_mark[4][j] = 0x00;
			//------------------------------------------------------------------------------------------
		}
	}
	led_UserConfig[0][0] = 0x00;
	led_UserConfig[0][1]=HorseTable[led_UserConfig[0][0]].row;
	led_UserConfig[0][2]=HorseTable[led_UserConfig[0][0]].col;
	led_key_mark[led_UserConfig[0][1]][led_UserConfig[0][2]] = 1;
	//------------------------------------------------------------------------------------------
	switch(g_variableData.LED_SPEED)
	{
		case 0:
			RGB_SpeedLevel_cnt = 350;
			break;
		case 1:
			RGB_SpeedLevel_cnt = 170;
			break;
		case 2:
			RGB_SpeedLevel_cnt = 120;
			break;
		case 3:
			RGB_SpeedLevel_cnt = 50;
			break;
		default:
			break;
	}
	//------------------------------------------------------------------------------------------

}
/******************************************************************************************************************
 *Function Name : LED_5_mode_RGBdataInit
 *Description   :
 *****************************************************************************************************************/
void LED_5_mode_RGBdataProcess(uint8_t RGB_index)
{
	uint8_t i;
	uint8_t row,col,test;

	//else
	{
		//ROW=0
		if(led_key_mark[0][RGB_index])
		{
			if((led_position_to_coord[0][RGB_index].row!= 0xff) && (led_position_to_coord[0][RGB_index].col!= 0xff))
			{
				row = led_position_to_coord[0][RGB_index].row;
				col = led_position_to_coord[0][RGB_index].col;
				if((LED_Buffer_R[row][col]<FIVE_SETPS) && (LED_Buffer_G[row][col]==0) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_R[row][col]+=25;
					led_right_move[0][RGB_index]+=25;
				}
				else if((LED_Buffer_R[row][col]==FIVE_SETPS) && (LED_Buffer_G[row][col]!=FIVE_SETPS) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_G[row][col]+=25;
				}
				else if((LED_Buffer_G[row][col]==FIVE_SETPS) && (LED_Buffer_R[row][col]!=0) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_R[row][col]-=25;
					led_right_move[0][RGB_index]-=25;
				}
				else if((LED_Buffer_R[row][col]==0) && (LED_Buffer_B[row][col]!=FIVE_SETPS) && (LED_Buffer_G[row][col]==FIVE_SETPS))
				{
					LED_Buffer_B[row][col]+=25;
				}
				else if((LED_Buffer_B[row][col]==FIVE_SETPS) && (LED_Buffer_G[row][col]!=0) && (LED_Buffer_R[row][col]==0))
				{
					LED_Buffer_G[row][col]-=25;
				}
				else if((LED_Buffer_G[row][col]==0) && (LED_Buffer_B[row][col]==FIVE_SETPS) && (LED_Buffer_R[row][col]!=FIVE_SETPS))
				{
					LED_Buffer_R[row][col]+=25;
					led_right_move[0][RGB_index]+=25;
				}
				else if((LED_Buffer_B[row][col]!=0))
				{
					LED_Buffer_B[row][col]-=25;
					LED_Buffer_R[row][col]-=25;
					led_right_move[0][RGB_index]-=25;
					if((LED_Buffer_B[row][col]==0)&&(LED_Buffer_R[row][col]==0))
					{
						led_right_move[0][RGB_index] = 0;
						led_key_mark[0][RGB_index]= 0;
					}

				}
			}
		}
		//ROW=1
		if(led_key_mark[1][RGB_index])
		{
			if((led_position_to_coord[1][RGB_index].row!= 0xff) && (led_position_to_coord[1][RGB_index].col!= 0xff))
			{
				row = led_position_to_coord[1][RGB_index].row;
				col = led_position_to_coord[1][RGB_index].col;
				if((LED_Buffer_R[row][col]<FIVE_SETPS) && (LED_Buffer_G[row][col]==0) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_R[row][col]+=25;
					led_right_move[1][RGB_index]+=25;
				}
				else if((LED_Buffer_R[row][col]==FIVE_SETPS) && (LED_Buffer_G[row][col]!=FIVE_SETPS) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_G[row][col]+=25;
				}
				else if((LED_Buffer_G[row][col]==FIVE_SETPS) && (LED_Buffer_R[row][col]!=0) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_R[row][col]-=25;
					led_right_move[1][RGB_index]-=25;
				}
				else if((LED_Buffer_R[row][col]==0) && (LED_Buffer_B[row][col]!=FIVE_SETPS) && (LED_Buffer_G[row][col]==FIVE_SETPS))
				{
					LED_Buffer_B[row][col]+=25;
				}
				else if((LED_Buffer_B[row][col]==FIVE_SETPS) && (LED_Buffer_G[row][col]!=0) && (LED_Buffer_R[row][col]==0))
				{
					LED_Buffer_G[row][col]-=25;
				}
				else if((LED_Buffer_G[row][col]==0) && (LED_Buffer_B[row][col]==FIVE_SETPS) && (LED_Buffer_R[row][col]!=FIVE_SETPS))
				{
					LED_Buffer_R[row][col]+=25;
					led_right_move[1][RGB_index]+=25;
				}
				else if((LED_Buffer_B[row][col]!=0))
				{
					LED_Buffer_B[row][col]-=25;
					//if(mode != 0)
					{
						LED_Buffer_R[row][col]-=25;
						led_right_move[1][RGB_index]-=25;
						if((LED_Buffer_B[row][col]==0)&&(LED_Buffer_R[row][col]==0))
						{
							led_right_move[1][RGB_index] = 0;
							led_key_mark[1][RGB_index]= 0;
						}

					}
				}
			}
		}
		//ROW=2
		if(led_key_mark[2][RGB_index])
		{
			if((led_position_to_coord[2][RGB_index].row!= 0xff) && (led_position_to_coord[2][RGB_index].col!= 0xff))
			{
				row = led_position_to_coord[2][RGB_index].row;
				col = led_position_to_coord[2][RGB_index].col;
				if((LED_Buffer_R[row][col]<FIVE_SETPS) && (LED_Buffer_G[row][col]==0) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_R[row][col]+=25;
					led_right_move[2][RGB_index]+=25;
				}
				else if((LED_Buffer_R[row][col]==FIVE_SETPS) && (LED_Buffer_G[row][col]!=FIVE_SETPS) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_G[row][col]+=25;
				}
				else if((LED_Buffer_G[row][col]==FIVE_SETPS) && (LED_Buffer_R[row][col]!=0) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_R[row][col]-=25;
					led_right_move[2][RGB_index]-=25;
				}
				else if((LED_Buffer_R[row][col]==0) && (LED_Buffer_B[row][col]!=FIVE_SETPS) && (LED_Buffer_G[row][col]==FIVE_SETPS))
				{
					LED_Buffer_B[row][col]+=25;
				}
				else if((LED_Buffer_B[row][col]==FIVE_SETPS) && (LED_Buffer_G[row][col]!=0) && (LED_Buffer_R[row][col]==0))
				{
					LED_Buffer_G[row][col]-=25;
				}
				else if((LED_Buffer_G[row][col]==0) && (LED_Buffer_B[row][col]==FIVE_SETPS) && (LED_Buffer_R[row][col]!=FIVE_SETPS))
				{
					LED_Buffer_R[row][col]+=25;
					led_right_move[2][RGB_index]+=25;
				}
				else if((LED_Buffer_B[row][col]!=0))
				{
					LED_Buffer_B[row][col]-=25;
					//if(mode != 0)
					{
						LED_Buffer_R[row][col]-=25;
						led_right_move[2][RGB_index]-=25;
						if((LED_Buffer_B[row][col]==0)&&(LED_Buffer_R[row][col]==0))
						{
							led_right_move[2][RGB_index] = 0;
							led_key_mark[2][RGB_index]= 0;
						}

					}
				}
			}
		}
		//ROW=3
		if(led_key_mark[3][RGB_index])
		{
			if((led_position_to_coord[3][RGB_index].row!= 0xff) && (led_position_to_coord[3][RGB_index].col!= 0xff))
			{
				row = led_position_to_coord[3][RGB_index].row;
				col = led_position_to_coord[3][RGB_index].col;
				if((LED_Buffer_R[row][col]<FIVE_SETPS) && (LED_Buffer_G[row][col]==0) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_R[row][col]+=25;
					led_right_move[3][RGB_index]+=25;
				}
				else if((LED_Buffer_R[row][col]==FIVE_SETPS) && (LED_Buffer_G[row][col]!=FIVE_SETPS) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_G[row][col]+=25;
				}
				else if((LED_Buffer_G[row][col]==FIVE_SETPS) && (LED_Buffer_R[row][col]!=0) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_R[row][col]-=25;
					led_right_move[3][RGB_index]-=25;
				}
				else if((LED_Buffer_R[row][col]==0) && (LED_Buffer_B[row][col]!=FIVE_SETPS) && (LED_Buffer_G[row][col]==FIVE_SETPS))
				{
					LED_Buffer_B[row][col]+=25;
				}
				else if((LED_Buffer_B[row][col]==FIVE_SETPS) && (LED_Buffer_G[row][col]!=0) && (LED_Buffer_R[row][col]==0))
				{
					LED_Buffer_G[row][col]-=25;
				}
				else if((LED_Buffer_G[row][col]==0) && (LED_Buffer_B[row][col]==FIVE_SETPS) && (LED_Buffer_R[row][col]!=FIVE_SETPS))
				{
					LED_Buffer_R[row][col]+=25;
					led_right_move[3][RGB_index]+=25;
				}
				else if((LED_Buffer_B[row][col]!=0))
				{
					LED_Buffer_B[row][col]-=25;
					//if(mode != 0)
					{
						LED_Buffer_R[row][col]-=25;
						led_right_move[3][RGB_index]-=25;
						if((LED_Buffer_B[row][col]==0)&&(LED_Buffer_R[row][col]==0))
						{
							led_right_move[3][RGB_index] = 0;
							led_key_mark[3][RGB_index] = 0;
						}

					}
				}
			}
		}
		//ROW=4
		if(led_key_mark[4][RGB_index])
		{
			if((led_position_to_coord[4][RGB_index].row!= 0xff) && (led_position_to_coord[4][RGB_index].col!= 0xff))
			{
				row = led_position_to_coord[4][RGB_index].row;
				col = led_position_to_coord[4][RGB_index].col;
				if((LED_Buffer_R[row][col]<FIVE_SETPS) && (LED_Buffer_G[row][col]==0) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_R[row][col]+=25;
					led_right_move[4][RGB_index]+=25;
				}
				else if((LED_Buffer_R[row][col]==FIVE_SETPS) && (LED_Buffer_G[row][col]!=FIVE_SETPS) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_G[row][col]+=25;
				}
				else if((LED_Buffer_G[row][col]==FIVE_SETPS) && (LED_Buffer_R[row][col]!=0) && (LED_Buffer_B[row][col]==0))
				{
					LED_Buffer_R[row][col]-=25;
					led_right_move[4][RGB_index]-=25;
				}
				else if((LED_Buffer_R[row][col]==0) && (LED_Buffer_B[row][col]!=FIVE_SETPS) && (LED_Buffer_G[row][col]==FIVE_SETPS))
				{
					LED_Buffer_B[row][col]+=25;
				}
				else if((LED_Buffer_B[row][col]==FIVE_SETPS) && (LED_Buffer_G[row][col]!=0) && (LED_Buffer_R[row][col]==0))
				{
					LED_Buffer_G[row][col]-=25;
				}
				else if((LED_Buffer_G[row][col]==0) && (LED_Buffer_B[row][col]==FIVE_SETPS) && (LED_Buffer_R[row][col]!=FIVE_SETPS))
				{
					LED_Buffer_R[row][col]+=25;
					led_right_move[4][RGB_index]+=25;
				}
				else if((LED_Buffer_B[row][col]!=0))
				{
					LED_Buffer_B[row][col]-=25;
					//if(mode != 0)
					{
						LED_Buffer_R[row][col]-=25;
						led_right_move[4][RGB_index]-=25;
						if((LED_Buffer_B[row][col]==0)&&(LED_Buffer_R[row][col]==0))
						{
							led_right_move[4][RGB_index] = 0;
							led_key_mark[4][RGB_index] = 0;
						}
					}
				}
			}
		}
	}
	if(RGB_index == 0)
	{
		/* 一次这个灯的操作用了42us */
		/* 后面再改这个时间，可以优化把下面这个for去掉,改成每次进来都判断一次，现在是21次才进来一次 */
		if(led_right_move[led_UserConfig[0][1]][led_UserConfig[0][2]]==ONE_SETPS)
		{
			if(led_UserConfig[0][0] < 67)
			{
				led_UserConfig[0][0]++;
				led_UserConfig[0][1]=HorseTable[led_UserConfig[0][0]].row;
				led_UserConfig[0][2]=HorseTable[led_UserConfig[0][0]].col;
				led_key_mark[led_UserConfig[0][1]][led_UserConfig[0][2]]=1;
			}
		}
		else
		{
			if(!led_key_mark[led_UserConfig[0][1]][led_UserConfig[0][2]])
			{
				led_UserConfig[0][0]=0;
				led_UserConfig[0][1]=HorseTable[led_UserConfig[0][0]].row;
				led_UserConfig[0][2]=HorseTable[led_UserConfig[0][0]].col;
				led_key_mark[led_UserConfig[0][1]][led_UserConfig[0][2]] = 1;
			}
		}
	}
	//else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI0_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI1_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI3_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI4_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI6_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/******************************************************************************************************************
 *Function Name : LED_6_mode_RGBdataInit
 *Description   : 往下亮
 *****************************************************************************************************************/
void LED_6_mode_RGBdataInit(void)
{
	uint8_t i;
	uint8_t j;
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			//------------------------------------------------------------------------------------------
			/* 亮度初始化 */
			LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
			//------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
			{
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
			}
			else
			{
				/* 光谱循环:不支持颜色设置，颜色初始化 */
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
			}
			//------------------------------------------------------------------------------------------
		}
	}
	//------------------------------------------------------------------------------------------
	switch(g_variableData.LED_SPEED)
	{
		case 0:
			RGB_SpeedLevel_cnt = 5;
			break;
		case 1:
			RGB_SpeedLevel_cnt = 5;
			break;
		case 2:
			RGB_SpeedLevel_cnt = 5;
			break;
		case 3:
			RGB_SpeedLevel_cnt = 5;
			break;
		default:
			break;
	}
	//------------------------------------------------------------------------------------------
}
void LED_6_mode_RGBdataProcess(uint8_t RGB_index)
{
	uint8_t i,j = 0;
	uint8_t p_row,p_col,led_row,led_col;
	uint8_t adjust = 0;
	RGB_STRU h_rgb;
	if(RGB_index == 0)
	{
		LED_RGB_buf_clear();
	}
	//------------------------------------------------------------------------------------------
	//Row=0
	if(((Hall_Node_SPI7[RGB_index] > adjust) && (Hall_Node_SPI7[RGB_index] <= 200)))
	{
		h_rgb = node_num_depend(0, Hall_Node_SPI7[RGB_index], 0, 0, 0, 0);
		//0表示第0行,第0行是ESC，是用SPI7来采的样
		LED_6_mode_keyAreaOn(0, RGB_index, h_rgb);
	}
	//------------------------------------------------------------------------------------------
	//Row=1
	if(((Hall_Node_SPI3[RGB_index] > adjust) && (Hall_Node_SPI3[RGB_index] <= 200)))
	{
		h_rgb = node_num_depend(1,0, Hall_Node_SPI3[RGB_index], 0, 0, 0);
		//1表示第1行,第1行是TAB，是用SPI3来采的样
		LED_6_mode_keyAreaOn(1, RGB_index, h_rgb);
	}
	//------------------------------------------------------------------------------------------
	//Row=2
	if(((Hall_Node_SPI4[RGB_index] > adjust) && (Hall_Node_SPI4[RGB_index] <= 200)))
	{
		h_rgb = node_num_depend(2, 0, 0,Hall_Node_SPI4[RGB_index], 0, 0);
		//2表示第2行,第2行是CAPS，是用SPI4来采的样
		LED_6_mode_keyAreaOn(2, RGB_index, h_rgb);
	}
	//------------------------------------------------------------------------------------------
	//Row=3
	if(((Hall_Node_SPI0[RGB_index] > adjust) && (Hall_Node_SPI0[RGB_index] <= 200)))
	{
		h_rgb = node_num_depend(3, 0, 0, 0, Hall_Node_SPI0[RGB_index], 0);
		//3表示第3行,第3行是shift，是用SPI0来采的样
		LED_6_mode_keyAreaOn(3, RGB_index, h_rgb);
	}
	//------------------------------------------------------------------------------------------
	//Row=4
	if(((Hall_Node_SPI1[RGB_index] > adjust) && (Hall_Node_SPI1[RGB_index] <= 200)))
	{
		h_rgb = node_num_depend(4, 0, 0, 0, 0,Hall_Node_SPI1[RGB_index]);
		//4表示第4行,第4行是ctrl，是用SPI1来采的样
		LED_6_mode_keyAreaOn(4, RGB_index, h_rgb);
	}
	//else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI0_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI1_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI3_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI4_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI6_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/******************************************************************************************************************
 *Function Name : LED_6_mode_RGBdataInit
 *Description   :
 *****************************************************************************************************************/
void led_mode_6_rgb_set(uint8_t row, uint8_t col, RGB_STRU p_rgb)
{
	unsigned char p_col, p_row, p_pos;

	p_row = LED_Layout_Position[row][col].row;
	p_col = LED_Layout_Position[row][col].col;
	if((p_row != 0xff) && (p_col != 0xff))
	{
		LED_Buffer_R[p_row][p_col] = p_rgb.red;
		LED_Buffer_G[p_row][p_col] = p_rgb.green;
		LED_Buffer_B[p_row][p_col] = p_rgb.blue;
	}

}
RGB_STRU node_num_depend(uint8_t n,uint16_t node0,uint16_t node1,uint16_t node2,uint16_t node3,uint16_t node4)
{
	RGB_STRU p_rgb;
#if 1
	{
	if((node0 - 0) < 50) node0 = 0;
	if((node1 - 0) < 50) node1 = 0;
	if((node2 - 0) < 50) node2 = 0;
	if((node3 - 0) < 50) node3 = 0;
	if((node4 - 0) < 50) node4 = 0;

	}
#endif
	if(n==0)
	{
		node0 = node0/5;
		if(g_variableData.LED_RGB_MODE)
		{
			p_rgb.red   =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_R] * 6 * node0)>>8);
			p_rgb.green =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_G] * 6 * node0)>>8);
			p_rgb.blue  =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_B] * 6 * node0)>>8);
		}
		else
		{
			p_rgb.red   =  g_variableData.LED_COLOR[0];
			p_rgb.green =  g_variableData.LED_COLOR[1];
			p_rgb.blue  =  g_variableData.LED_COLOR[2];
		}
	}
	if(n==1)
	{
		node1 = node1/5;
		if(g_variableData.LED_RGB_MODE)
		{
			p_rgb.red   =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_R] * 6 *node1)>>8);
			p_rgb.green =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_G] * 6 *node1)>>8);
			p_rgb.blue  =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_B] * 6 *node1)>>8);
		}
		else
		{
			p_rgb.red   =  g_variableData.LED_COLOR[0];
			p_rgb.green =  g_variableData.LED_COLOR[1];
			p_rgb.blue  =  g_variableData.LED_COLOR[2];
		}
	}
	if(n==2)
	{
		node2 = node2/5;
		if(g_variableData.LED_RGB_MODE)
		{
			p_rgb.red   =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_R] * 6 *node2)>>8);
			p_rgb.green =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_G] * 6 *node2)>>8);
			p_rgb.blue  =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_B] * 6 *node2)>>8);
		}
		else
		{
			p_rgb.red   =  g_variableData.LED_COLOR[0];
			p_rgb.green =  g_variableData.LED_COLOR[1];
			p_rgb.blue  =  g_variableData.LED_COLOR[2];
		}
	}
	if(n==3)
	{
		node3 = node3/5;
		if(g_variableData.LED_RGB_MODE)
		{
			p_rgb.red   =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_R] * 6 *node3)>>8);
			p_rgb.green =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_G] * 6 *node3)>>8);
			p_rgb.blue  =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_B] * 6 *node3)>>8);
		}
		else
		{
			p_rgb.red   =  g_variableData.LED_COLOR[0];
			p_rgb.green =  g_variableData.LED_COLOR[1];
			p_rgb.blue  =  g_variableData.LED_COLOR[2];
		}
	}
	if(n==4)
	{
		node4 = node4/5;
		if(g_variableData.LED_RGB_MODE)
		{
			p_rgb.red   =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_R] * 6 *node4)>>8);
			p_rgb.green =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_G] * 6 *node4)>>8);
			p_rgb.blue  =  ((LightingColorTable[g_variableData.LedColor_Index][RGB_B] * 6 *node4)>>8);
		}
		else
		{
			p_rgb.red   =  g_variableData.LED_COLOR[0];
			p_rgb.green =  g_variableData.LED_COLOR[1];
			p_rgb.blue  =  g_variableData.LED_COLOR[2];
		}
	}
	return p_rgb;
}
void LED_6_mode_keyAreaOn(uint8_t i, uint8_t j,RGB_STRU h_rgb)
{
	uint8_t p_row,p_col,led_row,led_col,n;
	//Row=4
	i = LED_MDOE6_Position[i][j].row;
	j = LED_MDOE6_Position[i][j].col;
	//触发键
	if((i < 5) && (j < 21))
	{
		p_row = i;
		p_col = j;
		led_mode_6_rgb_set(p_row, p_col,h_rgb);
	}
	//左上
	if((i < 5) && (i  > 0) && (j > 0))
	{
		p_row = i - 1;
		p_col = j - 1;
		led_mode_6_rgb_set(p_row, p_col,h_rgb);
	}
	//正上
	if((i < 5) && (i > 0) && (j < 21))
	{
		p_row = i - 1;
		p_col = j;
		led_mode_6_rgb_set(p_row, p_col,h_rgb);
	}
	//右上
	if((i < 5) && (i > 0) && (j < 20))
	{
		p_row = i - 1 ;
		p_col = j + 1;
		led_mode_6_rgb_set(p_row, p_col,h_rgb);
	}
	//正左
	if((i < 5) && (j > 0))
	{
		p_row = i;
		p_col = j - 1;
		led_mode_6_rgb_set(p_row, p_col,h_rgb);
	}
	//正右
	if((i < 5) && (j < 20))
	{
		p_row = i;
		p_col = j + 1;
		led_mode_6_rgb_set(p_row, p_col,h_rgb);
	}
	//左下
	if((i < 4) && (j > 0))
	{
		p_row = i + 1;
		p_col = j - 1;
		led_mode_6_rgb_set(p_row, p_col,h_rgb);
	}
	//正下
	if((i < 4) && (j < 20))
	{
		p_row = i + 1;
		p_col = j;
		led_mode_6_rgb_set(p_row, p_col,h_rgb);
	}
	//右下
	if((i < 4) && (j  < 20))
	{
		p_row = i + 1;
		p_col = j + 1;
		led_mode_6_rgb_set(p_row, p_col,h_rgb);
	}
}

/******************************************************************************************************************
 *Function Name : LED_7_mode_RGBdataInit
 *Description   : 旋转跑灯
 *****************************************************************************************************************/
void LED_7_mode_RGBdataInit(void)
{
	uint8_t i;
	uint8_t j;
	uint8_t k;
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			//------------------------------------------------------------------------------------------
			/* 亮度初始化 */
			LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
			//------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
			{
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
			}
			else
			{
				/* 光谱循环:不支持颜色设置，颜色初始化 */
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
			}
			//------------------------------------------------------------------------------------------
			led_ColorIndex[i][j] = 0x00;
			//------------------------------------------------------------------------------------------
			led_UserConfig[i][j] = 0x00;
		}
		for(k=0;k<LOOP_LINE_MAX;k++)
		{
			led_ColorIndex[0][k]= (12 - k) * 3;
		}
	}
	//------------------------------------------------------------------------------------------
	switch(g_variableData.LED_SPEED)
	{
		case 0:
			RGB_SpeedLevel_cnt = 300;
			break;
		case 1:
			RGB_SpeedLevel_cnt = 200;
			break;
		case 2:
			RGB_SpeedLevel_cnt = 120;
			break;
		case 3:
			RGB_SpeedLevel_cnt = 70;
			break;
		default:
			break;
	}
	//------------------------------------------------------------------------------------------
}
/******************************************************************************************************************
 *Function Name : LED_7_mode_RGBdataInit
 *Description   :
 *****************************************************************************************************************/
void LED_7_mode_RGBdataProcess(uint8_t RGB_index)
{
	uint8_t  i, k;
	uint8_t p_row, p_col;
	if(RGB_index < LOOP_LINE_MAX)
	{
		led_ColorIndex[0][RGB_index] += 3;
		if(led_ColorIndex[0][RGB_index] >= SPECTURN_COLOR_NUM)
		{
			led_ColorIndex[0][RGB_index] = 0;
		}
	}

	if(RGB_index < led_mode7_loop_len(1))
	{
		p_row = loop1[RGB_index].row;
		p_col = loop1[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 1, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(2))
	{
		p_row = loop2[RGB_index].row;
		p_col = loop2[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 2, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(3))
	{
		p_row = loop3[RGB_index].row;
		p_col = loop3[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 3, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(4))
	{
		p_row = loop4[RGB_index].row;
		p_col = loop4[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 4, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(5))
	{
		p_row = loop5[RGB_index].row;
		p_col = loop5[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 5, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(6))
	{
		p_row = loop6[RGB_index].row;
		p_col = loop6[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 6, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(7))
	{
		p_row = loop7[RGB_index].row;
		p_col = loop7[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 7, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(8))
	{
		p_row = loop8[RGB_index].row;
		p_col = loop8[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 8, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(9))
	{
		p_row = loop9[RGB_index].row;
		p_col = loop9[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 9, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(10))
	{
		p_row = loop10[RGB_index].row;
		p_col = loop10[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 10, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(11))
	{
		p_row = loop11[RGB_index].row;
		p_col = loop11[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 11, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(12))
	{
		p_row = loop12[RGB_index].row;
		p_col = loop12[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 12, RGB_index);
	}

	if(RGB_index < led_mode7_loop_len(13))
	{
		p_row = loop13[RGB_index].row;
		p_col = loop13[RGB_index].col;
		led_mode_rgb_set(p_row, p_col, 13, RGB_index);
	}
	//else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI0_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI1_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI3_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI4_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI6_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/******************************************************************************************************************
 *Function Name : LED_8_mode_RGBdataInit
 *Description   : 侧往中间浪
 *****************************************************************************************************************/
void LED_8_mode_RGBdataInit(void)
{
	uint8_t i;
	uint8_t j;
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			//------------------------------------------------------------------------------------------
			/* 亮度初始化 */
			LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
			//------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
			{
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
			}
			else
			{
				/* 光谱循环:不支持颜色设置，颜色初始化 */
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
			}
			//------------------------------------------------------------------------------------------
			led_ColorIndex[i][j] = 0x00;
			//------------------------------------------------------------------------------------------
			led_UserConfig[i][j] = 0x00;
		}
	}
	//------------------------------------------------------------------------------------------
	led_UserConfig[0][0] = 0;
	led_UserConfig[0][1] = LED_COL_MAX - 1;
	//------------------------------------------------------------------------------------------
	switch(g_variableData.LED_SPEED)
	{
		case 0:
			RGB_SpeedLevel_cnt = 600;
			break;
		case 1:
			RGB_SpeedLevel_cnt = 450;
			break;
		case 2:
			RGB_SpeedLevel_cnt = 220;
			break;
		case 3:
			RGB_SpeedLevel_cnt = 110;
			break;
		default:
			break;
	}
	//------------------------------------------------------------------------------------------
}
/******************************************************************************************************************
 *Function Name : LED_8_mode_RGBdataInit
 *Description   :
 *****************************************************************************************************************/
void LED_8_mode_RGBdataProcess(uint8_t RGB_index)
{
	unsigned char i,j,k;
	if(RGB_index == 0)
	{
		if(led_UserConfig[0][0] < (8))
		{
			led_UserConfig[0][0]++;
		}

		for(k = 0; k < led_UserConfig[0][0]; k++)
		{
			led_ColorIndex[0][k]++;
			if(led_ColorIndex[0][k] >= SPECTURN_COLOR_NUM)
			{
				led_ColorIndex[0][k] = 0;
			}
			led_ColorIndex[0][LED_COL_MAX - 7 - k] = led_ColorIndex[0][k];
		}
	}

	if((led_position_to_coord[0][RGB_index].row != 0xff) && (led_position_to_coord[0][RGB_index].col != 0xff))
	{

		LED_Buffer_R[ led_position_to_coord[0][RGB_index].row ][ led_position_to_coord[0][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][0];
		LED_Buffer_G[ led_position_to_coord[0][RGB_index].row ][ led_position_to_coord[0][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][1];
		LED_Buffer_B[ led_position_to_coord[0][RGB_index].row ][ led_position_to_coord[0][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][2];
	}

	if((led_position_to_coord[1][j].row != 0xff) && (led_position_to_coord[1][j].col != 0xff))
	{

		LED_Buffer_R[ led_position_to_coord[1][RGB_index].row ][ led_position_to_coord[1][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][0];
		LED_Buffer_G[ led_position_to_coord[1][RGB_index].row ][ led_position_to_coord[1][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][1];
		LED_Buffer_B[ led_position_to_coord[1][RGB_index].row ][ led_position_to_coord[1][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][2];
	}

	if((led_position_to_coord[2][j].row != 0xff) && (led_position_to_coord[2][j].col != 0xff))
	{

		LED_Buffer_R[ led_position_to_coord[2][RGB_index].row ][ led_position_to_coord[2][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][0];
		LED_Buffer_G[ led_position_to_coord[2][RGB_index].row ][ led_position_to_coord[2][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][1];
		LED_Buffer_B[ led_position_to_coord[2][RGB_index].row ][ led_position_to_coord[2][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][2];
	}

	if((led_position_to_coord[3][RGB_index].row != 0xff) && (led_position_to_coord[3][RGB_index].col != 0xff))
	{

		LED_Buffer_R[ led_position_to_coord[3][RGB_index].row ][ led_position_to_coord[3][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][0];
		LED_Buffer_G[ led_position_to_coord[3][RGB_index].row ][ led_position_to_coord[3][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][1];
		LED_Buffer_B[ led_position_to_coord[3][RGB_index].row ][ led_position_to_coord[3][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][2];
	}

	if((led_position_to_coord[4][RGB_index].row != 0xff) && (led_position_to_coord[4][RGB_index].col != 0xff))
	{

		LED_Buffer_R[ led_position_to_coord[4][RGB_index].row ][ led_position_to_coord[4][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][0];
		LED_Buffer_G[ led_position_to_coord[4][RGB_index].row ][ led_position_to_coord[4][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][1];
		LED_Buffer_B[ led_position_to_coord[4][RGB_index].row ][ led_position_to_coord[4][RGB_index].col ] = Specturn_colors[led_ColorIndex[0][RGB_index]][2];
	}
	//else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI0_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI1_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI3_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI4_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI6_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/******************************************************************************************************************
 *Function Name : LED_9_mode_RGBdataInit
 *Description   : 七彩喷泉
 *****************************************************************************************************************/
void LED_9_mode_RGBdataInit(void)
{
	uint8_t i;
	uint8_t j;
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			//------------------------------------------------------------------------------------------
			/* 亮度初始化 */
			LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
			//------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
			{
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
			}
			else
			{
				/* 光谱循环:不支持颜色设置，颜色初始化 */
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
			}
			//------------------------------------------------------------------------------------------
			led_ColorIndex[i][j] = 0x00;
			//------------------------------------------------------------------------------------------
			led_UserConfig[i][j] = 0x00;
		}
	}
	//------------------------------------------------------------------------------------------
	switch(g_variableData.LED_SPEED)
	{
		case 0:
			RGB_SpeedLevel_cnt = 750;
			break;
		case 1:
			RGB_SpeedLevel_cnt = 430;
			break;
		case 2:
			RGB_SpeedLevel_cnt = 380;
			break;
		case 3:
			RGB_SpeedLevel_cnt = 220;
			break;
		default:
			break;
	}
	//------------------------------------------------------------------------------------------

}
/******************************************************************************************************************
 *Function Name : LED_9_mode_RGBdataProcess
 *Description   : 七彩喷泉
 *****************************************************************************************************************/

void LED_9_mode_RGBdataProcess(uint8_t RGB_index)
{
	uint8_t  i, j;
	uint8_t p_row, p_col;
	if(RGB_index == 0)
	{
		if(led_UserConfig[0][0] < LED_MODE9_LOOP_LINE_MAX)
		{
			led_UserConfig[0][0]++;
		}
		//usb_echo("led_UserConfig[0][0] = %d.\r\n",led_UserConfig[0][0]);
		for(j = 0; j < led_UserConfig[0][0]; j++)
		{
			led_ColorIndex[0][j] += 3;
			if(led_ColorIndex[0][j] >= SPECTURN_COLOR_NUM)
			{
				led_ColorIndex[0][j] = 0;
			}
		}
	}

	for(j = 1; j <= (led_UserConfig[0][0]); j++)
	{
		if(j == 1)
		{

			if(RGB_index < led_mode9_loop_len(1))
			{
				p_row = led_mode9_loop1[RGB_index].row;
				p_col = led_mode9_loop1[RGB_index].col;
				led_mode_rgb_set(p_row, p_col, 1,RGB_index);
			}
		}
		else if(j == 2)
		{
			if(RGB_index < led_mode9_loop_len(2))
			{
				p_row = led_mode9_loop2[RGB_index].row;
				p_col = led_mode9_loop2[RGB_index].col;
				led_mode_rgb_set(p_row, p_col, 2,RGB_index);
			}
		}
		else if(j == 3)
		{
			if(RGB_index < led_mode9_loop_len(3))
			{
				p_row = led_mode9_loop3[RGB_index].row;
				p_col = led_mode9_loop3[RGB_index].col;
				led_mode_rgb_set(p_row, p_col, 3,RGB_index);
			}
		}
		else if(j == 4)
		{
			if(RGB_index < led_mode9_loop_len(4))
			{
				p_row = led_mode9_loop4[RGB_index].row;
				p_col = led_mode9_loop4[RGB_index].col;
				led_mode_rgb_set(p_row, p_col, 4,RGB_index);
			}
		}
		else if(j == 5)
		{
			if(RGB_index < led_mode9_loop_len(5))
			{
				p_row = led_mode9_loop5[RGB_index].row;
				p_col = led_mode9_loop5[RGB_index].col;
				led_mode_rgb_set(p_row, p_col, 5,RGB_index);
			}
		}
		else if(j == 6)
		{
			if(RGB_index < led_mode9_loop_len(6))
			{
				p_row = led_mode9_loop6[RGB_index].row;
				p_col = led_mode9_loop6[RGB_index].col;
				led_mode_rgb_set(p_row, p_col, 6,RGB_index);
			}
		}
		else if(j == 7)
		{
			if(RGB_index < led_mode9_loop_len(7))
			{
				p_row = led_mode9_loop7[RGB_index].row;
				p_col = led_mode9_loop7[RGB_index].col;
				led_mode_rgb_set(p_row, p_col, 7,RGB_index);
			}
		}
	}
	//else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI0_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI1_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI3_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI4_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI6_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}
/******************************************************************************************************************
 *Function Name : LED_10_mode_RGBdataInit
 *Description   : 激光
 *****************************************************************************************************************/
void LED_10_mode_RGBdataInit(void)
{
	uint8_t i;
	uint8_t j;
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			//------------------------------------------------------------------------------------------
			/* 亮度初始化 */
			LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
			//------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
			{
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
			}
			else
			{
				/* 光谱循环:不支持颜色设置，颜色初始化 */
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
			}
			//------------------------------------------------------------------------------------------
			led_ColorIndex[i][j] = 0xFF;//记录哪一行被按下
			//------------------------------------------------------------------------------------------
			LED_R_TAG[i][j] = 0x00;//距离左侧还剩多少
			//------------------------------------------------------------------------------------------
			LED_G_TAG[i][j] = 0x00;//距离右侧还剩多少
			//------------------------------------------------------------------------------------------
			LED_B_TAG[i][j] = 0x00;//记录是否到达左边
			//------------------------------------------------------------------------------------------
			led_UserConfig[i][j] = 0x00;//记录是否到达右边
			//------------------------------------------------------------------------------------------
			LED_R_LUM[i][j] = 0x00;
			LED_G_LUM[i][j] = 0x00;
			LED_B_LUM[i][j] = 0x00;
			//------------------------------------------------------------------------------------------
			Current_key_state_LED[i][j] = 0;
			//------------------------------------------------------------------------------------------
		}
	}
	/* 用[1][0]来存这个模式的颜色 */
	if(g_variableData.LED_RGB_MODE)
	{
		LED_R_TAG[1][0] = LightingColorTable[g_variableData.LedColor_Index][0];
		LED_G_TAG[1][0] = LightingColorTable[g_variableData.LedColor_Index][1];
		LED_B_TAG[1][0] = LightingColorTable[g_variableData.LedColor_Index][2];
	}
	else
	{
		LED_R_TAG[1][0] = g_variableData.LED_COLOR[0];
		LED_G_TAG[1][0] = g_variableData.LED_COLOR[1];
		LED_B_TAG[1][0] = g_variableData.LED_COLOR[2];
	}
	//------------------------------------------------------------------------------------------
	switch(g_variableData.LED_SPEED)
	{
		case 0:
			RGB_SpeedLevel_cnt = 850;
			break;
		case 1:
			RGB_SpeedLevel_cnt = 500;
			break;
		case 2:
			RGB_SpeedLevel_cnt = 320;
			break;
		case 3:
			RGB_SpeedLevel_cnt = 220;
			break;
		default:
			break;
	}
	//------------------------------------------------------------------------------------------

}
/******************************************************************************************************************
 *Function Name : LED_10_mode_RGBdataInit
 *Description   : 激光
 *****************************************************************************************************************/

void LED_10_mode_RGBdataProcess(uint8_t RGB_index)
{
	unsigned char n,m,i,j,k,z,x = 0;
	unsigned char addrr=0;
	unsigned char node;
	unsigned char row, col;
	unsigned char p_red, p_green, p_blue;
	if(RGB_index == 0)
	{
		for(i =  0; i < 5; i++)
		{
			if(led_ColorIndex[0][i] != 0xff)
			{
				if((LED_B_TAG[0][i] == 0) || (led_UserConfig[0][i] == 0))
				{
					//点亮左边
					j = LED_R_TAG[0][i];
					if(j > 0 && LED_B_TAG[0][i] == 0)
					{
						//等于0xFF的话，说明右边到头了，左边没有0xFF
						if(((led_position_to_coord[i][j].row == 0xff) && (led_position_to_coord[i][j].col == 0xff)))
						{
							LED_B_TAG[0][i] = 1;
						}
						led_mode_10_rgb_set(i, j);
						if(j)
						{
							j--;
						}
					}
					else if(j == 0)
					{
						//等于0xFF的话，说明右边到头了，左边没有0xFF
						led_mode_10_rgb_set(i, j);

						LED_B_TAG[0][i] = 1;
					}
					//点亮右边
					LED_R_TAG[0][i] = j;

					if(LED_G_TAG[0][i] <= 20 )
					{
						j = LED_G_TAG[0][i];
						if(j < 20 && led_UserConfig[0][i] == 0)
						{
							if((led_position_to_coord[i][j].row == 0xff && led_position_to_coord[i][j].col == 0xff))
							{
								j++;
								if(j == 20)
								{
									led_UserConfig[0][i] = 1;
								}
							}
							led_mode_10_rgb_set(i, j);
							if(j < 20)
							{
								j++;
							}
						}
						else if(j == 20)
						{
							led_mode_10_rgb_set(i, j);

							led_UserConfig[0][i] = 1;
						}
						LED_G_TAG[0][i] = j;
					}

				}
				/* 熄灭 */
				else if(LED_B_TAG[0][i] == 1 || led_UserConfig[0][i] == 1)	//熄灭
				{
					j = LED_R_TAG[0][i + 6];
					if(j > 0  && LED_B_TAG[0][i] == 1)
					{
						if((led_position_to_coord[i][j].row == 0xff && led_position_to_coord[i][j].col == 0xff))
						{
							j--;
							if(!j)
							{
								LED_B_TAG[0][i] = 2;
							}
						}
						led_mode_10_rgb_clear(i, j);
						if(j)
						{
							j--;
						}
					}

					else if(j == 0)
					{
						led_mode_10_rgb_clear(i, j);
						LED_B_TAG[0][i] = 2;
					}
					LED_R_TAG[0][i + 6] = j;
					//右边
					if(LED_G_TAG[0][i + 6] <= 20)
					{
						j = LED_G_TAG[0][i + 6];
						if(j < 20 && led_UserConfig[0][i] == 1)
						{
							if((led_position_to_coord[i][j].row == 0xff && led_position_to_coord[i][j].col == 0xff))
							{
								j++;
								if(j == 20)
								{
									led_UserConfig[0][i] = 2;
								}
							}
							led_mode_10_rgb_clear(i, j);
							if(j < 20)
							{
								j++;
							}
						}
						else if(LED_G_TAG[0][i + 6] == 20)
						{
							led_mode_10_rgb_clear(i, j);

							led_UserConfig[0][i] = 2;
						}
						LED_G_TAG[0][i + 6] = j;
					}

					if(LED_B_TAG[0][i] == 2 && led_UserConfig[0][i] == 2)
					{
						led_ColorIndex[0][i] = 0xff;	//记录哪一行被按下
						LED_R_TAG[0][i]	= 0;			//距离左侧还剩多少
						LED_G_TAG[0][i]	= 0;			//距离右侧还剩多少
						LED_B_TAG[0][i]	= 0;			//记录是否到达左边
						led_UserConfig[0][i] = 0;		//记录是否到达右边
					}

				}
			}
		}
	}
	/* 查哪一组有按键按下 */
	//----------------------------------------------------------------------------------------------
	//Row0
	if(Current_key_state[4][RGB_index])
	{
		if(!Current_key_state_LED[4][RGB_index])
		{
			Current_key_state_LED[4][RGB_index] = Current_key_state[4][RGB_index];
			LED_R_LUM[0][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_R];
			//------------------------------------------------------------------------------------------
			LED_G_LUM[0][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_G];
			//------------------------------------------------------------------------------------------
			LED_B_LUM[0][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_B];

			led_ColorIndex[0][0] = RGB_index;

			LED_R_TAG[0][0] = RGB_index;
			LED_R_TAG[0][0 + 6] = RGB_index;

			LED_G_TAG[0][0] = RGB_index;
			LED_G_TAG[0][0 + 6] = RGB_index;

			LED_B_TAG[0][0] = 0;
			led_UserConfig[0][0] = 0;
		}
	}
	else
	{
		Current_key_state_LED[4][RGB_index] = Current_key_state[4][RGB_index];
	}
	//Row1
	if(Current_key_state[2][RGB_index])
	{
		if(!Current_key_state_LED[2][RGB_index])
		{
			Current_key_state_LED[2][RGB_index] = Current_key_state[2][RGB_index];
			LED_R_LUM[1][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_R];
			//------------------------------------------------------------------------------------------
			LED_G_LUM[1][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_G];
			//------------------------------------------------------------------------------------------
			LED_B_LUM[1][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_B];

			led_ColorIndex[0][1] = RGB_index;


			LED_R_TAG[0][1] = RGB_index;
			LED_R_TAG[0][1 + 6] = RGB_index;

			LED_G_TAG[0][1] = RGB_index;
			LED_G_TAG[0][1 + 6] = RGB_index;

			LED_B_TAG[0][1] = 0;
			led_UserConfig[0][1] = 0;
		}
	}
	else
	{
		Current_key_state_LED[2][RGB_index] = Current_key_state[2][RGB_index];
	}
	//Row2
	if(Current_key_state[3][RGB_index])
	{
		if(!Current_key_state_LED[3][RGB_index])
		{
			Current_key_state_LED[3][RGB_index] = Current_key_state[3][RGB_index];
			LED_R_LUM[2][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_R];
			//------------------------------------------------------------------------------------------
			LED_G_LUM[2][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_G];
			//------------------------------------------------------------------------------------------
			LED_B_LUM[2][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_B];

			led_ColorIndex[0][2] = RGB_index;

			LED_R_TAG[0][2] = RGB_index;
			LED_R_TAG[0][2 + 6] = RGB_index;

			LED_G_TAG[0][2] = RGB_index;
			LED_G_TAG[0][2 + 6] = RGB_index;

			LED_B_TAG[0][2] = 0;
			led_UserConfig[0][2] = 0;
		}
	}
	else
	{
		Current_key_state_LED[3][RGB_index] = Current_key_state[3][RGB_index];
	}
	//Row3
	if(Current_key_state[0][RGB_index])
	{
		if(!Current_key_state_LED[0][RGB_index])
		{
			Current_key_state_LED[0][RGB_index] = Current_key_state[0][RGB_index];
			LED_R_LUM[3][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_R];
			//------------------------------------------------------------------------------------------
			LED_G_LUM[3][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_G];
			//------------------------------------------------------------------------------------------
			LED_B_LUM[3][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_B];

			led_ColorIndex[0][3] = RGB_index;

			LED_R_TAG[0][3] = RGB_index;
			LED_R_TAG[0][3 + 6] = RGB_index;

			LED_G_TAG[0][3] = RGB_index;
			LED_G_TAG[0][3 + 6] = RGB_index;

			LED_B_TAG[0][3] = 0;
			led_UserConfig[0][3] = 0;
		}
	}
	else
	{
		Current_key_state_LED[0][RGB_index] = Current_key_state[0][RGB_index];
	}
	//Row4
	if(Current_key_state[1][RGB_index])
	{
		if(!Current_key_state_LED[1][RGB_index])
		{
			Current_key_state_LED[1][RGB_index] = Current_key_state[1][RGB_index];
			LED_R_LUM[4][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_R];
			//------------------------------------------------------------------------------------------
			LED_G_LUM[4][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_G];
			//------------------------------------------------------------------------------------------
			LED_B_LUM[4][RGB_index] = LightingColorTable[g_variableData.LedColor_Index][RGB_B];

			led_ColorIndex[0][4] = RGB_index;

			LED_R_TAG[0][4] = RGB_index;
			LED_R_TAG[0][4 + 6] = RGB_index;

			LED_G_TAG[0][4] = RGB_index;
			LED_G_TAG[0][4 + 6] = RGB_index;

			LED_B_TAG[0][4] = 0;
			led_UserConfig[0][4] = 0;
		}
	}
	else
	{
		Current_key_state_LED[1][RGB_index] = Current_key_state[1][RGB_index];
	}
	//else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI0_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI0_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI1_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI1_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI3_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI3_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI4_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI4_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
		/* 高亮的时候要常亮白灯 */
		if(LED_HIGHLIGHT_Buffer[SPI6_GROUP][RGB_index] == 0xBF)
		{
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_R[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_G[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
			LED_Buffer_B[SPI6_GROUP][RGB_index] = 0xFF;
			//-----------------------------------------------------------------------------------------------------------------------
		}
		//---------------------------------------------------------------------------------------------------------------------------
	}
}

void LED_20_mode_RGBdataInit(void)
{
	uint8_t i;
	uint8_t j;
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			//------------------------------------------------------------------------------------------
			/* 亮度初始化 */
			LED_Buffer_I[i][j] = LedVolumeTable[g_variableData.LED_BRIGHTNESS];
			//------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
			{
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0xFF;
				//------------------------------------------------------------------------------------------
			}
			else
			{
				/* 光谱循环:不支持颜色设置，颜色初始化 */
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[i][j] = 0x00;
				//------------------------------------------------------------------------------------------
			}
			//------------------------------------------------------------------------------------------
		}
	}
	//------------------------------------------------------------------------------------------
	RGB_SpeedLevel_cnt = 5;
	//------------------------------------------------------------------------------------------
}

void LED_20_mode_RGBdataProcess(uint8_t RGB_index)
{
	uint8_t row;
	uint8_t col;
	if(RGB_index<21)
	{
		if((led_position_to_coord[0][RGB_index].row != 0xFF) || (led_position_to_coord[0][RGB_index].col != 0xFF))
		{
			LED_Buffer_R[led_position_to_coord[0][RGB_index].row][led_position_to_coord[0][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[3*RGB_index+0];
			LED_Buffer_G[led_position_to_coord[0][RGB_index].row][led_position_to_coord[0][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[3*RGB_index+1];
			LED_Buffer_B[led_position_to_coord[0][RGB_index].row][led_position_to_coord[0][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[3*RGB_index+2];
		}

		if((led_position_to_coord[1][RGB_index].row != 0xFF) || (led_position_to_coord[1][RGB_index].col != 0xFF))
		{
			LED_Buffer_R[led_position_to_coord[1][RGB_index].row][led_position_to_coord[1][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[63+3*RGB_index+0];
			LED_Buffer_G[led_position_to_coord[1][RGB_index].row][led_position_to_coord[1][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[63+3*RGB_index+1];
			LED_Buffer_B[led_position_to_coord[1][RGB_index].row][led_position_to_coord[1][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[63+3*RGB_index+2];
		}

		if((led_position_to_coord[2][RGB_index].row != 0xFF) || (led_position_to_coord[2][RGB_index].col != 0xFF))
		{
			LED_Buffer_R[led_position_to_coord[2][RGB_index].row][led_position_to_coord[2][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[126+3*RGB_index+0];
			LED_Buffer_G[led_position_to_coord[2][RGB_index].row][led_position_to_coord[2][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[126+3*RGB_index+1];
			LED_Buffer_B[led_position_to_coord[2][RGB_index].row][led_position_to_coord[2][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[126+3*RGB_index+2];
		}

		if((led_position_to_coord[3][RGB_index].row != 0xFF) || (led_position_to_coord[3][RGB_index].col != 0xFF))
		{
			LED_Buffer_R[led_position_to_coord[3][RGB_index].row][led_position_to_coord[3][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[189+3*RGB_index+0];
			LED_Buffer_G[led_position_to_coord[3][RGB_index].row][led_position_to_coord[3][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[189+3*RGB_index+1];
			LED_Buffer_B[led_position_to_coord[3][RGB_index].row][led_position_to_coord[3][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[189+3*RGB_index+2];
		}

		if((led_position_to_coord[4][RGB_index].row != 0xFF) || (led_position_to_coord[4][RGB_index].col != 0xFF))
		{
			LED_Buffer_R[led_position_to_coord[4][RGB_index].row][led_position_to_coord[4][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[252+3*RGB_index+0];
			LED_Buffer_G[led_position_to_coord[4][RGB_index].row][led_position_to_coord[4][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[252+3*RGB_index+1];
			LED_Buffer_B[led_position_to_coord[4][RGB_index].row][led_position_to_coord[4][RGB_index].col] = g_DrivetData_LED.User_Define_RGB[252+3*RGB_index+2];
		}
		//else
		{
			//---------------------------------------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[SPI0_GROUP][RGB_index] == 0xBF)
			{
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_R[SPI0_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_G[SPI0_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_B[SPI0_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
			}
			//---------------------------------------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[SPI1_GROUP][RGB_index] == 0xBF)
			{
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_R[SPI1_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_G[SPI1_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_B[SPI1_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
			}
			//---------------------------------------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[SPI3_GROUP][RGB_index] == 0xBF)
			{
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_R[SPI3_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_G[SPI3_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_B[SPI3_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
			}
			//---------------------------------------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[SPI4_GROUP][RGB_index] == 0xBF)
			{
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_R[SPI4_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_G[SPI4_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_B[SPI4_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
			}
			//---------------------------------------------------------------------------------------------------------------------------
			/* 高亮的时候要常亮白灯 */
			if(LED_HIGHLIGHT_Buffer[SPI6_GROUP][RGB_index] == 0xBF)
			{
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_R[SPI6_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_G[SPI6_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
				LED_Buffer_B[SPI6_GROUP][RGB_index] = 0xFF;
				//-----------------------------------------------------------------------------------------------------------------------
			}
			//---------------------------------------------------------------------------------------------------------------------------
		}
	}
}
void LED_FN_SPACE_mode_RGBdataInit(void)
{
	uint8_t i;
	uint8_t j;
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
			if(i == 0)
			{
				if((j == 6) || (j == 7) || (j == 8) || (j == 9) || (j == 10))
				{
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					/* 颜色初始化 */
					LED_Buffer_R[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
				}
				else
				{
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					/* 高亮的时候要常亮白灯 */
					if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
					{
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
					}
					else
					{
						/* 光谱循环:不支持颜色设置，颜色初始化 */
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
					}
				}
			}
			//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
			if(i == 1)
			{
				if(j == 5)
				{
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					/* 颜色初始化 */
					LED_Buffer_R[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
				}
				else
				{
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					/* 高亮的时候要常亮白灯 */
					if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
					{
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
					}
					else
					{
						/* 光谱循环:不支持颜色设置，颜色初始化 */
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
					}
				}
			}
			//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
			if(i == 2)
			{
				if((j == 6) || (j == 7) || (j == 8) || (j == 9) || (j == 10) || (j == 11) || (j == 12) || (j == 13))
				{
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					/* 颜色初始化 */
					LED_Buffer_R[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
				}
				else
				{
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					/* 高亮的时候要常亮白灯 */
					if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
					{
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
					}
					else
					{
						/* 光谱循环:不支持颜色设置，颜色初始化 */
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
					}
				}
			}
			//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
			if(i == 3)
			{
				if((j == 6) || (j == 7) || (j == 8) || (j == 9) || (j == 10))
				{
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					/* 颜色初始化 */
					LED_Buffer_R[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
				}
				else
				{
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					/* 高亮的时候要常亮白灯 */
					if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
					{
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
					}
					else
					{
						/* 光谱循环:不支持颜色设置，颜色初始化 */
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
					}
				}
			}
			//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
			if(i == 4)
			{
				if((j == 1) || (j == 2) || (j == 3) || (j == 4) || (j == 5) || (j == 6) || (j == 7) || (j == 8) || (j == 9) || (j == 10) || (j == 11) || (j == 12))
				{
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					/* 颜色初始化 */
					LED_Buffer_R[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
				}
				else
				{
					/* 亮度初始化 */
					LED_Buffer_I[i][j] = 0xFF;
					//------------------------------------------------------------------------------------------
					/* 高亮的时候要常亮白灯 */
					if(LED_HIGHLIGHT_Buffer[i][j] == 0xBF)
					{
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0xFF;
						//------------------------------------------------------------------------------------------
					}
					else
					{
						/* 光谱循环:不支持颜色设置，颜色初始化 */
						//------------------------------------------------------------------------------------------
						LED_Buffer_R[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
						LED_Buffer_G[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
						LED_Buffer_B[i][j] = 0x00;
						//------------------------------------------------------------------------------------------
					}
				}
			}
			//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
		}
	}
	//------------------------------------------------------------------------------------------
	RGB_SpeedLevel_cnt = 5;
	//------------------------------------------------------------------------------------------
}

void LED_Handle(void)
{
	if(Flag_LedMode_RGBUpdate & LED_Update_IRGB)
	{
		if(Flag_FnMode_TurnOnLed_cnt < 10)
		{
			Flag_FnMode_TurnOnLed_cnt++;
		}
		Hall_Update_PWM();
	}
	//高亮
	else if(Flag_LedMode_RGBUpdate & LED_Update_SET_Highlight)
	{
		Tick_10ms_LEDUpdatePWM = 0;
		Set_HighLight_Buffer(1,1,0,0);
		Hall_set_led_current_max();
		//------------------------------------------------------------------------------------------
		Flag_LedMode_RGBUpdate &= (~LED_Update_SET_Highlight);
		//------------------------------------------------------------------------------------------
	}
	//退出高亮
	else if(Flag_LedMode_RGBUpdate & LED_Update_Clear_Highlight)
	{
		Tick_10ms_LEDUpdatePWM = 0;
		Set_HighLight_Buffer(1,0,0,0);
		Hall_set_led_current_max();
		//------------------------------------------------------------------------------------------
		Flag_LedMode_RGBUpdate &= (~LED_Update_Clear_Highlight);
		//------------------------------------------------------------------------------------------
	}
	else
	{
		Tick_10ms_LEDUpdatePWM = 0;
	}
}
void Set_HighLight_Buffer(uint8_t Flag_AllKey, uint8_t Flag_Highlight, uint8_t KeyGroup, uint8_t Keyindex)
{
	uint8_t j;
	if(Flag_AllKey)
	{
		if(Flag_Highlight)
		{
			for(j= 0; j < TRANSFER_SIZE; j++)
			{
				LED_HIGHLIGHT_Buffer[0][j] = 0xBF;//0xBF = LED_MAX_9p0mA | (LED_MAX_9p0mA << 2) | (LED_MAX_9p0mA << 4) | (LED_ENABLE << 7);

				LED_HIGHLIGHT_Buffer[1][j] = 0xBF;//0xBF = LED_MAX_9p0mA | (LED_MAX_9p0mA << 2) | (LED_MAX_9p0mA << 4) | (LED_ENABLE << 7);

				LED_HIGHLIGHT_Buffer[2][j] = 0xBF;//0xBF = LED_MAX_9p0mA | (LED_MAX_9p0mA << 2) | (LED_MAX_9p0mA << 4) | (LED_ENABLE << 7);

				LED_HIGHLIGHT_Buffer[3][j] = 0xBF;//0xBF = LED_MAX_9p0mA | (LED_MAX_9p0mA << 2) | (LED_MAX_9p0mA << 4) | (LED_ENABLE << 7);

				LED_HIGHLIGHT_Buffer[4][j] = 0xBF;//0xBF = LED_MAX_9p0mA | (LED_MAX_9p0mA << 2) | (LED_MAX_9p0mA << 4) | (LED_ENABLE << 7);
			}
		}
		else
		{
			for(j= 0; j < TRANSFER_SIZE; j++)
			{
				LED_HIGHLIGHT_Buffer[0][j] = 0x95;//0x95 = LED_MAX_3p0mA | (LED_MAX_3p0mA << 2) | (LED_MAX_3p0mA << 4) | (LED_ENABLE << 7);

				LED_HIGHLIGHT_Buffer[1][j] = 0x95;//0x95 = LED_MAX_3p0mA | (LED_MAX_3p0mA << 2) | (LED_MAX_3p0mA << 4) | (LED_ENABLE << 7);

				LED_HIGHLIGHT_Buffer[2][j] = 0x95;//0x95 = LED_MAX_3p0mA | (LED_MAX_3p0mA << 2) | (LED_MAX_3p0mA << 4) | (LED_ENABLE << 7);

				LED_HIGHLIGHT_Buffer[3][j] = 0x95;//0x95 = LED_MAX_3p0mA | (LED_MAX_3p0mA << 2) | (LED_MAX_3p0mA << 4) | (LED_ENABLE << 7);

				LED_HIGHLIGHT_Buffer[4][j] = 0x95;//0x95 = LED_MAX_3p0mA | (LED_MAX_3p0mA << 2) | (LED_MAX_3p0mA << 4) | (LED_ENABLE << 7);
			}
			//指示灯保持高亮
			if(sys_led_status_backup & 0x02)
			{
				LED_HIGHLIGHT_Buffer[SPI4_GROUP][0] = 0xBF;
				//------------------------------------------------------------------------------------------
				//Fn space 模式打开的情况下才要设置白光
				if(Flag_FN_SPACE_Status)
				{
					LED_Buffer_R[SPI4_GROUP][0] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[SPI4_GROUP][0] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[SPI4_GROUP][0] = 0xFF;
					//------------------------------------------------------------------------------------------
				}
				//------------------------------------------------------------------------------------------
			}
			//win键保持高亮
			if(DriverAPP_WinLock_Switch)
			{
				LED_HIGHLIGHT_Buffer[SPI1_GROUP][1] = 0xBF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_R[SPI1_GROUP][1] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_G[SPI1_GROUP][1] = 0xFF;
				//------------------------------------------------------------------------------------------
				LED_Buffer_B[SPI1_GROUP][1] = 0xFF;
				//------------------------------------------------------------------------------------------
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		}
	}
	else
	{
		if(Flag_Highlight)
		{
			switch(KeyGroup)
			{
				case SPI0_GROUP:
					LED_HIGHLIGHT_Buffer[SPI0_GROUP][Keyindex] = 0xBF;//0xBF = LED_MAX_9p0mA | (LED_MAX_9p0mA << 2) | (LED_MAX_9p0mA << 4) | (LED_ENABLE << 7);
					//------------------------------------------------------------------------------------------
					LED_Buffer_R[SPI0_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[SPI0_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[SPI0_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					break;
				case SPI1_GROUP:
					LED_HIGHLIGHT_Buffer[SPI1_GROUP][Keyindex] = 0xBF;//0xBF = LED_MAX_9p0mA | (LED_MAX_9p0mA << 2) | (LED_MAX_9p0mA << 4) | (LED_ENABLE << 7);
					//------------------------------------------------------------------------------------------
					LED_Buffer_R[SPI1_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[SPI1_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[SPI1_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					break;
				case SPI3_GROUP:
					LED_HIGHLIGHT_Buffer[SPI3_GROUP][Keyindex] = 0xBF;//0xBF = LED_MAX_9p0mA | (LED_MAX_9p0mA << 2) | (LED_MAX_9p0mA << 4) | (LED_ENABLE << 7);
					//------------------------------------------------------------------------------------------
					LED_Buffer_R[SPI3_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[SPI3_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[SPI3_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					break;
				case SPI4_GROUP:
					LED_HIGHLIGHT_Buffer[SPI4_GROUP][Keyindex] = 0xBF;//0xBF = LED_MAX_9p0mA | (LED_MAX_9p0mA << 2) | (LED_MAX_9p0mA << 4) | (LED_ENABLE << 7);
					//------------------------------------------------------------------------------------------
					LED_Buffer_R[SPI4_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[SPI4_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[SPI4_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					break;
				case SPI6_GROUP:
					LED_HIGHLIGHT_Buffer[SPI6_GROUP][Keyindex] = 0xBF;//0xBF = LED_MAX_9p0mA | (LED_MAX_9p0mA << 2) | (LED_MAX_9p0mA << 4) | (LED_ENABLE << 7);
					//------------------------------------------------------------------------------------------
					LED_Buffer_R[SPI6_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_G[SPI6_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					LED_Buffer_B[SPI6_GROUP][Keyindex] = 0xFF;
					//------------------------------------------------------------------------------------------
					break;
				default:
					break;
			}
		}
		else
		{
			switch(KeyGroup)
			{
				case SPI0_GROUP:
					LED_HIGHLIGHT_Buffer[SPI0_GROUP][Keyindex] = 0x95;//0x95 = LED_MAX_3p0mA | (LED_MAX_3p0mA << 2) | (LED_MAX_3p0mA << 4) | (LED_ENABLE << 7);
					break;
				case SPI1_GROUP:
					LED_HIGHLIGHT_Buffer[SPI1_GROUP][Keyindex] = 0x95;//0x95 = LED_MAX_3p0mA | (LED_MAX_3p0mA << 2) | (LED_MAX_3p0mA << 4) | (LED_ENABLE << 7);
					break;
				case SPI3_GROUP:
					LED_HIGHLIGHT_Buffer[SPI3_GROUP][Keyindex] = 0x95;//0x95 = LED_MAX_3p0mA | (LED_MAX_3p0mA << 2) | (LED_MAX_3p0mA << 4) | (LED_ENABLE << 7);
					break;
				case SPI4_GROUP:
					LED_HIGHLIGHT_Buffer[SPI4_GROUP][Keyindex] = 0x95;//0x95 = LED_MAX_3p0mA | (LED_MAX_3p0mA << 2) | (LED_MAX_3p0mA << 4) | (LED_ENABLE << 7);
					break;
				case SPI6_GROUP:
					LED_HIGHLIGHT_Buffer[SPI6_GROUP][Keyindex] = 0x95;//0x95 = LED_MAX_3p0mA | (LED_MAX_3p0mA << 2) | (LED_MAX_3p0mA << 4) | (LED_ENABLE << 7);
					break;
				default:
					break;
			}
		}
	}
}

void sys_led_scan(void)
{
	if((s_SETREPORT_BUFFER[1] & 0x02) != sys_led_status_backup)
	{
		sys_led_status_backup = s_SETREPORT_BUFFER[1] & 0x02;

		if(sys_led_status_backup)
		{
			Set_HighLight_Buffer(0,1,SPI4_GROUP,0);
			Hall_set_led_current_max();
			Flag_LedMode_RGBUpdate |= LED_Update_IRGB;
		}
		else
		{
			Set_HighLight_Buffer(0,0,SPI4_GROUP,0);
			Hall_set_led_current_max();
			Flag_LedMode_RGBUpdate |= LED_Update_IRGB;
		}
	}
}



