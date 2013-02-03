#include <common.h>
#include <asm/arch/mt65xx.h>
#include <asm/arch/mt65xx_typedefs.h>
/* 2010-07-03 Should be enable after real-IC */
//#include <asm/arch/mt6577_pdn_sw.h>
#include <asm/errno.h>
#include "cust_nand.h"
#include <asm/io.h>
#include <asm/arch/mt6577_nand.h>
//#include <asm/arch/mtk_nand_device.h>
#include "nand_device_list.h"
#include "mt65xx_partition.h"
#include <asm/arch/bmt.h>
#include "nand_customer.h"
#include "partition_define.h"

#ifndef PART_SIZE_BMTPOOL
#define BMT_POOL_SIZE (80)
#else
#define BMT_POOL_SIZE (PART_SIZE_BMTPOOL)
#endif

#define PMT_POOL_SIZE (2)
/*Hong-Rong: comment for testing*/
//#if (CONFIG_COMMANDS & CFG_CMD_NAND)
#if defined(CONFIG_CMD_NAND)

#ifdef CFG_NAND_LEGACY
#include <linux/mtd/nand_legacy.h>
#else
#include <nand.h>
#include <linux/mtd/nand.h>
#endif

#define NAND_SECTOR_SIZE            (512)
#define NAND_SPARE_PER_SECTOR       (16)
#define CONFIG_MTD_NAND_MTK         1

#define CONFIG_SYS_NAND_QUIET_TEST	1
#define CONFIG_SYS_MAX_NAND_DEVICE  1
#define CONFIG_MTD_DEVICE           1

#define IO_WIDTH_8                  8
#define IO_WIDTH_16                 16

/*******************************************************************************
 * Macro definition 
 *******************************************************************************/

#define NFI_SET_REG32(reg, value)   (DRV_WriteReg32(reg, DRV_Reg32(reg) | (value)))
#define NFI_SET_REG16(reg, value)   (DRV_WriteReg16(reg, DRV_Reg16(reg) | (value)))
#define NFI_CLN_REG32(reg, value)   (DRV_WriteReg32(reg, DRV_Reg32(reg) & (~(value))))
#define NFI_CLN_REG16(reg, value)   (DRV_WriteReg16(reg, DRV_Reg16(reg) & (~(value))))

#define NFI_WAIT_STATE_DONE(state) do{;}while (__raw_readl(NFI_STA_REG32) & state)
#define NFI_WAIT_TO_READY()  do{;}while (!(__raw_readl(NFI_STA_REG32) & STA_BUSY2READY))

#define FIFO_PIO_READY(x)  (0x1 & x)
#define WAIT_NFI_PIO_READY(timeout) \
do {\
   while( (!FIFO_PIO_READY(DRV_Reg(NFI_PIO_DIRDY_REG16))) && (--timeout) );\
   if(timeout == 0)\
   {\
        MSG(ERR, "Error: FIFO_PIO_READY timeout at line=%d, file =%s\n", __LINE__, __FILE__);\
   }\
} while(0);

#define TIMEOUT_1   0x1fff
#define TIMEOUT_2   0x8ff
#define TIMEOUT_3   0xffff
#define TIMEOUT_4   5000   //PIO

/*
static struct nand_ecclayout mt6577_nand_oob = {
    //.useecc = MTD_NANDECC_AUTOPLACE,
    .eccbytes = 32,
	.eccpos = {32, 33, 34, 35, 36, 37, 38, 39,
	           40, 41, 42, 43, 44, 45, 46, 47,
	           48, 49, 50, 51, 52, 53, 54, 55,
	           56, 57, 58, 59, 60, 61, 62, 63},
    .oobfree = {{1, 7}, {9, 7}, {17, 7}, {25, 7}, {0, 0}}
};
*/

static struct nand_ecclayout nand_oob_16 = {
	.eccbytes = 8,
	.eccpos = {8, 9, 10, 11, 12, 13, 14, 15},
	.oobfree = {{1,6}, {0, 0}}
};

struct nand_ecclayout nand_oob_64 = {
	.eccbytes = 32,
	.eccpos = {32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55,
        56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = {{1, 7}, {9, 7}, {17, 7}, {25, 6}, {0, 0}}
};

struct nand_ecclayout nand_oob_128 = {
	.eccbytes = 64,
	.eccpos = {
        64, 65, 66, 67, 68, 69, 70, 71, 
        72, 73, 74, 75, 76, 77, 78, 79,
        80, 81, 82, 83, 84, 85, 86, 86,
        88, 89, 90, 91, 92, 93, 94, 95,
        96, 97, 98, 99, 100, 101, 102, 103,
        104, 105, 106, 107, 108, 109, 110, 111,
        112, 113, 114, 115, 116, 117, 118, 119,
        120, 121, 122, 123, 124, 125, 126, 127},
	.oobfree = {{1, 7}, {9, 7}, {17, 7}, {25, 7}, {33, 7}, {41, 7}, {49, 7}, {57, 6}}
};

static bmt_struct *g_bmt = NULL;

#define NFI_ISSUE_COMMAND(cmd, col_addr, row_addr, col_num, row_num) \
   do { \
      DRV_WriteReg(NFI_CMD_REG16,cmd);\
      while (DRV_Reg32(NFI_STA_REG32) & STA_CMD_STATE);\
      DRV_WriteReg32(NFI_COLADDR_REG32, col_addr);\
      DRV_WriteReg32(NFI_ROWADDR_REG32, row_addr);\
      DRV_WriteReg(NFI_ADDRNOB_REG16, col_num | (row_num<<ADDR_ROW_NOB_SHIFT));\
      while (DRV_Reg32(NFI_STA_REG32) & STA_ADDR_STATE);\
   }while(0);


#ifdef CFG_NAND_LEGACY
void nand_init(void)
{

}
#else

flashdev_info devinfo;

#define CHIPVER_ECO_1 (0x8a00)
#define CHIPVER_ECO_2 (0x8a01)

struct mt6577_CMD g_kCMD;
u32 total_size;
static u32 g_u4ChipVer;
static u32 g_i4ErrNum;
static bool g_bInitDone;
bool get_device_info(u16 id, u32 ext_id, flashdev_info *pdevinfo)
{
	u32 index;
	for(index=0;gen_FlashTable[index].id!=0;index++)
	{
	    if(id==gen_FlashTable[index].id && ext_id == gen_FlashTable[index].ext_id)
	    {
	    	pdevinfo->id = gen_FlashTable[index].id;
	    	pdevinfo->ext_id = gen_FlashTable[index].ext_id;
			pdevinfo->blocksize = gen_FlashTable[index].blocksize;
			pdevinfo->addr_cycle = gen_FlashTable[index].addr_cycle;
			pdevinfo->iowidth = gen_FlashTable[index].iowidth;
			pdevinfo->timmingsetting = gen_FlashTable[index].timmingsetting;
			pdevinfo->advancedmode = gen_FlashTable[index].advancedmode;
			pdevinfo->pagesize = gen_FlashTable[index].pagesize;
	        pdevinfo->totalsize = gen_FlashTable[index].totalsize;
			memcpy(pdevinfo->devciename,gen_FlashTable[index].devciename,sizeof(pdevinfo->devciename));
            MSG(INIT, "MTK Table, ID: %x, EXT_ID: %x\n", id, ext_id);
	
	        goto find;		
		}
	}
	
    for (index = 0; cust_FlashTable[index].id != 0; index++)
    {
        if (id == cust_FlashTable[index].id)
        {
            pdevinfo->id = cust_FlashTable[index].id;
            pdevinfo->blocksize = cust_FlashTable[index].blocksize;
            pdevinfo->addr_cycle = cust_FlashTable[index].addr_cycle;
            pdevinfo->iowidth = cust_FlashTable[index].iowidth;
            pdevinfo->timmingsetting = cust_FlashTable[index].timmingsetting;
            pdevinfo->advancedmode = cust_FlashTable[index].advancedmode;
            pdevinfo->pagesize = cust_FlashTable[index].pagesize;
            pdevinfo->totalsize = cust_FlashTable[index].totalsize;
            memcpy(pdevinfo->devciename, cust_FlashTable[index].devciename, sizeof(pdevinfo->devciename));
            MSG(INIT, "Cust Table, ID: %x\n", id);

            goto find;
        }
    }

find:
	if(0==pdevinfo->id)
	{
        MSG(INIT, "ID: %x not found\n", id);
	    return false;
	}
	else
	{
		return true;
	}
}
//-------------------------------------------------------------------------------
static void ECC_Config(void)
{
	u32 u4ENCODESize;
	u32 u4DECODESize;

    DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);
    do{;}while (!DRV_Reg16(ECC_DECIDLE_REG16));

