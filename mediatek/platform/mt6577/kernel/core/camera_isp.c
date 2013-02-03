/******************************************************************************
 * camera_isp.c - MT6577 Linux ISP Device Driver
 *
 * Copyright 2008-2009 MediaTek Co.,Ltd.
 *
 * DESCRIPTION:
 *     This file provid the other drivers ISP relative functions
 *
 ******************************************************************************/

#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <asm/io.h>
//#include <asm/tcm.h>
#include <linux/proc_fs.h>  //proc file use
//
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <asm/atomic.h>
#include <linux/sched.h>
#include <linux/mm.h>
//
#include <mach/mt6577_reg_base.h>
//#include <mach/mt6577_pll.h>
#include <mach/camera_isp.h>
//
#include <mach/mt6577_clock_manager.h>

/*******************************************************************************
*
********************************************************************************/
#define LOG_MSG(fmt, arg...)    printk(KERN_DEBUG   "[ISP][%s]"          fmt "\r\n", __FUNCTION__,           ##arg)
#define LOG_DBG(fmt, arg...)    printk(KERN_INFO    "[ISP][%s]"          fmt "\r\n", __FUNCTION__,           ##arg)
#define LOG_WRN(fmt, arg...)    printk(KERN_DEBUG   "[ISP][%s]WRN(%5d):" fmt "\r\n", __FUNCTION__, __LINE__, ##arg)
#define LOG_ERR(fmt, arg...)    printk(KERN_ERR     "[ISP][%s]ERR(%5d):" fmt "\r\n", __FUNCTION__, __LINE__, ##arg)
#define LOG_DMP(fmt, arg...)    printk(KERN_ERR     ""                   fmt,                                ##arg)
//
typedef unsigned char   MUINT8;
typedef unsigned short  MUINT16;
typedef unsigned int    MUINT32;
typedef char            MINT8;
typedef short           MINT16;
typedef int             MINT32;
typedef bool            MBOOL;
#define MTRUE           (1)
#define MFALSE          (0)
//
#if (defined(CONFIG_MT6575T_FPGA)||defined(CONFIG_MT6577_FPGA))
#define ISP_REAL_CHIP   (0)
#else
#define ISP_REAL_CHIP   (1)
#endif
//
#define ISP_WR32(addr, data)    iowrite32(data, addr)
#define ISP_RD32(addr)          ioread32(addr)
#define ISP_SET_BIT(reg, bit)   ((*(volatile MUINT32*)(reg)) |= (MUINT32)(1 << (bit)))
#define ISP_CLR_BIT(reg, bit)   ((*(volatile MUINT32*)(reg)) &= ~((MUINT32)(1 << (bit))))
//
#define ISP_BASE                CAMINF_BASE

#define ISP_DEV_NAME            "camera-isp"

#define CAMIODRV_BASE           (0xC0001000)

/*******************************************************************************
*
********************************************************************************/
// Register definition
#define ISP_PHSCNT              (ISP_BASE + 0x000)
#define ISP_VFCON               (ISP_BASE + 0x018)
#define ISP_INTEN               (ISP_BASE + 0x01c)
#define ISP_INTSTA              (ISP_BASE + 0x020)
#define ISP_PATH                (ISP_BASE + 0x024)
#define ISP_FLK_CON             (ISP_BASE + 0x0E0)
#define ISP_RAWGAIN0            (ISP_BASE + 0x16C)
#define ISP_RAWGAIN1            (ISP_BASE + 0x170)
#define ISP_RESET               (ISP_BASE + 0x1D8)
#define ISP_TGSTATUS            (ISP_BASE + 0x1DC)
#define ISP_SHADING1            (ISP_BASE + 0x214)
#define ISP_VERSION             (ISP_BASE + 0x274)
#define ISP_INTSTA_STNR         (ISP_BASE + 0x684)
//
#define ISP_G1MEMPDN            (MMSYS1_CONFIG_BASE + 0x30c)
//
#define ISP_INT_OVERRUN_MASK    ((MUINT32)0x6)
#define ISP_INT_STNR_STATUS     ((MUINT32)0x1)

#define ISP_DBG_INT             0x0001
#define ISP_DBG_HOLD_REG        0x0002
#define ISP_DBG_READ_REG        0x0004
#define ISP_DBG_WRITE_REG       0x0008
#define ISP_DBG_CLK             0x0010
#define ISP_DBG_TASKLET         0x0020
#define ISP_DBG_WORKQUEUE       0x0040
#define ISP_DBG_BUF_WRITE       0x0080
//
#define ISP_TASKLET_TIMEOUT         500 //ms
#define ISP_STNR_TIMEOUT_BASE       10  //ms
#define ISP_STNR_TIMEOUT_COUNT_MIN  2

/*******************************************************************************
*
********************************************************************************/
typedef enum
{
    ISP_SPINLOCK_IRQ,
    ISP_SPINLOCK_BUF_WRITE,
    ISP_SPINLOCK_AMOUNT
}ISP_SPINLOCK_ENUM;

MUINT32 g_jiffies_exp_done = 0;
MUINT32 g_jiffies_tasklet = 0;
static spinlock_t IspSpinlock[ISP_SPINLOCK_AMOUNT];
//static MUINT8* pcmd_buf = NULL;
//static MUINT8* pread_buf = NULL;
//static MUINT8* pwrite_buf = NULL;
//static MUINT32 buf_size = 4096;
static atomic_t hold_reg;
static atomic_t writing_reg;
//static MUINT32 hold_count;
static wait_queue_head_t isp_wait_queue;
static MUINT32 irq_status, irq_status_wait_irq;
static MUINT32 tg2_irq_status;
//static MUINT32 dbgMask = 0;
static MUINT32 g_u4DbgRegOfs = 0x0600;
static struct work_struct isp_work_queue;
static volatile MUINT32 IspVdCount;
static volatile MBOOL IspVdProcEn;
static volatile MBOOL IspOpen;
/*******************************************************************************
*
********************************************************************************/
#define ISP_BUF_SIZE            4096
#define ISP_BUF_SIZE_WRITE      1024
#define ISP_BUF_WRITE_AMOUNT    6

typedef enum
{
    ISP_BUF_STATUS_EMPTY,
    ISP_BUF_STATUS_WRITING,
    ISP_BUF_STATUS_HOLD,
    ISP_BUF_STATUS_READY
}ISP_BUF_STATUS_ENUM;

typedef struct
{
    volatile ISP_BUF_STATUS_ENUM    Status;
    volatile MUINT32                Size;
    MUINT8*                         pData;
}ISP_BUF_WRITE_STRUCT;

ISP_BUF_WRITE_STRUCT IspBufWrite[ISP_BUF_WRITE_AMOUNT];

/*******************************************************************************
*
********************************************************************************/
typedef struct
{
    MUINT32             LogMask;
    ISP_IOC_CMD_ENUM    CurCmd;
    MBOOL               IsWorkQueue;
    MBOOL               IsTasklet;
}ISP_DEBUG_INFO_STRUCT;

static volatile ISP_DEBUG_INFO_STRUCT IspDebugInfo;
/*******************************************************************************
*
********************************************************************************/
static void mt_isp_write_buf_init(void)
{
    MINT32 i;
    //
    if(IspDebugInfo.LogMask & ISP_DBG_BUF_WRITE)
    {
        LOG_MSG("");
    }
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        IspBufWrite[i].Status = ISP_BUF_STATUS_EMPTY;
        IspBufWrite[i].Size = 0;
        IspBufWrite[i].pData = NULL;
    }
}

