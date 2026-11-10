/*******************************************************************************
 * @file    my_charger.h
 * @brief   Charger IC BQ21128 (U2). I2C: PIO0_4 = SCL, PIO0_5 = SDA. /CE: PIO0_1.
 *          Same idea as the SY6974 reference code: change the charge current
 *          according to the battery temperature zone (TS_STAT).
*******************************************************************************/
#ifndef __my_charger_h__
#define __my_charger_h__

#include <stdint.h>

#define CHARGER_ADDR              0x6A    /* BQ21128 7-bit I2C address */

/*------ settings: change these after confirming with hardware / battery spec ------*/
#define CHARGER_CE_BY_MCU         1       /* 1: MCU drives PIO0_1 (/CE) low = allow charging */
#define CHARGER_ICHG_NORMAL_MA    320     /* charge current at 10~45 C. 40 mA step, 40~1520 */
#define CHARGER_ICHG_COOL_MA      120     /* charge current at 0~10 C */
#define CHARGER_POLL_MS           10000   /* polling period, must be < 50 s (watchdog) */

int32_t Charger_Init(void);
void    Charger_PollingTask(void);
int32_t Charger_Read(uint8_t reg, uint8_t *pData);
int32_t Charger_Write(uint8_t reg, uint8_t value);

#endif /* __my_charger_h__ */
