#ifndef _BUTTON_H_
#define _BUTTON_H_

//#include "platform.h"
#include "../inc/LPC11Uxx.h"
#include "device.h"
//#include "gloab.h"
#include "test.h"
#include "my_board.h"
    
#define Hall_sensor_Write 		0x80
#define REG_CNT					25

/***DrvGPIOOutput() is expected to take 1 microsecond. */
#define  DELAY_OUT(x)         (x - 1)
#define USE_HALL_SENSOR       5//GTRY12: 2, GTRY15: 5, GTRY16: 6.
#if (2 == USE_HALL_SENSOR)
#define  TD1_DELAY_US    							(DELAY_OUT(5))		//(5.5)+-5%
#define  TD2_CONTIUNS_DELAY_US 	 			(DELAY_OUT(10)) 		//>5
#define  TD2_SIGNLE_DELAY_US    			(DELAY_OUT(65)) 	//>60
#define  TD3_DELAY_US    							(DELAY_OUT(8))		//(8.5)+-5%
#define  TD4_DELAY_US    							(DELAY_OUT(13))		//(13)+-5%
#define  TD5_DELAY_US    							(DELAY_OUT(10)) 	//>5
//#define  TD6_DELAY_US    							(DELAY_OUT(39)) //datasheet no define. (39)+-5%
#define  TD7_DELAY_US    							(DELAY_OUT(66))	//(66)+-5%
//#define  TD8_DELAY_US    							(DELAY_OUT(392))  //datasheet no define. (392)+-5%
#define  TD9_DELAY_US    							(DELAY_OUT(500)) //(500)+-5%
#define  TD10_DELAY_US   							(DELAY_OUT(10))  //>6
#define  TD11_DELAY_US    						(DELAY_OUT(29))   //(29)+-5%
#define  TD12_DELAY_US    						(DELAY_OUT(5)) 		//>2
#define  TD13_DELAY_US   							(DELAY_OUT(15))  	//>12
#define  TD14_DELAY_US    						(DELAY_OUT(1000)) //(1000)+-5%
#elif (5 == USE_HALL_SENSOR)
#define  TD1_DELAY_US    							(DELAY_OUT(5))		//5+-5%
#define  TD2_CONTIUNS_DELAY_US 	 			(DELAY_OUT(5)) 		//>2
#define  TD2_SIGNLE_DELAY_US    			(DELAY_OUT(85)) 	//>80
#define  TD3_DELAY_US    							(DELAY_OUT(9))		//(9)+-5%
#define  TD4_DELAY_US    							(DELAY_OUT(24))		//(24)+-5%
#define  TD5_DELAY_US    							(DELAY_OUT(5)) 	//>2
//#define  TD6_DELAY_US    							(DELAY_OUT(39))   //datasheet no define.(39)+-5%
#define  TD7_DELAY_US    							(DELAY_OUT(128))	//(128)+-5%
//#define  TD8_DELAY_US    							(DELAY_OUT(392))  //datasheet no define.(392)+-5%
//#define  TD9_DELAY_US    							(DELAY_OUT(500)) //datasheet no define.
#define  TD10_DELAY_US   							(DELAY_OUT(5))  	//>2
#define  TD11_DELAY_US    						(DELAY_OUT(15))   //15+-5%
#define  TD12_DELAY_US    						(DELAY_OUT(5)) 		//>2
#define  TD13_DELAY_US   							(DELAY_OUT(15))  	//>12
#elif (6 == USE_HALL_SENSOR)
#define  TD1_DELAY_US    							(DELAY_OUT(5))		//5+-5%
#define  TD2_CONTIUNS_DELAY_US 	 			(DELAY_OUT(6)) 		//>4
#define  TD2_SIGNLE_DELAY_US    			(DELAY_OUT(30)) 	//>24
#define  TD3_DELAY_US    							(DELAY_OUT(12))		//(12)+-5%
#define  TD4_DELAY_US    							(DELAY_OUT(18))		//(18)+-5%
#define  TD5_DELAY_US    							(DELAY_OUT(5)) 	//>3
#define  TD6_DELAY_US    							(DELAY_OUT(39))   //(39)+-5%
#define  TD7_DELAY_US    							(DELAY_OUT(28))	//(28)+-5%
#define  TD8_DELAY_US    							(DELAY_OUT(2))  //(2)+-5%
#define  TD9_DELAY_US    							(DELAY_OUT(55)) //>50
#define  TD10_DELAY_US   							(DELAY_OUT(128))  	//128+-5%
#define  TD11_DELAY_US    						(DELAY_OUT(2000))   //2000+-5%
#define  TD12_DELAY_US    						(DELAY_OUT(55)) 		//>50
#endif