    DRV_WriteReg16(ECC_ENCCON_REG16, ENC_DE);
    do{;}while (!DRV_Reg32(ECC_ENCIDLE_REG32));

	/* setup FDM register base */
	DRV_WriteReg32(ECC_FDMADDR_REG32, NFI_FDM0L_REG32);

	u4ENCODESize = (NAND_SECTOR_SIZE + 8) << 3;
	u4DECODESize = ((NAND_SECTOR_SIZE + 8) << 3) + 4 * 13;

	/* configure ECC decoder && encoder*/
	
	DRV_WriteReg32(ECC_DECCNFG_REG32,
		ECC_CNFG_ECC4|DEC_CNFG_NFI|DEC_CNFG_EMPTY_EN|
		(u4DECODESize<<DEC_CNFG_CODE_SHIFT));
			

	DRV_WriteReg32(ECC_ENCCNFG_REG32, 
		ECC_CNFG_ECC4|ENC_CNFG_NFI|
		(u4ENCODESize << ENC_CNFG_MSG_SHIFT));

#if USE_AHB_MODE
	NFI_SET_REG32(ECC_DECCNFG_REG32, DEC_CNFG_CORRECT);
#else
	NFI_SET_REG32(ECC_DECCNFG_REG32, DEC_CNFG_EL);
#endif
}
//-------------------------------------------------------------------------------
static void ECC_Decode_Start(void)
{
   	/* wait for device returning idle */
	while(!(DRV_Reg16(ECC_DECIDLE_REG16) & DEC_IDLE));
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_EN);
}
//-------------------------------------------------------------------------------
static void ECC_Decode_End(void)
{
   /* wait for device returning idle */
	while(!(DRV_Reg16(ECC_DECIDLE_REG16) & DEC_IDLE));
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);
}
//-------------------------------------------------------------------------------
static void ECC_Encode_Start(void)
{
   /* wait for device returning idle */
	while(!(DRV_Reg32(ECC_ENCIDLE_REG32) & ENC_IDLE));
	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_EN);
}
//-------------------------------------------------------------------------------
static void ECC_Encode_End(void)
{
   /* wait for device returning idle */
	while(!(DRV_Reg32(ECC_ENCIDLE_REG32) & ENC_IDLE));
	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_DE);
}
//-------------------------------------------------------------------------------
static bool mt6577_nand_check_bch_error(u8* pDataBuf, u32 u4SecIndex, u32 u4PageAddr)
{
	bool bRet = true;
	u16 u2SectorDoneMask = 1 << u4SecIndex;
	u32 u4ErrorNumDebug, i, u4ErrNum;
	u32 timeout = 0xFFFF;
	
#if !USE_AHB_MODE
	u32 au4ErrBitLoc[6];
	u32 u4ErrByteLoc, u4BitOffset;
	u32 u4ErrBitLoc1th, u4ErrBitLoc2nd;
#endif

	//4 // Wait for Decode Done
	while (0 == (u2SectorDoneMask & DRV_Reg16(ECC_DECDONE_REG16))){		
		timeout--;
		if (0 == timeout){
			return false;
		}
	}

#if USE_AHB_MODE
	u4ErrorNumDebug = DRV_Reg32(ECC_DECENUM_REG32);
	if (0 != (u4ErrorNumDebug & 0xFFFF)){
		for (i = 0; i <= u4SecIndex; ++i){
			u4ErrNum = DRV_Reg32(ECC_DECENUM_REG32) >> (i << 2);
			u4ErrNum &= 0xF;
			if (0xF == u4ErrNum){
				MSG(ERR, "AHB_MODE : UnCorrectable at PageAddr=%d, Sector=%d\n", u4PageAddr, i);
				bRet = false; //4 //Uncorrectable
			} else {
				MSG(ERR, "AHB_MODE : Correct %d at PageAddr=%d, Sector=%d\n", u4ErrNum, u4PageAddr, i);
			}
		}
	}
#else
	memset(au4ErrBitLoc, 0x0, sizeof(au4ErrBitLoc));
	u4ErrorNumDebug = DRV_Reg32(ECC_DECENUM_REG32);
	u4ErrNum = DRV_Reg32(ECC_DECENUM_REG32) >> (u4SecIndex << 2);
	u4ErrNum &= 0xF;
	if (u4ErrNum){
		if (0xF == u4ErrNum){
			MSG(ERR, "UnCorrectable at PageAddr=%d\n", u4PageAddr);
			bRet = false;
		} else {
			for (i = 0; i < ((u4ErrNum+1)>>1); ++i){
				au4ErrBitLoc[i] = DRV_Reg32(ECC_DECEL0_REG32 + i);
				u4ErrBitLoc1th = au4ErrBitLoc[i] & 0x1FFF;
				if (u4ErrBitLoc1th < 0x1000){
					u4ErrByteLoc = u4ErrBitLoc1th/8;
					u4BitOffset = u4ErrBitLoc1th%8;
					pDataBuf[u4ErrByteLoc] = pDataBuf[u4ErrByteLoc]^(1<<u4BitOffset);
				} else {
					MSG(ERR, "UnCorrectable ErrLoc=%d\n", au4ErrBitLoc[i]);
				}

				u4ErrBitLoc2nd = (au4ErrBitLoc[i] >> 16) & 0x1FFF;
				if (0 != u4ErrBitLoc2nd){
					if (u4ErrBitLoc2nd < 0x1000){
						u4ErrByteLoc = u4ErrBitLoc2nd/8;
						u4BitOffset = u4ErrBitLoc2nd%8;
						pDataBuf[u4ErrByteLoc] = pDataBuf[u4ErrByteLoc]^(1<<u4BitOffset);
					} else {
						MSG(ERR, "UnCorrectable High ErrLoc=%d\n", au4ErrBitLoc[i]);
					}
				}
			}
			bRet = true;
		}

		if (0 == (DRV_Reg16(ECC_DECFER_REG16) & (1 << u4SecIndex)))
		{
			bRet = false;
		}
	}
#endif

	return bRet;
}
//-------------------------------------------------------------------------------
static bool mt6577_nand_RFIFOValidSize(u16 u2Size)
{
	u32 timeout = 0xFFFF;
	while (FIFO_RD_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) < u2Size){
		timeout--;
		if (0 == timeout){
			return false;
		}
	}
	if(u2Size==0)
	{
		while (FIFO_RD_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)))
		{
			timeout--;
			if (0 == timeout){
				printf("mt6577_nand_RFIFOValidSize failed: 0x%x\n",  u2Size);
				return false;
			}
		}
	}

	return true;
}
//-------------------------------------------------------------------------------
static bool mt6577_nand_WFIFOValidSize(u16 u2Size)
{
	u32 timeout = 0xFFFF;
	while (FIFO_WR_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) > u2Size){
		timeout--;
		if (0 == timeout){
			return false;
		}
	}
	if(u2Size==0)
	{
		while (FIFO_WR_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)))
		{
			timeout--;
			if (0 == timeout){
				printf("mt6577_nand_RFIFOValidSize failed: 0x%x\n",  u2Size);
				return false;
			}
		}
	}

	return true;
}
//-------------------------------------------------------------------------------
static bool mt6577_nand_status_ready(u32 u4Status)
{
	u32 timeout = 0xFFFF;
	while ((DRV_Reg32(NFI_STA_REG32) & u4Status) != 0){
		timeout--;
		if (0 == timeout){
			return false;
		}
	}
	return true;
}
//-------------------------------------------------------------------------------
static bool mt6577_nand_reset(void)
{
    int timeout = 0xFFFF;
    if (DRV_Reg16(NFI_MASTERSTA_REG16)) // master is busy
    {
        DRV_WriteReg16(NFI_CON_REG16, CON_FIFO_FLUSH | CON_NFI_RST);
        while (DRV_Reg16(NFI_MASTERSTA_REG16))
        {
            timeout--;
            if (!timeout)
            {
                MSG(FUC, "Wait for NFI_MASTERSTA timeout\n");
            }
        }
    }
	/* issue reset operation */
	DRV_WriteReg16(NFI_CON_REG16, CON_FIFO_FLUSH | CON_NFI_RST);

	return mt6577_nand_status_ready(STA_NFI_FSM_MASK|STA_NAND_BUSY) &&
		   mt6577_nand_RFIFOValidSize(0) &&
		   mt6577_nand_WFIFOValidSize(0);
}
//-------------------------------------------------------------------------------
static void mt6577_nand_set_mode(u16 u2OpMode)
{
	u16 u2Mode = DRV_Reg16(NFI_CNFG_REG16);
	u2Mode &= ~CNFG_OP_MODE_MASK;
	u2Mode |= u2OpMode;
	DRV_WriteReg16(NFI_CNFG_REG16, u2Mode);
}
//-------------------------------------------------------------------------------
static void mt6577_nand_set_autoformat(bool bEnable)
{
	if (bEnable){
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
	}else{
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
	}
}
//-------------------------------------------------------------------------------
static void mt6577_nand_configure_fdm(u16 u2FDMSize)
{
	NFI_CLN_REG16(NFI_PAGEFMT_REG16, PAGEFMT_FDM_MASK | PAGEFMT_FDM_ECC_MASK);
	NFI_SET_REG16(NFI_PAGEFMT_REG16, u2FDMSize << PAGEFMT_FDM_SHIFT);
	NFI_SET_REG16(NFI_PAGEFMT_REG16, u2FDMSize << PAGEFMT_FDM_ECC_SHIFT);
}
//-------------------------------------------------------------------------------
static void mt6577_nand_configure_lock(void)
{
	u32 u4WriteColNOB = 2;
	u32 u4WriteRowNOB = 3;
	u32 u4EraseColNOB = 0;
	u32 u4EraseRowNOB = 3;
	DRV_WriteReg16(NFI_LOCKANOB_REG16, 
		(u4WriteColNOB << PROG_CADD_NOB_SHIFT)  |
		(u4WriteRowNOB << PROG_RADD_NOB_SHIFT)  |
		(u4EraseColNOB << ERASE_CADD_NOB_SHIFT) |
		(u4EraseRowNOB << ERASE_RADD_NOB_SHIFT));		

	// Workaround method for ECO1 mt6577	
	if (CHIPVER_ECO_1 == g_u4ChipVer){
		int i;
		for (i = 0; i < 16; ++i){
			DRV_WriteReg32(NFI_LOCK00ADD_REG32 + (i << 1), 0xFFFFFFFF);
			DRV_WriteReg32(NFI_LOCK00FMT_REG32 + (i << 1), 0xFFFFFFFF);
		}
		//DRV_WriteReg16(NFI_LOCKANOB_REG16, 0x0);
		DRV_WriteReg32(NFI_LOCKCON_REG32, 0xFFFFFFFF);
		DRV_WriteReg16(NFI_LOCK_REG16, NFI_LOCK_ON);
	}
}
//-------------------------------------------------------------------------------
static bool mt6577_nand_set_command(u16 command)
{
	/* Write command to device */
	DRV_WriteReg16(NFI_CMD_REG16, command);	
	return mt6577_nand_status_ready(STA_CMD_STATE);
}
//-------------------------------------------------------------------------------
static bool mt6577_nand_set_address(u32 u4ColAddr, u32 u4RowAddr, u16 u2ColNOB, u16 u2RowNOB)
{
	/* fill cycle addr */
	DRV_WriteReg32(NFI_COLADDR_REG32, u4ColAddr);
	DRV_WriteReg32(NFI_ROWADDR_REG32, u4RowAddr);
	DRV_WriteReg16(NFI_ADDRNOB_REG16, u2ColNOB|(u2RowNOB << ADDR_ROW_NOB_SHIFT));
	return mt6577_nand_status_ready(STA_ADDR_STATE);
}
//-------------------------------------------------------------------------------
static bool mt6577_nand_check_RW_count(u16 u2WriteSize)
{
	u32 timeout = 0xFFFF;
	u16 u2SecNum = u2WriteSize >> 9;
	while (ADDRCNTR_CNTR(DRV_Reg16(NFI_ADDRCNTR_REG16)) < u2SecNum){
		timeout--;
		if (0 == timeout){
			return false;
		}
	}
	return true;
}
//-------------------------------------------------------------------------------
static bool mt6577_nand_ready_for_read(
	struct nand_chip *nand, u32 u4RowAddr, u32 u4ColAddr, bool bFull, u8 *buf)
{
	/* Reset NFI HW internal state machine and flush NFI in/out FIFO */	
	bool bRet = false;
	u16 sec_num = 1 << (nand->page_shift - 9);
    u32 col_addr = u4ColAddr;
    if (nand->options & NAND_BUSWIDTH_16)
        col_addr >>= 1;
	u32 colnob=2, rownob = devinfo.addr_cycle-2;

