#ifndef __LED_DRIVER__
#define __LED_DRIVER__

extern void Api_Led_Driver_IS31FL3732_Init(void);
extern void wnApi_Led_Driver_AN32180_CommonSettings(void);
extern void wnApi_Led_Driver_AN32180_DriverSettings(void);
extern void wnApi_Led_Driver_AN32180_LED_R(void);
extern void wnApi_Led_Driver_AN32180_LED_G(void);
extern void wnApi_Led_Driver_AN32180_LED_B(void);
extern void wnApi_Led_Driver_AN32180_LED_W(void);
extern void wnApi_Led_Driver_AN32180_LED_BLACK(void);
extern void Every_LED_Open_Test(void);
extern void LED_R(void);
extern void LED_G(void);
extern void LED_B(void);
extern void LED_Blink(void);
extern void LED_Breath(void);
       void LED_Ron(void);
			 void LED_Gon(void);
			 void LED_Bon(void);
			 void LED_RGB(void);
			 void LED_off(void);
extern void LED_flash(void);
extern void DrvDelay(unsigned long DelayData) ;
#endif

