/*******************************************************************************
 * @file    my_charger.c
 * @brief   Charger IC BQ21128 (U2) driver.
 *          Init   : PIO0_1 low (/CE), check part info, set charge current.
 *          Polling: every CHARGER_POLL_MS feed the watchdog, read the battery
 *                   temperature zone and set charge enable / current.
*******************************************************************************/
#include "my_charger.h"
#include "LPC11Uxx.h"
#include "my_i2c.h"
#include "my_tick.h"
#include "my_debug.h"

/* registers (BQ21128 datasheet 6.6) */
#define REG_ICHG_L          0x02    /* charge current, 16-bit: 0x02 low byte, 0x03 high byte */
#define REG_VREG_L          0x04    /* charge voltage, 16-bit: 0x04 low byte, 0x05 high byte */
#define REG_CHG_CTRL0       0x16    /* bit5 EN_CHG, bit2 WD_RST, bit1~0 WATCHDOG */
#define REG_NTC_CTRL0       0x1A    /* bit3~2 TS_ISET_WARM, bit1~0 TS_ISET_COOL */
#define REG_CHG_STAT1       0x1E    /* bit4~3 CHG_STAT, bit2~0 VBUS_STAT */
#define REG_FAULT_STAT0     0x1F    /* bit2~0 TS_STAT (battery temperature zone) */
#define REG_PART_INFO       0x38    /* 0x13 = BQ21128 */

#define EN_CHG_BIT          (0x01 << 5)
#define WD_RST_BIT          (0x01 << 2)

/* 0x1A low nibble (table 6-20): WARM = 00 charge suspend, COOL = 10 ICHG 40% */
#define NTC_ISET_MASK       0x0F
#define NTC_ISET_VALUE      0x02

/* TS_STAT values (register 0x1F bit2~0) */
typedef enum {
  TS_NORMAL  = 0,   /* 15 ~ 35 C (default thresholds, 103AT NTC) */
  TS_COLD    = 1,   /* < 0 C (also: TS not powered / no NTC) */
  TS_HOT     = 2,   /* > 60 C */
  TS_COOL    = 3,   /* 0 ~ 10 C */
  TS_WARM    = 4,   /* 45 ~ 60 C */
  TS_PRECOOL = 5,   /* 10 ~ 15 C */
  TS_PREWARM = 6,   /* 35 ~ 45 C */
  TS_FAULT   = 7,
}TsZone_te;

static void    Charger_CE_Init(void);
static int32_t Charger_FeedDog(void);
static int32_t Charger_SetChargeEnable(uint8_t enable);
static int32_t Charger_SetChargeCurrent(uint16_t ma);
static int32_t Charger_SetChargeVoltage(uint16_t mv);
static void    Charger_ApplyConfig(void);
static void    Charger_ThermalManage(TsZone_te zone);


int32_t Charger_Read(uint8_t reg, uint8_t *pData)
{
  /* soft_i2c_read_fast returns 0 on success */
  return soft_i2c_read_fast(&handleI2c1, CHARGER_ADDR, reg, pData, 1) ? -1 : 0;
}

int32_t Charger_Write(uint8_t reg, uint8_t value)
{
  return soft_i2c_write_fast(&handleI2c1, CHARGER_ADDR, reg, &value, 1) ? -1 : 0;
}


int32_t Charger_Init(void)
{
  uint8_t val = 0;

  soft_i2c_init(&handleI2c1);
  Charger_CE_Init();

  if(Charger_Read(REG_PART_INFO, &val))
  {
    InfoLog("charger no ACK");
    return -1;
  }
  InfoLog("charger part info 0x%02X (expect 0x13)", val);

  Charger_ApplyConfig();
  Charger_SetChargeEnable(1);
  return 0;
}


void Charger_PollingTask(void)
{
  uint8_t stat1 = 0;
  uint8_t fault = 0;
  uint8_t part  = 0;
  uint8_t ctrl0 = 0;
  uint8_t ntc0  = 0;
  uint8_t vreg[2] = {0, 0};

  POLLING_INTERVAL_MS(CHARGER_POLL_MS);

  if(Charger_FeedDog())
  {
    InfoLog("charger feed dog fail");
  }

  if(Charger_Read(REG_CHG_STAT1, &stat1) || Charger_Read(REG_FAULT_STAT0, &fault))
  {
    InfoLog("charger read status fail");
    return;
  }
  /* raw values too, so they can be decoded against the datasheet later */
  Charger_Read(REG_PART_INFO, &part);
  Charger_Read(REG_CHG_CTRL0, &ctrl0);
  Charger_Read(REG_NTC_CTRL0, &ntc0);
  soft_i2c_read_fast(&handleI2c1, CHARGER_ADDR, REG_VREG_L, vreg, 2);
  InfoLog("part 0x%02X, 0x16=0x%02X, 0x1A=0x%02X, 0x1E=0x%02X, 0x1F=0x%02X",
          part, ctrl0, ntc0, stat1, fault);
  InfoLog("CHG_STAT %u, VBUS_STAT %u, TS_STAT %u, VREG %u mV",
          (stat1 >> 3) & 0x03, stat1 & 0x07, fault & 0x07,
          ((((uint16_t)vreg[1] << 8) | vreg[0]) >> 3) * 10);

  Charger_ApplyConfig();             /* put settings back if the chip was reset */
  Charger_ThermalManage((TsZone_te)(fault & 0x07));
}


