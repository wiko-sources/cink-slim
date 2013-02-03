#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/xlog.h>
#include <linux/aee.h>

#include <mach/mt6577_boot.h>
#include <mach/irqs.h>
#include <mach/mt6577_sc.h>
#include <mach/mt6577_dormant.h>
#include <mach/mt6577_clock_manager.h>
#include <mach/mtk_ccci_helper.h>

#include <board-custom.h>
#include <mt6577_battery.h>

#define XLOG_MYTAG		"Power/Sleep"
#define SC_NAME			"mt6577-sc"

#define SC_CLK_CON_SRCLKEN	(1U << 1)
#define SC_CLK_CON_BYPASSDBG	(1U << 8)
#define SC_CLK_CON_CLKREQ2MD	(1U << 25)
#define SC_CLK_CON_3GPLLON	(1U << 31)

#define SC_TIMER_CON_EN		(1U << 0)
#define SC_TIMER_CON_DOWN	(1U << 1)
#define SC_TIMER_CON_DBG	(1U << 2)

#define SC_TIMER_CMD_PAUSE	(1U << 0)
#define SC_TIMER_CMD_WR		(1U << 2)
#define SC_TIMER_CMD_CNTWR	(1U << 13)
#define SC_TIMER_CMD_CONWR	(1U << 15)
#define SC_TIMER_CMD_KEY	(0x6575 << 16)

#define SC_TIMER_STA_CMDCPL	(1U << 1)

/* typical values */
#ifdef MODEM_UMTS_TDD128_MODE
#define SC_SYSCLK_SETTLE	128	/* T 32K */
#else
#define SC_SYSCLK_SETTLE	160	/* T 32K */
#endif
#define SC_PLL_SETTLE		3	/* T 32K */
#define SC_PLL2_SETTLE		3	/* T 32K */

#define SC_CLK_SETTLE_TYPICAL	((SC_PLL2_SETTLE << 24) |	\
                             	 (SC_PLL_SETTLE << 16) |	\
                             	 SC_SYSCLK_SETTLE)

#define SC_CLK_SETTLE_26MON	((SC_PLL2_SETTLE << 24) |	\
                           	 (SC_PLL_SETTLE << 16) |	\
                           	 1)

#define sc_read(addr)		(*(volatile u32 *)(addr))
#define sc_write(addr, val)	(*(volatile u32 *)(addr) = (u32)(val))

#define sc_write_sync()		\
do {				\
	dsb();			\
	outer_sync();		\
} while (0)

#define wfi_with_sync()				\
do {						\
	dsb();					\
	sc_write(PL310_BASE + 0x0730, 0);	\
	__asm__ __volatile__("wfi");		\
} while (0)

