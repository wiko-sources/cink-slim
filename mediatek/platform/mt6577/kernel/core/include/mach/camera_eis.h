#ifndef _MT_EIS_H
#define _MT_EIS_H

#include <linux/ioctl.h>

/*******************************************************************************
*
********************************************************************************/
#define EIS_MAGIC               'e'

/*******************************************************************************
*
********************************************************************************/
typedef struct mt_eis_reg_s {
    unsigned int val[21];    // register's value
} mt_eis_reg_t;

/*******************************************************************************
*
********************************************************************************/
//IOCTRL(inode * ,file * ,cmd ,arg )
//S means "set through a ptr"
//T means "tell by a arg value"
//G means "get by a ptr"
//Q means "get by return a value"
//X means "switch G and S atomically"
//H means "switch T and Q atomically"
// ioctrl commands

#define MT_EIS_IOC_T_DUMP_REG   _IOR(EIS_MAGIC, 1, mt_eis_reg_t)
#define MT_EIS_IOC_T_CLK_ONOFF  _IOW(EIS_MAGIC, 2, unsigned long)

//
#endif

