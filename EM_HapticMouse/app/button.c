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
#include "my_debug.h"
#include "WWDT.h"

UINT8 hall_spi_tx[4] = {0};
UINT8 hall_spi_rx[4] = {0};

/* H6 self-check ("exams"): results are printed over USB by ExamLog,
 * read them on the PC with tools/hid_log.py. Exam 1 = frequency calibration. */
typedef struct
{
  uint8_t cal_ok;      /* 1 = both chips returned 0x7C..0x84 */
  uint8_t cal_times;   /* number of calibration commands sent */
  uint8_t cal_r;       /* last value from the right-key chip (rx[1]) */
  uint8_t cal_l;       /* last value from the left-key chip (rx[3]) */
  uint8_t cal_rh;      /* high byte from the right-key chip (rx[0]), datasheet: not valid data */
  uint8_t cal_lh;      /* high byte from the left-key chip (rx[2]) */
  uint8_t echo[4];     /* exam 0: loopback pattern read back, expect A5 5A C3 3C */
  uint8_t echo2[4];    /* exam 0: loopback again after a soft reset */
  uint8_t miso_pd, miso_pu;   /* exam 0B: MISO pin level with pull-down / pull-up */
  uint8_t mosi_h, mosi_l;     /* MOSI pin read back while driven high / low */
  uint8_t sck_h, sck_l;       /* SCK pin read back while driven high / low */
  uint8_t sck_in_pu;          /* SCK as input with only the internal pull-up */
  uint8_t sck_off_h, sck_off_in_pu;   /* same SCK checks with the Hall chips powered off */
  uint8_t rd0c_r, rd0c_l;             /* exam 2: register 0x0C read back */
  uint8_t rd0c_rx[4];                 /* exam 2: all 4 bytes of the read-back frame */
  uint8_t wr30_r, wr30_l;             /* exam 3: 0x30 read back after writing 0x21 */
  uint8_t wr30b_r, wr30b_l;           /* exam 3: 0x30 read back after writing 0x20 again */
  uint8_t wr31_r, wr31_l;             /* exam 4: 0x31 read back after writing 0x21 */
  uint8_t wr31b_r, wr31b_l;           /* exam 4: 0x31 read back after writing 0x20 again */
  uint8_t wr31_h1, wr31_h2;           /* exam 4: first byte of the two write frames */
  uint8_t tbl_ok_n, tbl_bad_n;        /* exam 5: init table entries read back equal / different */
  uint8_t tbl_bad_addr, tbl_bad_r, tbl_bad_l;   /* exam 5: first entry that read back different */
  uint8_t seq_r[8], seq_l[8];         /* exam 1 trace: low byte of the first 8 tries */
  uint8_t more_r[8], more_l[8];       /* exam 1 trace: 8 extra commands after the pass */
  uint32_t iocon_mosi, iocon_miso, iocon_sck;   /* raw IOCON values (FUNC, MODE, ADMODE) */
} HallChk_ts;

volatile HallChk_ts g_hall_chk;

/* ---- H6 exam switches: change, rebuild, flash, read the log ---- */
#define HALL_EXAM             5   /* exams run in init: 1 = freq calibrate, 2 = + read reg 0x0C, 3 = + write 0x30, 4 = + write 0x31, 5 = + init table and sampling */
#define HALL_EXAM1_PULSE_US   128 /* calibration pulse: 128 normal; other values = experiment */
#define HALL_FREQ_CAL_TRIM_US 0   /* added to the pulse after a logic-analyzer check: trim = 128 - measured_us */
#define HALL_SCOPE_MODE       0   /* scope mode, no exams: 1 = repeat 128us calibration, 2 = repeat 5us sample command, every ~2 ms */
#define HALL_SCOPE_SECONDS    20  /* scope mode stops after about this long, then the normal flow (and firmware update) runs */
#define HALL_CONVERT_FINE_LOOPS 0 /* sample command fine loops: measured 0=4.87us, 1=5.25, 2=5.62, 3=6.00 (0.375us/loop) */
#define HALL_CONVERT_NOPS     6   /* 0..12 extra NOPs after the loops, about 0.02-0.03us each; target 5.0us, check with scope mode 2 */
#define HALL_EXAM2_PULSE_US   39  /* WR_REG pulse: 39 normal; 50 control test */
#define HALL_EXAM3_BAD_CRC    0   /* 1 = flip CRCWR on purpose (control test): writes must be ignored */
#define HALL_EXAM4_H5_CRC     0   /* 1 = exam 4 uses the old H5 CRCWR (data parity only), control test */
#define HALL_DIAG_PINS        0   /* 1 = also run the old pin / loopback / power-off checks */
#define HALL_EXAM1_TRACE      0   /* 1 = log every calibration reading, and send 8 more after the pass */

