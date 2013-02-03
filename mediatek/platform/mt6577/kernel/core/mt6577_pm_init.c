#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/xlog.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "mach/irqs.h"
#include "mach/sync_write.h"
#include "mach/mt6577_reg_base.h"
#include "mach/mt6577_clock_manager.h"
#include "mach/mt6577_typedefs.h"
#include "mach/mt6577_sc.h"
#include "mach/mt6577_boot.h"
#include "mach/mt6577_ca9_power.h"
#include "mach/mt6577_dcm.h"
#include "mach/pmic_mt6329_sw.h"
#include "mach/upmu_common_sw.h"

#define HW_RESV (0xF1019100)

/*********************************************************************
 * FUNCTION DEFINATIONS
 ********************************************************************/
extern CHIP_VER get_chip_ver(void);
extern ssize_t mt6329_read_byte(u8 cmd, u8 *returnData);

kal_uint32 mt6577_get_bus_freq(void)
{
    kal_uint32 fout = 0, fbdiv = 0, fbsel = 0, prediv = 0, postdiv = 0, ckdiv = 0;

    if ((DRV_Reg32(TOP_CKMUXSEL) & 0x3) == 0x0) // Using CLKSQ
    {
        fout = 26000;
    }
    else if ((DRV_Reg32(TOP_CKMUXSEL) & 0x3) == 0x2) // Using WPLL
    {
        fout = 197000;
    }
    else
    {
        fbdiv = (DRV_Reg32(MAINPLL_CON0) & 0x7F00) >> 8;

        fbsel = (DRV_Reg32(MAINPLL_CON0) & 0x0030) >> 4;
        if (fbsel == 0)
            fbsel = 1;
        else if (fbsel == 1)
            fbsel = 2;
        else
            fbsel = 4;

        prediv = (DRV_Reg32(MAINPLL_CON0) & 0x00C0) >> 6;
        if (prediv == 0)
            prediv = 1;
        else if (prediv == 1)
            prediv = 2;
        else
            prediv = 4;

        postdiv = (DRV_Reg32(MAINPLL_CON1) & 0x0030) >> 4;
        if (postdiv == 0)
            postdiv = 1;
        else if (postdiv == 1)
            postdiv = 2;
        else
            postdiv = 4;

        fout = 26000 * (fbdiv + 1) * fbsel / prediv / postdiv; // KHz
    }

    ckdiv = (DRV_Reg32(TOP_CKDIV0) & 0x00000018) >> 3;

    switch (ckdiv)
    {
        case 0:
            fout = fout; // bus clock will not be divided
            break;
        case 1:
            fout = fout / 4; // bus clock will be divided by 4
            break;
        case 2:
            fout = fout / 5; // bus clock will be divided by 5
            break;
        case 3:
            fout = fout / 6; // bus clock will be divided by 6
            break;
        default:
            break;
    }

    return fout;
}
EXPORT_SYMBOL(mt6577_get_bus_freq);

void mt6577_chip_dep_init(void)
{
    scu_control(SCU_IC_STANDBY_ON); // enable GIC auto gated

    scu_control(SCU_STANDBY_ON); // enable SCU auto gated

    mt6577_ca9_power_ctrl(CA9_DYNA_CLK_GATING_ENABLE); // enable dynamic clock gating

    mt6577_l2c_power_ctrl(L2C_STANDBY_ENABLE | L2C_DYNA_CLK_GATING_ENABLE); // enable standby mode and high-level dynamic clock gating

    scu_set_cpu_pwr_status(SCU_CPU_PWR_NORMAL); // current cpu power state
}

