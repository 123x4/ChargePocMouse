//***********************************************************************
//*Writer						ADAM									*
//*Function					WWDT									*
//***********************************************************************
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************
#include "../inc/LPC11Uxx.h"
#include "../inc/Main.h"
#include "../inc/OSC.h"
#include "../inc/WWDT.h"


/* Inprecise clock rates for the watchdog oscillator */
uint32_t wdtOSCRate[] =
{
		0,					/* WDT_OSC_ILLEGAL */
		600000,				/* WDT_OSC_0_60 */
		1050000,			/* WDT_OSC_1_05 */
		1400000,			/* WDT_OSC_1_40 */
		1750000,			/* WDT_OSC_1_75 */
		2100000,			/* WDT_OSC_2_10 */
		2400000,			/* WDT_OSC_2_40 */
		2700000,			/* WDT_OSC_2_70 */
		3000000,			/* WDT_OSC_3_00 */
		3250000,			/* WDT_OSC_3_25 */
		3500000,			/* WDT_OSC_3_50 */
		3750000,			/* WDT_OSC_3_75 */
		4000000,			/* WDT_OSC_4_00 */
		4200000,			/* WDT_OSC_4_20 */
		4400000,			/* WDT_OSC_4_40 */
		4600000				/* WDT_OSC_4_60 */
};


void WDT_IRQHandler(void)
{
		wwdt_feed();
}


// Feed watchdog timer
void wwdt_feed(void)
{
		LPC_WWDT->FEED = 0xAA;
		LPC_WWDT->FEED = 0x55;
}


// Config the Watchdog timer
void wwdt_config(uint32_t wwdt_time)	// set wwdt_time as 1 means 1ms
{
		uint32_t wdtFreq;
		uint32_t wdt_div;
		uint8_t clk;
		uint32_t WDTLFORate;

		LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 15);	// Clock to WWDT
		LPC_SYSCON->PDRUNCFG &= ~(1 << 6);			// Enable the WWDT clock source
		LPC_SYSCON->WDTOSCCTRL  = ((0x2) << 5) | ((20 >> 1) - 1);// Uses the watchdog oscillator set at a 50KHz (1Mhz / 20) clock rate.
		LPC_SYSCON->STARTERP1 &= ~(1 << 12);		// Enable the watchdog interrupt

		/* Disable watchdog */
		LPC_WWDT->MOD = 0;
		LPC_WWDT->TC = 0xFF;
		LPC_WWDT->WARNINT = 0xFFFF;
		LPC_WWDT->WINDOW = 0xFFFFFF;

		/* The WDT divides the input frequency into it by 4 */
		clk = ((LPC_SYSCON->WDTOSCCTRL >> 5) & 0xF);	// Get WDT oscillator settings
		wdt_div = LPC_SYSCON->WDTOSCCTRL & 0x1F;
		WDTLFORate = wdtOSCRate[clk] / ((wdt_div + 1) << 1);	// Compute clock rate and divided by divde value
		wdtFreq = WDTLFORate / 4;

		/* Select watchdog oscillator for WDT clock source */
		LPC_WWDT->CLKSEL = 0x00000001;

		/* Set watchdog feed time constant to approximately time_out ms
			Set watchdog warning time to 512 ticks after feed time constant */
		LPC_WWDT->TC = (wdtFreq / 1000) * wwdt_time;
		LPC_WWDT->WARNINT = 0;

		/* Configure WWDT to reset on timeout */
		LPC_WWDT->MOD |= 1 << 1;

		/* Clear watchdog warning and timeout interrupts */
		LPC_WWDT->MOD &= (~((uint32_t) (1 << 2))) & ((uint32_t) 0x1F);
		LPC_WWDT->MOD |= ((uint32_t) (1 << 3));

		/* Clear and enable watchdog interrupt */
		NVIC_ClearPendingIRQ(WDT_IRQn);
		NVIC_EnableIRQ(WDT_IRQn);

		/* Start watchdog */
		LPC_WWDT->MOD |= ((uint32_t) (1 << 0));
		wwdt_feed();
}


// Close watchdog timer
void wwdt_close()
{
//		LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 15);
		LPC_SYSCON->PDRUNCFG |= (1 << 6);			// Disable the WWDT clock source
}

