#include <linux/io.h>
#include <linux/module.h>

#include "mach/mt6577_reg_base.h"
#include "mach/sync_write.h"
#include "mach/mt6577_ca9_power.h"

extern void arm_ca9_dynamic_gating_enable(unsigned int n);
#define SCU_CTRL        (SCU_BASE + 0x00)
#define SCU_CPU_PWR     (SCU_BASE + 0x08)
#define L2C_PWR_CTRL    (PL310_BASE + 0xF80)

static DEFINE_SPINLOCK(scu_lock);

int scu_control(const SCU_CONTROL ctrl)
{
    unsigned int iVal;
    int iRet = 0;

    spin_lock(&scu_lock);

    iVal = readl(SCU_CTRL);

    switch (ctrl) {
    case SCU_IC_STANDBY_ON:
        iVal |= 0x40;
        break;

    case SCU_IC_STANDBY_OFF:
        iVal &= ~0x40;
        break;

    case SCU_STANDBY_ON:
        iVal |= 0x20;
        break;

    case SCU_STANDBY_OFF:
        iVal &= ~0x20;
        break;

    default:
        iRet = -1;
    }

    if (iRet >= 0) {
        mt65xx_reg_sync_writel(iVal, SCU_CTRL);
    }

    spin_unlock(&scu_lock);

    return iRet;
}

int scu_set_cpu_pwr_status(const SCU_CPU_PWR_STATE state)
{
    unsigned int iVal;
    int iRet = 0;
    spin_lock(&scu_lock);

    iVal = readl(SCU_CPU_PWR) & ~0x3;

    switch (state) {
    case SCU_CPU_PWR_NORMAL:
        iVal |= SCU_CPU_PWR_NORMAL;
        break;

    case SCU_CPU_PWR_DORMANT:
        iVal |= SCU_CPU_PWR_DORMANT;
        break;

    case SCU_CPU_PWR_OFF:
        iVal |= SCU_CPU_PWR_OFF;
        break;

    default:
        iRet = -1;
    }

    if (iRet >= 0) {
        mt65xx_reg_sync_writel(iVal, SCU_CPU_PWR);
    }

    spin_unlock(&scu_lock);

    return iRet;
}

int mt6577_ca9_power_ctrl(const CA9_CLK_GATING_CTRL ctrl)
{
    if (ctrl != CA9_DYNA_CLK_GATING_DISALBE && ctrl != CA9_DYNA_CLK_GATING_ENABLE) {
        return -1;  // invalid parameter
    }

    arm_ca9_dynamic_gating_enable(ctrl);

    return 0;
}

int mt6577_l2c_power_ctrl(const L2C_POWER_CTRL ctrl)
{
    unsigned int iVal = readl(L2C_PWR_CTRL) & ~0x3;

    if (ctrl & ~(L2C_STANDBY_ENABLE | L2C_DYNA_CLK_GATING_ENABLE)) {
        return -1;  // invalid parameter
    }

    if (ctrl & L2C_STANDBY_ENABLE) {
        iVal |= 0x1;
    }

    if (ctrl & L2C_DYNA_CLK_GATING_ENABLE) {
        iVal |= 0x2;
    }

    mt65xx_reg_sync_writel(iVal, L2C_PWR_CTRL);

    return 0;
}
