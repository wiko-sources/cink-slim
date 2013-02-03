#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/proc_fs.h>  //proc file use
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/delay.h>

//Arch dependent files
//#include <asm/tcm.h>
#include <mach/mt6577_reg_base.h>
#include <mach/camera_sysram.h>
#include <mach/camera_sysram_layout.h>
#include <mach/mt6577_clock_manager.h>
#include <mach/sync_write.h>

//--------------------------------------------Define-----------------------------------------------//
#define LOG_MSG(fmt, arg...)    printk(KERN_DEBUG "[SYSRAM]" fmt, ##arg)
#define LOG_WRN(fmt, arg...)    printk(KERN_DEBUG "[SYSRAM]Warning(%5d):" fmt,__LINE__, ##arg)
#define LOG_ERR(fmt, arg...)    printk(KERN_ERR   "[SYSRAM]Err(%5d):" fmt,__LINE__, ##arg)

/* Debug message*/
#define DBG_SYSRAM_NONE                 0x00000000
#define DBG_SYSRAM_OVER_PREDEFINE_SIZE  0x00000001
#define DBG_SYSRAM_ALL                  0xFFFFFFFF
unsigned int g_u4SysramDbgFlag = DBG_SYSRAM_ALL;
unsigned int g_u4EnableUserGroupSize = 1;
//MT6573: 0, we just show warning messages instead of returning failure.
//MT6577: 1, we do not allow anyone use un-predicted size.
unsigned int g_u4EnableOverSizeFail = 1;

#define DBG_SYSRAM_MASK     (g_u4SysramDbgFlag)
#define SYSRAM_SWITCH_BANK  (0)

#if (defined(CONFIG_MT6575T_FPGA)||defined(CONFIG_MT6577_FPGA))
#define SYSRAM_REAL_CHIP    (0)
#else
#define SYSRAM_REAL_CHIP    (1)
#endif
//-------------------------------------Global variables-------------------------
typedef struct
{
    pid_t           Pid;
    unsigned long   Table;
}SYSRAM_PRIVATE_DATA_STRUCT;

SYSRAM_PRIVATE_DATA_STRUCT SysramPrivateData;
//--------------------------------------Functions-------------------------------
static void MTxxx_SYSRAM_DUMPLAYOUT(void);

/*******************************************************************************
* Resource Manager
********************************************************************************/
static void Dump_ResMgr(void);
static inline int IsBadOwner(ESysramUser_T const eOwner);
static void SetOwnerTaskInfo(ESysramUser_T const eOwner);
static void ResetOwnerTaskInfo(ESysramUser_T const eOwner);
static inline void Lock_ResMgr(void);
static inline void Unlock_ResMgr(void);
static inline int IsLockedOwner_ResMgr(ESysramUser_T const eOwner);
static inline int IsUnlockedOwner_ResMgr(ESysramUser_T const eOwner);
static void LockOwner_ResMgr(ESysramUser_T const eOwner, unsigned long const u4Size);
static void UnlockOwner_ResMgr(ESysramUser_T const eOwner);

typedef struct TaskInfo
{
    pid_t   pid;                    // thread id
    pid_t   tgid;                   // process id
    char    comm[TASK_COMM_LEN];    // executable name
    //  Time stamp when alloc.
    unsigned long       t_jiffies;  // jiffies
    unsigned long       t_sec;      // nanosec_rem = do_div((t_jiffies-INITIAL_JIFFIES) * (USEC_PER_SEC / HZ), USEC_PER_SEC);
    unsigned long       t_usec;     // nanosec_rem / 1000)
} TaskInfo_T;

typedef struct
{
    spinlock_t          lock;
    unsigned long       u4TotalUserCount;
    unsigned long       u4AllocatedTbl;
    unsigned long       au4AllocatedSize[ESysramUser_CNT];
    TaskInfo_T          aOwnerTaskInfo[ESysramUser_CNT];
    P_MaxUsrSizeTbl_T   pMaxUsrSizeTbl;
    wait_queue_head_t   wq_head;
    bool                EnableClk;
    #if SYSRAM_SWITCH_BANK
    bool                EnableSwitchBank;
    #endif
} ResMgr_T;

static ResMgr_T g_ResMgr =
{
    .u4TotalUserCount   =   0, 
    .u4AllocatedTbl     =   0, 
    .au4AllocatedSize   =   {0}, 
    .pMaxUsrSizeTbl     =   &g_au4MaxUserSize,
    .EnableClk          =   false,
    #if SYSRAM_SWITCH_BANK
    .EnableSwitchBank   =   true
    #endif
};
//------------------------------------------------------------------------------
#if SYSRAM_SWITCH_BANK
//
#define SYSRAM_SWITCH_BANK_WAIT_COUNT       100
#define SYSRAM_SWITCH_BANK_WAIT_COUNT_ERR   200
//SM
//Reg Addr
#define SMI_REG_ADDR_SWITCH_BANK    (SMI_BASE+0x30)
//Reg
#define SMI_REG_SWITCH_BANK_SEL0    (0x01)
#define SMI_REG_SWITCH_BANK_SEL1    (0x02)
#define SMI_REG_SWITCH_BANK_SEL2    (0x04)
//MGF
//Reg Addr
#define MFG_REG_ADDR_WAY_EN         (MFG_APB_BASE+0x8)
//Reg
#define MFG_REG_WAY_EN_WAY_EN_0     (0x000000FF)
#define MFG_REG_WAY_EN_WAY_EN_1     (0x0000FF00)
#define MFG_REG_WAY_EN_WAY_EN_2     (0x00FF0000)
#define MFG_REG_WAY_EN_WAY_EN_3     (0xFF000000)
//------------------------------------------------------------------------------
#define SYSRAM_BANK2_BANK_AMOUNT    2
#define SYSRAM_BANK2_BANK_SIZE      (32*1024)
#define SYSRAM_BANK2_BANK0_OFFSET   SYSRAM_BANK2_BANK_SIZE
#define SYSRAM_BANK2_BANK1_OFFSET   (SYSRAM_BANK2_BANK0_OFFSET+SYSRAM_BANK2_BANK_SIZE)
#define SYSRAM_BANK2_BANK2_OFFSET   (SYSRAM_BANK2_BANK1_OFFSET+SYSRAM_BANK2_BANK_SIZE)
//------------------------------------------------------------------------------
static bool SysramSwitchBankMfg = true;
static bool SysramSwitchBankUser = true;
//------------------------------------------------------------------------------
static void ClearGdc(unsigned long WayEn)
{
    if(DRV_Reg32(MFG_APB_BASE) == 0x1008)
    {
        if (DRV_Reg32(MFG_REG_ADDR_WAY_EN) & WayEn)
        {
            mt65xx_reg_sync_writel(0x1, MFG_APB_BASE+0x04);//Set GDC clear
            mt65xx_reg_sync_writel(0x0, MFG_APB_BASE+0x04);//Set GDC clear
        }
    }
}
//------------------------------------------------------------------------------