static void Charger_CE_Init(void)
{
#if CHARGER_CE_BY_MCU
  LPC_IOCON->PIO0_1 &= ~0x07;       /* FUNC = 0: GPIO */
  LPC_GPIO->CLR[0] = (0x01 << 1);   /* output value low */
  LPC_GPIO->DIR[0] |= (0x01 << 1);  /* PIO0_1 as output -> /CE low */
#endif
}

static int32_t Charger_FeedDog(void)
{
  uint8_t val = 0;

  if(Charger_Read(REG_CHG_CTRL0, &val))
  {
    return -1;
  }
  return Charger_Write(REG_CHG_CTRL0, val | WD_RST_BIT);
}

/* battery limits that do not depend on the MCU being alive:
 * - 0x1A: COOL -> chip cuts ICHG to 40%, WARM -> chip suspends charging
 * - VREG and ICHG
 * Written at init and every polling period: a watchdog timeout halves ICHG,
 * a chip reset brings all of them back to defaults. */
static void Charger_ApplyConfig(void)
{
  uint8_t val = 0;

  if(0 == Charger_Read(REG_NTC_CTRL0, &val))
  {
    Charger_Write(REG_NTC_CTRL0, (val & ~NTC_ISET_MASK) | NTC_ISET_VALUE);
  }
  Charger_SetChargeVoltage(CHARGER_VREG_MV);
  Charger_SetChargeCurrent(CHARGER_ICHG_MA);
}

/* software decides on/off per zone; the current in COOL is cut by the chip */
static void Charger_ThermalManage(TsZone_te zone)
{
  switch(zone)
  {
    case TS_NORMAL:
    case TS_PRECOOL:
    case TS_PREWARM:
    case TS_COOL:
      Charger_SetChargeEnable(1);
      break;
    case TS_COLD:
    case TS_HOT:
    case TS_WARM:
    case TS_FAULT:
    default:
      Charger_SetChargeEnable(0);   /* stop charging */
      break;
  }
}

static int32_t Charger_SetChargeEnable(uint8_t enable)
{
  uint8_t val = 0;

  if(Charger_Read(REG_CHG_CTRL0, &val))
  {
    return -1;
  }
  if(enable)
  {
    val |= EN_CHG_BIT;
  }
  else
  {
    val &= ~EN_CHG_BIT;
  }
  return Charger_Write(REG_CHG_CTRL0, val);
}

static int32_t Charger_SetChargeCurrent(uint16_t ma)
{
  uint16_t code = ma / 40;          /* 40 mA per step */
  uint16_t reg = 0;
  uint8_t buf[2];

  if(code < 0x01) code = 0x01;      /* 40 mA min */
  if(code > 0x26) code = 0x26;      /* 1520 mA max */
  reg = code << 5;                  /* ICHG is bit11~5 */
  buf[0] = reg & 0xFF;              /* little endian: low byte to 0x02 */
  buf[1] = reg >> 8;                /*                high byte to 0x03 */
  return soft_i2c_write_fast(&handleI2c1, CHARGER_ADDR, REG_ICHG_L, buf, 2) ? -1 : 0;
}

static int32_t Charger_SetChargeVoltage(uint16_t mv)
{
  uint16_t code = mv / 10;          /* 10 mV per step (table 6-8) */
  uint16_t reg = 0;
  uint8_t buf[2];

  if(code < 350) code = 350;        /* 3500 mV min */
  if(code > 480) code = 480;        /* 4800 mV max */
  reg = code << 3;                  /* VREG is bit11~3 */
  buf[0] = reg & 0xFF;              /* little endian: low byte to 0x04 */
  buf[1] = reg >> 8;                /*                high byte to 0x05 */
  return soft_i2c_write_fast(&handleI2c1, CHARGER_ADDR, REG_VREG_L, buf, 2) ? -1 : 0;
}
