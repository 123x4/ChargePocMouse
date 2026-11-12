/*
 * Copyright (c) 2019 
 * All rights reserved.
 *
 */


#include "gtry_qspi_flash_drv.h"
#include "gtry_spi_drv.h"
#include "gtry_gpio_drv.h"
#include "Puya_Flash.h"

// 内层宏：负责拼接（此时参数已被外层宏展开）
#define _CPE_SPIBn_DMA_RX_EN(spi_no)	(GTRY_SPI##spi_no##_DMA_RX_EN)
#define _CPE_SPIBn_DMA_RX_CH(spi_no)	(GTRY_SPI##spi_no##_DMA_RX_CH)
#define _CPE_SPIBn_DMA_RX_ID(spi_no)	(GTRY_SPI##spi_no##_DMA_RX_REQID)
// 外层宏：先展开参数，再调用内层宏拼接
#define CPE_SPIBn_DMA_RX_EN(spi_no)		_CPE_SPIBn_DMA_RX_EN(spi_no)
#define CPE_SPIBn_DMA_RX_CH(spi_no)		_CPE_SPIBn_DMA_RX_CH(spi_no)
#define CPE_SPIBn_DMA_RX_ID(spi_no)		_CPE_SPIBn_DMA_RX_ID(spi_no)

// 内层宏：负责拼接（此时参数已被外层宏展开）
#define _CPE_SPIBn_BASE(spi_no)			(SPI##spi_no##_BASE)
// 外层宏：先展开参数，再调用内层宏拼接
#define CPE_SPIBn_BASE(spi_no)			_CPE_SPIBn_BASE(spi_no)

//#define USED_EN_TOKEN_READ_TRANS


#define FLASH_1MB_ID          		0x00146085
#define FLASH_32MB_ID         		0x0019405e
#define FLASH_128MB_ID        		0x002140ef
#define FLASH_PAGESIZE        		0x100//256B
#define FLASH_SECTORSIZE	  		0x1000//4KB
#define FLASH_TOTALSIZE	      		0x8000000//128MB-W25Q01JV//0x2000000//32MB-HG25Q256MW//0x100000//1MB-P25Q80SH


#define SPIB_TM_WRsim               0x0
#define SPIB_TM_WRonly              0x1
#define SPIB_TM_RDonly              0x2
#define SPIB_TM_WR_RD               0x3
#define SPIB_TM_RD_WR               0x4
#define SPIB_TM_WR_DY_RD            0x5
#define SPIB_TM_RD_DY_WR            0x6
#define SPIB_TM_NONE_DATA           0x7//None Data (must enable CmdEn or AddrEn in master mode)
#define SPIB_TM_DY_WR               0x8//Dummy, Write
#define SPIB_TM_DY_RD               0x9//Dummy, Read

#define SPIB_VERSION                0x02002000

/* SPIB register definition  */
#define SPIB_REG_VER        		(0x00)
#define SPIB_REG_IFSET      		(0x10)
#define SPIB_REG_PIO        		(0x14)
#define SPIB_REG_DCTRL      		(0x20)
#define SPIB_REG_CMD        		(0x24)
#define SPIB_REG_ADDR       		(0x28)
#define SPIB_REG_DATA       		(0x2c)
#define SPIB_REG_CTRL       		(0x30)
#define SPIB_REG_FIFOST     		(0x34)
#define SPIB_REG_INTEN      		(0x38)
#define SPIB_REG_INTST      		(0x3c)
#define SPIB_REG_REGTIMING  		(0x40)
#define SPIB_REG_EXTXCNT      		(0x6c)
#define SPIB_REG_EXRXCNT      		(0x70)


#define SPIB_IFSET_CPHA1_MASK   	0x00000001
#define SPIB_IFSET_CPOL1_MASK   	0x00000002
#define SPIB_IFSET_LSB_MASK   		0x00000008
#define SPIB_IFSET_DATAMERGE_MASK   0x00000080
#define SPIB_IFSET_3ADDR_MODE_MASK  0x00020000
#define SPIB_IFSET_4ADDR_MODE_MASK  0x00030000

/*-- Data Control Reg --*/
#define SPIB_DCTRL_CMDEN_MASK       0x40000000
#define SPIB_DCTRL_ADDREN_MASK      0x20000000
#define SPIB_DCTRL_TRAMODE_MASK     0x0f000000
#define SPIB_DCTRL_WCNT_MASK        0x001ff000
#define SPIB_DCTRL_TOKEN_EN_MASK    0x00200000
#define SPIB_DCTRL_TOKEN_VAL_0x0    0x00000000
#define SPIB_DCTRL_TOKEN_VAL_0x69   0x00000800
#define SPIB_DCTRL_DYCNT_S_8		0x0//Dummy data count. The actual dummy count is (DummyCnt +1).
#define SPIB_DCTRL_DYCNT_D_4		0x0
#define SPIB_DCTRL_DYCNT_Q_2		0x0
#define SPIB_DCTRL_DYCNT_Q_4		0x1
#define SPIB_DCTRL_DYCNT_Q_6		0x2
#define SPIB_DCTRL_DYCNT_MASK       0x00000600
#define SPIB_DCTRL_RCNT_MASK        0x000001ff
#define SPIB_DCTRL_EX_RCNT_MASK     0x003fffff
#define SPIB_DCTRL_CMDEN_OFFSET     30
#define SPIB_DCTRL_ADDREN_OFFSET    29
#define SPIB_DCTRL_TRAMODE_OFFSET   24
#define SPIB_DCTRL_WCNT_OFFSET      12
#define SPIB_DCTRL_DYCNT_OFFSET     9
#define SPIB_DCTRL_RCNT_OFFSET      0
#define SPIB_DCTRL_ADDRFMT_OFFSET   28
#define SPIB_DCTRL_ADDRFMT_SAME     (0x1ul<<SPIB_DCTRL_ADDRFMT_OFFSET)
#define SPIB_DCTRL_DUALQUAD_OFFSET  22
/*-- Control Reg --*/
#define SPIB_CTRL_TXFIFO_THRS(n)    (n<<16)
#define SPIB_CTRL_RXFIFO_THRS(n)    (n<<8)
#define SPIB_CTRL_TXDMAEN_MASK      0x00000010
#define SPIB_CTRL_RXDMAEN_MASK      0x00000008
#define SPIB_CTRL_TXFRST_MASK       0x00000004
#define SPIB_CTRL_RXFRST_MASK       0x00000002
#define SPIB_CTRL_SPIRST_MASK       0x00000001
/*-- FIFO Status Reg --*/
#define SPIB_FIFOST_TXFFL_MASK      0x00800000
#define SPIB_FIFOST_TXFEM_MASK      0x00400000
#define SPIB_FIFOST_TXFVE_MASK      0x001f0000
#define SPIB_FIFOST_RXFFL_MASK      0x00008000
#define SPIB_FIFOST_RXFEM_MASK      0x00004000
#define SPIB_FIFOST_RXFVE_MASK      0x00001f00
#define SPIB_FIFOST_SPIBSY_MASK     0x00000001
#define SPIB_FIFOST_TXFFL_OFFSET    23
#define SPIB_FIFOST_TXFEM_OFFSET    22
#define SPIB_FIFOST_TXFVE_OFFSET    16
#define SPIB_FIFOST_RXFFL_OFFSET    15
#define SPIB_FIFOST_RXFEM_OFFSET    14
#define SPIB_FIFOST_RXFVE_OFFSET    8
#define SPIB_FIFOST_SPIBSY_OFFSET   0
#define SPIB_FIFOST_SPIBSYnRXFEM    (SPIB_FIFOST_RXFEM_MASK|SPIB_FIFOST_SPIBSY_MASK)

/* Keep low-power cleanup bounded so a faulty SPI clock cannot block the CPU
 * before the deep-sleep WFI instruction. */
#define SPIB_SUSPEND_WAIT_TIMEOUT    100000U

/*SPI Interrupt Status Register*/
#define SPIB_INTSTS_ENDINT_MASK     0x00000010
#define SPIB_INTSTS_ALLINT_MASK     0x0000003F

#define spib_get_ifset()			*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_IFSET)
#define spib_set_ifset(setVal)		(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_IFSET) = setVal)
#define spib_get_pio()				*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_PIO)
#define spib_set_pio(setVal)		(*(unsigned int *)outw(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_PIO) = setVal)
#define spib_get_ctrl()				*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_CTRL)
#define spib_set_ctrl(setVal)		(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_CTRL) = setVal)
#define spib_get_fifost()			*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_FIFOST)
#define spib_get_inten()			*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_INTEN)
#define spib_set_inten(setVal)		(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_INTEN) = setVal)

#define spib_get_intst()			*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_INTST)
#define spib_set_intst(setVal)		(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_INTST) = setVal)
#define spib_get_dctrl()			*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_DCTRL)
#define spib_set_dctrl(setVal)		(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_DCTRL) = setVal)
#define spib_set_extxcnt(setVal)	(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_EXTXCNT) = setVal)
#define spib_set_exrxcnt(setVal)	(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_EXRXCNT) = setVal)
#define spib_get_cmd()				*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_CMD)
#define spib_set_cmd(setVal)		(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_CMD) = setVal)
#define spib_get_addr()				*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_ADDR)
#define spib_set_addr(setVal)		(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_ADDR) = setVal)
#define spib_get_data()				*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_DATA)
#define spib_set_data(setVal)		(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_DATA) = setVal)
#define spib_get_regtiming()		*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_REGTIMING)
#define spib_set_regtiming(setVal)	(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_REGTIMING) = setVal)

#define spib_get_version()			(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_VER))
#define spib_get_busy()				(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_FIFOST) & SPIB_FIFOST_SPIBSY_MASK)
#define spib_get_rx_empty()			(*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_FIFOST) & SPIB_FIFOST_RXFEM_MASK)
#define spib_get_rx_entries()		((*(unsigned int *)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_FIFOST) & SPIB_FIFOST_RXFVE_MASK) >> SPIB_FIFOST_RXFVE_OFFSET)

