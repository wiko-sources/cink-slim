#include <common.h>

#include <asm/io.h>
#include <asm/arch/mt65xx.h>
#include <asm/arch/boot_mode.h>
#include <asm/arch/mt65xx_typedefs.h>

#define CA9_SCU_BASE                (0xC000A000)
#define CA9_SCU_CONTROL             (CA9_SCU_BASE + 0x0000)
#define CA9_SCU_CONFIG              (CA9_SCU_BASE + 0x0004)
#define CA9_SCU_CPU_POWER_STATUS    (CA9_SCU_BASE + 0x0008)
#define CA9_SCU_INVAL_ALL           (CA9_SCU_BASE + 0x000C)
#define CA9_SCU_FILTERING_START     (CA9_SCU_BASE + 0x0040)
#define CA9_SCU_FILTERING_END       (CA9_SCU_BASE + 0x0044)
#define CA9_SCU_ACCESS_CONTROL      (CA9_SCU_BASE + 0x0050)
#define CA9_SCU_NS_ACCESS_CONTROL   (CA9_SCU_BASE + 0x0054)

/* SLPCTRL registers */

#define SC_CLK_SETTLE           (SLEEP_BASE + 0x0100)
#define SC_PWR_SETTLE           (SLEEP_BASE + 0x0104)
#define SC_PWR_CON0             (SLEEP_BASE + 0x0200)
#define SC_PWR_CON1             (SLEEP_BASE + 0x0204)
#define SC_PWR_CON2             (SLEEP_BASE + 0x0208)
#define SC_PWR_CON3             (SLEEP_BASE + 0x020c)
#define SC_PWR_CON4             (SLEEP_BASE + 0x0210)
#define SC_PWR_CON5             (SLEEP_BASE + 0x0214)
#define SC_PWR_CON6             (SLEEP_BASE + 0x0218)
#define SC_PWR_CON7             (SLEEP_BASE + 0x021c)
#define SC_PWR_CON8             (SLEEP_BASE + 0x0220)
#define SC_PWR_CON9             (SLEEP_BASE + 0x0224)
#define SC_CLK_CON              (SLEEP_BASE + 0x0300)
#define SC_MD_CLK_CON           (SLEEP_BASE + 0x0304)
#define SC_MD_INTF_CON          (SLEEP_BASE + 0x0308)
#define SC_MD_INTF_STS          (SLEEP_BASE + 0x030c)
#define SC_TMR_PWR              (SLEEP_BASE + 0x0400)
#define SC_DBG_CON              (SLEEP_BASE + 0x0410)
#define SC_PERI_CON             (SLEEP_BASE + 0x0500)
#define SC_STATE                (SLEEP_BASE + 0x0510)
#define SC_PWR_STA              (SLEEP_BASE + 0x0520)
#define SC_APMCU_PWRCTL         (SLEEP_BASE + 0x0600)
#define SC_AP_DVFS_CON          (SLEEP_BASE + 0x0610)
#define SC_AP_STANBY_EXT        (SLEEP_BASE + 0x0620)
#define SC_TIMER_CON            (SLEEP_BASE + 0x0700)
#define SC_TIMER_CMD            (SLEEP_BASE + 0x0710)
#define SC_TIMER_STA            (SLEEP_BASE + 0x0720)
#define SC_FINAL_PAUSE          (SLEEP_BASE + 0x0730)
#define SC_PAUSE                (SLEEP_BASE + 0x0740)
#define SC_DBG_WAKEUP           (SLEEP_BASE + 0x0750)
#define SC_WAKEUP_SRC           (SLEEP_BASE + 0x0800)
#define SC_WAKEUP_EVENT_MASK    (SLEEP_BASE + 0x0810)
#define SC_ISR_MASK             (SLEEP_BASE + 0x0900)
#define SC_ISR_STATUS           (SLEEP_BASE + 0x0904)

/* SLPCTRL wakeup sources */

