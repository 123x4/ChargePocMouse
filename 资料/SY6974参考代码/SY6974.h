/*
 * SY6974.h
 *
 *  Created on: Apr 30, 2026
 *      Author: GT028615
 */

#ifndef SRC_MISC_SY6974_H_
#define SRC_MISC_SY6974_H_

#include "Tombo80WL.h"

#define SY6974_ADDR		0x6B


int32_t SY6974_Init(void);
void SY6974_PollingTask(void);
int32_t SY6974_FeedDog(void);
int32_t SY6974_Write(uint8_t reg, uint8_t value);
int32_t SY6974_Read(uint8_t reg, uint8_t *pData);

#endif /* SRC_MISC_SY6974_H_ */



