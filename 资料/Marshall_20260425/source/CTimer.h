/*
 * ctimer.h
 *
 *  Created on: 2023年7月30日
 *      Author: Hao.Luo
 */

#ifndef CTIMER_H_
#define CTIMER_H_
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CTIMER          CTIMER2         /* Timer 2 */
#define CTIMER_MAT0_OUT kCTIMER_Match_1 /* Match output 1 */
#define CTIMER_CLK_FREQ CLOCK_GetCTimerClkFreq(2U)
/*******************************************************************************
 * Variable
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/
void CTimer_Init(void);

void CTimer_Start(void);

void CTimer_Stop(void);

#endif /* CTIMER_H_ */
