/*
 * Copyright (c) 2019
 * All rights reserved.
 *
 */

#ifndef __HASP_REGMAP_H__
#define __HASP_REGMAP_H__

#include "gtry_regmap.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifndef __ASSEMBLER__

/*****************************************************************************
 * HASP - GTRY RISCV
 ****************************************************************************/
typedef struct {
    __IO unsigned int LPCFG;                /* 0x00 low pass fillter config */
    __IO unsigned int LPCFG_RECVSTEP;		/* 0x04  */
    __IO unsigned int FDDEL_KDLP;           /* 0x08  */
    __IO unsigned int FDDEL_BBOTLP;         /* 0x0C  */
    __IO unsigned int FDDEL_BTOPLP;         /* 0x10  */
    __IO unsigned int NOISEV_KDLP;          /* 0x14  */
    __IO unsigned int NOISEV_BBOTLP;        /* 0x18  */
    __IO unsigned int NOISEV_BTOPLP;        /* 0x1C  */
    __IO unsigned int BBOTLP_PDLT;          /* 0x20  */
    __IO unsigned int BBOTLP_NDLT;          /* 0x24  */
    __IO unsigned int BTOPLP_PDLT;          /* 0x28  */
    __IO unsigned int BTOPLP_NDLT;          /* 0x2C  */
    __IO unsigned int CTRL;                 /* 0x30  */
    __IO unsigned int TD1;                  /* 0x34  */
    __IO unsigned int TD2;                  /* 0x38  */
    __IO unsigned int TD3;                  /* 0x3C  */
    __IO unsigned int TD5;                  /* 0x40  */
    __IO unsigned int TD6;                  /* 0x44  */
    __IO unsigned int TD7;                  /* 0x48  */
    __IO unsigned int TD9;                  /* 0x4C  */
    __IO unsigned int TD10;                 /* 0x50  */
    __IO unsigned int TD11;                 /* 0x54  */
    __IO unsigned int TD13;                 /* 0x58  */
    __IO unsigned int TD14;                 /* 0x5C  */
    __IO unsigned int TD15;                 /* 0x60  */
    __IO unsigned int TD17;                 /* 0x64  */
    __IO unsigned int NSCK;                 /* 0x68  */
    __IO unsigned int MAX_ABSDATA;          /* 0x6C  */
    __IO unsigned int MIN_ABSDATA;          /* 0x70  */
    __IO unsigned int RAW2ABS_REF;          /* 0x74  */
    __IO unsigned int BTOP_CALCYCLE;        /* 0x78  */
    __IO unsigned int BBOT_CALCYCLE;        /* 0x7C  */
    __IO unsigned int BTOP_ADDCNT;          /* 0x80  */
    __IO unsigned int BTOP_SUBCNT;          /* 0x84  */
    __IO unsigned int BTOP_ADDDLT;          /* 0x88  */
    __IO unsigned int BTOP_SUBDLT;          /* 0x8C  */
    __IO unsigned int BTOP_NHYST;           /* 0x90  */
    __IO unsigned int BTOP_PHYST;           /* 0x94  */
    __IO unsigned int BTOP_NDLT;            /* 0x98  */
    __IO unsigned int BTOP_PDLT;            /* 0x9C  */
    __IO unsigned int BBOT_PDLT;            /* 0xA0  */
    __IO unsigned int BBOT_NDLT;            /* 0xA4  */
    __IO unsigned int BBOT_HYST1;           /* 0xA8  */
    __IO unsigned int BBOT_HYST2;           /* 0xAC  */
    __IO unsigned int BBOT_LOCKCNT;         /* 0xB0  */
    __IO unsigned int BBOT_ADDDLT;          /* 0xB4  */
    __IO unsigned int BBOT_SUBDLT;          /* 0xB8  */
    __IO unsigned int BBOT_LKRLS;           /* 0xBC  */
    __IO unsigned int PRESSDN_CFG;          /* 0xC0  */
    __IO unsigned int KEYBUSY_VTH;          /* 0xC4  */
    __IO unsigned int START;                /* 0xC8  */
    __IO unsigned int INTEN;                /* 0xCC  */
    __IO unsigned int INTST;                /* 0xD0  */
    __IO unsigned int SCK_SPEC_SCALE;       /* 0xD4  */
    __IO unsigned int SCK_SPEC_STEP;        /* 0xD8  */
    __IO unsigned int INNER_CLK_INIT;       /* 0xDC  */
    __IO unsigned int INNER_CLK_CTRL;       /* 0xE0  */
    __IO unsigned int reserve0 [71];	    /* 0xE0~0x1FC  */
    __IO unsigned int TXDATA   [128];	    /* 0x200~0x3FC */
    __IO unsigned int RXRAWDATA[128];	    /* 0x400~0x5FC */
    __IO unsigned int RXLNDATA [128];	    /* 0x600~0x7FC */
    __IO unsigned int DATAERR  [128];	    /* 0x800~0x9FC */
    __IO unsigned int RXRAM    [128];	    /* 0xA00~0xBFC */
    __IO unsigned int BASLBOT  [128];	    /* 0xC00~0xDFC */
    __IO unsigned int BASLTOP  [128];	    /* 0xE00~0xFFC */
    __IO unsigned int LINETB   [8192];	    /* 0x1000~0x9000 */
    __IO unsigned int OFFSET   [128];	    /* 0x9000~0x91FC */
    __IO unsigned int RAWDLP   [128];	    /* 0x9200~0x93FC */
    __IO unsigned int BTOPLP   [128];	    /* 0x9400~0x95FC */
    __IO unsigned int BBOTLP   [128];	    /* 0x9600~0x97FC */
    __IO unsigned int BTOP_INIT[128];	    /* 0x9800~0x99FC */
} HASP_RegDef;

/*****************************************************************************
 * Memory Map
 ****************************************************************************/
#define HASP_BASE             	_IO_(0xE9000000)

/*****************************************************************************
 * Peripheral device declaration
 ****************************************************************************/
#define GTRY_HASP				((HASP_RegDef *)  HASP_BASE)

#define SMU_PCLK_HASP_EN_MASK	(0x1ul<<21)

#define SMU_PCSWKUP_SRC_ENL_MASK_HASP   (SMU_PCSWKUP_SRC_ENL_MASK | (0x1ul<<IRQ_HASP_SOURCE))

//#define RY_HASP_DRIVER_RET_MASK			(0x1ul<<12) // wr

#endif	/* __ASSEMBLER__ */

#ifdef __cplusplus
}
#endif

#endif	/* __HASP_REGMAP_H__ */