#define sc_xinfo(fmt, args...)		\
	xlog_printk(ANDROID_LOG_INFO, XLOG_MYTAG, fmt, ##args)

#define sc_xerror(fmt, args...)		\
	xlog_printk(ANDROID_LOG_ERROR, XLOG_MYTAG, fmt, ##args)

#define sc_info2(fmt, args...)						\
do {									\
	aee_sram_printk(fmt, ##args);					\
	xlog_printk(ANDROID_LOG_INFO, XLOG_MYTAG, fmt, ##args);		\
} while (0)

extern void __enable_irq(struct irq_desc *desc, unsigned int irq, bool resume);

extern int mt6577_irq_mask_all(struct mtk_irq_mask *mask);
extern int mt6577_irq_mask_restore(struct mtk_irq_mask *mask);
extern void mt6577_irq_unmask_for_sleep(unsigned int irq);

extern kal_int32 get_dynamic_period(int first_use, int first_wakeup_time, int battery_capacity_level);

extern void mtk_wdt_suspend(void);
extern void mtk_wdt_resume(void);

static DEFINE_SPINLOCK(sc_lock);

static int sc_pwake_en = 1;

static u16 sc_mdbus_user = 0;

static u32 sc_wake_src = (
#ifdef PLATFORM_EVB
	WAKE_SRC_KP | WAKE_SRC_MSDC1 | WAKE_SRC_EINT | WAKE_SRC_RTC |
	WAKE_SRC_CCIF_MD | WAKE_SRC_ACCDET
#else
	WAKE_SRC_MSDC1 | WAKE_SRC_EINT | WAKE_SRC_RTC |
	WAKE_SRC_CCIF_MD | WAKE_SRC_ACCDET
#endif
);

static u16 sc_wake_irq[NUM_WAKE_SRC] = {
	[2] = MT6577_KP_IRQ_ID,
	[3] = MT6577_MSDC1_IRQ_ID,
	[5] = MT6577_EINT_IRQ_ID,
	[6] = MT6577_RTC_IRQ_ID,
	[7] = MT6577_AP_CCIF_IRQ_ID,
	[8] = MT6577_ACCDET_IRQ_ID,
};

static void sc_output_wake_reason(u32 wakesrc, u32 wakesta, u32 isr, u32 pause)
{
	char str[128] = { 0 };
	bool ccif_md_wake = false;

	if (wakesta & WAKE_SRC_KP)
		strcat(str, "KP ");

	if (wakesta & WAKE_SRC_MSDC1)
		strcat(str, "MSDC1 ");

	if (wakesta & WAKE_SRC_EINT)
		strcat(str, "EINT ");

	if (wakesta & WAKE_SRC_RTC)
		strcat(str, "RTC ");

	if (wakesta & WAKE_SRC_CCIF_MD) {
		strcat(str, "CCIF_MD ");
		ccif_md_wake = true;
	}

	if (wakesta & WAKE_SRC_ACCDET)
		strcat(str, "ACCDET ");

#ifdef CONFIG_MTK_SC_DEBUG
	if (wakesta & WAKE_SRC_TS)
		strcat(str, "TS ");

	if (wakesta & WAKE_SRC_GPT)
		strcat(str, "GPT ");

	if (wakesta & WAKE_SRC_LOW_BAT)
		strcat(str, "LOW_BAT ");

	if (wakesta & WAKE_SRC_CA9_DBG)
		strcat(str, "CA9_DBG ");

	BUG_ON(wakesta == 0x0);
#endif

	sc_info2("wake up by %s(0x%x)(0x%x)(%u)\n", str, wakesta, isr, pause);
	if (ccif_md_wake)
		exec_ccci_kern_func(ID_GET_MD_WAKEUP_SRC, NULL, 0);
}

static void sc_wait_for_interrupt(unsigned int pwrlevel, bool dpidle)
{
	u32 ckdiv1;

	/* for shutdown mode issue-2 */
	ckdiv1 = sc_read(TOP_CKDIV1);
	if (!((ckdiv1 >= 0x14 && ckdiv1 < 0x18) || ckdiv1 >= 0x1d))	/* !(1/5 || 1/6) */
		sc_write(TOP_CKDIV1, 0x0b);	/* (1/4) */
	sc_write_sync();

	if (pwrlevel == 0) {
		wfi_with_sync();
	} else {
		if (dpidle)	/* debug help */
			aee_rr_rec_shutdown_mode(1);
		else
			aee_rr_rec_shutdown_mode(2);

		if (!cpu_power_down(pwrlevel == 1 ? DORMANT_MODE : SHUTDOWN_MODE))
			wfi_with_sync();

		aee_rr_rec_shutdown_mode(0);

		if (sc_read(SC_ISR_STATUS) == 0x0002)	/* Pause Abort */
			cpu_dormant_abort();
	}

	/* for shutdown mode issue-2 */
	sc_write(TOP_CKDIV1, 0x00);	/* (1/1) */
	sc_write(TOP_CKDIV1, ckdiv1);
	sc_write_sync();
}

static void sc_set_apmcu_pwrctl(unsigned int pwrlevel)
{
	switch (pwrlevel) {
	case 0:
		/* CPU standby + AP sleep => legacy sleep mode */
		sc_write(SC_PWR_CON0, 0x01a5);
		sc_write(SC_PWR_CON1, 0x0185);
		sc_write(SC_PWR_CON2, 0x01a5);
		sc_write(SC_PWR_CON3, 0x0185);
		sc_write(SC_APMCU_PWRCTL, 0x0);
		break;
	case 1:
		/* CPU dormant (L1/L2 sleep) + AP sleep */
		sc_write(SC_PWR_CON0, 0x2125);
		sc_write(SC_PWR_CON1, 0x0105);
		//sc_write(SC_PWR_CON2, 0x2525);	/* CPUSYS reset is last triggered */
		sc_write(SC_PWR_CON2, 0x2225);	/* for shutdown mode issue-1 */
		sc_write(SC_PWR_CON3, 0x0105);
		sc_write(SC_APMCU_PWRCTL, 0x2);
		break;
	case 2:
	default:
		/* CPU shutdown (L1/L2 power down) + AP sleep */
		sc_write(SC_PWR_CON0, 0x0125);
		sc_write(SC_PWR_CON1, 0x0105);
		//sc_write(SC_PWR_CON2, 0x0525);	/* CPUSYS reset is last triggered */
		sc_write(SC_PWR_CON2, 0x0225);	/* for shutdown mode issue-1 */
		sc_write(SC_PWR_CON3, 0x0105);
		sc_write(SC_APMCU_PWRCTL, 0x3);
	}
	sc_write_sync();
}

static u32 sc_get_wake_period(bool first)
{
	int period;

#ifdef PLATFORM_EVB
	period = 300;
#else
	period = get_dynamic_period(first, 300, 1);	/* battery decreases 1% */
	if (period <= 1) {
		sc_xerror("!!! CANNOT GET PERIOD FROM FUEL GAUGE !!!\n");
		period = 300;
	} else if (period > 36 * 3600) {	/* max period is 36.4 hours */
		period = 36 * 3600;
	}
#endif

	return period;
}

static wake_reason_t sc_enter_pause_mode(unsigned int pwrlevel, u32 sec, u32 wakesrc)
{
	u32 pause, wakesta, isr;
	wake_reason_t wr;

	sc_info2("sec = %u, wakesrc = 0x%x\n", sec, wakesrc);

	if (sec > 0) {
		sc_write(SC_PAUSE, sec * 32768);

		/* unmask wakeup sources */
		sc_write(SC_WAKEUP_EVENT_MASK, ~wakesrc);

		/* unmask Pause Interrupt, Pause Abort and Timer Timeout */
		sc_write(SC_ISR_MASK, 0);

		sc_write(SC_TIMER_CON, SC_TIMER_CON_DOWN | SC_TIMER_CON_EN);
	} else {
		sc_write(SC_PAUSE, 0xffffffff);

		/* unmask wakeup sources */
		sc_write(SC_WAKEUP_EVENT_MASK, ~wakesrc);

		/* unmask Pause Interrupt and Pause Abort */
		sc_write(SC_ISR_MASK, 0x0001);

		sc_write(SC_TIMER_CON, SC_TIMER_CON_EN);
	}

	sc_write(SC_TIMER_CMD, SC_TIMER_CMD_KEY | SC_TIMER_CMD_CONWR |
	                       SC_TIMER_CMD_CNTWR | SC_TIMER_CMD_WR);
	while (!(sc_read(SC_TIMER_STA) & SC_TIMER_STA_CMDCPL));

	sc_write(SC_TIMER_CMD, SC_TIMER_CMD_KEY | SC_TIMER_CMD_PAUSE);
	while (!(sc_read(SC_TIMER_STA) & SC_TIMER_STA_CMDCPL));

	sc_wait_for_interrupt(pwrlevel, false);

	wakesta = sc_read(SC_WAKEUP_SRC);
	isr = sc_read(SC_ISR_STATUS);
	pause = sc_read(SC_FINAL_PAUSE);

	/* delay to make sure ISR_STATUS can be cleared later */
	udelay(100);

	/* for cleaning wakeup source status */
	sc_write(SC_DBG_WAKEUP, 0);

	sc_write(SC_TIMER_CON, SC_TIMER_CON_DBG | SC_TIMER_CON_EN);
	sc_write(SC_TIMER_CMD, SC_TIMER_CMD_KEY | SC_TIMER_CMD_CONWR |
	                       SC_TIMER_CMD_WR);
	while (!(sc_read(SC_TIMER_STA) & SC_TIMER_STA_CMDCPL));

	sc_write(SC_ISR_MASK, 0x0007);
	sc_write(SC_ISR_STATUS, 0x0007);	/* write 1 clear */
	sc_write_sync();

	if (isr == 0x0001) {	/* Timer Timeout */
		wr = WR_SLP_TMR;
		sc_info2("wake up by SLP_TMR (0x%x)(0x%x)(%u)\n", wakesta, isr, pause);
	} else {
		wr = WR_WAKE_SRC;
		sc_output_wake_reason(wakesrc, wakesta, isr, pause);
	}

	return wr;
}

static int sc_enable_wake_irq(unsigned int irq)
{
	unsigned long flags;
	struct irq_desc *desc;

	desc = irq_to_desc(irq);
	if (!desc || !desc->action)	/* no IRQ handler */
		return -EINVAL;

	raw_spin_lock_irqsave(&desc->lock, flags);
	__enable_irq(desc, irq, true);
	raw_spin_unlock_irqrestore(&desc->lock, flags);

	return 0;
}

/*
 * pwrlevel:
 *     0: CPU standby + AP sleep => legacy sleep mode
 *     1: CPU dormant (L1/L2 sleep) + AP sleep
 *     2: CPU shutdown (L1/L2 power down) + AP sleep
 */
wake_reason_t sc_go_to_sleep(unsigned int pwrlevel)
{
	u32 wakesrc, dvfscon, sec = 0;
	int i, r;
	unsigned long flags;
	struct mtk_irq_mask mask;
	static wake_reason_t last_wr = WR_NONE;

	sc_info2("pwrlevel = %u\n", pwrlevel);

	if (sc_pwake_en)
		sec = sc_get_wake_period(!!(last_wr != WR_SLP_TMR));

	mtk_wdt_suspend();

	spin_lock_irqsave(&sc_lock, flags);
	wakesrc = sc_wake_src;
	for (i = 0; i < NUM_WAKE_SRC; i++) {
		if (wakesrc & (1U << i)) {
			r = sc_enable_wake_irq(sc_wake_irq[i]);
			if (r)
				wakesrc &= ~(1U << i);
		}
	}

	mt6577_irq_mask_all(&mask);

	mt6577_irq_unmask_for_sleep(MT6577_SLEEP_IRQ_ID);

	/* to workaround DVFS issue due to CKDIV1 failure */
	dvfscon = sc_read(SC_AP_DVFS_CON);
	//ckdiv1 = sc_read(TOP_CKDIV1);
	sc_write(SC_AP_DVFS_CON, dvfscon | 0x3);
	sc_write_sync();

	sc_set_apmcu_pwrctl(pwrlevel);

#ifdef CONFIG_MTK_SC_DEBUG
	/* bypass CSYSPWRUPREQ to avoid keeping SRCLKENA high */
	sc_write(SC_CLK_CON, sc_read(SC_CLK_CON) | SC_CLK_CON_BYPASSDBG);
	sc_write_sync();
#endif

	last_wr = sc_enter_pause_mode(pwrlevel, sec, wakesrc);

#ifdef CONFIG_MTK_SC_DEBUG
	/* restore SC_CLK_CON */
	sc_write(SC_CLK_CON, sc_read(SC_CLK_CON) & ~SC_CLK_CON_BYPASSDBG);
	sc_write_sync();
#endif

	/* to workaround DVFS issue due to CKDIV1 failure */
	//sc_write(TOP_CKDIV1, 0x00);
	//sc_write(TOP_CKDIV1, ckdiv1);
	sc_write(SC_AP_DVFS_CON, dvfscon);
	sc_write_sync();

	mt6577_irq_mask_restore(&mask);
	spin_unlock_irqrestore(&sc_lock, flags);

	mtk_wdt_resume();

	return last_wr;
}

#ifdef CONFIG_MTK_SC_DEBUG
void sc_set_wake_src(u32 wake_src, u16 wake_irq)
{
	int i;
	unsigned long flags;

	spin_lock_irqsave(&sc_lock, flags);
	sc_wake_src = wake_src;

	for (i = 0; i < NUM_WAKE_SRC; i++) {
		if (wake_src & (1U << i)) {
			sc_wake_irq[i] = wake_irq;
			break;
		}
	}
	spin_unlock_irqrestore(&sc_lock, flags);
}
#endif

void __attribute__((weak)) sc_dpidle_before_wfi(void)
{
}

void __attribute__((weak)) sc_dpidle_after_wfi(void)
{
}

bool __attribute__((weak)) sc_dpidle_can_enter(void)
{
	return true;
}

/*
 * pwrlevel:
 *     0: CPU standby + AP sleep (SRCLKENA high) => legacy deep idle
 *     1: CPU dormant (L1/L2 sleep) + AP sleep (SRCLKENA high)
 *     2: CPU shutdown (L1/L2 power down) + AP sleep (SRCLKENA high)
 */
void sc_go_to_dpidle(unsigned int pwrlevel)
{
	u32 wakesrc, dvfscon, clksettle;
	unsigned long flags;
	struct mtk_irq_mask mask;

	wakesrc = WAKE_SRC_KP | WAKE_SRC_MSDC1 | WAKE_SRC_EINT | WAKE_SRC_RTC |
	          WAKE_SRC_CCIF_MD | WAKE_SRC_ACCDET | WAKE_SRC_GPT |
	          WAKE_SRC_AFE;

	spin_lock_irqsave(&sc_lock, flags);
	mt6577_irq_mask_all(&mask);

	mt6577_irq_unmask_for_sleep(MT6577_SLEEP_IRQ_ID);

	/* to workaround DVFS issue due to CKDIV1 failure */
	dvfscon = sc_read(SC_AP_DVFS_CON);
	//ckdiv1 = sc_read(TOP_CKDIV1);
	sc_write(SC_AP_DVFS_CON, dvfscon | 0x3);
	sc_write_sync();

	sc_set_apmcu_pwrctl(pwrlevel);

	/* keep SRCLKENA high (26M on) when in sleep mode */
	sc_write(SC_CLK_CON, sc_read(SC_CLK_CON) | SC_CLK_CON_SRCLKEN);

	/* set SYSCLK settle time to 1T 32K */
	clksettle = sc_read(SC_CLK_SETTLE);
	sc_write(SC_CLK_SETTLE, SC_CLK_SETTLE_26MON);

	sc_write(SC_PAUSE, 0xffffffff);

	/* unmask wakeup sources */
	sc_write(SC_WAKEUP_EVENT_MASK, ~wakesrc);

	/* unmask Pause Interrupt and Pause Abort */
	sc_write(SC_ISR_MASK, 0x0001);

	sc_write(SC_TIMER_CON, SC_TIMER_CON_EN);
	sc_write(SC_TIMER_CMD, SC_TIMER_CMD_KEY | SC_TIMER_CMD_CONWR |
	                       SC_TIMER_CMD_CNTWR | SC_TIMER_CMD_WR);
	while (!(sc_read(SC_TIMER_STA) & SC_TIMER_STA_CMDCPL));

	if (sc_dpidle_can_enter()) {
		sc_write(SC_TIMER_CMD, SC_TIMER_CMD_KEY | SC_TIMER_CMD_PAUSE);
		while (!(sc_read(SC_TIMER_STA) & SC_TIMER_STA_CMDCPL));

		sc_dpidle_before_wfi();

		sc_wait_for_interrupt(pwrlevel, true);

		sc_dpidle_after_wfi();
	}

	/* delay to make sure ISR_STATUS can be cleared and CKDIV1 can be reset later */
	udelay(100);

	/* for cleaning wakeup source status */
	sc_write(SC_DBG_WAKEUP, 0);

	sc_write(SC_TIMER_CON, SC_TIMER_CON_DBG | SC_TIMER_CON_EN);
	sc_write(SC_TIMER_CMD, SC_TIMER_CMD_KEY | SC_TIMER_CMD_CONWR |
	                       SC_TIMER_CMD_WR);
	while (!(sc_read(SC_TIMER_STA) & SC_TIMER_STA_CMDCPL));

	sc_write(SC_ISR_MASK, 0x0007);
	sc_write(SC_ISR_STATUS, 0x0007);	/* write 1 clear */

	/* restore SC_CLK_SETTLE and SC_CLK_CON */
	sc_write(SC_CLK_SETTLE, clksettle);
	sc_write(SC_CLK_CON, sc_read(SC_CLK_CON) & ~SC_CLK_CON_SRCLKEN);
	sc_write_sync();

	/* to workaround DVFS issue due to CKDIV1 failure */
	//sc_write(TOP_CKDIV1, 0x00);
	//sc_write(TOP_CKDIV1, ckdiv1);
	sc_write(SC_AP_DVFS_CON, dvfscon);
	sc_write_sync();

	mt6577_irq_mask_restore(&mask);
	spin_unlock_irqrestore(&sc_lock, flags);
}

void sc_request_mdbus_clk(sc_mdbus_user_t user)
{
	unsigned long flags;

	if (user < SC_MDBUS_USER_SLEEP || user > SC_MDBUS_USER_AUDIO)
		return;

	spin_lock_irqsave(&sc_lock, flags);
	if (!(sc_mdbus_user & SC_MDBUS_USER_MASK)) {	/* first user */
		sc_write(SC_CLK_CON, sc_read(SC_CLK_CON) | SC_CLK_CON_CLKREQ2MD);
		sc_write_sync();
	}

	sc_mdbus_user |= (1U << user);
	//sc_xinfo("user %d requests MD bus clock (0x%x)\n", user, sc_mdbus_user);
	spin_unlock_irqrestore(&sc_lock, flags);
}

void sc_unrequest_mdbus_clk(sc_mdbus_user_t user)
{
	unsigned long flags;

	if (user < SC_MDBUS_USER_SLEEP || user > SC_MDBUS_USER_AUDIO)
		return;

	spin_lock_irqsave(&sc_lock, flags);
	sc_mdbus_user &= ~(1U << user);

	if (!(sc_mdbus_user & SC_MDBUS_USER_MASK)) {	/* no users */
		sc_write(SC_CLK_CON, sc_read(SC_CLK_CON) & ~SC_CLK_CON_CLKREQ2MD);
		sc_write_sync();
	}
	//sc_xinfo("user %d unrequests MD bus clock (0x%x)\n", user, sc_mdbus_user);
	spin_unlock_irqrestore(&sc_lock, flags);
}

static irqreturn_t sc_irq_handler(int irq, void *dev_id)
{
	sc_xerror("!!! SC ISR SHOULD NOT BE EXECUTED !!!\n");

	spin_lock(&sc_lock);
	sc_write(SC_ISR_MASK, 0x0007);
	sc_write(SC_ISR_STATUS, 0x0007);	/* write 1 clear */
	sc_write_sync();
	spin_unlock(&sc_lock);

	return IRQ_HANDLED;
}

void sc_mod_init(void)
{
	int r;
	unsigned long flags;

	cpu_dormant_init();

	spin_lock_irqsave(&sc_lock, flags);
#ifdef CONFIG_MTK_SC_DEBUG
	sc_pwake_en = 0;

	/* make AP control whole-chip sleep mode */
	sc_write(SC_MD_INTF_CON, 0x363f);
#else
	/* NOTE: reset MD_INTF_CON to 0 because it was modified in U-Boot */
	sc_write(SC_MD_INTF_CON, 0);
#endif

	/* typical values can cover MD's RM_SYSCLK_SETTLE = 3 ~ 5 ms */
	sc_write(SC_CLK_SETTLE, SC_CLK_SETTLE_TYPICAL);

	/* MTCMOS power-on sequence: CPUSYS(2) -> CPU(0) -> NEON(3) -> DBG(1) */
	sc_write(SC_PWR_CON8, (1 << 20) | (3 << 16) | (0 << 12) | (2 << 8));
	//sc_write(SC_PWR_CON9, 0x000f);
	sc_write(SC_PWR_CON9, 0);	/* for shutdown mode issue-1 */

	/* bit 17: DVFS_SEL[0] = 1 depends on AP_SRCLKEN (from E2) */
	/* bit 7: STANDBYWFI = STANDBYWFI_CPU & L2CC_IDLE & SCU_IDLE (from E2) */
	/* bit 4: enable SRCLKENAI pin */
	sc_write(SC_CLK_CON, (1U << 17) | (1U << 7) | (1U << 4));

	/* if bus clock source is from 3GPLL, make AP be able to control 3GPLL */
	if ((sc_read(TOP_CKMUXSEL) & 0x3) == 0x2)
		sc_write(SC_CLK_CON, sc_read(SC_CLK_CON) | SC_CLK_CON_3GPLLON);

	/* set peripheral control to handshake mode */
	sc_write(SC_PERI_CON, 0x0081);

	sc_write(SC_PAUSE, 0);

	/* mask all wakeup sources */
	sc_write(SC_WAKEUP_EVENT_MASK, 0xffffffff);

	/* for cleaning wakeup source status */
	sc_write(SC_DBG_WAKEUP, 0);

	sc_write(SC_TIMER_CON, SC_TIMER_CON_DBG | SC_TIMER_CON_EN);
	sc_write(SC_TIMER_CMD, SC_TIMER_CMD_KEY | SC_TIMER_CMD_CONWR |
	                       SC_TIMER_CMD_CNTWR | SC_TIMER_CMD_WR);
	while (!(sc_read(SC_TIMER_STA) & SC_TIMER_STA_CMDCPL));

	sc_write(SC_ISR_MASK, 0x0007);
	sc_write(SC_ISR_STATUS, 0x0007);	/* write 1 clear */
	sc_write_sync();
	spin_unlock_irqrestore(&sc_lock, flags);

	r = request_irq(MT6577_SLEEP_IRQ_ID, sc_irq_handler, IRQF_TRIGGER_LOW,
	                SC_NAME, NULL);
	if (r) {
		sc_xerror("register SC IRQ failed (%d)\n", r);
		WARN_ON(1);
	}
}

module_param(sc_pwake_en, bool, 0644);

MODULE_AUTHOR("Terry Chang <terry.chang@mediatek.com>");
MODULE_DESCRIPTION("MT6577 SLPCTRL Driver v4.6");