/*--------------------------------------------*/
/* SPI Flash ROM definition                   */
/*--------------------------------------------*/
#define SPIROM_ID_VERSION    FLASH_ID  /* PUYA P25Q80SH-0x00146085/P25Q40SH-0x00134285/P25Q20SH-0x00124285*/
#define SPIROM_ID_MASK       0x00ffffff
#define SPIROM_OP_READ				0x03        /*-- read --*/
#define SPIROM_OP_FAST_READ			0x0b        /*-- read in higher speed --*///no use
#define SPIROM_OP_FAST_4_READ		0x0c        /*-- read in higher speed --*///no use
#define SPIROM_OP_DUAL_READ			0x3b        /*-- read in dual speed --*///no use
#define SPIROM_OP_DUAL_2READ		0xbb        /*-- read in dual 2io speed --*///no use
#define SPIROM_OP_QUAD_2_2READ		0xbc        /*-- read in quad 4io speed --*///no use
#define SPIROM_OP_QUAD_READ			0x6b        /*-- read in quad speed --*///no use
#define SPIROM_OP_QUAD_4READ		0xeb        /*-- read in quad 4io speed --*///no use
#define SPIROM_OP_QUAD_4_4READ		0xec        /*-- read in quad 4io speed --*///no use
#define SPIROM_OP_RDID				0x9f        /*-- manufacturer/device ID read --*/
#define SPIROM_OP_READ_ID			0x90        /*-- manufacturer/device ID read --*/
#define SPIROM_OP_2READ_ID			0x92        /*-- manufacturer/device ID read --*/
#define SPIROM_OP_4READ_ID			0x94        /*-- manufacturer/device ID read --*/
#define SPIROM_OP_WREN       		0x06        /*-- write enable --*/
#define SPIROM_OP_WRENV      		0x50        /*-- Write Enable for Volatile Status Register --*/
#define SPIROM_OP_WRDI       		0x04        /*-- write disable --*/
#define SPIROM_OP_SE         		0x20        /*-- sector erase --*/
#define SPIROM_OP_BE32      	 	0x52        /*-- block erase 32KB --*/
#define SPIROM_OP_BE64       		0xD8        /*-- block erase 64KB --*/
#define SPIROM_OP_CE         		0xC7        /*-- chip erase --*/
#define SPIROM_OP_PP         		0x02        /*-- page program --*/
#define SPIROM_OP_RDSR       		0x05        /*-- read status register --*/
#define SPIROM_OP_RDSR1      		0x35        /*-- read status register-1 --*/
#define SPIROM_OP_RDSR2      		0x15        /*-- read status register-1 --*/
#define SPIROM_OP_WRSR       		0x01        /*-- write status register --*/
#define SPIROM_OP_WRSR1      		0x31        /*-- write status register-1 --*/
#define SPIROM_OP_WRSR2      		0x11        /*-- write status register-1 --*/
#define SPIROM_OP_SBLK       		0x36        /*-- single block lock --*/
#define SPIROM_OP_SBULK      		0x39        /*-- single block unlock --*/
#define SPIROM_OP_RDBLOCK    		0x3D
#define SPIROM_OP_RDCR       		0x15        /*--  Read out configure register --*/
#define SPIROM_OP_WRCR       		0x11        /*--  Write data to configuration register --*/
#define SPIROM_OP_GBLK       		0x7E
#define SPIROM_OP_GBULK      		0x98
#define SPIROM_OP_EQIO       		0x38
#define SPIROM_OP_RSTQIO     		0xFF
#define SPIROM_OP_4PP        		0x32
#define SPIROM_OP_RSTEN      		0x66
#define SPIROM_OP_RESET      		0x99
#define SPIROM_OP_DP         		0xB9        /*-- enter deep power-down --*/
#define SPIROM_OP_RDP        		0xAB        /*-- release from deep power-down --*/

/*-- bit mask for status register --*/
#define SPIROM_SR_WIP_MASK   		0x01   /*-- write in progress --*/
#define SPIROM_SR_WEL_MASK   		0x02   /*-- write enable --*/
#define SPIROM_SR_BP_MASK    		0x7C   /*-- BP protection mode --*/
#define SPIROM_SECTOR_SIZE   		FLASH_SECTORSIZE /*-- sector size in bytes, must be 4byte-aligned --*/
#define SPIROM_PAGE_SIZE     		FLASH_PAGESIZE  /*-- page size --*/
#define SPIROM_PAGEPROG_ADDR_MASK	0xFFFFFF00
/* Status Register-1 *//* security register */
#define SPIROM_SR_QE_MASK     		(0x01 << 1)
#define SPIROM_SR_EP_FAIL     		(0x01 << 2)
#define SPIROM_SR_ADP_MASK    		(0x01 << 1)
#define SPIROM_SR_ADS_MASK    		(0x01 << 0)
/* configuration register */
#define SPIROM_CR_WPS     	 		(0x04)

/*-- SPI ROM cmd --*/
#define SPIROM_CMD_READ      		0x0
#define SPIROM_CMD_DREAD     		0x10
#define SPIROM_CMD_QREAD     		0x20
#define SPIROM_CMD_RDID      		0x1
#define SPIROM_CMD_DRDID     		0x11
#define SPIROM_CMD_QRDID     		0x21
#define SPIROM_CMD_RDST      		0x2
#define SPIROM_CMD_WREN      		0x3
#define SPIROM_CMD_WRDI      		0x4
#define SPIROM_CMD_ERASE     		0x5
#define SPIROM_CMD_PROGRAM   		0x6
#define SPIROM_CMD_LOCK      		0x7
#define SPIROM_CMD_UNLOCK    		0x8
#define SPIROM_CMD_RDBLOCK   		0x9
#define SPIROM_CMD_RDSCUR    		0xA
#define SPIROM_CMD_RDST1     		0xB
#define SPIROM_CMD_RDST2     		0xBB
#define SPIROM_CMD_CHIP_UNLOCK		0xC
#define SPIROM_CMD_WRSR      		0xD
#define SPIROM_CMD_QUAD_ENA  		0xE
#define SPIROM_CMD_WRSR1     		0xF
#define SPIROM_CMD_WRSR2     		0xFF
#define SPIROM_CMD_RESET     		0x1F

#define SPIROM_REAL_SIZE     		FLASH_TOTALSIZE

#define FLASH_BLKSIZE    			SPIROM_SECTOR_SIZE
#define FLASH_CHIPSIZE   			SPIROM_REAL_SIZE
#define FLASH_RETRY_TIMES			0x100000 //0x1000

#if CPE_SPIBn_DMA_RX_EN(QSPI_FLASH_NO)

static unsigned int spib_dma_rddone_flag = 0;

static unsigned int spib_ctrl = 0;
static unsigned int spib_ifset = 0;
static unsigned int spib_suspend_ctrl = 0;
static unsigned int spib_suspend_ifset = 0;
static unsigned int spib_suspend_inten = 0;
static unsigned int spib_suspend_timing = 0;
static unsigned int spib_suspend_state_valid = 0;
static p_dma_int_cb spib_dma_cb_event = NULL;

static unsigned int spib_prepare_dctrl(
  unsigned int cmden,
  unsigned int addren,
  unsigned int tm,
  unsigned int wcnt,
  unsigned int dycnt,
  unsigned int rcnt)
{
	unsigned int v[8];
	unsigned int i;
	unsigned int dctrl = 0x0;

	v[0] = ((cmden << SPIB_DCTRL_CMDEN_OFFSET) & SPIB_DCTRL_CMDEN_MASK);
	v[1] = ((addren << SPIB_DCTRL_ADDREN_OFFSET) & SPIB_DCTRL_ADDREN_MASK);
	v[2] = ((tm << SPIB_DCTRL_TRAMODE_OFFSET) & SPIB_DCTRL_TRAMODE_MASK);
	v[3] = ((wcnt << SPIB_DCTRL_WCNT_OFFSET) & SPIB_DCTRL_WCNT_MASK);
	v[4] = ((dycnt << SPIB_DCTRL_DYCNT_OFFSET) & SPIB_DCTRL_DYCNT_MASK);
	v[5] = ((rcnt << SPIB_DCTRL_RCNT_OFFSET) & SPIB_DCTRL_RCNT_MASK);

	for (i = 0; i < 6; i++)
		dctrl |= v[i];
	return dctrl;
}

static unsigned int spib_wait_spi (void)
{
	unsigned int i;
	unsigned int timeout = FLASH_RETRY_TIMES;//100;

	for (i = 1; i < timeout; i++)
	{
	  if (spib_get_busy () == 0)
		  return 0;
	}
	return 1;
}

static void spib_clr_fifo (void)
{
	//unsigned int spib_ctrl = inw(SPIB_REG_CTRL);
	spib_ctrl |= (SPIB_CTRL_TXFRST_MASK | SPIB_CTRL_RXFRST_MASK);
	spib_set_ctrl(spib_ctrl);
}

static void spib_exe_cmmd (unsigned int op_addr, unsigned int spib_dctrl)
{
	/*-- execute command --*/
	spib_set_data(op_addr);        /*-- push flash command into tx fifo --*/
	spib_set_dctrl(spib_dctrl);    /*-- set dctrl --*/
	spib_set_cmd(0x0);             /*-- set dummy command to trigger transation start --*/
}

static void spib_en_exe_cmmd (unsigned int op_cmd, unsigned int op_addr, unsigned int spib_dctrl)
{
	/*-- execute command --*/
	spib_set_addr(op_addr);        /*-- push flash command into tx fifo --*/
	spib_set_dctrl(spib_dctrl);    /*-- set dctrl --*/
	spib_set_cmd(op_cmd);          /*-- set dummy command to trigger transation start --*/
}

static void spib_enex_exe_cmmd (unsigned int op_cmd, unsigned int op_addr, unsigned int spib_dctrl, unsigned int spib_rxcnt)
{
	/*-- execute command --*/
	spib_set_addr(op_addr);        /*-- push flash command into tx fifo --*/
#ifdef SPIB_DCTRL_EX_RCNT_MASK
	spib_set_exrxcnt(spib_rxcnt);
#endif
	spib_set_dctrl(spib_dctrl);    /*-- set dctrl --*/
	spib_set_cmd(op_cmd);          /*-- set dummy command to trigger transation start --*/
}

