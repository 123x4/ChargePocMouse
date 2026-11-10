//***********************************************************************
//*Writer						ADAM									*
//*Function						TIMER									*
//***********************************************************************
//***********************************************************************
//*								INCLUDE									*
//***********************************************************************
#include "../inc/LPC11Uxx.h"
//#include "../inc/device.h"
//#include "../inc/OSC.h"
#include "../inc/Timer.h"
#include "usbhw.h"
#include "macro.h"
#include "pc_communicate.h"
#include "usbcore.h"
#include "OSC.h"
#include "GPIO.h"
#include "my_tick.h"

//#include "usbcore.h"
//***********************************************************************
//*                             REGISTER                                *
//***********************************************************************
unsigned char TimerCount ;
extern uint8_t Debounce_cnt;
extern UINT8 suspend_UsbActive_timerOut;
uint8_t one_ms_cycle;
uint8_t tm_usb_sof=1;
extern uint8_t Timer_add;
extern UINT16 macro_delayTimer;
extern UINT16 dpitimerdelay;
uint32_t timer16_0_counter;
uint32_t timer16_resume_counter;
extern Profile_Mode_Stru profile_mode;
extern uint16_t profile_timer;
extern BOOL profile_keyflag;
extern uint8_t sensor_timer;
extern UINT16 timer_1s;
//extern BOOL led_pwmData_change;
extern BOOL LED_Test_Flag;
//extern UINT16 led_dpi_timer_count;

//***********************************************************************
//*                             DECLARE                                 *
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Init                            ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void InitCT16B0(unsigned int PR ,unsigned int Time) ;
void InitCT16B1(unsigned int PR ,unsigned int Time) ;
void InitCT32B0(unsigned int PR ,unsigned int Time) ;
void InitCT32B1(unsigned int PR ,unsigned int Time) ;
void USBFS_SOF_ISR_EntryCallback(void);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Drv                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//***********************************************************************
//*                             INTERRUPT_FUNCTION                      *
//***********************************************************************
#if 1
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 TIMER CT16B0 INTERRUPT          ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TIMER16_0_IRQHandler(void)
{

    if ( LPC_CT16B0->IR & (0x1<<0) )
    {
        LPC_CT16B0->IR = (0x01<<0) ;


		MyTick_IncreaseSysTickMs();
		if(Debounce_cnt!=0)
		{
			Debounce_cnt--;
		}	
		if(one_ms_cycle)one_ms_cycle--;

//		if(sensor_timer) sensor_timer--;

		if(g_USBCore_Setting.Enumeration_Finish == TRUE) 
		{
			suspend_UsbActive_timerOut = 5;
		}
		else if(suspend_UsbActive_timerOut)
		{
			suspend_UsbActive_timerOut--;
		}
		if (tm_usb_sof)tm_usb_sof--;
		if(timer_1s)timer_1s--;

//		if(led_dpi_timer_count)
//		{
//			led_dpi_timer_count--;
//			if(!led_dpi_timer_count)
//			{
////				DrvGPIOOutput(0, 19, 1);
////				DrvGPIOOutput(0, 12, 1);
////				DrvGPIOOutput(0, 18, 1);
//			}
//		}
//		if(profile_keyflag)
//		{
//			if(profile_timer)profile_timer--;
//		}
//		if(Timer_add<10)Timer_add++;
		if (macroEvent.type == TIMER)
		{
			if (macro_delayTimer)macro_delayTimer--;
		}
		if (optical_scan_cycle)optical_scan_cycle--;
		if(dpitimerdelay)
		{
			dpitimerdelay--;
		} 
		if(timer16_0_counter)timer16_0_counter--;	
		if( timer16_resume_counter)timer16_resume_counter--;
		
    }
    return ;
}
#endif

void TIMER16_1_IRQHandler(void)
{
    if ( LPC_CT16B1->IR & (0x1<<1) )
    {
		LPC_CT16B1->IR = (0x01<<1) ;
    }
    return ;
}