static bool CheckRange(
    unsigned long CheckAddr,
    unsigned long CheckSize,
    unsigned long RangeAddr,
    unsigned long RangeSize)
{
    //LOG_MSG("[CheckRange]CheckAddr = 0x%08X, StartAddr = 0x%08X, EndAddr = 0x%08X\n",CheckAddr,StartAddr,StartAddr+Size-1);

    if( (CheckAddr > (RangeAddr+RangeSize-1)) ||
        ((CheckAddr+CheckSize-1) < RangeAddr))
    {
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------
static void CheckSwitchBank(ESysramUser_T const eOwner)
{
    bool StaticBankAlloc = false, ShowLog, Bank2Use[SYSRAM_BANK2_BANK_AMOUNT] = {false,false};
    unsigned long i, WaitCount;
    unsigned long WayEnMask[SYSRAM_BANK2_BANK_AMOUNT] = {MFG_REG_WAY_EN_WAY_EN_1,MFG_REG_WAY_EN_WAY_EN_2};
    unsigned long Bank2Addr[SYSRAM_BANK2_BANK_AMOUNT] = {SYSRAM_BANK2_BANK0_OFFSET,SYSRAM_BANK2_BANK1_OFFSET};
    MemNode_T* pCurrNode = NULL;

    //LOG_MSG("[CheckSwitchBank]Start\n");

    if(!g_ResMgr.EnableSwitchBank)
    {
        return;
    }

    if((1<<eOwner) & ELogOwnersMask)
    {
        ShowLog = true;
    }
    else
    {
        ShowLog = false;
    }

    for(i=0; i<ESysramUser_CNT; i++)
    {
        if(g_au4StaticUserAddr[i] == 0)
        {
            break;
        }
        else
        if(g_ResMgr.u4AllocatedTbl & (1<<i))
        {
            StaticBankAlloc = true;
            break;
        }
    }

    if(StaticBankAlloc)
    {
        if(DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK) & SMI_REG_SWITCH_BANK_SEL2)
        {
            //LOG_MSG("[CheckSwitchBank]SMI_SWITCH_BANK 1= 0x%08X\n",DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK));
            mt65xx_reg_sync_writel(
                DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK)&(~SMI_REG_SWITCH_BANK_SEL2),
                SMI_REG_ADDR_SWITCH_BANK);
            //LOG_MSG("[CheckSwitchBank]SMI_SWITCH_BANK 2= 0x%08X\n",DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK));
            if(subsys_is_on(MT65XX_SUBSYS_MM2))
            {
                WaitCount = 0;
                while(DRV_Reg32(MFG_REG_ADDR_WAY_EN) & MFG_REG_WAY_EN_WAY_EN_3)
                {
                    WaitCount++;
                    if(WaitCount >= SYSRAM_SWITCH_BANK_WAIT_COUNT)
                    {
                        ClearGdc(MFG_REG_WAY_EN_WAY_EN_3);
                        if(WaitCount >= SYSRAM_SWITCH_BANK_WAIT_COUNT_ERR)
                        {
                            LOG_WRN("[CheckSwitchBank]eOwner = %d, Wait 3, MFG_WAY_EN = 0x%08X, WaitCount = %ld\n",eOwner,DRV_Reg32(MFG_REG_ADDR_WAY_EN),WaitCount);
                        }
                    }
                    //msleep(1);
                }
            }
            if(ShowLog)
            {
                LOG_MSG("[CheckSwitchBank]MFG to SYSRAM, SWITCH_BANK = 0x%08X\n",DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK));
            }
        }
    }
    else
    {
        if(!(DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK) & SMI_REG_SWITCH_BANK_SEL2))
        {
            mt65xx_reg_sync_writel(
                DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK)|SMI_REG_SWITCH_BANK_SEL2,
                SMI_REG_ADDR_SWITCH_BANK);
            if(ShowLog)
            {
                LOG_MSG("[CheckSwitchBank]SYSRAM to MFG, SWITCH_BANK = 0x%08X\n",DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK));
            }
        }
    }

    pCurrNode = &g_aMemPoolInfo[MEM_POOL_BANK2].paNodes[0];
    while( NULL != pCurrNode )
    {
        if( IsBadOwner(eOwner))
        {
            break;
        }
        else
        if((pCurrNode->eOwner) == ESysramUser_None)
        {
            pCurrNode = pCurrNode->pNext;
            continue;
        }
        else
        {
            if(ShowLog)
            {
                //LOG_MSG("[CheckSwitchBank]eOwner = %d, StartAddr = 0x%08lX, EndAddr = 0x%08lX\n",pCurrNode->eOwner,pCurrNode->u4Offset,pCurrNode->u4Offset+pCurrNode->u4Length-1);
            }

            for(i=0; i<SYSRAM_BANK2_BANK_AMOUNT; i++)
            {
                if(!Bank2Use[i])
                {
                    Bank2Use[i] = CheckRange(
                                pCurrNode->u4Offset,
                                pCurrNode->u4Length,
                                Bank2Addr[i],
                                SYSRAM_BANK2_BANK_SIZE);
                }
            }
        }
        pCurrNode = pCurrNode->pNext;
    };

    for(i=0; i<SYSRAM_BANK2_BANK_AMOUNT; i++)
    {
        if(ShowLog)
        {
            //LOG_MSG("[CheckSwitchBank]Bank2Use[%lu] = %d\n",i,Bank2Use[i]);
        }

        if(Bank2Use[i])
        {
            if(DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK) & (1<<i))
            {
                //LOG_MSG("[CheckSwitchBank]SMI_SWITCH_BANK 1= 0x%08X\n",DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK));
                mt65xx_reg_sync_writel(
                    DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK)&(~(1<<i)),
                    SMI_REG_ADDR_SWITCH_BANK);
                //LOG_MSG("[CheckSwitchBank]SMI_SWITCH_BANK 2= 0x%08X\n",DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK));
                if(subsys_is_on(MT65XX_SUBSYS_MM2))
                {
                    WaitCount = 0;
                    while(DRV_Reg32(MFG_REG_ADDR_WAY_EN) & WayEnMask[i])
                    {
                        WaitCount++;
                        if(WaitCount >= SYSRAM_SWITCH_BANK_WAIT_COUNT)
                        {
                            ClearGdc(WayEnMask[i]);
                            if(WaitCount >= SYSRAM_SWITCH_BANK_WAIT_COUNT_ERR)
                            {
                                LOG_WRN("[CheckSwitchBank]eOwner = %d, Wait 3, MFG_WAY_EN = 0x%08X, WaitCount = %ld\n",eOwner,DRV_Reg32(MFG_REG_ADDR_WAY_EN),WaitCount);
                            }
                        }
                        //msleep(1);
                    }
                }
                if(ShowLog)
                {
                    LOG_MSG("[CheckSwitchBank]MFG to SYSRAM, i = %lu, SWITCH_BANK = 0x%08X\n",i,DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK));
                }
            }
        }
        else
        {
            if(!(DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK) & (1<<i)))
            {
                mt65xx_reg_sync_writel(
                    DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK)|(1<<i),
                    SMI_REG_ADDR_SWITCH_BANK);
                if(ShowLog)
                {
                    LOG_MSG("[CheckSwitchBank]SYSRAM to MFG, i = %lu, SWITCH_BANK = 0x%08X\n",i,DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK));
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
static void EanbleSwitchBank(bool Enable)
{
    unsigned long WaitCount = 0;
    //LOG_MSG("[EanbleSwitchBank]Enable= %d\n",Enable);
    if(g_ResMgr.EnableSwitchBank != Enable)
    {
        g_ResMgr.EnableSwitchBank = Enable;
        //
        if(Enable)
        {
            CheckSwitchBank(0);
            LOG_MSG("[EanbleSwitchBank]SYSRAM to MFG, SWITCH_BANK = 0x%08X, u4AllocatedTbl = 0x%08lX\n",DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK),g_ResMgr.u4AllocatedTbl);
        }
        else
        {
            mt65xx_reg_sync_writel(
                DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK)&(~(SMI_REG_SWITCH_BANK_SEL0|SMI_REG_SWITCH_BANK_SEL1|SMI_REG_SWITCH_BANK_SEL2)),
                SMI_REG_ADDR_SWITCH_BANK);
            //
            if(subsys_is_on(MT65XX_SUBSYS_MM2))
            {
                while(DRV_Reg32(MFG_REG_ADDR_WAY_EN) & (MFG_REG_WAY_EN_WAY_EN_1|MFG_REG_WAY_EN_WAY_EN_2|MFG_REG_WAY_EN_WAY_EN_3))
                {
                    WaitCount++;
                    if(WaitCount >= SYSRAM_SWITCH_BANK_WAIT_COUNT)
                    {
                        ClearGdc(MFG_REG_WAY_EN_WAY_EN_1|MFG_REG_WAY_EN_WAY_EN_2|MFG_REG_WAY_EN_WAY_EN_3);
                        if(WaitCount >= SYSRAM_SWITCH_BANK_WAIT_COUNT_ERR)
                        {
                            LOG_WRN("[EanbleSwitchBank]Wait MFG_WAY_EN = 0x%08X, WaitCount = %ld\n",DRV_Reg32(MFG_REG_ADDR_WAY_EN),WaitCount);
                        }
                    }
                    //msleep(1);
                }
            }
            LOG_MSG("[EanbleSwitchBank]MFG to SYSRAM, SWITCH_BANK = 0x%08X, u4AllocatedTbl = 0x%08lX\n",DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK),g_ResMgr.u4AllocatedTbl);
        }
    }
}
//------------------------------------------------------------------------------
static void SYSRAM_USER_SWITCH_BANK(bool Enable)
{
    Lock_ResMgr();
    if(Enable)
    {
        if(SysramSwitchBankMfg)
        {
            EanbleSwitchBank(true);
        }
        else
        {
            EanbleSwitchBank(false);
        }
    }
    else
    {
        EanbleSwitchBank(false);
    }
    SysramSwitchBankUser = Enable;
    Unlock_ResMgr();
}
#endif
//------------------------------------------------------------------------------
void SYSRAM_MFG_SWITCH_BANK(bool Enable)
{
    #if SYSRAM_SWITCH_BANK
    Lock_ResMgr();
    if(Enable)
    {
        if(SysramSwitchBankUser)
        {
            EanbleSwitchBank(true);
        }
        else
        {
            EanbleSwitchBank(false);
        }
    }
    else
    {
        EanbleSwitchBank(false);
    }
    SysramSwitchBankMfg = Enable;
    Unlock_ResMgr();
    #endif
}
//------------------------------------------------------------------------------
static void CheckClock(void)
{
    //LOG_MSG("[CheckClock]u4AllocatedTbl = 0x%08X, EnableClk = %d\n",g_ResMgr.u4AllocatedTbl,g_ResMgr.EnableClk);
    if(g_ResMgr.u4AllocatedTbl)
    {
        if(!(g_ResMgr.EnableClk))
        {
            g_ResMgr.EnableClk = true;
            LOG_MSG("[CheckClock]u4AllocatedTbl = 0x%08lX, EnableClk = %d\n",g_ResMgr.u4AllocatedTbl,g_ResMgr.EnableClk);
            //
            #if SYSRAM_REAL_CHIP
            hwEnableClock(MT65XX_PDN_MM_GDC_SHARE_MACRO, "SYSRAM");
            hwEnableClock(MT65XX_PDN_MM_GDC_SHARE_MACRO_HALF, "SYSRAM");
            #endif
        }
    }
    else
    {
        if(g_ResMgr.EnableClk)
        {
            g_ResMgr.EnableClk = false;
            LOG_MSG("[CheckClock]u4AllocatedTbl = 0x%08lX, EnableClk = %d\n",g_ResMgr.u4AllocatedTbl,g_ResMgr.EnableClk);
            //
            #if SYSRAM_REAL_CHIP
            hwDisableClock(MT65XX_PDN_MM_GDC_SHARE_MACRO_HALF, "SYSRAM");
            hwDisableClock(MT65XX_PDN_MM_GDC_SHARE_MACRO, "SYSRAM");
            #endif
        }
    }
}
//------------------------------------------------------------------------------
static void Dump_ResMgr(void)
{
    unsigned int u4Idx = 0;
    LOG_MSG("[Dump_ResMgr](u4TotalUserCount, u4AllocatedTbl)=(%ld, 0x%lX)\n",g_ResMgr.u4TotalUserCount, g_ResMgr.u4AllocatedTbl);

    for (u4Idx = 0; u4Idx < ESysramUser_CNT; u4Idx++)
    {
        if  ( 0 < g_ResMgr.au4AllocatedSize[u4Idx] )
        {
            TaskInfo_T*const pTaskInfo = &g_ResMgr.aOwnerTaskInfo[u4Idx];
            LOG_MSG(
                "[id:%u][%s][size:0x%lX][pid:%d][tgid:%d][%s][%5lu.%06lu][%08lX]\n"
                , u4Idx, g_apszOwnerName[u4Idx], g_ResMgr.au4AllocatedSize[u4Idx]
                , pTaskInfo->pid, pTaskInfo->tgid, pTaskInfo->comm
                , pTaskInfo->t_sec, pTaskInfo->t_usec, pTaskInfo->t_jiffies
            );
        }
    }
    LOG_MSG("\n");
}