static unsigned int spirom_cmd_send (
    unsigned int cmd,
    unsigned int addr,
    unsigned int bytes,
    unsigned int *Retdata)
{
    unsigned int i;
    unsigned int timeout = FLASH_RETRY_TIMES;
    unsigned int spib_dctrl;
    unsigned int op_addr;
    unsigned int data = 0;
    unsigned int spib_rx_empty;

    /*-- wait if there is active transaction --*/
    if (spib_wait_spi() != 0)
    {
        *Retdata = 0;
        return 1;
    }
    /*-- clear tx/rx fifo --*/
    spib_clr_fifo();

    /*-- prepare opcode and address --*/
    if (cmd == SPIROM_CMD_READ)
    {
        /*-- execute command --*/
#if 0
    	op_addr = spirom_prepare_cmd(SPIROM_OP_READ, addr);
    	spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WR_RD, 3, 0, bytes-1);
    	spib_exe_cmmd(op_addr, spib_dctrl);
#else
#ifdef USED_EN_TOKEN_READ_TRANS
    	spib_dctrl = spib_prepare_dctrl (0x1, 0x1, SPIB_TM_RDonly, 0, 0, bytes-1);
    	spib_dctrl |= (SPIB_DCTRL_TOKEN_EN_MASK|SPIB_DCTRL_TOKEN_VAL_0x0);
    	spib_en_exe_cmmd(SPIROM_OP_FAST_READ, addr, spib_dctrl);
#else
    	spib_dctrl = spib_prepare_dctrl (0x1, 0x1, SPIB_TM_DY_RD, 0, SPIB_DCTRL_DYCNT_S_8, bytes-1);
    	spib_en_exe_cmmd(SPIROM_OP_FAST_READ, addr, spib_dctrl);
#endif
#endif
    }
    else if (cmd == SPIROM_CMD_DREAD)
    {
        /*-- execute command --*/
#ifdef USED_EN_TOKEN_READ_TRANS
        spib_dctrl = spib_prepare_dctrl (0x1, 0x1, SPIB_TM_RDonly, 0, 0, bytes-1);
        spib_dctrl |= (0x1ul<<SPIB_DCTRL_DUALQUAD_OFFSET)|(SPIB_DCTRL_TOKEN_EN_MASK|SPIB_DCTRL_TOKEN_VAL_0x0);//0x0: Regular (Single) mode/0x1: Dual I/O mode/0x2: Quad I/O mode
        spib_en_exe_cmmd(SPIROM_OP_DUAL_READ, addr, spib_dctrl);//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#else
        spib_dctrl = spib_prepare_dctrl (0x1, 0x1, SPIB_TM_DY_RD, 0, SPIB_DCTRL_DYCNT_D_4, bytes-1);
        spib_dctrl |= (0x1ul<<SPIB_DCTRL_DUALQUAD_OFFSET)|SPIB_DCTRL_ADDRFMT_SAME;//0x0: Regular (Single) mode/0x1: Dual I/O mode/0x2: Quad I/O mode
        spib_en_exe_cmmd(SPIROM_OP_DUAL_2READ, addr, spib_dctrl);//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#endif
    }
    else if (cmd == SPIROM_CMD_QREAD)
    {
        /*-- execute command --*/
#ifdef USED_EN_TOKEN_READ_TRANS
    	spib_dctrl = spib_prepare_dctrl (0x1, 0x1, SPIB_TM_RDonly, 0, 0, bytes-1);
        spib_dctrl |= (0x2ul<<SPIB_DCTRL_DUALQUAD_OFFSET)|(SPIB_DCTRL_TOKEN_EN_MASK|SPIB_DCTRL_TOKEN_VAL_0x0);//0x0: Regular (Single) mode/0x1: Dual I/O mode/0x2: Quad I/O mode
        spib_en_exe_cmmd(SPIROM_OP_QUAD_READ, addr, spib_dctrl);//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#else
        spib_dctrl = spib_prepare_dctrl (0x1, 0x1, SPIB_TM_DY_RD, 0, SPIB_DCTRL_DYCNT_Q_6, bytes-1);
        spib_dctrl |= (0x2ul<<SPIB_DCTRL_DUALQUAD_OFFSET)|SPIB_DCTRL_ADDRFMT_SAME;//0x0: Regular (Single) mode/0x1: Dual I/O mode/0x2: Quad I/O mode
        spib_en_exe_cmmd(SPIROM_OP_QUAD_4READ, addr, spib_dctrl);//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#endif
    }
    else if (cmd == SPIROM_CMD_RESET)
    {
        /*-- execute command --*/
        op_addr = SPIROM_OP_RSTEN;
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WRonly, 0, 0, 0);
		spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait completion --*/
        if (spib_wait_spi() != 0)
        {
            *Retdata = 0;
            return 1;
        }
        op_addr = SPIROM_OP_RESET;
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WRonly, 0, 0, 0);
		spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait completion --*/
        if (spib_wait_spi() != 0)
        {
            *Retdata = 0;
            return 2;
        }
    }
    else if (cmd == SPIROM_CMD_WREN)
    {
        /*-- execute command --*/
        op_addr = SPIROM_OP_WREN;
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WRonly, 0, 0, 0);
		spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait completion --*/
        if (spib_wait_spi() != 0)
        {
            *Retdata = 0;
            return 1;
        }
    }
    else if (cmd == SPIROM_CMD_WRDI)
    {
        /*-- execute command --*/
        op_addr = SPIROM_OP_WRDI;
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WRonly, 0, 0, 0);
        spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait completion --*/
        if (spib_wait_spi() != 0)
        {
            *Retdata = 0;
            return 1;
        }
    }
    else if (cmd == SPIROM_CMD_RDID)
    {
        /*-- execute command --*/
        op_addr = SPIROM_OP_RDID;
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WR_RD, 0, 0, 2);
        spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait data completion --*/
        for (i = 1; i < timeout; i++)
        {
            spib_rx_empty = spib_get_rx_empty ();
            if (spib_rx_empty == 0)
                break;
        }
        if (spib_rx_empty != 0)
        {
            *Retdata = 0;
            return 1;
        }
        data = spib_get_data();
    }
    else if (cmd == SPIROM_CMD_RDST)
    {
        /*-- execute command --*/
        op_addr = SPIROM_OP_RDSR;
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WR_RD, 0, 0, 0);
        spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait data completion --*/
        for (i = 1; i < timeout; i++)
        {
            spib_rx_empty = spib_get_rx_empty ();
            if (spib_rx_empty == 0)
                break;
        }
        if (spib_rx_empty != 0)
        {
            *Retdata = 0;
            return 1;
        }
        data = spib_get_data();
    }
    else if (cmd == SPIROM_CMD_RDBLOCK)
    {
        /*-- execute command --*/
        op_addr = SPIROM_OP_RDBLOCK;
        //op_addr = spirom_prepare_cmd (SPIROM_OP_RDBLOCK, addr);
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WR_RD, 3, 0, 0);
        spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait data completion --*/
        for (i = 1; i < timeout; i++)
        {
            spib_rx_empty = spib_get_rx_empty ();
            if (spib_rx_empty == 0)
                break;
        }
        if (spib_rx_empty != 0)
        {
            *Retdata = 0;
            return 1;
        }
        data = spib_get_data();
    }
    else if (cmd == SPIROM_CMD_RDSCUR)
    {
        /*-- execute command --*/
        op_addr = SPIROM_OP_RDCR;//SPIROM_OP_RDSCUR
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WR_RD, 0, 0, 0);
        spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait data completion --*/
        for (i = 1; i < timeout; i++)
        {
            spib_rx_empty = spib_get_rx_empty ();
            if (spib_rx_empty == 0)
                break;
        }
        if (spib_rx_empty != 0)
        {
            *Retdata = 0;
            return 1;
        }
        data = spib_get_data();
    }
    else if (cmd == SPIROM_CMD_RDST1)
    {
        /*-- execute command --*/
        op_addr = SPIROM_OP_RDSR1;
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WR_RD, 0, 0, 0);
        spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait data completion --*/
        for (i = 1; i < timeout; i++)
        {
            spib_rx_empty = spib_get_rx_empty ();
            if (spib_rx_empty == 0)
                break;
        }
        if (spib_rx_empty != 0)
        {
            *Retdata = 0;
            return 1;
        }
        data = spib_get_data();
    }
    else if (cmd == SPIROM_CMD_RDST2)
    {
        /*-- execute command --*/
        op_addr = SPIROM_OP_RDSR2;
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WR_RD, 0, 0, 0);
        spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait data completion --*/
        for (i = 1; i < timeout; i++)
        {
            spib_rx_empty = spib_get_rx_empty ();
            if (spib_rx_empty == 0)
                break;
        }
        if (spib_rx_empty != 0)
        {
            *Retdata = 0;
            return 1;
        }
        data = spib_get_data();
    }
    else if (cmd == SPIROM_CMD_CHIP_UNLOCK)
    {
        op_addr = SPIROM_OP_GBULK;
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WRonly, 0, 0, 0);
        spib_exe_cmmd(op_addr, spib_dctrl);

        if (spib_wait_spi() != 0)
        {
            *Retdata = 0;
            return 1;
        }
    }
    else if (cmd == SPIROM_CMD_WRSR)
    {
        /*-- execute command --*/
        op_addr = (SPIROM_OP_WRSR | (addr<<8));
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WRonly, 1, 0, 0);
        spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait completion --*/
        if (spib_wait_spi() != 0)
        {
            *Retdata = 0;
            return 1;
        }
    }
    else if (cmd == SPIROM_CMD_WRSR1)
    {
        /*-- execute command --*/
        op_addr = (SPIROM_OP_WRSR1 | (addr<<8));
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WRonly, 1, 0, 0);
        spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait completion --*/
        if (spib_wait_spi() != 0)
        {
            *Retdata = 0;
            return 1;
        }
    }
    else if (cmd == SPIROM_CMD_WRSR2)
    {
        /*-- execute command --*/
        op_addr = (SPIROM_OP_WRSR2 | (addr<<8));
        spib_dctrl = spib_prepare_dctrl (0x0, 0x0, SPIB_TM_WRonly, 1, 0, 0);
        spib_exe_cmmd(op_addr, spib_dctrl);

        /*-- wait completion --*/
        if (spib_wait_spi() != 0)
        {
            *Retdata = 0;
            return 1;
        }
    }
    else
    {
        *Retdata = 0;
        return 1;
    }

    *Retdata = data;
    return 0;
}

static int puya_unlock_chip(void)
{
    unsigned int j, RetData, timeout = FLASH_RETRY_TIMES;

    /*-- write enable --*/
    spirom_cmd_send (SPIROM_CMD_WREN, 0x0, 0, &RetData);

    /*-- get enable status --*/
    for (j = 1; j < timeout; j++)
    {
        spirom_cmd_send (SPIROM_CMD_RDST, 0x0, 0, &RetData);
        if ((RetData & SPIROM_SR_WIP_MASK) == 0)
            break;
    }
    if ((RetData & SPIROM_SR_WEL_MASK) == 0) {
    }
    /*-- Chip-UnLock --*/
    spirom_cmd_send (SPIROM_CMD_CHIP_UNLOCK, 0x0, 0, &RetData);

    /*-- get ULock status --*/
    for (j = 1; j < timeout; j++)
    {
        spirom_cmd_send (SPIROM_CMD_RDST, 0x0, 0, &RetData);
        if ((RetData & SPIROM_SR_WIP_MASK) == 0)
            break;
    }
    if ((RetData & SPIROM_SR_WIP_MASK) != 0)
    {
        return 1;
    }
    spirom_cmd_send (SPIROM_CMD_RDBLOCK, 0, 0, &RetData);

    if ((RetData & 0xFF) == 0xFF)
    {
        return 2;
    }
    return 0;
}


#define _spin_irq_handler(spi_no)	(spi##spi_no##_irq_handler)
#define spin_irq_handler(spi_no)    _spin_irq_handler(spi_no)

void spin_irq_handler(QSPI_FLASH_NO)(void)
{
	if(spib_get_intst()&SPIB_INTSTS_ENDINT_MASK)
	{
		spib_dma_rddone_flag = 0x1;
		dma_channel_disable(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO));
		dma_channel_clear_intsts(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO));