#define CRC_ERROR 0xffff
/*Macros defining button position for all buttons of Touch Mouse RDK */ 
//#define DPI_BUTTON_POSITION					(13u) //Port0    (1) /* Port0*/
//#define DPI_UP_BUTTON_POSITION      (1)  //port0
//#define DPI_DOWN_BUTTON_POSITION    (18)  //
//#define PROFILE_BUTTON_POSITION     (19u) // Port1 (5) /* Port3 */
#define DPI_CYCLE_BUTTON_POSITION    (DPI_BUTTON_PIN)  //
#define MIDDLE_BUTTON_POSITION      (MIDDLE_BUTTON_PIN)
#define RIGHT_BUTTON_POSITION       (17u) //port0   (3) /* Port3 */
#define LEFT_BUTTON_POSITION        (7u) //Port0  (4) /* Port2*/
#define VOLUME_UP_BUTTON_POSITION   (18u)
#define VOLUME_DOWN_BUTTON_POSITION   (19u)
#define K4_BUTTON_POSITION          (FRONT_BUTTON_PIN) //Port0 (6) /* Port3 */
#define K5_BUTTON_POSITION					(BACK_BUTTON_PIN) //Port0 (7) /* Port3*/





/*Macros defining button mask for all buttons of Touch Mouse RDK */    
#define LEFT_BUTTON_MASK                (1u<<LEFT_BUTTON_POSITION)
#define K5_BUTTON_MASK                  (1u<<K5_BUTTON_POSITION)
#define K4_BUTTON_MASK                  (1u<<K4_BUTTON_POSITION)
#define VOLUME_UP_BUTTON_MASK           (1u<<VOLUME_UP_BUTTON_POSITION)
#define VOLUME_DOWN_BUTTON_MASK         (1u<<VOLUME_DOWN_BUTTON_POSITION)
#define DPI_CYCLE_BUTTON_MASK           (1u<<DPI_CYCLE_BUTTON_POSITION)
#define MIDDLE_BUTTON_MASK              (1u<<MIDDLE_BUTTON_POSITION)
#define RIGHT_BUTTON_MASK               (1u<<RIGHT_BUTTON_POSITION)
//#define PROFILE_BUTTON_MASK				      (1u<<PROFILE_BUTTON_POSITION) 


#define BUTTON_PROCESS_SCAN		0
#define BUTTON_PROCESS_ANALYSE  1

#define BUTTON_DEBOUNCE_COUNT   2

#define LBFT_BUTTON_REPORT_BIT		0u
#define RIGHT_BUTTON_REPORT_BIT		1u
#define MIDDLE_BUTTON_REPORT_BIT	2u
#define K4_BUTTON_REPORT_BIT		5u
#define K5_BUTTON_REPORT_BIT		6u
#define VOLUME_UP_BUTTON_REPORT_BIT  7u
#define VOLUME_DOWN_BUTTON_REPORT_BIT 8u
#define DPI_CYCLE_BUTTON_REPORT_BIT	9u

#define LBFT_BUTTON_REPORT_BIT_MASK		(1u << LBFT_BUTTON_REPORT_BIT)
#define RIGHT_BUTTON_REPORT_BIT_MASK	(1u << RIGHT_BUTTON_REPORT_BIT)
#define MIDDLE_BUTTON_REPORT_BIT_MASK	(1u << MIDDLE_BUTTON_REPORT_BIT)
#define K4_BUTTON_REPORT_BIT_MASK		  (1u << K4_BUTTON_REPORT_BIT)
#define K5_BUTTON_REPORT_BIT_MASK		  (1u << K5_BUTTON_REPORT_BIT)
#define VOLUME_UP_BUTTON_REPORT_BIT_MASK  (1u<< VOLUME_UP_BUTTON_REPORT_BIT)
#define VOLUME_DOWN_BUTTON_REPORT_BIT_MASK (1u<<VOLUME_DOWN_BUTTON_REPORT_BIT)
#define DPI_CYCLE_BUTTON_REPORT_BIT_MASK	(1u<<DPI_CYCLE_BUTTON_REPORT_BIT)


#define ALL_BUTTON_REPORT_BIT ((1u << LBFT_BUTTON_REPORT_BIT) |\
	                          (1u << RIGHT_BUTTON_REPORT_BIT) |\
							  (1u << MIDDLE_BUTTON_REPORT_BIT) |\
							  (1u << K4_BUTTON_REPORT_BIT) |\
							  (1u << K5_BUTTON_REPORT_BIT) |\
								(1u << VOLUME_UP_BUTTON_REPORT_BIT)|\
								(1u << VOLUME_DOWN_BUTTON_REPORT_BIT) |\
							  (1u << DPI_CYCLE_BUTTON_REPORT_BIT))