	if (!mt6577_nand_reset()){
		goto cleanup;
	}

    /* Enable HW ECC */
    NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);

	mt6577_nand_set_mode(CNFG_OP_READ);
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
	DRV_WriteReg16(NFI_CON_REG16, sec_num << CON_NFI_SEC_SHIFT);

    if (bFull)
    {
#if USE_AHB_MODE
	    NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AHB);
        DRV_WriteReg32(NFI_STRADDR_REG32, buf);
#else
	    NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
#endif
	    NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
    }
    else
    {
        NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
        NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
    }

	mt6577_nand_set_autoformat(bFull);
    if (bFull)
	    ECC_Decode_Start();

	if (!mt6577_nand_set_command(NAND_CMD_READ0)){
		goto cleanup;
	}

	//1 FIXED ME: For Any Kind of AddrCycle
	if (!mt6577_nand_set_address(col_addr, u4RowAddr, colnob, rownob)){
		goto cleanup;
	}

	if (!mt6577_nand_set_command(NAND_CMD_READSTART)){
		goto cleanup;
	}

	if (!mt6577_nand_status_ready(STA_NAND_BUSY)){
		goto cleanup;
	}

	bRet = true;
	
cleanup:
	return bRet;
}
//-----------------------------------------------------------------------------
static bool mt6577_nand_ready_for_write(
	struct nand_chip *nand, u32 u4RowAddr, u8 *buf)
{
	bool bRet = false;
	u16 sec_num = 1 << (nand->page_shift - 9);
	u32 colnob=2, rownob=devinfo.addr_cycle-2;	
	/* Reset NFI HW internal state machine and flush NFI in/out FIFO */	
	if (!mt6577_nand_reset()){
		return false;
	}

	mt6577_nand_set_mode(CNFG_OP_PRGM);
	
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
	
	DRV_WriteReg16(NFI_CON_REG16, sec_num << CON_NFI_SEC_SHIFT);

#if USE_AHB_MODE
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AHB);
    DRV_WriteReg32(NFI_STRADDR_REG32, buf);
#else
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
#endif

	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	
	mt6577_nand_set_autoformat(true);

	ECC_Encode_Start();

	if (!mt6577_nand_set_command(NAND_CMD_SEQIN)){
		goto cleanup;
	}

	//1 FIXED ME: For Any Kind of AddrCycle
	if (!mt6577_nand_set_address(0, u4RowAddr, colnob, rownob)){
		goto cleanup;
	}

	if (!mt6577_nand_status_ready(STA_NAND_BUSY)){
		goto cleanup;
	}

	bRet = true;