//		//while(dma_channel_get_intTC(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH) == 0x0);
//		dma_channel_clear_intsts(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH);
	}
	spib_set_intst (SPIB_INTSTS_ALLINT_MASK);
}

/**
  * @brief  QSPI_FLASH引脚初始化
  * @param  无
  * @retval 无
  */
unsigned int QSPI_DMA_FLASH_init(unsigned int spi_div, p_dma_int_cb cb_event)
{
    int stat = 0x0;

   //spib_dma_cb_event = cb_event;  /* Save the callback so resume can rebuild the DMA channel without reinitializing DMA0. */

	// set spi clk
    spib_set_regtiming((spib_get_regtiming() & (~0xFF)) | spi_div);

    spib_ctrl = spib_get_ctrl();
//	spib_ctrl |= /*SPIB_CTRL_RXFIFO_THRS(0x4)|*/SPIB_CTRL_RXFIFO_THRS(0x4);
//	spib_set_ctrl(spib_ctrl);
    spib_ifset = spib_get_ifset();

    // enable Data Merge
    spib_ifset |= (SPIB_IFSET_DATAMERGE_MASK|SPIB_IFSET_CPOL1_MASK|SPIB_IFSET_CPHA1_MASK);
    spib_set_ifset(spib_ifset);

    // enable endint interrupt
    spib_set_inten(SPIB_INTSTS_ENDINT_MASK);

    dma_initialize (DMA0_QSPI);

	// configure DMA channel
	stat = dma_channel_configure(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO),
				     (unsigned long)(CPE_SPIBn_BASE(QSPI_FLASH_NO)+SPIB_REG_DATA),
				     (unsigned long)(0x0ul),
				     0x0/*dma_rx_num*/,
				     DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)            |
				     //DMA width setting: 1 byte:0x0, 2 byte:0x1, 4 byte:0x2
					 DMA_CH_CTRL_SWIDTH(DMA_WIDTH_WORD) |
				     DMA_CH_CTRL_DWIDTH(DMA_WIDTH_WORD) |
				     DMA_CH_CTRL_SMODE_HANDSHAKE                |
				     DMA_CH_CTRL_SRCADDR_FIX                    |
				     DMA_CH_CTRL_DSTADDR_INC                    |
					 DMA_CH_CTRL_SRCREQ(CPE_SPIBn_DMA_RX_ID(QSPI_FLASH_NO))    |
				     DMA_CH_CTRL_EN_INTABT                         |
				     DMA_CH_CTRL_EN_INTERR                         |
					 (cb_event == NULL?DMA_CH_CTRL_DIS_INTTC:DMA_CH_CTRL_EN_INTTC)     |
					 0x0/*DMA_CH_CTRL_ENABLE*/,
					 cb_event);
	if (stat == -1)
	{
		dma_uninitialize (DMA0_QSPI);
	    return 1;
	}

	return 0;
}

// TODO: modify this function to target-specific function - check flash
unsigned int QSPI_DMA_FLASH_check(void)
{
    unsigned int result, RetData, SRData;
    unsigned int flash_id = 0x0;

	// disable RX DMA
    spib_ctrl &= (~SPIB_CTRL_RXDMAEN_MASK);
    spib_set_ctrl(spib_ctrl);


	result = spirom_cmd_send (SPIROM_CMD_RESET, 0x0, 0, &RetData);
    if (result != 0)
    {
        return 1;
    }
    ry_delay_us(100);
    result = spirom_cmd_send (SPIROM_CMD_RDID, 0x0, 0, &RetData);
    if (result != 0)
    {
        return 2;
    }
    flash_id = RetData;

#if EXT_FLASH_USE_QSPI
	/*-- spi flash read QE mode --*/
	spirom_cmd_send (SPIROM_CMD_RDST1, 0x0, 0, &SRData);
//    info("read1 status1 reg = 0x%x\r\n", RetData);
    if ((SRData&SPIROM_SR_QE_MASK) != SPIROM_SR_QE_MASK)
    {
    	/*-- spi flash write enable --*/
    	spirom_cmd_send (SPIROM_CMD_WREN, 0x0, 0, &RetData);
    	do{
    		//ry_delay_us(100);
    		spirom_cmd_send (SPIROM_CMD_RDST, 0x0, 0, &RetData);
    	}while(RetData & SPIROM_SR_WIP_MASK);
    	/*-- spi flash write QE mode --*/
    	result = spirom_cmd_send (SPIROM_CMD_WRSR1, (SRData&0xff)|SPIROM_SR_QE_MASK, 0, &RetData);
        if (result != 0)
        {
            return 5;
        }

    	do{
    		//ry_delay_us(100);
    		spirom_cmd_send (SPIROM_CMD_RDST, 0x0, 0, &RetData);
    	}while(RetData & SPIROM_SR_WIP_MASK);//SPIROM_SR_WEL_MASK
    	result = spirom_cmd_send (SPIROM_CMD_RDST1, 0x0, 0, &RetData);
        if ((RetData&SPIROM_SR_QE_MASK) != SPIROM_SR_QE_MASK)
        {
            return 6;
        }
    }
#endif

    if(flash_id == FLASH_32MB_ID || flash_id == FLASH_128MB_ID)
    {
    	/*-- spi flash read ADP mode --*/
    	spirom_cmd_send (SPIROM_CMD_RDST2, 0x0, 0, &SRData);
		if ((SRData & (SPIROM_SR_ADP_MASK|SPIROM_SR_ADS_MASK)) != (SPIROM_SR_ADP_MASK|SPIROM_SR_ADS_MASK))
		{
			/*-- spi flash write enable --*/
			spirom_cmd_send (SPIROM_CMD_WREN, 0x0, 0, &RetData);
			/*-- spi flash read RDST0 --*/
			do{
				//ry_delay_us(100);
				spirom_cmd_send (SPIROM_CMD_RDST, 0x0, 0, &RetData);
			}while(RetData & SPIROM_SR_WIP_MASK);//SPIROM_SR_WEL_MASK
			/*-- spi flash write ADP 4addr mode --*/
			result = spirom_cmd_send (SPIROM_CMD_WRSR2, (SRData&0xff)|SPIROM_SR_ADP_MASK, 0, &RetData);
			if (result != 0)
			{
				return 3;
			}
	    	do{
	    		//ry_delay_us(100);
	    		spirom_cmd_send (SPIROM_CMD_RDST, 0x0, 0, &RetData);
	    	}while(RetData & SPIROM_SR_WIP_MASK);//SPIROM_SR_WEL_MASK
			result = spirom_cmd_send (SPIROM_CMD_RDST2, 0x0, 0, &SRData);
			if ((SRData & (SPIROM_SR_ADP_MASK|SPIROM_SR_ADS_MASK)) != (SPIROM_SR_ADP_MASK|SPIROM_SR_ADS_MASK))
			{
				return 4;
			}
		}
    }

    // check if "WPSEL"==1, then do chip-unlock
    spirom_cmd_send (SPIROM_CMD_RDSCUR, 0x0, 0, &RetData);//spirom_cmd_send (SPIROM_CMD_RDSCUR, 0x0, 0, NULL, &RetData);
    if(RetData & SPIROM_CR_WPS)//if(RetData & 0x80)
    {
        puya_unlock_chip();
    }

	// enable RX DMA
    spib_ctrl |= (SPIB_CTRL_RXDMAEN_MASK);
    spib_set_ctrl(spib_ctrl);

    if(flash_id == FLASH_32MB_ID || flash_id == FLASH_128MB_ID)
    {
        // enable 4addr read mode
        spib_ifset |= (SPIB_IFSET_4ADDR_MODE_MASK);
    }
    else
    {
        // enable 3addr read mode
    	spib_ifset |= (SPIB_IFSET_3ADDR_MODE_MASK);
    }
    spib_set_ifset(spib_ifset);

    spib_dma_rddone_flag = 0x0;

    return flash_id;
}

void QSPI_DMA_FLASH_disable_rxdma(void)
{
	spib_ctrl &= ~SPIB_CTRL_RXDMAEN_MASK;  /* Clear only the SPIB RX DMA enable bit for an isolated suspend test. */
	spib_set_ctrl(spib_ctrl);              /* Commit the updated cached control value back to the SPIB hardware. */
}

unsigned int QSPI_DMA_FLASH_enter_deep_power_down(void)
{
	unsigned int dctrl;
	unsigned int inten;

	/* The 0xB9 command must be sent before SPI DMA/FIFO cleanup and before
	 * the pins are changed away from their SPI5 alternate function. */
	if (spib_wait_spi() != 0U)
	{
		return 1U;
	}

	/* Prevent END_INT from running the normal DMA completion handler while
	 * this power-state command is being sent. */
	inten = spib_get_inten();
	spib_set_inten(0U);
	spib_clr_fifo();
	spib_set_intst(SPIB_INTSTS_ALLINT_MASK);
	dctrl = spib_prepare_dctrl(0U, 0U, SPIB_TM_WRonly, 0U, 0U, 0U);
	spib_exe_cmmd(SPIROM_OP_DP, dctrl);

	if (spib_wait_spi() != 0U)
	{
		spib_set_inten(inten);
		return 2U;
	}

	spib_set_intst(SPIB_INTSTS_ALLINT_MASK);
	spib_set_inten(inten);

	/* Keep CS high for longer than the Flash deep-power-down entry time. */
	ry_delay_us(10U);
	return 0U;
}

void QSPI_DMA_FLASH_prepare_suspend(void)
{
#if EXT_FLASH_SUSPEND_STEP3_DISABLE_DMA
	const unsigned int dma_rx_channel = CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO);
#endif

	/* Save every register modified by suspend cleanup. Resume uses this exact
	 * snapshot instead of rerunning full Flash initialization. */
	spib_suspend_ctrl = spib_get_ctrl();
	spib_suspend_ifset = spib_get_ifset();
	spib_suspend_inten = spib_get_inten();
	spib_suspend_timing = spib_get_regtiming();
	spib_suspend_state_valid = 1U;

#if EXT_FLASH_SUSPEND_STEP3_DISABLE_DMA
	/* Step 3: clear the DMA channel enable bit even when no active transfer
	 * was present for the abort operation. */
	dma_channel_disable(DMA0_QSPI, dma_rx_channel);
#endif

	/* Synchronize the software cache with the actual hardware state. This is
	 * important while individual diagnostic steps are compiled out. */
	spib_ctrl = spib_get_ctrl();
}

