/*
 * SY6974.c
 *
 *  Created on: Apr 30, 2026
 *      Author: GT028615
 */


#include "SY6974.h"

typedef enum{
	NTC_NORMAL = 0,		// 10C ~ 45C
	NTC_WARM = 0B10,	// 45C ~ 60C
	NTC_COOL = 0B11,	// 0C ~ 10C
	NTC_COLD = 0B101,	// < 0C
	NTC_HOT = 0B110,	// > 60C
}NTC_FaultStatus_te;

typedef enum {
	ICHG_0MA = 0,		//disable charge
	ICHG_600MA = 10,
	ICHG_660MA = 11,
	ICHG_1320MA = 22,
}FastChargeCurrent_te;


static int32_t SY6974_ThermalManage(NTC_FaultStatus_te ntcStatus);
//static int32_t SY6974_SearchOffset(uint8_t regNum, uint8_t *pRowOffset);
static int32_t SY6974_SetChargeEnable(uint8_t enable);
static int32_t SY6974_SetFastChargeCurrent(FastChargeCurrent_te ma);

uint8_t regTable[][2] =
{
	{0x00, 19},
	{0x01, 0x9A},
	{0x02, 0xD6},
	{0x03, 0x11},
	{0x04, 0x88},
	{0x05, 0x9F},
	{0x06, 0x64},
	{0x07, 0x4C},
//	{0x0A, 0x00},
//	{0x0B, 0x00},

};


int32_t SY6974_Init(void)
{
	int32_t ret;
	uint8_t val = 0;

	/*********cfg_init**************************/
	uint8_t regNum = sizeof(regTable) / sizeof(regTable[0][0]) / 2;
	MY_InfoLog("Total regNum: %u", regNum);

	for(uint8_t i = 0; i < regNum; i++)
	{
		ret = SY6974_Write(regTable[i][0], regTable[i][1]);
		if(ret)
		{
			MY_ErrorLog("fail. ret: %d, reg 0x%02X.", ret, regTable[i][0]);
		}
		DelayMs(5);
	}


	for(uint8_t i = 0; i < regNum; i++)
	{
		val = 0;
		ret = SY6974_Read(regTable[i][0], &val);
		if(ret)
		{
			MY_ErrorLog("fail. ret: %d, read reg 0x%02X.", ret, regTable[i][0]);
		}
		else
		{
			MY_InfoLog("success. read reg 0x%02X, value 0x%02X.", regTable[i][0], val);
		}
		DelayMs(5);
	}

	return 0;
}

void SY6974_PollingTask(void)
{
	POLLING_INTERVAL_MS(10000);

	uint8_t ntcStatus = 0;

	if(SY6974_FeedDog())
	{
		MY_ErrorLog("Failed to feed dog.");
	}

	uint8_t val = 0;
	for(uint8_t i = 0; i <= 0x0B; i++)
	{
		if(0 == SY6974_Read(i, &val))
		{
			MY_InfoLog("SY6974. reg 0x%02X, value 0x%02X.", i, val);
		}
		else
		{
			MY_InfoLog("SY6974. reg 0x%02X, read value fail.", i);
		}

		if(i == 0x09)
		{
			ntcStatus = val & 0x07;
		}
	}
	SY6974_ThermalManage((NTC_FaultStatus_te)ntcStatus);
}

int32_t SY6974_FeedDog(void)
{
	uint8_t val = 0;
	if(0 == SY6974_Read(0x01, &val))
	{
		val |= (0x01 << 6);
		if(0 == SY6974_Write(0x01, val))
		{
			return 0;
		}
	}
	return -1;
}


int32_t SY6974_Write(uint8_t reg, uint8_t value)
{
	return Tombo80WL_I2C0_MemWrite(SY6974_ADDR, reg, &value, 1, 5);
}

int32_t SY6974_Read(uint8_t reg, uint8_t *pData)
{
	return Tombo80WL_I2C0_MemRead(SY6974_ADDR, reg, pData, 1, 5);
}




static int32_t SY6974_ThermalManage(NTC_FaultStatus_te ntcStatus)
{
	static NTC_FaultStatus_te previousNtcStatus = NTC_NORMAL;

	if(ntcStatus != previousNtcStatus)
	{
		previousNtcStatus = ntcStatus;
		switch(ntcStatus)
		{
			case NTC_NORMAL:// 10C ~ 45C
			{
				SY6974_SetChargeEnable(1);
				SY6974_SetFastChargeCurrent(ICHG_1320MA);//1.3A
			}
			break;
			case NTC_WARM:	// 45C ~ 60C
			{
				SY6974_SetChargeEnable(0);//stop charge
			}
			break;
			case NTC_COOL:	// 0C ~ 10C
			{
				SY6974_SetChargeEnable(1);
				SY6974_SetFastChargeCurrent(ICHG_600MA);//600mA
			}
			break;
			case NTC_COLD:	// < 0C
			{
				SY6974_SetChargeEnable(0);//stop charge
			}
			break;
			case NTC_HOT:	// > 60C
			{
				SY6974_SetChargeEnable(0);;//stop charge
			}
			break;
		}
	}
	return 0;
}

/*
static int32_t SY6974_SearchOffset(uint8_t regNum, uint8_t *pRowOffset)
{
	uint8_t regTabletNum = sizeof(regTable) / sizeof(regTable[0][0]) / 2;

	for(uint8_t i = 0; i < regTabletNum; i++)
	{
		if(regNum == regTable[i][0])
		{
			*pRowOffset = i;
			return 0;
		}
	}

	return -1;
}
*/

static int32_t SY6974_SetChargeEnable(uint8_t enable)
{
	uint8_t regVal = 0;

	SY6974_Read(0x01, &regVal);

	if(enable)
	{
		regVal |= (1u << 4);
	}
	else
	{
		regVal &= (~(1u << 4));
	}

	SY6974_Write(0x01, regVal);

	return 0;
}

static int32_t SY6974_SetFastChargeCurrent(FastChargeCurrent_te ma)
{
	uint8_t regVal = 0;

	SY6974_Read(0x02, &regVal);

	regVal &= (~(0x3F));
	regVal |= ma;
	SY6974_Write(0x02, regVal);

	return 0;
}