cleanup:

	return bRet;
}
//-----------------------------------------------------------------------------
static bool mt6577_nand_dma_transfer_data(u8* pDataBuf, u32 u4Size)
{
	u32 timeout = 0xFFFF;
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	// DRV_WriteReg32(NFI_STRADDR_REG32, pDataBuf);
    
    DRV_Reg16 (NFI_INTR_REG16);
    DRV_WriteReg16 (NFI_INTR_EN_REG16, INTR_AHB_DONE_EN);
    NFI_SET_REG16(NFI_CON_REG16, CON_NFI_BRD);
   
    while ( !(DRV_Reg16(NFI_INTR_REG16) & INTR_AHB_DONE) )
    {
        timeout--;
        if (0 == timeout)
        {
            return false;     //4  // AHB Mode Time Out!
        }
    }
    
    timeout = 0xFFFF;
    while ( (u4Size >> 9) > ((DRV_Reg16(NFI_BYTELEN_REG16) & 0xf000) >> 12) )
    {
		timeout--;
		if (0 == timeout){
			return false; //4  // AHB Mode Time Out!
		}
	}

    return true;
}

static bool mt6577_nand_mcu_transfer_data(u8* pDataBuf, u32 length)
{
	u32 timeout = 0xFFFF;
	u32 i;
	u32* pBuf32;

    if (length % 4)
        NFI_SET_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);

	// DRV_WriteReg32(NFI_STRADDR_REG32, 0);
	NFI_SET_REG16(NFI_CON_REG16, CON_NFI_BRD);
	pBuf32 = (u32*)pDataBuf;

    if (length % 4)
    {
    	for (i = 0; (i < length) && (timeout > 0);)
        {
    		if (FIFO_RD_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) >= 4){
    			*pDataBuf++ = DRV_Reg32(NFI_DATAR_REG32);
    			i++;
    		} else {
    			timeout--;
    		}
    		if (0 == timeout){
    			return false; //4 // MCU  Mode Time Out!
    		}
    	}
    }
    else
    {
        for (i = 0; (i < (length >> 2)) && (timeout > 0);)
        {
    		if (FIFO_RD_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) >= 4){
    			*pBuf32++ = DRV_Reg32(NFI_DATAR_REG32);
    			i++;
    		} else {
    			timeout--;
    		}
    		if (0 == timeout){
    			return false; //4 // MCU  Mode Time Out!
    		}
    	}
    }
	return true;
}

static bool mt6577_nand_read_page_data(u8 *buf, u32 length)
{
#if USE_AHB_MODE
    return mt6577_nand_dma_transfer_data(buf, length);
#else
    return mt6577_nand_mcu_transfer_data(buf, length);
#endif
}

//-----------------------------------------------------------------------------
static bool mt6577_nand_write_page_data(u8* pDataBuf, u32 u4Size)
{
	u32 timeout = 0xFFFF;
#if USE_AHB_MODE	
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	// DRV_WriteReg32(NFI_STRADDR_REG32, pDataBuf);
	NFI_SET_REG16(NFI_CON_REG16, CON_NFI_BWR);
	// while (u4Size > DRV_Reg16(NFI_BYTELEN_REG16)){
    while ( (u4Size >> 9) > ((DRV_Reg16(NFI_BYTELEN_REG16) & 0xf000) >> 12) ) {
		timeout--;
		if (0 == timeout){
			return false; //4  // AHB Mode Time Out!
		}
	}
#else
	u32 i;
	u32* pBuf32;
	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	// DRV_WriteReg32(NFI_STRADDR_REG32, 0);
	NFI_SET_REG16(NFI_CON_REG16, CON_NFI_BWR);
	pBuf32 = (u32*)pDataBuf;
	for (i = 0; (i < (u4Size >> 2))&&(timeout > 0);){
		if (FIFO_WR_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) <= 12){
			DRV_WriteReg32(NFI_DATAW_REG32, *pBuf32++);
			i++;
		} else {
			timeout--;
		}
		if (0 == timeout){
			return false; //4 // MCU Mode Time Out!		
		}
	}
#endif

	return true;
}
//-----------------------------------------------------------------------------
static void mt6577_nand_read_fdm_data(u8* pDataBuf, u32 u4SecNum)
{
	u32 i;
	u32* pBuf32 = (u32*)pDataBuf;
	for (i = 0; i < u4SecNum; ++i){
		*pBuf32++ = DRV_Reg32(NFI_FDM0L_REG32 + (i<<1));
		*pBuf32++ = DRV_Reg32(NFI_FDM0M_REG32 + (i<<1));
	}
}
//-----------------------------------------------------------------------------
static void mt6577_nand_write_fdm_data(u8* pDataBuf, u32 u4SecNum)
{
	u32 i;
	u32* pBuf32 = (u32*)pDataBuf;
	for (i = 0; i < u4SecNum; ++i)
	{
		DRV_WriteReg32(NFI_FDM0L_REG32 + (i<<1), *pBuf32++);
		DRV_WriteReg32(NFI_FDM0M_REG32 + (i<<1), *pBuf32++);
	}
}
//-----------------------------------------------------------------------------
static void mt6577_nand_stop_read(void)
{
	NFI_CLN_REG16(NFI_CON_REG16, CON_NFI_BRD);
	ECC_Decode_End();
}
//-----------------------------------------------------------------------------
static void mt6577_nand_stop_write(void)
{
	NFI_CLN_REG16(NFI_CON_REG16, CON_NFI_BWR);
	ECC_Encode_End();
}
//-----------------------------------------------------------------------------
void dump_nfi(void)
{
    printf( "NFI_ACCCON: 0x%x\n",  DRV_Reg32(NFI_ACCCON_REG32));
    printf( "NFI_PAGEFMT: 0x%x\n", DRV_Reg16(NFI_PAGEFMT_REG16));
    printf( "NFI_CNFG: 0x%x\n", DRV_Reg16(NFI_CNFG_REG16));
    printf( "NFI_CON: 0x%x\n", DRV_Reg16(NFI_CON_REG16));
    printf( "NFI_STRDATA: 0x%x\n", DRV_Reg16(NFI_STRDATA_REG16));
    printf( "NFI_ADDRCNTR: 0x%x\n", DRV_Reg16(NFI_ADDRCNTR_REG16));
    printf( "NFI_FIFOSTA: 0x%x\n", DRV_Reg16(NFI_FIFOSTA_REG16));
    printf( "NFI_ADDRNOB: 0x%x\n", DRV_Reg16(NFI_ADDRNOB_REG16));
    printf( "NFI_FDM_0L: 0x%x\n", DRV_Reg32(NFI_FDM0L_REG32));
    printf( "NFI_STA: 0x%x\n", DRV_Reg32(NFI_STA_REG32));
    printf( "NFI_FDM_0M: 0x%x\n", DRV_Reg32(NFI_FDM0M_REG32));
    printf( "NFI_IOCON: 0x%x\n", DRV_Reg16(NFI_IOCON_REG16));
    printf( "NFI_BYTELEN: 0x%x\n", DRV_Reg16(NFI_BYTELEN_REG16));
    printf( "NFI_COLADDR: 0x%x\n", DRV_Reg32(NFI_COLADDR_REG32));
    printf( "NFI_ROWADDR: 0x%x\n", DRV_Reg32(NFI_ROWADDR_REG32));
    printf( "ECC_ENCCNFG: 0x%x\n", DRV_Reg32(ECC_ENCCNFG_REG32));
    printf( "ECC_ENCCON: 0x%x\n", DRV_Reg16(ECC_ENCCON_REG16));
    printf( "ECC_DECCNFG: 0x%x\n", DRV_Reg32(ECC_DECCNFG_REG32));
    printf( "ECC_DECCON: 0x%x\n", DRV_Reg16(ECC_DECCON_REG16));
    // printf( "NFI clock register: 0x%x: %s\n", DRV_Reg32((volatile u32 *)0x00000000),
    //         (DRV_Reg32((volatile u32 *)0xF0039300) & (1 << 17)) ? "miss" : "OK");
}

