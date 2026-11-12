/*
 * Copyright (c) 2019
 * All rights reserved.
 *
 */

#ifndef __GTRY_REGMAP_H__
#define __GTRY_REGMAP_H__

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * PLIC Interrupt source definition
 ****************************************************************************/
typedef enum IRQn{
	reserved0_IRQn = 0		,
	IRQ_RTC_PERIOD_SOURCE   ,
	IRQ_RTC_ALARM_SOURCE    ,
	IRQ_HASP_SOURCE			,
	reserved4_IRQn,
	IRQ_SPI0_SOURCE         ,
	IRQ_I2C0_SOURCE         ,
	IRQ_GPIO_SOURCE         ,
	IRQ_UART0_SOURCE        ,
	IRQ_UART1_SOURCE        ,
	IRQ_DMA0_SOURCE         ,
	reserved11_IRQn,
	IRQ_DMA1_SOURCE         ,
	IRQ_UART2_SOURCE        ,
	IRQ_I2C1_SOURCE         ,
	reserved15_IRQn,
	reserved16_IRQn,
	IRQ_USBDMA_SOURCE       ,
	IRQ_USB_SOURCE          ,
	IRQ_SPI1_SOURCE         ,
	IRQ_SPI2_SOURCE         ,
	IRQ_SPI3_SOURCE         ,
	reserved22_IRQn,
	reserved23_IRQn,
	reserved24_IRQn,
	reserved25_IRQn,
	reserved26_IRQn,
	IRQ_WAKEUP_SOURCE		,
	reserved28_IRQn,
	reserved29_IRQn,
	reserved30_IRQn,
	reserved31_IRQn,
	IRQ_SPI4_SOURCE         ,
	IRQ_SPI5_SOURCE         ,
	IRQ_SPI6_SOURCE         ,
	IRQ_PWM_SOURCE          ,
	IRQ_TIMER_SOURCE        ,
	IRQ_TRNG_SOURCE         ,
	IRQ_IOAM_SOURCE         ,
	IRQ_I2S_SOURCE          ,
	IRQ_R8080_SOURCE        ,
	IRQ_ACMP0_SOURCE       	,
	IRQ_ACMP1_SOURCE       	,
	IRQ_RGB_SOURCE          ,
	IRQ_ADC_SOURCE          ,
	IRQ_TSC_SOURCE          ,
	IRQ_DVP_SOURCE          ,
	IRQ_AES_SOURCE          ,
	IRQ_LTIMER0_SOURCE      ,
	IRQ_LTIMER1_SOURCE      ,
	IRQ_SDIO_SOURCE         ,
	IRQ_ARGB_SOURCE         ,
	IRQ_MAX_SOURCE,
}IRQn_Type;

#ifndef __ASSEMBLER__

/*****************************************************************************
 * Device Specific Peripheral Registers structures
 ****************************************************************************/

#define __I					volatile const	/* 'read only' permissions      */
#define __O					volatile        /* 'write only' permissions     */
#define __IO				volatile        /* 'read / write' permissions   */

/*****************************************************************************
 * L2CACHE - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__I  unsigned long long L2C_CFG;        /* 0x00 Configuration Register */
	__IO unsigned long long L2C_CTRL;       /* 0x08 Control Register */
	__IO unsigned long long L2C_HPM_CTL0;   /* 0x10 HPM Control Register0 */
	__IO unsigned long long L2C_HPM_CTL1;   /* 0x18 HPM Control Register1 */
	__IO unsigned long long L2C_HPM_CTL2;   /* 0x20 HPM Control Register2 */
	__IO unsigned long long L2C_HPM_CTL3;   /* 0x28 HPM Control Register3 */
	__IO unsigned long long L2C_ASYN_ERR;   /* 0x30 Asynchronous Error Register */
	__IO unsigned long long L2C_ERR;        /* 0x38 Error Register */
	__IO unsigned long long L2C_CCTL_CMD;   /* 0x40 Core 0 CCTL Command Register */
	__IO unsigned long long L2C_CCTL_ACC;   /* 0x48 Core 0 CCTL Access Line Register */
	     unsigned long long RESERVED[6];   /* 0x50 ~ 0x7F Reserved */
	__I  unsigned long long L2C_CCTL_STS;	/* 0x80 Core 0 CCTL Status Register */
} L2C_RegDef;

/*****************************************************************************
 * PLMT - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO unsigned long long MTIME;          /* 0x00 Machine Time */
	__IO unsigned long long MTIMECMP;       /* 0x08 Machine Time Compare */
} PLMT_RegDef;

/*****************************************************************************
 * SMU - GTRY RISCV
 ****************************************************************************/
typedef struct {
	     unsigned int RESERVED0[2];         /* 0x04 Reserved */
	__I  unsigned int SMU_SYS_CFG;          /* 0x08 SYSTEM configuration register */
	  	 unsigned int RESERVED1;			/* 0x0C Reserved */
		 unsigned int RESERVED2[2];    		/* 0x10 ~ 0x14 Reserved */
		 unsigned int RESERVED3[2];   		/* 0x18 ~ 0x1C Reserved */
	__I  unsigned int SMU_SYS_STS;          /* 0x20 system status Register */
	__IO unsigned int SMU_CLK_CFG;          /* 0x24 system clock config Register */
	__IO unsigned int SMU_PWR_CFG0;         /* 0x28 system power config0 Register */
	__IO unsigned int SMU_PWR_CFG1;         /* 0x2C system power config1 Register */
		 unsigned int RESERVED4[2];    		/* 0x30 ~ 0x34 Reserved */
	__IO unsigned int SMU_PCLK_EN;          /* 0x38 Peripheral Clock enable Register */
		 unsigned int RESERVED5[2];    		/* 0x3C ~ 0x40 Reserved */
		 unsigned int RESERVED6[3];         /* 0x44 ~ 0x4C Reserved */
	__IO unsigned int SMU_RST_VEC_LP;       /* 0x50 CPU Reset Vector Register[31:0] */
	 	 unsigned int RESERVED7[3];         /* 0x54 ~ 0x5C Reserved */
	__IO unsigned int SMU_RST_VEC_HP;       /* 0x60 CPU Reset Vector Register[63:32] */
		 unsigned int RESERVED8[7];			/* (0x64 ~ 0x7C) Reserved register */
		 unsigned int RESERVED9[6];			/* (0x80 ~ 0x94) Reserved register */
	__IO unsigned int SMU_PCS_SRST;			/* (0x98) PCS soft reset register */
	__IO unsigned int SMU_PCS_SRST_ST;		/* (0x9C) PCS soft reset status register */
	 	 unsigned int RESERVED10[16];		/* (0xA0 ~ 0xDC) Reserved register */
	 	 unsigned int RESERVED11;			/* (0xE0) Reserved */
	__IO unsigned int SMU_PCS_SCRATCH;		/* (0xE4) PCS scratch pad */
		 unsigned int RESERVED12;			/* (0xE8) Reserved */
		 unsigned int RESERVED13;			/* (0xEC) Reserved */
	__IO unsigned int SMU_PCS_WKUP_ENL;		/* (0xF0) PCS wake up enable[31:0] */
	__IO unsigned int SMU_PCS_WKUP_ENH;		/* (0xF4) PCS wake up enable[63:32] */
	__IO unsigned int SMU_PCS_CTRL;			/* (0xF8) PCS control */
	__IO unsigned int SMU_PCS_STS;			/* (0xFC) PCS status */
	 	 unsigned int RESERVED14[24];		/* (0x100 ~ 0x15C) Reserved register */
	 	 unsigned int RESERVED15[8];		/* (0x160 ~ 0x17C) Reserved register */
	__IO unsigned int SMU_ANA_LP_EN;		/* (0x180) Analog circuit low power enable */
		 unsigned int RESERVED16;			/* (0x184) Reserved */
	__IO unsigned int SMU_ANA_WKUP_EN;		/* (0x188) Analog circuit wakeup enable */
	__IO unsigned int SMU_WKUP_DEB_EN;		/* (0x18C) GPIO wakeup signal debounce enable */
	__IO unsigned int SMU_IMO_CFG0;			/* (0x190) IMO config reg0 */
	__IO unsigned int SMU_IMO_CFG1;			/* (0x194) IMO config reg1 */
	__IO unsigned int SMU_PLL_CFG0;			/* (0x198) FNPLL config reg0 */
	__IO unsigned int SMU_PLL_CFG1;			/* (0x19C) FNPLL config reg1 */
	__IO unsigned int SMU_XTAL_ILO_CFG;		/* (0x1A0) XTAL and ILO config reg */
		 unsigned int RESERVED17[3];        /* (0x1A4 ~ 0x1AC) Reserved */
		 unsigned int RESERVED18[8];        /* (0x1B0 ~ 0x1CC) Reserved */
	__IO unsigned int SMU_WKUP_SRCL;		/* (0x1D0) wakeup source[31:0] */
	__IO unsigned int SMU_WKUP_SRCH;		/* (0x1D4) wakeup source[63:32] */
} SMU_RegDef;

/*****************************************************************************
 * UARTx - GTRY RISCV
 ****************************************************************************/