#if 0
void TIMER32_0_IRQHandler(void)                                     //LED
{

    if ( LPC_CT32B0->IR & (0x1<<0) )
    {
        LPC_CT32B0->IR = (0x01<<0) ;	
	}
    return ;
}
void TIMER32_1_IRQHandler(void)
{
    if ( LPC_CT32B1->IR & (0x1<<0) )
    {
        LPC_CT32B1->IR = (0x01<<0) ;
    }
    return ;
}
#endif 
//***********************************************************************
//*                             SUB_FUNCTION                            *
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Init                            ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//-----InitCT16B0-----
void InitCT16B0(unsigned int PR ,unsigned int Time)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7) ;                           //Set AHB Clock To The CT16B0 Domain
    LPC_CT16B0->PR = PR ;
    LPC_CT16B0->MR0 = Time ;                                        //Set MR0 Data
	LPC_CT16B0->MR1 = Time ;                                        //Set MR1 Data
	LPC_CT16B0->MR2 = Time ;                                        //Set MR2 Data
	LPC_CT16B0->MR3 = Time ;                                        //Set MR3 Data
// 	LPC_CT16B0->PWMC = 0x0F ;                                       //Set PWM Out (BIT0:MAT0 , BIT1:MAT1 , BIT2:MAT2 , BIT3:MAT3)
    LPC_CT16B0->MCR = ( 3 << 0 ) ;                                  //Setup Interrupt Fg & Reset TC
    LPC_CT16B0->TCR = 1 ;
    NVIC_EnableIRQ(TIMER_16_0_IRQn) ;
    return ;
}
void InitCT16B1(unsigned int PR ,unsigned int Time)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8) ;                           //Set AHB Clock To The CT16B1 Domain
	//-----GPIO TO CT16B1-----
// 	LPC_IOCON->PIO0_20 &= ~0x07 ;
// 	LPC_IOCON->PIO0_20 |= 0x01 ;								    //CAP0
// 	LPC_IOCON->PIO0_21 &= ~0x07 ;
// 	LPC_IOCON->PIO0_21 |= 0x01 ;									//MAT0
// 	LPC_IOCON->PIO0_22 &= ~0x07 ;
// 	LPC_IOCON->PIO0_22 |= 0x02 ;								    //MAT1
  
// 	LPC_IOCON->PIO1_18 &= ~0x07 ;
// 	LPC_IOCON->PIO1_18 |= 0x01 ;								    //CAP1
// 	LPC_IOCON->PIO1_23 &= ~0x07 ;
// 	LPC_IOCON->PIO1_23 |= 0x01 ;									//MAT1
	//-----
    LPC_CT16B1->PR = PR ;
    LPC_CT16B1->MR0 = Time ;
	  LPC_CT16B1->MR1 = Time ;
	  LPC_CT16B1->MR2 = Time ;
	  LPC_CT16B1->MR3 = Time ;
// 	LPC_CT16B1->PWMC = 0x0F ;                                       //Set PWM Out (BIT0:MAT0 , BIT1:MAT1 , BIT2:MAT2 , BIT3:MAT3)
    LPC_CT16B1->MCR = ( 3 << 3 ) ;                                  //Setup Interrupt Fg & Reset TC
    LPC_CT16B1->TCR = 1 ;
	NVIC_SetPriority(TIMER_16_1_IRQn, 2);
    NVIC_EnableIRQ(TIMER_16_1_IRQn) ;
    return ;
}
void CT16B1_DelayUs(unsigned short us)
{
	LPC_CT16B1->TCR = 0;//stop timer1
	if(us <= 1)
	{
		return;
	}
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8) ;//Set AHB Clock To The CT16B1 Domain
	LPC_CT16B1->TC = 0;//clear
	LPC_CT16B1->PC = 0;//clear
	LPC_CT16B1->PR = 47 ;//TC will increase by 1 every 1us(1MHz = 48MHz/48).
//	LPC_CT16B1->MR0 = 0xFFFF;
//	LPC_CT16B1->MCR = ( 0x01 << 1 ) ; //enable MR0 match
	LPC_CT16B1->MCR = 0;
	LPC_CT16B1->TCR = 1 ;//start timer1
	if(us <= 2)
	{
		LPC_CT16B1->TCR = 0 ;//stop timer
		return;
	}
	while((LPC_CT16B1->TC) < (us - 2));
	LPC_CT16B1->TCR = 0 ;//stop timer
}
#if 0
void InitCT32B0(unsigned int PR ,unsigned int Time)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9) ;                           //Set AHB Clock To The CT16B0 Domain
	//-----GPIO TO CT32B0-----