void mt6577_pmic_low_power_init(void)
{
    /********************
    * PMIC VPROC setting
    *********************/

    upmu_buck_vosel_srclken_0(BUCK_VPROC, 0x08); // VPROC 0.9V in sleep mode

    if (get_chip_ver() >= CHIP_6577_E1)
    {
        upmu_buck_vosel_dvs_00(BUCK_VPROC, 0x08); // 0.900V DVS_VOL_00
        upmu_buck_vosel_dvs_01(BUCK_VPROC, 0x0F); // 1.075V DVS_VOL_01
        upmu_buck_vosel_dvs_10(BUCK_VPROC, 0x13); // 1.175V DVS_VOL_10
        if ((DRV_Reg32(HW_RESV) & (0x1 << 23)) && ((DRV_Reg32(HW_RESV) & (0x1 << 20)) == 0))
        {
            upmu_buck_vosel_dvs_11(BUCK_VPROC, 0x19); // P.K. DVS_VOL_11
        }
        else
        {
            if ((DRV_Reg32(HW_RESV) & (0x1 << 21)))
            {
                upmu_buck_vosel_dvs_11(BUCK_VPROC, 0x19); // P.K. DVS_VOL_11
            }
            else if ((DRV_Reg32(HW_RESV) & (0x1 << 22)))
            {
                upmu_buck_vosel_dvs_11(BUCK_VPROC, 0x18); // P.K. DVS_VOL_11
            }
            else
            {
                upmu_buck_vosel_dvs_11(BUCK_VPROC, 0x17); // P.K. DVS_VOL_11
            }
        }
    }
    else if (get_chip_ver() >= CHIP_6575_E2)
    {
        upmu_buck_vosel_dvs_00(BUCK_VPROC, 0x08); // 0.900V DVS_VOL_00
        upmu_buck_vosel_dvs_01(BUCK_VPROC, 0x0F); // 1.075V DVS_VOL_01
        upmu_buck_vosel_dvs_10(BUCK_VPROC, 0x13); // 1.175V DVS_VOL_10
        upmu_buck_vosel_dvs_11(BUCK_VPROC, 0x16); // 1.250V DVS_VOL_11
    }
    else if (get_chip_ver() >= CHIP_6575_E1)
    {
        upmu_buck_vosel_dvs_00(BUCK_VPROC, 0x13); // 1.175V DVS_VOL_00
        upmu_buck_vosel_dvs_01(BUCK_VPROC, 0x16); // 1.250V DVS_VOL_01
        upmu_buck_vosel_dvs_10(BUCK_VPROC, 0x13); // 1.175V DVS_VOL_10
        upmu_buck_vosel_dvs_11(BUCK_VPROC, 0x16); // 1.250V DVS_VOL_11
    }
    else
    {
        upmu_buck_vosel_dvs_00(BUCK_VPROC, 0x16); // 1.250V DVS_VOL_00
        upmu_buck_vosel_dvs_01(BUCK_VPROC, 0x16); // 1.250V DVS_VOL_01
        upmu_buck_vosel_dvs_10(BUCK_VPROC, 0x16); // 1.250V DVS_VOL_10
        upmu_buck_vosel_dvs_11(BUCK_VPROC, 0x16); // 1.250V DVS_VOL_11
    }

    DRV_WriteReg32(SC_AP_DVFS_CON, ((DRV_Reg32(SC_AP_DVFS_CON) & 0xFFFFFFFC) | 0x03)); // set cpu to top voltage

    upmu_buck_ctrl(BUCK_VPROC, 0x3); // VPROC controlled by SRCLKEN and AP_DVFS_CON1/0

    /********************
    * PMIC VCORE setting
    *********************/

    if ((DRV_Reg32(HW_RESV) & (0x1 << 19)))
    {
        upmu_buck_vosel(BUCK_VCORE, UPMU_VOLT_0_8_0_0_V); // VCORE 0.8V in sleep mode
    }
    else
    {
        upmu_buck_vosel(BUCK_VCORE, UPMU_VOLT_0_9_0_0_V); // VCORE 0.9V in sleep mode
    }

    /********************
    * PMIC Other setting
    *********************/

    pmic_config_interface(0x8B, 0x08, 0x1F, 0x0); // VM12_INT 0.9V in sleep mode
    pmic_config_interface(0x8C, 0x10, 0x1F, 0x0); // VM12_INT_LOW_BOUND
    pmic_config_interface(0x8F, 0x01, 0x01, 0x4); // VM12_INT Tracking VPROC
    pmic_config_interface(0x90, 0x01, 0x01, 0x0); // VM12_INT_LP_SEL HW control

    pmic_config_interface(0x85, 0x01, 0x01, 0x0); // VM12_1_LP_SEL HW control
    pmic_config_interface(0x89, 0x01, 0x01, 0x0); // VM12_2_LP_SEL HW control

    pmic_config_interface(0xA9, 0x01, 0x01, 0x0); // VMC_LP_SEL HW control
    pmic_config_interface(0xAD, 0x01, 0x01, 0x0); // VMCH_LP_SEL HW control

    pmic_config_interface(0xC6, 0x01, 0x01, 0x0); // VA1_LP_SEL HW control

    pmic_config_interface(0xC1, 0x01, 0x01, 0x1); // VTCXO_ON_CTRL HW control

    pmic_config_interface(0x4F, 0x01, 0x01, 0x6); // BUCK clock keep 2MHz select
    pmic_config_interface(0x4F, 0x01, 0x01, 0x7); // OSC10M and 2M auto select function enable
}