static bool mt6577_nand_check_dececc_done(u32 u4SecNum)
{
    u32 timeout, dec_mask;
    timeout = 0xffff;
    dec_mask = (1<<u4SecNum)-1;
    while((dec_mask != DRV_Reg(ECC_DECDONE_REG16)) && timeout>0)
        timeout--;
    if(timeout == 0){
        dump_nfi();
        MSG(ERR, "ECC_DECDONE: timeout\n");
        return false;
    }
    return true;
}
//-------------------------------------------------------------------------------


int mt6577_nand_exec_read_page_hw(
	struct nand_chip *nand, u32 u4RowAddr, u32 u4PageSize, u8* pPageBuf, u8* pFDMBuf)
{
	bool bRet = true;
	u32 u4SecNum = u4PageSize >> 9;
	
	if (mt6577_nand_ready_for_read(nand, u4RowAddr, 0, true, pPageBuf)) {
		if (!mt6577_nand_read_page_data(pPageBuf, u4PageSize)) {
			bRet = false;
		}
		if (!mt6577_nand_status_ready(STA_NAND_BUSY)) {
			bRet = false;
		}

        if(!mt6577_nand_check_dececc_done(u4SecNum)){
            bRet = false;
        }       
        
		mt6577_nand_read_fdm_data(pFDMBuf, u4SecNum);
		
		if (!mt6577_nand_check_bch_error(pPageBuf, u4SecNum - 1, u4RowAddr)) {
			g_i4ErrNum++;
		}
		mt6577_nand_stop_read();
	}

	return bRet;
}

static bool mt6577_nand_exec_read_page(
	struct nand_chip *nand, u32 u4RowAddr, u32 u4PageSize, u8* pPageBuf, u8* pFDMBuf)
{
    u32 page_per_block = 1 << (nand->phys_erase_shift - nand->page_shift);
    int block = u4RowAddr / page_per_block;
    int page_in_block = u4RowAddr % page_per_block;
    int mapped_block;
    int i, start, len, offset;
    struct nand_oobfree *free;
    u8 oob[0x80];

    mapped_block = get_mapping_block_index(block);

    if (!mt6577_nand_exec_read_page_hw(nand, (mapped_block * page_per_block + page_in_block), u4PageSize, pPageBuf, oob))
        return false;

    offset = 0;
    free = nand->ecc.layout->oobfree;
    for (i = 0; free[i].length; i++)
    {
        start = free[i].offset;
        len = free[i].length;
        memcpy(pFDMBuf + offset, oob + start, len);
        offset += len;
    }

    return false;
}

//-------------------------------------------------------------------------------
static bool mt6577_nand_exec_write_page(
	struct nand_chip *nand, u32 u4RowAddr, u32 u4PageSize, u8* pPageBuf, u8* pFDMBuf)
{
	bool bRet = true;
	u32 u4SecNum = u4PageSize >> 9;
	if (mt6577_nand_ready_for_write(nand, u4RowAddr, pPageBuf)) {
		mt6577_nand_write_fdm_data(pFDMBuf, u4SecNum);
		if (!mt6577_nand_write_page_data(pPageBuf, u4PageSize)) {
			bRet = false;
		}
		if (!mt6577_nand_check_RW_count(u4PageSize)) {
			bRet = false;
		}
		mt6577_nand_stop_write();
		mt6577_nand_set_command(NAND_CMD_PAGEPROG);
		while(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY);		
	}
	return bRet;
}

#define OOB_AVAIL_PER_SECTOR 8
static bool mt6577_nand_read_oob_raw(struct nand_chip *chip, u32 page_addr, u32 length, u8 *buf)
{
    u32 sector = 0;
    u32 col_addr = 0;

    if (length > 32 || length % OOB_AVAIL_PER_SECTOR || !buf)
    {
        printf("[%s] invalid parameter, length: %d, buf: %p\n",
                __FUNCTION__, length, buf);
        return false;
    }

    while (length > 0)
    {
        col_addr = NAND_SECTOR_SIZE + sector * (NAND_SECTOR_SIZE + NAND_SPARE_PER_SECTOR);
        if (!mt6577_nand_ready_for_read(chip, page_addr, col_addr, false, NULL))
            return false;
        if (!mt6577_nand_mcu_transfer_data(buf, length))
            return false;
        NFI_CLN_REG16(NFI_CON_REG16, CON_NFI_BRD);
        sector++;
        length -= 8;
    }

    return true;
}


bool nand_block_bad_hw(struct nand_chip *nand, u32 offset)
{
    u32 page_per_block = 1 << (nand->phys_erase_shift - nand->page_shift);
    // u32 u4PageSize  = 1 << nand->page_shift;
    u32 page_addr = offset >> nand->page_shift;
    
    u8 oob_buf[OOB_AVAIL_PER_SECTOR];
    memset(oob_buf, 0, OOB_AVAIL_PER_SECTOR);

    page_addr &= ~(page_per_block - 1);

    if (!mt6577_nand_read_oob_raw(nand, page_addr, OOB_AVAIL_PER_SECTOR, oob_buf))
    {
        printf("mt6577_nand_read_oob_raw return fail\n");
    }

    if (oob_buf[0] != 0xff)
    {
        printf("Bad block detect at block 0x%x, oob_buf[0] is %x\n", page_addr/page_per_block, oob_buf[0]);
        return true;
    }

    return false;
#if 0
    iaCheckPageID[0] = 0;
	iaCheckPageID[1] = 1;
	iaCheckPageID[2] = page_num - 3;
	iaCheckPageID[3] = page_num - 1;
    
	for (i = 0; i < 4; i++) 
    {
        mt6577_nand_exec_read_page(nand, (page_addr+iaCheckPageID[i]), u4PageSize, nand->buffers->databuf, g_kCMD.au1OOB);

        pspare = &g_kCMD.au1OOB;
        
		if (pspare[0] != 0xFF || pspare[8] != 0xFF || pspare[16] != 0xFF || pspare[24] != 0xFF) 
        {
			bRet = TRUE;
			break;
		}
	}
    
    if(bRet)
    {
        printf("BAD BLOCK detected at 0x%x\n", page_addr);
        return bRet;
    }
    else
    {
        return bRet;        
    }
#endif
}

static bool nand_block_bad(struct nand_chip *nand, u32 page_addr)
{
    u32 page_per_block = 1 << (nand->phys_erase_shift - nand->page_shift);
    int block = page_addr / page_per_block;
    int mapped_block = get_mapping_block_index(block);

    return nand_block_bad_hw(nand, mapped_block << nand->phys_erase_shift);
}

