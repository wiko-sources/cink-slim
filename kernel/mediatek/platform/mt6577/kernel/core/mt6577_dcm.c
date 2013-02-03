#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/aee.h>

#include "linux/leds-mt65xx.h"

#include "mach/mt6577_boot.h"
#include "mach/mt6577_typedefs.h"
#include "mach/mt6577_reg_base.h"
#include "mach/irqs.h"
#include "mach/mt6577_irq.h"
#include "mach/mt6577_gpt.h"
#include "mach/mt6577_dcm.h"
#include "mach/mt6577_clock_manager.h"
#include "mach/mt6577_clkmgr_internal.h"
#include "mach/mt6577_sc.h"

#ifdef CONFIG_SMP
#include <linux/cpu.h>
#endif

#ifdef CONFIG_LOCAL_TIMERS
#include <asm/smp_twd.h>
#endif

#define USING_XLOG

#ifdef USING_XLOG 
#include <linux/xlog.h>

#define TAG     "Power/dcm"

#define dcm_err(fmt, args...)       \
    xlog_printk(ANDROID_LOG_ERROR, TAG, fmt, ##args)
#define dcm_warn(fmt, args...)      \
    xlog_printk(ANDROID_LOG_WARN, TAG, fmt, ##args)
#define dcm_info(fmt, args...)      \
    xlog_printk(ANDROID_LOG_INFO, TAG, fmt, ##args)
#define dcm_dbg(fmt, args...)       \
    xlog_printk(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define dcm_ver(fmt, args...)       \
    xlog_printk(ANDROID_LOG_VERBOSE, TAG, fmt, ##args)

#else /* !USING_XLOG */

#define TAG     "[Power/dcm] "

#define dcm_err(fmt, args...)       \
    printk(KERN_ERR TAG);           \
    printk(KERN_CONT fmt, ##args) 
#define dcm_warn(fmt, args...)      \
    printk(KERN_WARNING TAG);       \
    printk(KERN_CONT fmt, ##args)
#define dcm_info(fmt, args...)      \
    printk(KERN_NOTICE TAG);        \
    printk(KERN_CONT fmt, ##args)
#define dcm_dbg(fmt, args...)       \
    printk(KERN_INFO TAG);          \
    printk(KERN_CONT fmt, ##args)
#define dcm_ver(fmt, args...)       \
    printk(KERN_DEBUG TAG);         \
    printk(KERN_CONT fmt, ##args)

#endif



/*
 ********************************** 
 *          DCM PART
 **********************************
 */


/**
 * dcm_enable - This function will enable DCM
 * @type: CA9_DCM, TOPAXI_DCM, EMI_DCM, PERI_DCM, MM1_DCM, MM2_DCM, ALL_DCM
 */
void dcm_enable(unsigned short type)
{
    unsigned int temp;

    switch (type)
    {
        case CA9_DCM:
            DRV_WriteReg32(TOP_CA9DCMFSEL, 0x07000000);
            DRV_WriteReg32(TOP_DCMCTL, 0x6);
            break;
        case TOPAXI_DCM:
            DRV_WriteReg32(INFRA_DCMDBC, 0x17F);
            DRV_WriteReg32(INFRA_DCMFSEL, 0x0);
            DRV_WriteReg32(INFRA_DCMCTL, ((0x1 << 8) | (0x1)));
            break;
        case EMI_DCM:
            temp = DRV_Reg32(0xF00041DC);
            temp |= (0x3 << 24);
            DRV_WriteReg32(0xF00041DC, temp);
            
            DRV_WriteReg32(INFRA_MISC, 0x1);
            DRV_WriteReg32(INFRA_DCMCTL, ((0x1 << 8) | (0x1)));
            break;
        case PERI_DCM:
            DRV_WriteReg32(PERI_GLOBALCON_DCMFSEL, 0x0);
            DRV_WriteReg32(PERI_GLOBALCON_DCMCTL, (0x1 << 10) | (0x1 << 9) | (0x1 << 8) | (0x1 << 0));
            break;
        case MM1_DCM:
            DRV_WriteReg32(SMI_LARB0_CON_SET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB1_CON_SET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB2_CON_SET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB3_CON_SET, (0x1 << 11));
            break;
         case MM2_DCM:
            DRV_WriteReg32(MMSYS2_DCMFSEL, 0x0);
            DRV_WriteReg32(MMSYS2_DCMCTL, ((0x1 << 8) | (0x1 << 0)));
            break;
         case ALL_DCM:
            // CA9_DCM
            DRV_WriteReg32(TOP_CA9DCMFSEL, 0x07000000);
            DRV_WriteReg32(TOP_DCMCTL, 0x6);
            
            // TOPAXI_DCM
            DRV_WriteReg32(INFRA_DCMDBC, 0x17F);
            DRV_WriteReg32(INFRA_DCMFSEL, 0x0);
            DRV_WriteReg32(INFRA_DCMCTL, ((0x1 << 8) | (0x1)));
            
            // EMI_DCM
            temp = DRV_Reg32(0xF00041DC);
            temp |= (0x3 << 24);
            DRV_WriteReg32(0xF00041DC, temp);
            
            DRV_WriteReg32(INFRA_MISC, 0x1);
            DRV_WriteReg32(INFRA_DCMCTL, ((0x1 << 8) | (0x1)));
        
            // PERI_DCM
            DRV_WriteReg32(PERI_GLOBALCON_DCMFSEL, 0x0);
            DRV_WriteReg32(PERI_GLOBALCON_DCMCTL, (0x1 << 10) | (0x1 << 9) | (0x1 << 8) | (0x1 << 0));
           
            // MM1_DCM 
            DRV_WriteReg32(SMI_LARB0_CON_SET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB1_CON_SET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB2_CON_SET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB3_CON_SET, (0x1 << 11));
         
            // MM2_DCM
            DRV_WriteReg32(MMSYS2_DCMFSEL, 0x0);
            DRV_WriteReg32(MMSYS2_DCMCTL, ((0x1 << 8) | (0x1 << 0)));
            break;     
        default:
            dcm_dbg("[%s]: unknown dcm type 0x%x\n", __func__, type);
            break;
    }
}
EXPORT_SYMBOL(dcm_enable);

/**
 * dcm_disable - This function will disable DCM
 * @type: CA9_DCM, TOPAXI_DCM, EMI_DCM, PERI_DCM, MM1_DCM, MM2_DCM, ALL_DCM
 */
void dcm_disable(unsigned short type)
{
    unsigned int temp;
    
    switch (type)
    {
        case CA9_DCM:
            DRV_WriteReg32(TOP_DCMCTL, 0x0);
            break;
        case TOPAXI_DCM:
            DRV_WriteReg32(INFRA_DCMCTL, 0x0);
            break;
        case EMI_DCM:
            temp = DRV_Reg32(0xF00041DC);
            temp &= 0xFCFFFFFF;
            DRV_WriteReg32(0xF00041DC, temp);
            
            DRV_WriteReg32(INFRA_MISC, 0x0);
            DRV_WriteReg32(INFRA_DCMCTL, 0x0);
            break;
        case PERI_DCM:
            DRV_WriteReg32(PERI_GLOBALCON_DCMCTL, 0x0);
            break;
        case MM1_DCM:
            DRV_WriteReg32(SMI_LARB0_CON_RESET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB1_CON_RESET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB2_CON_RESET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB3_CON_RESET, (0x1 << 11));
            break;
        case MM2_DCM:
            DRV_WriteReg32(MMSYS2_DCMCTL, 0x0);
            break;
        case ALL_DCM:
            // CA9_DCM
            DRV_WriteReg32(TOP_DCMCTL, 0x0);
            
            // TOPAXI_DCM
            DRV_WriteReg32(INFRA_DCMCTL, 0x0);
        
            // EMI_DCM
            temp = DRV_Reg32(0xF00041DC);
            temp &= 0xFCFFFFFF;
            DRV_WriteReg32(0xF00041DC, temp);
            
            DRV_WriteReg32(INFRA_MISC, 0x0);
            DRV_WriteReg32(INFRA_DCMCTL, 0x0);

            // PERI_DCM
            DRV_WriteReg32(PERI_GLOBALCON_DCMCTL, 0x0);
            
            // MM1_DCM
            DRV_WriteReg32(SMI_LARB0_CON_RESET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB1_CON_RESET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB2_CON_RESET, (0x1 << 11));
            DRV_WriteReg32(SMI_LARB3_CON_RESET, (0x1 << 11));
            
            // MM2_DCM
            DRV_WriteReg32(MMSYS2_DCMCTL, 0x0);
            break;
        default:
            dcm_dbg("[%s]: unknown dcm type 0x%x\n", __func__, type);
            break;
    }
}
EXPORT_SYMBOL(dcm_disable);

void dcm_enable_all(void)
{
    dcm_enable(ALL_DCM);
}
EXPORT_SYMBOL(dcm_enable_all);

void dcm_disable_all(void)
{
    dcm_disable(ALL_DCM);
}
EXPORT_SYMBOL(dcm_disable_all);


/*
 ********************************** 
 *          idle/dpidle
 **********************************
 */
#ifdef CONFIG_SMP
extern volatile u32 cpu1_killed;
#endif

#ifdef CONFIG_LOCAL_TIMERS
#define WAKEUP_GPT  GPT4
#endif

#ifdef CONFIG_LOCAL_WDT
static unsigned long wdt_counter_pre = 0;
static unsigned long wdt_counter_pos = 0;
static unsigned long wdt_tick_pre = 0;
static unsigned long wdt_tick_pos = 0;

extern unsigned long mpcore_wdt_get_counter(void);
extern void mpcore_wdt_set_counter(unsigned long counter_value);
#endif

#ifdef CONFIG_MTK_MMC
extern void msdc_clk_status(int * status);
#else
void msdc_clk_status(int * status) { *status = 0; }
#endif

static int chip_ver = 0;

static int idle_switch = 1;     /* 1: enable idle,   0: disable idle */
static int dpidle_switch = 1;   /* 1: enable dpidle, 0: disable dpidle */

static unsigned int dpidle_level = 2;
static unsigned long dpidle_time_critera = 26000;
static int dpidle_state = 0;

static unsigned long dpidle_counter = 0; 
#ifndef CONFIG_LOCAL_TIMERS
static unsigned long dpidle_compare = 0;
static unsigned long dpidle_compare_update = 0;
#endif

static unsigned long idle_count[NR_CPUS] = {0};
static unsigned long dpidle_count = 0;

/* For debug and profile */
enum {
    DEBUG_TRACING   = 1U << 0,
    DEBUG_PROFILE   = 1U << 1,
};

static int dpidle_debug_mask = 0;
static unsigned int dpidle_block_mask[MT65XX_CLOCK_CATEGORY_COUNT];
static unsigned long dpidle_block_interval = 0;
static unsigned long dpidle_cm_block_cnt = 0;
static unsigned long dpidle_sc_block_cnt = 0;
static unsigned long dpidle_single_count = 0;
static unsigned long idle_single_count[NR_CPUS] = {0};

static unsigned int timer_delay = 0;
static unsigned long timer_pre = 0; 
static unsigned long timer_pos = 0; 

//#define PROFILE_IDLE
#ifdef PROFILE_IDLE
static unsigned long idle_tick_pre[NR_CPUS] = {0};
static unsigned long idle_tick_pos[NR_CPUS] = {0};
static unsigned long idle_counter[NR_CPUS] = {0};
#ifndef CONFIG_LOCAL_TIMERS
static unsigned long idle_compare[NR_CPUS] = {0};
#endif
static int idle_profile_idx[NR_CPUS] = {0};
static int idle_profile_mask = 0;
#endif

//#define PROFILE_DPIDLE
#ifdef PROFILE_DPIDLE
static unsigned long dpidle_tick_pre = 0; 
static unsigned long dpidle_tick_mid = 0;
static unsigned long dpidle_tick_pos = 0;
static unsigned int dpidle_wakeup_src = 0;
static int dpidle_profile_idx = 0;
#endif

#if 0
static unsigned int topmisc;
#endif
static int mmsys_switched_off = 0;

static unsigned int clk_mux_mm = 0;
#define mm_clk_pll2sq() \
do {    \
    clk_mux_mm = DRV_Reg32(TOP_CKMUXSEL);  \
    DRV_WriteReg32(TOP_CKMUXSEL, (clk_mux_mm & 0x03cf)); \
} while (0)

#define mm_clk_sq2pll() \
do {    \
    unsigned int temp = DRV_Reg32(TOP_CKMUXSEL);	\
    temp = (temp & 0x03cf) | (clk_mux_mm & 0x0030);	\
    DRV_WriteReg32(TOP_CKMUXSEL, temp); \
} while (0)


/* 
 * bit = 0, when the clock is on, CPU should keep at high speed
 */
static unsigned int hispeed_condition_mask[][MT65XX_CLOCK_CATEGORY_COUNT] = {
    {  
        /* for E2 */ 
        0x77980180, /* PERI_PDN0, SIM,HIF,IRDA,UART,I2C */
        0x00000000, /* PERI_PDN1, no skip bit */
        0x00000000, /* MMSYS1_PDN0, no skip bit */
        0x610E0120, /* MMSYS1_PDN1, LCD,R_DMA1,OVL_DMA_MIMO,OVL_DMA_BPS,OVL_DMA,JPEG_DMA,R_DMA0_MOUT,R_DMA0 */
        0x00000132, /* MMSYS1_PDN2, SPI,DPI,DSI */
        0x00084010, /* MMSYS2_PDN, G3D, M4U */
        0x00000060, /* AUDIO_PDN, ADC, I2S */
    }, {   
        /* for E1 */ 
        0x70001000, /* PERI_PDN0, APDMA,I2C */
        0x00000000, /* PERI_PDN1, no skip bit */
        0x00000000, /* MMSYS1_PDN0, no skip bit */
        0x610E0120, /* MMSYS1_PDN1, LCD,R_DMA1,OVL_DMA_MIMO,OVL_DMA_BPS,OVL_DMA,JPEG_DMA,R_DMA0_MOUT,R_DMA0 */
        0x00000132, /* MMSYS1_PDN2, SPI,DPI,DSI */
        0x00084010, /* MMSYS2_PDN, G3D, M4U */
        0x00000060, /* AUDIO_PDN, ADC, I2S */
    },
};


/* 
 * bit = 1, the clock bit can't be skipped, deep idle is skipped
 *          that is to say, if clock is on, we can't enter deep idle
 * bit = 0, the clock bit could be skipped or omit to enter deep idle
 *          not-clock bit will also be set zero
 */
static unsigned int dpidle_condition_mask[][MT65XX_CLOCK_CATEGORY_COUNT] = {
    {  
        /* for E2 */ 
        0x77980781, /* PERI_PDN0, NFI,SIM,USB,HIF,IRDA,UART,I2C */
        0x00000000, /* PERI_PDN1, no skip bit */
        0x00000000, /* MMSYS1_PDN0, no skip bit */
        0x610E0120, /* MMSYS1_PDN1, LCD,R_DMA1,OVL_DMA_MIMO,OVL_DMA_BPS,OVL_DMA,JPEG_DMA,R_DMA0_MOUT,R_DMA0 */
        0x00000132, /* MMSYS1_PDN2, SPI,DPI,DSI */
        0x00084010, /* MMSYS2_PDN, G3D, M4U */
        0x00000060, /* AUDIO_PDN, ADC, I2S */
    }, {   
        /* for E1 */ 
        0x70001601, /* PERI_PDN0, NFI,USB1,USB2,APDMA,I2C */
        0x00000000, /* PERI_PDN1, no skip bit */
        0x00000000, /* MMSYS1_PDN0, no skip bit */
        0x610E0120, /* MMSYS1_PDN1, LCD,R_DMA1,OVL_DMA_MIMO,OVL_DMA_BPS,OVL_DMA,JPEG_DMA,R_DMA0_MOUT,R_DMA0 */
        0x00000132, /* MMSYS1_PDN2, SPI,DPI,DSI */
        0x00084010, /* MMSYS2_PDN, G3D, M4U */
        0x00000060, /* AUDIO_PDN, ADC, I2S */
    },
};


static DEFINE_MUTEX(dpidle_condition);

/*
 * If the clock bit is on, we should keep CPU at high speed
 */
static void enable_hispeed_by_mask(int category, unsigned int mask)
{
    mutex_lock(&dpidle_condition);
    hispeed_condition_mask[chip_ver][category] &= ~mask;
    mutex_unlock(&dpidle_condition);
}

void enable_hispeed_by_bit(enum mt65xx_clock_id id)
{
    int category = id / 32;
    int offset = id % 32;
    BUG_ON(INVALID_CATEGORY_ID(category));
    enable_hispeed_by_mask(category, 0x1 << offset);
}
EXPORT_SYMBOL(enable_hispeed_by_bit);

/*
 * Even when the clock is on, we no need to keep CPU at high speed 
 */
static void disable_hispeed_by_mask(int category, unsigned int mask)
{
    mutex_lock(&dpidle_condition);
    hispeed_condition_mask[chip_ver][category] |= mask;
    mutex_unlock(&dpidle_condition);
}

void disable_hispeed_by_bit(enum mt65xx_clock_id id)
{
    int category = id / 32;
    int offset = id % 32;
    BUG_ON(INVALID_CATEGORY_ID(category));
    disable_hispeed_by_mask(category, 0x1 << offset);
}
EXPORT_SYMBOL(disable_hispeed_by_bit);


/*
 * Even when the clock bit is on, we can enter into dpidle
 */
static void enable_dpidle_by_mask(int category, unsigned int mask)
{
    mutex_lock(&dpidle_condition);
    dpidle_condition_mask[chip_ver][category] &= ~mask;
    hispeed_condition_mask[chip_ver][category] &= ~mask;
    mutex_unlock(&dpidle_condition);
}

void enable_dpidle_by_bit(enum mt65xx_clock_id id)
{
    int category = id / 32;
    int offset = id % 32;
    BUG_ON(INVALID_CATEGORY_ID(category));
    enable_dpidle_by_mask(category, 0x1 << offset);
}
EXPORT_SYMBOL(enable_dpidle_by_bit);

/*
 * If the clock bit is on, we can't enter into dpidle
 */
static void disable_dpidle_by_mask(int category, unsigned int mask)
{
    mutex_lock(&dpidle_condition);
    dpidle_condition_mask[chip_ver][category] |= mask;
    hispeed_condition_mask[chip_ver][category] |= mask;
    mutex_unlock(&dpidle_condition);
}

void disable_dpidle_by_bit(enum mt65xx_clock_id id)
{
    int category = id / 32;
    int offset = id % 32;
    BUG_ON(INVALID_CATEGORY_ID(category));
    disable_dpidle_by_mask(category, 0x1 << offset);
}
EXPORT_SYMBOL(disable_dpidle_by_bit);

/*
 * true: CPU should keep at high speed 
 *  scenario when CPU should keep at high speed: 
 *      1. meet dpidle condition
 *      2. USB works
 *      3. Nand works
 */
bool clkmgr_high_speed_check(void)
{
    int i = 0;
    unsigned int block_mask = 0;
    unsigned int sd_mask = 0;

    if (!get_clkmgr_init_state()) {
        return false;
    }

    msdc_clk_status(&sd_mask);
    if (sd_mask) {
        return false;
    }

    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {        
        block_mask = clk_mgr.clock_state[i] & hispeed_condition_mask[chip_ver][i];
        if (block_mask) {
            return false;  
        }
    }

    return true;
}
EXPORT_SYMBOL(clkmgr_high_speed_check);

static bool clkmgr_dpidle_can_enter(void)
{
    int i = 0;
    unsigned int block_mask = 0;
    unsigned int sd_mask = 0;

    if (!get_clkmgr_init_state()) {
        return false;
    }

    msdc_clk_status(&sd_mask);
    if (sd_mask) {
        if (dpidle_debug_mask & DEBUG_TRACING) {
            dpidle_block_mask[MT65XX_CLOCK_PERI_PDN0] |= sd_mask;
            dpidle_cm_block_cnt++;
        }
        return false;
    }

    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {        
        block_mask = clk_mgr.clock_state[i] & dpidle_condition_mask[chip_ver][i];
        if (block_mask) {
            if (dpidle_debug_mask & DEBUG_TRACING) {
                dpidle_block_mask[i] |= block_mask;
                dpidle_cm_block_cnt++;
            }
            return false;  
        }
    }

    return true;
}

bool sc_dpidle_can_enter(void)
{
#ifdef PROFILE_IDLE
    dpidle_tick_pre = GPT_GetCounter(GPT2);
#endif

#ifdef CONFIG_LOCAL_TIMERS
    dpidle_counter = localtimer_get_counter();
    if (dpidle_counter >= dpidle_time_critera) { // 13000 => 1ms
        GPT_SetCompare(WAKEUP_GPT, dpidle_counter);
        GPT_Start(WAKEUP_GPT);
        dpidle_state = 1;
        return true; 
    }

    if (dpidle_debug_mask & DEBUG_TRACING) {
        if (dpidle_counter > dpidle_block_interval) {
            dpidle_block_interval = dpidle_counter;
        }
        dpidle_sc_block_cnt++;
    }
#else /* !SMP */
    dpidle_counter = GPT_GetCounter(GPT1);
    dpidle_compare = GPT_GetCompare(GPT1);

    if (dpidle_compare >= dpidle_counter + dpidle_time_critera) { // 13000 => 1ms
        dpidle_state = 1;
        return true; 
    }

    if (dpidle_debug_mask & DEBUG_TRACING) {
        unsigned int delta = dpidle_compare - dpidle_counter;
        if (delta > dpidle_block_interval) {
            dpidle_block_interval = delta;
        }
        dpidle_sc_block_cnt++;
    }
#endif

    dpidle_state = 0;
    return false;
}

void sc_dpidle_before_wfi(void)
{
#if 0
    /* keep CA9 clock frequency when WFI to sleep */
    topmisc = DRV_Reg32(TOP_MISC);
    DRV_WriteReg32(TOP_MISC, (topmisc & ~(1U << 0)));
#endif
    
    if (unlikely(get_chip_eco_ver() == CHIP_E1)) {        
		DRV_ClrReg32(WPLL_CON0, 0x1);
    } else {
        if (!(clk_mgr.subsys_state & 0x30) && clk_mgr.mmsys_state) {
            mm_clk_pll2sq();
            DRV_SetReg16(MDPLL_CON0, 0x1);
            mmsys_switched_off = 1;
        }
    }

#ifndef CONFIG_LOCAL_TIMERS
    if (mmsys_switched_off) {
        dpidle_compare_update = dpidle_compare - dpidle_counter - 260;
    } else {
        dpidle_compare_update = dpidle_compare - dpidle_counter;
    }

    gpt_set_next_compare(dpidle_compare_update);
#endif

#ifdef CONFIG_LOCAL_WDT
    wdt_counter_pre = mpcore_wdt_get_counter();
    wdt_tick_pre = GPT_GetCounter(GPT2);
#endif

#ifdef PROFILE_DPIDLE
    dpidle_tick_mid = GPT_GetCounter(GPT2);
#endif
}

void sc_dpidle_after_wfi(void)
{
#ifdef PROFILE_DPIDLE
    dpidle_tick_pos = GPT_GetCounter(GPT2);
    dpidle_wakeup_src = DRV_Reg32(SC_WAKEUP_SRC);
    if (dpidle_debug_mask & DEBUG_PROFILE) {
#ifdef CONFIG_LOCAL_TIMERS
        dcm_info("[%s]%5d %10u %10u %10u %10u %08x\n", __func__, 
                dpidle_profile_idx, dpidle_tick_pre, dpidle_tick_mid, dpidle_tick_pos,
                dpidle_counter, dpidle_wakeup_src);
#else
        dcm_info("[%s]%5d %10u %10u %10u %10u %10u %10u %08x\n", __func__, 
                dpidle_profile_idx, dpidle_tick_pre, dpidle_tick_mid, dpidle_tick_pos,
                dpidle_counter, dpidle_compare, dpidle_compare_update, dpidle_wakeup_src);

#endif
        dpidle_profile_idx++;
    }
#endif

#ifdef CONFIG_LOCAL_WDT
    wdt_tick_pos = GPT_GetCounter(GPT2);
    if (wdt_counter_pre > (wdt_tick_pos - wdt_tick_pre)) {
        wdt_counter_pos = wdt_counter_pre - (wdt_tick_pos - wdt_tick_pre);
        mpcore_wdt_set_counter(wdt_counter_pos);
    } else {
        dcm_info("[%s]:wdt_counter_pre=%10lu, wdt_tick_pre=%10lu, wdt_tick_pos=%10lu\n", 
                __func__, wdt_counter_pre, wdt_tick_pre, wdt_tick_pos);
        mpcore_wdt_set_counter(1);
    }
#endif

#ifdef CONFIG_LOCAL_TIMERS
    if (GPT_Get_IRQ(WAKEUP_GPT)) {
        /* waked up by WAKEUP_GPT */
        localtimer_set_next_event(1);
    } else {
        /* waked up by other wakeup source */
        unsigned int temp1 = GPT_GetCompare(WAKEUP_GPT);
        unsigned int temp2 = GPT_GetCounter(WAKEUP_GPT);
        if (unlikely(temp1 <= temp2)) {
            dcm_err("[%s]GPT%d: counter = %10u, compare = %10u\n", __func__, temp1, temp2);
            BUG();
        }

        localtimer_set_next_event(temp1-temp2);
        GPT_Stop(WAKEUP_GPT);
        GPT_ClearCount(WAKEUP_GPT);
    }
#endif

    if (get_chip_eco_ver() == CHIP_E1) {
        DRV_SetReg32(WPLL_CON0, 0x1);        
    } else {
        if (mmsys_switched_off) {
            DRV_ClrReg16(MDPLL_CON0, 0x1);
            udelay(20);
            mm_clk_sq2pll();
            mmsys_switched_off = 0;
        }
    }   

#if 0
    /* restore TOP_MISC */
    DRV_WriteReg32(TOP_MISC, topmisc);
#endif

    dpidle_count++;
    if ((dpidle_debug_mask & DEBUG_TRACING)) {
        dpidle_single_count++;
    }
}

#ifdef CONFIG_SMP
#define CPU_NUM 2
#define LOG_NR 10
typedef struct {
	unsigned long long time;
	char* flag;
}mtk_log;
mtk_log idle_log[CPU_NUM][LOG_NR];

static void log_idle(int cpu, char* str)
{
	static int i = 0;
	static int j = 0;
	int k = 0;

	k = (cpu == 0 ? i:j);
	idle_log[cpu][k].time = sched_clock();
	idle_log[cpu][k].flag = str;
	if (++k == LOG_NR) k = 0;
	if (cpu){
		j = k;
	} else {
		i = k;
	}
}

void dump_log_idle(void)
{
	int i = 0;
	int j = 0;
	for (;i < CPU_NUM; i++){
		for (;j < LOG_NR; j++){
      dcm_info("[%s]cpu%d %s%llu ns\n", __func__, i, idle_log[i][j].flag, idle_log[i][j].time);
		}
		j = 0;
	}
}
#endif

static void idle_before_wfi(int cpu)
{
#ifdef PROFILE_IDLE
    idle_tick_pre[cpu] = GPT_GetCounter(GPT2);

#ifdef CONFIG_LOCAL_TIMERS
    idle_counter[cpu] = localtimer_get_counter();
#else
    idle_counter[cpu] = GPT_GetCounter(GPT1);
    idle_compare[cpu] = GPT_GetCompare(GPT1);	
#endif
#endif

#ifdef CONFIG_SMP
		log_idle(cpu,"+");
#endif
}

static void idle_after_wfi(int cpu)
{
#ifdef CONFIG_SMP
		log_idle(cpu,"-");
#endif
#ifdef PROFILE_IDLE
    idle_tick_pos[cpu] = GPT_GetCounter(GPT2);

    if (idle_profile_mask & (0x1 << cpu)) {
#ifdef CONFIG_LOCAL_TIMERS
        dcm_info("[%s:cpu%d]%5d %10u %10u %10u\n", __func__, cpu, 
                idle_profile_idx[cpu], idle_tick_pre[cpu],
                idle_tick_pos[cpu], idle_counter[cpu]);
#else
        dcm_info("[%s:cpu%d]%5d %10u %10u %10u %10u\n", __func__, cpu, 
                idle_profile_idx[cpu], idle_tick_pre[cpu], 
                idle_tick_pos[cpu], idle_counter[cpu], idle_compare[cpu]);
#endif
        idle_profile_idx[cpu]++;
    }
#endif

    idle_count[cpu]++;
    if ((dpidle_debug_mask & DEBUG_TRACING)) {
        idle_single_count[cpu]++;
    }
}

static void noinline go_to_idle(void)
{
    int cpu = smp_processor_id();

    idle_before_wfi(cpu);

    dsb();
    __asm__ __volatile__("wfi" ::: "memory");

    idle_after_wfi(cpu);
}

static int hwe_protect_dummy(int cpunum)
{
    printk(KERN_ERR "**Cannot find HWE module**\n");
    return 0;    
}

static int arch_idle_count = 0;
#define ARCH_IDLE_COUNT_MAX 5000

static int (*hwe_protect)(int) = &hwe_protect_dummy;
EXPORT_SYMBOL(hwe_protect);

void arch_idle(void)
{
#ifdef CONFIG_SMP
    if (smp_processor_id() == 0) {
#endif
        arch_idle_count++;
        if (arch_idle_count >= ARCH_IDLE_COUNT_MAX)
        {
            if ((*hwe_protect)(num_online_cpus()) == -1)
                printk(KERN_ERR "Detect Conflict: ARMPLL = 0x%x, CPU NUM = %d\n", DRV_Reg32(ARMPLL_CON0), num_online_cpus());
            arch_idle_count = 0;
        }
#ifdef CONFIG_SMP
    }
#endif

    aee_rr_rec_idle_jiffies(jiffies);

#ifdef CONFIG_SMP
    if (dpidle_switch && (smp_processor_id() == 0) && (num_online_cpus() == 1) && (cpu1_killed == 1)) {
#else
    if (dpidle_switch) {
#endif
        if (clkmgr_dpidle_can_enter()) {
            sc_go_to_dpidle(dpidle_level);
        }
        
        if (dpidle_state == 1) {
            dpidle_state = 0;
            aee_rr_rec_idle_jiffies(0);
            return;
        }
    } 

    if (idle_switch) {
        go_to_idle();
    }

    aee_rr_rec_idle_jiffies(0);
}

/*
 ********************************** 
 * dpidle/idle/dcm debug/dump info
 **********************************
 */

#define dcm_attr(_name)                         \
static struct kobj_attribute _name##_attr = {   \
    .attr = {                                   \
        .name = __stringify(_name),             \
        .mode = 0644,                           \
    },                                          \
    .show = _name##_show,                       \
    .store = _name##_store,                     \
}

extern struct kobject *power_kobj;

static ssize_t dpidle_state_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int len = 0;
    char *p = buf;
    int i = 0;

    p += sprintf(p, "*********** Deep Idle State ************\n");
    for (i = 0; i < nr_cpu_ids; i++) {
        p += sprintf(p, "idle_count[%d] = %lu ", i, idle_count[i]);
    }
    p += sprintf(p, "\n");
    p += sprintf(p, "dpidle_count = %lu\n", dpidle_count);
    p += sprintf(p, "idle_switch = %d, dpidle_switch = %d\n", idle_switch, dpidle_switch);
    p += sprintf(p, "dpidle_level = %u\n", dpidle_level);
    p += sprintf(p, "dpidle_time_critera = %lu\n", dpidle_time_critera);
    p += sprintf(p, "dpidle_debug_mask = %d\n", dpidle_debug_mask);
    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {
        p += sprintf(p, "dpidle_condition_mask[%d][%d] = 0x%08x, hispeed_condition_mask[%d][%d] = 0x%08x\n", 
            chip_ver, i, dpidle_condition_mask[chip_ver][i],
            chip_ver, i, hispeed_condition_mask[chip_ver][i]);
    }

    for (i = 0; i < nr_cpu_ids; i++) {
        p += sprintf(p, "idle_single_count[%d] = %lu ", i, idle_single_count[i]);
    }
    p += sprintf(p, "\n");
    p += sprintf(p, "dpidle_single_count = %lu\n", dpidle_single_count);
    p += sprintf(p, "dpidle_cm_block_cnt = %lu, dpidle_sc_block_cnt = %lu\n", dpidle_cm_block_cnt, dpidle_sc_block_cnt);
    p += sprintf(p, "dpidle_block_interval = %lu\n", dpidle_block_interval);
    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {
        p += sprintf(p, "dpidle_block_mask[%d] = 0x%08x\n", i, dpidle_block_mask[i]);
    }

    p += sprintf(p, "timer_delay = %u, timer_pre = %lu, timer_pos = %lu\n", timer_delay, timer_pre, timer_pos);
#ifdef PROFILE_IDLE
    p += sprintf(p, "idle_profile_mask = %d\n", idle_profile_mask);
#endif

    p += sprintf(p, "\n*********** Deep Idle Command Help ************\n");
    p += sprintf(p, "enable idle:               echo idle 1 > /sys/power/dpidle_state\n");
    p += sprintf(p, "disable idle:              echo idle 0 > /sys/power/dpidle_state\n");
    p += sprintf(p, "enable deep idle:          echo [dpidle] 1 > /sys/power/dpidle_state\n");
    p += sprintf(p, "disable deep idle:         echo [dpidle] 0 > /sys/power/dpidle_state\n");
    p += sprintf(p, "modify deep idle level:    echo level 0/1/2 > /sys/power/dpidle_state\n");
    p += sprintf(p, "modify time critera:       echo time value(decimal) > /sys/power/dpidle_state\n");
    p += sprintf(p, "enable dpidle condition:   echo enable id > /sys/power/dpidle_state\n");
    p += sprintf(p, "disable dpidle condition:  echo disable id > /sys/power/dpidle_state\n");
    p += sprintf(p, "enable hispeed condition:  echo enable_hi id > /sys/power/dpidle_state\n");
    p += sprintf(p, "disable hispeed condition: echo disable_hi id > /sys/power/dpidle_state\n");
    p += sprintf(p, "start single tracing:      echo trace 1 > /sys/power/dpidle_state\n");
    p += sprintf(p, "stop single tracing:       echo trace 0 > /sys/power/dpidle_state\n");
    p += sprintf(p, "test GPT timer :           echo timer 5 > /sys/power/dpidle_state\n");
#ifdef PROFILE_IDLE
    p += sprintf(p, "switch idle profile:       echo profile on/off cpumask(hex) > /sys/power/dpidle_state\n");
#endif
#ifdef PROFILE_DPIDLE
    p += sprintf(p, "switch dpidle profile:     echo profile 0/1 > /sys/power/dpidle_state\n");
#endif

    len = p - buf;
    return len;
}

static ssize_t dpidle_state_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t n)
{
    char command[32];
#ifdef PROFILE_IDLE
    char sec_cmd[32];
    int i = 0;
#endif
    int temp;

#ifdef PROFILE_IDLE
    if (sscanf(buf, "%s %s %x", command, sec_cmd, &temp) == 3) {
        if (!strcmp(command, "profile")) {
            if (!strcmp(sec_cmd, "on")) {
                idle_profile_mask |= temp;
                for (i = 0; i < nr_cpu_ids; i++) {
                    //memset(idle_profile_idx, 0, NR_CPUS * sizeof(int));
                    if (temp & (0x1 << i)) { 
                        idle_profile_idx[i] = 0;
                    }
                }
            } else if (!strcmp(sec_cmd, "off")) {
                idle_profile_mask &= ~temp;
            }
        }
        printk("[test]idle_profile_mask=0x%x\n", idle_profile_mask);
    } else
#endif
    if (sscanf(buf, "%s %d", command, &temp) == 2) {
        if (!strcmp(command, "idle")) {
            idle_switch = temp;
        } else if (!strcmp(command, "dpidle")) {
            dpidle_switch = temp;
        } else if (!strcmp(command, "level")) {
			if (unlikely(temp < 0 || temp > 2)) {
				dcm_warn("[%s]invalid dpidle_level %d\n", __func__, temp);
			} else {
	            dpidle_level = (unsigned int)temp;
			}
        } else if (!strcmp(command, "time")) {
            dpidle_time_critera = temp;
        } else if (!strcmp(command, "enable")) {
            enable_dpidle_by_bit(temp);
        } else if (!strcmp(command, "disable")) {
            disable_dpidle_by_bit(temp);
        } else if (!strcmp(command, "enable_hi")) {
            enable_hispeed_by_bit(temp);
        } else if (!strcmp(command, "disable_hi")) {
            disable_hispeed_by_bit(temp);
        } else if (!strcmp(command, "trace")) {
            if (temp == 1) {
                memset(idle_single_count, 0, NR_CPUS * sizeof(unsigned int));
                dpidle_single_count = 0;
                dpidle_cm_block_cnt = 0;
                dpidle_sc_block_cnt = 0;
                dpidle_block_interval = 0;
                memset(dpidle_block_mask, 0, MT65XX_CLOCK_CATEGORY_COUNT * sizeof(unsigned int));
                dpidle_debug_mask |= DEBUG_TRACING;
            } else if (temp == 0) {
                dpidle_debug_mask &= ~DEBUG_TRACING;
            }
        } else if (!strcmp(command, "timer")) {
            timer_delay = temp;
            timer_pre = GPT_GetCounter(GPT2);
            udelay(timer_delay);
            timer_pos = GPT_GetCounter(GPT2);
#ifdef PROFILE_DPIDLE
        } else if (!strcmp(command, "profile")) {
            if (temp == 1) {
                dpidle_debug_mask |= DEBUG_PROFILE;
                dpidle_profile_idx = 0;
            } else if (temp == 0) {
                dpidle_debug_mask &= ~DEBUG_PROFILE;
            }
#endif
        }
        return n;
    } else if (sscanf(buf, "%x", &temp) == 1) {
        dpidle_switch = temp;
        return n;
    }

    return -EINVAL;
}
dcm_attr(dpidle_state);

#define IDLE_LOW_POWER_TEST
#define TEST_ARCH_IDLE  0
#define MODE_SINGLE_CPU_IDLE 

static DEFINE_SPINLOCK(factory_lock);

#ifdef CONFIG_SMP
static DEFINE_MUTEX(ftm_cpu_prepare);
extern void disable_hotplug_policy(bool disable, int limit_cpu_num);
#endif

#ifdef CONFIG_LOCAL_WDT
extern void mpcore_wk_wdt_stop(void);
#endif

extern int mt6577_irq_mask_all(struct mtk_irq_mask *mask);
extern int mt6577_irq_mask_restore(struct mtk_irq_mask *mask);
extern void mt6577_irq_unmask_for_sleep(unsigned int irq);

extern void mtk_wdt_disable(void);

#if defined(CONFIG_MTK_FB)
extern int DISP_PanelEnable(BOOL bEnable);
extern int DISP_PowerEnable(BOOL bEnable);
#endif


#ifdef MODE_SINGLE_CPU_IDLE
static struct mtk_irq_mask mask;

#ifdef IDLE_LOW_POWER_TEST
void enable_low_power_settings(void)
{
    int ret;
    dcm_ver("[%s]: entry\n", __func__);
    
    // turn off backlight
#if defined(CONFIG_MTK_LEDS)
    mt65xx_leds_brightness_set(MT65XX_LED_TYPE_LCD, 0);
#endif
    
    // turn off LCD
    //#if defined(CONFIG_MTK_FB)
    #if 0
    DISP_PanelEnable(FALSE);
    DISP_PowerEnable(FALSE);	
    #endif
    
    // save interrupt status and mask all interrupt
    ret = mt6577_irq_mask_all(&mask);
    if (!ret) {
        dcm_ver("[%s]: success to mask all irq lines\n", __func__);
    } else {
        dcm_err("[%s]: fail to mask all irq lines\n", __func__);
    }
    
    mt6577_irq_unmask_for_sleep(MT6577_KP_IRQ_ID);
}

void disable_low_power_settings(void)
{
    int ret;
    dcm_ver("[%s]: entry\n", __func__);
    
    // restore interrupt mask
    ret = mt6577_irq_mask_restore(&mask);
    if (!ret) {
        dcm_ver("[%s]: success to restore all irq lines\n", __func__);
    } else {
        dcm_err("[%s]: fail to restore all irq lines\n", __func__);
    }

    // turn on LCD
    //#if defined(CONFIG_MTK_FB)
    #if 0
    DISP_PowerEnable(TRUE);
    DISP_PanelEnable(TRUE);	
    #endif
    
    // turn on backlight
#if defined(CONFIG_MTK_LEDS)
    mt65xx_leds_brightness_set(MT65XX_LED_TYPE_LCD, LED_FULL);
#endif
}
#endif

void factory_cpu0_idle_test(void)
{
    int cpu = 0;
#ifdef CONFIG_SMP
    int i = 0;
    int ret = 0;
    int cpu_pwrdn_flag[nr_cpu_ids];
#endif

    spin_lock(&factory_lock);
    cpu = smp_processor_id();
    spin_unlock(&factory_lock);

    printk("[%s]it's cpu%d\n", __func__, cpu);

#ifdef CONFIG_SMP
    mutex_lock(&ftm_cpu_prepare);
    disable_hotplug_policy(true, nr_cpu_ids);
    memset(cpu_pwrdn_flag, 0, nr_cpu_ids * sizeof(int));
    for (i = 1; i < nr_cpu_ids; i++) {
        if (cpu_online(i)) {
            cpu_pwrdn_flag[i] = 1;
            ret = cpu_down(i);
            dcm_info("[%s]cpu_down(cpu%d) return %d, cpu1_killed=%u\n", __func__, i, ret, cpu1_killed);
        } else {
            dcm_info("[%s]no need to power down cpu%d\n", __func__, i);
        }
    }
    mutex_unlock(&ftm_cpu_prepare);
#endif

#ifdef CONFIG_LOCAL_WDT
    mpcore_wk_wdt_stop();
#endif
    mtk_wdt_disable(); // disable watch dog
    
    //this should be set by low power requirement.
#ifdef IDLE_LOW_POWER_TEST
    enable_low_power_settings();
#endif
    local_irq_disable();
    go_to_idle();
    local_irq_enable();
#ifdef IDLE_LOW_POWER_TEST
    disable_low_power_settings();
#endif

#ifdef CONFIG_SMP
    mutex_lock(&ftm_cpu_prepare);
    for (i = 1; i < nr_cpu_ids; i++) {
        if (cpu_pwrdn_flag[i] == 1) {
            ret = cpu_up(i);
            dcm_info("[%s]cpu_up(cpu%d) return %d, cpu1_killed=%u\n", __func__, i, ret, cpu1_killed);
        } else {
            dcm_info("[%s]no need to power up cpu%d\n", __func__, i);
        }
    }
    disable_hotplug_policy(false, nr_cpu_ids);
    mutex_unlock(&ftm_cpu_prepare);
#endif
}

#else

#include <asm/hardware/gic.h>
#include <linux/kthread.h>

static struct mtk_irq_mask cpu_irq_mask[NR_CPUS];
static DECLARE_COMPLETION(all_threads_done);
static struct completion each_thread_done[NR_CPUS];
static int cpuid[NR_CPUS] = {0};
#if 0
static int wake_flag[NR_CPUS] = {0};
#endif

static int primary_cpu_enter_wfi(void *data)
{
#if 0
    int i = 0;
#endif
    unsigned int irqstat = 0;
    int cpu = *(int*)data;

    dcm_info("[%s]: thread idle-%d start\n", __func__, cpu);

#ifdef CONFIG_LOCAL_WDT
    mpcore_wk_wdt_stop();
#endif

    mt6577_irq_mask_all(&cpu_irq_mask[cpu]);
    mt6577_irq_unmask_for_sleep(MT6577_KP_IRQ_ID);

    dcm_info("[%s]: cpu%d waiting all threads done\n", __func__, cpu);
    complete(&each_thread_done[cpu]);
    wait_for_completion(&all_threads_done);
    local_irq_disable();
    dcm_info("[%s]: cpu%d before wfi\n", __func__, cpu);

#if 0
    do {
        go_to_idle();
        irqstat = DRV_Reg32(GIC_CPU_BASE + GIC_CPU_INTACK) & 0x3FF; 
        dcm_info("[%s]: cpu%d after wfi(irqstat:0x%x/%u)\n", __func__, cpu, irqstat, irqstat);
        if (irqstat == MT6577_KP_IRQ_ID) {
            break;
        }
    } while (1);
#else
    go_to_idle();
    irqstat = DRV_Reg32(GIC_CPU_BASE + GIC_CPU_INTACK) & 0x3FF; 
    dcm_info("[%s]: cpu%d after wfi(irqstat:0x%x/%u)\n", __func__, cpu, irqstat, irqstat);
#endif

    local_irq_enable();

    spin_lock(&factory_lock);
    mt6577_irq_mask_restore(&cpu_irq_mask[cpu]);
    spin_unlock(&factory_lock);

#if 0
    for (i = 1; i < nr_cpu_ids; i++) {
        wake_flag[i] = 1;
        smp_send_reschedule(i);
    }
#endif
    
#if defined(CONFIG_MTK_LEDS)
    mt65xx_leds_brightness_set(MT65XX_LED_TYPE_LCD, LED_FULL);
#endif

    dcm_info("[%s]: thread idle-%d end\n", __func__, cpu);
    return 0;
}

#ifdef CONFIG_SMP
static int secondary_cpu_enter_wfi(void *data)
{
    int cpu = *(int*)data;
    unsigned int irqstat;

    dcm_info("[%s]: thread idle-%d start\n", __func__, cpu);

#ifdef CONFIG_LOCAL_WDT
    mpcore_wk_wdt_stop();
#endif

    mt6577_irq_mask_all(&cpu_irq_mask[cpu]);

    dcm_info("[%s]: cpu%d waiting all threads done\n", __func__, cpu);
    complete(&each_thread_done[cpu]);
    wait_for_completion(&all_threads_done);
    local_irq_disable();
    dcm_info("[%s]: cpu%d before wfi\n", __func__, cpu);

#if 0
    while (!wake_flag[cpu]) {
        go_to_idle();
        irqstat = DRV_Reg32(GIC_CPU_BASE + GIC_CPU_INTACK) & 0x3FF; 
        dcm_info("[%s]: cpu%d after wfi(irqstat:0x%x/%u)\n", __func__, cpu, irqstat, irqstat);
    }
#else
    go_to_idle();
    irqstat = DRV_Reg32(GIC_CPU_BASE + GIC_CPU_INTACK) & 0x3FF; 
    dcm_info("[%s]: cpu%d after wfi(irqstat:0x%x/%u)\n", __func__, cpu, irqstat, irqstat);
#endif

    local_irq_enable();

    spin_lock(&factory_lock);
    mt6577_irq_mask_restore(&cpu_irq_mask[cpu]);
    spin_unlock(&factory_lock);
    dcm_info("[%s]: thread idle-%d end\n", __func__, cpu);
    return 0;
}
#endif

static int (*cpu_enter_wfi[NR_CPUS])(void *data) = {
    [0] = primary_cpu_enter_wfi,
#ifdef CONFIG_SMP
    [1 ... NR_CPUS-1] = secondary_cpu_enter_wfi,
#endif
};


void factory_cpus_idle_test(void)
{
    int cpu = 0;
    int i = 0;
    unsigned char name[10] = {'\0'};
    struct task_struct *thread[nr_cpu_ids];
#ifdef CONFIG_SMP
    int ret = 0;
#endif

    spin_lock(&factory_lock);
    cpu = smp_processor_id();
    spin_unlock(&factory_lock);
    dcm_info("[%s]: it's cpu%d, num_online_cpus=%d\n", __func__, cpu, num_online_cpus());

#ifdef CONFIG_SMP
    mutex_lock(&ftm_cpu_prepare);
    disable_hotplug_policy(true, nr_cpu_ids);
    for (i = 1; i < nr_cpu_ids; i++) {
        ret = cpu_up(i);
        dcm_info("[%s]cpu_up(cpu%d) return %d, cpu1_killed=%u\n", __func__, i, ret, cpu1_killed);
    }
    mutex_unlock(&ftm_cpu_prepare);
#endif

    mtk_wdt_disable(); // disable watch dog

    // turn off backlight
#if defined(CONFIG_MTK_LEDS)
    mt65xx_leds_brightness_set(MT65XX_LED_TYPE_LCD, 0);
#endif

    for (i = nr_cpu_ids-1; i >= 0; i--) {
        cpuid[i] = i;
        init_completion(&each_thread_done[i]);
        sprintf(name, "idle-%d", i);
        thread[i] = kthread_create(cpu_enter_wfi[i], &cpuid[i], name);
        if (IS_ERR(thread[i])) {
            int ret = PTR_ERR(thread[i]);
            thread[i] = NULL;
            dcm_info("[%s]: kthread_create %s fail(%d)\n", __func__, name, ret);
            return;
        }
        dcm_info("[%s]: kthread_create %s done\n", __func__, name);
        kthread_bind(thread[i], i);
        dcm_info("[%s]: kthread_bind %s done\n", __func__, name);
        wake_up_process(thread[i]);
        dcm_info("[%s]: wake_up_process %s done\n", __func__, name);
        wait_for_completion(&each_thread_done[i]);
    }
    dcm_info("[%s]: cpu%d starts to complete_all all_threads_done\n", __func__, cpu);
    complete_all(&all_threads_done);
}
#endif

static ssize_t idle_state_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int len = 0;
    char *p = buf;

#ifdef CONFIG_SMP
		dump_log_idle();
#endif
			
    p += sprintf(p, "=============== Idle State ===============\n");
    p += sprintf(p, "unused for now\n");
    p += sprintf(p, "=========================================\n");

    len = p - buf;
    return len;
}

static ssize_t idle_state_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t n)
{
    unsigned int test_mode;

    dcm_ver("\n=============== Idle State ================\n");
    if (sscanf(buf, "%d", &test_mode) == 1) {
        if (test_mode == TEST_ARCH_IDLE) {
#ifdef MODE_SINGLE_CPU_IDLE
            factory_cpu0_idle_test();
#else
            factory_cpus_idle_test();
#endif
            dcm_ver("=========================================\n");
            return n;
        } else {
            dcm_err("bad argument!!\n");
        }
    } else {
        dcm_err("bad argument!!\n");
    }

    dcm_ver("=========================================\n");

    return -EINVAL;
}
dcm_attr(idle_state);

static ssize_t dcm_state_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int len = 0;
    char *p = buf;
    
    if (DRV_Reg32(TOP_DCMCTL) != 0x6)
        p += sprintf(p, "disable - top (0x%x)\n", DRV_Reg32(TOP_DCMCTL));
    else if ((DRV_Reg32(INFRA_DCMCTL) & 0x0001) != 0x0001)
        p += sprintf(p, "disable - infra (0x%x)\n", DRV_Reg32(INFRA_DCMCTL));
    else if ((DRV_Reg32(PERI_GLOBALCON_DCMCTL) & 0x0001) != 0x0001)
        p += sprintf(p, "disable - peri (0x%x)\n", DRV_Reg32(PERI_GLOBALCON_DCMCTL));
    else if ((DRV_Reg32(MMSYS2_DCMCTL) & 0x0001) != 0x0001)
        p += sprintf(p, "disable - mmsys2 (0x%x)\n", DRV_Reg32(MMSYS2_DCMCTL));
    else if ((DRV_Reg32(SMI_LARB0_CON) & 0x0800) != 0x0800)
        p += sprintf(p, "disable - mmsys1 larb0 (0x%x)\n", DRV_Reg32(SMI_LARB0_CON));
    else if ((DRV_Reg32(SMI_LARB1_CON) & 0x0800) != 0x0800)
        p += sprintf(p, "disable - mmsys1 larb1 (0x%x)\n", DRV_Reg32(SMI_LARB1_CON));
    else if ((DRV_Reg32(SMI_LARB2_CON) & 0x0800) != 0x0800)
        p += sprintf(p, "disable - mmsys1 larb2 (0x%x)\n", DRV_Reg32(SMI_LARB2_CON));
    else if ((DRV_Reg32(SMI_LARB3_CON) & 0x0800) != 0x0800)
        p += sprintf(p, "disable - mmsys1 larb3 (0x%x)\n", DRV_Reg32(SMI_LARB3_CON));
    else
        p += sprintf(p, "enable\n");

    len = p - buf;
    return len;
}

static ssize_t dcm_state_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t n)
{
    char dcm_mode[20];

    dcm_ver("\n=============== DCM State ================\n");
    if (sscanf(buf, "%s", dcm_mode) == 1) {
        if (!strncmp(dcm_mode, "enable", 6)) {
            dcm_dbg("enable dcm\n");
            dcm_enable_all();
            dcm_ver("==========================================\n");
            return n;
        } else if (!strncmp(dcm_mode, "disable", 7)) {
            dcm_dbg("disable dcm\n");
            dcm_disable_all();
            dcm_ver("==========================================\n");
            return n;
        } else {
            dcm_err("bad argument!! argument should be \"enable\" or \"disable\"\n");
            dcm_ver("==========================================\n");
        }
    } else {
        dcm_err("bad argument!!\n");
        dcm_ver("==========================================\n");
    }
    
    return -EINVAL;
}
dcm_attr(dcm_state);

void mt6577_dcm_init(void)
{
    int ret = 0;
#ifdef CONFIG_LOCAL_TIMERS
    GPT_CONFIG gpt_config = {
        .num = WAKEUP_GPT,
        .mode = GPT_ONE_SHOT,
        .clkSrc = GPT_CLK_SRC_SYS,
        .clkDiv = 0x0000,
        .bIrqEnable = true,
        .u4CompareL = 0x0,
    };
#endif
    
    dcm_info("[%s]: enable HW dcm\n", __func__);
    dcm_enable_all();

    chip_ver = (get_chip_eco_ver() == CHIP_E1) ? 1 : 0; 
    memset(dpidle_block_mask, 0, MT65XX_CLOCK_CATEGORY_COUNT * sizeof(unsigned int));

#ifdef CONFIG_LOCAL_TIMERS
    GPT_Init(gpt_config.num, NULL);
    if (GPT_Config(gpt_config) == FALSE) {
        dcm_err("[%s]: config GPT%d failed\n", __func__, gpt_config.num);
    }
#endif
    
    ret = sysfs_create_file(power_kobj, &dcm_state_attr.attr);
    ret = sysfs_create_file(power_kobj, &idle_state_attr.attr);
    ret = sysfs_create_file(power_kobj, &dpidle_state_attr.attr);

    if (ret) {
        dcm_err("[%s]: create sysfs failed\n", __func__);
    }
}

void mt6577_dcm_exit(void)
{
    dcm_info("[%s]: disable HW dcm\n", __func__);
    dcm_disable_all();
}