typedef struct {
	     unsigned int RESERVED0[5];         /* 0x00 ~ 0x10 Reserved */
	__IO unsigned int UART_OSCR;            /* 0x14 Over Sample Control Register */
	__IO unsigned int UART_DMA_CFG;         /* 0x18 DMA REV Control Register */
	     unsigned int RESERVED1;        	/* 0x1C Reserved */
	union {
		__IO unsigned int UART_RBR;         /* 0x20 Receiver Buffer Register */
		__O  unsigned int UART_THR;         /* 0x20 Transmitter Holding Register */
		__IO unsigned int UART_DLL;         /* 0x20 Divisor Latch LSB */
	};
	union {
		__IO unsigned int UART_IER;         /* 0x24 Interrupt Enable Register */
		__IO unsigned int UART_DLM;         /* 0x24 Divisor Latch MSB */
	};
	union {
		__IO unsigned int UART_IIR;         /* 0x28 Interrupt Identification Register */
		__O  unsigned int UART_FCR;         /* 0x28 FIFO Control Register */
	};
	__IO unsigned int UART_LCR;             /* 0x2C Line Control Register */
	__IO unsigned int UART_MCR;             /* 0x30 Modem Control Register */
	__IO unsigned int UART_LSR;             /* 0x34 Line Status Register */
	__IO unsigned int UART_MSR;             /* 0x38 Modem Status Register */
	__IO unsigned int UART_SCR;             /* 0x3C Scratch Register */
} UART_RegDef;

/*****************************************************************************
 * TSC - GTRY RISCV
 ****************************************************************************/
typedef struct
{
	__IO unsigned int TSC_START;    	 	/* 0x00  start  register.  */
	__IO unsigned int TSC_INT_CTRL;     	/* 0x04  int control register. */
	__IO unsigned int TSC_CTRL;    	    	/* 0x08  status  register.  */
	__IO unsigned int TSC_CVT_TIME;    		/* 0x0C  convert time config register.  */
	__IO unsigned int TSC_CHL_EN;   		/* 0x10  ch enable register.  */
	__I  unsigned int TSC_CNT_CHL[32];      /* 0x14~0x90  data  register.  */
}TSC_RegDef;

/*****************************************************************************
 * WDT - GTRY RISCV
 ****************************************************************************/
typedef struct {
	     unsigned int RESERVED[4];          /* 0x00 ~ 0x0C Reserved */
	__IO unsigned int WDT_CTRL;             /* 0x10 Control Register */
	__O  unsigned int WDT_RESTART;          /* 0x14 Restart Register */
	__O  unsigned int WDT_WR_EN;            /* 0x18 Write Enable Register */
	__IO unsigned int WDT_STATUS;           /* 0x1C Status Register */
} WDT_RegDef;

/*****************************************************************************
 * RTC - GTRY RISCV
 ****************************************************************************/
typedef struct {
	     unsigned int RESERVED[4];          /* 0x00 ~ 0x0C Reserved */
	__IO unsigned int RTC_CNTR;             /* 0x10 Counter Register */
	__IO unsigned int RTC_ALARM;            /* 0x14 Alarm Register */
	__IO unsigned int RTC_CTRL;             /* 0x18 Control Register */
	__IO unsigned int RTC_STATUS;           /* 0x1C Status Register */
} RTC_RegDef;

/*****************************************************************************
 * ARGB - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO unsigned int ARGB_CTRL;			/* 0x00  */
	__IO unsigned int ARGB_CHL_CFG;			//0x04
	__IO unsigned int ARGB_LED_NUM;			//0x08
	__IO unsigned int ARGB_LED_BYTE;		//0x0C
	__IO unsigned int ARGB_BIT_WIDTH;		//0x10
	__IO unsigned int ARGB_T0H_WIDTH;		//0x14
	__IO unsigned int ARGB_T1H_WIDTH;		//0x18
	__IO unsigned int ARGB_RST_WIDTH;		//0x1C
	__IO unsigned int ARGB_COM_BITS;		//0x20
	__IO unsigned int ARGB_COM_DATA[4];		//0x24~0x30
	__IO unsigned int ARGB_INT_EN;			//0x34
	__IO unsigned int ARGB_STATUS;			//0x38
		 unsigned int RESERVED;          	//0x3c
	__IO unsigned int ARGB_FIFO_ADDR;		//0x40
	__IO unsigned int ARGB_FIFO_CLR;		//0x44
	__O  unsigned int ARGB_START;			//0x48
} ARGB_RegDef;

/*****************************************************************************
 * GPIO - GTRY RISCV
 ****************************************************************************/
typedef struct {
		 unsigned int RESERVED0[2];                	/* 0x00 ~ 0x04 Reserved */
	union{
		struct {
			__I  unsigned int GPIO_DATAIN[3];       /* 0x08~0x10 data-in register */
		};
		struct {
			__I  unsigned int GPA_DATAIN[2];        /* 0x08 data-in register */
			__I  unsigned int GPB_DATAIN;           /* 0x10 data-in register */
		};
	};
	union{
		struct {
			__IO unsigned int GPIO_DATAOUT[3];      /* 0x14~0x1C data-out register */
		};
		struct {
			__IO unsigned int GPA_DATAOUT[2];       /* 0x14~0x18 data-out register */
			__IO unsigned int GPB_DATAOUT;          /* 0x1c data-out register */
		};
	};
	union{
		struct {
			__O  unsigned int GPIO_DOUTCLEAR[3];    /* 0x20~0x28 data-out clear register */
		};
		struct {
			__O  unsigned int GPA_DOUTCLEAR[2];     /* 0x20~0x24 data-out clear register */
			__O  unsigned int GPB_DOUTCLEAR;        /* 0x28 data-out clear register */
		};
	};
	union{
		struct {
			__O  unsigned int GPIO_DOUTSET[3];      /* 0x2C~0x34 data-out set register */
		};
		struct {
			__O  unsigned int GPA_DOUTSET[2];       /* 0x2c~0x30 data-out set register */
			__O  unsigned int GPB_DOUTSET;          /* 0x34 data-out set register */
		};
	};
	union{
		struct {
			__IO unsigned int GPIO_INT_EN[3];       /* 0x38~0x40 Interrupt enable register */
		};
		struct {
			__IO unsigned int GPA_INT_EN[2];        /* 0x38~0x3c Interrupt enable register */
			__IO unsigned int GPB_INT_EN;           /* 0x40 Interrupt enable register */
		};
	};
	__IO unsigned int GPIO_SENSE_MODE[8];        		/* 0x44~0x60 Interrupt mode register (0~7) */
	union{
		struct {
			__IO unsigned int GPIO_INT_STS[3];   /* 0x64~0x6C Interrupt status register */
		};
		struct {
			__IO unsigned int GPA_INT_STS[2];    /* 0x64~0x68 Interrupt status register */
			__IO unsigned int GPB_INT_STS;       /* 0x6c Interrupt status register */
		};
	};
	union{
		struct {
			__IO unsigned int GPIO_DEB_EN[3];   /* 0x70~0x78 De-bounce enable register */
		};
		struct {
			__IO unsigned int GPA_DEB_EN[2];    /* 0x70~0x74 De-bounce enable register */
			__IO unsigned int GPB_DEB_EN;       /* 0x78 De-bounce enable register */
		};
	};
	__IO unsigned int GPA_DEB_CTRL;     			/* 0x7c De-bounce control register */
	__IO unsigned int GPB_DEB_CTRL;     			/* 0x80 De-bounce control register */
		 unsigned int RESERVED1[31];		    		/* 0x84 ~ 0xfc Reserved */
	union{
		struct {
			__IO unsigned int GPIO_CFG[64];         /* 0x100~0x1fc Configuration register */
		};
		struct {
			__IO unsigned int GPA_CFG[48];          /* 0x100~0x1bc Configuration register */
			__IO unsigned int GPB_CFG[16];			/* 0x1c0~0x1fc Configuration register */
		};
	};
} GPIO_RegDef;

/*****************************************************************************
 * I2Cx - GTRY RISCV
 ****************************************************************************/
typedef struct {
	     unsigned int RESERVED[5];          /* 0x00 ~ 0x10 Reserved */
	__IO unsigned int I2C_INT_EN;           /* 0x14 Interrupt Enable Register */
	__IO unsigned int I2C_STATUS;           /* 0x18 Status Register */
	__IO unsigned int I2C_ADDR;             /* 0x1C Address Register */
	__IO unsigned int I2C_DATA;             /* 0x20 Data Register */
	__IO unsigned int I2C_CTRL;             /* 0x24 Control Register */
	__IO unsigned int I2C_CMD;              /* 0x28 Command Register */
	__IO unsigned int I2C_SETUP;            /* 0x2C Setup Register */
		 unsigned int RESERVED1;        	/* 0x30 Reserved */
	__IO unsigned int I2C_ENH_CNT;      	/* 0x34 ENH CNT Register */
} I2C_RegDef;

/*****************************************************************************
 * SPIx - GTRY RISCV
 ****************************************************************************/
