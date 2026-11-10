#ifndef RAM_MALLOC_H	
#define RAM_MALLOC_H	

//#include "platform.h"
#include "gloab.h"

#define MALLOC_MAX	256//2048//2048
#define MALLOC_BLOCK_NUM  16//128//128
#define MALLOC_BLOCK_SPACE (MALLOC_MAX/MALLOC_BLOCK_NUM)  //1024/64 = 16


typedef struct
{
	UINT8 *address;
	UINT16 lenth;
}MALLOC_ASSIGN_STRU;

typedef struct
{
	UINT8 *address;
	BOOL hadSpace;
}MALLOC_STRU;

extern UINT8 malloc_ram[MALLOC_MAX];

    
void RamMalloc_Test(void);
void RamMalloc_Init(void);
MALLOC_STRU RamMalloc_Assign(UINT16 mallocSpace);
void RamMalloc_Free(UINT8 *address, UINT16 length);
BOOL RamMalloc_Get_Usage(void);


#endif