/*******************************************************************************
*
********************************************************************************/
static void mt_isp_write_buf_dump(void)
{
    MINT32 i;
    //
    LOG_DBG("");
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        LOG_DBG("i(%d),Status(%d),Size(%d)",
                i,
                IspBufWrite[i].Status,
                IspBufWrite[i].Size);
    }
}


/*******************************************************************************
*
********************************************************************************/
static void mt_isp_write_buf_free(void)
{
    MINT32 i;
    //
    if(IspDebugInfo.LogMask & ISP_DBG_BUF_WRITE)
    {
        LOG_MSG("");
    }
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        IspBufWrite[i].Status = ISP_BUF_STATUS_EMPTY;
        IspBufWrite[i].Size = 0;
        if(IspBufWrite[i].pData != NULL)
        {
            kfree(IspBufWrite[i].pData);
            IspBufWrite[i].pData = NULL;
        }
    }
}

/*******************************************************************************
*
********************************************************************************/
static MBOOL mt_isp_write_buf_alloc(void)
{
    MINT32 i;
    //
    if(IspDebugInfo.LogMask & ISP_DBG_BUF_WRITE)
    {
        LOG_MSG("");
    }
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        IspBufWrite[i].Status = ISP_BUF_STATUS_EMPTY;
        IspBufWrite[i].Size = 0;
        IspBufWrite[i].pData = (MUINT8*)kmalloc(ISP_BUF_SIZE_WRITE, GFP_KERNEL);
        if(IspBufWrite[i].pData == NULL)
        {
            LOG_ERR("i(%d),pData is NULL",i);
            mt_isp_write_buf_free();
            return MFALSE;
        }
    }
    return MTRUE;
}

/*******************************************************************************
*
********************************************************************************/
static void mt_isp_write_buf_reset(void)
{
    MINT32 i;
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        IspBufWrite[i].Status = ISP_BUF_STATUS_EMPTY;
        IspBufWrite[i].Size = 0;
    }
}

/*******************************************************************************
*
********************************************************************************/
static __inline MUINT32 mt_isp_write_buf_get_amount(void)
{
    MINT32 i, Count = 0;
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        if(IspBufWrite[i].Status == ISP_BUF_STATUS_READY)
        {
            Count++;
        }
    }
    //
    if(IspDebugInfo.LogMask & ISP_DBG_BUF_WRITE)
    {
        LOG_MSG("Count(%d)",Count);
    }
    return Count;
}

/*******************************************************************************
*
********************************************************************************/
static MBOOL mt_isp_write_buf_add(
    MUINT32     Size,
    MUINT8*     pData)
{
    MINT32 i;
    MBOOL Ret = MTRUE;
    MUINT8* pWriteData;
    //
    //LOG_MSG("");
    if(Size > ISP_BUF_SIZE_WRITE)
    {
        LOG_ERR("Size(%d) > %d",Size,ISP_BUF_SIZE_WRITE);
        Ret = MFALSE;
        goto EXIT;
    }
    //
    spin_lock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        if(IspBufWrite[i].Status == ISP_BUF_STATUS_HOLD)
        {
            if((IspBufWrite[i].Size+Size) > ISP_BUF_SIZE_WRITE)
            {
                spin_unlock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
                LOG_ERR("i(%d), BufWriteSize(%d)+Size(%d) > %d",
                        i,
                        IspBufWrite[i].Size,
                        Size,
                        ISP_BUF_SIZE_WRITE);
                Ret = MFALSE;
                goto EXIT;
            }
            //
            IspBufWrite[i].Status = ISP_BUF_STATUS_WRITING;
            pWriteData = IspBufWrite[i].pData+IspBufWrite[i].Size;
            IspBufWrite[i].Size += Size;
            break;
        }
    }
    spin_unlock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
    //
    if(i<ISP_BUF_WRITE_AMOUNT)
    {
        if(copy_from_user(pWriteData, pData, Size) != 0)
        {
            LOG_ERR("copy_from_user failed");
            Ret = MFALSE;
            goto EXIT;
        }
        //
        spin_lock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
        IspBufWrite[i].Status = ISP_BUF_STATUS_HOLD;
        spin_unlock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
    }
    else
    {
        spin_lock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
        for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
        {
            if(IspBufWrite[i].Status == ISP_BUF_STATUS_EMPTY)
            {
                IspBufWrite[i].Status = ISP_BUF_STATUS_WRITING;
                IspBufWrite[i].Size = Size;
                break;
            }
        }
        spin_unlock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
        //
        if(i<ISP_BUF_WRITE_AMOUNT)
        {
            if(copy_from_user(IspBufWrite[i].pData, pData, Size) != 0)
            {
                LOG_ERR("copy_from_user failed");
                Ret = MFALSE;
                goto EXIT;
            }
            //
            spin_lock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
            IspBufWrite[i].Status = ISP_BUF_STATUS_HOLD;
            spin_unlock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
            //
            if(IspDebugInfo.LogMask & ISP_DBG_BUF_WRITE)
            {
                LOG_MSG("i(%d), Size(%d)",i,Size);
            }
        }
        else
        {
            LOG_ERR("All write buffer are full of data!");
            Ret = MFALSE;
        }
    }
    //
    EXIT:
    //
    if(!Ret)
    {
        LOG_ERR("process(%s),pid(%d),tgid(%d)",
                current->comm,
                current->pid,
                current->tgid);
    }
    //
    return Ret;
}

/*******************************************************************************
*
********************************************************************************/
static void mt_isp_write_buf_data_ready(void)
{
    MINT32 i;
    //
    //LOG_MSG("");
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        if(IspBufWrite[i].Status == ISP_BUF_STATUS_HOLD)
        {
            IspBufWrite[i].Status = ISP_BUF_STATUS_READY;
        }
    }
}

/*******************************************************************************
*
********************************************************************************/
static MBOOL mt_isp_write_buf_get(
    MUINT32*    pIndex,
    MUINT32*    pSize,
    MUINT8**    ppData)
{
    MINT32 i;
    //
    //LOG_MSG("");
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        if(IspBufWrite[i].Status == ISP_BUF_STATUS_READY)
        {
            *pIndex = i;
            *pSize = IspBufWrite[i].Size;
            *ppData = IspBufWrite[i].pData;
            return MTRUE;
        }
    }
    //
    return MFALSE;
}

/*******************************************************************************
*
********************************************************************************/
static MBOOL mt_isp_write_buf_clear(MUINT32  Index)
{
    //LOG_MSG("");
    //
    if(IspBufWrite[Index].Status == ISP_BUF_STATUS_READY)
    {
        IspBufWrite[Index].Size = 0;
        IspBufWrite[Index].Status = ISP_BUF_STATUS_EMPTY;
        return MTRUE;
    }
    else
    {
        //LOG_WRN("Index(%d) is not ready! Status(%d)",Index,IspBufWrite[Index].Status);
        return MFALSE;
    }
}