typedef struct {
	     unsigned int RESERVED0[4];         /* 0x00 ~ 0x0c Reserved */
	__IO unsigned int SPI_TRANS_FMT;        /* 0x10 SPI transfer format register */
	__IO unsigned int SPI_DIRECT_IO;        /* 0x14 SPI direct IO control register */
	     unsigned int RESERVED1[2];         /* 0x18 ~ 0x1c Reserved */
	__IO unsigned int SPI_TRANS_CTRL;       /* 0x20 SPI transfer control register */
	__IO unsigned int SPI_CMD;              /* 0x24 SPI command register */
	__IO unsigned int SPI_ADDR;             /* 0x28 SPI address register */
	__IO unsigned int SPI_DATA;             /* 0x2c SPI data register */
	__IO unsigned int SPI_CTRL;             /* 0x30 SPI conrtol register */
	__I  unsigned int SPI_STATUS;           /* 0x34 SPI status register */
	__IO unsigned int SPI_INT_EN;           /* 0x38 SPI interrupt enable register */
	__O  unsigned int SPI_INT_ST;           /* 0x3c SPI interrupt status register */
	__IO unsigned int SPI_TIMING;           /* 0x40 SPI interface timing register */
	     unsigned int RESERVED2[3];         /* 0x44 ~ 0x4c Reserved */
	__IO unsigned int SPI_MEM_CTRL;         /* 0x50 SPI memory access control register */
	     unsigned int RESERVED3[3];         /* 0x54 ~ 0x5c Reserved */
	__IO unsigned int SPI_SLV_ST;           /* 0x60 SPI slave status register */
	__IO unsigned int SPI_HAS_TXCNT;        /* 0x64 SPI has send data count register */
	__IO unsigned int SPI_HAS_RXCNT;        /* 0x68 SPI has recv data count register */
	__IO unsigned int SPI_TRANS_TXCNT;        /* 0x6C SPI should trans data tx count register */
	__IO unsigned int SPI_TRANS_RXCNT;        /* 0x70 SPI should trans data rx count register */
} SPI_RegDef;

/*****************************************************************************
 * DMA - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO unsigned int CHL_CTRL;             /* DMA Channel Control Register */
	__IO unsigned int CHL_TRAN_SIZE;        /* DMA Channel Transfer Size Register */
	__IO unsigned int CHL_SRC_ADDRL;        /* DMA Channel Source Address(low part) Register */
	__IO unsigned int CHL_SRC_ADDRH;     	/* DMA Channel Source Address(high part) Register */
	__IO unsigned int CHL_DST_ADDRL;        /* DMA Channel Destination Address Register(low part) */
	__IO unsigned int CHL_DST_ADDRH;        /* DMA Channel Destination Address Register(high part) */
	__IO unsigned int CHL_LL_PL;            /* DMA Channel Linked List Pointer Register(low part) */
	__IO unsigned int CHL_LL_PH;            /* DMA Channel Linked List Pointer Register(high part) */
} DMA_CHL_REG;

typedef struct {
	  	 unsigned int RESERVED0;           	/* 0x00 Reserved */
	     unsigned int RESERVED1[3];         /* 0x04 ~ 0x0C Reserved */
	__I  unsigned int DMA_CFG;              /* 0x10 DMA Configure Register */
	     unsigned int RESERVED2[3];         /* 0x14 ~ 0x1C Reserved */
	__IO unsigned int DMA_CTRL;             /* 0x20 DMA Control Register */
	__O  unsigned int DMA_CHL_ABT;          /* 0x24 DMA Channel Abort Register */
	     unsigned int RESERVED3[2];         /* 0x28 ~ 0x2C Reserved */
	__IO unsigned int DMA_INT_STS;       	/* 0x30 Interrupt Status Register */
	__I  unsigned int DMA_CHL_STS;			/* 0x34 Channel Enable Register*/
	     unsigned int RESERVED4[2];         /* 0x38 ~ 0x3C Reserved */
	DMA_CHL_REG 	DMA_CHL[8];       		/* 0x40 ~ 0x13C Channel #n Registers */
} DMA_RegDef;

/*****************************************************************************
 * PWM - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO unsigned int PWM_ENABLE;			/*0x00 PWM ENABLE register */
	__IO unsigned int PWM_CONFIG[4];		/*0x04 ~ 0x10 PWM CONFIG register */
	__IO unsigned int PWM_POLARITY;			/*0x14 PWM PLOARITY register */
		 unsigned int RESERVED[6];      	/*0x18 ~ 0x2C Reserved */
	__IO unsigned int PWM_CYCLE[4];			/*0x30 ~ 0x3C PWM CYCLE register */
	__IO unsigned int PWM_COMPARE[32];		/*0x40 ~ 0xBC PWM COMPARE register */
	__IO unsigned int PWM_SCALER;			/*0xC0 PWM SCALER register */
	__IO unsigned int PWM_UPDATE_EN;		/*0xC4 ~ 0x1C PWM update register */
	__IO unsigned int PWM_MODESEL;			/*0xC8 PWM MODESEL register */
	__IO unsigned int PWM_STOP;				/*0xCC PWM STOP register */
	__IO unsigned int PWM_ADC_MASK;			/*0xD0*/
	__IO unsigned int PWM_ADC_SEL;			/*0xD4*/
	__IO unsigned int PWM_INT_MASK;			/*0xD8*/
	__IO unsigned int PWM_INT_STS;			/*0xDC*/
	__IO unsigned int PWM_INT_CLR;			/*0xE0*/
	__IO unsigned int PWM_STOP_MODE;		/*0xE4*/
	__IO unsigned int PWM_DMA_CONFIG;		/*0xE8*/
} PWM_RegDef;

/*****************************************************************************
 * TIMERx - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO  unsigned int TIMn_CONFIG[3];      /* 0x0~0x8  ID and Revision Register */
	__O   unsigned int TIM_TRIGGER;         /* 0xC  ID and Revision Register */
	__IO  unsigned int TIM_CC_CLEAR;        /* 0x10 ID and Revision Register */
	__IO  unsigned int INT_EN_MASK;         /* 0x14 ID and Revision Register */
	__IO  unsigned int INT_STS_CLEAR;       /* 0x18 ID and Revision Register */
	__IO  unsigned int TIM_BIT_MODE;        /* 0x1c ID and Revision Register */
	__IO  unsigned int TIM_SCALER;         	/* 0x20 ID and Revision Register */
	__IO  unsigned int TIM_STOP;         	/* 0x24 ID and Revision Register */
	__IO  unsigned int TIM_DMA;         	/* 0x28 ID and Revision Register */
	__I   unsigned int TIM_INT_STS;         /* 0x2C ID and Revision Register */
	__IO  unsigned int TIMn_CCx[3][4];      /* 0x30~0x5c ID and Revision Register */
	__I	  unsigned int RESERVED0[4];		/* 0x60~0x6c ID and Revision Register */
	__IO  unsigned int TIMn_RELOAD[3];      /* 0x70~0x78 ID and Revision Register */
	__I	  unsigned int RESERVED1;			/* 0x7c ID and Revision Register */
	__IO  unsigned int TIMn_CNT_CFG[3];     /* 0x80~0x88 ID and Revision Register */
} TIMER_RegDef;

/*****************************************************************************
 * LTIMERx - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__O  unsigned int LTIM_START;			/* 0x00  */
	__O  unsigned int LTIM_STOP;            //0x04
	__O  unsigned int LTIM_CLEAR;           //0x08
	__O  unsigned int LTIM_TRIG_OVRFLW;     //0x0C
	__IO unsigned int LTIM_TICK_EN;         //0x10
	__IO unsigned int LTIM_TICK_STS;        //0x14
	__IO unsigned int LTIM_OVRFLW;          //0x18
		 unsigned int RESERVED0;            //0x1C
	__IO unsigned int LTIM_COMPARE[4];      //0x20~0x2c
	__IO unsigned int LTIM_INT_EN;          //0x30
	__O  unsigned int LTIM_INT_EN_SET;      //0x34
	__O  unsigned int LTIM_INT_EN_CLR;      //0x38
		 unsigned int RESERVED1;            //0x3c
	__I  unsigned int LTIM_COUNT;           //0x40
	__IO unsigned int LTIM_PRESCALER;       //0x44
		 unsigned int RESERVED2[2];         //0x48~0x4c
	__IO unsigned int LTIM_CC[4];			//0x50~0x5c
	__I  unsigned int LTIM_CC_STATUS;		//0x60
} LTIM_RegDef;

/*****************************************************************************
 * TRNG - GTRY RISCV
 ****************************************************************************/
typedef struct
{
	__IO unsigned int  TRNG_START;			/* 0x00  start  register.  */
	__IO unsigned int  TRNG_STOP;     		/* 0x04  stop  register. */
	__IO unsigned int  TRNG_STATUS;    	    /* 0x08  status  register.  */
	__IO unsigned int  TRNG_INT_EN;    		/* 0x0C  interrupt enable  register.  */
	__IO unsigned int  TRNG_CONFIG;   		/* 0x10  configure register.  */
	__I  unsigned int  TRNG_DATA;     		/* 0x14  data  register.  */
}TRNG_RegDef;

/*****************************************************************************
 * IOAM - GTRY RISCV
 ****************************************************************************/
typedef struct
{
	__IO unsigned int  IOAM_EN;             //0x00       			/*!<   register.                         */
	__IO unsigned int  IOAM_CHL_ON;			//0x04       			/*!<   register.                         */
		 unsigned int  RESERVED[2];         //0x08~0x0C       		/*!<   register.                         */
	__IO unsigned int  IOAM_SAMP_INTV;      //0x10       			/*!<   register.                         */
	__IO unsigned int  IOAM_RON_SAP_INTV;   //0x14       			/*!<   register.                         */
	__I  unsigned int  IOAM_CHL_VAL[10];    //0x18~0x3C  			/*!<   register.                         */
	__I  unsigned int  IOAM_SAMP_NUM;       //0x40       			/*!<   register.                         */
	__IO unsigned int  IOAM_INT_MASK;       //0x44       			/*!<   register.                         */
	__I  unsigned int  IOAM_INT_STATUS;     //0x48       			/*!<   register.                         */
	__O  unsigned int  IOAM_INT_CLEAR;      //0x4C       			/*!<   register.                         */
	__IO unsigned int  IOAM_CHL_SEL;		//0x50       			/*!<   register.                         */
	__IO unsigned int  IOAM_SAMP_TH;		//0x54       			/*!<   register.                         */
} IOAM_RegDef;

