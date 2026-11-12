/*
 * Copyright (c) 2019
 * All rights reserved.
 *
 */

#ifndef __GTRY_BUILD_CFG_H__
#define __GTRY_BUILD_CFG_H__


//----------------------------------------------------------------------------------------------------

// Build mode select
// The BUILD_MODE can be specified to BUILD_XIP/BUILD_BURN/BUILD_LOAD only.
//
#define BUILD_MODE	BUILD_LOAD


//----------------------------------------------------------------------------------------------------
// The followings are predefined settings
// Please do not modify them

#define	BUILD_LOAD	1		// The program is loaded by GDB or eBIOS
#define	BUILD_BURN	2		// The program is burned to the flash, but run in RAM
#define	BUILD_XIP	3		// The program is burned to the flash, and run in flash (xip linker script files must be used)

#if BUILD_MODE == BUILD_LOAD
#define CFG_LOAD
#endif
#if BUILD_MODE == BUILD_BURN
#define CFG_BURN
#endif
#if BUILD_MODE == BUILD_XIP
#define CFG_XIP
#endif


#endif // __GTRY_BUILD_CFG_H__