static inline int IsBadOwner(ESysramUser_T const eOwner)
{
    return  ( ESysramUser_CNT <= eOwner || eOwner < 0 );
}

static unsigned long GetJiffies(void)
{
    unsigned long CurJiffies = 0;
    //
    if(time_after(jiffies,(unsigned long)0xFFFFFFFF))
    {
        CurJiffies = jiffies+(0xFFFFFFFF-INITIAL_JIFFIES);
    }
    else
    {
        CurJiffies = jiffies-INITIAL_JIFFIES;
    }
    //LOG_MSG("CurJiffies(%08lX)\n",CurJiffies);
    return CurJiffies;
}

static void SetOwnerTaskInfo(ESysramUser_T const eOwner)
{
    if  ( ! IsBadOwner(eOwner) )
    {
        TaskInfo_T*const pInfo = &g_ResMgr.aOwnerTaskInfo[eOwner];
        pInfo->pid = current->pid;
        pInfo->tgid = current->tgid;
        memcpy(pInfo->comm, current->comm, sizeof(pInfo->comm));
        pInfo->t_jiffies= GetJiffies();
        pInfo->t_sec    = pInfo->t_jiffies * (USEC_PER_SEC / HZ);
        pInfo->t_usec   = do_div(pInfo->t_sec , USEC_PER_SEC) / 1000;   //  pInfo->t_sec is changed after do_div().
    }
}

static void ResetOwnerTaskInfo(ESysramUser_T const eOwner)
{
    if  ( ! IsBadOwner(eOwner) )
    {
        TaskInfo_T*const pInfo = &g_ResMgr.aOwnerTaskInfo[eOwner];
        memset(pInfo, 0, sizeof(*pInfo));
    }
}

static inline void Lock_ResMgr(void)
{
    spin_lock(&g_ResMgr.lock);
}

static inline void Unlock_ResMgr(void)
{
    spin_unlock(&g_ResMgr.lock);
}

static inline int IsLockedOwner_ResMgr(ESysramUser_T const eOwner)
{
    return  ((1 << eOwner) & g_ResMgr.u4AllocatedTbl);
}

static inline int IsUnlockedOwner_ResMgr(ESysramUser_T const eOwner)
{
    return  0 == IsLockedOwner_ResMgr(eOwner);
}

static void LockOwner_ResMgr(ESysramUser_T const eOwner, unsigned long const u4Size)
{
    if  ( IsLockedOwner_ResMgr(eOwner) )
        return;

    g_ResMgr.u4TotalUserCount++;
    g_ResMgr.u4AllocatedTbl |= (1 << eOwner);
    g_ResMgr.au4AllocatedSize[eOwner] = u4Size;
    SetOwnerTaskInfo(eOwner);

    //  Debug Log.
    if  ( (1<<eOwner) & ELogOwnersMask )
    {
        TaskInfo_T*const pTaskInfo = &g_ResMgr.aOwnerTaskInfo[eOwner];
        LOG_MSG(
            "[LockOwner][%s][%lu bytes] succeed [Lock Time:%5lu.%06lu][%08lX]\n"
            , g_apszOwnerName[eOwner], g_ResMgr.au4AllocatedSize[eOwner]
            , pTaskInfo->t_sec, pTaskInfo->t_usec, pTaskInfo->t_jiffies
        );
    }
}

static void UnlockOwner_ResMgr(ESysramUser_T const eOwner)
{
    unsigned long t_jiffies = 0;
    //
    if  ( IsUnlockedOwner_ResMgr(eOwner) )
        return;

    //  Debug Log.
    if  ( (1<<eOwner) & ELogOwnersMask )
    {
        TaskInfo_T*const pTaskInfo = &g_ResMgr.aOwnerTaskInfo[eOwner];
        //
        t_jiffies = GetJiffies();
        //
        if((t_jiffies-pTaskInfo->t_jiffies) < 0)
        {
            LOG_WRN("t_jiffies(%08lX) is smaller than pTaskInfo->t_jiffies((%08lX))\n",
                    t_jiffies,
                    pTaskInfo->t_jiffies);
        }
        //
        LOG_MSG(
            "[UnlockOwner][%s][%lu bytes]...[Lock Time:%5lu.%06lu][Lock Period:%lu usec][%08lX]\n"
            , g_apszOwnerName[eOwner], g_ResMgr.au4AllocatedSize[eOwner]
            , pTaskInfo->t_sec, pTaskInfo->t_usec, ((t_jiffies - pTaskInfo->t_jiffies)*(USEC_PER_SEC/HZ))
            , t_jiffies
        );
    }

    if  ( g_ResMgr.u4TotalUserCount > 0 )
        g_ResMgr.u4TotalUserCount--;
    g_ResMgr.u4AllocatedTbl &= (~(1 << eOwner));
    g_ResMgr.au4AllocatedSize[eOwner] = 0;
    ResetOwnerTaskInfo(eOwner);
}


/*******************************************************************************
* Memory Node Manager (instead of kmalloc/kfree)
********************************************************************************/
static MemNode_T* AllocNode(MemPoolInfo_T*const pMemPoolInfo)
{
    MemNode_T* pNode = NULL;
    unsigned long u4Index = 0;
    for(u4Index = 0; u4Index < pMemPoolInfo->u4OwnerCnt; u4Index+=1)
    {
        if  ((pMemPoolInfo->u4IndexTbl) & (1 << u4Index))
        {
            pMemPoolInfo->u4IndexTbl &= (~(1 << u4Index));
            //  A free node is found.
            pNode = &pMemPoolInfo->paNodes[u4Index];
            pNode->eOwner   = ESysramUser_None;
            pNode->u4Offset = 0;
            pNode->u4Length = 0;
            pNode->pPrev    = NULL;
            pNode->pNext    = NULL;
            pNode->u4Index  = u4Index;
            break;
        }
    }
    //  Shouldn't happen.
    if  ( ! pNode )
    {
        LOG_ERR("[AllocNode]returns NULL - pMemPoolInfo->u4IndexTbl(%lX)\n", pMemPoolInfo->u4IndexTbl);
    }
    return  pNode;
}

static void FreeNode(MemPoolInfo_T*const pMemPoolInfo, MemNode_T*const pNode)
{
    pMemPoolInfo->u4IndexTbl |= (1<<pNode->u4Index);
    pNode->eOwner   = ESysramUser_None;
    pNode->u4Offset = 0;
    pNode->u4Length = 0;
    pNode->pPrev    = NULL;
    pNode->pNext    = NULL;
    pNode->u4Index  = 0;
}

/*******************************************************************************
*
*
*
********************************************************************************/
static int IsLegalSizeToAlloc(MEM_POOL_BANK_NO_T const eMemPoolBankNo, ESysramUser_T const eOwner, unsigned long const u4Size)
{
    unsigned long u4MaxSize = 0;

    //  (1) Check the memory pool.
    switch  (eMemPoolBankNo)
    {
    case MEM_POOL_BAD:
    case MEM_POOL_CNT:
        //  Illegal Memory Pool: return "illegal"
        //  Shouldn't happen.
        goto lbExit;
    case MEM_POOL_STATIC:   //  Static
        //  Static Memory Pool: compare with the max. user size.
        u4MaxSize = g_au4MaxUserSize[eOwner];
        goto lbExit;
    default:                //  Dynamic
        break;
    }

    //  (2)
    //  Here we use the dynamic memory pools.
    u4MaxSize = (*g_ResMgr.pMaxUsrSizeTbl)[eOwner];

    //  (3) User Group.
    if  ( 0 != g_u4EnableUserGroupSize )
    {
        int bIsInGroup = 1;
        unsigned long u4GroupSize = 0;
        unsigned long u4MaxGroupSize = 0;
        switch (eOwner)
        {
            //  (3.1) User Group: RGB_ROT0~2
            case ESysramUser_RGB_ROT0:
            case ESysramUser_RGB_ROT1:
            case ESysramUser_RGB_ROT2:
            {
                u4GroupSize     =   g_ResMgr.au4AllocatedSize[ESysramUser_RGB_ROT0]+
                                    g_ResMgr.au4AllocatedSize[ESysramUser_RGB_ROT1]+
                                    g_ResMgr.au4AllocatedSize[ESysramUser_RGB_ROT2];
                u4MaxGroupSize  =   SYSRAM_SIZE_RGB_GROUP;
                break;
            }
            default:
            {
                bIsInGroup = 0; //  Not in group.
                break;
            }
        }

        if  ( bIsInGroup != 0 )  //  In group ?
        {   
            if  ( u4Size + u4GroupSize > u4MaxGroupSize )
            {
                int i;
                for(i=ESysramUser_RGB_ROT0; i<=ESysramUser_RGB_ROT2; i++)
                {
                    LOG_ERR("[IsLegalSizeToAlloc]Owner(%d)(%s),size(%ld)\n",i,g_apszOwnerName[i],g_ResMgr.au4AllocatedSize[i]);
                }
                LOG_ERR(
                    "[IsLegalSizeToAlloc][Owner:%d in group]size to alloc(%ld) + group size(%ld) > max group size(%ld)\n"
                    , eOwner, u4Size, u4GroupSize, u4MaxGroupSize);
                return  0;
            }
            u4MaxSize = (u4MaxGroupSize - u4GroupSize);
        }
    }

lbExit:
    if  (u4MaxSize < u4Size)
    {
        if  ( DBG_SYSRAM_OVER_PREDEFINE_SIZE & DBG_SYSRAM_MASK )
        {
            LOG_ERR(
                "[IsLegalSizeToAlloc][owner: %s]requested size(0x%lX) > max size(0x%lX)\n"
                ,g_apszOwnerName[eOwner], u4Size, u4MaxSize
            );
            MTxxx_SYSRAM_DUMPLAYOUT();
        }
        return   0; 
    }
    return  1;
}


