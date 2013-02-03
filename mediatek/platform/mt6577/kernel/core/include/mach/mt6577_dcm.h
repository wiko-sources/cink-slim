#ifndef _MT6577_DCM_H
#define _MT6577_DCM_H

#include <linux/module.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "mach/sync_write.h"
#include "mach/mt6577_reg_base.h"
#include "mach/mt6577_typedefs.h"
#include "mach/mt6577_clock_manager.h"

/*******************************************************************************
 * DCM Register Definition
 ******************************************************************************/
#define TOP_DCMCTL              (INFRA_SYS_CFG_BASE + 0x010)
#define TOP_MISC                (INFRA_SYS_CFG_BASE + 0x014)
#define TOP_CA9DCMFSEL          (INFRA_SYS_CFG_BASE + 0x018)

#define INFRA_DCMCTL            (INFRA_SYS_CFG_BASE + 0x050)
#define INFRA_DCMDBC            (INFRA_SYS_CFG_BASE + 0x054)
#define INFRA_DCMFSEL           (INFRA_SYS_CFG_BASE + 0x058)

#define INFRA_MISC              (INFRA_SYS_CFG_BASE + 0xF00)

#define PERI_GLOBALCON_DCMCTL   (PERICFG_BASE + 0x020)
#define PERI_GLOBALCON_DCMDBC   (PERICFG_BASE + 0x024)
#define PERI_GLOBALCON_DCMFSEL  (PERICFG_BASE + 0x028)

#define MMSYS2_DCMCTL           (MMSYS2_CONFG_BASE + 0x320)
#define MMSYS2_DCMDBC           (MMSYS2_CONFG_BASE + 0x324)
#define MMSYS2_DCMFSEL          (MMSYS2_CONFG_BASE + 0x328)

#define SMI_LARB0_CON           (SMI_LARB0_BASE + 0x010)
#define SMI_LARB0_CON_SET       (SMI_LARB0_BASE + 0x014)
#define SMI_LARB0_CON_RESET     (SMI_LARB0_BASE + 0x018)
#define SMI_LARB1_CON           (SMI_LARB1_BASE + 0x010)
#define SMI_LARB1_CON_SET       (SMI_LARB1_BASE + 0x014)
#define SMI_LARB1_CON_RESET     (SMI_LARB1_BASE + 0x018)
#define SMI_LARB2_CON           (SMI_LARB2_BASE + 0x010)
#define SMI_LARB2_CON_SET       (SMI_LARB2_BASE + 0x014)
#define SMI_LARB2_CON_RESET     (SMI_LARB2_BASE + 0x018)
#define SMI_LARB3_CON           (SMI_LARB3_BASE + 0x010)
#define SMI_LARB3_CON_SET       (SMI_LARB3_BASE + 0x014)
#define SMI_LARB3_CON_RESET     (SMI_LARB3_BASE + 0x018)

#define CA9_DCM     0x1
#define TOPAXI_DCM  0x2
#define EMI_DCM     0x3
#define PERI_DCM    0x4
#define MM1_DCM     0x5
#define MM2_DCM     0x6
#define ALL_DCM     0x7

#define AHB0_OFF_AHB1_OFF   ((0 << 10) | (0 << 9))
#define AHB0_ON_AHB1_OFF    ((0 << 10) | (1 << 9))
#define AHB0_OFF_AHB1_ON    ((1 << 10) | (0 << 9))
#define AHB0_ON_AHB1_ON     ((1 << 10) | (1 << 9)) 

// DCM API
extern void dcm_enable_all(void);
extern void dcm_disable_all(void);

extern void dcm_enable(unsigned short type);
extern void dcm_disable(unsigned short type);

extern void enable_dpidle_by_bit(enum mt65xx_clock_id id);
extern void disable_dpidle_by_bit(enum mt65xx_clock_id id);

extern void enable_hispeed_by_bit(enum mt65xx_clock_id id);
extern void disable_hispeed_by_bit(enum mt65xx_clock_id id);

extern void mt6577_dcm_init(void);

#endif