unsigned int QSPI_DMA_FLASH_prepare_resume(void)
{
	unsigned int ctrl;
	unsigned int timeout;
	unsigned int dctrl;
	int stat;
	const unsigned int dma_rx_channel = CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO);

	if (spib_suspend_state_valid == 0U)
	{
		return 1U;
	}

	/* Restore format and timing before issuing any command. Keep DMA requests
	 * disabled until the DMA channel has been rebuilt. */
	spib_set_regtiming(spib_suspend_timing);
	spib_set_ifset(spib_suspend_ifset);
	ctrl = spib_suspend_ctrl & ~(SPIB_CTRL_TXDMAEN_MASK |
	                             SPIB_CTRL_RXDMAEN_MASK |
	                             SPIB_CTRL_TXFRST_MASK |
	                             SPIB_CTRL_RXFRST_MASK);
	spib_set_ctrl(ctrl);
	spib_ctrl = ctrl;

	/* Start from empty FIFOs and clear stale SPI/DMA status generated during
	 * the sleep transition. */
	spib_set_ctrl(ctrl | SPIB_CTRL_TXFRST_MASK | SPIB_CTRL_RXFRST_MASK);
	timeout = SPIB_SUSPEND_WAIT_TIMEOUT;
	while ((spib_get_ctrl() & (SPIB_CTRL_TXFRST_MASK | SPIB_CTRL_RXFRST_MASK)) &&
	       (--timeout != 0U))
	{
		/* Wait for the self-clearing FIFO reset bits. */
	}
	spib_set_intst(SPIB_INTSTS_ALLINT_MASK);
	dma_channel_clear_intsts(DMA0_QSPI, dma_rx_channel);

	/* Recreate DMA0 channel 3 without calling dma_initialize(), avoiding an
	 * extra DMA initialization reference on every suspend/resume cycle. */
	stat = dma_channel_configure(DMA0_QSPI, dma_rx_channel,
	                            (unsigned long)(CPE_SPIBn_BASE(QSPI_FLASH_NO) + SPIB_REG_DATA),
	                            (unsigned long)0U,
	                            0U,
	                            DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1) |
	                            DMA_CH_CTRL_SWIDTH(DMA_WIDTH_WORD) |
	                            DMA_CH_CTRL_DWIDTH(DMA_WIDTH_WORD) |
	                            DMA_CH_CTRL_SMODE_HANDSHAKE |
	                            DMA_CH_CTRL_SRCADDR_FIX |
	                            DMA_CH_CTRL_DSTADDR_INC |
	                            DMA_CH_CTRL_SRCREQ(CPE_SPIBn_DMA_RX_ID(QSPI_FLASH_NO)) |
	                            DMA_CH_CTRL_EN_INTABT |
	                            DMA_CH_CTRL_EN_INTERR |
	                            (spib_dma_cb_event == NULL ? DMA_CH_CTRL_DIS_INTTC : DMA_CH_CTRL_EN_INTTC),
	                            spib_dma_cb_event);
	if (stat != 0)
	{
		return 2U;
	}

	/* Keep SPI interrupts disabled while sending 0xAB. Otherwise END_INT can
	 * run the normal completion ISR and disable the newly rebuilt DMA channel. */
	spib_set_inten(0U);
	spib_set_intst(SPIB_INTSTS_ALLINT_MASK);

#if EXT_FLASH_DEEP_POWER_DOWN_ENABLE
	/* Send 0xAB only after SPI5 pins, registers, FIFO, and DMA are usable
	 * again. The command itself does not need an RX DMA request. */
	dctrl = spib_prepare_dctrl(0U, 0U, SPIB_TM_WRonly, 0U, 0U, 0U);
	spib_exe_cmmd(SPIROM_OP_RDP, dctrl);
	if (spib_wait_spi() != 0U)
	{
		return 3U;
	}

	spib_set_intst(SPIB_INTSTS_ALLINT_MASK);

	/* Allow the external Flash oscillator and read circuitry to restart. */
	ry_delay_us(30U);
#else
	(void)dctrl;
#endif

	spib_set_inten(spib_suspend_inten);

	/* Restore the original DMA request enables only after DMA0 channel 3 and
	 * the external Flash are ready to service them. */
	ctrl = spib_suspend_ctrl & ~(SPIB_CTRL_TXFRST_MASK | SPIB_CTRL_RXFRST_MASK);
	spib_set_ctrl(ctrl);
	spib_ctrl = ctrl;
	spib_ifset = spib_suspend_ifset;
	spib_dma_rddone_flag = 0U;
	spib_suspend_state_valid = 0U;
	return 0U;
}

// TODO: modify this function to target-specific function - read flash
unsigned int QSPI_DMA_FLASH_read(unsigned int FlashAddr, unsigned char *DataBuf, unsigned int DataSize)
{
#define SPIROM_OP_QUAD_4READ_DCTRL	(((0x1 << SPIB_DCTRL_CMDEN_OFFSET) & SPIB_DCTRL_CMDEN_MASK) \
									|((0x1 << SPIB_DCTRL_ADDREN_OFFSET) & SPIB_DCTRL_ADDREN_MASK) \
									|((SPIB_TM_DY_RD << SPIB_DCTRL_TRAMODE_OFFSET) & SPIB_DCTRL_TRAMODE_MASK) \
									|((0 << SPIB_DCTRL_WCNT_OFFSET) & SPIB_DCTRL_WCNT_MASK) \
									|((SPIB_DCTRL_DYCNT_Q_6 << SPIB_DCTRL_DYCNT_OFFSET) & SPIB_DCTRL_DYCNT_MASK) \
									|(0x2ul<<SPIB_DCTRL_DUALQUAD_OFFSET) \
									|(0x1ul<<SPIB_DCTRL_ADDRFMT_OFFSET))
	/*SPIB_DCTRL_DUALQUAD_OFFSET = 0x0: Regular (Single) mode/0x1: Dual I/O mode/0x2: Quad I/O mode*/

    unsigned int CurrSize;

    spib_dma_rddone_flag = 0x0;

    // clear interrupt status
    spib_set_intst (SPIB_INTSTS_ALLINT_MASK);

    /*-- SPIB_DCTRL_RCNT_MASK(0x1ff) --*/
	if(DataSize == 0x0)
	{
		return 1;
	}
#ifdef SPIB_DCTRL_EX_RCNT_MASK
	else if(DataSize >= (SPIB_DCTRL_EX_RCNT_MASK+1))
		CurrSize = (SPIB_DCTRL_EX_RCNT_MASK+1);
#else
	else if(DataSize >= (SPIB_DCTRL_RCNT_MASK+1))
		CurrSize = (SPIB_DCTRL_RCNT_MASK+1);
#endif
	else
		CurrSize = DataSize;
	dma_spichl_rx_reconfig(CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO), ((unsigned long)DataBuf), CurrSize>>DMA_WIDTH_WORD);

	/*-- wait if there is active transaction --*/
	if (spib_wait_spi() != 0)
	{
		return 1;
	}
	/*-- clear tx/rx fifo --*/
	spib_clr_fifo();
#ifdef SPIB_DCTRL_EX_RCNT_MASK
	spib_enex_exe_cmmd(SPIROM_OP_QUAD_4READ, FlashAddr, SPIROM_OP_QUAD_4READ_DCTRL|(0x0), CurrSize-1);//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#else
	spib_en_exe_cmmd(SPIROM_OP_QUAD_4READ, FlashAddr, SPIROM_OP_QUAD_4READ_DCTRL|(CurrSize-1));//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#endif

    return 0;
}

// TODO: modify this function to target-specific function - read flash
unsigned int QSPI_DMA_FLASH_4addr_read(unsigned int FlashAddr, unsigned char *DataBuf, unsigned int DataSize)
{
#define SPIROM_OP_QUAD_4_4READ_DCTRL	(((0x1 << SPIB_DCTRL_CMDEN_OFFSET) & SPIB_DCTRL_CMDEN_MASK) \
									|((0x1 << SPIB_DCTRL_ADDREN_OFFSET) & SPIB_DCTRL_ADDREN_MASK) \
									|((SPIB_TM_DY_RD << SPIB_DCTRL_TRAMODE_OFFSET) & SPIB_DCTRL_TRAMODE_MASK) \
									|((0 << SPIB_DCTRL_WCNT_OFFSET) & SPIB_DCTRL_WCNT_MASK) \
									|((SPIB_DCTRL_DYCNT_Q_6 << SPIB_DCTRL_DYCNT_OFFSET) & SPIB_DCTRL_DYCNT_MASK) \
									|(0x2ul<<SPIB_DCTRL_DUALQUAD_OFFSET) \
									|(0x1ul<<SPIB_DCTRL_ADDRFMT_OFFSET))
	/*SPIB_DCTRL_DUALQUAD_OFFSET = 0x0: Regular (Single) mode/0x1: Dual I/O mode/0x2: Quad I/O mode*/

    unsigned int CurrSize;

    spib_dma_rddone_flag = 0x0;

    // clear interrupt status
    spib_set_intst (SPIB_INTSTS_ALLINT_MASK);

    /*-- wait if there is active transaction --*/
    if (spib_wait_spi() != 0)
    {
        return 1;
    }
    /*-- SPIB_DCTRL_RCNT_MASK(0x1ff) --*/
	if(DataSize == 0x0)
	{
		return 1;
	}
#ifdef SPIB_DCTRL_EX_RCNT_MASK
	else if(DataSize >= (SPIB_DCTRL_EX_RCNT_MASK+1))
		CurrSize = (SPIB_DCTRL_EX_RCNT_MASK+1);
#else
	else if(DataSize >= (SPIB_DCTRL_RCNT_MASK+1))
		CurrSize = (SPIB_DCTRL_RCNT_MASK+1);
#endif
	else
		CurrSize = DataSize;

	dma_spichl_rx_reconfig(CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO), ((unsigned long)DataBuf), CurrSize>>DMA_WIDTH_WORD);

	/*-- clear tx/rx fifo --*/
	spib_clr_fifo();

#ifdef SPIB_DCTRL_EX_RCNT_MASK
	spib_enex_exe_cmmd(SPIROM_OP_QUAD_4_4READ, FlashAddr, SPIROM_OP_QUAD_4_4READ_DCTRL|(0x0), CurrSize-1);//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#else
	spib_en_exe_cmmd(SPIROM_OP_QUAD_4_4READ, FlashAddr, SPIROM_OP_QUAD_4_4READ_DCTRL|(CurrSize-1));//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#endif

    return 0;
}