#if (6 == USE_HALL_SENSOR)
#define HALL_EXAM1_WIDTH      (DELAY_OUT(HALL_EXAM1_PULSE_US) + (HALL_FREQ_CAL_TRIM_US))   /* 128, trim 0 = TD10_DELAY_US */
#define HALL_EXAM1_WIDTH_US   HALL_EXAM1_PULSE_US
#define HALL_EXAM2_WIDTH      DELAY_OUT(HALL_EXAM2_PULSE_US)   /* 39 gives the same value as TD6_DELAY_US */
#else
#define HALL_EXAM2_WIDTH      HALL_TD_WR_REG                   /* H5: unchanged */
#define HALL_EXAM1_WIDTH      HALL_TD_FREQ_CAL                 /* H5: unchanged */
#define HALL_EXAM1_WIDTH_US   0
#endif

#if (6 == USE_HALL_SENSOR)
/* Exam 5: H6 init table, copied from the XENO reference code (h6_hal.c), except
 * 0x09 (no LED current on the mouse) and 0x33 (chip baseline left off so the
 * samples are raw ADC values for the existing MCU baseline algorithm).
 * Written in this order; 0x33 must be last. */
static const uint8_t h6_init_table[][2] = {
  {0x0C, 0x83},   /* fixed value, datasheet p14; XENO L1221 */
  {0x26, 0x0A},   /* fixed value, p15; XENO L1224 */
  {0x27, 0x05},   /* fixed value, p15; XENO L1225 */
  {0x28, 0x04},   /* fixed value, p15; XENO L1226 */
  {0x13, 0x7F},   /* XENO "typical mode" L1228, not in datasheet */
  {0x15, 0x05},   /* XENO L1229, not in datasheet */
  {0x16, 0x02},   /* XENO L1230, not in datasheet */
  {0x18, 0x03},   /* XENO L1231, not in datasheet */
  {0x1C, 0xFF},   /* XENO L1232, not in datasheet */
  {0x1E, 0x80},   /* XENO L1233, not in datasheet */
  {0x20, 0x80},   /* XENO L1234, not in datasheet */
  {0x25, 0x29},   /* XENO L1235, not in datasheet */
  {0x09, 0x20},   /* bit5=1 -> LNA 24x, LED current 0 (XENO 0x25 adds LED current) p13 */
  {0x0B, 0x88},   /* LNA 24x, PGA 22.8x = XENO default gain POWER_ON_W_GAIN, p13 */
  {0x29, 0x60},   /* LNA 24x pair value, p15; XENO L958 */
  {0x2A, 0x28},   /* LNA 24x pair value, p15; XENO L959 */
  {0x0F, 0x07},   /* sample interval 7 x 4us = 28us, p14; XENO L1285 */
  {0x10, 0x20},   /* interval high bits 0, fixed [7:4]=2, p14; XENO L1286 */
  {0x2F, 0x14},   /* fixed value, p16; XENO L1340 */
  {0x33, 0x80},   /* LAST: continuous conversion, BASL_EN off (XENO writes 0x84), p17 */
};
#define H6_INIT_TABLE_N   (sizeof(h6_init_table) / sizeof(h6_init_table[0]))
#endif

#if HALL_EXAM1_TRACE
/* "R=84 83 -- ..  L=.." for up to 8 bytes; slots >= n print as "--". */
static char *HallChk_Hex8(char *p, volatile uint8_t *v, uint8_t n)
{
  static const char hx[] = "0123456789ABCDEF";
  uint8_t i;
  for(i = 0; i < 8; i++)
  {
    if(i < n) { *p++ = hx[v[i] >> 4]; *p++ = hx[v[i] & 0x0F]; }
    else      { *p++ = '-'; *p++ = '-'; }
    *p++ = ' ';
  }
  *p = 0;
  return p;
}
#endif