/*****************************************************************************
 * I2S - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO unsigned int I2S_ENABLE;  			/*0x000 1 bit R/W I2S Enable Register */
	__IO unsigned int I2S_RX_EN; 			/*0x004 1 bit R/W I2S Receiver Enable Register */
	__IO unsigned int I2S_TX_EN; 			/*0x008 1 bit R/W I2S Transmitter Enable Register */
	__IO unsigned int I2S_CLK_EN;			/*0x00C 1 bit R/W Clock Enable Register */
	__IO unsigned int I2S_CLK_CFG; 			/*0x010 5 bits R/W Clock Configuration Register */
	__O  unsigned int I2S_RX_RST; 			/*0x014 1 bit W Receiver FIFO reset Register */
	__O  unsigned int I2S_TX_RST; 			/*0x018 1 bit W Transmitter FIFO reset Register */
		 unsigned int RESERVED0[1];   		/*0x1C Reserved*/
	union {
	__I  unsigned int I2S_LRX_BUF;			/*0x020 32 bits R Left Receive Buffer */
	__O  unsigned int I2S_LTX_BUF;			/*0x020 32 bits W Left Transmit Holding Register */
	};
	union {
	__I  unsigned int I2S_RRX_BUF;			/*0x024 32 bits R Right Receive Buffer */
	__O  unsigned int I2S_RTX_BUF;			/*0x024 32 bits W Right Transmit Holding Register */
	};
	__IO unsigned int I2S_RER;				/*0x028 1 bit R/W Receive Enable Register 0*/
	__IO unsigned int I2S_TER;				/*0x02C 1 bit R/W Transmit Enable Register 0*/
	__IO unsigned int I2S_RCR;				/*0x030 3 bits R/W Receive Configuration Register 0 */
	__IO unsigned int I2S_TCR;				/*0x034 3 bits R/W Transmit Configuration Register 0*/
	__I  unsigned int I2S_ISR;				/*0x038 6 bits R Interrupt Status Register 0*/
	__IO unsigned int I2S_IMR; 				/*0x03C 6 bits R/W Interrupt Mask Register 0*/
	__I  unsigned int I2S_ROR;  			/*0x040 1 bit R Receive Overrun Register 0*/
	__I  unsigned int I2S_TOR; 				/*0x044 1 bit R Transmit Overrun Register 0*/
	__IO unsigned int I2S_RFCR; 			/*0x048 4 bits R/W Receive FIFO Configuration Register 0*/
	__IO unsigned int I2S_TFCR;				/*0x04C 4 bits R/W Transmit FIFO Configuration Register 0 */
	__O  unsigned int I2S_RFF;				/*0x050 1 bit W Receive FIFO Flush 0 */
	__O  unsigned int I2S_TFF;				/*0x054 1 bit W Transmit FIFO Flush 0*/
		 unsigned int RESERVED1[2];   		/*0x58 ~ 0x5C Reserved */
	 	 unsigned int RESERVED2[46];   		/*0x60 ~ 0x114 Reserved */
	__IO unsigned int I2S_CONFIG; 			/*0x118 4 bits R/W  I2S Config Reg*/
	__IO unsigned int I2S_TX_DMA_CNT; 		/*0x11C 22 bits R/W  I2S DMA Number of Transmissions Reg*/
	__IO unsigned int I2S_RX_DMA_CNT; 		/*0x120 22 bits R/W  I2S DMA EN Reg*/
	__IO unsigned int I2S_TX_DMA_EN; 		/*0x124 22 bits R/W  I2S DMA Number of Transmissions Reg*/
	__IO unsigned int I2S_RX_DMA_EN; 		/*0x128 22 bits R/W  I2S DMA EN Reg*/
	__IO unsigned int I2S_DMA_BUSY; 		/*0x12C 22 bits R/W  I2S DMA EN Reg*/
	__IO unsigned int I2S_DMA_RL_SEL; 		/*0x130 22 bits R/W  I2S DMA Transmission channel Reg*/
	__IO unsigned int I2S_TXRF_STS;			/*0x134 22 bits R/W  I2S DMA Right channel sends FIFO status Reg*/
	__IO unsigned int I2S_TXLF_STS;			/*0x138 22 bits R/W  I2S DMA left channel sends FIFO status Reg*/
	__IO unsigned int I2S_RXRF_STS;			/*0x13C 22 bits R/W  I2S DMA Right channel receive FIFO status Reg*/
	__IO unsigned int I2S_RXLF_STS;			/*0x140 22 bits R/W  I2S DMA Left channel receive FIFO status Reg*/
	__IO unsigned int I2S_TX_DMA_ADDR;		/*0x144 22 bits R/W  I2S DMA Transport address Reg*/
	__IO unsigned int I2S_RX_DMA_ADDR;		/*0x148 22 bits R/W  I2S DMA Transport address Reg*/
	__IO unsigned int I2S_INT_MASK;			/*0x14C 22 bits R/W  I2S DMA Send interrupt mask and interrupt status Reg*/
	__IO unsigned int I2S_INT_STS;			/*0x150 22 bits R/W  I2S DMA Send interrupt to clear Reg*/
	__IO unsigned int I2S_INT_CLR;			/*0x154 22 bits R/W  I2S DMA Send interrupt to clear Reg*/
	__IO unsigned int I2S_DMA_BURST_SEL;	/*0x158 22 bits R/W  I2S DMA Send interrupt to clear Reg*/
	__IO unsigned int I2S_MODE_SEL;			/*0x15C 1 bits R/W  I2S Mode select Reg*/
	__IO unsigned int I2S_SCALER_REG;		/*0x160 1 bits R/W  I2S Mode select Reg*/
}I2S_RegDef;

/*****************************************************************************
 * AES - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__O  unsigned int AES_ECB_START;        //0x00				/*!<   register.                         */
	__O  unsigned int AES_ECB_STOP;         //0x04				/*!<   register.                         */
	__IO unsigned int AES_ECB_END;          //0x08       		/*!<   register.                         */
	__IO unsigned int AES_ECB_ERROR;        //0x0C       	 	/*!<   register.                         */
	__O  unsigned int AES_ECB_INT_EN_SET;   //0x10         		/*!<   register.                         */
	__O  unsigned int AES_ECB_INT_EN_CLR;   //0x14         		/*!<   register.                         */
	__IO unsigned int AES_ECB_DATA_PTR;     //0x18        		/*!<   register.                         */
	__I  unsigned int RESERVED0[57];
	__O  unsigned int AES_CCM_KSGEN;        //0x100        		/*!<   register.                         */
	__O  unsigned int AES_CCM_CRYPT;        //0x104        		/*!<   register.                         */
	__IO unsigned int AES_CCM_END_KSGEN;    //0x108        		/*!<   register.                         */
	__IO unsigned int AES_CCM_END_CRYPT;    //0x10C				/*!<   register.                         */
	__IO unsigned int AES_CCM_ERROR;        //0x110         	/*!<   register.                         */
	__IO unsigned int AES_CCM_SHORTS;       //0x114          	/*!<   register.                         */
	__O  unsigned int AES_CCM_INT_EN_SET;   //0x118            	/*!<   register.                         */
	__O  unsigned int AES_CCM_INT_EN_CLR;   //0x11C            	/*!<   register.                         */
	__I  unsigned int AES_CCM_MIC_STATUS;   //0x120            	/*!<   register.                         */
	__IO unsigned int AES_CCM_AAR_ENABLE;   //0x124            	/*!<   register.                         */
	__IO unsigned int AES_CCM_MODE;         //0x128        		/*!<   register.                         */
	__IO unsigned int AES_CCM_CNF_PTR;      //0x12C           	/*!<   register.                         */
	__IO unsigned int AES_CCM_IN_PTR;       //0x130          	/*!<   register.                         */
	__IO unsigned int AES_CCM_OUT_PTR;      //0x134           	/*!<   register.                         */
	__IO unsigned int AES_CCM_SCRATCH_PTR;  //0x138				/*!<   register.                         */
	__I  unsigned int RESERVED1[49];
	__O  unsigned int AES_AAR_START;        //0x200        		/*!<   register.                         */
	__IO unsigned int AES_AAR_END;          //0x204       		/*!<   register.                         */
	__IO unsigned int AES_AAR_RESOLVED;     //0x208				/*!<   register.                         */
	__IO unsigned int AES_AAR_NOT_RESOLVED; //0x20C				/*!<   register.                         */
	__O  unsigned int AES_AAR_INT_EN_SET;   //0x210				/*!<   register.                         */
	__O  unsigned int AES_AAR_INT_EN_CLR;   //0x214				/*!<   register.                         */
	__I  unsigned int AES_AAR_STATUS;       //0x218          	/*!<   register.                         */
	__I  unsigned int RESERVED2;
	__IO unsigned int AES_AAR_NIRK;         //0x220				/*!<   register.                         */
	__IO unsigned int AES_AAR_IRK_PTR;      //0x224           	/*!<   register.                         */
	__IO unsigned int AES_AAR_ADD_PTR;      //0x228           	/*!<   register.                         */
	__IO unsigned int AES_AAR_SCRATCH_PTR;  //0x22C				/*!<   register.                         */
} AES_RegDef;

