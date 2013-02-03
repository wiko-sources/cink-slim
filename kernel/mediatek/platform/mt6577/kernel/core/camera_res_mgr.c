//----------------------------------------------------------------------------
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <mach/camera_res_mgr_imp.h>
//----------------------------------------------------------------------------
static volatile MUINT32 ResMgrLog = RES_MGR_LOG_ERR;
//----------------------------------------------------------------------------
#define LOG_MSG(fmt, arg...)\
    if(ResMgrLog & RES_MGR_LOG_MSG) \
    { \
        printk(KERN_DEBUG "[RES_MGR][%s]" fmt,__FUNCTION__, ##arg); \
    }
#define LOG_WRN(fmt, arg...) \
    if(ResMgrLog & RES_MGR_LOG_WRN) \
    { \
        printk(KERN_DEBUG "[RES_MGR][%s]Warning(%5d):" fmt, __FUNCTION__,__LINE__, ##arg); \
    }
#define LOG_ERR(fmt, arg...) \
    if(ResMgrLog & RES_MGR_LOG_ERR) \
    { \
        printk(KERN_ERR   "[RES_MGR][%s]Err(%5d):" fmt, __FUNCTION__,__LINE__, ##arg); \
    }
#define LOG_DUMP(fmt, arg...)   printk(KERN_ERR "[RES_MGR][%s]" fmt,__FUNCTION__, ##arg)
//----------------------------------------------------------------------------
static RES_MGR_STRUCT ResMgr;
//----------------------------------------------------------------------------
static inline void ResMgr_SpinLock(void)
{
    //LOG_MSG("[SpinLock]\r\n");
    spin_lock(&(ResMgr.SpinLock));
}
//----------------------------------------------------------------------------
static inline void ResMgr_SpinUnlock(void)
{
    //LOG_MSG("[SpinUnlock]\r\n");
    spin_unlock(&(ResMgr.SpinLock));
}
//----------------------------------------------------------------------------
static unsigned long ResMgr_MsToJiffies(MUINT32 Ms)
{
    return ((Ms*HZ + 512) >> 10);
}
//----------------------------------------------------------------------------
static void ResMgr_DumpLockInfo(void)
{
    MUINT32 i;
    //
    LOG_DUMP("E\r\n");
    for(i=0;i<RES_MGR_ITEM_AMOUNT;i++)
    {
        if( ResMgr.LockInfo[i].Pid != 0 &&
            ResMgr.LockInfo[i].Tgid != 0)
        {
            LOG_DUMP("Item(%ld,%s),Pid(%d),Tgid(%d),TaskName(%s),Jiffies(%ld),Time(%ld.%ld)\r\n",
                i,
                ResMgrItemName[i],
                ResMgr.LockInfo[i].Pid,
                ResMgr.LockInfo[i].Tgid,
                ResMgr.LockInfo[i].TaskName,
                ResMgr.LockInfo[i].Jiffies,
                ResMgr.LockInfo[i].TimeS,
                ResMgr.LockInfo[i].TimeUS);
        }
    }
    LOG_DUMP("X\r\n");
}
//----------------------------------------------------------------------------
static void ResMgr_StoreLockInfo(MUINT32 ResMask)
{
    MUINT32 i;
    //
    //LOG_MSG("ResMask(0x%08X)\r\n",ResMask);
    //
    for(i=0;i<RES_MGR_ITEM_AMOUNT;i++)
    {
        if((1<<i) & ResMask)
        {
            ResMgr.LockInfo[i].Pid = current->pid;
            ResMgr.LockInfo[i].Tgid = current->tgid;
            memcpy(ResMgr.LockInfo[i].TaskName,current->comm,TASK_COMM_LEN);
            ResMgr.LockInfo[i].Jiffies = jiffies;
            ResMgr.LockInfo[i].TimeS = (ResMgr.LockInfo[i].Jiffies - INITIAL_JIFFIES)*(USEC_PER_SEC / HZ);
            ResMgr.LockInfo[i].TimeUS = do_div(ResMgr.LockInfo[i].TimeS,USEC_PER_SEC)/1000;
        }
    }
}
//----------------------------------------------------------------------------
static void ResMgr_RemoveLockInfo(MUINT32 ResMask)
{
    MUINT32 i;
    //
    //LOG_MSG("ResMask(0x%08X)\r\n",ResMask);
    //
    for(i=0;i<RES_MGR_ITEM_AMOUNT;i++)
    {
        if((1<<i) & ResMask)
        {
            ResMgr.LockInfo[i].Pid = 0;
            ResMgr.LockInfo[i].Tgid = 0;
        }
    }
}
//----------------------------------------------------------------------------
static RES_MGR_LOCK_STATUS_ENUM ResMgr_ResLock(RES_MGR_RES_LOCK_STRUCT* pResLock)
{
    MUINT32 Timeout;
    RES_MGR_RES_LOCK_STRUCT ResLock;
    RES_MGR_LOCK_STATUS_ENUM Result = RES_MGR_LOCK_STATUS_OK;
    //
    memcpy(
        (void*)&ResLock,
        (void*)pResLock,
        sizeof(RES_MGR_RES_LOCK_STRUCT));
    //
    if((ResMgr.ResLockMask & ResLock.ResMask) == 0)
    {
        ResMgr.ResLockMask |= ResLock.ResMask;
        ResMgr.PermanentMask |= ResLock.PermanentMask;
        ResMgr_StoreLockInfo(ResLock.ResMask);
        //LOG_MSG("Lock OK, Resource(0x%08lX), Permanent(0x%08lX), ResMask(0x%08lX)\r\n",ResMgr.ResLockMask,ResMgr.PermanentMask,ResLock.ResMask);
        Result = RES_MGR_LOCK_STATUS_OK;
    }
    else
    if((ResMgr.PermanentMask & ResLock.ResMask) != 0)
    {
        //LOG_ERR("Lock fail, Resource(0x%08lX), Permanent(0x%08lX), ResMask(0x%08lX)\r\n",ResMgr.ResLockMask,ResMgr.PermanentMask,ResLock.ResMask);
        Result = RES_MGR_LOCK_STATUS_FAIL;
    }
    else
    {
        ResMgr_SpinUnlock();
        Timeout = wait_event_interruptible_timeout(
                    ResMgr.WaitQueueHead, 
                    (ResMgr.ResLockMask & ResLock.ResMask) == 0,
                    ResMgr_MsToJiffies(ResLock.Timeout));
        ResMgr_SpinLock();
        if((ResMgr.ResLockMask & ResLock.ResMask) == 0)
        {
            ResMgr.ResLockMask |= ResLock.ResMask;
            ResMgr.PermanentMask |= (ResLock.PermanentMask & ResLock.ResMask); //Make sure ResLock.PermanentMask is not out of range for ResLock.ResMask.
            ResMgr_StoreLockInfo(ResLock.ResMask);
            //LOG_MSG("Lock OK, Resource(0x%08lX), Permanent(0x%08lX), ResMask(0x%08lX)\r\n",ResMgr.ResLockMask,ResMgr.PermanentMask,ResLock.ResMask);
            Result = RES_MGR_LOCK_STATUS_OK;
        }
        else
        if( Timeout == 0 &&
            (ResMgr.ResLockMask & ResLock.ResMask) != 0)
        {
            //LOG_ERR("Lock timeout, Resource(0x%08lX), Permanent(0x%08lX), ResMask(0x%08lX)\r\n",ResMgr.ResLockMask,ResMgr.PermanentMask,ResLock.ResMask);
            Result = RES_MGR_LOCK_STATUS_TIMEOUT;
        }
        else
        {
            //LOG_ERR("Lock unknow, Timeout(%lu), Resource(0x%08lX), Permanent(0x%08lX), ResMask(0x%08lX)\r\n",Timeout,ResMgr.ResLockMask,ResMgr.PermanentMask,ResLock.ResMask);
            Result = RES_MGR_LOCK_STATUS_UNKNOW_ERROR;
        }
    }
    //
    return Result;
}
//----------------------------------------------------------------------------
static void ResMgr_ResUnlock(MUINT32 ResMask)
{
    ResMgr.ResLockMask &= (~ResMask);
    ResMgr.PermanentMask &= (~ResMask);
    ResMgr_RemoveLockInfo(ResMask);
    wake_up_interruptible(&(ResMgr.WaitQueueHead));
    //
    //LOG_MSG("Unlock OK, Resource(0x%08lX), Permanent(0x%08lX), ResMask(0x%08lX)\r\n",ResMgr.ResLockMask,ResMgr.PermanentMask,ResMask);
}
//----------------------------------------------------------------------------
static int ResMgr_open(
    struct inode*   pInode,
    struct file*    pFile)
{
    int Ret = 0;
    RES_MGR_OPEN_INFO_STRUCT*   pOpenInfo;
    //
    LOG_MSG("Current:process(%s), pid(%d), tgid(%d)\r\n", current->comm, current->pid, current->tgid);
    //
    ResMgr_SpinLock();
    //
    pFile->private_data = NULL;
    pFile->private_data = kmalloc(sizeof(RES_MGR_OPEN_INFO_STRUCT) , GFP_ATOMIC);
    if(pFile->private_data == NULL)
    {
        Ret = -ENOMEM;
    }
    else
    {
        pOpenInfo = (RES_MGR_OPEN_INFO_STRUCT*)pFile->private_data;
        pOpenInfo->Pid = 0;
        pOpenInfo->Tgid = 0;
        pOpenInfo->Resource = 0;
    }
    //
    ResMgr_SpinUnlock();
    //
    if(Ret == (-ENOMEM))
    {
        LOG_ERR("No enough memory,Current:process(%s), pid(%d), tgid(%d)\r\n", current->comm, current->pid, current->tgid);
    }
    //
    //LOG_MSG("OK\r\n");
    return Ret;
}
//----------------------------------------------------------------------------
static int ResMgr_release(
    struct inode*   pInode,
    struct file*    pFile)
{
    RES_MGR_OPEN_INFO_STRUCT*   pOpenInfo;
    //
    LOG_MSG("Current:process(%s), pid(%d), tgid(%d)\r\n", current->comm, current->pid, current->tgid);
    //
    if(pFile->private_data != NULL)
    {
        pOpenInfo = (RES_MGR_OPEN_INFO_STRUCT*)pFile->private_data;
        LOG_MSG("OpenInfo:pid(%d), tgid(%d), Resource(0x%08lX)\r\n",pOpenInfo->Pid,pOpenInfo->Tgid,pOpenInfo->Resource);
        if(pOpenInfo->Resource)
        {
            LOG_WRN("free resource and memory,OpenInfo:pid(%d), tgid(%d), Resource(0x%08lX)\r\n",pOpenInfo->Pid,pOpenInfo->Tgid,pOpenInfo->Resource);
        }
        //
        ResMgr_SpinLock();
        if(pOpenInfo->Resource)
        {
            ResMgr_ResUnlock(pOpenInfo->Resource);
            pOpenInfo->Resource = 0;
        }
        kfree(pFile->private_data);
        pFile->private_data = NULL;
        ResMgr_SpinUnlock();
    }
    //
    //LOG_MSG("OK\r\n");
    return 0;
}
//----------------------------------------------------------------------------
static int ResMgr_flush(
    struct file*    pFile,
    fl_owner_t      Id)
{
    RES_MGR_OPEN_INFO_STRUCT*   pOpenInfo;
    //
    LOG_MSG("Current:process(%s), pid(%d), tgid(%d)\r\n", current->comm, current->pid, current->tgid);
    //
    if(pFile->private_data != NULL)
    {
        pOpenInfo = (RES_MGR_OPEN_INFO_STRUCT*)pFile->private_data;
        LOG_MSG("OpenInfo:pid(%d), tgid(%d), Resource(0x%08lX)\r\n",pOpenInfo->Pid,pOpenInfo->Tgid,pOpenInfo->Resource);
        if(pOpenInfo->Tgid == 0)
        {
            if(pOpenInfo->Resource != 0)
            {
                LOG_ERR("No Tgid info about resource\r\n");
                LOG_ERR("OpenInfo:pid(%d), tgid(%d), Resource(0x%08lX)\r\n",pOpenInfo->Pid,pOpenInfo->Tgid,pOpenInfo->Resource);
                LOG_ERR("Current:process(%s), pid(%d), tgid(%d)\r\n",current->comm , current->pid, current->tgid);
            }
        }
        else
        if( (pOpenInfo->Tgid == current->tgid) ||
            ((pOpenInfo->Tgid != current->tgid) && (strcmp(current->comm, "binder") == 0)))
        {
            if(pOpenInfo->Resource)
            {
                LOG_WRN("free resource and memory,OpenInfo:pid(%d), tgid(%d), Resource(0x%08lX)\r\n",pOpenInfo->Pid,pOpenInfo->Tgid,pOpenInfo->Resource);
                ResMgr_SpinLock();
                ResMgr_ResUnlock(pOpenInfo->Resource);
                pOpenInfo->Resource = 0;
                ResMgr_SpinUnlock();
            }
        }
    }
    //
    //LOG_MSG("OK\r\n");
    return 0;
}
//----------------------------------------------------------------------------
static long ResMgr_ioctl(
    struct file*    pFile,
    unsigned int    Cmd,
    unsigned long   Param)
{
    MINT32  Ret = 0;
    MUINT32 LogMask;
    RES_MGR_RES_LOCK_STRUCT     ResLock;
    RES_MGR_RES_CHECK_STRUCT    ResCheck;
    RES_MGR_OPEN_INFO_STRUCT*   pOpenInfo = (RES_MGR_OPEN_INFO_STRUCT*)pFile->private_data;
    RES_MGR_LOCK_STATUS_ENUM    LockStatus;
    //
    //LOG_MSG("[ioctl]\r\n");
    //
    if(pFile->private_data == NULL)
    {
        LOG_ERR("private_data is NULL,Current:process(%s), pid(%d), tgid(%d)\r\n", current->comm, current->pid, current->tgid);
        Ret = -EFAULT;
        goto EXIT;
    }
    //
    switch(Cmd)
    {
        case RES_MGR_RES_LOCK:
        {
            if(copy_from_user(&ResLock, (void*)Param, sizeof(RES_MGR_RES_LOCK_STRUCT)) == 0)
            {
                ResMgr_SpinLock();
                LockStatus = ResMgr_ResLock(&ResLock);
                if(LockStatus == RES_MGR_LOCK_STATUS_OK)
                {
                    pOpenInfo->Resource |= ResLock.ResMask;
                    if(pOpenInfo->Tgid == 0)
                    {
                        pOpenInfo->Pid = current->pid;
                        pOpenInfo->Tgid = current->tgid;
                        ResMgr_SpinUnlock();
                        LOG_MSG("Lock Resource(0x%08lX),Current:process(%s), pid(%d), tgid(%d)\r\n",ResLock.ResMask,current->comm,current->pid,current->tgid);
                    }
                    else
                    {
                        ResMgr_SpinUnlock();
                        if(pOpenInfo->Tgid != current->tgid)
                        {
                            LOG_ERR("Tgid is inconsistent,OpenInfo.Tgid(%d) != Current.Tgid(%d)\r\n",pOpenInfo->Tgid,current->tgid);
                        }
                    }
                }
                else
                {
                    ResMgr_SpinUnlock();
                    Ret = -EFAULT;
                    LOG_ERR("LockStatus(%d)\r\n",LockStatus);
                }
            }
            else
            {
                LOG_ERR("copy_from_user fail\r\n");
                Ret = -EFAULT;
            }
            break;
        }

        case RES_MGR_RES_UNLOCK:
        {
            if(copy_from_user(&(ResLock.ResMask), (void*)Param, sizeof(unsigned long)) == 0)
            {
                ResMgr_SpinLock();
                if(pOpenInfo->Resource & ResLock.ResMask)
                {
                    ResMgr_ResUnlock(ResLock.ResMask);
                    pOpenInfo->Resource &= (~ResLock.ResMask);
                    if(pOpenInfo->Resource == 0)
                    {
                        pOpenInfo->Pid = 0;
                        pOpenInfo->Tgid = 0;
                    }
                    ResMgr_SpinUnlock();
                    LOG_MSG("Unlock Resource(0x%08lX),Current:process(%s), pid(%d), tgid(%d)\r\n",ResLock.ResMask,current->comm,current->pid,current->tgid);
                    LOG_MSG("OpenInfo:Resource(0x%08lX), pid(%d), tgid(%d)\r\n",pOpenInfo->Resource,pOpenInfo->Pid,pOpenInfo->Tgid);
                }
                else
                {
                    ResMgr_SpinUnlock();
                    Ret = -EFAULT;
                    LOG_ERR("Unlock resource(0x%08lX) which was not locked before\r\n",ResLock.ResMask);
                    LOG_ERR("Current:process(%s), pid(%d), tgid(%d)\r\n",current->comm,current->pid,current->tgid);
                    LOG_ERR("OpenInfo:Resource(0x%08lX), pid(%d), tgid(%d)\r\n",pOpenInfo->Resource,pOpenInfo->Pid,pOpenInfo->Tgid);
                }
               
            }
            else
            {
                LOG_ERR("copy_from_user fail\r\n");
                Ret = -EFAULT;
            }
            break;
        }

        case RES_MGR_RES_CHECK:
        {
            ResMgr_SpinLock();
            ResCheck.ResLockMask = ResMgr.ResLockMask;
            ResCheck.PermanentMask = ResMgr.PermanentMask;
            ResMgr_SpinUnlock();
            //
            if(copy_to_user((void*)Param, (void*)&ResCheck, sizeof(RES_MGR_RES_CHECK_STRUCT)) != 0)
            {
                LOG_ERR("copy_to_user fail\n");
                Ret = -EFAULT;
            }
            //LOG_MSG("[ioctl]ResLockMask(0x%08lX), ResWaitMask(0x%08lX)\r\n",pResCheck->ResLockMask,pResCheck->ResWaitMask);
            break;
        }

        case RES_MGR_LOG_ENABLE:
        {
            if(copy_from_user(&LogMask, (void*)Param, sizeof(unsigned long)) == 0)
            {
                ResMgr_SpinLock();
                ResMgrLog |= LogMask;
                ResMgr_SpinUnlock();
            }
            else
            {
                LOG_ERR("copy_from_user fail\r\n");
                Ret = -EFAULT;
            }
            break;
        }

        case RES_MGR_LOG_DISABLE:
        {
            if(copy_from_user(&LogMask, (void*)Param, sizeof(unsigned long)) == 0)
            {
                ResMgr_SpinLock();
                ResMgrLog &= (~LogMask);
                ResMgr_SpinUnlock();
            }
            else
            {
                LOG_ERR("copy_from_user fail\r\n");
                Ret = -EFAULT;
            }
            break;
        }

        default:
        {
            LOG_ERR("Unknow cmd(%d)\r\n",Cmd);
            Ret = -EFAULT;
            break;
        }
    }

    if(Ret != 0)
    {
        LOG_ERR("Fail\r\n");
        LOG_ERR("OpenInfo:pid(%d), tgid(%d), Resource(0x%08lX)\r\n",pOpenInfo->Pid,pOpenInfo->Tgid,pOpenInfo->Resource);
        LOG_ERR("Current:process(%s), pid(%d), tgid(%d)\r\n",current->comm , current->pid, current->tgid);
        ResMgr_DumpLockInfo();
    }

    EXIT:
    return Ret;
}
//----------------------------------------------------------------------------
static const struct file_operations ResMgr_FileOper = 
{
    .owner          = THIS_MODULE,
    .open           = ResMgr_open,
    .release        = ResMgr_release,
    .flush          = ResMgr_flush,
    .unlocked_ioctl = ResMgr_ioctl
};
//----------------------------------------------------------------------------
static int ResMgr_RegCharDev(void)
{
    MINT32 Ret = 0;
    //
    LOG_MSG("\r\n");
    //
    ResMgr.DevNo = 0;
    Ret = alloc_chrdev_region(&(ResMgr.DevNo), RES_MGR_DEV_NO_MINOR, RES_MGR_DEV_NUM, RES_MGR_DEV_NAME);
    if(Ret < 0)
    {
        LOG_ERR("alloc_chrdev_region fail:Ret(%ld)\r\n", Ret);
        return Ret;
    }
    //Allocate memory for driver
    ResMgr.pCharDrv = cdev_alloc();
    if(ResMgr.pCharDrv == NULL)
    {
        unregister_chrdev_region(ResMgr.DevNo, RES_MGR_DEV_NUM);
        LOG_ERR("Allocate mem for kobject failed\r\n");
        return -ENOMEM;
    }
    //Attatch file operation.
    cdev_init(ResMgr.pCharDrv, &ResMgr_FileOper);
    ResMgr.pCharDrv->owner = THIS_MODULE;
    //Add to system
    if(cdev_add(ResMgr.pCharDrv, ResMgr.DevNo, RES_MGR_DEV_MINOR_NUM))
    {
        LOG_ERR("Attatch file operation failed\r\n");
        unregister_chrdev_region(ResMgr.DevNo, RES_MGR_DEV_NUM);
        return -EAGAIN;
    }
    //
    LOG_MSG("OK\r\n");
    return Ret;
}
//----------------------------------------------------------------------------
static inline void ResMgr_UnregCharDev(void)
{
    LOG_MSG("\r\n");
    //Release char driver
    cdev_del(ResMgr.pCharDrv);
    unregister_chrdev_region(ResMgr.DevNo, RES_MGR_DEV_NUM);
    //
    LOG_MSG("OK\r\n");
}
//----------------------------------------------------------------------------
static int ResMgr_probe(struct platform_device *pDev)
{
    MINT32 Ret = 0;
    MUINT32 i;
    struct device* pDevice = NULL;
    //
    LOG_MSG("\r\n");
    //
    Ret = ResMgr_RegCharDev();
    if(Ret < 0)
    {
        LOG_ERR("RegCharDev fail:Ret(%ld)\r\n", Ret);
        return Ret;
    }

    ResMgr.pClass = class_create(THIS_MODULE, "ResourceMgrDrv");
    if (IS_ERR(ResMgr.pClass)) {
        Ret = PTR_ERR(ResMgr.pClass);
        LOG_ERR("class_create fail:Ret(%ld)\r\n", Ret);
        return Ret;            
    }
    pDevice = device_create(
                ResMgr.pClass,
                NULL,
                ResMgr.DevNo,
                NULL,
                RES_MGR_DEV_NAME);
    if(IS_ERR(pDevice))
    {
        LOG_ERR("device_create fail\r\n");
        return (int)pDevice;
    }
    //Initial variable
    ResMgrLog = RES_MGR_LOG_ERR;
    spin_lock_init(&(ResMgr.SpinLock));
    init_waitqueue_head(&(ResMgr.WaitQueueHead));
    for(i=0;i<RES_MGR_ITEM_AMOUNT;i++)
    {
        ResMgr.LockInfo[i].Pid = 0;
        ResMgr.LockInfo[i].Tgid = 0;
    }
    //
    LOG_MSG("OK\r\n");
    return Ret;
}
//----------------------------------------------------------------------------
static int ResMgr_remove(struct platform_device *pdev)
{
    //
    LOG_MSG("\r\n");
    //unregister char driver.
    ResMgr_UnregCharDev();
    //
    device_destroy(ResMgr.pClass, ResMgr.DevNo);
    class_destroy(ResMgr.pClass);
    //
    LOG_MSG("OK\r\n");
    return 0;
}
//----------------------------------------------------------------------------
static int ResMgr_suspend(
    struct platform_device* pDev,
    pm_message_t            Mesg)
{
    LOG_MSG("\r\n");
    return 0;
}
//----------------------------------------------------------------------------
static int ResMgr_resume(struct platform_device *pDev)
{
    LOG_MSG("\r\n");
    return 0;
}
//----------------------------------------------------------------------------
static struct platform_driver ResMgr_PlatformDriver =
{
    .probe      = ResMgr_probe,
    .remove     = ResMgr_remove,
    .suspend    = ResMgr_suspend,
    .resume     = ResMgr_resume,
    .driver     =
    {
        .name   = RES_MGR_DEV_NAME,
        .owner  = THIS_MODULE,
    }
};
//----------------------------------------------------------------------------
static int ResMgr_ReadLog(
    char*   pPage,
    char**  ppStart,
    off_t   Off,
    int     Count,
    int*    pEof,
    void*   pData)
{
    char *p = pPage;
    MUINT32 Len = 0;
    //
    p += sprintf(p, "[RSE_MGR]ResMgrLog(0x%08lX)\r\n",ResMgrLog);
    //
    *ppStart = pPage + Off;
    Len = p - pPage;
    if(Len > Off)
    {
        Len -= Off;
    }
    else
    {
        Len = 0;
    }
    //
    return Len < Count ? Len  : Count;
}
//----------------------------------------------------------------------------
static int ResMgr_WriteLog(
    struct file*    pFile,
    const char*     pBuffer,
    unsigned long   Count,
    void*           pData)
{
    char Buf[32]; 
    MUINT32 CopySize = 0, Log = 0;
    //
    CopySize = (Count < (sizeof(Buf) - 1)) ? Count : (sizeof(Buf) - 1);
    if(copy_from_user(Buf, pBuffer, CopySize))
    {
        return 0;
    }
    Buf[CopySize] = '\0';
    //
    sscanf(Buf, "%lX", &Log);
    ResMgrLog = Log;
    //
    return Count;
}
//----------------------------------------------------------------------------
static int __init ResMgr_Init(void)
{
    MINT32 Ret = 0;
    struct proc_dir_entry *pEntry;
    //
    LOG_MSG("\r\n");
    //
    Ret = platform_driver_register(&ResMgr_PlatformDriver);
    if(Ret < 0)
    {
        LOG_ERR("Failed to register driver:Ret(%ld)\r\n", Ret);
        return Ret;
    }
    //
    pEntry = create_proc_entry("driver/ResMgrLog", 0, NULL); 
    if(pEntry)
    {
        pEntry->read_proc = ResMgr_ReadLog;
        pEntry->write_proc = ResMgr_WriteLog;
    }
    else
    {
        LOG_ERR("add /proc/driver/ResMgrLog entry fail \n");
    }
    //
    LOG_MSG("OK\r\n");
    return Ret;
}
//----------------------------------------------------------------------------
static void __exit ResMgr_Exit(void)
{
    LOG_MSG("\r\n");
    platform_driver_unregister(&ResMgr_PlatformDriver);
    LOG_MSG("OK\r\n");
}
//----------------------------------------------------------------------------
module_init(ResMgr_Init);
module_exit(ResMgr_Exit);
MODULE_DESCRIPTION("Camera Resource Manager Driver");
MODULE_AUTHOR("Marx <Marx.Chiu@Mediatek.com>");
MODULE_LICENSE("GPL");
//----------------------------------------------------------------------------