// 	LPC_IOCON->PIO0_1 &= ~0x07 ;
// 	LPC_IOCON->PIO0_1 |= 0x02 ;										//MAT2
// 	LPC_IOCON->TDI_PIO0_11 &= ~0x07 ;
// 	LPC_IOCON->TDI_PIO0_11 |= 0x03 ;    							//MAT3
// 	LPC_IOCON->PIO0_17 &= ~0x07 ;
// 	LPC_IOCON->PIO0_17 |= 0x02 ;									//CAP0
// 	LPC_IOCON->PIO0_18 &= ~0x07 ;
// 	LPC_IOCON->PIO0_18 |= 0x02 ;									//MAT0
// 	LPC_IOCON->PIO0_19 &= ~0x07 ;
// 	LPC_IOCON->PIO0_19 |= 0x02 ;									//MAT1

// 	LPC_IOCON->PIO1_24 &= ~0x07 ;
// 	LPC_IOCON->PIO1_24 |= 0x01 ;								    //MAT0
	LPC_IOCON->PIO1_25 &= ~0x07 ;
	LPC_IOCON->PIO1_25 |= 0x01 ;									//MAT1
	LPC_IOCON->PIO1_26 &= ~0x07 ;
	LPC_IOCON->PIO1_26 |= 0x01 ;								    //MAT2
	LPC_IOCON->PIO1_27 &= ~0x07 ;
	LPC_IOCON->PIO1_27 |= 0x01 ;                                    //MAT3
// 	LPC_IOCON->PIO1_28 &= ~0x07 ;
// 	LPC_IOCON->PIO1_28 |= 0x01 ;                                    //CAP0
// 	LPC_IOCON->PIO1_29 &= ~0x07 ;
// 	LPC_IOCON->PIO1_29 |= 0x02 ;                                    //CAP1

	//-----
    LPC_CT32B0->MR0 = Time ;
	LPC_CT32B0->MR1 = 0 ;
	LPC_CT32B0->MR2 = 0 ;
	LPC_CT32B0->MR3 = 0 ;
	LPC_CT32B0->PWMC = 0x0E ;                                       //Set PWM Out (BIT0:MAT0 , BIT1:MAT1 , BIT2:MAT2 , BIT3:MAT3)
    LPC_CT32B0->MCR = (3<<0) ;                                      //Setup Interrupt Fg & Reset TC
    LPC_CT32B0->TCR = 1 ;
    NVIC_EnableIRQ(TIMER_32_0_IRQn) ;
    return ;
}
void InitCT32B1(unsigned int PR ,unsigned int Time)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10) ;							//Set AHB Clock To The CT16B0 Domain
	//-----
// 	LPC_IOCON->TMS_PIO0_12 &= ~0x07 ;
// 	LPC_IOCON->TMS_PIO0_12 |= 0x03 ;                                //CAP0
// 	LPC_IOCON->TDO_PIO0_13 &= ~0x07 ;
// 	LPC_IOCON->TDO_PIO0_13 |= 0x03 ;								//MAT0
// 	LPC_IOCON->TRST_PIO0_14 &= ~0x07 ;
// 	LPC_IOCON->TRST_PIO0_14 |= 0x03 ;								//MAT1
// 	LPC_IOCON->SWDIO_PIO0_15 &= ~0x07 ;
// 	LPC_IOCON->SWDIO_PIO0_15 |= 0x03 ;								//MAT2
// 	LPC_IOCON->PIO0_16 &= ~0x07 ;
// 	LPC_IOCON->PIO0_16 |= 0x02 ;									//MAT3