/*****************************************************************************
 * ADC - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO unsigned int ADC_CONFIG;			/* 0x00   */
	__IO unsigned int ADC_CALIB_CFG;		/* 0x04   */
	__IO unsigned int ADC_CONV_CFG;			/* 0x08   */
	__IO unsigned int ADC_COMP_CFG;			/* 0x0C   */
	__IO unsigned int RESERVED0;			/* 0x10 RES   */
	__IO unsigned int ADC_DMA_SIZE;			/* 0x14   */
	__IO unsigned int RESERVED1;			/* 0x18 RES   */
	__IO unsigned int ADC_FIFO_REG;			/* 0x1C   */
	__IO unsigned int ADC_SWITCH_CFG;		/* 0x20   */
	__I  unsigned int ADC_READ_DATA;		/* 0x24   */
	__IO unsigned int ADC_COMP_CALP;		/* 0x28   */
	__IO unsigned int ADC_COMP_CALN;		/* 0x2C   */
	__I  unsigned int ADC_CHANNEL[30];		/* 0x30~0xa4  */
	__IO unsigned int ADC_CAP_WEIGHT[14];	/* 0xA8~0xDC  */
	__O  unsigned int ADC_INT_CLR;			/* 0xE0   */
	__I  unsigned int ADC_INT_STS;			/* 0xE4   */
	__IO unsigned int ADC_INT_EN;  			/* 0xE8   */
} ADC_RegDef;

/*****************************************************************************
 * ACMP_OPA - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO unsigned int ACMP1_CSR;			/* 0x00 Comparator1 control status Register */
	__IO unsigned int ACMP2_CSR;			/* 0x04 Comparator2 control status Register */
	__IO unsigned int ACMP_CRV;				/* 0x08 SYSTEM configuration register */
	__IO unsigned int ACMP_INT_CTRL;		/* 0x0C Comparator intertuptt control register */
	__IO unsigned int OPA_CTRL;				/* 0x10 OPA */
} ACMP_OPA_RegDef;

/*****************************************************************************
 * USB - GTRY RISCV
 ****************************************************************************/
