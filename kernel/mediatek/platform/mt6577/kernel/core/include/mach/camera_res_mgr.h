//----------------------------------------------------------------------------
#ifndef CAMERA_RES_MGR_H
#define CAMERA_RES_MGR_H
//----------------------------------------------------------------------------
#define RES_MGR_DEV_NAME    "camera-resmgr"
#define RES_MGR_MAGIC_NO    'p'
//----------------------------------------------------------------------------
#define RES_MGR_LOG_MSG     0x00000001
#define RES_MGR_LOG_WRN     0x00000002
#define RES_MGR_LOG_ERR     0x00000004
//----------------------------------------------------------------------------
#define RES_MGR_RES_ISP     0x00000001
//----------------------------------------------------------------------------
typedef struct
{
    unsigned long   ResMask;
    unsigned long   PermanentMask;
    unsigned long   Timeout;
}RES_MGR_RES_LOCK_STRUCT;

typedef struct
{
    unsigned long   ResLockMask;
    unsigned long   PermanentMask;
}RES_MGR_RES_CHECK_STRUCT;
//----------------------------------------------------------------------------
typedef enum
{
    RES_MGR_CMD_RES_LOCK,
    RES_MGR_CMD_RES_UNLOCK,
    RES_MGR_CMD_RES_CHECK,
    RES_MGR_CMD_RES_LOG_ENABLE,
    RES_MGR_CMD_RES_LOG_DISABLE
}RES_MGR_CMD_ENUM;
//----------------------------------------------------------------------------
#define RES_MGR_RES_LOCK        _IOW(RES_MGR_MAGIC_NO, RES_MGR_CMD_RES_LOCK,        RES_MGR_RES_LOCK_STRUCT)
#define RES_MGR_RES_UNLOCK      _IOW(RES_MGR_MAGIC_NO, RES_MGR_CMD_RES_UNLOCK,      unsigned long)
#define RES_MGR_RES_CHECK       _IOR(RES_MGR_MAGIC_NO, RES_MGR_CMD_RES_CHECK,       RES_MGR_RES_CHECK_STRUCT)
#define RES_MGR_LOG_ENABLE      _IOR(RES_MGR_MAGIC_NO, RES_MGR_CMD_RES_LOG_ENABLE,  unsigned long)
#define RES_MGR_LOG_DISABLE     _IOR(RES_MGR_MAGIC_NO, RES_MGR_CMD_RES_LOG_DISABLE, unsigned long)
//----------------------------------------------------------------------------
#endif