/*******************************************************************************
*Alignment should be 2^N, 4/8/2048 bytes alignment only
*First fit algorithm
********************************************************************************/
static unsigned long Allocate(
    ESysramUser_T const eOwner, 
    unsigned long const u4Size, 
    unsigned long const u4Alignment, 
    MEM_POOL_BANK_NO_T const eMemPoolBankNo
)
{
    MemNode_T* pSplitNode = NULL;
    MemNode_T* pCurrNode = NULL;
    unsigned long u4AlingnedAddr = 0;
    unsigned long u4ActualSize = 0;

    MemPoolInfo_T*const pMemPoolInfo = GetMemPoolInfo(eMemPoolBankNo);
    if  ( ! pMemPoolInfo )
    {
        return  0;
    }

    pCurrNode = &pMemPoolInfo->paNodes[0];
    for (; pCurrNode && pCurrNode->u4Offset < pMemPoolInfo->u4Size; pCurrNode = pCurrNode->pNext)
    {
        if  (ESysramUser_None == pCurrNode->eOwner)
        {
            //Free space
            u4AlingnedAddr = (pCurrNode->u4Offset + u4Alignment - 1)&(~(u4Alignment - 1));
            u4ActualSize = u4Size + u4AlingnedAddr - pCurrNode->u4Offset;
            if  (u4ActualSize <= pCurrNode->u4Length)
            {
                // Hit!! Split into 2
                // pSplitNode pointers to the next available (free) node.
                pSplitNode = AllocNode(pMemPoolInfo);
                pSplitNode->u4Offset = pCurrNode->u4Offset + u4ActualSize;
                pSplitNode->u4Length = pCurrNode->u4Length - u4ActualSize;
                pSplitNode->pPrev    = pCurrNode;
                pSplitNode->pNext    = pCurrNode->pNext;
                //
                pCurrNode->eOwner = eOwner;
                pCurrNode->u4Length = u4ActualSize;
                pCurrNode->pNext = pSplitNode;
                //
                if  (NULL != pSplitNode->pNext)
                {
                    pSplitNode->pNext->pPrev = pSplitNode;
                }
                //
                pMemPoolInfo->u4UserCount++;
                break;
            }
            //Not hit
            u4ActualSize = 0;
        }
    };

    return u4ActualSize ? (u4AlingnedAddr + pMemPoolInfo->u4StartAddr) : 0;
}

/*******************************************************************************
*
*Return:
*   Nonzero indicates eOwner node is found & then removed.
*   Zero indicate eOwner node can not be found.
*
********************************************************************************/
static int Free(ESysramUser_T const eOwner, MEM_POOL_BANK_NO_T const eMemPoolBankNo)
{
    int bRet = 0;
    MemNode_T* pPrevOrNextNode = NULL;
    MemNode_T* pCurrNode = NULL;
    MemNode_T* pTempNode = NULL;

    MemPoolInfo_T*const pMemPoolInfo = GetMemPoolInfo(eMemPoolBankNo);
    if  ( ! pMemPoolInfo )
    {
        LOG_ERR("[Free]pMemPoolInfo==NULL (eOwner, eMemPoolBankNo)=(%d, %d)\n", eOwner, eMemPoolBankNo);
        return  0;
    }

    pCurrNode = &pMemPoolInfo->paNodes[0];
    for (; pCurrNode; pCurrNode = pCurrNode->pNext)
    {
        if  (eOwner == pCurrNode->eOwner)
        {
            bRet = 1;   //  owner is found.
            if  ( pMemPoolInfo->u4UserCount > 0 )
                pMemPoolInfo->u4UserCount--;

            pCurrNode->eOwner = ESysramUser_None;
            if  (NULL != pCurrNode->pPrev)
            {
                pPrevOrNextNode = pCurrNode->pPrev;
#if 0
                if  ( pPrevOrNextNode->pNext != pCurrNode )
                {
                    LOG_ERR("[Free]pPrevOrNextNode->pNext != pCurrNode (pNext, pNext->u4Index, pCurrNode, pCurrNode->u4Index)=(%p, %ld, %p, %ld)\n", pPrevOrNextNode->pNext, pPrevOrNextNode->pNext->u4Index, pCurrNode, pCurrNode->u4Index);
                }
#endif
                if  (ESysramUser_None == pPrevOrNextNode->eOwner)
                {
                    //Merge previous: prev(o) + curr(x)
                    pTempNode = pCurrNode;
                    pCurrNode = pPrevOrNextNode;
                    pCurrNode->u4Length += pTempNode->u4Length;
                    pCurrNode->pNext = pTempNode->pNext;
                    if  (NULL != pTempNode->pNext)
                    {
                        pTempNode->pNext->pPrev = pCurrNode;
                    }
                    FreeNode(pMemPoolInfo, pTempNode);
                }
            }

            if  (NULL != pCurrNode->pNext)
            {
                pPrevOrNextNode = pCurrNode->pNext;
#if 0
                if  ( pPrevOrNextNode->pPrev != pCurrNode )
                {
                    LOG_ERR("[Free]pPrevOrNextNode->pPrev != pCurrNode (pPrev, pPrev->u4Index, pCurrNode, pCurrNode->u4Index)=(%p, %ld, %p, %ld)\n", pPrevOrNextNode->pPrev, pPrevOrNextNode->pPrev->u4Index, pCurrNode, pCurrNode->u4Index);
                }
#endif
                if  (ESysramUser_None == pPrevOrNextNode->eOwner)
                {
                    //Merge next: curr(o) + next(x)
                    pTempNode = pPrevOrNextNode;
                    pCurrNode->u4Length += pTempNode->u4Length;
                    pCurrNode->pNext = pTempNode->pNext;
                    if  (NULL != pCurrNode->pNext)
                    {
                        pCurrNode->pNext->pPrev = pCurrNode;
                    }
                    FreeNode(pMemPoolInfo, pTempNode);
                }
            }
            break;
        }
    }
    return  bRet;
}

/*******************************************************************************
*
********************************************************************************/
static unsigned long AllocOwner(ESysramUser_T const eOwner, unsigned long u4Size, unsigned long const u4Alignment)
{
    unsigned long u4Addr = 0;
    MEM_POOL_BANK_NO_T const eMemPoolBankNo = GetMemPoolNo(eOwner);

    if  ( IsBadOwner(eOwner) )
    {
        LOG_ERR("[AllocOwner]eOwner(%d) out of range [0, %d) \n", eOwner, ESysramUser_CNT);
        return  0;
    }

    if  ( ! IsLegalSizeToAlloc(eMemPoolBankNo, eOwner, u4Size) )
    {
        if(g_u4EnableOverSizeFail)
        {
            if(eOwner != ESysramUser_BRZ)
            {
                return 0;
            }
        }
    }

    //The following code are only for MT6577.
    //  Re-evaluate the size to alloc due to dedicated-bank user or something else.
    u4Size = ReevaluateUserSizeToAlloc(eOwner, u4Size);

    switch  (eMemPoolBankNo)
    {
    case MEM_POOL_BAD:
    case MEM_POOL_CNT:
        //  Do nothing.
        break;
    case MEM_POOL_STATIC:   //  Static
        u4Addr = g_au4StaticUserAddr[eOwner];
        if  ( 0 == u4Addr )
        {   //  Shoundn't happen.
            LOG_ERR("[AllocOwner]g_au4StaticUserAddr[%d] == 0\n", eOwner);
        }
        if  ( g_au4MaxUserSize[eOwner] < u4Size )
        {
            LOG_ERR("[AllocOwner][Static User: %s]forbid allocating!!\n", g_apszOwnerName[eOwner]);
            u4Addr = 0;
        }
        break;
    default:                //  Dynamic
        u4Addr = Allocate(eOwner , u4Size, u4Alignment , eMemPoolBankNo);
        break;
    }

    if  ( 0 < u4Addr )
    {
        LockOwner_ResMgr(eOwner, u4Size);
    }
    return u4Addr;
}