static void HallChk_Print(void)
{
  ExamLog("EXAM1 CAL ok=%u n=%u R=%02X:%02X L=%02X:%02X width=%u trim=%d (ok=1: chips converged to 7C..84)",
          (unsigned)g_hall_chk.cal_ok, (unsigned)g_hall_chk.cal_times,
          (unsigned)g_hall_chk.cal_rh, (unsigned)g_hall_chk.cal_r,
          (unsigned)g_hall_chk.cal_lh, (unsigned)g_hall_chk.cal_l,
          (unsigned)HALL_EXAM1_WIDTH_US, (int)HALL_FREQ_CAL_TRIM_US);
#if HALL_EXAM1_TRACE
  {
    char r[25], l[25];
    uint8_t n = (g_hall_chk.cal_times < 8) ? g_hall_chk.cal_times : 8;
    HallChk_Hex8(r, g_hall_chk.seq_r, n);
    HallChk_Hex8(l, g_hall_chk.seq_l, n);
    ExamLog("EXAM1 seq  R=%s L=%s", r, l);
    n = g_hall_chk.cal_ok ? 8 : 0;
    HallChk_Hex8(r, g_hall_chk.more_r, n);
    HallChk_Hex8(l, g_hall_chk.more_l, n);
    ExamLog("EXAM1 more R=%s L=%s", r, l);
  }
#endif
#if (HALL_EXAM >= 2)
  ExamLog("EXAM2 RD 0x0C rx=%02X %02X %02X %02X width=%u (expect R=AA L=AA)",
          (unsigned)g_hall_chk.rd0c_rx[0], (unsigned)g_hall_chk.rd0c_rx[1],
          (unsigned)g_hall_chk.rd0c_rx[2], (unsigned)g_hall_chk.rd0c_rx[3],
          (unsigned)HALL_EXAM2_PULSE_US);
#endif
#if (HALL_EXAM >= 3)
  ExamLog("EXAM3 WR 0x30=21 -> R=%02X L=%02X | =20 -> R=%02X L=%02X crc=%s (expect 21 21 | 20 20)",
          (unsigned)g_hall_chk.wr30_r, (unsigned)g_hall_chk.wr30_l,
          (unsigned)g_hall_chk.wr30b_r, (unsigned)g_hall_chk.wr30b_l,
          HALL_EXAM3_BAD_CRC ? "bad" : "ok");
#endif
#if (HALL_EXAM >= 4)
  ExamLog("EXAM4 WR 0x31=21 head=%02X -> R=%02X L=%02X | =20 head=%02X -> R=%02X L=%02X crc=%s (expect 21 21 | 20 20)",
          (unsigned)g_hall_chk.wr31_h1, (unsigned)g_hall_chk.wr31_r, (unsigned)g_hall_chk.wr31_l,
          (unsigned)g_hall_chk.wr31_h2, (unsigned)g_hall_chk.wr31b_r, (unsigned)g_hall_chk.wr31b_l,
          HALL_EXAM4_H5_CRC ? "h5" : "h6");
#endif
#if ((6 == USE_HALL_SENSOR) && (HALL_EXAM >= 5))
  if(g_hall_chk.tbl_bad_n)
  {
    ExamLog("EXAM5 TBL ok=%u/%u bad=%u first_bad=0x%02X R=%02X L=%02X",
            (unsigned)g_hall_chk.tbl_ok_n, (unsigned)H6_INIT_TABLE_N, (unsigned)g_hall_chk.tbl_bad_n,
            (unsigned)g_hall_chk.tbl_bad_addr, (unsigned)g_hall_chk.tbl_bad_r, (unsigned)g_hall_chk.tbl_bad_l);
  }
  else
  {
    ExamLog("EXAM5 TBL ok=%u/%u bad=0 (expect all ok)",
            (unsigned)g_hall_chk.tbl_ok_n, (unsigned)H6_INIT_TABLE_N);
  }
#endif
#if HALL_DIAG_PINS
  ExamLog("PIN MISO pd=%u pu=%u MOSI h=%u l=%u SCK h=%u l=%u in_pu=%u",
          (unsigned)g_hall_chk.miso_pd, (unsigned)g_hall_chk.miso_pu,
          (unsigned)g_hall_chk.mosi_h, (unsigned)g_hall_chk.mosi_l,
          (unsigned)g_hall_chk.sck_h, (unsigned)g_hall_chk.sck_l,
          (unsigned)g_hall_chk.sck_in_pu);
  ExamLog("SCK pwr-off h=%u in_pu=%u | pwr-on h=%u in_pu=%u",
          (unsigned)g_hall_chk.sck_off_h, (unsigned)g_hall_chk.sck_off_in_pu,
          (unsigned)g_hall_chk.sck_h, (unsigned)g_hall_chk.sck_in_pu);
  ExamLog("IOCON MOSI=%02lX MISO=%02lX SCK=%02lX",
          (unsigned long)g_hall_chk.iocon_mosi, (unsigned long)g_hall_chk.iocon_miso,
          (unsigned long)g_hall_chk.iocon_sck);
  ExamLog("ECHO %02X %02X %02X %02X (expect A5 5A C3 3C)",
          (unsigned)g_hall_chk.echo[0], (unsigned)g_hall_chk.echo[1],
          (unsigned)g_hall_chk.echo[2], (unsigned)g_hall_chk.echo[3]);
  ExamLog("ECHO2 %02X %02X %02X %02X (after soft reset)",
          (unsigned)g_hall_chk.echo2[0], (unsigned)g_hall_chk.echo2[1],
          (unsigned)g_hall_chk.echo2[2], (unsigned)g_hall_chk.echo2[3]);
#endif
}

/* Exam 0: loopback through the daisy chain, no SCK command pulse involved.
 * Each chip is a 16-bit shift register (datasheet 11), so 32 clocks push a
 * pattern through both chips and the next 32 clocks bring it back on MISO.
 * Right result means power, SCK, MOSI, MISO and the H1->H2 link all work. */
