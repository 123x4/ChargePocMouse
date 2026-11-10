

#include "IAP_CMD.h"



//*********code begin****************
//*Interrupts during IAP
//*The on-chip flash memory and EEPROM are not accessible during erase/write 
//*operations. When the user application code starts executing, the interrupt vectors from the 
//*user flash area are active. Before making any IAP call, either disable the interrupts or 
//*ensure that the user interrupt vectors are active in RAM and that the interrupt handlers 
//*reside in RAM. The IAP code does not use or disable interrupts.

//*RAM used by IAP command handler
//*Flash programming commands use the top 32 bytes of on-chip local RAM. The maximum 
//*stack usage in the user allocated stack space is 128 bytes and grows downwards.

//*The flash memory is not accessible during a write or erase operation. IAP commands, 
//*which results in a flash write/erase operation, use 32 bytes of space in the top portion of 
//*the on-chip local RAM for execution. The user program should not be using this space if 
//*IAP flash programming is permitted in the application.


/* IAP command definitions */
#define LPC_IAP_READ_UID_CMD            58	/*!< Read UID */
#define LPC_IAP_EEPROM_WRITE            61	/*!< EEPROM Write command */
#define LPC_IAP_EEPROM_READ             62	/*!< EEPROM READ command */

/* IAP response definitions */
#define LPC_IAP_CMD_SUCCESS             0	/*!< Command is executed successfully */
#define LPC_IAP_INVALID_COMMAND         1	/*!< Invalid command */
#define LPC_IAP_SRC_ADDR_NOT_MAPPED     4	/*!< Source address is not mapped in the memory map */
#define LPC_IAP_DST_ADDR_NOT_MAPPED     5	/*!< Destination address is not mapped in the memory map */

/* Pointer to ROM IAP entry functions */
#define LPC_IAP_ENTRY_LOCATION        0X1FFF1FF1

/* IAP_ENTRY API function type */
typedef void (*LPC_IAP_ENTRY_T)(unsigned int[], unsigned int[]);

/**
 * @brief LPC11XX IAP_ENTRY API function type
 */
static __inline void lpc_iap_entry(unsigned int cmd_param[], unsigned int status_result[])
{
	__disable_irq();
	((LPC_IAP_ENTRY_T) LPC_IAP_ENTRY_LOCATION)(cmd_param, status_result);
	__enable_irq ();
}

//*Remark: The top 64 bytes of the 4 kB EEPROM memory are reserved and 
//*cannot be written to. The entire EEPROM is writable for smaller EEPROM sizes.
//*Range: 0x0 ~ 0xFBF

/* Write data to EEPROM */
uint8_t Chip_EEPROM_Write(uint32_t dstAdd, uint8_t *ptr, uint32_t byteswrt)
{
	uint32_t command[5], result[5];

	command[0] = LPC_IAP_EEPROM_WRITE;
	command[1] = dstAdd;
	command[2] = (uint32_t) ptr;
	command[3] = byteswrt;
	//command[4] = SystemCoreClock / 1000;
	command[4] = MCU_OSC / 1000;
	lpc_iap_entry(command, result);

	return result[0];
}

/* Read data from EEPROM */
uint8_t Chip_EEPROM_Read(uint32_t srcAdd, uint8_t *ptr, uint32_t bytesrd)
{
	uint32_t command[5], result[5];

	command[0] = LPC_IAP_EEPROM_READ;
	command[1] = srcAdd;
	command[2] = (uint32_t) ptr;
	command[3] = bytesrd;
	//command[4] = SystemCoreClock / 1000;
	command[4] = MCU_OSC / 1000;
	lpc_iap_entry(command, result);

	return result[0];
}



/* Read the unique ID */
uint32_t Chip_IAP_ReadUID(uint32_t *uid)
{
	uint32_t command[5], result[5];
	uint32_t i;

	command[0] = LPC_IAP_READ_UID_CMD;
	lpc_iap_entry(command, result);

	for (i = 0; i < 4; i++) {
		*(uid + i) = result[i + 1];
	}

	return result[0];
}