static void FreeOwner(ESysramUser_T const eOwner)
{
    MEM_POOL_BANK_NO_T const eMemPoolBankNo = GetMemPoolNo(eOwner);

    switch  (eMemPoolBankNo)
    {
    case MEM_POOL_BAD:
    case MEM_POOL_CNT:
        //  Do nothing.
        break;
    case MEM_POOL_STATIC:   //  Static
        UnlockOwner_ResMgr(eOwner);
        break;
    default:                //  Dynamic
        if  ( Free(eOwner, eMemPoolBankNo) )
        {
            UnlockOwner_ResMgr(eOwner);
        }
        else
        {
            LOG_ERR("[FreeOwner]Cannot free eOwner(%d)\n", eOwner);
            MTxxx_SYSRAM_DUMPLAYOUT();
        }
        break;
    }
}

/*******************************************************************************
*
********************************************************************************/
static unsigned long MsToJiffies(unsigned long u4ms)
{
    return ((u4ms*HZ + 512) >> 10);
}

static unsigned long TryAllocOwner(ESysramUser_T const eOwner, unsigned long const u4Size, unsigned long const u4Alignment)
{
    unsigned long u4Addr = 0;

    Lock_ResMgr();

    if  ( IsLockedOwner_ResMgr(eOwner) )
    {
        Unlock_ResMgr();
        LOG_ERR("[TryAllocOwner][User: %s]has been already allocated!!\n" , g_apszOwnerName[eOwner]);
        return 0;
    }

    u4Addr = AllocOwner(eOwner, u4Size, u4Alignment);
    if  (0 == u4Addr)
    {
#if 0
        LOG_ERR("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        LOG_ERR("[SYSRAM][User: %s] fails to allocate %lu BYTES with %lu alignment\n", g_apszOwnerName[eOwner], u4Size, u4Alignment);
        MTxxx_SYSRAM_DUMPLAYOUT();
#endif
    }
    else
    {
        CheckClock();

        #if SYSRAM_SWITCH_BANK
        CheckSwitchBank(eOwner);
        #endif
    }

    Unlock_ResMgr();

    return  u4Addr;
}

static unsigned long MTxxx_SYSRAM_ALLOC_TIMEOUT(ESysramUser_T const eOwner, unsigned long const u4Size, unsigned long u4Alignment, unsigned long const u4TimeoutInMS)
{
    unsigned long u4Addr = 0;
    long iTimeOut = 0;

    if  ( IsBadOwner(eOwner) )
    {
        LOG_ERR("[ALLOC_TIMEOUT]eOwner(%d) out of range [0, %d) \n", eOwner, ESysramUser_CNT);
        return  0;
    }

    if  (0 == u4Size)
    {
        LOG_ERR("[ALLOC_TIMEOUT][User: %s]allocates 0 size!!\n", g_apszOwnerName[eOwner]);
        return  0;
    }

    u4Addr = TryAllocOwner(eOwner, u4Size, u4Alignment);
    if  (
            0 != u4Addr         //success
        ||  0 == u4TimeoutInMS  //failure without a timeout specified
        )
    {
        goto lbExit;
    }

    iTimeOut = wait_event_interruptible_timeout(
        g_ResMgr.wq_head, 
        0 != ( u4Addr = TryAllocOwner(eOwner, u4Size, u4Alignment) ), 
        MsToJiffies(u4TimeoutInMS)
    );
    if  (0 == iTimeOut && 0 == u4Addr )
    {
        LOG_ERR("[ALLOC_TIMEOUT][User: %s]allocate timeout\n", g_apszOwnerName[eOwner]);
    }

lbExit:
    if  (0 == u4Addr)
    {   //  Failure
        LOG_ERR(
            "[ALLOC_TIMEOUT][User: %s]fails to allocate (Size in Bytes, Alignment, u4TimeoutInMS)=(%lu, %lu, %lu)\n"
            , g_apszOwnerName[eOwner], u4Size, u4Alignment, u4TimeoutInMS
        );
        MTxxx_SYSRAM_DUMPLAYOUT();
    }
    else
    {   //  Success
#if 1
        if  ( (1<<eOwner) & ELogOwnersMask )
        {
            LOG_MSG("[ALLOC_TIMEOUT][User: %s]alloc %lu bytes successfully\n", g_apszOwnerName[eOwner], u4Size);
        }
#endif
    }

    return u4Addr;
}

static unsigned long MTxxx_SYSRAM_ALLOC(ESysramUser_T eOwner, unsigned long u4Size, unsigned long u4Alignment)
{
    return  MTxxx_SYSRAM_ALLOC_TIMEOUT(eOwner, u4Size, u4Alignment, 0);
}

static void MTxxx_SYSRAM_FREE(ESysramUser_T eOwner)
{
    if  ( IsBadOwner(eOwner) )
    {
        LOG_ERR("[FREE]eOwner(%d) out of range [0, %d) \n", eOwner, ESysramUser_CNT);
        return;
    }

    Lock_ResMgr();
    FreeOwner(eOwner);

    wake_up_interruptible(&g_ResMgr.wq_head);

    #if SYSRAM_SWITCH_BANK
    CheckSwitchBank(eOwner);
    #endif

    CheckClock();

    Unlock_ResMgr();

    if  ( (1<<eOwner) & ELogOwnersMask )
    {
        LOG_MSG("[Free][User: %s]try to free\n", g_apszOwnerName[eOwner]);
    }
}

/*******************************************************************************
*
********************************************************************************/
static int SYSRAM_Open(struct inode * a_pstInode, struct file * a_pstFile)
{
    int i4RetVal = 0;
    SYSRAM_PRIVATE_DATA_STRUCT* pPrivateData;

    Lock_ResMgr();

    //LOG_MSG("[Open](process, pid, tgid)=(%s, %d, %d)\n", current->comm , current->pid, current->tgid);
    //
    a_pstFile->private_data = kmalloc(sizeof(SYSRAM_PRIVATE_DATA_STRUCT) , GFP_ATOMIC);
    if(a_pstFile->private_data != NULL)
    {
        pPrivateData = (SYSRAM_PRIVATE_DATA_STRUCT*)(a_pstFile->private_data);
        pPrivateData->Pid = current->pid;
        pPrivateData->Table = 0;
    }
    else
    {
        LOG_ERR("[Open]No enough memory, (process, pid, tgid)=(%s, %d, %d)\r\n", current->comm, current->pid, current->tgid);
        i4RetVal = -ENOMEM;
        goto lbExit;
    }

lbExit:
    Unlock_ResMgr();
    return i4RetVal;
}

static int SYSRAM_Release(struct inode * a_pstInode, struct file * a_pstFile)
{
    unsigned int u4Index = 0;
    SYSRAM_PRIVATE_DATA_STRUCT* pPrivateData;
    //
    Lock_ResMgr();
    //
    //LOG_MSG("[Release](process, pid, tgid)=(%s, %d, %d)\n", current->comm , current->pid, current->tgid);
    //
    if(a_pstFile->private_data != NULL)
    {
        pPrivateData = (SYSRAM_PRIVATE_DATA_STRUCT*)(a_pstFile->private_data);
        //LOG_MSG("[Release](Pid, Table) = (%d, 0x%08lX)\n",pPrivateData->Pid,pPrivateData->Table);
        //
        if(pPrivateData->Table)
        {
            LOG_WRN("[Release]Process(%s), (Pid, Table) = (%d, 0x%08lX) are not freed before.\n",current->comm,pPrivateData->Pid,pPrivateData->Table);
            MTxxx_SYSRAM_DUMPLAYOUT();
        
            for(u4Index = 0 ; u4Index < ESysramUser_CNT; u4Index++)
            {
                if(pPrivateData->Table & (1 << u4Index))
                {
                    Unlock_ResMgr();
                    MTxxx_SYSRAM_FREE((ESysramUser_T)u4Index);
                    Lock_ResMgr();
                }
            }
        }
        //
        kfree(a_pstFile->private_data);
        a_pstFile->private_data = NULL;
    }
    Unlock_ResMgr();
    return 0;
}

static int SYSRAM_Flush(struct file * a_pstFile , fl_owner_t a_id)
{
    unsigned int u4Index = 0;
    SYSRAM_PRIVATE_DATA_STRUCT* pPrivateData;

    Lock_ResMgr();  //+

    //LOG_MSG("[Flush](process, pid, tgid)=(%s, %d, %d)\n", current->comm , current->pid, current->tgid);

    if(a_pstFile->private_data != NULL)
    {
        pPrivateData = (SYSRAM_PRIVATE_DATA_STRUCT *)a_pstFile->private_data;
        //LOG_MSG("[Flush](Pid, Table) = (%d, 0x%08lX)\n",pPrivateData->Pid,pPrivateData->Table);
        if( (pPrivateData->Pid == current->pid) ||
            ((pPrivateData->Pid != current->pid) && (strcmp(current->comm, "binder") == 0)))
        {
            if(pPrivateData->Table)
            {
                LOG_WRN("[Flush]Process(%s), (Pid, Table) = (%d, 0x%08lX) are not freed before.\n",current->comm,pPrivateData->Pid,pPrivateData->Table);
                MTxxx_SYSRAM_DUMPLAYOUT();
                //
                for(u4Index = 0 ; u4Index < ESysramUser_CNT; u4Index++)
                {
                    if(pPrivateData->Table & (1 << u4Index))
                    {
                        Unlock_ResMgr();//-
                        MTxxx_SYSRAM_FREE((ESysramUser_T)u4Index);
                        Lock_ResMgr();  //+
                    }
                }            
                //
                pPrivateData->Table= 0;
            }
        }
        else
        {
            //LOG_WRN("[Flush]This process did not open SYSRAM, Pid(%d), (process, pid)=(%s, %d)\n",pPrivateData->Pid,current->comm,current->pid);
        }
    }
    //
    Unlock_ResMgr();//-

    return 0;
}

static int SYSRAM_mmap(struct file *file, struct vm_area_struct *vma)
{
    //LOG_MSG("[mmap]\n");
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    if  (   remap_pfn_range(
            vma, vma->vm_start, vma->vm_pgoff, 
            vma->vm_end - vma->vm_start, vma->vm_page_prot
            )
        )
    {
        LOG_ERR("[mmap]fail\n");
        return -EAGAIN;
    }

    return 0;
}

static long SYSRAM_Ioctl(struct file* a_pstFile, unsigned int a_u4Command, unsigned long a_u4Param)
{
    int i4RetVal = 0;
    SYSRAM_PRIVATE_DATA_STRUCT* pPrivateData;
    stSysramParam stParam;
    bool EnableSwicthBank;

    if  ( _IOC_NONE != _IOC_DIR(a_u4Command) )
    {
        if(a_u4Command == SYSRAM_S_SWITCH_BANK)
        {
            if  ( copy_from_user(&EnableSwicthBank, (void *)a_u4Param, sizeof(bool)) )
            {
                LOG_ERR("[Ioctl]copy from user failed\n");
                i4RetVal = -EFAULT;
                return  i4RetVal;
            }
        }
        else
        {
            if  ( copy_from_user(&stParam, (void *)a_u4Param, sizeof(stSysramParam)) )
            {
                LOG_ERR("[Ioctl]copy from user failed\n");
                i4RetVal = -EFAULT;
                return  i4RetVal;
            }
        }
    }

    if(a_pstFile->private_data == NULL)
    {
        LOG_WRN("[Ioctl]private_data is NULL,(process, pid, tgid)=(%s, %d, %d)\n", current->comm , current->pid, current->tgid);
        i4RetVal = -1;
        goto lbExit;
    }

    Lock_ResMgr();  //+
    pPrivateData = (SYSRAM_PRIVATE_DATA_STRUCT *)a_pstFile->private_data;
    Unlock_ResMgr();//-

    switch(a_u4Command)
    {
    case SYSRAM_X_USRALLOC_TIMEOUT:
        stParam.u4Addr = MTxxx_SYSRAM_ALLOC_TIMEOUT(stParam.u4Owner , stParam.u4Size, stParam.u4Alignment, stParam.u4TimeoutInMS);
        if  (0 != stParam.u4Addr)
        {
            Lock_ResMgr();  //+
            pPrivateData->Table |= (1 << stParam.u4Owner);
            Unlock_ResMgr();//-
        }
        else
        {   //  Fail to alloc since address == 0.
            i4RetVal = -EFAULT;
        }
        break;
    case SYSRAM_X_USRALLOC:
        stParam.u4Addr = MTxxx_SYSRAM_ALLOC(stParam.u4Owner , stParam.u4Size, stParam.u4Alignment);
        if  (0 != stParam.u4Addr)
        {
            Lock_ResMgr();  //+
            pPrivateData->Table |= (1 << stParam.u4Owner);
            Unlock_ResMgr();//-
        }
        else
        {   //  Fail to alloc since address == 0.
            i4RetVal = -EFAULT;
        }
        break;
    case SYSRAM_S_USRFREE:
        Lock_ResMgr();  //+
        if  ((~pPrivateData->Table) & (1 << stParam.u4Owner))
        {
            LOG_WRN("[Ioctl]Freeing unallocated buffer, owner = %d, (Pid, Table) = (%d, 0x%08lX)\n", stParam.u4Owner,pPrivateData->Pid,pPrivateData->Table);
            i4RetVal = -1;
            Unlock_ResMgr();//-
            goto lbExit;
        }
        pPrivateData->Table &= (~(1 << stParam.u4Owner));
        Unlock_ResMgr();//-
        MTxxx_SYSRAM_FREE(stParam.u4Owner);
        break;

    case SYSRAM_T_DUMPLAYOUT:
        MTxxx_SYSRAM_DUMPLAYOUT();
        break;

    case SYSRAM_S_SWITCH_BANK:
        #if SYSRAM_SWITCH_BANK
        SYSRAM_USER_SWITCH_BANK(EnableSwicthBank);
        #endif
        break;


    default:
        LOG_WRN("[Ioctl]No such command %d, (Pid, Table) = (%d, 0x%08lX)\n",a_u4Command,pPrivateData->Pid,pPrivateData->Table);
        i4RetVal = -EINVAL;
        goto lbExit;
        break;
    }

    if(i4RetVal != 0)
    {
        LOG_ERR("[Ioctl]Fail, (Pid, Table) = (%d, 0x%08lX), (process, pid, tgid)=(%s, %d, %d)\r\n" , pPrivateData->Pid,pPrivateData->Table,current->comm , current->pid, current->tgid);
    }

lbExit:

    if  ( _IOC_READ & _IOC_DIR(a_u4Command) )
    {
        if  ( copy_to_user((void __user *)a_u4Param, &stParam, sizeof(stSysramParam)) )
        {
            LOG_ERR("[Ioctl]copy to user failed\n");
            i4RetVal = -EFAULT;
        }
    }

    return i4RetVal;
}

static const struct file_operations g_stSYSRAM_fops = 
{
    .owner = THIS_MODULE,
    .open  = SYSRAM_Open,
    .release = SYSRAM_Release,
    .flush = SYSRAM_Flush,
    .unlocked_ioctl = SYSRAM_Ioctl,
    .mmap  = SYSRAM_mmap,
};

/*******************************************************************************
*
********************************************************************************/
static struct cdev * g_pSYSRAM_CharDrv = NULL;
static dev_t g_SYSRAMdevno = 0;
static inline int RegisterSYSRAMCharDrv(void)
{
    if( alloc_chrdev_region(&g_SYSRAMdevno, 0, 1,SYSRAM_DEV_NAME) )
    {
        LOG_ERR("[RegisterSYSRAMCharDrv]Allocate device no failed\n");

        return -EAGAIN;
    }

    //Allocate driver
    g_pSYSRAM_CharDrv = cdev_alloc();

    if(NULL == g_pSYSRAM_CharDrv)
    {
        unregister_chrdev_region(g_SYSRAMdevno, 1);

        LOG_ERR("[RegisterSYSRAMCharDrv]Allocate mem for kobject failed\n");

        return -ENOMEM;
    }

    //Attatch file operation.
    cdev_init(g_pSYSRAM_CharDrv, &g_stSYSRAM_fops);

    g_pSYSRAM_CharDrv->owner = THIS_MODULE;

    //Add to system
    if(cdev_add(g_pSYSRAM_CharDrv, g_SYSRAMdevno, 1))
    {
        LOG_ERR("[RegisterSYSRAMCharDrv]Attatch file operation failed\n");

        unregister_chrdev_region(g_SYSRAMdevno, 1);

        return -EAGAIN;
    }

    return 0;
}

static inline void UnregisterSYSRAMCharDrv(void)
{
    //Release char driver
    cdev_del(g_pSYSRAM_CharDrv);

    unregister_chrdev_region(g_SYSRAMdevno, 1);
}

static struct class *pSYSRAM_CLASS = NULL;
// Called to probe if the device really exists. and create the semaphores
static int SYSRAM_probe(struct platform_device *pdev)
{
    unsigned long u4PoolIdx = 0;
    struct device* sysram_device = NULL;

    LOG_MSG("[probe]Start\n");

    //register char driver
    //Allocate major no
    if(RegisterSYSRAMCharDrv())
    {
        LOG_ERR("[probe]register char failed\n");
        return -EAGAIN;
    }

    pSYSRAM_CLASS = class_create(THIS_MODULE, "sysramdrv");
    if (IS_ERR(pSYSRAM_CLASS)) {
        int ret = PTR_ERR(pSYSRAM_CLASS);
        LOG_ERR("[probe]Unable to create class, err = %d\n", ret);
        return ret;            
    }
    sysram_device = device_create(pSYSRAM_CLASS, NULL, g_SYSRAMdevno, NULL, SYSRAM_DEV_NAME);

    //Initialize variables
    spin_lock_init(&g_ResMgr.lock);
    g_ResMgr.u4TotalUserCount   =   0;
    g_ResMgr.u4AllocatedTbl     =   0;
    g_ResMgr.pMaxUsrSizeTbl     =   &g_au4MaxUserSize;
    memset(g_ResMgr.au4AllocatedSize, 0, sizeof(g_ResMgr.au4AllocatedSize));
    memset(g_ResMgr.aOwnerTaskInfo, 0, sizeof(g_ResMgr.aOwnerTaskInfo));
    init_waitqueue_head(&g_ResMgr.wq_head);
    g_ResMgr.EnableClk = false;
    //
    for (u4PoolIdx = 0; u4PoolIdx < MEM_POOL_CNT; u4PoolIdx++)
    {
        g_aMemPoolInfo[u4PoolIdx].paNodes[0].eOwner   = ESysramUser_None;
        g_aMemPoolInfo[u4PoolIdx].paNodes[0].u4Offset = 0;
        g_aMemPoolInfo[u4PoolIdx].paNodes[0].u4Length = g_aMemPoolInfo[u4PoolIdx].u4Size;
        g_aMemPoolInfo[u4PoolIdx].paNodes[0].u4Index = 0;
        g_aMemPoolInfo[u4PoolIdx].paNodes[0].pNext = NULL;
        g_aMemPoolInfo[u4PoolIdx].paNodes[0].pPrev = NULL;
        g_aMemPoolInfo[u4PoolIdx].u4IndexTbl = (~0x1);
        g_aMemPoolInfo[u4PoolIdx].u4UserCount= 0;
    }
    //
    #if SYSRAM_SWITCH_BANK
    SYSRAM_USER_SWITCH_BANK(true);
    SYSRAM_MFG_SWITCH_BANK(false);
    #endif
    LOG_MSG("[probe]probe SYSRAM success\n");

    return 0;
}

// Called when the device is being detached from the driver
static int SYSRAM_remove(struct platform_device *pdev)
{
    //unregister char driver.
    UnregisterSYSRAMCharDrv();
    //
    device_destroy(pSYSRAM_CLASS, g_SYSRAMdevno);
    class_destroy(pSYSRAM_CLASS);
    //
    #if SYSRAM_SWITCH_BANK
    mt65xx_reg_sync_writel(
        DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK)|SMI_REG_SWITCH_BANK_SEL0|SMI_REG_SWITCH_BANK_SEL1|SMI_REG_SWITCH_BANK_SEL2,
        SMI_REG_ADDR_SWITCH_BANK);
    #endif
    //
    LOG_MSG("[remove]\n");
    //
    return 0;
}