/*******************************************************************************
*
********************************************************************************/
static MUINT32 ms_to_jiffies(MUINT32 ms)
{
    return ((ms * HZ + 512) >> 10);
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_dump_reg(void)
{
    MINT32 ret = 0;
    MINT32 i;

    LOG_DBG("Start");
    //
    //spin_lock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
    //
    for (i = 0; i < 0x818; i += 4)
    {
        LOG_DBG("addr(0x%08X),val(0x%08X)", ISP_BASE + i, ISP_RD32(ISP_BASE + i));
    }
    //  CSI2 debug message
    for (i = 0x2000; i < 0x2030; i += 4)
    {
        LOG_DBG("addr(0x%08X),val(0x%08X)", ISP_BASE + i, ISP_RD32(ISP_BASE + i));
    }

    //  CSI2 debug message dump again to check the flag update to latest.
    for (i = 0x2000; i <= 0x2030; i += 4)
    {
        LOG_DBG("addr(0x%08X),val(0x%08X)", ISP_BASE + i, ISP_RD32(ISP_BASE + i));
    }
    //
    LOG_DBG("addr(0x%08X),val(0x%08X)", MMSYS1_CONFIG_BASE + 0x24, ISP_RD32(MMSYS1_CONFIG_BASE + 0x24));
    //
    LOG_DBG("addr(0x%08X),val(0x%08X)", AP_RGU_BASE + 0x1890, ISP_RD32(AP_RGU_BASE + 0x1890));
    //
    LOG_DBG("addr(0x%08X),val(0x%08X)", MIPI_CONFG_BASE + 0x800, ISP_RD32(MIPI_CONFG_BASE + 0x800));
    LOG_DBG("addr(0x%08X),val(0x%08X)", MIPI_CONFG_BASE + 0x844, ISP_RD32(MIPI_CONFG_BASE + 0x844));
    LOG_DBG("addr(0x%08X),val(0x%08X)", MIPI_CONFG_BASE + 0x850, ISP_RD32(MIPI_CONFG_BASE + 0x850));
    LOG_DBG("addr(0x%08X),val(0x%08X)", MIPI_CONFG_BASE + 0x854, ISP_RD32(MIPI_CONFG_BASE + 0x854));
    //
    //spin_unlock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
    //
    LOG_DBG("End" );

    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_clk_ctrl(MINT32 en)
{
    if(IspDebugInfo.LogMask & ISP_DBG_CLK)
    {
        LOG_MSG("%d", en);
    }
    //
    if(en)
    {
        #if ISP_REAL_CHIP
        if(enable_pll(MT65XX_IPLL, "ISP"))
        {
            LOG_ERR("MT65XX_IPLL enable fail");
        }
        hwEnableClock(MT65XX_PDN_MM_RESZ_LB, "ISP");
        #endif
        //
        ISP_CLR_BIT(ISP_G1MEMPDN, 9);
        //
        #if ISP_REAL_CHIP
        hwEnableClock(MT65XX_PDN_MM_CAM, "ISP");
        #endif
    }
    else
    {
        #if ISP_REAL_CHIP
        if(disable_pll(MT65XX_IPLL, "ISP"))
        {
            LOG_ERR("MT65XX_IPLL disable fail");
        }
        hwDisableClock(MT65XX_PDN_MM_RESZ_LB, "ISP");
        #endif
        //
        ISP_SET_BIT(ISP_G1MEMPDN, 9);
        //
        #if ISP_REAL_CHIP
        hwDisableClock(MT65XX_PDN_MM_CAM, "ISP");
        #endif
    }

    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static inline void mt_isp_reset(void)
{
    LOG_MSG("E");
    //
    spin_lock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
    // ensure the view finder is disabe
    ISP_CLR_BIT(ISP_VFCON, 6);
    mb();
    // do hw reset
    ISP_WR32(ISP_RESET, 1);
    mb();
    // delay at least 100us for reset the ISp (HW suggest)
    udelay(120);
    mb();
    ISP_WR32(ISP_RESET, 0);
    mb();
    ISP_WR32(ISP_VFCON, ISP_RD32(ISP_VFCON)|0xE000); //0x0018 must set bit [15:13] as 7
    mb();
    //
    irq_status = 0;
    irq_status_wait_irq = 0;
    //
    spin_unlock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
    //
    LOG_MSG("X");
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_read_reg(mt_isp_reg_io_t *preg_io)
{
    MINT32 i, ret = 0, size = preg_io->count * sizeof(mt_isp_reg_t);
    mt_isp_reg_t* preg = (mt_isp_reg_t*)kmalloc(size, GFP_KERNEL);
    //
    if (IspDebugInfo.LogMask & ISP_DBG_READ_REG) {
        LOG_MSG("data(0x%08X),count(%d)",
                (MUINT32)preg_io->data,
                (MUINT32)preg_io->count);
    }
    //
    if(preg == NULL)
    {
        LOG_ERR("kmalloc failed");
        ret = -ENOMEM;
        goto EXIT;
    }
    //
    if (size > ISP_BUF_SIZE) {
        LOG_ERR("size too big");
    }
    //
    if (copy_from_user((MUINT8 *)preg, (MUINT8 *) preg_io->data, size) != 0) {
        LOG_ERR("copy_from_user failed");
        ret = -EFAULT;
        goto EXIT;
    }
    //
    for (i = 0; i < preg_io->count; i++) {
        preg[i].val = ISP_RD32(ISP_BASE + preg[i].addr);
        if (IspDebugInfo.LogMask & ISP_DBG_READ_REG) {
            LOG_MSG("addr(0x%08X),val(0x%08X)",
                    (MUINT32)(ISP_BASE + preg[i].addr),
                    (MUINT32)preg[i].val);
        }
    }
    //
    if (copy_to_user((MUINT8 *) preg_io->data, (MUINT8 *) preg, size) != 0) {
        LOG_ERR("copy_to_user failed");
        ret = -EFAULT;
        goto EXIT;
    }
    //
    EXIT:
    if(preg != NULL)
    {
        kfree((MUINT8*)preg);
        preg = NULL;
    }
    //
    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_write_reg_to_hw(
    mt_isp_reg_t*   preg,
    MUINT32         count)
{
    MINT32 ret = 0;
    MINT32 i;
    //
    for (i = 0; i < count; i++)
    {
        ISP_WR32(ISP_BASE + preg[i].addr, preg[i].val);
    }
    //
    return ret;
}

/*******************************************************************************
*
********************************************************************************/
void mt_isp_tasklet_write_reg(unsigned long arg)
{
    MUINT32     Index, BufSize;
    MUINT8*     pBuf;
    //
    IspDebugInfo.IsTasklet = MTRUE;
    //
    if(IspDebugInfo.LogMask & ISP_DBG_TASKLET)
    {
        LOG_MSG("write buf amount(%d)",mt_isp_write_buf_get_amount());
    }
    //  Record it for debug.
    g_jiffies_tasklet = jiffies;
    //
    while(mt_isp_write_buf_get(&Index,&BufSize,&pBuf))
    {
        mt_isp_write_reg_to_hw((mt_isp_reg_t*)pBuf, BufSize/sizeof(mt_isp_reg_t));
        mt_isp_write_buf_clear(Index);
    }
    //LOG_MSG("No more buf.");
    atomic_set(&writing_reg, 0);
    wake_up_interruptible(&isp_wait_queue);
    //
    if(IspDebugInfo.LogMask & ISP_DBG_TASKLET)
    {
        LOG_MSG("End");
    }
    //
    IspDebugInfo.IsTasklet = MFALSE;
}

DECLARE_TASKLET(tasklet_write_reg, mt_isp_tasklet_write_reg, 0);

/*******************************************************************************
*
********************************************************************************/
void mt_isp_tasklet_handle_overrun(unsigned long arg)
{
    // FIXME, if 75 still need workaround for overrun
    #if 0
    LOG_MSG("[handle_overrun]Start");

    //Incomplete frame error handling
    if(ioread32((ROT_DMA0_BASE + 0x18)) >> 31)
    {
        LOG_MSG("[handle_overrun]Speed up ROTDMA0!!");
        mt65xx_reg_sync_writel(0 , (ROT_DMA0_BASE + 0x300));
    }
    else if(ioread32((ROT_DMA1_BASE + 0x18)) >> 31)
    {
        LOG_MSG("[handle_overrun]Speed up ROTDMA1!!");
        mt65xx_reg_sync_writel(0 , (ROT_DMA1_BASE + 0x300));
    }
    else if(ioread32((ROT_DMA2_BASE + 0x18)) >> 31)
    {
        LOG_MSG("[handle_overrun]Speed up ROTDMA2!!");
        mt65xx_reg_sync_writel(0 , (ROT_DMA2_BASE + 0x300));
    }
    LOG_MSG("[handle_overrun]speed up ROTDMA done ");
    
    // ensure the view finder is disabe
    ISP_CLR_BIT(ISP_VFCON, 6);
    // do hw reset
    //ISP_WR32(ISP_RESET, 1);
    mt65xx_reg_sync_writel(0x10000,ISP_RESET);
    //ISP_SET_BIT(ISP_RESET, 16); //SW reset
    //ISP_SET_BIT(ISP_RESET, 0); //HW reset
    // delay at least 100us for reset the ISp (HW suggest)
    udelay(120);
    //ISP_WR32(ISP_RESET, 0);
    mt65xx_reg_sync_writel(0,ISP_RESET);
    //ISP_CLR_BIT(ISP_RESET, 16); //SW reset
    //ISP_CLR_BIT(ISP_RESET, 0); //HW reset
    ISP_SET_BIT(ISP_VFCON, 6);

    LOG_MSG("[handle_overrun]End");
    #endif
}

DECLARE_TASKLET(tasklet_handle_overrun, mt_isp_tasklet_handle_overrun, 0);

/*******************************************************************************
*
********************************************************************************/
MBOOL mt_isp_tasklet_wait_done(void)
{
    if(IspDebugInfo.IsTasklet)
    {
        MINT32 timeout;
        //
        if(IspDebugInfo.LogMask & ISP_DBG_TASKLET)
        {
            LOG_MSG("Start");
        }
        //
        timeout = wait_event_interruptible_timeout(
            isp_wait_queue,
            !(IspDebugInfo.IsTasklet),
            ms_to_jiffies(ISP_TASKLET_TIMEOUT));
        //
        if(IspDebugInfo.LogMask & ISP_DBG_TASKLET)
        {
            LOG_MSG("End");
        }
        //
        if(timeout == 0)
        {
            LOG_ERR("Timeout should not happen!");
            return MFALSE;
        }
    }
    else
    {
        if(IspDebugInfo.LogMask & ISP_DBG_TASKLET)
        {
            LOG_MSG("Tasklet is not working");
        }
    }
    return MTRUE;
}

MBOOL mt_isp_tasklet_enable(MBOOL Enable)
{
    if(IspDebugInfo.LogMask & ISP_DBG_TASKLET)
    {
        LOG_MSG("Enable(%d)",Enable);
    }
    //
    if(Enable)
    {
        local_bh_enable();
    }
    else
    {
        local_bh_disable();
        return mt_isp_tasklet_wait_done();
    }
    return MTRUE;
}

/*******************************************************************************
*
********************************************************************************/
extern MINT32 kdSensorSyncFunctionPtr(void);
extern MINT32 kdGetRawGainInfoPtr(MUINT16 *pRAWGain);
extern MINT32 kdSetSensorSyncFlag(MINT32 bSensorSync);

MUINT16 rawGain[4] = {0, 0, 0, 0};

void mt_isp_work_queue(struct work_struct *work)
{
    // Synchronization the exposure time, sensor gain and raw gain.
    IspDebugInfo.IsWorkQueue = MTRUE;
    kdSensorSyncFunctionPtr();
    kdSetSensorSyncFlag(0);
//    kdGetRawGainInfoPtr(&rawGain[0]);
    IspDebugInfo.IsWorkQueue = MFALSE;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_write_reg(mt_isp_reg_io_t *preg_io)
{
    MINT32 ret = 0;
    long ms_tasklet, ms_exp_done;
    //
    if(IspDebugInfo.LogMask & ISP_DBG_WRITE_REG)
    {
        LOG_MSG("data(0x%08X),count(%d)",
                (MUINT32)preg_io->data,
                (MUINT32) preg_io->count);
    }
    //
    if(atomic_read(&hold_reg))
    {
        if(mt_isp_write_buf_add(preg_io->count*sizeof(mt_isp_reg_t), (MUINT8*)(preg_io->data)))
        {
            //LOG_MSG("Add write buffer OK");
        }
        else
        {
            LOG_ERR("Add write buffer fail");
            ms_tasklet = ((long)jiffies-(long)g_jiffies_tasklet) * 1000 / HZ;
            ms_exp_done = ((long)jiffies-(long)g_jiffies_exp_done) * 1000 / HZ;
            LOG_ERR("size too big! hold(%d)",atomic_read(&hold_reg));
            LOG_ERR("exp done before %ld ms, tasklet done before %ld ms",
                    ms_exp_done,
                    ms_tasklet);
            mt_isp_write_buf_dump();
            spin_lock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
            mt_isp_write_buf_reset();
            spin_unlock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
            mt_isp_dump_reg();
            ret = -EFAULT;
            goto EXIT;
        }
    }
    else
    {
        spin_lock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
        ret = mt_isp_write_reg_to_hw((mt_isp_reg_t*)(preg_io->data), preg_io->count);
        spin_unlock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
    }
    //
    EXIT:
    //
    return ret;
}


/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_hold_reg(MUINT32 is_hold)
{
    MINT32 ret = 0, BufAmount;
    //
    if(IspDebugInfo.LogMask & ISP_DBG_TASKLET)
    {
        LOG_MSG("is_hold(%d)",is_hold);
    }
    //
    if(!mt_isp_tasklet_enable(MFALSE))
    {
        LOG_ERR("mt_isp_tasklet_enable(1) fail");
        goto EXIT;
    }
    //  Here we get the lock.
    if(is_hold == 0)
    {
        spin_lock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
        mt_isp_write_buf_data_ready();
        spin_unlock(&IspSpinlock[ISP_SPINLOCK_BUF_WRITE]);
        BufAmount = mt_isp_write_buf_get_amount();
        //
        if(BufAmount)
        {
            atomic_set(&writing_reg, 1);
        }
    }
    //
    if(IspDebugInfo.LogMask & ISP_DBG_HOLD_REG)
    {
        LOG_MSG("is_hold(%d),hold_reg(%d),BufAmount(%d)",
                is_hold,
                atomic_read(&hold_reg),
                BufAmount);
    }
    //
    atomic_set(&hold_reg, is_hold);
    //
    EXIT:
    //
    mt_isp_tasklet_enable(MTRUE);
    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_reset_buf(void)
{
    LOG_MSG("hold_reg(%d),BufAmount(%d)",
            atomic_read(&hold_reg),
            mt_isp_write_buf_get_amount());
    mt_isp_tasklet_enable(MFALSE);
    //
    mt_isp_write_buf_reset();
    atomic_set(&hold_reg, 0);
    atomic_set(&writing_reg, 0);
    //
    mt_isp_tasklet_enable(MTRUE);
    LOG_MSG("End");
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_wait_irq(mt_isp_wait_irq_t* pWaitIrq)
{
    MINT32 ret = 0;
    MINT32 timeout;
    volatile MUINT32 StnrStatus;
    //
    if(IspDebugInfo.LogMask & ISP_DBG_INT)
    {
        LOG_MSG("Start mode(0x%08lX),irq_status(0x%08X),timeout(%ld)",
                pWaitIrq->mode,
                irq_status,
                pWaitIrq->timeout);
    }
    
    if(pWaitIrq->mode & ISP_INT_CLEAR_WAIT)
    {
        pWaitIrq->mode &= ~(ISP_INT_CLEAR_WAIT);
        spin_lock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
        irq_status &= ~(pWaitIrq->mode);
        irq_status_wait_irq &= ~(pWaitIrq->mode);
        spin_unlock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);

        if(pWaitIrq->mode & ISP_INT_STNR)
        {
            ISP_WR32(ISP_INTSTA_STNR,ISP_RD32(ISP_INTSTA_STNR)& (~ISP_INT_STNR_STATUS));
        }
    }

    if(pWaitIrq->mode & ISP_INT_CLEAR_ALL)
    {
        pWaitIrq->mode &= ~(ISP_INT_CLEAR_ALL);
        spin_lock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
        irq_status = 0;
        irq_status_wait_irq = 0;
        spin_unlock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);

        if(pWaitIrq->mode & ISP_INT_STNR)
        {
            ISP_WR32(ISP_INTSTA_STNR,ISP_RD32(ISP_INTSTA_STNR)& (~ISP_INT_STNR_STATUS));
        }
    }

    if(pWaitIrq->mode & ISP_INT_STNR)
    {
        timeout = (pWaitIrq->timeout/ISP_STNR_TIMEOUT_BASE)+ISP_STNR_TIMEOUT_COUNT_MIN;

        do
        {
            msleep(ISP_STNR_TIMEOUT_BASE);
            timeout--;
            StnrStatus = ISP_RD32(ISP_INTSTA_STNR) & ISP_INT_STNR_STATUS;
        }
        while((StnrStatus != ISP_INT_STNR_STATUS) && timeout);

        if( timeout != 0 &&
            StnrStatus == ISP_INT_STNR_STATUS)
        {
            ISP_WR32(ISP_INTSTA_STNR,ISP_RD32(ISP_INTSTA_STNR)& (~ISP_INT_STNR_STATUS));
        }
        //LOG_MSG("[mt_isp_wait_irq]STNR timeout(%d)", timeout*ISP_STNR_TIMEOUT_BASE);
    }
    else
    {
        timeout = wait_event_interruptible_timeout(
                    isp_wait_queue,
                    (pWaitIrq->mode & irq_status_wait_irq),
                    ms_to_jiffies(pWaitIrq->timeout));
    }

    if(timeout == 0)
    {
        LOG_ERR("Timeout mode(0x%08lX),irq_status(0x%08X),irq_status_wait_irq(0x%08X),timeout(%lu)",
                pWaitIrq->mode,
                irq_status,
                irq_status_wait_irq,
                pWaitIrq->timeout);
        mt_isp_dump_reg();
        return -EAGAIN;
    }
    //
    if(IspDebugInfo.LogMask & ISP_DBG_INT)
    {
        LOG_MSG("Timeout mode(0x%08lX),irq_status(0x%08X),irq_status_wait_irq(0x%08X)",
                pWaitIrq->mode,
                irq_status,
                irq_status_wait_irq);
    }
    //
    spin_lock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
    //
    irq_status &= ~(pWaitIrq->mode);
    irq_status_wait_irq &= ~(pWaitIrq->mode);
    //
    spin_unlock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
    //LOG_MSG("[mt_isp_wait_irq] End" );
    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static __inline MBOOL mt_isp_check_exp_done(void)
{
    if(irq_status & ISP_INT_EXP_DONE)
    {
        // Record it for debug
        g_jiffies_exp_done = jiffies;
        //
        if(atomic_read(&writing_reg))
        {
            if(IspDebugInfo.LogMask & ISP_DBG_TASKLET)
            {
                LOG_MSG("tasklet_schedule");
            }
            tasklet_schedule(&tasklet_write_reg);
            irq_status &= ~(ISP_INT_EXP_DONE);
        }
        //LOG_MSG("EXP DONE");
        return MTRUE;
    }
    //LOG_MSG("Not EXP DONE");
    return MFALSE;
}

/*******************************************************************************
*
********************************************************************************/
static irqreturn_t mt_isp_irq(
    MINT32  irq,
    void*   dev_id)
{
    MUINT32 irq_status_reg;

    // Read irq status
    //irq_status |= ISP_RD32(ISP_INTSTA);
    irq_status_reg = ISP_RD32(ISP_INTSTA);
    irq_status |= irq_status_reg;
    //LOG_MSG("[mt_isp_irq] 0x%x ", irq_status_reg);

    //Tg2
    tg2_irq_status |= ISP_RD32(ISP_BASE + 0xD24);

    if(IspDebugInfo.LogMask & ISP_DBG_INT)
    {
        LOG_MSG("0x%X,0x%X,0x%X,0x%X",
                irq_status_reg,
                irq_status,
                irq_status_wait_irq,
                tg2_irq_status);
    }
    //
    if(irq_status & ISP_INT_OVERRUN_MASK)
    {
        LOG_ERR("ISP_INT_OVERRUN_MASK(0x%08X)",irq_status);
        tasklet_schedule(&tasklet_handle_overrun);
        mt_isp_check_exp_done();
        irq_status = 0;
    }
    //
    if(irq_status_wait_irq & ISP_INT_EXP_DONE)
    {
        irq_status_wait_irq = (irq_status|ISP_INT_EXP_DONE);
    }
    else
    {
        irq_status_wait_irq = irq_status;
    }
    wake_up_interruptible(&isp_wait_queue);
    //
    if(irq_status_reg & ISP_INT_VSYNC)
    {
        if(IspDebugInfo.LogMask & ISP_DBG_WORKQUEUE)
        {
            LOG_MSG("schedule_work");
        }
        //
        IspVdCount++;
        //
        if(IspVdProcEn)
        {
            kdGetRawGainInfoPtr(&rawGain[0]);
            if ((rawGain[0] != 0) && (rawGain[1] != 0) && (rawGain[2] != 0) && (rawGain[3] != 0) )            
            {
                ISP_WR32(ISP_RAWGAIN0, ((MUINT32) rawGain[1] << 0) | ((MUINT32) rawGain[0] << 16));
                ISP_WR32(ISP_RAWGAIN1, ((MUINT32) rawGain[2] << 0) | ((MUINT32) rawGain[3] << 16));
            }
            //
            schedule_work(&isp_work_queue);
        }
        //irq_status &= ~(ISP_INT_VSYNC);
    }
    // If ISP exp MINT32 is done, wake up tasklet to write register if it is buffered
    mt_isp_check_exp_done();
    //
    return IRQ_HANDLED;
}

/*******************************************************************************
*
********************************************************************************/
static long mt_isp_ioctl(
    struct file*    file,
    MUINT32         cmd, 
    unsigned long   arg)
{
    MINT32 ret = 0;
    MUINT32 IsHold;
    mt_isp_reg_io_t RegIo;
    mt_isp_wait_irq_t WaitIrq;
    //
    switch(cmd)
    {
        case MT_ISP_IOC_T_RESET:
        {
            IspDebugInfo.CurCmd = ISP_IOC_CMD_RESET;
            mt_isp_reset();
            break;
        }
        case MT_ISP_IOC_G_READ_REG:
        {
            if(copy_from_user(&RegIo, (void *) arg, sizeof(mt_isp_reg_io_t)) != 0)
            {
                LOG_ERR("copy_from_user failed");
                ret = -EFAULT;
                break;
            }
            IspDebugInfo.CurCmd = ISP_IOC_CMD_READ_REG;
            ret = mt_isp_read_reg(&RegIo);
            break;
        }
        case MT_ISP_IOC_S_WRITE_REG:
        {
            if(copy_from_user(&RegIo, (void *) arg, sizeof(mt_isp_reg_io_t)) != 0)
            {
                LOG_ERR("copy_from_user failed");
                ret = -EFAULT;
                break;
            }
            IspDebugInfo.CurCmd = ISP_IOC_CMD_WRITE_REG;
            ret = mt_isp_write_reg(&RegIo);
            break;
        }
        case MT_ISP_IOC_T_HOLD_REG:
        {
            if(copy_from_user(&IsHold, (void *) arg, sizeof(MUINT32)) != 0)
            {
                LOG_ERR("copy_from_user failed");
                ret = -EFAULT;
                break;
            }
            IspDebugInfo.CurCmd = ISP_IOC_CMD_HOLD_REG;
            ret = mt_isp_hold_reg(IsHold);
            break;
        }
        case MT_ISP_IOC_T_WAIT_IRQ:
        {
            if(copy_from_user(&WaitIrq, (void *) arg, sizeof(mt_isp_wait_irq_t)) != 0)
            {
                LOG_ERR("copy_from_user failed");
                ret = -EFAULT;
                break;
            }
            IspDebugInfo.CurCmd = ISP_IOC_CMD_WAIT_IRQ;
            ret = mt_isp_wait_irq(&WaitIrq);
            break;
        }
        case MT_ISP_IOC_T_DUMP_REG:
        {
            IspDebugInfo.CurCmd = ISP_IOC_CMD_DUMP_REG;
            ret = mt_isp_dump_reg();
            break;
        }
        case MT_ISP_IOC_T_DBG_FLAG:
        {
            LOG_MSG("DBG_FLAG(0x%08X)",*(MUINT32*)arg);
            IspDebugInfo.CurCmd = ISP_IOC_CMD_DBG_FLAG;
            spin_lock(&IspSpinlock[ISP_SPINLOCK_IRQ]);
            IspDebugInfo.LogMask = *(MUINT32*)arg;
            spin_unlock(&IspSpinlock[ISP_SPINLOCK_IRQ]);
            break;
        }
        case MT_ISP_IOC_T_RESET_BUF:
        {
            IspDebugInfo.CurCmd = ISP_IOC_CMD_RESET_BUF;
            ret = mt_isp_reset_buf();
            break;
        }
        case MT_ISP_IOC_T_ENABLE_CAM_CLOCK:
        {
            IspDebugInfo.CurCmd = ISP_IOC_CMD_ENABLE_CAM_CLOCK;
            #if ISP_REAL_CHIP
            enable_clock(MT65XX_PDN_MM_CAM, "isp");
            mdelay(100);
            #endif
            LOG_MSG("ENABLE_CAM_CLOCK");
            break;
        }
        case MT_ISP_IOC_T_ENABLE_VD_PROC:
        {
            IspDebugInfo.CurCmd = ISP_IOC_CMD_ENABLE_VD_PROC;
            spin_lock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
            IspVdProcEn = *((MBOOL*)arg);
            spin_unlock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
            LOG_MSG("IspVdProcEn(%d)",IspVdProcEn);
            break;
        }
        case MT_ISP_IOC_T_RESET_VD_COUNT:
        {
            IspDebugInfo.CurCmd = ISP_IOC_CMD_RESET_VD_COUNT;
            spin_lock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
            IspVdCount = 0;
            spin_unlock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
            LOG_MSG("IspVdCount reset");
            break;
        }
        case MT_ISP_IOC_G_GET_VD_COUNT:
        {
            IspDebugInfo.CurCmd = ISP_IOC_CMD_GET_VD_COUNT;
            spin_lock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
            *((MUINT32*)arg) = IspVdCount;
            spin_unlock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
            LOG_MSG("Get IspVdCount(%d)",*((MUINT32*)arg));
            break;
        }
        default:
        {
            LOG_ERR("unknown cmd");
            ret = -EPERM;
            break;
        }
    }
    //
    if(ret < 0)
    {
        LOG_ERR("CurCmd(%d),process(%s),pid(%d),tgid(%d)",
                IspDebugInfo.CurCmd,
                current->comm,
                current->pid,
                current->tgid);
    }
    //
    IspDebugInfo.CurCmd = ISP_IOC_CMD_NONE;
    //
    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_open(
    struct inode*   inode,
    struct file*    file)
{
    MINT32 ret = 0;
    //
    LOG_MSG("current:process(%s),pid(%d),tgid(%d)",
            current->comm,
            current->pid,
            current->tgid);
    //
    spin_lock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
    if(IspOpen)
    {
        spin_unlock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
        LOG_WRN("One more user");
        goto EXIT;
    }
    else
    {
        IspOpen = MTRUE;
    }
    spin_unlock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
    //
    if(!mt_isp_write_buf_alloc())
    {
        LOG_ERR("Allocate write buf fail");
        ret = -ENOMEM;
        goto EXIT;
    }
    //
    atomic_set(&hold_reg, 0);
    atomic_set(&writing_reg, 0);
    irq_status = 0;
    IspVdCount = 0;
    IspVdProcEn = MFALSE;
    // Enable clock
    mt_isp_clk_ctrl(1);
    //
    EXIT:
    if(ret < 0)
    {
        mt_isp_write_buf_free();
    }
    //
    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_release(
    struct inode*   inode,
    struct file*    file)
{
    MUINT32 TempData;
    //
    LOG_MSG("current:process(%s),pid(%d),tgid(%d)",
            current->comm,
            current->pid,
            current->tgid);
    //
    if(!IspOpen)
    {
        LOG_WRN("No user before");
        goto EXIT;
    }
    //Check flicker
    TempData = ISP_RD32(ISP_FLK_CON);
    if(TempData & 0x08000000)
    {
        //Force to disable flciker.
        LOG_WRN("Flicker:disable,ISP_FLK_CON(0x%08X)",TempData);
        ISP_CLR_BIT(ISP_FLK_CON,0);
        mt_isp_reset();
        TempData = ISP_RD32(ISP_FLK_CON);
        if(TempData & 0x08000000)
        {
            LOG_ERR("Flicker:Still running,ISP_FLK_CON(0x%08X)",TempData);
        }
        else
        {
            LOG_MSG("Flicker:done,ISP_FLK_CON(0x%08X)",TempData);
        }
    }
    else
    {
        //Force to disable view finder.
        ISP_CLR_BIT(ISP_VFCON,6);
    }
    mb();
    //Force to disable shading.
    ISP_CLR_BIT(ISP_SHADING1,28);
    ISP_CLR_BIT(ISP_SHADING1,29);
    //Force to disable and clear INT.
    ISP_WR32(ISP_INTEN, 0);
    mb();
    TempData = ISP_RD32(ISP_INTSTA);
    mb();
    //
    mt_isp_clk_ctrl(0);
    //
    mt_isp_write_buf_free();
    //
    spin_lock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
    IspOpen = MFALSE;
    spin_unlock_irq(&IspSpinlock[ISP_SPINLOCK_IRQ]);
    //
    EXIT:
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_mmap(
    struct file*            file,
    struct vm_area_struct*  vma)
{
    //LOG_MSG("");

    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
        vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
        return -EAGAIN;
    }

    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static dev_t dev_isp;
static struct cdev *pcdev_isp = NULL;
static struct class *pclass_isp = NULL;
static const struct file_operations mt_isp_fops = {
    .owner   = THIS_MODULE,
    .open    = mt_isp_open,
    .release = mt_isp_release,
    //.flush   = mt_isp_flush,
    .mmap    = mt_isp_mmap,
    .unlocked_ioctl = mt_isp_ioctl
};
static MINT32 isp_irq_num;

/*******************************************************************************
*
********************************************************************************/
inline static void mt_isp_unregister_char_driver(void)
{
    LOG_MSG("");
    //Release char driver
    if(pcdev_isp != NULL)
    {
        cdev_del(pcdev_isp);
        pcdev_isp = NULL;
    }
    //
    unregister_chrdev_region(dev_isp, 1);
}

/*******************************************************************************
*
********************************************************************************/
inline static MINT32 mt_isp_register_char_driver(void)
{
    MINT32 ret = 0;
    //
    LOG_MSG("");
    //
    if((ret = alloc_chrdev_region(&dev_isp, 0, 1, ISP_DEV_NAME)) < 0)
    {
        LOG_ERR("alloc_chrdev_region failed,ret(%d)", ret);
        return ret;
    }
    //Allocate driver
    pcdev_isp = cdev_alloc();
    if(pcdev_isp == NULL)
    {
        LOG_ERR("cdev_alloc failed");
        ret = -ENOMEM;
        goto EXIT;
    }
    //Attatch file operation.
    cdev_init(pcdev_isp, &mt_isp_fops);
    //
    pcdev_isp->owner = THIS_MODULE;
    //Add to system
    if((ret = cdev_add(pcdev_isp, dev_isp, 1)) < 0)
    {
        LOG_ERR("Attatch file operation failed,ret(%d)", ret);
        goto EXIT;
    }
    //
    EXIT:
    //
    if(ret < 0)
    {
        mt_isp_unregister_char_driver();
    }
    //
    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_probe(struct platform_device *pdev)
{
    MINT32 ret = 0;
    struct resource *pres = NULL;
    MINT32 i;
    //
    LOG_MSG("E");
    // Check platform_device parameters
    if(pdev == NULL)
    {
        dev_err(&pdev->dev, "pdev is NULL");
        return -ENXIO;
    }
    // Register char driver
    if((ret = mt_isp_register_char_driver()))
    {
        dev_err(&pdev->dev, "register char failed");
        return ret;
    }
    // Mapping CAM_REGISTERS
    for(i = 0; i < 2; i++)
    {
        pres = platform_get_resource(pdev, IORESOURCE_MEM, i);
        if(pres == NULL)
        {
            dev_err(&pdev->dev, "platform_get_resource failed");
            ret = -ENOMEM;
            goto EXIT;
        }
        pres = request_mem_region(pres->start, pres->end - pres->start + 1, pdev->name);
        if(pres == NULL)
        {
            dev_err(&pdev->dev, "request_mem_region failed");
            ret = -ENOMEM;
            goto EXIT;
        }
    }
    // Request CAM_ISP IRQ
    if(request_irq(MT6577_CAM_IRQ_LINE, mt_isp_irq, IRQF_TRIGGER_LOW, "isp", NULL))
    {
        LOG_ERR("MT6577_CAM_IRQ_LINE IRQ LINE NOT AVAILABLE!");
        goto EXIT;
    }
    //mt65xx_irq_unmask(MT6577_CAM_IRQ_LINE);
    //enable_irq(MT6577_CAM_IRQ_LINE); 
    // Create class register
    pclass_isp = class_create(THIS_MODULE, "ispdrv");
    if(IS_ERR(pclass_isp))
    {
        ret = PTR_ERR(pclass_isp);
        LOG_ERR("Unable to create class, err(%d)", ret);
        return ret;
    }
    //
    mt_isp_write_buf_init();
    //
    IspOpen = MFALSE;
    IspDebugInfo.CurCmd = ISP_IOC_CMD_NONE;
    IspDebugInfo.IsWorkQueue = MFALSE;
    IspDebugInfo.IsTasklet = MFALSE;
    // FIXME: error handling
    device_create(pclass_isp, NULL, dev_isp, NULL, ISP_DEV_NAME);
    // Initialize critical section
    for(i=0;i<ISP_SPINLOCK_AMOUNT;i++)
    {
        spin_lock_init(&IspSpinlock[i]);
    }
    //
    init_waitqueue_head(&isp_wait_queue);
    //
    INIT_WORK(&isp_work_queue, mt_isp_work_queue);
    //
    EXIT:
    //
    if(ret < 0)
    {
        mt_isp_unregister_char_driver();
    }
    //
    LOG_MSG("X");
    //
    return ret;
}

/*******************************************************************************
* Called when the device is being detached from the driver
********************************************************************************/
static MINT32 mt_isp_remove(struct platform_device *pdev)
{
    struct resource *pres;
    MINT32 i;
    MINT32 irq_num;
    //
    LOG_MSG("E");
    // unregister char driver.
    mt_isp_unregister_char_driver();
    // unmaping ISP CAM_REGISTER registers
    for(i = 0; i < 2; i++)
    {
        pres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        release_mem_region(pres->start, (pres->end - pres->start + 1));
    }
    // Release IRQ
    disable_irq(isp_irq_num);
    irq_num = platform_get_irq(pdev, 0);
    free_irq(irq_num , NULL);
    //
    device_destroy(pclass_isp, dev_isp);
    //
    class_destroy(pclass_isp);
    pclass_isp = NULL;
    //
    LOG_MSG("X");
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_suspend(
    struct platform_device* pdev,
    pm_message_t            mesg)
{
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 mt_isp_resume(struct platform_device *pdev)
{
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static struct platform_driver isp_driver = {
    .probe   = mt_isp_probe,
    .remove  = mt_isp_remove,
    .suspend = mt_isp_suspend,
    .resume  = mt_isp_resume,
    .driver  = {
        .name  =  ISP_DEV_NAME,
        .owner = THIS_MODULE,
    }
};

/*******************************************************************************
*
********************************************************************************/
//JHYu for HQA
static MINT32
mt_camiodrv_dump_reg_to_proc(
    char*       page,
    char**      start,
    off_t       off,
    MINT32      count,
    MINT32*     eof,
    void*       data)
{
    char *p = page;
    MINT32 len = 0;
    MUINT32 u4Index = 0 ;
    //
    LOG_MSG("");
    //
    p += sprintf(p, "\n\r MT6577 CAMIODRV Register \n\r");
    p += sprintf(p,"======General Setting ====\n\r");
    p += sprintf(p,"+0x%0x 0x%0x\n\r", u4Index, ISP_RD32(CAMIODRV_BASE + 0x824 ));
    *start = page + off;
    len = p - page;
    if(len > off)
    {
        len -= off;
    }
    else
    {
        len = 0;
    }
    //
    return len < count ? len : count;
}

/*******************************************************************************
*
********************************************************************************/
//JHYu for HQA
static MINT32  mt_camiodrv_reg_debug(
    struct file*    file,
    const char*     buffer,
    unsigned long   count,
    void*           data)
{
    char regBuf[64];
    MUINT32 u4CopyBufSize = (count < (sizeof(regBuf) - 1)) ? (count) : (sizeof(regBuf) - 1);
    MUINT32 u4Addr = 0;
    MUINT32 u4Data = 0;
    //
    if(copy_from_user(regBuf, buffer, u4CopyBufSize))
    {
        return -EFAULT;
    }
    //
    if(sscanf(regBuf, "%x %x",  &u4Addr, &u4Data) == 2)
    {
        iowrite32(u4Data, CAMIODRV_BASE + u4Addr);
        LOG_DBG("Write:addr(0x%08X),data(0x%08X)",
                u4Addr,
                ioread32(CAMIODRV_BASE + u4Addr));
    }
    else
    if (sscanf(regBuf, "%x", &u4Addr) == 1)
    {
        LOG_DBG("Read:addr(0x%08X),data(0x%08X)",
                u4Addr,
                ioread32(CAMIODRV_BASE + u4Addr));
    }
    //
    return count;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32
mt_isp_dump_reg_to_proc(
    char*       page,
    char**      start,
    off_t       off,
    MINT32      count,
    MINT32*     eof,
    void*       data)
{
    char *p = page;
    MINT32 len = 0;
    MUINT32 u4Index = 0 ;
    //
    LOG_MSG("");
    //
    p += sprintf(p, "\n\r MT6577 ISP Register \n\r");
    p += sprintf(p,"======General Setting ====\n\r");
    for(; u4Index < 0x3C; u4Index += 4)
    {
        p += sprintf(p,"+0x%0x 0x%0x\n\r", u4Index, ISP_RD32(ISP_BASE + u4Index));
    }
    p += sprintf(p,"======Result Win Setting ====\n\r");
    for(u4Index = 0x174; u4Index < 0x190; u4Index += 4)
    {
        p += sprintf(p,"+0x%0x 0x%0x\n\r", u4Index, ISP_RD32(ISP_BASE + u4Index));
    }
    //
    for(u4Index = 0x1D4; u4Index <= 0x1D8; u4Index +=4)
    {
        p += sprintf(p, "+0x%0x 0x%0x\n\r", u4Index, ISP_RD32(ISP_BASE + u4Index));
    }
    //
    *start = page + off;
    //
    len = p - page;
    //
    if(len > off)
    {
        len -= off;
    }
    else
    {
        len = 0;
    }
    //
    return len < count ? len : count;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32  mt_isp_reg_debug(
    struct file*    file,
    const char*     buffer,
    unsigned long   count,
    void*           data)
{
    char regBuf[64] = {'\0'}; 
    MUINT32 u4CopyBufSize = (count < (sizeof(regBuf) - 1)) ? (count) : (sizeof(regBuf) - 1);
    MUINT32 u4Addr = 0;
    MUINT32 u4Data = 0;
    //
    if(copy_from_user(regBuf, buffer, u4CopyBufSize))
    {
        return -EFAULT;
    }
    //
    if(sscanf(regBuf, "%x %x",  &u4Addr, &u4Data) == 2)
    {
        iowrite32(u4Data, ISP_BASE + u4Addr);
        LOG_DBG("Write:addr(0x%08X),data(0x%08X)",
                u4Addr,
                ioread32(ISP_BASE + u4Addr));
    }
    else
    if(sscanf(regBuf, "%x", &u4Addr) == 1)
    {
        LOG_DBG("Read:addr(0x%08X),data(0x%08X)",
                u4Addr,
                ioread32(ISP_BASE + u4Addr));
    }
    return count;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32
mt_isp_read_flag_by_proc(
    char*       page,
    char**      start,
    off_t       off,
    MINT32      count,
    MINT32*     eof,
    void*       data)
{
    char *p = page;
    MINT32 len = 0;
    //
    p += sprintf(p, "[isp debug]\r\n");
    p += sprintf(p, "IspDebugInfo:LogMask(0x%08X),CurCmd(%d),IsWorkQueue(%d),IsTasklet(%d)\r\n",
                    IspDebugInfo.LogMask,
                    IspDebugInfo.CurCmd,
                    IspDebugInfo.IsWorkQueue,
                    IspDebugInfo.IsTasklet);
    p += sprintf(p, "Read:addr(0x%08X),val(0x%08X)\r\n",
                    g_u4DbgRegOfs,
                    ISP_RD32(ISP_BASE + g_u4DbgRegOfs));
    *start = page + off;
    len = p - page;
    if (len > off)
    {
        len -= off;
    }
    else
    {
        len = 0;
    }
    return len < count ? len  : count;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32
mt_isp_write_flag_by_proc( 
    struct file*    file,
    const char*     buffer,
    unsigned long   count,
    void*           data)
{
    char acBuf[32]; 
    MUINT32 u4CopySize = 0;
    MUINT32 u4DbgMask = 0;
    MUINT32 u4DbgRegOfs = 0;
    //
    u4CopySize = (count < (sizeof(acBuf) - 1)) ? count : (sizeof(acBuf) - 1);
    if(copy_from_user(acBuf, buffer, u4CopySize))
    {
        return 0;
    }
    acBuf[u4CopySize] = '\0';
    if(2 == sscanf(acBuf, "%x%x", &u4DbgMask, &u4DbgRegOfs))
    {
        IspDebugInfo.LogMask = u4DbgMask;
        g_u4DbgRegOfs = u4DbgRegOfs;
    }
    else
    if(1 == sscanf(acBuf, "%x", &u4DbgMask))
    {
        IspDebugInfo.LogMask = u4DbgMask;
    }
    return count;
}

/*******************************************************************************
*
********************************************************************************/
static MINT32 __init mt_isp_init(void)
{
    MINT32 ret = 0;
    struct proc_dir_entry *prEntry;

    LOG_MSG("");
    //
    if((ret = platform_driver_register(&isp_driver)) < 0)
    {
        LOG_ERR("platform_driver_register fail");
        return ret;
    }
    //
    prEntry = create_proc_entry("driver/isp_reg", 0, NULL);
    if(prEntry)
    {
        prEntry->read_proc = mt_isp_dump_reg_to_proc;
        prEntry->write_proc = mt_isp_reg_debug;
    }
    else
    {
        LOG_ERR("add /proc/driver/isp_reg entry fail");
    }
    //
    prEntry = create_proc_entry("driver/isp_flag", 0, NULL); 
    if(prEntry)
    {
        prEntry->read_proc = mt_isp_read_flag_by_proc;
        prEntry->write_proc = mt_isp_write_flag_by_proc;
    }
    else
    {
        LOG_ERR("add /proc/driver/isp_flag entry fail");
    }
    //
    prEntry = create_proc_entry("driver/camio_reg", 0, NULL);//JH for HQA
    if(prEntry)
    { //JH for HQA
        prEntry->read_proc = mt_camiodrv_dump_reg_to_proc;
        prEntry->write_proc = mt_camiodrv_reg_debug;    	
    }
    else
    {
        LOG_ERR("add /proc/driver/camio_reg entry fail");
    }
    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static void __exit mt_isp_exit(void)
{
    LOG_MSG("");
    platform_driver_unregister(&isp_driver);
}
/*******************************************************************************
*
********************************************************************************/
void mt_isp_mclk_ctrl(MINT32 en)
{
    if(en)
    {
        ISP_SET_BIT(ISP_PHSCNT, 29);
    }
    else
    {
        ISP_CLR_BIT(ISP_PHSCNT, 29);
    }
}
/*******************************************************************************
*
********************************************************************************/
EXPORT_SYMBOL(mt_isp_mclk_ctrl);
module_init(mt_isp_init);
module_exit(mt_isp_exit);
MODULE_DESCRIPTION("Camera ISP driver");
MODULE_AUTHOR("ME3");
MODULE_LICENSE("GPL");