static void Hall_LoopbackTest(volatile uint8_t *out)
{
  hall_spi_tx[0] = 0xA5;
  hall_spi_tx[1] = 0x5A;
  hall_spi_tx[2] = 0xC3;
  hall_spi_tx[3] = 0x3C;
  SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);   /* shift the pattern in */

  hall_spi_tx[0] = 0;
  hall_spi_tx[1] = 0;
  hall_spi_tx[2] = 0;
  hall_spi_tx[3] = 0;
  SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);   /* push it out on MISO */

  out[0] = hall_spi_rx[0];
  out[1] = hall_spi_rx[1];
  out[2] = hall_spi_rx[2];
  out[3] = hall_spi_rx[3];
}

/* Multimeter check: 1 = drive SCK (PIO1_15, MCU pin 28) high forever and
 * stop here. Set to 0 to run the normal test flow again. */
#define HALL_SCK_HOLD_HIGH  0

static void Hall_SckHoldHigh(void)
{
  uint32_t reg = LPC_IOCON->PIO1_15;

  LPC_IOCON->PIO1_15 = (reg & ~0x1FUL) | 0x10;   /* GPIO, pull-up */
  LPC_GPIO->DIR[1] |= (1UL << 15);               /* output */
  DrvGPIOOutput(1, 15, 1);                       /* SCK high */
  while (1)
  {
    wwdt_feed();                                 /* watchdog is on (device.c) */
  }
}

/* Exam 0C: SCK with the Hall chips powered off (HALL_EN = PIO0_16 low).
 * SCK is only an input on GTRY164 (datasheet pin table). If SCK can be pulled
 * high now but not when the chips are powered, a powered chip holds it low;
 * if it stays low either way, it is a passive short (solder bridge, damaged
 * input, PCB or MCU pin). */
static void Hall_SckPowerOffTest(void)
{
  uint32_t reg;

  DrvGPIOOutput(HALL_EN_PORT, HALL_EN_PIN, 0);   /* Hall power off */
  CT16B1_DelayUs(50000);                         /* let C26/C51 discharge */

  reg = LPC_IOCON->PIO1_15;
  LPC_IOCON->PIO1_15 = (reg & ~0x1FUL) | 0x10;   /* GPIO, pull-up */
  LPC_GPIO->DIR[1] &= ~(1UL << 15);              /* input */
  CT16B1_DelayUs(20);
  g_hall_chk.sck_off_in_pu = DrvGPIORead(1, 15);
  DrvGPIOOutput(1, 15, 1);
  CT16B1_DelayUs(5);
  g_hall_chk.sck_off_h = DrvGPIORead(1, 15);
  DrvGPIOOutput(1, 15, 0);
  LPC_IOCON->PIO1_15 = reg;

  DrvGPIOOutput(HALL_EN_PORT, HALL_EN_PIN, 1);   /* Hall power on again */
  CT16B1_DelayUs(50000);                         /* let the chips start up */
}

/* Exam 0B: pin check. UM10462 Table 164/165 (PDF p.165): the GPIO PIN register
 * reads the real pin level regardless of the selected function, except that a
 * pin in analog mode (IOCON ADMODE bit7 = 0) always reads 0. */
static void Hall_PinTest(void)
{
  uint32_t reg;

  g_hall_chk.iocon_mosi = LPC_IOCON->PIO0_21;
  g_hall_chk.iocon_miso = LPC_IOCON->PIO0_22;
  g_hall_chk.iocon_sck  = LPC_IOCON->PIO1_15;

  /* MISO P0.22: keep MISO1 function, only switch the pull resistor (MODE bits 4:3) */
  reg = LPC_IOCON->PIO0_22 & ~0x18UL;
  LPC_IOCON->PIO0_22 = reg | 0x08;            /* pull-down */
  CT16B1_DelayUs(20);
  g_hall_chk.miso_pd = DrvGPIORead(0, 22);
  LPC_IOCON->PIO0_22 = reg | 0x10;            /* pull-up, the reset default */
  CT16B1_DelayUs(20);
  g_hall_chk.miso_pu = DrvGPIORead(0, 22);

  /* MOSI P0.21: GPIO for a moment, drive and read back, then MOSI1 again */
  reg = LPC_IOCON->PIO0_21;
  LPC_IOCON->PIO0_21 = reg & ~0x07UL;
  DrvGPIOOutput(0, 21, 1);
  CT16B1_DelayUs(5);
  g_hall_chk.mosi_h = DrvGPIORead(0, 21);
  DrvGPIOOutput(0, 21, 0);
  CT16B1_DelayUs(5);
  g_hall_chk.mosi_l = DrvGPIORead(0, 21);
  LPC_IOCON->PIO0_21 = reg;

  /* SCK P1.15: same method as MOSI (5 us before each read); the high level
   * lasts about 5 us, which the chip would at most take as one sample command. */
  reg = LPC_IOCON->PIO1_15;
  LPC_IOCON->PIO1_15 = (reg & ~0x1FUL) | 0x10;   /* GPIO, pull-up */
  LPC_GPIO->DIR[1] &= ~(1UL << 15);              /* input: only the internal pull-up acts */
  CT16B1_DelayUs(20);
  g_hall_chk.sck_in_pu = DrvGPIORead(1, 15);     /* 0 here = something outside holds SCK low */
  DrvGPIOOutput(1, 15, 1);                       /* output high */
  CT16B1_DelayUs(5);
  g_hall_chk.sck_h = DrvGPIORead(1, 15);
  DrvGPIOOutput(1, 15, 0);                       /* output low */
  CT16B1_DelayUs(10);
  g_hall_chk.sck_l = DrvGPIORead(1, 15);
  LPC_IOCON->PIO1_15 = reg;
}

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
	// LED constant-current range settings
	//{0x18, 0x0A}, 	//disable led
	{0x19, 0xFF},		//LED_Current_1
	{0x1A, 0xFF},		//LED_Current_2
	{0x1B, 0xFF},		//LED_Current_3
	{0x1C, 0xFF},		//LED_Current_4
	// LED constant-current source settings
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