#if 0
static int mt6577_nand_part_write(part_dev_t *dev, uchar *src, ulong dst, int size)
{
	struct nand_chip *nand = (struct nand_chip *)dev->blkdev;
	uint8_t	res;        // *bbt = nand->bbt;
	u32 u4PageSize  = 1 << nand->page_shift;
	u32 u4PageNumPerBlock = 1 << (nand->phys_erase_shift - nand->page_shift);
	u32 u4BlkEnd  = (nand->chipsize >> nand->phys_erase_shift) << 1;
	u32 u4BlkAddr = (dst >> nand->phys_erase_shift) << 1;
	u32 u4ColAddr = dst & (u4PageSize - 1);
	u32 u4RowAddr = dst >> nand->page_shift;
	u32 u4RowEnd;
	u32 u4WriteLen = 0;
	int i4Len;

    int k=0;
/*
	MSG(OPS, "|--------------------------|\n");
	MSG(OPS, "start addr = 0x%08x\n", src);
	MSG(OPS, "dest addr = 0x%08x\n", dst);
	MSG(OPS, "size = %d\n", size);
	MSG(OPS, "u4BlkAddr = 0x%08x\n", (u4BlkAddr/2));
	MSG(OPS, "u4ColAddr = 0x%08x\n", u4ColAddr);
	MSG(OPS, "u4RowAddr = 0x%08x\n", u4RowAddr);
	MSG(OPS, "|--------------------------|\n");
*/
    for(k=0; k<sizeof(g_kCMD.au1OOB); k++)
        *(g_kCMD.au1OOB+k) = 0xFF;
    
    while ((size > u4WriteLen) && (u4BlkAddr < u4BlkEnd))
	{
        if(!u4ColAddr)
        {
            MSG(OPS, "Erase the block of 0x%08x\n", u4BlkAddr);
            mt6577_nand_reset();
		    mt6577_nand_set_mode(CNFG_OP_ERASE);
	    	mt6577_nand_set_command(NAND_CMD_ERASE1);
    		mt6577_nand_set_address(0, u4RowAddr, 0, 3);
            mt6577_nand_set_command(NAND_CMD_ERASE2);
		    while(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY);
        }
    
		res = nand_block_bad(nand, ((u4BlkAddr >> 1) * u4PageNumPerBlock));
		
		if(!res)
		{
			u4RowEnd = (u4RowAddr + u4PageNumPerBlock) & (~u4PageNumPerBlock + 1);
			for (; u4RowAddr < u4RowEnd; u4RowAddr++)
			{
				i4Len = min(size - u4WriteLen, u4PageSize - u4ColAddr);
				if (0 >= i4Len)
				{
					break;
				}
				if ((u4ColAddr == 0) && (i4Len == u4PageSize)) 
				{
					mt6577_nand_exec_write_page(nand, u4RowAddr, u4PageSize, 
											   src + u4WriteLen, g_kCMD.au1OOB);
				} 
				else 
				{
                    memcpy(nand->buffers->databuf + u4ColAddr, src + u4WriteLen, i4Len);
					mt6577_nand_exec_write_page(nand, u4RowAddr, u4PageSize, 
											   nand->buffers->databuf, g_kCMD.au1OOB);
				}
				u4WriteLen += i4Len;
				u4ColAddr = (u4ColAddr + i4Len) & (u4PageSize - 1);
			}
		} 
		else 
		{		
		    printf("Detect bad block at block 0x%x\n", u4BlkAddr);
			u4RowAddr += u4PageNumPerBlock;
		}
		u4BlkAddr += 2;
	}

	return (int)u4WriteLen;

}
#endif

