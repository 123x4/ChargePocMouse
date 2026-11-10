#ifndef _FINGERPRINT_H_
#define _FINGERPRINT_H_

#include "../inc/SPI.h"

#define M4_ID1  (0x4D)
#define M4_ID2  (0x34)

#define	Write_Finger(Addr,Data)		SPI_Write_Finger(Addr,Data)
#define	Read_Finger(Addr)			SPI_Read_Finger(Addr)
void Finger_Check_ID(void);
extern void TimerDelayMsec(uint32_t t_count);
#endif