#if ((6 == USE_HALL_SENSOR) && (HALL_EXAM >= 5))
  {
    /* Exam 5: check every sample (2-bit CRC + 14-bit data, datasheet 7.2) and
     * print raw / min / max / ok / err per chip every 500 ms. k=0 right, k=1 left. */
    static uint16_t s_raw[2], s_min[2] = {0xFFFF, 0xFFFF}, s_max[2];
    static uint16_t s_ok[2], s_err[2], s_prt;
    uint8_t k;
    for(k = 0; k < 2; k++)
    {
      uint8_t  hi = hall_spi_rx[2 * k];
      uint8_t  lo = hall_spi_rx[2 * k + 1];
      uint16_t v  = ((uint16_t)(hi & 0x3F) << 8) | lo;
      if(((hi & 0xC0) == (bit_error[hi & 0x3F] ^ bit_error[lo])) && (v != 0))
      {
        s_ok[k]++;
        s_raw[k] = v;
        if(v < s_min[k]) { s_min[k] = v; }
        if(v > s_max[k]) { s_max[k] = v; }
      }
      else
      {
        s_err[k]++;                   /* CRC wrong, or all zero (passes CRC but means no data) */
      }
    }
    if(++s_prt >= 500)
    {
      s_prt = 0;
      ExamLog("EXAM5 R raw=%u min=%u max=%u ok=%u err=%u | L raw=%u min=%u max=%u ok=%u err=%u",
              (unsigned)s_raw[0], (unsigned)s_min[0], (unsigned)s_max[0], (unsigned)s_ok[0], (unsigned)s_err[0],
              (unsigned)s_raw[1], (unsigned)s_min[1], (unsigned)s_max[1], (unsigned)s_ok[1], (unsigned)s_err[1]);
      for(k = 0; k < 2; k++)
      {
        s_min[k] = 0xFFFF; s_max[k] = 0; s_ok[k] = 0; s_err[k] = 0;
      }
    }
  }
#endif

  {
    static uint16_t prt = 0;
    static uint8_t  shown = 0;
    if((shown < 3) && (++prt >= 2000))   /* LR_GetState runs every 1 ms: print at ~2, 4, 6 s, then stop */
    {
      prt = 0;
      shown++;
      HallChk_Print();
    }
  }
  
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


#if (6 == USE_HALL_SENSOR)
/* Short busy loop for sub-microsecond trimming. volatile keeps the compiler
 * from removing it; the real length per loop is measured with a logic analyzer. */
static void Hall_FineDelay(uint8_t loops)
{
	volatile uint8_t n = loops;
	while(n)
	{
		__NOP();
		n--;
	}
}

/* HAL_CONVERT (td1 = 5us +/-5%, datasheet p12). Same steps as Hall_CLK_Command_SET(),
 * but CT16B1_DelayUs only has 1us steps: DelayUs(4) measured 5.29us (too long),
 * DelayUs(3) is about 4.29us (too short). So use DelayUs(3) plus a fine loop. */
