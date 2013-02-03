#ifndef _MT6577_CPE_H
#define _MT6577_CPE_H

#include <linux/module.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "mach/sync_write.h"
#include "mach/mt6577_reg_base.h"
#include "mach/mt6577_typedefs.h"

/*******************************************************************************
 * CPM Register Definition
 ******************************************************************************/
#define AP_ABIST_FQMTR_CON0         (APMIXED_BASE + 0x400)
#define AP_ABIST_FQMTR_CON1         (APMIXED_BASE + 0x404)
#define AP_ABIST_FQMTR_CON3         (APMIXED_BASE + 0x40C)
#define AP_ABIST_FQMTR_DATA         (APMIXED_BASE + 0x410)
 
#define CA9_ROSC_CON0               (APMIXED_BASE + 0x900)
#define CA9_ROSC_CON1               (APMIXED_BASE + 0x904)
#define CA9_ROSC_CON2               (APMIXED_BASE + 0x908)
#define CA9_ROSC_CON3               (APMIXED_BASE + 0x90C)
#define CA9_ROSC_CON4               (APMIXED_BASE + 0x910)
#define CA9_ROSC_CON5               (APMIXED_BASE + 0x914)
#define CA9_ROSC_CON6               (APMIXED_BASE + 0x918)
#define CA9_ROSC_CON_STATUS         (APMIXED_BASE + 0x91C)

#define MD_ROSC_CON0                (APMIXED_BASE + 0x920)
#define MD_ROSC_CON3                (APMIXED_BASE + 0x92C)
#define MD_ROSC_CON4                (APMIXED_BASE + 0x930)
#define MD_ROSC_CON5                (APMIXED_BASE + 0x934)

#define DM_ROSC_CON0                (APMIXED_BASE + 0x980)
#define DM_CORE_PULSE_WIDTH_IN_L    (APMIXED_BASE + 0x984)
#define DM_CORE_PULSE_WIDTH_IN_H    (APMIXED_BASE + 0x988)
#define DM_FSM_WAIT_COUNTER_MAX     (APMIXED_BASE + 0x98C)
#define DM_ROSC_CON1                (APMIXED_BASE + 0x990)
#define DM_ROSC_STAT0               (APMIXED_BASE + 0x994)
#define DM_CORE_ROSC_NUMBER_1_L     (APMIXED_BASE + 0x9A0)
#define DM_CORE_ROSC_NUMBER_1_H     (APMIXED_BASE + 0x9A4)
#define DM_CORE_ROSC_NUMBER_2_L     (APMIXED_BASE + 0x9A8)
#define DM_CORE_ROSC_NUMBER_2_H     (APMIXED_BASE + 0x9AC)

#define CA9_ROSC_GATED_CLK_CTRL     (APMIXED_BASE + 0x9B0)
#define CA9_ROSC_MON_CTRL_0         (APMIXED_BASE + 0x9B4)
#define CA9_ROSC_MON_CTRL_1         (APMIXED_BASE + 0x9B8)
#define CA9_ROSC_MON_COUNT_MIN      (APMIXED_BASE + 0x9BC)
#define CA9_DM_DATA_MIN_1_L         (APMIXED_BASE + 0x9C0)
#define CA9_DM_DATA_MIN_1_H         (APMIXED_BASE + 0x9C4)
#define CA9_DM_DATA_MIN_2_L         (APMIXED_BASE + 0x9C8)
#define CA9_DM_DATA_MIN_2_H         (APMIXED_BASE + 0x9CC)

/*******************************************************************************
 * CPM Register Mask Definition
 ******************************************************************************/
#define CA9_FQMTR_BUSY              (1 << 0)
#define CA9_FQMTR_OVERFLOW          (1 << 1)

extern kal_uint16 mtk_cpe_one_shot_handler(kal_uint16 path1, kal_uint16 path2, kal_uint16 path3);

#endif