// 	LPC_IOCON->PIO1_0 &= ~0x07 ;
// 	LPC_IOCON->PIO1_0 |= 0x01 ;								        //MAT0
// 	LPC_IOCON->PIO1_1 &= ~0x07 ;
// 	LPC_IOCON->PIO1_1 |= 0x01 ;									    //MAT1
// 	LPC_IOCON->PIO1_2 &= ~0x07 ;
// 	LPC_IOCON->PIO1_2 |= 0x01 ;								        //MAT2
// 	LPC_IOCON->PIO1_3 &= ~0x07 ;
// 	LPC_IOCON->PIO1_3 |= 0x01 ;                                     //MAT3
// 	LPC_IOCON->PIO1_4 &= ~0x07 ;
// 	LPC_IOCON->PIO1_4 |= 0x01 ;                                     //CAP0
// 	LPC_IOCON->PIO1_5 &= ~0x07 ;
// 	LPC_IOCON->PIO1_5 |= 0x02 ;                                     //CAP1    
	//-----
    LPC_CT32B1->MR0 = Time ;
	LPC_CT32B1->MR1 = Time ;
	LPC_CT32B1->MR2 = Time ;
	LPC_CT32B1->MR3 = Time ;
// 	LPC_CT32B1->PWMC = 0x0F ;                                       //Set PWM Out (BIT0:MAT0 , BIT1:MAT1 , BIT2:MAT2 , BIT3:MAT3)
    LPC_CT32B1->MCR = (3<<9) ;                                      //Setup Interrupt Fg & Reset TC
    LPC_CT32B1->TCR = 1 ;
    NVIC_EnableIRQ(TIMER_32_1_IRQn) ;
    return ;
}
#endif 

void USBFS_SOF_ISR_EntryCallback(void)
{
//	if (optical_scan_cycle)optical_scan_cycle--;
//	if (tm_usb_sof)tm_usb_sof--;
	
}

/* Resets the timer terminal and prescale counts to 0 */
void TIMER_Reset(LPC_CTxxBx_Type *pTMR)
{
	uint32_t reg;

	/* Disable timer, set terminal count to non-0 */
	reg = pTMR->TCR;
	pTMR->TCR = 0;
	pTMR->TC = 1;

	/* Reset timer counter */
	pTMR->TCR = 1<<1;

	/* Wait for terminal count to clear */
	while (pTMR->TC != 0) {}

	/* Restore timer state */
	pTMR->TCR = reg;
}
void TIMER_MatchEnableInt(LPC_CTxxBx_Type *pTMR, int8_t matchnum)
{
	pTMR->MCR |= 1<<((matchnum)*3);
}
void TIMER_ResetOnMatchEnable(LPC_CTxxBx_Type *pTMR, int8_t matchnum)
{
	pTMR->MCR |= 1<<(((matchnum)*3) + 1);
}
void TIMER_PWMEnable(LPC_CTxxBx_Type *pTMR, int8_t pwmnum)
{
	pTMR->PWMC |= 1<< pwmnum;
}
void TIMER_PWMDisable(LPC_CTxxBx_Type *pTMR, int8_t pwmnum)
{
	pTMR->PWMC &= ~(1<< pwmnum);
}
void TIMER_SetMatch(LPC_CTxxBx_Type *pTMR, int8_t matchnum, uint32_t matchval)
{
	pTMR->MR[matchnum] = matchval;
}
void TIMER_Enable(LPC_CTxxBx_Type *pTMR)
{
	pTMR->TCR |= 1<<0;
}
void TIMER_Disable(LPC_CTxxBx_Type *pTMR)
{
	pTMR->TCR &= ~(1<<0);
}
/* Get the Value of Match Register */
uint32_t TIMER_GetMatch(LPC_CTxxBx_Type *pTMR, int8_t matchnum)
{
	return pTMR->MR[matchnum];
}

void PWM_Config(uint32_t pwm_frc )
{
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10) ;   // 1<<10 32B1         //Set AHB Clock To The CT32B1 Domain
	TIMER_Reset( LPC_CT32B1 );
//	LPC_SYSCON->SYSAHBCLKCTRL |= ((1<<6)|(1<<19)|(1<<23)|(1<<24)) ;							//Setup AHB Clock To The GPIO Domain