static void Hall_Convert_Command(uint8_t loops)
{
	uint32_t reg;

	NVIC_DisableIRQ(USB_IRQn);
	reg = LPC_IOCON->PIO1_15;
	reg &= ~0x07;
	LPC_IOCON->PIO1_15 = reg;                      /* SCK pin as GPIO */
	DrvGPIOOutput(1, 15, 1);
	CT16B1_DelayUs(3);                             /* coarse: about 4.29us with overhead */
	Hall_FineDelay(loops);                         /* fine: bring the pulse to about 5.0us */
	/* finest step: HALL_CONVERT_NOPS single-cycle instructions, unrolled at compile time */
#if (HALL_CONVERT_NOPS > 0)
	__NOP();
#endif
#if (HALL_CONVERT_NOPS > 1)
	__NOP();
#endif
#if (HALL_CONVERT_NOPS > 2)
	__NOP();
#endif
#if (HALL_CONVERT_NOPS > 3)
	__NOP();
#endif
#if (HALL_CONVERT_NOPS > 4)
	__NOP();
#endif
#if (HALL_CONVERT_NOPS > 5)
	__NOP();
#endif
#if (HALL_CONVERT_NOPS > 6)
	__NOP();
#endif
#if (HALL_CONVERT_NOPS > 7)
	__NOP();
#endif
#if (HALL_CONVERT_NOPS > 8)
	__NOP();
#endif
#if (HALL_CONVERT_NOPS > 9)
	__NOP();
#endif
#if (HALL_CONVERT_NOPS > 10)
	__NOP();
#endif
#if (HALL_CONVERT_NOPS > 11)
	__NOP();
#endif
	DrvGPIOOutput(1, 15, 0);
	CT16B1_DelayUs(HALL_TD_CONVERT_WAIT);          /* td2 */
	reg = LPC_IOCON->PIO1_15;
	reg &= ~0x07;
	reg |= 0x03;
	LPC_IOCON->PIO1_15 = reg;                      /* back to SSP1 SCK */
	NVIC_EnableIRQ(USB_IRQn);
}
#endif

void hall_get_sample(void)
{
#if (6 == USE_HALL_SENSOR)
	Hall_Convert_Command(HALL_CONVERT_FINE_LOOPS); /* td1 5us +/-5%, trimmed */
#else
	Hall_CLK_Command_SET(HALL_TD_CONVERT, HALL_TD_CONVERT_WAIT);
#endif

	SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);
}

void HALL_Read_Instru_cnt(void)
{
	Hall_CLK_Command_SET(HALL_EXAM1_WIDTH, HALL_TD_FREQ_WAIT);   /* normally td10 128us */

	SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);
}

#if (6 == USE_HALL_SENSOR)
/* Frequency calibration, following GTRY164 datasheet 9.1 Fig 11.
 * Returns 1 when both chips answer 0x7C..0x84, 0 after 128 failed tries. */
uint8_t Hall_osc_calibrate(void)
{
	uint8_t times;

	g_hall_chk.cal_ok = 0;
	for(times = 0; times < 128; times++)
	{
		/* Datasheet Fig 9: SDI must be 0 while the result is read back,
		 * and SSP sends hall_spi_tx while it receives, so clear all 4 bytes. */
		hall_spi_tx[0] = 0;
		hall_spi_tx[1] = 0;
		hall_spi_tx[2] = 0;
		hall_spi_tx[3] = 0;

		HALL_Read_Instru_cnt();                /* td10 128us pulse, td12 wait, 32 clocks */

		g_hall_chk.cal_r = hall_spi_rx[1];     /* right-key chip, low 8 bits valid */
		g_hall_chk.cal_l = hall_spi_rx[3];     /* left-key chip */
		g_hall_chk.cal_rh = hall_spi_rx[0];    /* high bytes, only logged */
		g_hall_chk.cal_lh = hall_spi_rx[2];
		g_hall_chk.cal_times = times + 1;
#if HALL_EXAM1_TRACE
		if(times < 8)
		{
			g_hall_chk.seq_r[times] = hall_spi_rx[1];
			g_hall_chk.seq_l[times] = hall_spi_rx[3];
		}
#endif

		if((hall_spi_rx[1] >= 0x7C) && (hall_spi_rx[1] <= 0x84) &&
		   (hall_spi_rx[3] >= 0x7C) && (hall_spi_rx[3] <= 0x84))
		{
			g_hall_chk.cal_ok = 1;
#if HALL_EXAM1_TRACE
			{
				uint8_t k;
				for(k = 0; k < 8; k++)             /* experiment: does the reading keep moving? */
				{
					HALL_Read_Instru_cnt();        /* tx is still all 0 */
					g_hall_chk.more_r[k] = hall_spi_rx[1];
					g_hall_chk.more_l[k] = hall_spi_rx[3];
				}
			}
#endif
			CT16B1_DelayUs(20000);             /* Fig 11: delay 20 ms after success */
			return 1;
		}
	}

	return 0;
}
#else
/* GTRY154 (H5): original project code, unchanged by the H6 port.
 * H5 datasheet 9 Fig: send 30 FREQ_Calibrate commands, ignore the data. */