static int SYSRAM_suspend(struct platform_device *pdev, pm_message_t mesg)
{
    return 0;
}

static int SYSRAM_resume(struct platform_device *pdev)
{
    return 0;
}

/*******************************************************************************
*
*******************************************************************************/
#if 0
static int SYSRAM_Check(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    char TestData = 0xFF;
    long i;
    unsigned long DiffStart = 0, DiffEnd = 0, DiffSize = 0;
    unsigned long TestSize = 32*1024;
    unsigned long TestAddr = 0xF2020000;

    for(i=0;i<TestSize;i++)
    {
        if(*(char*)(TestAddr+i) != TestData)
        {
            DiffSize++;
        }
    }

    for(i=0;i<TestSize;i++)
    {
        if(*(char*)(TestAddr+i) != TestData)
        {
            DiffStart = i;
            break;
        }
    }

    for(i=TestSize-1;i>=0;i--)
    {
        if(*(char*)(TestAddr+i) != TestData)
        {
            DiffEnd= i;
            break;
        }
    }

    LOG_MSG("\nDiffSize = %d, DiffStart = 0x%08X, DiffEnd = 0x%08X",DiffSize,DiffStart,DiffEnd);
    LOG_MSG("\n");
    for(i=0;i<TestSize;i++)
    {
        if(i%16 == 0)
        {
            LOG_MSG("\nAddr=0x%08X:",TestAddr+i);
        }
        LOG_MSG("%02X.",*(char*)(TestAddr+i));
    }
    LOG_MSG("\n");

    return 0;
}