//-------------------------------------------------------------------------------	
static int mt6577_nand_part_read(part_dev_t *dev, ulong source, uchar *dst, int size)
{
	struct nand_chip *nand = (struct nand_chip *)dev->blkdev;
	uint8_t	res;        // *bbt = nand->bbt;
	u32 u4PageSize  = 1 << nand->page_shift;
	u32 u4PageNumPerBlock = 1 << (nand->phys_erase_shift - nand->page_shift);
	u32 u4BlkEnd  = (nand->chipsize >> nand->phys_erase_shift); // << 1;
	u32 u4BlkAddr = (source >> nand->phys_erase_shift); // << 1;
	u32 u4ColAddr = source & (u4PageSize - 1);
	u32 u4RowAddr = source >> nand->page_shift;
	u32 u4RowEnd;
	u32 u4ReadLen = 0;
	int i4Len;
/*
	MSG(OPS, "|--------------------------|\n");
	MSG(OPS, "start addr = 0x%08x\n", source);
	MSG(OPS, "dest addr = 0x%08x\n", dst);
	MSG(OPS, "size = %d\n", size);
	MSG(OPS, "u4BlkAddr = 0x%08x\n", (u4BlkAddr/2));
	MSG(OPS, "u4ColAddr = 0x%08x\n", u4ColAddr);
	MSG(OPS, "u4RowAddr = 0x%08x\n", u4RowAddr);
	MSG(OPS, "|--------------------------|\n");	
*/
	while ((size > u4ReadLen) && (u4BlkAddr < u4BlkEnd))
	{
		//res = (bbt[u4BlkAddr >> 3] >> (u4BlkAddr & 0x06)) & 0x03;
		res = nand_block_bad(nand, (u4BlkAddr * u4PageNumPerBlock));
		
		if(!res)
		{
			u4RowEnd = (u4RowAddr + u4PageNumPerBlock) & (~u4PageNumPerBlock + 1);
			for (; u4RowAddr < u4RowEnd; u4RowAddr++)
			{
				i4Len = min(size - u4ReadLen, u4PageSize - u4ColAddr);
				if (0 >= i4Len)
				{
					break;
				}
				if ((u4ColAddr == 0) && (i4Len == u4PageSize)) 
				{
					mt6577_nand_exec_read_page(nand, u4RowAddr, u4PageSize, 
											   dst + u4ReadLen, g_kCMD.au1OOB);
				} 
				else 
				{
					mt6577_nand_exec_read_page(nand, u4RowAddr, u4PageSize, 
											   nand->buffers->databuf, g_kCMD.au1OOB);
					memcpy(dst + u4ReadLen, nand->buffers->databuf + u4ColAddr, i4Len);
				}
				u4ReadLen += i4Len;
				u4ColAddr = (u4ColAddr + i4Len) & (u4PageSize - 1);
			}
		} 
		else 
		{		
		    printf("Detect bad block at block 0x%x\n", u4BlkAddr);
			u4RowAddr += u4PageNumPerBlock;
		}
		u4BlkAddr++;  //  += 2;
	}
	return (int)u4ReadLen;
}
//-------------------------------------------------------------------------------
#if 0
static void mt6577_nand_release_device (struct mtd_info *mtd)
{
    struct nand_chip *this = mtd->priv;

	MSG_FUNC_ENTRY();

    /* De-select the NAND device */
    this->select_chip(mtd, -1);
}
#endif
//-------------------------------------------------------------------------------
static void mt6577_nand_command_bp(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{  
	struct nand_chip* nand = mtd->priv;
    u32 timeout;
    
    switch (command) 
    {
	case NAND_CMD_SEQIN:
		/* Reset g_kCMD */
		if (g_kCMD.u4RowAddr != page_addr) {
			memset(g_kCMD.au1OOB, 0xFF, sizeof(g_kCMD.au1OOB));
			g_kCMD.pDataBuf = NULL;
		}
		g_kCMD.u4RowAddr = page_addr;
		g_kCMD.u4ColAddr = column;
        break;

    case NAND_CMD_PAGEPROG:
       	if (g_kCMD.pDataBuf || (0xFF != g_kCMD.au1OOB[0])) {
       		u8* pDataBuf = g_kCMD.pDataBuf ? g_kCMD.pDataBuf : nand->buffers->databuf;
			mt6577_nand_exec_write_page(nand, g_kCMD.u4RowAddr, mtd->writesize,
										pDataBuf, g_kCMD.au1OOB);
			g_kCMD.u4RowAddr = (u32)-1;
			g_kCMD.u4OOBRowAddr = (u32)-1;
       	}
        break;

    case NAND_CMD_READOOB:
		g_kCMD.u4RowAddr = page_addr;
		g_kCMD.u4ColAddr = column + mtd->writesize;
		g_i4ErrNum  = 0;		
		break;
			
    case NAND_CMD_READ0:
		g_kCMD.u4RowAddr = page_addr;
		g_kCMD.u4ColAddr = column;
		g_i4ErrNum  = 0;
		break;

    case NAND_CMD_ERASE1:
       	mt6577_nand_reset();
		mt6577_nand_set_mode(CNFG_OP_ERASE);
		mt6577_nand_set_command(NAND_CMD_ERASE1);
		mt6577_nand_set_address(0,page_addr,0,devinfo.addr_cycle-2);
        break;
            
    case NAND_CMD_ERASE2:
       	mt6577_nand_set_command(NAND_CMD_ERASE2);
		while(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY);
        break;
            
    case NAND_CMD_STATUS:
       	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
       	mt6577_nand_reset();        	
		mt6577_nand_set_mode(CNFG_OP_SRD);
		mt6577_nand_set_command(NAND_CMD_STATUS);
       	NFI_CLN_REG16(NFI_CON_REG16, CON_NFI_NOB_MASK);
		DRV_WriteReg16(NFI_CON_REG16, CON_NFI_SRD|(1 << CON_NFI_NOB_SHIFT));
        break;
            
    case NAND_CMD_RESET:
       	mt6577_nand_reset();
		//mt6577_nand_exec_reset_device();
        break;

	case NAND_CMD_READID:      
        /* Issue NAND chip reset command */
        NFI_ISSUE_COMMAND(NAND_CMD_RESET, 0, 0, 0, 0);

        timeout = TIMEOUT_4;

        while(timeout)
            timeout--;

        mt6577_nand_reset();
        /* Disable HW ECC */
        NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);

        /* Disable 16-bit I/O */
        NFI_CLN_REG16(NFI_PAGEFMT_REG16, PAGEFMT_DBYTE_EN);
                
        NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN|CNFG_BYTE_RW);
        //NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
		mt6577_nand_set_mode(CNFG_OP_SRD);
		mt6577_nand_set_command(NAND_CMD_READID);
		mt6577_nand_set_address(0,0,1,0);
		DRV_WriteReg16(NFI_CON_REG16, CON_NFI_SRD);
		while(DRV_Reg32(NFI_STA_REG32) & STA_DATAR_STATE);
		break; 
    default:
		printf("[ERR] mt6577_nand_command_bp : unknow command %d\n", command);
        BUG();        
        break;
    }
}
//-----------------------------------------------------------------------------
static void mt6577_nand_select_chip(struct mtd_info *mtd, int chip)
{
    u32 busw = 0;

	if (chip == -1 && false == g_bInitDone){
		struct nand_chip *nand = mtd->priv;
            /* Infinity { */
            if (nand->page_shift == 12) { 
                NFI_SET_REG16(NFI_PAGEFMT_REG16, PAGEFMT_SPARE_16 | PAGEFMT_4K);
                nand->cmdfunc = mt6577_nand_command_bp;		    
            }
            /* Infinity } */
	    else if (nand->page_shift == 11){ //2 //Large Page
			NFI_SET_REG16(NFI_PAGEFMT_REG16, PAGEFMT_SPARE_16 | PAGEFMT_2K);
        	nand->cmdfunc = mt6577_nand_command_bp;
	    }
    	else if(nand->page_shift == 9) //2 // Small Page
	    {
			NFI_SET_REG16(NFI_PAGEFMT_REG16, PAGEFMT_SPARE_16 | PAGEFMT_512);
        	//nand->cmdfunc = mt6577_nand_command_sp;
	    }

        busw = devinfo.iowidth;

        if(IO_WIDTH_16 == busw)
        {
            MSG(FUC, "Setting the NFI PAGEFMT to enable %d bit I/O\n", busw);
            NFI_SET_REG16(NFI_PAGEFMT_REG16, PAGEFMT_DBYTE_EN);            
        }
        else if(IO_WIDTH_8 == busw)
        {
            MSG(FUC, "Setting the NFI PAGEFMT to enable %d bit I/O\n", busw);
        }
        else
        {
            MSG(FUC, "Setting NFI_BUS_WIDTH (%d) is error, please check the NAND setting in UBOOT\n", busw);
            BUG();
        }
/*        
#if defined(CFG_MT6516_NAND_WIDTH_16)
        printf("Setting the NFI PAGEFMT to enable 16 bit I/O\n");
        NFI_SET_REG16(NFI_PAGEFMT_REG16, PAGEFMT_DBYTE_EN);
#endif        
*/
		g_bInitDone = true;
	}

    switch(chip) 
    {
	case 0: 
	case 1:
	    /* Jun Shen, 2011.04.13 */
	    /* Note: Should be fix to find why chip var is ?? */
		DRV_WriteReg16(NFI_CSEL_REG16, chip);
		/* Jun Shen, 2011.04.13 */
		break;
    }
}
//-----------------------------------------------------------------------------
static u_char mt6577_nand_read_byte(struct mtd_info *mtd)
{
    /* Check the PIO bit is ready or not */
    unsigned int timeout = TIMEOUT_4;
    WAIT_NFI_PIO_READY(timeout);
    return DRV_Reg32(NFI_DATAR_REG32);
}
//-----------------------------------------------------------------------------
static void mt6577_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{	
	struct nand_chip* nand = (struct nand_chip*)mtd->priv;
	struct mt6577_CMD* pkCMD = &g_kCMD;
	u32 u4ColAddr = pkCMD->u4ColAddr;
	u32 u4PageSize = mtd->writesize;

	if (u4ColAddr < u4PageSize) {
		if ((u4ColAddr == 0) && (len >= u4PageSize)) {
			mt6577_nand_exec_read_page(nand, pkCMD->u4RowAddr, u4PageSize, 
									   buf, pkCMD->au1OOB);
			if (len > u4PageSize) {
				u32 u4Size = min(len - u4PageSize, sizeof(pkCMD->au1OOB));
				memcpy(buf + u4PageSize, pkCMD->au1OOB, u4Size);
			}
		} else {
			mt6577_nand_exec_read_page(nand, pkCMD->u4RowAddr, u4PageSize, 
									   nand->buffers->databuf, pkCMD->au1OOB);
			memcpy(buf, nand->buffers->databuf + u4ColAddr, len);
		}
		pkCMD->u4OOBRowAddr = pkCMD->u4RowAddr;
	} else {
		u32 u4Offset = u4ColAddr - u4PageSize;
		u32 u4Size = min(len - u4PageSize - u4Offset, sizeof(pkCMD->au1OOB));
		if (pkCMD->u4OOBRowAddr != pkCMD->u4RowAddr) {
			mt6577_nand_exec_read_page(nand, pkCMD->u4RowAddr, u4PageSize,
									   nand->buffers->databuf, pkCMD->au1OOB);
			pkCMD->u4OOBRowAddr = pkCMD->u4RowAddr;
		}
		memcpy(buf, pkCMD->au1OOB + u4Offset, u4Size);
	}
	pkCMD->u4ColAddr += len;
}
//-----------------------------------------------------------------------------
static void mt6577_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	struct mt6577_CMD* pkCMD = &g_kCMD;
	u32 u4ColAddr = pkCMD->u4ColAddr;
	u32 u4PageSize = mtd->writesize;
	u32 i;

	if (u4ColAddr >= u4PageSize) {
		u8* pOOB = pkCMD->au1OOB;
		u32 u4Size = min(len, sizeof(pkCMD->au1OOB));
		for (i = 0; i < u4Size; i++) {
			pOOB[i] &= buf[i];
		}
	} else {
		pkCMD->pDataBuf = (u8*)buf;
	}
	pkCMD->u4ColAddr += len;
}
//-----------------------------------------------------------------------------
static int mt6577_nand_verify_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	return 0;	/* FIXME. Always return success */
}
//-----------------------------------------------------------------------------
static int mt6577_nand_dev_ready(struct mtd_info *mtd)
{
	return !(DRV_Reg32(NFI_STA_REG32) & STA_NAND_BUSY);
}

#if 0
/******************************************************************************
 * mt6577_read_multi_page_cache
 *
 * DESCRIPTION:
 *   read multi page data using cache read
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, int page, int len
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   Only available for NAND flash support cache read.
 *   Read main data only.
 *
 *****************************************************************************/
static int mt6577_read_multi_page_cache(struct mtd_info *mtd, struct nand_chip *chip,
        uint8_t *buf, int page, int len)
{
    // mt6577_nand_ready_for_read(chip, page, 0, true);
            
    while (len > 0)
    {
        mt6577_nand_set_mode(CNFG_OP_CUST);
        DRV_WriteReg16(NFI_CON_REG16, 8 << CON_NFI_SEC_SHIFT);
        ECC_Decode_Start();

        if (len > mtd->writesize)               // remained more than one page
            mt6577_nand_set_command(0x31);      // TODO: add cache read command
        else
            mt6577_nand_set_command(0x3f);      // last page remained

        mt6577_nand_status_ready(STA_NAND_BUSY);

#ifdef USE_AHB_MODE
        mt6577_nand_dma_transfer_data(buf, mtd->writesize);
#else
        mt6577_nand_mcu_transfer_data(buf, mtd->writesize);
#endif
        // get ecc error info
        
        ECC_Decode_End();

        len -= mtd->writesize;
        buf += mtd->writesize;
        mt6577_nand_reset();
    }
}
#endif

