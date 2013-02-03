#ifndef _MT6577_SC_
#define _MT6577_SC_

#include <linux/kernel.h>
#include <mach/mt6577_reg_base.h>

/* SLPCTRL registers */
#define SC_CLK_SETTLE		(SLEEP_BASE + 0x0100)
#define SC_PWR_SETTLE		(SLEEP_BASE + 0x0104)
#define SC_PWR_CON0		(SLEEP_BASE + 0x0200)
#define SC_PWR_CON1		(SLEEP_BASE + 0x0204)
#define SC_PWR_CON2		(SLEEP_BASE + 0x0208)
#define SC_PWR_CON3		(SLEEP_BASE + 0x020c)
#define SC_PWR_CON4		(SLEEP_BASE + 0x0210)
#define SC_PWR_CON5		(SLEEP_BASE + 0x0214)
#define SC_PWR_CON6		(SLEEP_BASE + 0x0218)
#define SC_PWR_CON7		(SLEEP_BASE + 0x021c)
#define SC_PWR_CON8		(SLEEP_BASE + 0x0220)
#define SC_PWR_CON9		(SLEEP_BASE + 0x0224)
#define SC_CLK_CON		(SLEEP_BASE + 0x0300)
#define SC_MD_CLK_CON		(SLEEP_BASE + 0x0304)
#define SC_MD_INTF_CON		(SLEEP_BASE + 0x0308)
#define SC_MD_INTF_STS		(SLEEP_BASE + 0x030c)
#define SC_TMR_PWR		(SLEEP_BASE + 0x0400)
#define SC_DBG_CON		(SLEEP_BASE + 0x0410)
#define SC_PERI_CON		(SLEEP_BASE + 0x0500)
#define SC_STATE		(SLEEP_BASE + 0x0510)
#define SC_PWR_STA		(SLEEP_BASE + 0x0520)
#define SC_APMCU_PWRCTL		(SLEEP_BASE + 0x0600)
#define SC_AP_DVFS_CON		(SLEEP_BASE + 0x0610)
#define SC_AP_STANBY_EXT	(SLEEP_BASE + 0x0620)
#define SC_TIMER_CON		(SLEEP_BASE + 0x0700)
#define SC_TIMER_CMD		(SLEEP_BASE + 0x0710)
#define SC_TIMER_STA		(SLEEP_BASE + 0x0720)
#define SC_FINAL_PAUSE		(SLEEP_BASE + 0x0730)
#define SC_PAUSE		(SLEEP_BASE + 0x0740)
#define SC_DBG_WAKEUP		(SLEEP_BASE + 0x0750)
#define SC_WAKEUP_SRC		(SLEEP_BASE + 0x0800)
#define SC_WAKEUP_EVENT_MASK	(SLEEP_BASE + 0x0810)
#define SC_ISR_MASK		(SLEEP_BASE + 0x0900)
#define SC_ISR_STATUS		(SLEEP_BASE + 0x0904)

/* SLPCTRL wakeup sources */
#define WAKE_SRC_TS		(1U << 1)
#define WAKE_SRC_KP		(1U << 2)
#define WAKE_SRC_MSDC1		(1U << 3)
#define WAKE_SRC_GPT		(1U << 4)
#define WAKE_SRC_EINT		(1U << 5)
#define WAKE_SRC_RTC		(1U << 6)
#define WAKE_SRC_CCIF_MD	(1U << 7)
#define WAKE_SRC_ACCDET		(1U << 8)
#define WAKE_SRC_LOW_BAT	(1U << 9)
#define WAKE_SRC_CA9_DBG	(1U << 19)
#define WAKE_SRC_AFE		(1U << 21)

#define NUM_WAKE_SRC		22

typedef enum {
	WR_NONE = 0,
	WR_SLP_TMR = 1,
	WR_WAKE_SRC = 2,
} wake_reason_t;

extern wake_reason_t sc_go_to_sleep(unsigned int pwrlevel);

#ifdef CONFIG_MTK_SC_DEBUG
extern void sc_set_wake_src(u32 wake_src, u16 wake_irq);
#endif

/*
 * NOTE:
 * 1. sc_dpidle_before/after_wfi() and sc_dpidle_can_enter() can be redefined
 *    in other source file
 * 2. sc_dpidle_can_enter() needs to tell if the system can enter deep idle mode
 */
extern void sc_dpidle_before_wfi(void);
extern void sc_dpidle_after_wfi(void);
extern bool sc_dpidle_can_enter(void);
extern void sc_go_to_dpidle(unsigned int pwrlevel);

#define SC_MDBUS_USER_MASK	((1U << 2) - 1)

typedef enum {
	SC_MDBUS_USER_SLEEP	= 0,
	SC_MDBUS_USER_AUDIO	= 1,
} sc_mdbus_user_t;

/*
 * NOTE:
 * 1. if request MD bus clock, when AP bus is active, MD bus will be also active
 *    so that it will increase power consumption
 */
extern void sc_request_mdbus_clk(sc_mdbus_user_t user);
extern void sc_unrequest_mdbus_clk(sc_mdbus_user_t user);

extern void sc_mod_init(void);


extern void slp_mod_init(void);

#endif