unsigned int QSPI_DMA_FLASH_2addr_read(unsigned int FlashAddr, unsigned char *DataBuf, unsigned int DataSize)
{
#define SPIROM_OP_QUAD_2_2READ_DCTRL	(((0x1 << SPIB_DCTRL_CMDEN_OFFSET) & SPIB_DCTRL_CMDEN_MASK) \
									|((0x1 << SPIB_DCTRL_ADDREN_OFFSET) & SPIB_DCTRL_ADDREN_MASK) \
									|((SPIB_TM_DY_RD << SPIB_DCTRL_TRAMODE_OFFSET) & SPIB_DCTRL_TRAMODE_MASK) \
									|((0 << SPIB_DCTRL_WCNT_OFFSET) & SPIB_DCTRL_WCNT_MASK) \
									|((SPIB_DCTRL_DYCNT_D_4 << SPIB_DCTRL_DYCNT_OFFSET) & SPIB_DCTRL_DYCNT_MASK) \
									|(0x1ul<<SPIB_DCTRL_DUALQUAD_OFFSET) \
									|(0x1ul<<SPIB_DCTRL_ADDRFMT_OFFSET))
	/*SPIB_DCTRL_DUALQUAD_OFFSET = 0x0: Regular (Single) mode/0x1: Dual I/O mode/0x2: Quad I/O mode*/

    unsigned int CurrSize;

    spib_dma_rddone_flag = 0x0;

    // clear interrupt status
    spib_set_intst (SPIB_INTSTS_ALLINT_MASK);

    /*-- wait if there is active transaction --*/
    if (spib_wait_spi() != 0)
    {
        return 1;
    }
    /*-- SPIB_DCTRL_RCNT_MASK(0x1ff) --*/
	if(DataSize == 0x0)
	{
		return 1;
	}
#ifdef SPIB_DCTRL_EX_RCNT_MASK
	else if(DataSize >= (SPIB_DCTRL_EX_RCNT_MASK+1))
		CurrSize = (SPIB_DCTRL_EX_RCNT_MASK+1);
#else
	else if(DataSize >= (SPIB_DCTRL_RCNT_MASK+1))
		CurrSize = (SPIB_DCTRL_RCNT_MASK+1);
#endif
	else
		CurrSize = DataSize;

	dma_spichl_rx_reconfig(CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO), ((unsigned long)DataBuf), CurrSize>>DMA_WIDTH_WORD);

	/*-- clear tx/rx fifo --*/
	spib_clr_fifo();

#ifdef SPIB_DCTRL_EX_RCNT_MASK
	spib_enex_exe_cmmd(SPIROM_OP_QUAD_2_2READ, FlashAddr, SPIROM_OP_QUAD_2_2READ_DCTRL|(0x0), CurrSize-1);//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#else
	spib_en_exe_cmmd(SPIROM_OP_QUAD_2_2READ, FlashAddr, SPIROM_OP_QUAD_2_2READ_DCTRL|(CurrSize-1));//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#endif

    return 0;
}


// TODO: modify this function to target-specific function - read flash
unsigned int QSPI_DMA_FLASH_1read(unsigned int FlashAddr, unsigned char *DataBuf, unsigned int DataSize)
{
#define SPIROM_OP_QUAD_1READ_DCTRL	(((0x1 << SPIB_DCTRL_CMDEN_OFFSET) & SPIB_DCTRL_CMDEN_MASK) \
									|((0x1 << SPIB_DCTRL_ADDREN_OFFSET) & SPIB_DCTRL_ADDREN_MASK) \
									|((SPIB_TM_DY_RD << SPIB_DCTRL_TRAMODE_OFFSET) & SPIB_DCTRL_TRAMODE_MASK) \
									|((0 << SPIB_DCTRL_WCNT_OFFSET) & SPIB_DCTRL_WCNT_MASK) \
									|((SPIB_DCTRL_DYCNT_S_8 << SPIB_DCTRL_DYCNT_OFFSET) & SPIB_DCTRL_DYCNT_MASK) \
									|(0x0ul<<SPIB_DCTRL_DUALQUAD_OFFSET) \
									|(0x1ul<<SPIB_DCTRL_ADDRFMT_OFFSET))
	/*SPIB_DCTRL_DUALQUAD_OFFSET = 0x0: Regular (Single) mode/0x1: Dual I/O mode/0x2: Quad I/O mode*/

    unsigned int CurrSize;

    spib_dma_rddone_flag = 0x0;

    // clear interrupt status
    spib_set_intst (SPIB_INTSTS_ALLINT_MASK);

    /*-- SPIB_DCTRL_RCNT_MASK(0x1ff) --*/
	if(DataSize == 0x0)
	{
		return 1;
	}
#ifdef SPIB_DCTRL_EX_RCNT_MASK
	else if(DataSize >= (SPIB_DCTRL_EX_RCNT_MASK+1))
		CurrSize = (SPIB_DCTRL_EX_RCNT_MASK+1);
#else
	else if(DataSize >= (SPIB_DCTRL_RCNT_MASK+1))
		CurrSize = (SPIB_DCTRL_RCNT_MASK+1);
#endif
	else
		CurrSize = DataSize;

	dma_spichl_rx_reconfig(CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO), ((unsigned long)DataBuf), CurrSize>>DMA_WIDTH_WORD);

	/*-- wait if there is active transaction --*/
	if (spib_wait_spi() != 0)
	{
		return 1;
	}
	/*-- clear tx/rx fifo --*/
	spib_clr_fifo();
#ifdef SPIB_DCTRL_EX_RCNT_MASK
	spib_enex_exe_cmmd(SPIROM_OP_FAST_4_READ, FlashAddr, SPIROM_OP_QUAD_1READ_DCTRL|(0x0), CurrSize-1);//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#else
	spib_en_exe_cmmd(SPIROM_OP_FAST_4_READ/*SPIROM_OP_FAST_READ*/, FlashAddr, SPIROM_OP_QUAD_1READ_DCTRL|(CurrSize-1));//op_addr = spirom_prepare_cmd(SPIROM_OP_DUAL_READ, addr);
#endif

    return 0;
}



unsigned int QSPI_DMA_FLASH_dflag(void)
{
	if(spib_get_intst()&SPIB_INTSTS_ENDINT_MASK)
	{
		spib_dma_rddone_flag = 0x1;
		while(dma_channel_get_intTC(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO)) == 0x0);
		dma_channel_disable(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO));
		dma_channel_clear_intsts(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO));
		spib_set_intst (SPIB_INTSTS_ALLINT_MASK);
	}
	return spib_dma_rddone_flag;
}

unsigned int QSPI_DMA_FLASH_fend(void)
{
	spib_dma_rddone_flag = 0x1;
	//while(dma_channel_get_intTC(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH) == 0x0);
	dma_channel_disable(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO));
	dma_channel_clear_intsts(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO));
	spib_set_dctrl(0x0);
	spib_set_intst (SPIB_INTSTS_ALLINT_MASK);

	return spib_dma_rddone_flag;
}

////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief  ??Flash???(????????DMA)
 * @retval 0=??, 1=??
 */
static unsigned int ext_flash_write_enable(void)
{
	unsigned int RetData = 0;
	unsigned int ret;
	unsigned int retry = 0;

	// ??????????WREN
	for (retry = 0; retry < 3; retry++) {
		// ??WREN??
		ret = spirom_cmd_send(SPIROM_CMD_WREN, 0x0, 0, &RetData);
		if (ret != 0) {
			info("WREN cmd failed (attempt %d)\r\n", retry + 1);
			continue;
		}

		// ??QSPI????(????Flash busy,WREN???Flash busy)
		if (spib_wait_spi() != 0) {
			info("WREN wait timeout (attempt %d)\r\n", retry + 1);
			continue;
		}

		// ????,?Flash????WREN??
		for (volatile int i = 0; i < 100; i++);

		//??WEL??????
		spirom_cmd_send(SPIROM_CMD_RDST, 0x0, 0, &RetData);

		if ((RetData & SPIROM_SR_WEL_MASK) != 0) {
			// WEL???,??
			if (retry > 0) {
				info("WREN succeeded on attempt %d, SR1=0x%02X\r\n", retry + 1, RetData & 0xFF);
			}
			return 0;
		}

		// WEL???,??????
		info("WARN: WEL not set after WREN (attempt %d), SR1=0x%02X\r\n", retry + 1, RetData & 0xFF);

		// ??BP?
		if ((RetData & 0x7C) != 0) {
			info("ERROR: Write is blocked by BP bits=0x%02X!\r\n", (RetData >> 2) & 0x1F);
			return 1;  // BP??????,????
		}

		// ??FIFO???
		spib_clr_fifo();
	}

	// ???????
//	info("\r\n!!! FATAL: WREN failed after %d attempts! SR1=0x%02X !!!\r\n", retry, RetData & 0xFF);
//	info("*** HARDWARE WRITE PROTECTION DETECTED ***\r\n");
//	info("Flash chip is rejecting Write Enable (WREN) command.\r\n");
//	info("This means WP# pin (Pin 3) is pulled LOW or FLOATING.\r\n");
//	info("\r\n*** IMMEDIATE ACTION REQUIRED: ***\r\n");
//	info("1. Power off the board\r\n");
//	info("2. Check Pin 3 (WP#) - must be connected to 3.3V\r\n");
//	info("3. Check Pin 7 (HOLD#) - must be connected to 3.3V\r\n");
//	info("4. Use multimeter to verify pin voltages\r\n");
//	info("5. Check PCB schematic and verify pull-up resistors\r\n");
//	info("====================================\r\n\r\n");
	return 1;
}


static unsigned int ext_flash_wait_busy(void)
{
	unsigned int RetData = 0;
	unsigned int timeout = 1000000;
	unsigned int ret;
	unsigned int dbg_cnt = 0;

	do {
		ret = spirom_cmd_send(SPIROM_CMD_RDST, 0x0, 0, &RetData);

		if (dbg_cnt < 3) {
//			info("WB[%d]: ret=%d SR=0x%02X\r\n", dbg_cnt, ret, RetData & 0xFF);
			dbg_cnt++;
		}

		if (ret != 0) {
			timeout--;
			continue;
		}

		if ((RetData & SPIROM_SR_WIP_MASK) == 0) {
			return 0;
		}
		timeout--;
	} while(timeout > 0);

//	info("WB: TIMEOUT last SR=0x%02X\r\n", RetData & 0xFF);
	return 1;
}


#if 1
/**
 * @brief  Erasing the external Flash sector (4KB)
 * @param  sector_addr: Sector address (must be 4KB aligned)?0x0000, 0x1000, 0x2000...)
 * @retval 0 = Success, any other value ? 0 = Failure
 */

