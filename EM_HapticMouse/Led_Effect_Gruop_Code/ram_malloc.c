#include "ram_malloc.h"
#include "gloab.h"
#include "test.h"
#include "operation_flash.h"
//#include "pc_communicate.h"

UINT8 malloc_heart[MALLOC_BLOCK_NUM];

UINT8 malloc_ram[MALLOC_MAX];

BOOL malloc_hadSpace;

void RamMalloc_Init(void)
{
	UINT16 ii;
	
	for (ii = 0;ii < MALLOC_BLOCK_NUM;ii++)
	{
		malloc_heart[ii] = 0;
	}

	malloc_hadSpace = TRUE;
}

MALLOC_STRU RamMalloc_Assign(UINT16 mallocSpace)
{
	UINT16 ii,kk;
	UINT16 blockNum = 0;
	UINT8 *address = NULL;
	UINT16 spaceSum;
	BOOL firstAddress_flag = FALSE;
	UINT16 firstAddress_index = 0;
	MALLOC_STRU mallocStru = {NULL,FALSE};

	mallocStru.hadSpace = FALSE;

	for (ii = 0;ii < MALLOC_BLOCK_NUM;ii++)
	{
		if (malloc_heart[ii] == 0)
		{
			blockNum++;

			///record first empty ram block
			if (firstAddress_flag == FALSE)
			{
				firstAddress_flag = TRUE;
				firstAddress_index = ii;
			}
		}else
		{
			firstAddress_flag = FALSE;
			blockNum = 0;
		}

		spaceSum = blockNum * MALLOC_BLOCK_SPACE;

		if (spaceSum >=  mallocSpace)
		{
			///calculate first emtpy ram address
			address = (UINT8*)(malloc_ram + (firstAddress_index * MALLOC_BLOCK_SPACE));

			///clear ram heart
			if(mallocSpace != OPERATE_MODE_SETTING_SIZE)
			{
				for (kk = firstAddress_index;kk < (ii+1);kk++)
				{
					malloc_heart[kk] = 1;
				}
			}

			mallocStru.hadSpace = TRUE;

			break;
		}
	}

	mallocStru.address = address;

	malloc_hadSpace = mallocStru.hadSpace;

	return mallocStru;
}

void RamMalloc_Free(UINT8 *address, UINT16 length)
{
	UINT16 blockIndex;
	UINT16 ii;
	UINT16 blockCnt;

	blockIndex = (UINT32)(address - malloc_ram) / MALLOC_BLOCK_SPACE;

	if (((UINT32)(address - malloc_ram) % MALLOC_BLOCK_SPACE) != 0)
	{
		blockIndex++;
	}

	blockCnt = length / MALLOC_BLOCK_SPACE;

	if ((length % MALLOC_BLOCK_SPACE) != 0)
	{
		blockCnt++;
	}

	for (ii = blockIndex; ii < blockCnt;ii++)
	{
		malloc_heart[ii] = 0;
	}
}

BOOL RamMalloc_Get_Usage(void)
{
	return malloc_hadSpace;
}

void RamMalloc_Test(void)
{
	//UINT8 *address = NULL;
	//UINT16 ii;

	//for (ii = 0;ii < MALLOC_MAX;ii++)
	//{
	//	malloc_ram[ii] = ii;
	//}


	//address = RamMalloc_Assign(128);


	//if (address != NULL)
	//{
	//	
	//}		
}
