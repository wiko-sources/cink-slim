//----------------------------------------------------------------------------
#ifndef CAMERA_RES_MGR_IMP_H
#define CAMERA_RES_MGR_IMP_H
//----------------------------------------------------------------------------
#include <mach/camera_res_mgr.h>
//----------------------------------------------------------------------------
typedef unsigned long   MUINT32;
typedef long            MINT32;
typedef unsigned char   MUINT8;
typedef char            MINT8;
typedef bool            MBOOL;
#define MTRUE           true
#define MFALSE          false
//----------------------------------------------------------------------------
typedef enum
{
    RES_MGR_ITEM_ISP,
    RES_MGR_ITEM_AMOUNT
}RES_MGR_ITEM_ENUM;
//----------------------------------------------------------------------------
typedef enum
{
    RES_MGR_LOCK_STATUS_OK,
    RES_MGR_LOCK_STATUS_FAIL,
    RES_MGR_LOCK_STATUS_TIMEOUT,
    RES_MGR_LOCK_STATUS_UNKNOW_ERROR
}RES_MGR_LOCK_STATUS_ENUM;
//----------------------------------------------------------------------------
#define RES_MGR_DEV_NUM         1
#define RES_MGR_DEV_MINOR_NUM   1
#define RES_MGR_DEV_NO_MINOR    0
//----------------------------------------------------------------------------
typedef struct
{
    pid_t   Pid;
    pid_t   Tgid;
    MUINT32 Resource;
}RES_MGR_OPEN_INFO_STRUCT;

typedef struct
{
    pid_t               Pid;
    pid_t               Tgid;
    char                TaskName[TASK_COMM_LEN];
    MUINT32             Jiffies;    // jiffies
    MUINT32             TimeS;          // nanosec_rem = do_div((t_jiffies-INITIAL_JIFFIES) * (USEC_PER_SEC / HZ), USEC_PER_SEC);
    MUINT32             TimeUS;         // nanosec_rem / 1000)
}RES_MGR_LOCK_INFO_STRUCT;

typedef struct
{
    MUINT32                     ResLockMask;
    MUINT32                     PermanentMask;
    spinlock_t                  SpinLock;
    dev_t                       DevNo;
    struct cdev*                pCharDrv;
    struct class*               pClass;
    wait_queue_head_t           WaitQueueHead;
    RES_MGR_LOCK_INFO_STRUCT    LockInfo[RES_MGR_ITEM_AMOUNT];
}RES_MGR_STRUCT;
//----------------------------------------------------------------------------
static char const*const ResMgrItemName[RES_MGR_ITEM_AMOUNT] = 
{
    [RES_MGR_ITEM_ISP] = "ISP"
};
//----------------------------------------------------------------------------
#endif