typedef struct {
//Common USB registers (00h-C0Fh) -- These registers provide control and status for the complete core.
	__IO unsigned char   USB_FADDR;			/*!< 0x00, USB Device Functional Address                            */
	__IO unsigned char   USB_POWER;         /*!< 0x01, USB Power                                                */
	__IO unsigned short  USB_TXIS;          /*!< 0x02, USB Transmit Interrupt Status                            */
	__IO unsigned short  USB_RXIS;          /*!< 0x04, USB Receive Interrupt Status                             */
	__IO unsigned short  USB_TXIE;          /*!< 0x06, USB Transmit Interrupt Enable                            */
	__IO unsigned short  USB_RXIE;          /*!< 0x08, USB Receive Interrupt Enable                             */
	__IO unsigned char   USB_IS;            /*!< 0x0a, USB General Interrupt Status                          */
	__IO unsigned char   USB_IE;            /*!< 0x0b, USB Interrupt Enable                                  */
	__IO unsigned short  USB_FRAME;         /*!< 0x0c, USB Frame Value                                          */
	__IO unsigned char   USB_EPIDX;         /*!< 0x0e, USB Endpoint Index                                       */
	__IO unsigned char   USB_TEST;          /*!< 0x0f, USB Test Mode
	                                        */
//Indexed registers ? Peripheral mode (10h ? 1Fh)
#if 0
	__IO unsigned char   USB_EPIDX_CSR[16];
#else
	struct {
	  __IO unsigned short tx_maxp;       // 0x00: TXMAXP
	  union {
		__IO unsigned char USB_CSR0L;        // 0x02: CSR0
		__IO unsigned char tx_csrl;      // 0x02: TX CSRL
	  };
	  union {
		__IO unsigned char USB_CSR0H;        // 0x03: CSR0H
		__IO unsigned char tx_csrh;      // 0x03: TX CSRH
	  };

	  __IO unsigned short rx_maxp;       // 0x04: RX MAXP
	  __IO unsigned char  rx_csrl;       // 0x06: RX CSRL
	  __IO unsigned char  rx_csrh;       // 0x07: RX CSRH
	};

	union {
	__IO unsigned short USB_COUNT0;      // 0x08: COUNT0
	__IO unsigned short rx_count;    // 0x08: RX COUNT
	};
	union {
	__IO unsigned char USB_TYPE0;        // 0x0A: TYPE0 (host only)
	__IO unsigned char tx_type;      // 0x0A: TX TYPE
	};
	__IO unsigned char tx_interval;    // 0x0B: TX INTERVAL
	__IO unsigned char rx_type;        // 0x0C: RX TYPE
	__IO unsigned char rx_interval;    // 0x0D: RX INTERVAL
	__IO unsigned char reserved_0x0e;  // 0x0E: Reserved
	union {
	__IO unsigned char config_data0; // 0x0F: CONFIG DATA
	__IO unsigned char fifo_size;    // 0x0F: FIFO_SIZE
	};
#endif
//FIFOs (20h-C5Fh) -- This address range provides access to the endpoint FIFOs.
	union {
		union {
		__IO unsigned char   USB_FIFO0_BYTE;                      /*!< USB FIFO Endpoint 0                                                   */
		__IO unsigned short  USB_FIFO0_HALF;                      /*!< USB FIFO Endpoint 0                                                   */
		__IO unsigned int    USB_FIFO0_WORD;                      /*!< USB FIFO Endpoint 0                                                   */
		};
		union {
		__IO unsigned char   USB_FIFO1_BYTE;                      /*!< USB FIFO Endpoint 1                                                   */
		__IO unsigned short  USB_FIFO1_HALF;                      /*!< USB FIFO Endpoint 1                                                   */
		__IO unsigned int    USB_FIFO1_WORD;                      /*!< USB FIFO Endpoint 1                                                   */
		};
		union {
		__IO unsigned char   USB_FIFO2_BYTE;                      /*!< USB FIFO Endpoint 2                                                   */
		__IO unsigned short  USB_FIFO2_HALF;                      /*!< USB FIFO Endpoint 2                                                   */
		__IO unsigned int    USB_FIFO2_WORD;                      /*!< USB FIFO Endpoint 2                                                   */
		};
		union {
		__IO unsigned char   USB_FIFO3_BYTE;                      /*!< USB FIFO Endpoint 3                                                   */
		__IO unsigned short  USB_FIFO3_HALF;                      /*!< USB FIFO Endpoint 3                                                   */
		__IO unsigned int    USB_FIFO3_WORD;                      /*!< USB FIFO Endpoint 3                                                   */
		};
		union {
		__IO unsigned char   USB_FIFO4_BYTE;                      /*!< USB FIFO Endpoint 4                                                   */
		__IO unsigned short  USB_FIFO4_HALF;                      /*!< USB FIFO Endpoint 4                                                   */
		__IO unsigned int    USB_FIFO4_WORD;                      /*!< USB FIFO Endpoint 4                                                   */
		};
		union {
		__IO unsigned char   USB_FIFO5_BYTE;                      /*!< USB FIFO Endpoint 5                                                   */
		__IO unsigned short  USB_FIFO5_HALF;                      /*!< USB FIFO Endpoint 5                                                   */
		__IO unsigned int    USB_FIFO5_WORD;                      /*!< USB FIFO Endpoint 5                                                   */
		};
		union {
		__IO unsigned char   USB_FIFO6_BYTE;                      /*!< USB FIFO Endpoint 6                                                   */
		__IO unsigned short  USB_FIFO6_HALF;                      /*!< USB FIFO Endpoint 6                                                   */
		__IO unsigned int    USB_FIFO6_WORD;                      /*!< USB FIFO Endpoint 6                                                   */
		};
		union {
		__IO unsigned char   USB_FIFO7_BYTE;                      /*!< USB FIFO Endpoint 7                                                   */
		__IO unsigned short  USB_FIFO7_HALF;                      /*!< USB FIFO Endpoint 7                                                   */
		__IO unsigned int    USB_FIFO7_WORD;                      /*!< USB FIFO Endpoint 7                                                   */
		};
		__IO unsigned int    USB_FIFO[8];
	} ;//FIFOs(20h ~ 3Fh)
	__I  unsigned int  RESERVED1[8];						//FIFOs(40h ~ 5Fh)

//Additional Control & Configuration Registers (60h - 7Fh) -- These registers provide additional device status and control.
	__IO unsigned char   USB_DEVCTL;        /*!< 0x60, USB Device Control                                                    */
	__IO unsigned char   USB_MISC;          /*!< 0x61, USB Miscellaneous Register(Common Configuration)               */
	__IO unsigned char   USB_TXFIFOSZ;      /*!< 0x62, USB Transmit Dynamic FIFO Sizing                                      */
	__IO unsigned char   USB_RXFIFOSZ;      /*!< 0x63, USB Receive Dynamic FIFO Sizing                                       */
	__IO unsigned short  USB_TXFIFOADD;     /*!< 0x64, USB Transmit FIFO Start Address                                       */
	__IO unsigned short  USB_RXFIFOADD;     /*!< 0x66, USB Receive FIFO Start Address                                        */
	union {
		__O  unsigned int USB_VCONTROL;     /* 0x68~0x6b Returns details of core configuration. */
		__I  unsigned int USB_VSTATUS;      /* 0x68~0x6b Returns the configured size of the selected RX FIFO and TX FIFOs. */
	};
	__I  unsigned short  USB_HWVERS;        /* 0x6C Hardware Version Number Register.*/
	__IO unsigned short  RESERVED2;         /* 0x6E~0x6F Reserved. */
	__I  unsigned int    RESERVED3[2];		/* 0x70~0x77 Reserved. */
	__IO unsigned char   USB_EPINFO;        /*!< 0x78, USB Endpoint Information                                              */
	__IO unsigned char   USB_RAMINFO;       /*!< 0x79, USB RAM Information                                                   */
	__IO unsigned char   USB_CONTIM;        /*!< 0x7a, USB Connect Timing                                                    */
	__IO unsigned char   USB_VPLEN;         /*!< 0x7b, USB OTG VBUS Pulse Timing                                             */
	__IO unsigned char   USB_HSEOF;         /*!< 0x7c, USB High-Speed Last Transaction to End of Frame Timing                */
	__IO unsigned char   USB_FSEOF;         /*!< 0x7d, USB Full-Speed Last Transaction to End of Frame Timing                */
	__IO unsigned char   USB_LSEOF;         /*!< 0x7e, USB Low-Speed Last Transaction to End of Frame Timing                 */
	__IO unsigned char   USB_SOFTRST;		/* 0x7F Soft Reset. */

//Target Address Registers (80h - FFh) -- When the multipoint option is enabled in the configuration GUI, these registers provide target function and hub address details for each of the endpoints.
	__IO unsigned char   USB_TXFUNCADDR0;   /*!< 0x80, USB Transmit Functional Address Endpoint 0                            */
	__I  unsigned char   RESERVED4;			/* 0x81 Reserved. */
	__IO unsigned char   USB_TXHUBADDR0;    /*!< 0x82, USB Transmit Hub Address Endpoint 0                                   */
	__IO unsigned char   USB_TXHUBPORT0;    /*!< 0x83, USB Transmit Hub Port Endpoint 0                                      */
	__I  unsigned int    RESERVED5;			/*!< 0x84, The RxFuncAddr register does not exist on EP0.                        */
	struct{
		__IO unsigned char   USB_TXFUNCADDR;    /*!< 0x88+8*n(0~6), USB Transmit Functional Address Endpoint 1~7  */
		__I  unsigned char   USB_RESERVED6;
		__IO unsigned char   USB_TXHUBADDR;     /*!< 0x8a+8*n(0~6), USB Transmit Hub Address Endpoint 1~7  */
		__IO unsigned char   USB_TXHUBPORT;     /*!< 0x8b+8*n(0~6), USB Transmit Hub Port Endpoint 1~7                                      */
		__IO unsigned char   USB_RXFUNCADDR;    /*!< 0x8c+8*n(0~6), USB Receive Functional Address Endpoint 1~7                             */
		__I  unsigned char   USB_RESERVED7;
		__IO unsigned char   USB_RXHUBADDR;     /*!< 0x8e+8*n(0~6), USB Receive Hub Address Endpoint 1~7  */
		__IO unsigned char   USB_RXHUBPORT;     /*!< 0x8f+8*n(0~6), USB Receive Hub Port Endpoint 1~7                                       */
	}Hub_EP[7];
	__I  unsigned int  RESERVED8[16];		/* 0xC0~0xFF Reserved. */

//Indexed Endpoint Control/Status registers(100h ~ 1ffh),
//The registers available at 10h?1Fh, accessible independently of the setting of the Index register. 100h?10Fh EP0 registers; 110h?11Fh EP1 registers; 120h?12Fh EP2; and so on.
#if 1
	__IO unsigned char   USB_EP0_CSR[16];
#else
	__I  unsigned short  RESERVED9;			/* 0x100 ep0 nouse. */
	//__O  unsigned char   CSRL0;					/*!< USB Control and Status Endpoint 0 Low								 */
	//__O  unsigned char   CSRH0;					/*!< USB Control and Status Endpoint 0 High								 */
	__O  unsigned short  USB_CSR0;			/*!< 0x102, USB Control and Status Endpoint 0 16bit								 */
	__I  unsigned short  RESERVED10[2];
	__IO unsigned char   USB_COUNT0;		/*!< 0x108, USB Receive Byte Count Endpoint 0									 */
	__I  unsigned char   RESERVED11;		/* 0x109 ep0 nouse. */
	__IO unsigned char   USB_TYPE0;			/*!< 0x10a, USB Type Endpoint 0													 */
	__IO unsigned char   USB_NAKLMT0;		/*!< 0x10b, USB NAK Limit 0														 */
	__I  unsigned char   RESERVED12[3];		/* 0x10c~0x10e ep0 nouse. */
	__IO unsigned char   USB_CFGDATA0;		/*!< 0x10f, Returns details of core configuration.                            */
#endif
	struct{
		__IO unsigned short  USB_TXMAXP;        /*!< 0x110+0x10*n(0~6), USB Maximum Transmit Data Endpoint 1~7                            */
		__IO unsigned short  USB_TXCSR;			/*!< 0x112+0x10*n(0~6), USB Transmit Control and Status Endpoint 1~7                          */
		__IO unsigned short  USB_RXMAXP;        /*!< 0x114+0x10*n(0~6), USB Maximum Receive Data Endpoint 1~7                                   */
		__IO unsigned short  USB_RXCSR;         /*!< 0x116+0x10*n(0~6), USB Receive Control and Status Endpoint 1~7                                      */
		__IO unsigned short  USB_RXCOUNT;       /*!< 0x118+0x10*n(0~6), USB Receive Byte Count Endpoint 1~7                             */
		__IO unsigned char   USB_TXTYPE;	 	/*!< 0x11a+0x10*n(0~6), USB Host Transmit Configure Type Endpoint 1~7                            */
		__IO unsigned char   USB_TXINTERVAL;    /*!< 0x11b+0x10*n(0~6), USB Host Transmit Interval Endpoint 1~7                           */
		__IO unsigned char   USB_RXTYPE;        /*!< 0x11c+0x10*n(0~6), USB Receive Hub Address Endpoint 1~7                                    */
		__IO unsigned char   USB_RXINTERVAL;    /*!< 0x11d+0x10*n(0~6), USB Host Receive Polling Interval Endpoint 1~7                        */
		__I  unsigned char   USB_RESERVED11;
		__IO unsigned char   USB_FIFOSIZE;		/*!< 0x11f+0x10*n(0~6), Returns the configured size of the selected Rx FIFO and TX FIFOs 1~7									   */
	}Target_EP[7];
	__I  unsigned short  RESERVED13[64];	/* 0x10c~0x10e other ep nouse. */

//DMA Control/Status registers(200h~27Ch)
	__IO unsigned char   USB_DMAINTR;       /*!< 0x200, USB DMA Interrup, DMA channel 1~8                   */
	__I  unsigned char   RESERVED14[3];
	struct{
		__IO unsigned short  USB_DMACNTL;       /*!< 0x204+0x10*n(chl-1=0~7), USB DMA Control                    */
		__I  unsigned short  RESERVED15;
		__IO unsigned int    USB_DMAADDR;       /*!< 0x208+0x10*n(chl-1=0~7), USB DMA Address                    */
		__IO unsigned int    USB_DMACOUNT;      /*!< 0x20c+0x10*n(chl-1=0~7), USB DMA Count                      */
		__I  unsigned int    RESERVED16;
	}DMA_EP[8];
	__I  unsigned int  RESERVED17[32];

//RqPktCount Registers (304h ? 33Ch) ? These registers are used in Host mode in conjunction with AutoReq
	__IO unsigned int  RESERVED18;			/*!< 0x300, ep0 nouse 		  */
	struct{
		__IO unsigned short  USB_RQPKTCOUNT;    /*!< 0x304+0x4*n(ep-1=0~7), Number of requested packets for Receive Endpoint n 1~7           */
		__I  unsigned short  RESERVED19;
	}RPC_EP[7];
	__I  unsigned short  RESERVED20[16];	/*!< 0x320 ? 0x33f, other ep nouse 		  */
//DPktBufDis Registers (340h ? 343h) ? These registers provide direct user control over disabling double packet buffering.
	__IO unsigned short USB_RXDPBUFDIS;		/* 0x340, Double Packet Buffer Disable register for RX EPs 1 to 7. */
	__IO unsigned short USB_TXDPBUFDIS;		/* 0x342, Double Packet Buffer Disable register for TX EPs 1 to 7. */
//C_T_UCH Registers (344h ? 345h) ? These registers set the Chirp Timeout timer.
	__IO unsigned short USB_CTUCH;			/* 0x344, This register sets the Chirp Timeout Timer. */
//C_T_HSRTN Registers (346h ? 347h) ? These registers set the delay from the end of High Speed resume signaling to enabling UTM normal operating mode.
	__IO unsigned short USB_CTHSRTN;		/* 0x346, This register sets the delay from the end of High Speed resume signaling to enable UTM normal operating mode. */
	__IO unsigned short USB_CTHSBT;			/* 0x348, This register represents the value to be added to the mimumum high speed timeout period of 736 bit times. */
} USB_RegDef;


