#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/suspend.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/console.h>
#include <linux/xlog.h>
#include <linux/aee.h>

#include <asm/io.h>
#include <mach/mt6577_boot.h>
#include <mach/mt6577_sc.h>
#include <mach/mt6577_clock_manager.h>
#include <mach/mt6577_dcm.h>
#include <mach/mtk_rtc.h>
#include <mach/mt6577_gpio.h>
#include <mach/pmic_mt6329_sw.h>
#include <mach/upmu_common_sw.h>

#include <board-custom.h>
#include <mt6577_battery.h>

#define XLOG_MYTAG		"Power/Sleep"

#define slp_read16(addr)	(*(volatile u16 *)(addr))
#define slp_write16(addr, val)	(*(volatile u16 *)(addr) = (u16)(val))

#define slp_read32(addr)	(*(volatile u32 *)(addr))
#define slp_write32(addr, val)	(*(volatile u32 *)(addr) = (u32)(val))

#define slp_write_sync()	\
do {				\
	dsb();			\
	outer_sync();		\
} while (0)

#define slp_xverb(fmt, args...)		\
	xlog_printk(ANDROID_LOG_VERBOSE, XLOG_MYTAG, fmt, ##args)

#define slp_xinfo(fmt, args...)		\
	xlog_printk(ANDROID_LOG_INFO, XLOG_MYTAG, fmt, ##args)

#define slp_xerror(fmt, args...)	\
	xlog_printk(ANDROID_LOG_ERROR, XLOG_MYTAG, fmt, ##args)

extern void pmic_status_for_sleep(void);

static wake_reason_t slp_wake_reason;

/*
 * 0: CPU standby + AP sleep => legacy sleep mode
 * 1: CPU dormant (L1/L2 sleep) + AP sleep
 * 2: CPU shutdown (L1/L2 power down) + AP sleep
 */
static unsigned int slp_pwr_level = 2;

static int slp_dump_pmic = 0;
static int slp_dump_gpio = 0;
static int slp_dump_regs = 1;

static unsigned long RM_CLK_SETTLE = 0;
static unsigned long RM_TMR_SSTA = 0;

void slp_debug_for_audio(void)
{
	aee_sram_printk("RM_CLK_SETTLE = 0x%x\n", RM_CLK_SETTLE ? slp_read32(RM_CLK_SETTLE) : 0);
}

static int slp_md_sta_show(struct seq_file *m, void *v)
{
	u32 ssta, mdssta = 0;
	int i;

	if (!RM_TMR_SSTA)
		return -ENOMEM;

	sc_request_mdbus_clk(SC_MDBUS_USER_SLEEP);
	ssta = slp_read32(RM_TMR_SSTA);
	sc_unrequest_mdbus_clk(SC_MDBUS_USER_SLEEP);

	for (i = 0; i < 4; i++) {
		/* check if timer is in Pause or Pre-Pause state */
		if ((ssta & 0xf) == 0x2 || (ssta & 0xf) == 0x4)
			mdssta |= (1U << (i * 4));
		ssta >>= 4;
	}

	seq_printf(m, "0x%x\n", mdssta);

	return 0;
}

static int slp_md_sta_open(struct inode *inode, struct file *file)
{
	return single_open(file, slp_md_sta_show, NULL);
}

static struct file_operations slp_md_sta_fops = {
	.open		= slp_md_sta_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static void slp_dump_pm_regs(void)
{
	/* TOPSM registers */
	sc_request_mdbus_clk(SC_MDBUS_USER_SLEEP);
	if (RM_CLK_SETTLE)
		slp_xverb("RM_CLK_SETTLE   0x%x = 0x%x\n", 0xd1090000, slp_read32(RM_CLK_SETTLE));
	sc_unrequest_mdbus_clk(SC_MDBUS_USER_SLEEP);

	/* ACP related registers */
	slp_xverb("PERIAXIBUS_CTL1 0x%x = 0x%x\n", 0xf1000200, slp_read32(0xf1000200));
	slp_xverb("PG_DMA_CACHE    0x%x = 0x%x\n", 0xf1000100, slp_read32(0xf1000100));
	slp_xverb("VDEC_DMACFG     0x%x = 0x%x\n", 0xf2084024, slp_read32(0xf2084024));
	slp_xverb("VENC_MC_CTRL    0x%x = 0x%x\n", 0xf208506c, slp_read32(0xf208506c));

	/* PLL registers */
	slp_xverb("PLL_CON1        0x%x = 0x%x\n", PLL_CON1   , slp_read16(PLL_CON1));
	slp_xverb("PLL_CON2        0x%x = 0x%x\n", PLL_CON2   , slp_read16(PLL_CON2));
	slp_xverb("IPLL_CON0       0x%x = 0x%x\n", IPLL_CON0  , slp_read16(IPLL_CON0));
	slp_xverb("UPLL_CON0       0x%x = 0x%x\n", UPLL_CON0  , slp_read16(UPLL_CON0));
	slp_xverb("MDPLL_CON0      0x%x = 0x%x\n", MDPLL_CON0 , slp_read16(MDPLL_CON0));
	slp_xverb("WPLL_CON0       0x%x = 0x%x\n", WPLL_CON0  , slp_read16(WPLL_CON0));
	slp_xverb("AUDPLL_CON0     0x%x = 0x%x\n", AUDPLL_CON0, slp_read16(AUDPLL_CON0));

	/* TOPCKGEN/PERICFG/MMSYS/MCUSYS registers */
	slp_xverb("TOP_CKMUXSEL    0x%x = 0x%x\n", TOP_CKMUXSEL       , slp_read32(TOP_CKMUXSEL));
	slp_xverb("TOP_DCMCTL      0x%x = 0x%x\n", TOP_DCMCTL         , slp_read32(TOP_DCMCTL));
	slp_xverb("TOP_MISC        0x%x = 0x%x\n", TOP_MISC           , slp_read32(TOP_MISC));
	slp_xverb("GLOBALCON_PDN0  0x%x = 0x%x\n", PERI_GLOBALCON_PDN0, slp_read32(PERI_GLOBALCON_PDN0));
	slp_xverb("GLOBALCON_PDN1  0x%x = 0x%x\n", PERI_GLOBALCON_PDN1, slp_read32(PERI_GLOBALCON_PDN1));
	slp_xverb("GLOBALCON_PDN2  0x%x = 0x%x\n", PERI_GLOBALCON_PDN2, slp_read32(PERI_GLOBALCON_PDN2));
	slp_xverb("GLOBALCON_PDN3  0x%x = 0x%x\n", PERI_GLOBALCON_PDN3, slp_read32(PERI_GLOBALCON_PDN3));
	slp_xverb("MMSYS1_CG_CON0  0x%x = 0x%x\n", MMSYS1_CG_CON0     , slp_read32(MMSYS1_CG_CON0));
	slp_xverb("MMSYS1_CG_CON1  0x%x = 0x%x\n", MMSYS1_CG_CON1     , slp_read32(MMSYS1_CG_CON1));
	slp_xverb("MMSYS1_CG_CON2  0x%x = 0x%x\n", MMSYS1_CG_CON2     , slp_read32(MMSYS1_CG_CON2));
	slp_xverb("MMSYS2_CG_CON   0x%x = 0x%x\n", MMSYS2_CG_CON      , slp_read32(MMSYS2_CG_CON));
	slp_xverb("PWR_CTL1        0x%x = 0x%x\n", 0xf0009024         , slp_read32(0xf0009024));
	slp_xverb("PWR_MON         0x%x = 0x%x\n", 0xf00090a8         , slp_read32(0xf00090a8));

	/* SLPCTRL registers */
	slp_xverb("SC_CLK_SETTLE   0x%x = 0x%x\n", SC_CLK_SETTLE  , slp_read32(SC_CLK_SETTLE));
	slp_xverb("SC_PWR_SETTLE   0x%x = 0x%x\n", SC_PWR_SETTLE  , slp_read32(SC_PWR_SETTLE));
	slp_xverb("SC_PWR_CON0     0x%x = 0x%x\n", SC_PWR_CON0    , slp_read32(SC_PWR_CON0));
	slp_xverb("SC_PWR_CON1     0x%x = 0x%x\n", SC_PWR_CON1    , slp_read32(SC_PWR_CON1));
	slp_xverb("SC_PWR_CON2     0x%x = 0x%x\n", SC_PWR_CON2    , slp_read32(SC_PWR_CON2));
	slp_xverb("SC_PWR_CON3     0x%x = 0x%x\n", SC_PWR_CON3    , slp_read32(SC_PWR_CON3));
	slp_xverb("SC_PWR_CON4     0x%x = 0x%x\n", SC_PWR_CON4    , slp_read32(SC_PWR_CON4));
	slp_xverb("SC_PWR_CON5     0x%x = 0x%x\n", SC_PWR_CON5    , slp_read32(SC_PWR_CON5));
	slp_xverb("SC_PWR_CON6     0x%x = 0x%x\n", SC_PWR_CON6    , slp_read32(SC_PWR_CON6));
	slp_xverb("SC_PWR_CON7     0x%x = 0x%x\n", SC_PWR_CON7    , slp_read32(SC_PWR_CON7));
	slp_xverb("SC_PWR_CON8     0x%x = 0x%x\n", SC_PWR_CON8    , slp_read32(SC_PWR_CON8));
	slp_xverb("SC_PWR_CON9     0x%x = 0x%x\n", SC_PWR_CON9    , slp_read32(SC_PWR_CON9));
	slp_xverb("SC_CLK_CON      0x%x = 0x%x\n", SC_CLK_CON     , slp_read32(SC_CLK_CON));
	slp_xverb("SC_MD_CLK_CON   0x%x = 0x%x\n", SC_MD_CLK_CON  , slp_read32(SC_MD_CLK_CON));
	slp_xverb("SC_MD_INTF_CON  0x%x = 0x%x\n", SC_MD_INTF_CON , slp_read32(SC_MD_INTF_CON));
	slp_xverb("SC_MD_INTF_STS  0x%x = 0x%x\n", SC_MD_INTF_STS , slp_read32(SC_MD_INTF_STS));
	slp_xverb("SC_TMR_PWR      0x%x = 0x%x\n", SC_TMR_PWR     , slp_read32(SC_TMR_PWR));
	slp_xverb("SC_PERI_CON     0x%x = 0x%x\n", SC_PERI_CON    , slp_read32(SC_PERI_CON));
	slp_xverb("SC_PWR_STA      0x%x = 0x%x\n", SC_PWR_STA     , slp_read32(SC_PWR_STA));
	slp_xverb("SC_APMCU_PWRCTL 0x%x = 0x%x\n", SC_APMCU_PWRCTL, slp_read32(SC_APMCU_PWRCTL));
	slp_xverb("SC_AP_DVFS_CON  0x%x = 0x%x\n", SC_AP_DVFS_CON , slp_read32(SC_AP_DVFS_CON));
}

static int slp_suspend_ops_valid(suspend_state_t pm_state)
{
	return pm_state == PM_SUSPEND_MEM;
}

static int slp_suspend_ops_begin(suspend_state_t state)
{
	/* legacy log */
	slp_xinfo("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	slp_xinfo("_Chip_pm_begin (%u) @@@@@@@@@@@@@@@@@@\n", slp_pwr_level);
	slp_xinfo(" @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

	slp_wake_reason = WR_NONE;

	return 0;
}

static int slp_suspend_ops_prepare(void)
{
	/* legacy log */
	aee_sram_printk("_Chip_pm_prepare\n");
	slp_xinfo("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	slp_xinfo("_Chip_pm_prepare @@@@@@@@@@@@@@@@@@@@\n");
	slp_xinfo(" @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

	if (slp_dump_pmic)
		pmic_status_for_sleep();

	/* ccci callback is removed and moved to ccci_helper module */

	return 0;
}

static int slp_suspend_ops_enter(suspend_state_t state)
{
	u32 topmisc;
	unsigned int pwrlevel;

	/* legacy log */
	aee_sram_printk("_Chip_pm_enter\n");
	slp_xinfo("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	slp_xinfo("_Chip_pm_enter @@@@@@@@@@@@@@@@@@@@@@\n");
	slp_xinfo(" @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

	if (slp_dump_gpio)
		gpio_dump_regs();

	/* keep CA9 clock frequency when WFI to sleep */
	topmisc = slp_read32(TOP_MISC);
	slp_write32(TOP_MISC, topmisc & ~(1U << 0));
	slp_write_sync();

	if (slp_dump_regs)
		slp_dump_pm_regs();

	rtc_disable_writeif();

	pwrlevel = slp_pwr_level;
	if ((pwrlevel != 0) && (slp_read32(0xf0009024) & 0x8000)) {
		slp_xerror("!!! WILL NOT POWER DOWN CPUSYS DUE TO CPU1 ON !!!\n");
		pwrlevel = 0;
	}
	slp_wake_reason = sc_go_to_sleep(pwrlevel);

	rtc_enable_writeif();

	/* restore TOP_MISC */
	slp_write32(TOP_MISC, topmisc);
	slp_write_sync();

	return 0;
}

static void slp_suspend_ops_finish(void)
{
	/* legacy log */
	aee_sram_printk("_Chip_pm_finish\n");
	slp_xinfo("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	slp_xinfo("_Chip_pm_finish @@@@@@@@@@@@@@@@@@@@@\n");
	slp_xinfo(" @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

	/* debug help */
	slp_xinfo("Battery_Voltage = %lu\n", BAT_Get_Battery_Voltage(0));
}

static void slp_suspend_ops_end(void)
{
	/* legacy log */
	slp_xinfo("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	slp_xinfo("_Chip_pm_end @@@@@@@@@@@@@@@@@@@@@@@@\n");
	slp_xinfo(" @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
}

static struct platform_suspend_ops slp_suspend_ops = {
	.valid		= slp_suspend_ops_valid,
	.begin		= slp_suspend_ops_begin,
	.prepare	= slp_suspend_ops_prepare,
	.enter		= slp_suspend_ops_enter,
	.finish		= slp_suspend_ops_finish,
	.end		= slp_suspend_ops_end,
};

#if 0
void slp_pmic_init(void)
{
	/* VPROC = 0.9V in sleep mode */
	upmu_buck_vosel_srclken_0(BUCK_VPROC, 0x08);
	upmu_buck_ctrl(BUCK_VPROC, 0x3);

	/* VCORE = 0.9V in sleep mode */
	upmu_buck_vosel(BUCK_VCORE, UPMU_VOLT_0_9_0_0_V);

	/* VM12_INT = 0.9V in sleep mode */
	pmic_config_interface(0x8b, 0x08, 0x1f, 0x0);
	pmic_config_interface(0x8f, 0x1, 0x1, 0x4);
}
#endif

void slp_mod_init(void)
{
	sc_mod_init();

	suspend_set_ops(&slp_suspend_ops);

	RM_CLK_SETTLE = (unsigned long)ioremap_nocache(0xd1090000, 4);
	if (!RM_CLK_SETTLE)
		slp_xerror("ioremap RM_CLK_SETTLE failed\n");

	RM_TMR_SSTA = (unsigned long)ioremap_nocache(0xd1090040, 4);
	if (!RM_TMR_SSTA)
		slp_xerror("ioremap RM_TMR_SSTA failed\n");
	proc_create_data("slp_md_sta", 0444, NULL, &slp_md_sta_fops, NULL);

	//console_suspend_enabled = 0;
}

module_param(slp_pwr_level, uint, 0644);

module_param(slp_dump_pmic, bool, 0644);
module_param(slp_dump_gpio, bool, 0644);
module_param(slp_dump_regs, bool, 0644);

MODULE_AUTHOR("Terry Chang <terry.chang@mediatek.com>");
MODULE_DESCRIPTION("MT6577 Sleep Driver v3.6");