unsigned int QSPI_EXT_FLASH_erase_sector(unsigned int sector_addr)
{
	unsigned int op_addr = 0;
	unsigned int spib_dctrl = 0;
	unsigned int spib_ctrl_save = spib_ctrl;

	spib_ctrl &= ~SPIB_CTRL_RXDMAEN_MASK;
	spib_set_ctrl(spib_ctrl);

	spib_clr_fifo();
	spib_set_intst(SPIB_INTSTS_ALLINT_MASK);

	if (spib_wait_spi() != 0) {
		spib_clr_fifo();
		if (spib_wait_spi() != 0) {
			spib_ctrl = spib_ctrl_save;
			spib_set_ctrl(spib_ctrl);
			return 1;
		}
	}

	if (ext_flash_write_enable() != 0) {
		unsigned int SR1 = 0;
		spirom_cmd_send(SPIROM_CMD_RDST, 0x0, 0, &SR1);
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 2;
	}

	op_addr = (SPIROM_OP_SE | (sector_addr << 8));
	spib_dctrl = spib_prepare_dctrl(0x0, 0x0, SPIB_TM_WRonly, 3, 0, 0);
	spib_exe_cmmd(op_addr, spib_dctrl);

	if (spib_wait_spi() != 0) {
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 3;
	}

	if (ext_flash_wait_busy() != 0) {
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 4;
	}

	spib_clr_fifo();
	spib_ctrl = spib_ctrl_save;
	spib_set_ctrl(spib_ctrl);
	return 0;
}
unsigned int QSPI_EXT_FLASH_erase_block_32k(unsigned int block_addr)
{
	unsigned int op_addr = 0;
	unsigned int spib_dctrl = 0;

	unsigned int spib_ctrl_save = spib_ctrl;
	spib_ctrl &= ~SPIB_CTRL_RXDMAEN_MASK;
	spib_set_ctrl(spib_ctrl);


	spib_clr_fifo();

	if (spib_wait_spi() != 0) {
		spib_clr_fifo();
		if (spib_wait_spi() != 0) {
			spib_ctrl = spib_ctrl_save;
			spib_set_ctrl(spib_ctrl);
			return 1;
		}
	}

	if (ext_flash_write_enable() != 0) {
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 2;
	}

	op_addr = (SPIROM_OP_BE32 | (block_addr << 8));
	spib_dctrl = spib_prepare_dctrl(0x0, 0x0, SPIB_TM_WRonly, 3, 0, 0);
	spib_exe_cmmd(op_addr, spib_dctrl);

	if (spib_wait_spi() != 0) {
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 3;
	}

	if (ext_flash_wait_busy() != 0) {
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 4;
	}

	spib_clr_fifo();
	spib_ctrl = spib_ctrl_save;
	spib_set_ctrl(spib_ctrl);
	return 0;
}

/**
 * @brief  Erasing 64KB block of external Flash
 * @param  block_addr: Block Address (must be 64KB aligned),?0x0000, 0x10000, 0x20000...)
 * @retval 0 = Success, any other value ? 0 = Failure
 */
unsigned int QSPI_EXT_FLASH_erase_block_64k(unsigned int block_addr)
{
	unsigned int op_addr = 0;
	unsigned int spib_dctrl = 0;

	unsigned int spib_ctrl_save = spib_ctrl;
	spib_ctrl &= ~SPIB_CTRL_RXDMAEN_MASK;
	spib_set_ctrl(spib_ctrl);


	spib_clr_fifo();

	if (spib_wait_spi() != 0) {
		spib_clr_fifo();
		if (spib_wait_spi() != 0) {
			spib_ctrl = spib_ctrl_save;
			spib_set_ctrl(spib_ctrl);
			return 1;
		}
	}

	if (ext_flash_write_enable() != 0) {
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 2;
	}

	op_addr = (SPIROM_OP_BE64 | (block_addr << 8));
	spib_dctrl = spib_prepare_dctrl(0x0, 0x0, SPIB_TM_WRonly, 3, 0, 0);
	spib_exe_cmmd(op_addr, spib_dctrl);

	if (spib_wait_spi() != 0) {
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 3;
	}

	if (ext_flash_wait_busy() != 0) {
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 4;
	}

	spib_clr_fifo();
	spib_ctrl = spib_ctrl_save;
	spib_set_ctrl(spib_ctrl);
	return 0;
}

/**
 * @brief  Erase the entire external Flash (P25Q80SH = 1MB)
 * @retval 0 = Success, any other value ? 0 = Failure
 */
unsigned int QSPI_EXT_FLASH_erase_chip(void)
{
	unsigned int op_addr = 0;
	unsigned int spib_dctrl = 0;

	unsigned int spib_ctrl_save = spib_ctrl;
	spib_ctrl &= ~SPIB_CTRL_RXDMAEN_MASK;
	spib_set_ctrl(spib_ctrl);


	spib_clr_fifo();

	if (spib_wait_spi() != 0) {
		spib_clr_fifo();
		if (spib_wait_spi() != 0) {
			spib_ctrl = spib_ctrl_save;
			spib_set_ctrl(spib_ctrl);
			return 1;
		}
	}

	if (ext_flash_write_enable() != 0) {
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 2;
	}

	op_addr = SPIROM_OP_CE;
	spib_dctrl = spib_prepare_dctrl(0x0, 0x0, SPIB_TM_WRonly, 0, 0, 0);
	spib_exe_cmmd(op_addr, spib_dctrl);

	if (spib_wait_spi() != 0) {
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 3;
	}

	if (ext_flash_wait_busy() != 0) {
		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
		return 4;
	}

	spib_clr_fifo();
	spib_ctrl = spib_ctrl_save;
	spib_set_ctrl(spib_ctrl);
	return 0;
}

/**
 * @brief  Write to the external Flash page (up to 256 bytes)
 * @param  page_addr: ???
 * @param  data: ?????
 * @param  size: ????(<=256??)
 * @retval 0=??, ?0=??
 */
// ????:??????(??spib_exe_cmmd??,?????)
static unsigned int write_small_chunk(unsigned int addr, const uint8_t *data, unsigned int size)
{
	unsigned int spib_dctrl = 0;
	unsigned int i;
	unsigned int tx_data;
	unsigned int byte_idx = 0;
	unsigned int timeout;
//	unsigned int op_addr;
	static unsigned int debug_first_chunk = 1;  // ??????chunk

	// ??QSPI??
	if (spib_wait_spi() != 0) {
		return 1;
	}

	spib_clr_fifo();

	if (ext_flash_write_enable() != 0) {
		return 2;
	}

	// ??:?????chunk?????????chunk???
	if ((debug_first_chunk && addr == 0) || addr == 0x025800) {
		if (addr == 0) {
			debug_first_chunk = 0;
		}
	}

	// ??FIFO
	spib_clr_fifo();

	// ===== ??:?????? =====
	if (addr >= 0x200000) {  // 2MB = 0x200000
		info("ERROR: Address 0x%06X out of range for 2MB Flash!\r\n", addr);
		return 6;
	}

	// ===== ????:???????,??????? =====
	// Page Program: CMD(1) + ADDR(3) + DATA(n)
	// ?? cmden=1, addren=1 ??(CMD?ADDR????)

	// ??:????(?????)
//	if (addr == 0 || addr == 0x025800) {
//		info(">>>>>> NEW CODE: Using address register mode, addr=0x%06X, size=%d\r\n", addr, size);
//	}

	// 1. ??????FIFO(???CMD?ADDR)
	unsigned int prefill_words = 3;  // FIFO??4word,?????
	if ((size + 3) / 4 < prefill_words) {
		prefill_words = (size + 3) / 4;
	}

	for (i = 0; i < prefill_words; i++) {
		tx_data = 0;
		if (byte_idx < size) tx_data |= data[byte_idx++];
		if (byte_idx < size) tx_data |= (data[byte_idx++] << 8);
		if (byte_idx < size) tx_data |= (data[byte_idx++] << 16);
		if (byte_idx < size) tx_data |= (data[byte_idx++] << 24);
		spib_set_data(tx_data);
	}

	// 2. ??DCTRL?????
	// cmden=1 (??CMD???), addren=1 (??ADDR???)
	// wcnt = size - 1 (???????)
	spib_dctrl = spib_prepare_dctrl(0x1, 0x1, SPIB_TM_WRonly, size - 1, 0, 0);
	spib_set_addr(addr);  // ???????
	spib_set_dctrl(spib_dctrl);
	spib_set_cmd(SPIROM_OP_PP);  // ????????????

	// 4. ??????????
	unsigned int data_words = (size + 3) / 4;
	for (i = prefill_words; i < data_words; i++) {
		// ??FIFO???
		timeout = 100000;
		while ((spib_get_fifost() & SPIB_FIFOST_TXFFL_MASK) != 0) {
			timeout--;
			if (timeout == 0) {
				info("PP FIFO full timeout!\r\n");
				return 3;
			}
		}

		tx_data = 0;
		if (byte_idx < size) tx_data |= data[byte_idx++];
		if (byte_idx < size) tx_data |= (data[byte_idx++] << 8);
		if (byte_idx < size) tx_data |= (data[byte_idx++] << 16);
		if (byte_idx < size) tx_data |= (data[byte_idx++] << 24);
		spib_set_data(tx_data);
	}

	// 5. ??????
	if (spib_wait_spi() != 0) {
		info("PP wait timeout!\r\n");
		return 4;
	}

	// 6. ??Flash??????
	if (ext_flash_wait_busy() != 0) {
		info("PP: Flash busy timeout!\r\n");
		return 5;
	}

	// 7. ????????(??SR1)
	unsigned int SR1_check = 0;
	spirom_cmd_send(SPIROM_CMD_RDST, 0x0, 0, &SR1_check);

	// ???????????(??Flash?P-FAIL?)
	if (SR1_check & 0x60) {  // ??bit5?bit6
		info("PP: Program failed! SR1=0x%02X (addr=0x%06X)\r\n", SR1_check & 0xFF, addr);
	}

	// ??FIFO
	spib_clr_fifo();

	return 0;
}
/**
 * @brief  ??????Flash??(????)
 * @param  flash_addr: Flash??
 * @param  buf: ?????
 * @param  size: ?????(??16)
 * @retval ????????
 */
unsigned int debug_read_external_flash(unsigned int flash_addr, uint8_t *buf, unsigned int size)
{
	unsigned int i;
	unsigned int spib_rx_empty;
	unsigned int rx_data;
	unsigned int rx_words;
	unsigned int byte_idx = 0;
	unsigned int spib_dctrl;
	unsigned int op_addr;
	unsigned int timeout;

	if (size > 16) size = 16;

	unsigned int spib_ctrl_save = spib_ctrl;
	spib_ctrl &= ~SPIB_CTRL_RXDMAEN_MASK;
	spib_set_ctrl(spib_ctrl);


	spib_clr_fifo();

	if (spib_wait_spi() != 0) {
		spib_clr_fifo();
		if (spib_wait_spi() != 0) {
			spib_ctrl = spib_ctrl_save;
			spib_set_ctrl(spib_ctrl);
			return 0;
		}
	}

	spib_clr_fifo();
	// op_addr = CMD | (ADDR << 8)
	op_addr = 0x03 | (flash_addr << 8);  // 0x03 = ??????

	// ??DCTRL: cmden=0, addren=0, tm=WR_RD, wcnt=3 (??4??CMD+ADDR), rcnt=size-1
	spib_dctrl = spib_prepare_dctrl(0x0, 0x0, SPIB_TM_WR_RD, 3, 0, size - 1);

	// ??spib_exe_cmmd??
	spib_set_data(op_addr);
	spib_set_dctrl(spib_dctrl);
	spib_set_cmd(0x0);  // ????

	// ??????
	rx_words = (size + 3) / 4;

	for (i = 0; i < rx_words; i++) {
		timeout = 50000;
		do {
			spib_rx_empty = spib_get_rx_empty();
			if (spib_rx_empty == 0) break;
			timeout--;
		} while (timeout > 0);

		if (timeout == 0) {
			info("Read: RX timeout at word %d, FIFOST=0x%08X\r\n", i, spib_get_fifost());
			break;
		}

		rx_data = spib_get_data();

		// ??:????32???
//		if (i == 0) {
//			info("Raw rx_data[0]=0x%08X\r\n", rx_data);
//		}

		// ??32????4???
		if (byte_idx < size) buf[byte_idx++] = (rx_data >> 0) & 0xFF;
		if (byte_idx < size) buf[byte_idx++] = (rx_data >> 8) & 0xFF;
		if (byte_idx < size) buf[byte_idx++] = (rx_data >> 16) & 0xFF;
		if (byte_idx < size) buf[byte_idx++] = (rx_data >> 24) & 0xFF;
	}

	spib_clr_fifo();
	spib_ctrl = spib_ctrl_save;
	spib_set_ctrl(spib_ctrl);

	return byte_idx;
}
/**
 * @brief  Read arbitrary length data from external Flash.
 *         Self-contained SPI read,
 *         Reads in 16-byte chunks due to SPI RX FIFO depth (4 words = 16 bytes).
 * @param  flash_addr: start address in Flash
 * @param  buf: output buffer (caller must ensure sufficient size)
 * @param  size: number of bytes to read (no upper limit)
 * @retval actual number of bytes read
 */

