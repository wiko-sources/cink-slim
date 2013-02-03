#ifndef _MT_ISP_H
#define _MT_ISP_H

#include <linux/ioctl.h>

/*******************************************************************************
*
********************************************************************************/
#define ISP_DEV_MAJOR_NUMBER    251
#define ISP_MAGIC               'k'
/*******************************************************************************
*
********************************************************************************/
#define ISP_INT_EXP_DONE        ((u32)0x1)
#define ISP_INT_IDLE            ((u32)0x1 << 3)
#define ISP_INT_ISP_DONE        ((u32)0x1 << 4)
#define ISP_INT_VSYNC           ((u32)0x1 << 10)
#define ISP_INT_STNR            ((u32)0x1 << 29)
#define ISP_INT_CLEAR_ALL       ((u32)0x1 << 30)
#define ISP_INT_CLEAR_WAIT      ((u32)0x1 << 31)

/*******************************************************************************
*
********************************************************************************/
typedef struct mt_isp_reg_s {
    unsigned long addr;   // register's addr
    unsigned long val;    // register's value
} mt_isp_reg_t;

typedef struct mt_isp_reg_io_s {
    unsigned long data;   // pointer to mt_isp_reg_t
    unsigned long count;  // count
} mt_isp_reg_io_t;

typedef struct mt_isp_wait_irq_s {
    unsigned long mode;     // Mode for wait irq
    unsigned long timeout;  // Timeout for wait irq, uint:ms
} mt_isp_wait_irq_t;
/*******************************************************************************
*
********************************************************************************/
typedef enum
{
    ISP_IOC_CMD_NONE,
    ISP_IOC_CMD_RESET,
    ISP_IOC_CMD_READ_REG,
    ISP_IOC_CMD_WRITE_REG,
    ISP_IOC_CMD_HOLD_REG,
    ISP_IOC_CMD_RUN,
    ISP_IOC_CMD_WAIT_IRQ,
    ISP_IOC_CMD_DUMP_REG,
    ISP_IOC_CMD_DBG_FLAG,
    ISP_IOC_CMD_RESET_BUF,
    ISP_IOC_CMD_ENABLE_CAM_CLOCK,
    ISP_IOC_CMD_ENABLE_VD_PROC,
    ISP_IOC_CMD_RESET_VD_COUNT,
    ISP_IOC_CMD_GET_VD_COUNT
}ISP_IOC_CMD_ENUM;
//IOCTRL(inode * ,file * ,cmd ,arg )
//S means "set through a ptr"
//T means "tell by a arg value"
//G means "get by a ptr"
//Q means "get by return a value"
//X means "switch G and S atomically"
//H means "switch T and Q atomically"
// ioctrl commands
// Reset
#define MT_ISP_IOC_T_RESET              _IO     (ISP_MAGIC, ISP_IOC_CMD_RESET)
// Read register from driver
#define MT_ISP_IOC_G_READ_REG           _IOWR   (ISP_MAGIC, ISP_IOC_CMD_READ_REG,           mt_isp_reg_io_t)
// Write register to driver
#define MT_ISP_IOC_S_WRITE_REG          _IOWR   (ISP_MAGIC, ISP_IOC_CMD_WRITE_REG,          mt_isp_reg_io_t)
// Hold reg write to hw, on/off
#define MT_ISP_IOC_T_HOLD_REG           _IOW    (ISP_MAGIC, ISP_IOC_CMD_HOLD_REG,           u32)
// MT_ISP_IOC_T_RUN : Tell ISP to run/stop
#define MT_ISP_IOC_T_RUN                _IOW    (ISP_MAGIC, ISP_IOC_CMD_RUN,                u32)
// Wait IRQ
#define MT_ISP_IOC_T_WAIT_IRQ           _IOW    (ISP_MAGIC, ISP_IOC_CMD_WAIT_IRQ,           mt_isp_wait_irq_t)
// Dump ISP registers , for debug usage
#define MT_ISP_IOC_T_DUMP_REG           _IO     (ISP_MAGIC, ISP_IOC_CMD_DUMP_REG)
// Dump message level
#define MT_ISP_IOC_T_DBG_FLAG           _IOW    (ISP_MAGIC, ISP_IOC_CMD_DBG_FLAG,           u32)
// Reset SW Buffer
#define MT_ISP_IOC_T_RESET_BUF          _IO     (ISP_MAGIC, ISP_IOC_CMD_RESET_BUF)
// enable cam gate clock
#define MT_ISP_IOC_T_ENABLE_CAM_CLOCK   _IO     (ISP_MAGIC, ISP_IOC_CMD_ENABLE_CAM_CLOCK)
//
#define MT_ISP_IOC_T_ENABLE_VD_PROC     _IOW    (ISP_MAGIC, ISP_IOC_CMD_ENABLE_VD_PROC,     bool)
//
#define MT_ISP_IOC_T_RESET_VD_COUNT     _IO     (ISP_MAGIC, ISP_IOC_CMD_RESET_VD_COUNT)
//
#define MT_ISP_IOC_G_GET_VD_COUNT       _IOR    (ISP_MAGIC, ISP_IOC_CMD_GET_VD_COUNT,       u32)
/*******************************************************************************
*
********************************************************************************/
void mt_isp_mclk_ctrl(int en);
//
#endif