static int SYSRAM_Set(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    char TestData = 0xFF;
    unsigned long TestSize = 32*1024;
    unsigned long TestAddr = 0xF2020000;

    memset(TestAddr,TestData,TestSize);
    return 0;
}

static int SYSRAM_SWB0(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    DRV_WriteReg32(SMI_REG_ADDR_SWITCH_BANK,DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK)&(~(1<<0)));
    return 0;
}

static int SYSRAM_SWB1(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    DRV_WriteReg32(SMI_REG_ADDR_SWITCH_BANK,DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK)|(1<<0));
    return 0;
}


static int SYSRAM_Test(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    char TestData = 0xFF;
    long i;
    unsigned long DiffSize = 0, TestSize = 32*1024;
    unsigned long TestAddr = 0xF2028000;

    LOG_MSG("\n[SYSRAM_Test]Low to High");
    DiffSize = 0;
    for(i=0;i<TestSize;i++)
    {
        *(char*)(TestAddr+i) = (i & 0xFF);
    }
    for(i=0;i<TestSize;i++)
    {
        if(*(char*)(TestAddr+i) != (i & 0xFF))
        {
            DiffSize++;
        }
    }
    LOG_MSG("\n[SYSRAM_Test]Low to High:DiffSize = %d",DiffSize);

    LOG_MSG("\n");
    for(i=0;i<TestSize;i++)
    {
        if(i%16 == 0)
        {
            LOG_MSG("\nAddr=0x%08X:",TestAddr+i);
        }
        LOG_MSG("%02X.",*(char*)(TestAddr+i));
    }
    LOG_MSG("\n");

    LOG_MSG("\n[SYSRAM_Test]High to Low");
    DiffSize = 0;

    for(i=TestSize-1;i>=0;i--)
    {
        *(char*)(TestAddr+i) = (i & 0xFF);
    }

    for(i=TestSize-1;i>=0;i--)
    {
        if(*(char*)(TestAddr+i) != (i & 0xFF))
        {
            DiffSize++;
        }
    }

    LOG_MSG("\n[SYSRAM_Test]High to Low:DiffSize = %d",DiffSize);

    LOG_MSG("\n");
    for(i=0;i<TestSize;i++)
    {
        if(i%16 == 0)
        {
            LOG_MSG("\nAddr=0x%08X:",TestAddr+i);
        }
        LOG_MSG("%02X.",*(char*)(TestAddr+i));
    }
    LOG_MSG("\n");

    return 0;
}
#endif
/*******************************************************************************
*
********************************************************************************/
static struct platform_driver g_stSYSRAM_Platform_Driver = {
    .probe      = SYSRAM_probe,
    .remove     = SYSRAM_remove,
    .suspend    = SYSRAM_suspend,
    .resume     = SYSRAM_resume,
    .driver     = {
        .name   = SYSRAM_DEV_NAME,
        .owner  = THIS_MODULE,
    }
};

static int SYSRAM_dump_layout_to_proc(char *page, char **start, off_t off, int count, int *eof, void *data);
static int SYSRAM_read_flag(char *page, char **start, off_t off, int count, int *eof, void *data);
static int SYSRAM_write_flag(struct file *file, const char *buffer, unsigned long count, void *data);
static int __init SYSRAM_Init(void)
{
    struct proc_dir_entry *prEntry;

    if(platform_driver_register(&g_stSYSRAM_Platform_Driver)){
        LOG_ERR("[Init]failed to register sysram driver\n");
        return -ENODEV;
    }
    //
    prEntry = create_proc_entry("sysram", 0, NULL);
    if (prEntry) {
        prEntry->read_proc = SYSRAM_dump_layout_to_proc;
        prEntry->write_proc = NULL;
    }
    else {
        LOG_ERR("[Init]add /proc/sysram entry fail \n");  
    }
    //
    #if 0
    prEntry = create_proc_entry("sysram_check", 0, NULL);
    if (prEntry) {
        prEntry->read_proc = SYSRAM_Check;
        prEntry->write_proc = NULL;
    }
    else {
        LOG_MSG("add /proc/sysram_check entry fail \n");  
    }
    //
    prEntry = create_proc_entry("sysram_set", 0, NULL);
    if (prEntry) {
        prEntry->read_proc = SYSRAM_Set;
        prEntry->write_proc = NULL;
    }
    else {
        LOG_MSG("add /proc/SYSRAM_Set entry fail \n");  
    }
    //
    prEntry = create_proc_entry("sysram_swb0", 0, NULL);
    if (prEntry) {
        prEntry->read_proc = SYSRAM_SWB0;
        prEntry->write_proc = NULL;
    }
    else {
        LOG_MSG("add /proc/SYSRAM_Set entry fail \n");  
    }
    //
    prEntry = create_proc_entry("sysram_swb1", 0, NULL);
    if (prEntry) {
        prEntry->read_proc = SYSRAM_SWB1;
        prEntry->write_proc = NULL;
    }
    else {
        LOG_MSG("add /proc/SYSRAM_Set entry fail \n");  
    }
    //
    prEntry = create_proc_entry("sysram_test", 0, NULL);
    if (prEntry) {
        prEntry->read_proc = SYSRAM_Test;
        prEntry->write_proc = NULL;
    }
    else {
        LOG_MSG("add /proc/SYSRAM_Set entry fail \n");  
    }
    #endif
    //
    prEntry = create_proc_entry("sysram_flag", 0, NULL);
    if (prEntry) {
        prEntry->read_proc  = SYSRAM_read_flag;
        prEntry->write_proc = SYSRAM_write_flag;
    }
    else {
        LOG_ERR("[Init]add /proc/sysram_flag entry fail \n");  
    }
    //
    return 0;
}

static void __exit SYSRAM_Exit(void)
{
    platform_driver_unregister(&g_stSYSRAM_Platform_Driver);
}

module_init(SYSRAM_Init);
module_exit(SYSRAM_Exit);
MODULE_DESCRIPTION("Camera sysram driver");
MODULE_AUTHOR("Marx <Marx.Chiu@Mediatek.com>");
MODULE_AUTHOR("Jonas <Jonas.Lai@Mediatek.com>");
MODULE_LICENSE("GPL");