/*****************************************************************************
 * RGB - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO unsigned int RGB_CTRL;             /* 0x00 control register  */
	__IO unsigned int RGB_STATUS;           /* 0x04 status register */
	__IO unsigned int RGB_HTIM;             /* 0x08 Horizontal Timing Register */
	__IO unsigned int RGB_VTIM;             /* 0x0C Vertical Timing Register */
	__IO unsigned int RGB_HVLEN;            /* 0x10 Horizontal and Vertical Length Register */
	__IO unsigned int RGB_VB_ADDRA;         /* 0x14 Video Memory Base Address Register A */
	__IO unsigned int RGB_VB_ADDRB;         /* 0x18 Video Memory Base Address Register B */
	__IO unsigned int RGB_CLK_ENABLE;       /* 0x1C rgb enable */
	__IO unsigned int RGB_CLK_SCALER;       /* 0x20 rgb enable */
	__IO unsigned int RGB_VB_SIZEA;         /* 0x24 vedio dataA dma addr size */
	__IO unsigned int RGB_VB_SIZEB;         /* 0x28 vedio dataB dma addr size */
	__IO unsigned int RGB_DATA_SEL;  	    /* 0x2C Reserved */
	__IO unsigned int RGB_C0XY;             /* 0x30 AC-link Tx slot valid register  */
	__IO unsigned int RGB_C0BAR;            /* 0x34 AC-link Rx slot valid register */
         unsigned int RESERVED0[2];         /* 0x38 ~ 0x3C Reserved */
    __IO unsigned int RGB_C0CR[8];          /* 0x40 ~ 0x5C Cursor0 Color Registers */
         unsigned int RESERVED1[4];         /* 0x60 ~ 0x6C Reserved */
	__IO unsigned int RGB_C1XY;             /* 0x70 AC-link Tx slot valid register  */
	__IO unsigned int RGB_C1BAR;            /* 0x74 AC-link Rx slot valid register */
		 unsigned int RESERVED2[2];         /* 0x78 ~ 0x7C Reserved */
    __IO unsigned int RGB_C1CR[8];          /* 0x80 ~ 0x9C Cursor0 Color Registers */
		 unsigned int RESERVED3[472];       /* 0xA0 ~ 0x7FC Reserved */
    __IO unsigned int RGB_PCLT[512];        /* 0x800 ~ 0xFFC Cursor0 Color Registers */
}RGB_RegDef;

/*****************************************************************************
 * DVP - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO unsigned int DVP_CAPTURE;          /* 0x00 enable register  */
	__IO unsigned int DVP_ENABLE;           /* 0x04 enable register  */
	__IO unsigned int DVP_SCALER;           /* 0x08 scaler register */
	__IO unsigned int DVP_CONFIG;  	        /* 0x0C dvp mode  register */
	__IO unsigned int DVP_ENH_CFG;          /* 0x10 H signal config register  */
	__IO unsigned int DVP_DMA_BURST;        /* 0x14 dma burst register */
    __IO unsigned int DVP_DATA_FORMAT;      /* 0x18 data format register  */
    __IO unsigned int DVP_DE_CNT;           /* 0x1C Horizontal Sync Counter Register in DE Mode. */
	__IO unsigned int DVP_VS_TIMING;        /* 0x20 V signal config register */
    __IO unsigned int DVP_CW_ST;            /* 0x24 cut window start Registers */
    __IO unsigned int DVP_CW_SZ;            /* 0x28 cut window size Registers */
    __IO unsigned int DVP_HS_CF;            /* 0x2C HS CF Registers */
    __IO unsigned int DVP_VS_CF;            /* 0x30 VS CF Registers */
    __IO unsigned int DVP_FRF_CTRL;         /* 0x34 RF control Registers */
    __IO unsigned int DVP_BIN_TH;           /* 0x38 binary threshold Registers */
	__IO unsigned int DVP_FIFO;             /* 0x3C FIFO config Register */
	__IO unsigned int DVP_DP_ADDRA;         /* 0x40 BUFFER A Register */
	__IO unsigned int DVP_DP_CNTA;          /* 0x44 BUFFER count Register A */
	__IO unsigned int DVP_DP_ADDRB;         /* 0x48 BUFFER B Register */
	__IO unsigned int DVP_DP_CNTB;          /* 0x4C BUFFER count Register B */
	__IO unsigned int DVP_STATE;            /* 0x50 dvp state register  */
	__IO unsigned int DVP_INT_EN;         	/* 0x54 int enable register */
	__IO unsigned int DVP_INT_STS;         	/* 0x58 int status register */
	__IO unsigned int DVP_INT_CLR;          /* 0x5C int clear register */
}DVP_RegDef;

/*****************************************************************************
 * R8080 - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO unsigned int R8080_CS_EN;          /* 0x00 status register */
	__IO unsigned int R8080_SCALER_CFG;     /* 0x04 Horizontal Timing Register */
	__IO unsigned int R8080_CORE_CTRL;      /* 0x08 control register  */
	__IO unsigned int R8080_DATA_FMT;       /* 0x0C data format register */
	__IO unsigned int R8080_CMD_CFG;        /* 0x10 Video Memory Base Address Register A */
	__IO unsigned int R8080_WR_DATA;        /* 0x14 Reserved */
	__IO unsigned int R8080_FIFO_STS;       /* 0x18 FIFO config Register */
	__IO unsigned int R8080_BUF_ADDRA;      /* 0x1C Vertical Timing Register */
	__IO unsigned int R8080_BUF_CNTA;       /* 0x20 Vertical Timing Register */
	__IO unsigned int R8080_BUF_ADDRB;      /* 0x24 Vertical Timing Register */
	__IO unsigned int R8080_BUF_CNTB;       /* 0x28 Vertical Timing Register */
	__IO unsigned int R8080_CORE_STS;       /* 0x2C AC-link Tx slot valid register  */
	__IO unsigned int R8080_INT_EN;         /* 0x30 int enable register */
	__IO unsigned int R8080_INT_STS;        /* 0x34 int status register */
	__IO unsigned int R8080_INT_CLR;        /* 0x38 int clear register */
} R8080_RegDef;

/*****************************************************************************
 * SDIO - GTRY RISCV
 ****************************************************************************/
typedef struct {
	__IO unsigned int SDIO_SYS_CTRL;        /* 0x00 Control register */
	__IO unsigned int SDIO_POWER_EN;        /* 0x04 Power-enable register */
	__IO unsigned int SDIO_CLK_DIV;         /* 0x08 control register  */
	__IO unsigned int SDIO_CLK_SRC;         /* 0x0C Clock-source register */
	__IO unsigned int SDIO_CLK_EN;          /* 0x10 Clock-enable register */
	__IO unsigned int SDIO_TIME_OUT;        /* 0x14 Time-out register (number of card clock output clocks-Ccclk_out) */
	__IO unsigned int SDIO_CARD_TYPE;       /* 0x18 Card-type register */
	__IO unsigned int SDIO_BLOCK_SZ;        /* 0x1C Block-size register */
	__IO unsigned int SDIO_BYTE_CNT;        /* 0x20 Byte-count register */
	__IO unsigned int SDIO_INT_MASK;        /* 0x24 Interrupt-mask register */
	__IO unsigned int SDIO_CMD_ARG;         /* 0x2C Command register */
	__IO unsigned int SDIO_CMD_REG;         /* 0x28 Command-argument register */
	__IO unsigned int SDIO_RESPONSE[4];     /* 0x30-0x3C Response-0/1/2/3 register  */
	__IO unsigned int SDIO_MINT_STS;     	/* 0x40 Masked interrupt-status register  */
	__IO unsigned int SDIO_RINT_STS;     	/* 0x44 Raw interrupt-status register */
	__IO unsigned int SDIO_STATUS;          /* 0x48 Status register; mainly for debug purposes */
	__IO unsigned int SDIO_FIFO_TH;         /* 0x4C FIFO threshold register */
	__IO unsigned int SDIO_CARD_DET;        /* 0x50 Card-detect register */
	__IO unsigned int SDIO_WR_PROT;         /* 0x54 Write-protect register */
	__IO unsigned int RESERVED0;         /* 0x58 GPIO Input register */
	__IO unsigned int SDIO_TCIU_CNT;        /* 0x5C Transferred CIU card byte count */
	__IO unsigned int SDIO_TBIU_CNT;        /* 0x60 Transferred host/DMA to/from BIU-FIFO byte count */
	__IO unsigned int SDIO_DET_BNCE;        /* 0x64 Card detect debounce register (number of host clocks – clk) */
	__IO unsigned int SDIO_USR_ID;          /* 0x68 User ID register */
	__IO unsigned int SDIO_VER_ID;          /* 0x6C Synopsys version ID register */
	__IO unsigned int SDIO_H_CONFIG;        /* 0x70 Hardware configuration register(32'h5342_270a) */
	__IO unsigned int SDIO_UHS1_CFG;        /* 0x74 UHS-1 register */
	__IO unsigned int SDIO_RST_N;           /* 0x78 Hardware reset register */
	__IO unsigned int RESERVED1;         	/* 0x7C reserve1 register  */
	__IO unsigned int SDIO_BUS_MODE;        /* 0x80 Bus Mode Register; controls the Host Interface Mode. */
	__IO unsigned int SDIO_POLL_DEM;        /* 0x84 Poll Demand Register. Used by the host to instruct the IDMAC to poll the Descriptor List while in suspend. */
	__IO unsigned int SDIO_DLB_ADDR;        /* 0x88 Descriptor List Base Address Register. Points the IDMAC to the start of the Descriptor list. */
	__IO unsigned int SDIO_IDMA_STS;        /* 0x8C Internal DMA Status Register. */
	__IO unsigned int SDIO_IDMA_INT;        /* 0x90 Internal DMA Interrupt Enable Register. */
	__IO unsigned int SDIO_CHD_ADDR;        /* 0x94 Current Host Descriptor Address Register. Points to the start of current Descriptor read by the IDMAC. */
	__IO unsigned int SDIO_CHB_ADDR;        /* 0x98 Current Host Buffer Address Register. Points to the current Buffer address accessed by the IDMAC. */
	__IO unsigned int SDIO_CHB64_RES;       /* 0x9c res for Lower and Upper Register. */
	__IO unsigned int SDIO_CHBL_ADDR;       /* 0xa0 Current Host Buffer Address Lower Register. */
	__IO unsigned int SDIO_CHBU_ADDR;       /* 0xa4 Card clock enable Register. */
	__IO unsigned int SDIO_CARD_CLK_EN0;  	/* 0xa8 Card clock generate configuration Register. */
	__IO unsigned int SDIO_CARD_CLK_GEN0;  	/* 0xac Current Host Buffer Address Upper Register. */
	__IO unsigned int RESERVED2[20];        /* 0xB0-0xFC reserve1 register  */
	__IO unsigned int SDIO_CARD_TH_EN;      /* 0x100 Card Read/Write Threshold Enable (CardWrThrEn) */
	__IO unsigned int SDIO_BACK_PWR;        /* 0x104 Back-end Power */
	__IO unsigned int SDIO_UHS_EXT;         /* 0x108 eMMC 4.5 1.2V register */
	__IO unsigned int SDIO_EMMC_DDR;		/* 0x10c eMMC DDR START bit detection or HS400 mode enable control register */
	__IO unsigned int SDIO_SHIFT_CTRL;      /* 0x110 Phase shift control register */
	__IO unsigned int SDIO_CARD_CLK_EN;  	/* 0x114 Card clock generate configuration Register. */
	__IO unsigned int SDIO_CARD_CLK_GEN;  	/* 0x118 Current Host Buffer Address Upper Register. */
	__IO unsigned int SDIO_FIFOW_DMA_CTRL;  /* 0x11C  */
	__IO unsigned int SDIO_FIFOW_DMA_CNT;  	/* 0x120  */
	__IO unsigned int SDIO_FIFOR_DMA_CTRL;  /* 0x124  */
	__IO unsigned int SDIO_FIFOR_DMA_CNT;  	/* 0x128  */
	__IO unsigned int RESERVED3[53];  		/* 0x12C~0x1fc reserve3 register  */
	__IO unsigned int SDIO_DATA_FIFO[128];  /* 0x200~0x3fc Data FIFO read/write register */
} SDIO_RegDef;