//-----------------------------------------------------------------------------
#if 0
static void mt6577_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{	
	//1 No need to implement
    return;
}
#endif
//-----------------------------------------------------------------------------
static int mt6577_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
    /* ECC is encoded and decoded automatically at reading and programning buf. */
	memset(ecc_code, 0xFF, 32);
    return 0;
}
//-----------------------------------------------------------------------------
static int mt6577_nand_correct_data(struct mtd_info *mtd, u_char *dat, u_char *read_ecc, u_char *calc_ecc)
{
    return g_i4ErrNum;
}

static void mt6577_nand_hwctl(struct mtd_info *mtd, int mode)
{
}
bool getflashid(u16 *id)
{
    	u8 maf_id=0;
	u8 dev_id=0;
	struct mtd_info *mtd = NULL;
	//PDN_Power_CONA_DOWN(PDN_PERI_NFI, KAL_FALSE); 

 	DRV_WriteReg16(NFI_CNFG_REG16, 0);
	DRV_WriteReg16(NFI_PAGEFMT_REG16, 0);	
	
	/* Set default NFI access timing control */
	DRV_WriteReg32(NFI_ACCCON_REG32, NFI_DEFAULT_ACCESS_TIMING);
	

	mt6577_nand_command_bp(mtd, NAND_CMD_READID, 0,0);
	maf_id = mt6577_nand_read_byte (mtd);
	dev_id = mt6577_nand_read_byte (mtd);

	if(maf_id==0||dev_id==0)
	{
		return false;
	}
	//*id= (dev_id<<8)|maf_id;
        *id= (maf_id<<8)|dev_id;
	return true;
}

//-----------------------------------------------------------------------------
static int mt6577_nand_init(struct nand_chip *this)
{
    /* Power on NFI HW component. */    
    //PDN_Power_CONA_DOWN(PDN_PERI_NFI, KAL_FALSE); 

	/* Dynamic Control */
	g_bInitDone = false;
    g_u4ChipVer = DRV_Reg32(CONFIG_BASE/*HW_VER*/);
	g_kCMD.u4OOBRowAddr  = (u32)-1;
	
	/* Jun Shen, 2011.04.13  */
	/* Note: MT6577 EVB NAND  is mounted on CS0, but FPGA is CS1  */
	DRV_WriteReg16(NFI_CSEL_REG16, NFI_DEFAULT_CS);
	/* Jun Shen, 2011.04.13  */

	DRV_WriteReg16(NFI_CNFG_REG16, 0);
	DRV_WriteReg16(NFI_PAGEFMT_REG16, 0);	
	
	/* Set default NFI access timing control */
	DRV_WriteReg32(NFI_ACCCON_REG32, NFI_DEFAULT_ACCESS_TIMING);

    /* Reset NFI HW internal state machine and flush NFI in/out FIFO */    
    mt6577_nand_reset();

	/* Initilize interrupt. Clear interrupt, read clear. */    
	DRV_Reg16(NFI_INTR_REG16);
	
	/* Interrupt arise when read data or program data to/from AHB is done. */    
	DRV_WriteReg16(NFI_INTR_EN_REG16, 0);

	DRV_WriteReg16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	ECC_Config();
	mt6577_nand_configure_fdm(8);
	mt6577_nand_configure_lock();

    return 0;
}
//-----------------------------------------------------------------------------
int board_nand_init(struct nand_chip *nand)
{
	int res = mt6577_nand_init(nand);
    int busw = IO_WIDTH_8 ;
	int id, nand_maf_id,nand_dev_id;
    u8 ext_id1, ext_id2, ext_id3;
    u32 ext_id;
	struct mtd_info *mtd = NULL;	
	nand->select_chip 	= mt6577_nand_select_chip;
	nand->cmdfunc 		= mt6577_nand_command_bp;
	nand->read_byte   	= mt6577_nand_read_byte;
	nand->write_buf   	= mt6577_nand_write_buf;
	nand->dev_ready   	= mt6577_nand_dev_ready;
	//nand->enable_hwecc 	= mt6577_nand_enable_hwecc;
	nand->ecc.calculate     = mt6577_nand_calculate_ecc;
	nand->ecc.correct 	= mt6577_nand_correct_data;
	nand->verify_buf 	= mt6577_nand_verify_buf;
	nand->read_buf  	= mt6577_nand_read_buf;
	//nand->block_markbad = mt6577_nand_default_block_markbad;

	nand->ecc.mode 		= NAND_ECC_HW;
    nand->ecc.hwctl     = mt6577_nand_hwctl;
    nand->ecc.layout    = &nand_oob_64;
    nand->ecc.size      = 512;
	nand->options 	    = NAND_NO_AUTOINCR;

    mt6577_nand_command_bp(mtd, NAND_CMD_READID, 0,0);
    nand_maf_id = mt6577_nand_read_byte (mtd);
    nand_dev_id = mt6577_nand_read_byte (mtd);
    //add for multi-device support
    id = nand_dev_id| (nand_maf_id << 8);
    
    ext_id1 = mt6577_nand_read_byte (mtd);
    ext_id2 = mt6577_nand_read_byte (mtd);
    ext_id3 = mt6577_nand_read_byte (mtd);
    ext_id = ext_id1 << 16 | ext_id2 << 8 | ext_id3;
    
    if(get_device_info(id, ext_id, &devinfo))
    {
        busw = devinfo.iowidth;
        DRV_WriteReg32 (NFI_ACCCON_REG32, devinfo.timmingsetting);
        // MSG (FUC, "NAND device found in MTK table!!!\n");

        if (devinfo.pagesize == 4096)
            nand->ecc.layout = &nand_oob_128;
        else if (devinfo.pagesize == 2048)
            nand->ecc.layout = &nand_oob_64;
        else if (devinfo.pagesize == 512)
            nand->ecc.layout = &nand_oob_16;
    }
    else
    {
        MSG (INIT, "No NAND device found!\n");
    }

    if(IO_WIDTH_16 == busw)
    {
        MSG(FUC, "Setting the MTD option to enable %d bit I/O\n", busw);
        nand->options |= NAND_BUSWIDTH_16;
    }
    else if(IO_WIDTH_8 == busw)
    {
        MSG(FUC, "Setting the MTD option to enable %d bit I/O\n", busw);
    }
    else
    {
        MSG(FUC, "Setting NFI_BUS_WIDTH (%d) is error, please check the NAND setting in UBOOT\n", busw);
        BUG();
    } 


    return res;
	
}
#endif
//-----------------------------------------------------------------------------
#ifdef CONFIG_SYS_NAND_SELECT_DEVICE
extern int mt6577_part_register_device(part_dev_t *dev);
void board_nand_select_device(struct nand_chip *nand, int chip)
{
	static part_dev_t dev;
	mt6577_nand_select_chip(NULL, chip);
	
    nand->chipsize -= BMT_POOL_SIZE << nand->phys_erase_shift;
    total_size = nand->chipsize- (PMT_POOL_SIZE<<nand->phys_erase_shift);
    if (!g_bmt)
    {
        if ( !(g_bmt = init_bmt(nand, BMT_POOL_SIZE)) )
        {
            MSG(INIT, "Error: init bmt failed\n");
            // ASSERT(0);
            return;
        }
    }
	//move to here so can access nand when init partition	
	dev.id     = 0;
	dev.init   = 1;
	dev.blkdev = (block_dev_desc_t*)nand;
	dev.read   = mt6577_nand_part_read;
	MSG(FUC, "board_nand_select_device before register\n");
	mt6577_part_register_device(&dev);
	
	//	PDN_Power_CONA_DOWN(PDN_PERI_NFI, KAL_TRUE);

    return;
}
#endif
//-----------------------------------------------------------------------------

#endif