#define LEFT_BUTTON_OFFSET		(LEFT_BUTTON_POSITION - LBFT_BUTTON_REPORT_BIT)
#define RIGHT_BUTTON_OFFSET		(RIGHT_BUTTON_POSITION - RIGHT_BUTTON_REPORT_BIT)
#define MIDDLE_BUTTON_OFFSET	(MIDDLE_BUTTON_POSITION - MIDDLE_BUTTON_REPORT_BIT)
#define K4_BUTTON_OFFSET		  (K4_BUTTON_POSITION - K4_BUTTON_REPORT_BIT)
#define K5_BUTTON_OFFSET		  (K5_BUTTON_POSITION - K5_BUTTON_REPORT_BIT)
#define DPI_BUTTON_OFFSET		  (DPI_BUTTON_POSITION - DPI_BUTTON_REPORT_BIT)
#define PROFILE_BUTTON_OFFSET	(PROFILE_BUTTON_POSITION - PROFILE_BUTTON_REPORT_BIT)

//#define BUTTON_PORT2 ((~Port2_Pins_PS) & (RIGHT_BUTTON_MASK| MIDDLE_BUTTON_MASK))
//#define BUTTON_PORT3 ((~Port3_Pins_PS )& (PROFILE_BUTTON_MASK | LEFT_BUTTON_MASK |K4_BUTTON_MASK |K5_BUTTON_MASK  ))
//#define BUTTON_PORT0 ((~Port0_Pins_PS )& (DPI_BUTTON_MASK ))

//#define BUTTON_PORT2 ((~Port2_Pins_PS) & (RIGHT_BUTTON_MASK| MIDDLE_BUTTON_MASK))
//#define BUTTON_PORT1 (U32)((~LPC_GPIO->PIN[1])& (RIGHT_BUTTON_MASK |PROFILE_BUTTON_MASK | K5_BUTTON_MASK))

/*
#define BUTTON_PORT0 (U32) ((~LPC_GPIO->PIN[0])& (LEFT_BUTTON_MASK | RIGHT_BUTTON_MASK | MIDDLE_BUTTON_MASK |K4_BUTTON_MASK |\
                                                   K5_BUTTON_MASK |VOLUME_UP_BUTTON_MASK | VOLUME_DOWN_BUTTON_MASK |DPI_CYCLE_BUTTON_MASK ))
*/
#if (defined(HAPTIC_V2) || defined (HAPTIC_V3) || defined (HAPTIC_V4) || defined (IRPT_V4) || defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))
#define BUTTON_PORT0 (U32)((~LPC_GPIO->PIN[0])& (MIDDLE_BUTTON_MASK | DPI_CYCLE_BUTTON_MASK | K5_BUTTON_MASK | K4_BUTTON_MASK))
#elif (defined(P723_POC))
#define BUTTON_PORT0 (U32)((~LPC_GPIO->PIN[0])& (MIDDLE_BUTTON_MASK | DPI_CYCLE_BUTTON_MASK | K5_BUTTON_MASK))
#define BUTTON_PORT1 (U32)((~LPC_GPIO->PIN[1])& (K4_BUTTON_MASK))
#elif (defined(GEN_2_H_V1))
#define BUTTON_PORT0 (U32)((~LPC_GPIO->PIN[0])& (MIDDLE_BUTTON_MASK | DPI_CYCLE_BUTTON_MASK | K4_BUTTON_MASK))
#define BUTTON_PORT1 (U32)((~LPC_GPIO->PIN[1])& (K5_BUTTON_MASK))
#else
  #error "Hardware information is undefined."
#endif


typedef struct
{
	UINT8 process_state;
	UINT32 scan_result_cur;
	UINT32	scan_result_pre;
	UINT8  debounce[6];
	BOOL button_changed;
}BUTTON_SCAN_DATA;

extern BUTTON_SCAN_DATA buttonScan_stru;

void Hall_ReadReg(uint8_t reg, uint8_t *leftValue, uint8_t *rightValue);

UINT32 LR_GetState(void);
void hall_get_sample(void);
void Hall_osc_calibrate(void);
void HallSensor_INIT(void);
void Button_Init(void);

BOOL Button_Scan(void);
UINT32 Button_GetState(void);

#endif /* _BUTTON_H_ */