/*****************************************************************************
 * Memory Map
 ****************************************************************************/

#define _IO_(addr)              (addr)

#define EILM_BASE               _IO_(0x00000000)
#define EDLM_BASE               _IO_(0x00200000)
#define SRAM_BASE               _IO_(0x00210000)
#define FLASH_BASE				_IO_(0x80000000)
#define BMC_BASE                _IO_(0xC0000000)
#define AHBDEC_BASE             _IO_(0xE0000000)
#define MAC_BASE                _IO_(0xE0100000)
#define LCDC_BASE               _IO_(0xE0200000)
#define SMC_BASE                _IO_(0xE0400000)
#define L2C_BASE                _IO_(0xE0500000)
#define PLIC_BASE               _IO_(0xE4000000)
#define PLMT_BASE               _IO_(0xE6000000)
#define PLIC_SW_BASE            _IO_(0xE6400000)
#define PLDM_BASE               _IO_(0xE6800000)
#define APBBRG_BASE             _IO_(0xF0000000)
#define SMU_BASE                _IO_(0xF0100000)
#define UART0_BASE              _IO_(0xF0200000)
#define UART1_BASE              _IO_(0xF0300000)
#define UART2_BASE              _IO_(0xF0400000)
#define TSC_BASE                _IO_(0xF0500000)
#define WDT_BASE                _IO_(0xF0600000)
#define RTC_BASE				_IO_(0xF0700000)
#define ARGB_BASE               _IO_(0xF0800000)
#define GPIO_BASE               _IO_(0xF0900000)
#define I2C0_BASE               _IO_(0xF0A00000)
#define I2C1_BASE               _IO_(0xF0C00000)
#define SPI0_BASE               _IO_(0xF0D00000)
#define SPI1_BASE               _IO_(0xF0E00000)
#define SPI2_BASE               _IO_(0xF0F00000)
#define SPI3_BASE               _IO_(0xF1000000)
#define SPI4_BASE               _IO_(0xF1100000)
#define SPI5_BASE               _IO_(0xF1200000)
#define SPI6_BASE               _IO_(0xF1300000)
#define DMA0_BASE               _IO_(0xF1400000)
#define DMA1_BASE               _IO_(0xF1500000)
#define PWM_BASE                _IO_(0xF1600000)
#define TIMER_BASE              _IO_(0xF1700000)
#define LTIM0_BASE				_IO_(0xF1800000)
#define TRNG_BASE               _IO_(0xF1900000)
#define IOAM_BASE               _IO_(0xF1A00000)
#define I2S_BASE                _IO_(0xF1B00000)
#define AES_BASE                _IO_(0xF1C00000)
#define LTIM1_BASE              _IO_(0xF1D00000)
#define ADC_BASE                _IO_(0xF1E00000)
#define ACMP_OPA_BASE        	_IO_(0xF1F00000)
#define DTROM_BASE              _IO_(0xF2000000)
#define USB_BASE                _IO_(0xE8000000)
#define RGB_BASE                _IO_(0xEA000000)
#define DVP_BASE                _IO_(0xEB000000)
#define R8080_BASE              _IO_(0xEC000000)
#define SDIO_BASE               _IO_(0xED000000)

/*****************************************************************************
 * Peripheral device declaration
 ****************************************************************************/
#define GTRY_L2C                ((L2C_RegDef *)  L2C_BASE)
#define GTRY_PLMT               ((PLMT_RegDef *) PLMT_BASE)
#define GTRY_SMU                ((SMU_RegDef *)  SMU_BASE)
#define GTRY_UART0              ((UART_RegDef *) UART0_BASE)
#define GTRY_UART1              ((UART_RegDef *) UART1_BASE)
#define GTRY_UART2              ((UART_RegDef *) UART2_BASE)
#define GTRY_TSC                ((TSC_RegDef *)  TSC_BASE)
#define GTRY_WDT                ((WDT_RegDef *)  WDT_BASE)
#define GTRY_RTC				((RTC_RegDef *)  RTC_BASE)
#define GTRY_ARGB				((ARGB_RegDef *)  ARGB_BASE)
#define GTRY_GPIO               ((GPIO_RegDef *) GPIO_BASE)
#define GTRY_I2C0               ((I2C_RegDef *)  I2C0_BASE)
#define GTRY_I2C1               ((I2C_RegDef *)  I2C1_BASE)
#define GTRY_SPI0               ((SPI_RegDef *)  SPI0_BASE)
#define GTRY_SPI1               ((SPI_RegDef *)  SPI1_BASE)
#define GTRY_SPI2               ((SPI_RegDef *)  SPI2_BASE)
#define GTRY_SPI3               ((SPI_RegDef *)  SPI3_BASE)
#define GTRY_SPI4               ((SPI_RegDef *)  SPI4_BASE)
#define GTRY_SPI5               ((SPI_RegDef *)  SPI5_BASE)
#define GTRY_SPI6               ((SPI_RegDef *)  SPI6_BASE)
#define GTRY_DMA0               ((DMA_RegDef *)  DMA0_BASE)
#define GTRY_DMA1               ((DMA_RegDef *)  DMA1_BASE)
#define GTRY_PWM                ((PWM_RegDef *)  PWM_BASE)
#define GTRY_TIMER              ((TIMER_RegDef *)  TIMER_BASE)
#define GTRY_LTIM0				((LTIM_RegDef *)  LTIM0_BASE)
#define GTRY_TRNG               ((TRNG_RegDef *)  TRNG_BASE)
#define GTRY_IOAM               ((IOAM_RegDef *)  IOAM_BASE)
#define GTRY_I2S                ((I2S_RegDef *)  I2S_BASE)
#define GTRY_AES				((AES_RegDef *)  AES_BASE)
#define GTRY_LTIM1				((LTIM_RegDef *)  LTIM1_BASE)
#define GTRY_ADC                ((ADC_RegDef *)  ADC_BASE)
#define GTRY_ACMP_OPA			((ACMP_OPA_RegDef *)  ACMP_OPA_BASE)
#define GTRY_USB                ((USB_RegDef *)  USB_BASE)
#define GTRY_RGB				((RGB_RegDef *)  RGB_BASE)
#define GTRY_DVP                ((DVP_RegDef *)  DVP_BASE)
#define GTRY_R8080              ((R8080_RegDef *)  R8080_BASE)
#define GTRY_SDIO               ((SDIO_RegDef *)  SDIO_BASE)

#endif	/* __ASSEMBLER__ */

/*
 * Define 'NDS_PLIC_BASE' and 'NDS_PLIC_SW_BASE' before include platform
 * intrinsic header file to active PLIC/PLIC_SW related intrinsic functions.
 */
#define NDS_PLIC_BASE        PLIC_BASE
#define NDS_PLIC_SW_BASE     PLIC_SW_BASE
#include "nds_v5_platform.h"

#ifdef __cplusplus
}
#endif

#endif	/* __GTRY_REGMAP_H__ */