#define WAKE_SRC_TS             (1U << 1)
#define WAKE_SRC_KP             (1U << 2)
#define WAKE_SRC_MSDC1          (1U << 3)
#define WAKE_SRC_GPT            (1U << 4)
#define WAKE_SRC_EINT           (1U << 5)
#define WAKE_SRC_RTC            (1U << 6)
#define WAKE_SRC_CCIF_MD        (1U << 7)
#define WAKE_SRC_ACCDET         (1U << 8)
#define WAKE_SRC_LOW_BAT        (1U << 9)
#define WAKE_SRC_CA9_DBG        (1U << 19)
#define WAKE_SRC_AFE            (1U << 21)

#define NUM_WAKE_SRC            (22)

/* Top Clock Generator registers */

#define TOP_CKMUXSEL            (CONFIG_BASE + 0x0000)
#define TOP_CKDIV0              (CONFIG_BASE + 0x0004)
#define TOP_CKDIV1              (CONFIG_BASE + 0x0008)
#define TOP_CKDIV23             (CONFIG_BASE + 0x000C)
#define TOP_DCMCTL              (CONFIG_BASE + 0x0010)
#define TOP_MISC                (CONFIG_BASE + 0x0014)
#define TOP_CA9DCMFSEL          (CONFIG_BASE + 0x0018)
#define TOP_CKCTL               (CONFIG_BASE + 0x0020)

#define MIPI_CON                (CONFIG_BASE + 0x0890)

/* Clock Gating registers */

#define PERI_GLOBALCON_PDN0     (PERI_CON_BASE + 0x0010)
#define PERI_GLOBALCON_PDN1     (PERI_CON_BASE + 0x0014)
#define PERI_GLOBALCON_PDN2     (PERI_CON_BASE + 0x0018)
#define PERI_GLOBALCON_PDN3     (PERI_CON_BASE + 0x001C)
#define PERI_GLOBALCON_DCMCTL   (PERI_CON_BASE + 0x020)
#define PERI_GLOBALCON_DCMDBC   (PERI_CON_BASE + 0x024)
#define PERI_GLOBALCON_DCMFSEL  (PERI_CON_BASE + 0x028)

#define MMSYS1_CG_CON0          (MMSYS1_CONFIG_BASE + 0x0000)
#define MMSYS1_CG_CON1          (MMSYS1_CONFIG_BASE + 0x0004)
#define MMSYS1_CG_CON2          (MMSYS1_CONFIG_BASE + 0x0008)
#define MMSYS1_CG_SET0          (MMSYS1_CONFIG_BASE + 0x0010)
#define MMSYS1_CG_SET1          (MMSYS1_CONFIG_BASE + 0x0014)
#define MMSYS1_CG_SET2          (MMSYS1_CONFIG_BASE + 0x0018)
#define MMSYS1_CG_CLR0          (MMSYS1_CONFIG_BASE + 0x0020)
#define MMSYS1_CG_CLR1          (MMSYS1_CONFIG_BASE + 0x0024)
#define MMSYS1_CG_CLR2          (MMSYS1_CONFIG_BASE + 0x0028)

#define MMSYS2_CG_CON           (MMSYS2_CONFG_BASE + 0x0300)
#define MMSYS2_CG_SET           (MMSYS2_CONFG_BASE + 0x0304)
#define MMSYS2_CG_CLR           (MMSYS2_CONFG_BASE + 0x0308)
#define MMSYS2_MM2AXI_CON0      (MMSYS2_CONFG_BASE + 0x0400)

#define AUDIO_TOP_CON0          (AUDIO_BASE + 0x0000)

/* MixedSys registers */