/*******************************************************************************
*
********************************************************************************/
static void MTxxx_SYSRAM_DUMPLAYOUT(void)
{
    unsigned int u4PoolIdx = 0;
    MemNode_T* pCurrNode = NULL;

    printk("[SYSRAM][Layout]\n");
    #if SYSRAM_SWITCH_BANK
    printk("SWITCH_BANK = 0x%08X\n",DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK));
    #endif
    printk("u4AllocatedTbl = 0x%08lX\n",g_ResMgr.u4AllocatedTbl);
    printk("=========================================\n");
    for (u4PoolIdx = 0; u4PoolIdx < MEM_POOL_CNT; u4PoolIdx++)
    {
        printk("\n [Mem Pool %d] (u4IndexTbl, u4UserCount)=(%lX, %ld)\n", u4PoolIdx, g_aMemPoolInfo[u4PoolIdx].u4IndexTbl, g_aMemPoolInfo[u4PoolIdx].u4UserCount);
        printk("[Locked Time/jiffies] [Owner   Offset   Size  Index pCurrent pPrevious pNext]  [pid tgid] [Proc Name / Owner Name]\n");
        pCurrNode = &g_aMemPoolInfo[u4PoolIdx].paNodes[0];
        while ( NULL != pCurrNode )
        {
            ESysramUser_T const eOwner = pCurrNode->eOwner;
            if  ( IsBadOwner(eOwner) )
            {
                printk(
                    "------------ --------"
                    " %2d\t0x%05lX 0x%05lX  %ld    %p %p\t%p\n", 
                    pCurrNode->eOwner, pCurrNode->u4Offset, pCurrNode->u4Length, 
                    pCurrNode->u4Index, pCurrNode, pCurrNode->pPrev, pCurrNode->pNext
                );
            }
            else
            {
                TaskInfo_T*const pInfo = &g_ResMgr.aOwnerTaskInfo[eOwner];
                printk(
                    "%5lu.%06lu %08lX"
                    " %2d\t0x%05lX 0x%05lX  %ld    %p %p\t%p"
                    "  %-4d %-4d \"%s\" / \"%s\"\n", 
                    pInfo->t_sec, pInfo->t_usec, pInfo->t_jiffies, 
                    eOwner, pCurrNode->u4Offset, pCurrNode->u4Length, 
                    pCurrNode->u4Index, pCurrNode, pCurrNode->pPrev, pCurrNode->pNext, 
                    pInfo->pid, pInfo->tgid, pInfo->comm, g_apszOwnerName[eOwner]
                );
            }
            pCurrNode = pCurrNode->pNext;
        };
    }

    printk("\n");

    Dump_ResMgr();
}


/*******************************************************************************
*
********************************************************************************/
static int SYSRAM_dump_layout_to_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;

    unsigned int u4PoolIdx = 0;
    MemNode_T * pCurrNode = NULL;

    //
    p += sprintf(p, "\n[SYSRAM Layout]\n");
    #if SYSRAM_SWITCH_BANK
    p += sprintf(p, "SWITCH_BANK = 0x%08X\n",DRV_Reg32(SMI_REG_ADDR_SWITCH_BANK));
    #endif
    p += sprintf(p, "u4AllocatedTbl = 0x%08lX\n",g_ResMgr.u4AllocatedTbl);
    p += sprintf(p, "=========================================\n" );
    for (u4PoolIdx = 0; u4PoolIdx < MEM_POOL_CNT; u4PoolIdx++)
    {
        p += sprintf(p, "\n [Mem Pool %d] (u4IndexTbl, u4UserCount)=(%lX, %ld)\n", u4PoolIdx, g_aMemPoolInfo[u4PoolIdx].u4IndexTbl, g_aMemPoolInfo[u4PoolIdx].u4UserCount);
        p += sprintf(p, "[Locked Time/jiffies] [Owner   Offset   Size  Index pCurrent pPrevious pNext]  [pid tgid] [Proc Name / Owner Name]\n");
        pCurrNode = &g_aMemPoolInfo[u4PoolIdx].paNodes[0];
        while ( NULL != pCurrNode )
        {
            ESysramUser_T const eOwner = pCurrNode->eOwner;
            if  ( IsBadOwner(eOwner) )
            {
                p += sprintf(p, 
                    "------------ --------"
                    " %2d\t0x%05lX 0x%05lX  %ld    %p %p\t%p\n", 
                    pCurrNode->eOwner, pCurrNode->u4Offset, pCurrNode->u4Length, 
                    pCurrNode->u4Index, pCurrNode, pCurrNode->pPrev, pCurrNode->pNext
                );
            }
            else
            {
                TaskInfo_T*const pInfo = &g_ResMgr.aOwnerTaskInfo[eOwner];
                p += sprintf(p, 
                    "%5lu.%06lu %08lX"
                    " %2d\t0x%05lX 0x%05lX  %ld    %p %p\t%p"
                    "  %-4d %-4d \"%s\" / \"%s\"\n", 
                    pInfo->t_sec, pInfo->t_usec, pInfo->t_jiffies, 
                    eOwner, pCurrNode->u4Offset, pCurrNode->u4Length, 
                    pCurrNode->u4Index, pCurrNode, pCurrNode->pPrev, pCurrNode->pNext, 
                    pInfo->pid, pInfo->tgid, pInfo->comm, g_apszOwnerName[eOwner]
                );
            }
            pCurrNode = pCurrNode->pNext;
        };
    }

    *start = page + off;

    len = p - page;
    if (len > off) {
        len -= off;
    }
    else {
        len = 0;
    }

    return len < count ? len : count;
}

static int SYSRAM_read_flag(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    //
    p += sprintf(p, "\r\n[SYSRAM debug flag]\r\n");
    p += sprintf(p, "=========================================\r\n" );
    p += sprintf(p, "g_u4SysramDbgFlag = 0x%08X\r\n", g_u4SysramDbgFlag);
    p += sprintf(p, "g_u4EnableUserGroupSize = %u\r\n", g_u4EnableUserGroupSize);
    p += sprintf(p, "g_u4EnableOverSizeFail = %u\r\n", g_u4EnableOverSizeFail);

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}


static int SYSRAM_write_flag(struct file *file, const char *buffer, unsigned long count, void *data)
{
    char acBuf[32]; 
    unsigned long u4CopySize = 0;
    unsigned int  u4SysramDbgFlag = 0;
    unsigned int  u4EnableUserGroupSize = 0;
    unsigned int  u4EnableOverSizeFail = 0;

    u4CopySize = (count < (sizeof(acBuf) - 1)) ? count : (sizeof(acBuf) - 1);
    if(copy_from_user(acBuf, buffer, u4CopySize))
        return 0;
    acBuf[u4CopySize] = '\0';

    if ( 3 == sscanf(acBuf, "%x%u%u", &u4SysramDbgFlag, &u4EnableUserGroupSize, &u4EnableOverSizeFail) )
    {
        g_u4SysramDbgFlag = u4SysramDbgFlag;
        g_u4EnableUserGroupSize = u4EnableUserGroupSize;
        g_u4EnableOverSizeFail = u4EnableOverSizeFail;
    }
    return count;
}

//-----------------------------------------------------------------------------
//This is compatible with other chip.
unsigned long SYSRAM_TVOUT_ALLOC_TIMEOUT(unsigned long const u4Size, unsigned long u4Alignment, unsigned long const u4TimeoutInMS)
{
    return MTxxx_SYSRAM_ALLOC_TIMEOUT(ESysramUser_TVOUT, u4Size, u4Alignment, u4TimeoutInMS);
}

void SYSRAM_TVOUT_FREE(void)
{
    MTxxx_SYSRAM_FREE(ESysramUser_TVOUT);
}

unsigned long SYSRAM_TV_ROT_ALLOC_TIMEOUT(unsigned long const u4Size, unsigned long u4Alignment, unsigned long const u4TimeoutInMS)
{
    return MTxxx_SYSRAM_ALLOC_TIMEOUT(ESysramUser_TV_ROT, u4Size, u4Alignment, u4TimeoutInMS);
}

void SYSRAM_TV_ROT_FREE(void)
{
    MTxxx_SYSRAM_FREE(ESysramUser_TV_ROT);
}
//-----------------------------------------------------------------------------
unsigned long SYSRAM_AUDIO_ALLOC_TIMEOUT(unsigned long const u4Size, unsigned long u4Alignment, unsigned long const u4TimeoutInMS)
{
    return MTxxx_SYSRAM_ALLOC_TIMEOUT(ESysramUser_AUDIO, u4Size, u4Alignment, u4TimeoutInMS);
}

void SYSRAM_AUDIO_FREE(void)
{
    MTxxx_SYSRAM_FREE(ESysramUser_AUDIO);
}
//-----------------------------------------------------------------------------
unsigned long SYSRAM_VIDEO_ALLOC_TIMEOUT(unsigned long const u4Size, unsigned long u4Alignment, unsigned long const u4TimeoutInMS)
{
    return MTxxx_SYSRAM_ALLOC_TIMEOUT(ESysramUser_VIDEO, u4Size, u4Alignment, u4TimeoutInMS);
}

void SYSRAM_VIDEO_FREE(void)
{
    MTxxx_SYSRAM_FREE(ESysramUser_VIDEO);
}
//-----------------------------------------------------------------------------

EXPORT_SYMBOL(SYSRAM_TVOUT_ALLOC_TIMEOUT);
EXPORT_SYMBOL(SYSRAM_TVOUT_FREE);
EXPORT_SYMBOL(SYSRAM_TV_ROT_ALLOC_TIMEOUT);
EXPORT_SYMBOL(SYSRAM_TV_ROT_FREE);
EXPORT_SYMBOL(SYSRAM_AUDIO_ALLOC_TIMEOUT);
EXPORT_SYMBOL(SYSRAM_AUDIO_FREE);
EXPORT_SYMBOL(SYSRAM_VIDEO_ALLOC_TIMEOUT);
EXPORT_SYMBOL(SYSRAM_VIDEO_FREE);
EXPORT_SYMBOL(SYSRAM_MFG_SWITCH_BANK);