uint8_t Hall_osc_calibrate(void)
{
	uint16_t i, times = 0;

	for(i = 0; i < 2; i++)
	{
		hall_spi_tx[i * 2 + 1] = 0x55;
	}

	while(times < 30)
	{
		HALL_Read_Instru_cnt();
		times++;
		nsAPI_SSP_Delay1us(100);
	}

	return 1;   /* result is not checked here, same as before */
}
#endif
void Hall_read_register(void)
{
	SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);

	Hall_CLK_Command_SET(HALL_EXAM2_WIDTH, HALL_TD_WR_WAIT);   /* td6 39us WR_REG */
	
	SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);
}

void Hall_ReadReg(uint8_t reg, uint8_t *leftValue, uint8_t *rightValue)
{
	hall_spi_tx[0] =  reg & (~(0x01 << 7));
	hall_spi_tx[1] = 0x55;//any value
	hall_spi_tx[2] =  reg & (~(0x01 << 7));
	hall_spi_tx[3] = 0x55;//any value
	
	Hall_read_register();
	
	*rightValue = hall_spi_rx[1];
	*leftValue = hall_spi_rx[3];
}

void Hall_write_register(void)
{
	SSP_Send_Receive(hall_spi_tx, hall_spi_rx, 4);

	Hall_CLK_Command_SET(HALL_TD_WR_REG, HALL_TD_WR_WAIT);
}

#if (6 == USE_HALL_SENSOR)
/* GTRY164 datasheet 7.4 Fig 6: write frame = 1 | CRCWR | A5..A0, then D7..D0.
 * CRCWR = A5^A4^A3^A2^A1^A0^D7^...^D0 (parity of address AND data; the H5
 * table bit_crc_write_reg[] only covers the data). Both chips get the same value. */
static uint8_t Hall_CrcWr(uint8_t addr, uint8_t data)
{
	uint16_t v = ((uint16_t)(addr & 0x3F) << 8) | data;
	uint8_t p = 0;
	while(v)
	{
		p ^= (uint8_t)(v & 1);
		v >>= 1;
	}
#if HALL_EXAM3_BAD_CRC
	p ^= 1;                                       /* control test: wrong on purpose */
#endif
	return p;
}

/* Build the write frame with the given CRCWR bit and send it. Returns the first byte. */
static uint8_t Hall_WriteRegCrc(uint8_t addr, uint8_t data, uint8_t crc)
{
	uint8_t head = 0x80 | (uint8_t)((crc & 1) << 6) | (addr & 0x3F);

	hall_spi_tx[0] = head;
	hall_spi_tx[1] = data;
	hall_spi_tx[2] = head;
	hall_spi_tx[3] = data;
	Hall_write_register();                        /* SSP 4 bytes, then td6 39us + td5 */
	return head;
}

static void Hall_WriteReg(uint8_t addr, uint8_t data)
{
	(void)Hall_WriteRegCrc(addr, data, Hall_CrcWr(addr, data));
}

#if (HALL_EXAM >= 4)
/* Exam 4 CRC: H6 rule (address + data), or the old H5 rule (data only) for the control test. */
static uint8_t Hall_Exam4Crc(uint8_t addr, uint8_t data)
{
#if HALL_EXAM4_H5_CRC
	(void)addr;
	return (uint8_t)(bit_crc_write_reg[data] >> SB);   /* H5 table: data parity at bit6 */
#else
	return Hall_CrcWr(addr, data);
#endif
}
#endif
#endif

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
#if (6 == USE_HALL_SENSOR)
#if HALL_SCOPE_MODE
	/* Scope mode: send exactly the same command as the real code (same function,
	 * same overhead) every ~2 ms, so a logic analyzer on SCK can measure the real
	 * high-pulse width. It stops after about HALL_SCOPE_SECONDS and then falls
	 * through to the normal flow, because the firmware-update command is only
	 * handled by PC_Communicate() in the main loop. */
	{
	uint16_t n;
	for(n = 0; n < (uint16_t)(HALL_SCOPE_SECONDS * 500U); n++)   /* one burst is about 2 ms */
	{
		hall_spi_tx[0] = 0;
		hall_spi_tx[1] = 0;
		hall_spi_tx[2] = 0;
		hall_spi_tx[3] = 0;
#if (1 == HALL_SCOPE_MODE)
		HALL_Read_Instru_cnt();                    /* ~128us high, td12 low, 32 SPI clocks */
#else
		hall_get_sample();                         /* same path as normal sampling: ~5us HAL_CONVERT; changes no register */
#endif
		CT16B1_DelayUs(2000);                      /* idle gap between bursts */
		wwdt_feed();
	}
	}
	/* The chip only received calibration / sample commands; the exams below start again from calibration. */
#endif
	/* Runs exam 1 .. HALL_EXAM (switches near g_hall_chk). The register table is
	 * still the H5 one (H5 addresses), so it must not be written to H6 yet. */
