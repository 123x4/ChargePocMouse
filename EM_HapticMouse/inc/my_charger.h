/*******************************************************************************
 * @file    my_charger.h
 * @brief   Charger IC BQ21128 (U7 on G2 board, U2 on V0.1). I2C: PIO0_4 = SCL,
 *          PIO0_5 = SDA. /CE: PIO0_1.
 *          Same idea as the SY6974 reference code: change the charge current
 *          according to the battery temperature zone (TS_STAT).
*******************************************************************************/
#ifndef __my_charger_h__
#define __my_charger_h__

#include <stdint.h>

#define CHARGER_ADDR              0x6A    /* BQ21128 7-bit I2C address */

/*------ /CE: confirmed from schematic G2 V0.1 ------
 * Net /CE = BQ21128 pin 14 + MCU pin 3 (PIO0_1) + R50 2.2M to 5V / R51 1M to GND.
 * The push-pull GPIO overrides the weak divider, so the MCU owns /CE.
 * Before Charger_CE_Init(): PIO0_1 is input with the default internal pull-up
 * -> /CE high -> charging disabled until firmware enables it.
 */
#define CHARGER_CE_BY_MCU         1       /* 1: MCU drives PIO0_1 (/CE) low = allow charging */

/*------ battery: Ganfeng 522528 450 mAh, 3.87 V nominal (资料/195000882 电池规格书.pdf) ------
 * Spec 4.2.1 : charge voltage limit 4.45 V (0~45 C), 4.1 V at 45~55 C.
 * Spec 4.2.10: max charge current 90 mA at 0~10 C, 225 mA at 10~15 C,
 *              450 mA at 15~45 C but 225 mA above 4.25 V, 225 mA at 45~55 C.
 * BQ21128 default zones: COOL 0~10, PRECOOL 10~15, NORMAL 15~35,
 *              PREWARM 35~45, WARM 45~60 C.
 * Choice: one charge current for all allowed zones, below every limit;
 * the chip itself cuts it in COOL and stops in WARM (see Charger_ApplyConfig),
 * so the limits hold even if the MCU stops polling.
 */
#define CHARGER_VREG_MV           4450    /* charge voltage, 10 mV step, 3500~4800 */
#define CHARGER_ICHG_MA           200     /* 40 mA step: <= 225 mA; COOL: 40% = 80 mA <= 90 mA */
#define CHARGER_POLL_MS           10000   /* polling period, must be < 50 s (watchdog) */

int32_t Charger_Init(void);
void    Charger_PollingTask(void);
int32_t Charger_Read(uint8_t reg, uint8_t *pData);
int32_t Charger_Write(uint8_t reg, uint8_t value);

#endif /* __my_charger_h__ */