void mt6577_pmic_low_power_reg_dump(void)
{
    kal_uint8 i, value;

    for (i = 0x44; i <= 0x4F; i++)
    {
        mt6329_read_byte(i, &value);
        xlog_printk(ANDROID_LOG_INFO, "Power/PM_INIT", "[pmic_dump_register] Reg[0x%X]=0x%X\n", i, value);
    }

    for (i = 0x57; i <= 0x59; i++)
    {
        mt6329_read_byte(i, &value);
        xlog_printk(ANDROID_LOG_INFO, "Power/PM_INIT", "[pmic_dump_register] Reg[0x%X]=0x%X\n", i, value);
    }

    for (i = 0x85; i <= 0x90; i++)
    {
        mt6329_read_byte(i, &value);
        xlog_printk(ANDROID_LOG_INFO, "Power/PM_INIT", "[pmic_dump_register] Reg[0x%X]=0x%X\n", i, value);
    }

    mt6329_read_byte(0xA9, &value);
    xlog_printk(ANDROID_LOG_INFO, "Power/PM_INIT", "[pmic_dump_register] Reg[0x%X]=0x%X\n", 0xA9, value);

    mt6329_read_byte(0xAD, &value);
    xlog_printk(ANDROID_LOG_INFO, "Power/PM_INIT", "[pmic_dump_register] Reg[0x%X]=0x%X\n", 0xAD, value);

    mt6329_read_byte(0xC1, &value);
    xlog_printk(ANDROID_LOG_INFO, "Power/PM_INIT", "[pmic_dump_register] Reg[0x%X]=0x%X\n", 0xC1, value);

    mt6329_read_byte(0xC6, &value);
    xlog_printk(ANDROID_LOG_INFO, "Power/PM_INIT", "[pmic_dump_register] Reg[0x%X]=0x%X\n", 0xC6, value);

    mt6329_read_byte(0xD1, &value);
    xlog_printk(ANDROID_LOG_INFO, "Power/PM_INIT", "[pmic_dump_register] Reg[0x%X]=0x%X\n", 0xD1, value);
}

static int pmic_reg_dump_read(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    mt6577_pmic_low_power_reg_dump();
    return 0;
}

void mt6577_power_management_init(void)
{
    struct proc_dir_entry *entry = NULL;
    struct proc_dir_entry *pm_init_dir = NULL;

    #if !defined (CONFIG_MT6577_FPGA)
    xlog_printk(ANDROID_LOG_INFO, "Power/PM_INIT", "Chip Version = 0x%x, Bus Frequency = %d KHz\n", get_chip_ver(), mt6577_get_bus_freq());

    mt6577_chip_dep_init(); // set specific chip setting

    slp_mod_init(); // sleep controller init

    mt6577_clk_mgr_init(); // clock manager init, including clock gating init

    mt6577_pm_log_init(); // power management log init

    mt6577_dcm_init(); // dynamic clock management init

    pm_init_dir = proc_mkdir("pm_init", NULL);
    if (!pm_init_dir)
    {
        pr_err("[%s]: mkdir /proc/pm_init failed\n", __FUNCTION__);
    }
    else
    {
        entry = create_proc_entry("pmic_reg_dump", S_IRUGO, pm_init_dir);
        if (entry)
        {
            entry->read_proc = pmic_reg_dump_read;
        }
    }
    #endif
}