#if HALL_DIAG_PINS
#if HALL_SCK_HOLD_HIGH
	Hall_SckHoldHigh();                               /* never returns */
#endif
	Hall_SckPowerOffTest();                           /* exam 0C */
	Hall_PinTest();                                   /* exam 0B */
	Hall_LoopbackTest(g_hall_chk.echo);               /* exam 0 */
	Hall_CLK_Command_SET(TD11_DELAY_US, TD9_DELAY_US); /* soft reset: td11 >1280us, td9 >50us */
	CT16B1_DelayUs(2000);
	Hall_LoopbackTest(g_hall_chk.echo2);              /* exam 0 again after reset */
#endif
	/* Datasheet 9.1 Fig 11: first step after power-on (HALL_EN in InitGPIO)
	 * is frequency calibration. No soft reset before it. */
	Hall_osc_calibrate();                             /* exam 1 */
#if (HALL_EXAM >= 2)
	if(g_hall_chk.cal_ok)
	{
		uint8_t l, r;
		Hall_ReadReg(0x0C, &l, &r);                   /* exam 2: H6 reset value of 0x0C is 0xAA */
		g_hall_chk.rd0c_r = r;
		g_hall_chk.rd0c_l = l;
		g_hall_chk.rd0c_rx[0] = hall_spi_rx[0];
		g_hall_chk.rd0c_rx[1] = hall_spi_rx[1];
		g_hall_chk.rd0c_rx[2] = hall_spi_rx[2];
		g_hall_chk.rd0c_rx[3] = hall_spi_rx[3];
	}
#endif
#if (HALL_EXAM >= 3)
	if(g_hall_chk.cal_ok && (0xAA == g_hall_chk.rd0c_r) && (0xAA == g_hall_chk.rd0c_l))
	{
		uint8_t l, r;
		Hall_WriteReg(0x30, 0x21);                    /* exam 3: 0x30 default is 0x20 */
		Hall_ReadReg(0x30, &l, &r);
		g_hall_chk.wr30_r = r;
		g_hall_chk.wr30_l = l;
		Hall_WriteReg(0x30, 0x20);                    /* write the default back */
		Hall_ReadReg(0x30, &l, &r);
		g_hall_chk.wr30b_r = r;
		g_hall_chk.wr30b_l = l;
	}
#endif
#if (HALL_EXAM >= 4)
	if((0x21 == g_hall_chk.wr30_r) && (0x21 == g_hall_chk.wr30_l) &&
	   (0x20 == g_hall_chk.wr30b_r) && (0x20 == g_hall_chk.wr30b_l))
	{
		uint8_t l, r;
		/* exam 4: 0x31 = 110001 has an odd number of 1s, so the H6 and H5 CRC rules differ */
		g_hall_chk.wr31_h1 = Hall_WriteRegCrc(0x31, 0x21, Hall_Exam4Crc(0x31, 0x21));
		Hall_ReadReg(0x31, &l, &r);
		g_hall_chk.wr31_r = r;
		g_hall_chk.wr31_l = l;
		g_hall_chk.wr31_h2 = Hall_WriteRegCrc(0x31, 0x20, Hall_Exam4Crc(0x31, 0x20));   /* default value */
		Hall_ReadReg(0x31, &l, &r);
		g_hall_chk.wr31b_r = r;
		g_hall_chk.wr31b_l = l;
	}
#endif
#if (HALL_EXAM >= 5)
	if(g_hall_chk.cal_ok && (0xAA == g_hall_chk.rd0c_r) && (0xAA == g_hall_chk.rd0c_l))
	{
		uint8_t i, l, r;
		for(i = 0; i < H6_INIT_TABLE_N; i++)          /* exam 5: write the init table */
		{
			Hall_WriteReg(h6_init_table[i][0], h6_init_table[i][1]);
		}
		CT16B1_DelayUs(100);                          /* p17: wait >50us after switching conversion mode */
		g_hall_chk.tbl_ok_n = 0;
		g_hall_chk.tbl_bad_n = 0;
		for(i = 0; i < H6_INIT_TABLE_N; i++)          /* read every entry back; do not stop on a mismatch */
		{
			Hall_ReadReg(h6_init_table[i][0], &l, &r);
			if((l == h6_init_table[i][1]) && (r == h6_init_table[i][1]))
			{
				g_hall_chk.tbl_ok_n++;
			}
			else
			{
				if(0 == g_hall_chk.tbl_bad_n)
				{
					g_hall_chk.tbl_bad_addr = h6_init_table[i][0];
					g_hall_chk.tbl_bad_r = r;
					g_hall_chk.tbl_bad_l = l;
				}
				g_hall_chk.tbl_bad_n++;
			}
		}
	}
#endif
#else
	Hall_osc_calibrate();

	ana_hall2_phy_set_mode(2);
#endif
}