//	LPC_IOCON->PIO0_16 = ( 0x2<<0 | 0x2<<3 );    
//	LPC_IOCON->TDO_PIO0_13 = ( 0x3<<0 | 0x2<<3 );  
//	LPC_IOCON->TRST_PIO0_14 = ( 0x3<<0 | 0x2<<3 );    

	TIMER_MatchEnableInt(LPC_CT32B1 , 0 );
	TIMER_MatchEnableInt(LPC_CT32B1 , 3 );  	
	TIMER_MatchEnableInt(LPC_CT32B1 , 1 );
		
	TIMER_ResetOnMatchEnable(LPC_CT32B1 , 2);
	
	TIMER_SetMatch(LPC_CT32B1 , 2 , MCU_OSC / pwm_frc );	
	

	
//	TIMER_Enable(LPC_CT32B1);
//	
//	frq = TIMER_GetMatch(LPC_CT32B1, 2);
//	TIMER_SetMatch(LPC_CT32B1, 3, 0);
//	TIMER_SetMatch(LPC_CT32B1, 0, 0);
//	TIMER_SetMatch(LPC_CT32B1, 1, 0);
}

void PWM_Enable(void)
{
		LPC_IOCON->PIO0_16 = ( 0x2<<0 | 0x2<<3 );    
	LPC_IOCON->TDO_PIO0_13 = ( 0x3<<0 | 0x2<<3 );  
	LPC_IOCON->TRST_PIO0_14 = ( 0x3<<0 | 0x2<<3 ); 
	TIMER_PWMEnable(LPC_CT32B1 , 1 );
	TIMER_PWMEnable(LPC_CT32B1 , 3 );
	TIMER_PWMEnable(LPC_CT32B1 , 0 );
	TIMER_Enable(LPC_CT32B1);
}

//void PWM_Disable(void)
//{
//	TIMER_PWMDisable(LPC_CT32B1 , 1 );
//	TIMER_PWMDisable(LPC_CT32B1 , 3 );
//	TIMER_PWMDisable(LPC_CT32B1 , 0 );
//	TIMER_Disable(LPC_CT32B1);
//	LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<10) ;
//	LPC_IOCON->TDO_PIO0_13 &= ~0x07 ;
//	LPC_IOCON->TDO_PIO0_13 |= 0x01 ;								//GPIO
//	LPC_IOCON->TRST_PIO0_14 &= ~0x07 ;
//	LPC_IOCON->TRST_PIO0_14 |= 0x01 ;								//GPIO
//	LPC_IOCON->PIO0_16 &= ~0x07 ;
//	
//	DrvGPIOOutput(0,16,1) ;
//	DrvGPIOOutput(0,13,1) ;
//	DrvGPIOOutput(0,14,1) ;
//}

void PWM_set_duty( RGB_STRU rgb, UINT8 led_mode)
{
	uint32_t frq, pwm_r, pwm_g, pwm_b;

	frq = TIMER_GetMatch(LPC_CT32B1, 2);

	if(led_mode == LED_MODE_BREATH)
	{
			pwm_r = frq * rgb.red * rgb.red / 0xffff;
			if(pwm_r <= rgb.red)pwm_r = rgb.red;

			pwm_g = frq * rgb.green * rgb.green / 0xffff;
			if(pwm_g <= rgb.green)pwm_g = rgb.green;
		
			pwm_b = frq * rgb.blue * rgb.blue / 0xffff;
			if(pwm_b <= rgb.blue)pwm_b = rgb.blue;

	}
	else if (led_mode == LED_MODE_LIGHT_ON)
	{
		pwm_r = frq * rgb.red * rgb.red / 0xffff;
		
		pwm_g = frq * rgb.green * rgb.green / 0xffff;
		
		pwm_b = frq * rgb.blue * rgb.blue / 0xffff;
	}
	else 
	{
		pwm_r = frq * rgb.red / 0xff;
		pwm_g = frq * rgb.green / 0xff;
		pwm_b = frq * rgb.blue / 0xff;
	}
	
	TIMER_SetMatch(LPC_CT32B1, 3, pwm_r);

//	frq = TIMER_GetMatch(LPC_CT32B1, 2);
	TIMER_SetMatch(LPC_CT32B1, 0, pwm_g);

//	frq = TIMER_GetMatch(LPC_CT32B1, 2);
	TIMER_SetMatch(LPC_CT32B1, 1, pwm_b);	
	
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Drv                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//***********************************************************************
//*                             END                                     *
//***********************************************************************

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Drv                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//***********************************************************************
//*                             END                                     *
//***********************************************************************