/**
 * Pack READ command + 24-bit address into a 32-bit word for TX FIFO.
 * SPI Flash expects bytes on the wire in order: CMD, A[23:16], A[15:8], A[7:0].
 * The ATCSPI200 TX FIFO sends the lowest byte first (little-endian), so we
 * must place A[23:16] in byte-1, A[15:8] in byte-2, A[7:0] in byte-3.
 */
static unsigned int flash_read_op_addr(unsigned int addr)
{
	return 0x03                              /* byte 0: READ command */
		| (((addr >> 16) & 0xFF) <<  8)      /* byte 1: A[23:16] */
		| (((addr >>  8) & 0xFF) << 16)      /* byte 2: A[15:8]  */
		| (( addr        & 0xFF) << 24);     /* byte 3: A[7:0]   */
}

unsigned int QSPI_EXT_FLASH_read(unsigned int flash_addr, uint8_t *buf, unsigned int size)
{
	unsigned int total_read = 0;

	/* Save and disable RX DMA so we can do PIO reads */
	unsigned int spib_ctrl_save = spib_ctrl;
	spib_ctrl &= ~SPIB_CTRL_RXDMAEN_MASK;
	spib_set_ctrl(spib_ctrl);

	while (total_read < size) {
		/* Chunk size: max 16 bytes per SPI transaction (FIFO = 4 x 32-bit) */
		unsigned int chunk = size - total_read;
		if (chunk > 16) chunk = 16;

		unsigned int cur_addr = flash_addr + total_read;

		spib_clr_fifo();
		if (spib_wait_spi() != 0) {
			spib_clr_fifo();
			if (spib_wait_spi() != 0)
				break;                  /* SPI stuck, abort */
		}
		spib_clr_fifo();

		/* Build CMD+ADDR word with correct big-endian byte order for Flash */
		unsigned int op_addr = flash_read_op_addr(cur_addr);

		/* DCTRL: cmden=0, addren=0, tm=WR_RD, wcnt=3 (4 TX bytes), rcnt=chunk-1 */
		unsigned int spib_dctrl = spib_prepare_dctrl(
				0x0, 0x0, SPIB_TM_WR_RD, 3, 0, chunk - 1);

		/* Push CMD+ADDR into TX FIFO, configure DCTRL, trigger transfer */
		spib_set_data(op_addr);
		spib_set_dctrl(spib_dctrl);
		spib_set_cmd(0x0);              /* trigger */

		/* Read RX FIFO word by word */
		unsigned int rx_words = (chunk + 3) / 4;
		unsigned int byte_idx = 0;
		unsigned int i;

		for (i = 0; i < rx_words; i++) {
			unsigned int timeout = 50000;
			while (spib_get_rx_empty() && --timeout)
				;
			if (timeout == 0)
				break;                  /* RX timeout, abort this chunk */

			unsigned int rx_data = spib_get_data();

			/* Unpack 32-bit word into bytes (little-endian FIFO order) */
			if (total_read + byte_idx < size)
				buf[total_read + byte_idx++] = (rx_data >>  0) & 0xFF;
			if (total_read + byte_idx < size)
				buf[total_read + byte_idx++] = (rx_data >>  8) & 0xFF;
			if (total_read + byte_idx < size)
				buf[total_read + byte_idx++] = (rx_data >> 16) & 0xFF;
			if (total_read + byte_idx < size)
				buf[total_read + byte_idx++] = (rx_data >> 24) & 0xFF;
		}

		if (byte_idx == 0)
			break;                      /* no data received, abort */

		total_read += byte_idx;
	}

	/* Restore original SPI controller state */
	spib_clr_fifo();
	spib_ctrl = spib_ctrl_save;
	spib_set_ctrl(spib_ctrl);

	return total_read;
}

unsigned int QSPI_EXT_FLASH_write_page(unsigned int page_addr, const uint8_t *data, unsigned int size)
{
	unsigned int ret;
	static unsigned int debug_first_page = 1;

	if (size > 256) {
		info("ERROR: Page write size > 256 bytes\r\n");
		return 1;
	}

	unsigned int spib_ctrl_save = spib_ctrl;
	spib_ctrl &= ~SPIB_CTRL_RXDMAEN_MASK;
	spib_set_ctrl(spib_ctrl);
	// ??:????????????????????
	if ((debug_first_page && page_addr == 0) || page_addr == 0x025800) {
//		info("DEBUG: Writing page 0x%06X, data[0..3]=0x%02X 0x%02X 0x%02X 0x%02X\r\n",
//		     page_addr, data[0], data[1], data[2], data[3]);
	}

	// ????,????64??(??FIFO?????)
	#define CHUNK_SIZE 64
	unsigned int offset = 0;
	unsigned int chunk_size;

	while (offset < size) {
		chunk_size = size - offset;
		if (chunk_size > CHUNK_SIZE) {
			chunk_size = CHUNK_SIZE;
		}

		ret = write_small_chunk(page_addr + offset, data + offset, chunk_size);
		if (ret != 0) {
			spib_ctrl = spib_ctrl_save;
			spib_set_ctrl(spib_ctrl);
			return ret + 10;
		}

		offset += chunk_size;
	}

	spib_ctrl = spib_ctrl_save;
	spib_set_ctrl(spib_ctrl);

	if ((debug_first_page && page_addr == 0) || page_addr == 0x025800) {
		if (page_addr == 0) {
			debug_first_page = 0;
		}

		for (volatile int i = 0; i < 10000; i++);

		uint8_t verify_buf[16] = {0};
		debug_read_external_flash(page_addr, verify_buf, 16);
	}

	return 0;
}

unsigned int QSPI_EXT_FLASH_write(unsigned int flash_addr, const uint8_t *data, unsigned int size)
{
	unsigned int sector_addr, page_addr;
	unsigned int bytes_written = 0;
	unsigned int bytes_to_write;
	unsigned int ret;

//	EXT_FLASH_TRACE("EFW_START_FIXED\r\n");                         /* Fixed trace avoids formatted printf in the failing boot path. */

	sector_addr = flash_addr & ~0xFFF;  // 4KB??

	for (unsigned int addr = sector_addr; addr < (flash_addr + size); addr += 0x1000)
	{
		ret = QSPI_EXT_FLASH_erase_sector(addr);
		if (ret != 0)
		{
			return ret;
		}
	}

	while (bytes_written < size)
	{
		page_addr = flash_addr + bytes_written;

		// ?????????
		bytes_to_write = 256 - (page_addr & 0xFF);
		if (bytes_to_write > (size - bytes_written))
		{
			bytes_to_write = size - bytes_written;
		}

		ret = QSPI_EXT_FLASH_write_page(page_addr, data + bytes_written, bytes_to_write);
		if (ret != 0)
		{
			return 10 + ret;
		}

		bytes_written += bytes_to_write;

		// ?????????
		if (bytes_written == size)
		{
//			info("Write complete: %d bytes\r\n", bytes_written);
		}
	}

	{
		unsigned int spib_ctrl_save = spib_ctrl;
		spib_ctrl &= ~SPIB_CTRL_RXDMAEN_MASK;
		spib_set_ctrl(spib_ctrl);


		if (ext_flash_wait_busy() != 0) {
		}

		spib_clr_fifo();

		if (spib_wait_spi() != 0) {
			spib_clr_fifo();
			if (spib_wait_spi() != 0) {
			}
		}

		spib_set_intst(SPIB_INTSTS_ALLINT_MASK);

		spib_ctrl = spib_ctrl_save;
		spib_set_ctrl(spib_ctrl);
	}

	return 0;
}



/**
 * @brief  ???Flash????????Flash
 * @param  internal_flash_addr: ??Flash XIP??(?0x80030000)
 * @param  external_flash_addr: ??Flash????(?0x000000)
 * @param  size: ????
 * @retval 0=??, ?0=??
 */
unsigned int copy_internal_flash_to_external_flash(unsigned int external_flash_addr,  unsigned int size)
{
	// ??Flash XIP????????????
	const uint8_t *src_ptr = (const uint8_t *)array;

	// ????Flash
	unsigned int ret = QSPI_EXT_FLASH_write(external_flash_addr, src_ptr, size);

//	if (ret == 0)
//	{
//		info("Picture copy SUCCESS!\r\n");
//
//	} else
//	{
//		info("Picture copy FAILED! Error code: %d\r\n\r\n", ret);
//	}

	// ===== ??????QSPI DMA???? =====
//	info("Cleaning up QSPI...\r\n");

	unsigned int spib_ctrl_save = spib_ctrl;
	spib_ctrl &= ~SPIB_CTRL_RXDMAEN_MASK;
	spib_set_ctrl(spib_ctrl);

	dma_channel_disable(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO));
	dma_channel_clear_intsts(DMA0_QSPI, CPE_SPIBn_DMA_RX_CH(QSPI_FLASH_NO));

	spib_clr_fifo();
	spib_set_intst(SPIB_INTSTS_ALLINT_MASK);
	spib_set_inten(SPIB_INTSTS_ENDINT_MASK);

	if (spib_wait_spi() != 0) {
		spib_clr_fifo();
	}

	spib_ctrl = spib_ctrl_save | SPIB_CTRL_RXDMAEN_MASK;
	spib_set_ctrl(spib_ctrl);

	spib_clr_fifo();
	spib_dma_rddone_flag = 0x0;

//	info("QSPI cleanup complete\r\n");

	return ret;
}
#endif

#endif
/*********************************************END OF FILE**********************/