#define XOSC_CON0               (APMIXED_BASE + 0x0000)
#define XOSC_CON1               (APMIXED_BASE + 0x0004)
#define CLKSQ_CON0              (APMIXED_BASE + 0x0020)
#define PLL_CON0                (APMIXED_BASE + 0x0040)
#define PLL_CON1                (APMIXED_BASE + 0x0044)
#define PLL_CON2                (APMIXED_BASE + 0x0048)
#define PLL_CON3                (APMIXED_BASE + 0x004C)	
#define PLL_CON4                (APMIXED_BASE + 0x0050)
#define PLL_CON11               (APMIXED_BASE + 0x006C)
#define ARMPLL_CON0             (APMIXED_BASE + 0x0100)
#define ARMPLL_CON1             (APMIXED_BASE + 0x0104)
#define ARMPLL_CON2             (APMIXED_BASE + 0x0108)
#define ARMPLL_CON3             (APMIXED_BASE + 0x010C)
#define MAINPLL_CON0            (APMIXED_BASE + 0x0120)
#define MAINPLL_CON1            (APMIXED_BASE + 0x0124)
#define MAINPLL_CON2            (APMIXED_BASE + 0x0128)
#define MAINPLL_CON3            (APMIXED_BASE + 0x012C)
#define IPLL_CON0               (APMIXED_BASE + 0x0140)
#define IPLL_CON1               (APMIXED_BASE + 0x0144)
#define IPLL_CON2               (APMIXED_BASE + 0x0148)
#define IPLL_CON3               (APMIXED_BASE + 0x014C)
#define UPLL_CON0               (APMIXED_BASE + 0x0160)
#define UPLL_CON1               (APMIXED_BASE + 0x0164)
#define UPLL_CON2               (APMIXED_BASE + 0x0168)
#define UPLL_CON3               (APMIXED_BASE + 0x016C)
#define MDPLL_CON0              (APMIXED_BASE + 0x0180)
#define MDPLL_CON1              (APMIXED_BASE + 0x0184)
#define MDPLL_CON2              (APMIXED_BASE + 0x0188)
#define MDPLL_CON3              (APMIXED_BASE + 0x018C)
#define TVDDS_CON0              (APMIXED_BASE + 0x01A0)
#define TVDDS_CON1              (APMIXED_BASE + 0x01A4)
#define TVDDS_CON2              (APMIXED_BASE + 0x01A8)
#define TVDDS_CON3              (APMIXED_BASE + 0x01AC)
#define WPLL_CON0               (APMIXED_BASE + 0x01C0)
#define WPLL_CON1               (APMIXED_BASE + 0x01C4)
#define WPLL_CON2               (APMIXED_BASE + 0x01C8)
#define AUDPLL_CON0             (APMIXED_BASE + 0x01E0)
#define AUDPLL_CON1             (APMIXED_BASE + 0x01E4)
#define AUDPLL_CON2             (APMIXED_BASE + 0x01E8)
#define MEMPLL_CON0             (APMIXED_BASE + 0x0200)
#define MEMPLL_CON1             (APMIXED_BASE + 0x0204)
#define MEMPLL_CON2             (APMIXED_BASE + 0x0208)
#define MEMPLL_CON3             (APMIXED_BASE + 0x020C)
#define MEMPLL_CON4             (APMIXED_BASE + 0x021C)

#define TOPCKGEN_CON0           (APMIXED_BASE + 0x0B00)
#define TOPCKGEN_CON1           (APMIXED_BASE + 0x0B04)
#define TOPCKGEN_CON2           (APMIXED_BASE + 0x0B08)
#define TOPCKGEN_CON3           (APMIXED_BASE + 0x0B0C)

/* DCM register */

#define INFRA_DCMCTL            (CONFIG_BASE + 0x050)
#define INFRA_DCMDBC            (CONFIG_BASE + 0x054)
#define INFRA_DCMFSEL           (CONFIG_BASE + 0x058)

#define INFRA_MISC              (CONFIG_BASE + 0xF00)

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

#define PMIC_SETTLE_TIME        (100) // us

/* extern function */
extern void sc_mod_init(void);
extern void sc_mod_exit(void);
extern void gpt_one_shot_irq(unsigned int ms);
extern void mt6577_sleep(u32 timeout, kal_bool en_deep_idle);
