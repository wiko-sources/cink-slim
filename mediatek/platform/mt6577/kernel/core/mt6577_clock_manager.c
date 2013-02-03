#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>

#include "mach/mt6577_boot.h"
#include "mach/mt6577_typedefs.h"
#include "mach/mt6577_irq.h"
#include "mach/mtk_rtc.h"
#include "mach/mt6577_clock_manager.h"
#include "mach/mt6577_clkmgr_internal.h"
#include "mach/mt6577_cpe.h"


#if defined(CONFIG_MT6577_FPGA)
#define GIC_PRIVATE_SIGNALS                 32
#define MT6577_SMI_LARB0_MMU_IRQ_ID         (GIC_PRIVATE_SIGNALS + 75)
#define MT6577_SMI_LARB1_MMU_IRQ_ID         (GIC_PRIVATE_SIGNALS + 77)
#define MT6577_SMI_LARB2_MMU_IRQ_ID         (GIC_PRIVATE_SIGNALS + 79)
#define MT6577_LARB3_MMU_IRQ_ID             (GIC_PRIVATE_SIGNALS + 90)
#endif


#define USING_XLOG

#ifdef USING_XLOG 
#include <linux/xlog.h>

#define TAG     "Power/clkmgr"

#define clk_err(fmt, args...)       \
    xlog_printk(ANDROID_LOG_ERROR, TAG, fmt, ##args)
#define clk_warn(fmt, args...)      \
    xlog_printk(ANDROID_LOG_WARN, TAG, fmt, ##args)
#define clk_info(fmt, args...)      \
    xlog_printk(ANDROID_LOG_INFO, TAG, fmt, ##args)
#define clk_dbg(fmt, args...)       \
    xlog_printk(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define clk_ver(fmt, args...)       \
    xlog_printk(ANDROID_LOG_VERBOSE, TAG, fmt, ##args)

#else

#define TAG     "[Power/clkmgr] "

#define clk_err(fmt, args...)       \
    printk(KERN_ERR TAG);           \
    printk(KERN_CONT fmt, ##args) 
#define clk_warn(fmt, args...)      \
    printk(KERN_WARNING TAG);       \
    printk(KERN_CONT fmt, ##args)
#define clk_info(fmt, args...)      \
    printk(KERN_NOTICE TAG);        \
    printk(KERN_CONT fmt, ##args)
#define clk_dbg(fmt, args...)       \
    printk(KERN_INFO TAG);          \
    printk(KERN_CONT fmt, ##args)
#define clk_ver(fmt, args...)       \
    printk(KERN_DEBUG TAG);         \
    printk(KERN_CONT fmt, ##args)

#endif

static int enable_subsys_locked(enum mt65xx_subsys_id id, char *mod_name);
static int disable_subsys_locked(enum mt65xx_subsys_id id, char *mod_name);
static void enable_larb_internal(int larb_idx, char* mod_name);
static void disable_larb_internal(int larb_idx, char* mod_name);

/* 
 * clock bits number in every category, include those bits AP won't operate with 
 */
const int mt65xx_clock_number_per_category[MT65XX_CLOCK_CATEGORY_COUNT] = {
    (MT65XX_PERI_PDN0_END - MT65XX_PERI_PDN0_BEGIN + 1),
    (MT65XX_PERI_PDN1_END - MT65XX_PERI_PDN1_BEGIN + 1),
    (MT65XX_MM1_PDN0_END - MT65XX_MM1_PDN0_BEGIN + 1),
    (MT65XX_MM1_PDN1_END - MT65XX_MM1_PDN1_BEGIN + 1),
    (MT65XX_MM1_PDN2_END - MT65XX_MM1_PDN2_BEGIN + 1),
    (MT65XX_MM2_PDN_END - MT65XX_MM2_PDN_BEGIN + 1),
    (MT65XX_AUDIO_PDN_END - MT65XX_AUDIO_PDN_BEGIN + 1),
};


/*
 * clock & pll & subsys register array definition
 */

/* register for clock power down and power up control */
const unsigned int mt65xx_cg_con_regs[MT65XX_CLOCK_CATEGORY_COUNT] = {
    PERI_GLOBALCON_PDN0,
    PERI_GLOBALCON_PDN1,
    MMSYS1_CG_CON0,
    MMSYS1_CG_CON1,
    MMSYS1_CG_CON2,
    MMSYS2_CG_CON,
    AUDIO_TOP_CON0,
};

static const unsigned int mt65xx_cg_clr_regs[MT65XX_CLOCK_CATEGORY_COUNT] = {
    PERI_GLOBALCON_PDN0,
    PERI_GLOBALCON_PDN1,
    MMSYS1_CG_CLR0,
    MMSYS1_CG_CLR1,
    MMSYS1_CG_CLR2,
    MMSYS2_CG_CLR,
    AUDIO_TOP_CON0,
};

static const unsigned int mt65xx_cg_set_regs[MT65XX_CLOCK_CATEGORY_COUNT] = {
    PERI_GLOBALCON_PDN0,
    PERI_GLOBALCON_PDN1,
    MMSYS1_CG_SET0,
    MMSYS1_CG_SET1,
    MMSYS1_CG_SET2,
    MMSYS2_CG_SET,
    AUDIO_TOP_CON0,
};

/* register for pll power down and power up control */
const unsigned int mt65xx_pll_regs[MT65XX_PLL_COUNT] = {
    ARMPLL_CON0,
    MAINPLL_CON0,
    IPLL_CON0,
    UPLL_CON0,
    MDPLL_CON0,
    TVDDS_CON0,
    WPLL_CON0,
    AUDPLL_CON0,
    MEMPLL_CON0,
};

/* register for subsystem power down and power up control */
const unsigned int mt65xx_subsys_regs[MT65XX_SUBSYS_COUNT] = {
    SC_PWR_CON0,
    SC_PWR_CON1,
    SC_PWR_CON2,
    SC_PWR_CON3,
    SC_PWR_CON4,
    SC_PWR_CON5,
};


/*
 * clock & pll & subsys name array definition
 */

/* clock category name definition */
const char mt65xx_category_name[MT65XX_CLOCK_CATEGORY_COUNT][MAX_NAME_LEN] = {
    "PERI_PDN0",
    "PERI_PDN1",    
    "MM1_PDN0 ",
    "MM1_PDN1 ",
    "MM1_PDN2 ",
    "MM2_PDN  ",
    "AUDIO_PDN",
};

/* clock name definition */
static const char mt65xx_peri_pdn0_name[MT65XX_PERI_PDN0_END - MT65XX_PERI_PDN0_BEGIN + 1][MAX_NAME_LEN] = {
    "PERI0_NFI",                    /* bit 0 */
    "PERI0_THERM",
    "PERI0_PWM1",
    "PERI0_PWM2",
    "PERI0_PWM3",
    "PERI0_PWM456",                 /* bit 5 */
    "PERI0_PWM7",
    "PERI0_SIMIF0",
    "PERI0_SIMIF1",
    "PERI0_USB1",
    "PERI0_USB2",                   /* bit 10 */
    "PERI0_CCIF",
    "PERI0_APDMA",
    "PERI0_APXGPT",
    "PERI0_NOP",                    /* bit 14, KP is not used */
    "PERI0_MSDC0",                  /* bit 15 */
    "PERI0_MSDC1",
    "PERI0_MSDC2",
    "PERI0_MSDC3",
    "PERI0_APHIF",
    "PERI0_MDHIF",                  /* bit 20 */
    "PERI0_NLI_ARB",
    "PERI0_ACCDET",
    "PERI0_IRDA",
    "PERI0_UART0",
    "PERI0_UART1",                  /* bit 25 */
    "PERI0_UART2",
    "PERI0_UART3",
    "PERI0_I2C0",
    "PERI0_I2C1",
    "PERI0_I2C_DUAL",               /* bit 30 */
    "PERI0_AUXADC",
};

static const char mt65xx_peri_pdn1_name[MT65XX_PERI_PDN1_END - MT65XX_PERI_PDN1_BEGIN + 1][MAX_NAME_LEN] = {
    "PERI1_SEJ",                    /* bit 0 */
    "PERI1_TRNG",
};

static const char mt65xx_mm1_pdn0_name[MT65XX_MM1_PDN0_END - MT65XX_MM1_PDN0_BEGIN + 1][MAX_NAME_LEN] = {
    "MM10_VBUF",                    /* bit 0 */
    "MM10_VDEC",
    "MM10_VENC",
    "MM10_SMI_LARB2_ACP_BUS",
    "MM10_SMI_LARB2_260MHZ",
    "MM10_SMI_LARB2_EMI",           /* bit 5 */
    "MM10_SMI_LARB2_ACP_BUS_EMI",
    "MM10_SMI_LARB1_EMI",
    "MM10_SMI_LARB0_EMI",
};

static const char mt65xx_mm1_pdn1_name[MT65XX_MM1_PDN1_END - MT65XX_MM1_PDN1_BEGIN + 1][MAX_NAME_LEN] = {
    "MM11_VRZ1",                    /* bit 0 */
    "MM11_CSI2",
    "MM11_FD", 
    "MM11_RESZ_LB", 
    "MM11_TV_ROT",
    "MM11_LCD",                     /* bit 5 */
    "MM11_RGB_ROT2",
    "MM11_PRZ1",
    "MM11_R_DMA1",
    "MM11_VDO_ROT1",
    "MM11_RGB_ROT1",                /* bit 10 */
    "MM11_VRZ",
    "MM11_RGB_ROT0",
    "MM11_PRZ0_MOUT",
    "MM11_PRZ0",
    "MM11_VDO_ROT0",                /* bit 15 */
    "MM11_MOUT",
    "MM11_OVL_DMA_MIMO",
    "MM11_OVL_DMA_BPS",
    "MM11_OVL_DMA",
    "MM11_IPP_MOUT",                /* bit 20 */
    "MM11_IPP",
    "MM11_EIS",
    "MM11_CRZ",
    "MM11_JPEG_DMA",
    "MM11_BRZ_MOUT",                /* bit 25 */
    "MM11_BRZ",
    "MM11_JPEG_DEC",
    "MM11_JPEG_ENC",
    "MM11_R_DMA0_MOUT",
    "MM11_R_DMA0",                  /* bit 30 */
    "MM11_CAM",
};

static const char mt65xx_mm1_pdn2_name[MT65XX_MM1_PDN2_END - MT65XX_MM1_PDN2_BEGIN + 1][MAX_NAME_LEN] = {
    "MM12_SCAM",                    /* bit 0 */
    "MM12_SPI",
    "MM12_TVC",
    "MM12_TVE",
    "MM12_DPI",
    "MM12_DSI",                     /* bit 5 */
    "MM12_SMI_LARB2",
    "MM12_SMI_LARB1",
    "MM12_SMI_LARB0_LCD_EMI",
};

static const char mt65xx_mm2_pdn_name[MT65XX_MM2_PDN_END - MT65XX_MM2_PDN_BEGIN + 1][MAX_NAME_LEN] = {
    "MM2_SMI_LARB3_FULL",           /* bit 0 */
    "MM2_NOP",                      
    "MM2_GDC_SHARE_MACRO",          /* bit 2 */
    "MM2_G2D",                      /* bit 3 */
    "MM2_MFG",                      /* bit 4 */
    "MM2_NOP",
    "MM2_NOP",
    "MM2_NOP",
    "MM2_NOP",
    "MM2_NOP",
    "MM2_NOP",
    "MM2_NOP",
    "MM2_NOP",
    "MM2_NOP",
    "MM2_G3D",                      /* bit 14 */
    "MM2_F26M",                     /* bit 15 */
    "MM2_GDC_SHARE_MACRO_HALF",     /* bit 16 */
    "MM2_SMI_LARB3_HALF",           /* bit 17 */
    "MM2_AUDIO_CORE",               /* bit 18 */
    "MM2_MFG_HALF",                 /* bit 19 */
};

static const char mt65xx_audio_pdn_name[MT65XX_AUDIO_PDN_END - MT65XX_AUDIO_PDN_BEGIN + 1][MAX_NAME_LEN] = {
    "AUDIO_NOP",                    /* bit 0 */
    "AUDIO_NOP",                    /* bit 1 */
    "AUDIO_AFE",                    /* bit 2 */
    "AUDIO_NOP",                
    "AUDIO_NOP",
    "AUDIO_ADC",                    /* bit 5 */
    "AUDIO_I2S",                    /* bit 6 */
};

static char* mt65xx_clock_name_idx[MT65XX_CLOCK_CATEGORY_COUNT] = {
    (char *)mt65xx_peri_pdn0_name,
    (char *)mt65xx_peri_pdn1_name,
    (char *)mt65xx_mm1_pdn0_name,
    (char *)mt65xx_mm1_pdn1_name,
    (char *)mt65xx_mm1_pdn2_name,
    (char *)mt65xx_mm2_pdn_name,
    (char *)mt65xx_audio_pdn_name,
};

/* pll name definition */
const char mt65xx_pll_name[MT65XX_PLL_COUNT][MAX_NAME_LEN] = {
    "ARMPLL ",
    "MAINPLL",
    "IPLL   ",
    "UPLL   ",
    "MDPLL  ",
    "TVDDS  ",
    "WPLL   ",
    "AUDPLL ",
    "MEMPLL ",
};

/* subsystem(MTCMOS domain) name definition */
const char mt65xx_subsys_name[MT65XX_SUBSYS_COUNT][MAX_NAME_LEN] = {
    "CA9_SUBSYS   ",
    "CA9MP_SUBSYS ",
    "CPUSYS_SUBSYS",
    "NEON_SUBSYS  ",
    "MM1_SUBSYS   ",
    "MM2_SUBSYS   ",
};


/* 
 * bit = 1, it's a valid clock bit
 * bit = 0, it's an invalid clock bit, or it's not allowed operated by AP side
 */
const unsigned int valid_clock_mask[MT65XX_CLOCK_CATEGORY_COUNT] = {
    0xFFEFBE7F, /* MT65XX_CLOCK_PERI_PDN0, exclude MDHIF, KP, SIMIF0, SIMIF1 */
    0x00000003, /* MT65XX_CLOCK_PERI_PDN1, there are 2 clock bits in all */
    0x000001FF, /* MT65XX_CLOCK_MMSYS1_PDN0, there are 9 clock bits in all */
    0xFFFFFFFF, /* MT65XX_CLOCK_MMSYS1_PDN1, there are 32 clock bits in all */
    0x000001FF, /* MT65XX_CLOCK_MMSYS1_PDN2, there are 9 clock bits in all */
    0x000FC01D, /* MT65XX_CLOCK_MMSYS2_PDN, there are 10 clock bits in all */
    0x00000064, /* MT65XX_CLOCK_AUDIO_PDN, there are 3 clock bits in all */
};


static int initialized = 0;
struct clock_manager clk_mgr;
static int g_chip12 = 0;


static int mm1_backing_up = 0;
static int mm2_backing_up = 0;

#define MM1_TRIGGER_MM2
#ifdef MM1_TRIGGER_MM2
static int pdn_mm2_pending = 0;
#endif

/*
 * Secuity Mechanism
 */
static DEFINE_SPINLOCK(clkmgr_lock);


/*
 * Debug and Log Mechanism
 */
#ifdef OP_MODE_DEBUG
/* 
 * op_mode = 0, clk mgr do noting, return at function entry
 * op_mode = 1, clk mgr run processing logic 
 */ 
int op_mode = 1; 
#endif

#ifdef FORCE_UNGATE_DEBUG
/* 
 * bit = 1, the clock bit won't be gated forcibly when requested,
 * bit = 0, the clock bit will be gated and ungated when requested
 */
unsigned int force_ungate_mask[MT65XX_CLOCK_CATEGORY_COUNT];
#endif

/* 
 * bit = 1, the clock bit log will be dumped at runtime
 * bit = 0, the clock bit log won't be dumped at runtime
 */
unsigned int partial_log_mask[MT65XX_CLOCK_CATEGORY_COUNT];

/* clock manager debug level */
enum {
    DEBUG_INIT   = 1U << 0,
    DEBUG_CLOCK  = 1U << 1,
    DEBUG_PLL    = 1U << 2,
    DEBUG_SUBSYS = 1U << 3,
    DEBUG_LARB   = 1U << 4,
    DEBUG_MIPI   = 1U << 5,
};


int clkmgr_debug_mask = 0x0;


#define CATEGORY_AP(category)           ((0x1 << (category)) & 0x3)     /* 0b0000011 */
#if 0
#define CATEGORY_MM(category)           ((0x1 << (category)) & 0x7C)    /* 0b1111100 */
#else
#define CATEGORY_MM(category)           ((category >= MT65XX_CLOCK_MMSYS1_PDN0) && (category <= MT65XX_CLOCK_AUDIO_PDN))  
#endif

#define CATEGORY_AP1(category)          ((0x1 << (category)) & 0x1)     /* 0b0000001 */

#define CATEGORY_MM1(category)          ((0x1 << (category)) & 0x1C)    /* 0b0011100 */
#define CATEGORY_MM2(category)          ((0x1 << (category)) & 0x60)    /* 0b1100000 */
#define CATEGORY_AUD(category)          ((0x1 << (category)) & 0x40)    /* 0b1000000 */

#define CATEGORY_UNIFIED_REG(category)  ((0x1 << (category)) & 0x43)    /* 0b1000011 */

#define HW_RESV (0XF1019100)
#define MFG_CHECK_MASK      (0x00084010)
#define MFG_CORE_CLK	MT65XX_PDN_MM_G3D
#define ID_MFG(id) (id == MFG_CORE_CLK)
/*
 * dump a single clock register states
 */
static void dump_clk_reg(int category)
{
    clk_info("[%s]: %s = 0x%08x\n", __func__, mt65xx_category_name[category], 
            DRV_Reg32(mt65xx_cg_con_regs[category]));

    if (CATEGORY_AP(category)) {
        char* temp = "MD_SIDE  ";
        clk_info("[%s]: %s = 0x%08x\n", __func__, temp, 
            DRV_Reg32(mt65xx_cg_con_regs[category] + 0x8));
    }
}

/*
 * dump all of clock registers states
 */
void dump_clk_regs(void)
{
    int i = 0;
    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {
        dump_clk_reg(i);
    }
}
EXPORT_SYMBOL(dump_clk_regs);


/*
 * dump a single pll register states
 */
static void dump_pll_reg(int id)
{
    clk_info("[%s]: %s%s = 0x%04x\n", __func__, mt65xx_pll_name[id], 
            "_CON0", DRV_Reg16(mt65xx_pll_regs[id]));

    if (id == MT65XX_WPLL) {
        clk_info("[%s]: %s%s = 0x%04x\n", __func__, mt65xx_pll_name[id], 
                "_CON1", DRV_Reg16(mt65xx_pll_regs[id] + 4));
    } else if (id == MT65XX_TVDDS) {
        clk_info("[%s]: %s%s = 0x%04x\n", __func__, mt65xx_pll_name[id], 
                "_CON1", DRV_Reg16(mt65xx_pll_regs[id] + 4));
        clk_info("[%s]: %s%s = 0x%04x\n", __func__, mt65xx_pll_name[id], 
                "_CON2", DRV_Reg16(mt65xx_pll_regs[id] + 4 * 2));
    }
}

/*
 * dump all of pll registers states
 */
void dump_pll_regs(void)
{
    int i = 0;
    for (i = 0; i < MT65XX_PLL_COUNT; i++) {
        dump_pll_reg(i);
    }
}
EXPORT_SYMBOL(dump_pll_regs);


/* 
 * get clock states of a certain category,
 * support local state check and register state check
 * support multi-bits state check
 * @return STATE_ON if clock is enabled, else return STATE_OFF 
 */
static int get_clock_state(int category, unsigned int mask, int mode)
{
    unsigned int ret = 0;

    switch (mode) {
    case LOCAL_STATE:
        ret = clk_mgr.clock_state[category] & mask;
        /* For clock manager internal state,
         * 0: Power DOWN,   1: Power UP
         */ 
        return (ret == mask ? STATE_ON : STATE_OFF);
    
    case REG_STATE:
    default:
        ret = DRV_Reg32(mt65xx_cg_con_regs[category]) & mask;
        /* For AP Peri & MMSYS & AUDIO register,
         * 0: Power UP,     1: Power DOWN
         */ 
        return (ret == 0 ? STATE_ON : STATE_OFF);
    }
}

#define PLL_STATE_MASK      (0x1)
#define TVDDS_STATE_MASK    (0x7C00)

/* 
 * @return STATE_ON if pll is enabled, else return STATE_OFF 
 */
static int get_pll_state(int id, int mode)
{
    unsigned short ret = 0;
    unsigned int mask = 0x1 << id;
    int offset = (id == MT65XX_TVDDS) ? (4 * 2) : 0;

    switch (mode) {
    case LOCAL_STATE:
        ret = clk_mgr.pll_state & mask; 
        /* For clock manager internal state,
         * 0: Power DOWN,   1: Power UP
         */ 
        return (ret == mask ? STATE_ON : STATE_OFF);

    case REG_STATE:
    default:
        ret = DRV_Reg16(mt65xx_pll_regs[id] + offset);
        if (id == MT65XX_WPLL || id == MT65XX_AUDPLL) {
            /* For WPLL & AUDPLL: 0: Power Down, 1: Power Up */ 
            return ((ret & PLL_STATE_MASK) ? STATE_ON : STATE_OFF);
        } else if (id == MT65XX_TVDDS) {
            return (((ret & TVDDS_STATE_MASK) ^ TVDDS_STATE_MASK) ? STATE_OFF : STATE_ON);
        } else {
            return ((ret & PLL_STATE_MASK) ? STATE_OFF : STATE_ON);
        }
    }
}


/* 
 * @return STATE_ON if subsys is enabled, else return STATE_OFF 
 */
static int get_subsys_state(int id, int mode)
{
    unsigned int ret = 0;
    unsigned int mask = 0x1 << id;    

    switch (mode) {
    case LOCAL_STATE:
        ret = clk_mgr.subsys_state & mask; 
        break;
    case REG_STATE:
    default:
        ret = DRV_Reg32(SC_PWR_STA) & mask;  
    }

    /* 0: Power Off,    1: Power On  */
    return (ret == mask ? STATE_ON : STATE_OFF);  
}

bool subsys_is_on(enum mt65xx_subsys_id id)
{
    BUG_ON(INVALID_SUBSYS_ID(id));
    if (get_subsys_state(id, REG_STATE) == STATE_ON) {
        return true;
    } else {
        return false;
    }
}
EXPORT_SYMBOL(subsys_is_on);

int get_state(int type, int id, int mode)
{
    BUG_ON(mode >= (LOCAL_STATE | REG_STATE));

    switch (type) {
    case TYPE_PLL:
        BUG_ON(INVALID_PLL_ID(id));
        return get_pll_state(id, mode);

    case TYPE_SUBSYS:
        BUG_ON(INVALID_SUBSYS_ID(id));
        return get_subsys_state(id, mode);
    default:
        clk_warn("[%s]: type = %d\n", __func__, type);
        return -1;
    }
}

/*
 * inspect whether some clock bits are still on in a certain subsys
 * return 0 if subsys could be disabled 
 * return non-zero value if subsys is invalid or can't be disabled
 */
static int subsys_has_clock_on(int subsys)
{
    unsigned int ret = 0;   
    int i = 0;

    switch (subsys) {
    case MT65XX_SUBSYS_MM1:
        for (i = MT65XX_CLOCK_MMSYS1_PDN0; i <= MT65XX_CLOCK_MMSYS1_PDN2; i++) {
            ret = clk_mgr.clock_state[i];
            if (ret) {
                break;
            }
        }
        break;

    case MT65XX_SUBSYS_MM2:
		ret = clk_mgr.clock_state[MT65XX_CLOCK_MMSYS2_PDN] & MFG_CHECK_MASK;
        break;
    }

    return ret;
}


static int should_power_on_subsys(int subsys_id) {
    switch (subsys_id) {
    case MT65XX_SUBSYS_MM1:
    case MT65XX_SUBSYS_MM2:
        return (get_subsys_state(subsys_id, LOCAL_STATE) == STATE_OFF);

    default:
        clk_warn("[%s]bad subsys id %d\n", __func__, subsys_id);
        BUG();
    }
}

static int could_power_off_subsys(int subsys_id) {
    switch (subsys_id) {
    case MT65XX_SUBSYS_MM1:
        return (!subsys_has_clock_on(subsys_id) && !clk_mgr.mipi_state && !mm1_backing_up);

    case MT65XX_SUBSYS_MM2:
        return (!subsys_has_clock_on(subsys_id) && !mm2_backing_up);

    default:
        clk_warn("[%s]bad subsys id %d\n", __func__, subsys_id);
        BUG();
    }
}

static int should_power_on_mmsys_clksrc(int usr_id) {
	int ret = 0;
	if (g_chip12){
		ret = ID_MFG(usr_id) ? !clk_mgr.mmsys_state_mfg : !clk_mgr.mmsys_state;
	} else {
		ret = !clk_mgr.mmsys_state;
	}
	return ret;
}

static int could_power_off_mmsys_clksrc(void) {
    return  !(clk_mgr.subsys_state & 0x30) && 
            !(clk_mgr.clock_state[MT65XX_CLOCK_MMSYS2_PDN]);
}

static int __request_mmsys_clksrc(int opcode, int is_mfg);
static void mm_power_prepare(int subsys_id, char *usr, int usr_id)
{
	/*select mfg clksrc*/
	if (ID_MFG(usr_id)){
		if (g_chip12 == 1) {
			DRV_SetReg16(PLL_CON11, BIT(10));
			DRV_ClrReg16(PLL_CON11, BIT(11));
		} else {
			DRV_SetReg16(PLL_CON11, BIT(10) | BIT(11));
		}
	}

    if (should_power_on_mmsys_clksrc(usr_id)) {
        clk_info("[%s]: enable mmsys clksrc for %s %d\n", __func__, usr, usr_id);
        __request_mmsys_clksrc(1, ID_MFG(usr_id));

		clk_info("[%s]: set MM way_en\n", __func__);
		DRV_SetReg32(TOPAXI_SI4_CTL, 0x1 << 5);
    }

    if (subsys_id) {
        if ((subsys_id == MT65XX_SUBSYS_MM1) && should_power_on_subsys(MT65XX_SUBSYS_MM1)) {
            clk_info("[%s]: subsys MM1 is OFF now, enable it first for %s %d\n", 
                    __func__, usr, usr_id);
            enable_subsys_locked(MT65XX_SUBSYS_MM1, "clkmgr");
        } else if (subsys_id == MT65XX_SUBSYS_MM2) {
            if (should_power_on_subsys(MT65XX_SUBSYS_MM2)) {
                clk_info("[%s]: subsys MM2 is OFF now, enable it first for %s %d\n", 
                        __func__, usr, usr_id);
                enable_subsys_locked(MT65XX_SUBSYS_MM2, "clkmgr");
            } else {
#ifdef MM1_TRIGGER_MM2
                if (pdn_mm2_pending)
                    pdn_mm2_pending = 0;
#endif
            }
        }
    }
}

static void mm_power_finish(int subsys_id, char *usr, int usr_id) {
    if (subsys_id == MT65XX_SUBSYS_MM1 && could_power_off_subsys(MT65XX_SUBSYS_MM1)) {
        clk_info("[%s]: disable subsys MM1 for %s %d\n", __func__, usr, usr_id);
        disable_subsys_locked(MT65XX_SUBSYS_MM1, "clkmgr");
#ifdef MM1_TRIGGER_MM2
        if (pdn_mm2_pending) {
            clk_info("[%s]: disable subsys MM2 triggered by MM1\n", __func__);
            disable_subsys_locked(MT65XX_SUBSYS_MM2, "clkmgr");
            pdn_mm2_pending = 0;
        }
#endif
    } else if (subsys_id == MT65XX_SUBSYS_MM2 && could_power_off_subsys(MT65XX_SUBSYS_MM2)) {
#ifdef MM1_TRIGGER_MM2
        if (clk_mgr.subsys_state & (0x1 << MT65XX_SUBSYS_MM1)) {
            if (!pdn_mm2_pending) {
                //clk_info("[%s]: disable subsys MM2 for %s %d pending\n", __func__, usr, usr_id);
                pdn_mm2_pending = 1;
            }
        } else {
            clk_info("[%s]: disable subsys MM2 for %s %d now\n", __func__, usr, usr_id);
            disable_subsys_locked(MT65XX_SUBSYS_MM2, "clkmgr");
            pdn_mm2_pending = 0;
        }
#else
        clk_info("[%s]: disable subsys MM2 for %s %d\n", __func__, usr, usr_id);
        disable_subsys_locked(MT65XX_SUBSYS_MM2, "clkmgr");
#endif
    }

		if (g_chip12 == 1 && ID_MFG(usr_id)){
				__request_mmsys_clksrc(0,1);
		}

    if (clk_mgr.mmsys_state && !(clk_mgr.subsys_state & 0x30) && !(clk_mgr.clock_state[MT65XX_CLOCK_MMSYS2_PDN])) {
        clk_info("[%s]: clr MM way_en\n", __func__);
		DRV_ClrReg32(TOPAXI_SI4_CTL, 0x1 << 5);

        clk_info("[%s]: disable mmsys clksrc for %s %d\n", __func__, usr, usr_id);
        __request_mmsys_clksrc(0,0);
    }
}


#ifdef CLK_MGR_DEBUG
/* 
 *  opt=1, enable clock; opt=0, disable clock
 */
static void clk_stat_locked(int category, int offset, char* mod_name, int opt)
{
    struct list_head *p = NULL;
    struct module_info *info = NULL;
    bool new_module = true;

    list_for_each(p, &clk_mgr.clock[category][offset].list) {
        info = list_entry(p, struct module_info, link); 
        if (!strcmp(info->module_name, mod_name)) {
            new_module = false;
            break;
        }
    }

    if (new_module) {
        info = NULL;
        info = kzalloc(sizeof(struct module_info), GFP_ATOMIC);   
        if (!info) {
            clk_err("[%s]: malloc module_info for %s fail\n", 
                __func__, mod_name);
            return;
        } else {
            sprintf(info->module_name, "%s", mod_name);
            list_add_tail(&info->link, &clk_mgr.clock[category][offset].list);
        }    
    }    

    if (opt) {
        info->enable_cnt++;
    } else {
        info->disable_cnt++;
    }
}


/* 
 *  opt=1, enable pll; opt=0, disable pll
 */
static void pll_stat_locked(int id, char* mod_name, int opt)
{
    struct list_head *p = NULL;
    struct module_info *info = NULL;
    bool new_module = true;

    list_for_each(p, &clk_mgr.pll[id].list) {
        info = list_entry(p, struct module_info, link); 
        if (!strcmp(info->module_name, mod_name)) {
            new_module = false;
            break;
        }
    }

    if (new_module) {
        info = NULL;
        info = kzalloc(sizeof(struct module_info), GFP_ATOMIC);   
        if (!info) {
            clk_err("[%s]: malloc module_info for %s fail\n", 
                __func__, mod_name);
            return;
        } else {
            sprintf(info->module_name, "%s", mod_name);
            list_add_tail(&info->link, &clk_mgr.pll[id].list);
        }    
    }    

    if (opt) {
        info->enable_cnt++;
    } else {
        info->disable_cnt++;
    }
}

/* 
 *  opt=1, enable mipi; opt=0, disable mipi
 */
#if 0
static void mipi_stat_locked(int id, char* mod_name, int opt)
{
    struct list_head *p = NULL;
    struct module_info *info = NULL;
    bool new_module = true;

    list_for_each(p, &mipi[id].list) {
        info = list_entry(p, struct module_info, link); 
        if (!strcmp(info->module_name, mod_name)) {
            new_module = false;
            break;
        }
    }

    if (new_module) {
        info = NULL;
        info = kzalloc(sizeof(struct module_info), GFP_KERNEL);   
        if (!info) {
            clk_err("[%s]: malloc module_info for %s fail\n", 
                __func__, mod_name);
            return;
        } else {
            sprintf(info->module_name, "%s", mod_name);
            list_add_tail(&info->link, &mipi[id].list);
        }    
    }    

    if (opt) {
        info->enable_cnt++;
    } else {
        info->disable_cnt++;
    }
}
#endif
#endif


/* 
 * Clock Bit Un-Gating (Power Up or CG Clear)
 * For AP Peri, write 0 in PERI_GLOBALCON_PDNx register 
 * For MMSYS,   write 1 in XXX_CG_CLRx register
 * For AUDIO,   write 0 in AUDIO_TOP_CON0 register
 */
static void enable_clock_op(int category, unsigned int mask)
{
    if (CATEGORY_UNIFIED_REG(category)) {
        DRV_ClrReg32(mt65xx_cg_clr_regs[category], mask);
    } else {
        DRV_WriteReg32(mt65xx_cg_clr_regs[category], mask);
    } 
}

/* 
 * Clock Bit Gating (Power Down or CG Set)
 * For AP Peri, write 1 in PERI_GLOBALCON_PDNx register 
 * For MMSYS,   write 1 in XXX_CG_SETx register
 * For AUDIO,   write 1 in AUDIO_TOP_CON0 register
 */
static void disable_clock_op(int category, unsigned int mask)
{
    dsb();
    outer_sync();
    if (CATEGORY_UNIFIED_REG(category)) {
        DRV_SetReg32(mt65xx_cg_set_regs[category], mask);
    } else {
        DRV_WriteReg32(mt65xx_cg_set_regs[category], mask);
    }
}


//#define MFG_CHECK_MASK      (0x00084010)
#define AUD_CHECK_MASK      (0x00000064)
#define AUD_CORE_MASK       (0x00040000)

#define WAY_EN_MASK_MFG     (0x1 << 2)
#define WAY_EN_MASK_AUD     (0x1 << 3)

#define WAY_EN_IDX_MFG      (0x0)
#define WAY_EN_IDX_AUD      (0x1)


/* way_en[0] is for MFG engine, way_en[1] is for AUD engine */
static int way_en[2] = {1, 1};

#define MSDC_DEBUG_ENABLED

#ifdef MSDC_DEBUG_ENABLED
static unsigned int local_on_mask = 0x78000;
#endif

static int enable_clock_internal(int category, unsigned int mask)
{
#ifdef BUG_ON_DEBUG
    /* only for debug */
    BUG_ON(get_clock_state(category, mask, LOCAL_STATE) !=
            get_clock_state(category, mask, REG_STATE));
#endif

#ifndef MSDC_DEBUG_ENABLED
#if 0
    if (get_clock_state(category, mask, LOCAL_STATE) == STATE_ON) {
        return 0;
    }
#endif
#else
    if (get_clock_state(category, mask, LOCAL_STATE) == STATE_ON) {
        if ((category == 0) && (mask & 0x78000)) {
            if (local_on_mask & mask) {
                local_on_mask &= ~mask;
            } else {
                clk_info("[%s]: local_on: (%d, 0x%x) (0x%x 0x%x)\n",
                    __func__, category, mask, clk_mgr.clock_state[category],
                    DRV_Reg32(mt65xx_cg_con_regs[category]));
                WARN_ON(1);
            }
        }
    }
#endif

    if (partial_log_mask[category] & mask) {
        clk_info("[%s]: <== UNgate 0x%x @ %s\n", __func__, mask,
                mt65xx_category_name[category]);
        dump_clk_reg(category);
    }

    enable_clock_op(category, mask);

    if (partial_log_mask[category] & mask) {
        clk_info("[%s]: ==> UNgate 0x%x @ %s\n", __func__, mask,
                mt65xx_category_name[category]);
        dump_clk_reg(category);
    }

    if (get_clock_state(category, mask, REG_STATE) == STATE_OFF) {
        clk_err("[%s]: enable 0x%x @ %s fail\n", __func__, mask, 
                mt65xx_category_name[category]);
        return 1;
    }

    clk_mgr.clock_state[category] |= mask;

    /* once turn on mfg clock, set way_en for mfg engine */
    if(category == MT65XX_CLOCK_MMSYS2_PDN && (mask & MFG_CHECK_MASK) && 
            ((clk_mgr.clock_state[category] & MFG_CHECK_MASK) == MFG_CHECK_MASK) &&
            !way_en[WAY_EN_IDX_MFG]) {
        if (partial_log_mask[category] & mask) {
            clk_info("[%s]: set way_en for mfg\n", __func__);
        }
        DRV_SetReg32(MMSYS2_MM2AXI_CON0, WAY_EN_MASK_MFG);        
        way_en[WAY_EN_IDX_MFG] = 1; 
    }

    /* once turn on audio core clock, set way_en for audio engine */
    if (category == MT65XX_CLOCK_MMSYS2_PDN && (mask & AUD_CORE_MASK) && 
            !way_en[WAY_EN_IDX_AUD]) {
        if (partial_log_mask[category] & mask) {
            clk_info("[%s]: set way_en for audio\n", __func__);
        }
        DRV_SetReg32(MMSYS2_MM2AXI_CON0, WAY_EN_MASK_AUD);        
        way_en[WAY_EN_IDX_AUD] = 1; 
    }

    return 0;
}


static int disable_clock_internal(int category, unsigned int mask)
{
#ifdef BUG_ON_DEBUG
    /* only for debug */
    BUG_ON(get_clock_state(category, mask, LOCAL_STATE) !=
            get_clock_state(category, mask, REG_STATE));
#endif

#ifndef MSDC_DEBUG_ENABLED
#if 0
    if (get_clock_state(category, mask, LOCAL_STATE) == STATE_OFF) {
        return 0;
    }
#endif
#else
    if (get_clock_state(category, mask, LOCAL_STATE) == STATE_OFF) {
        if ((category == 0) && (mask & 0x78000)) {
            clk_info("[%s]: local_off: (%d, 0x%x) (0x%x 0x%x)\n",
                __func__, category, mask, clk_mgr.clock_state[category],
                DRV_Reg32(mt65xx_cg_con_regs[category]));
            WARN_ON(1);
        }
    }
#endif

    /* once turn off mfg clock, set way_en for mfg engine */
    if (category == MT65XX_CLOCK_MMSYS2_PDN && (mask & MFG_CHECK_MASK) && 
            way_en[WAY_EN_IDX_MFG]) {
        if (partial_log_mask[category] & mask) {
            clk_info("[%s]: clr way_en for mfg\n", __func__);
        }
        DRV_ClrReg32(MMSYS2_MM2AXI_CON0, WAY_EN_MASK_MFG);        
        way_en[WAY_EN_IDX_MFG] = 0; 
    }

    /* once turn off audio core clock, clr way_en for audio engine */
    if (category == MT65XX_CLOCK_MMSYS2_PDN && (mask & AUD_CORE_MASK) &&
            way_en[WAY_EN_IDX_AUD]) {
        if (partial_log_mask[category] & mask) {
            clk_info("[%s]: clr way_en for audio\n", __func__);
        }
        DRV_ClrReg32(MMSYS2_MM2AXI_CON0, WAY_EN_MASK_AUD);        
        way_en[WAY_EN_IDX_AUD] = 0; 
    }

    if (partial_log_mask[category] & mask) {
        clk_info("[%s]: <== gate 0x%x @ %s\n", __func__, mask,
                mt65xx_category_name[category]);
        dump_clk_reg(category);
    }

    disable_clock_op(category, mask);

    if (partial_log_mask[category] & mask) {
        clk_info("[%s]: ==> gate 0x%x @ %s\n", __func__, mask,
                mt65xx_category_name[category]);
        dump_clk_reg(category);
    }

    if (get_clock_state(category, mask, REG_STATE) == STATE_ON) {
        clk_err("[%s]: disable 0x%x @ %s fail\n", __func__, mask, 
                mt65xx_category_name[category]);
        return 1;
    }
   
    clk_mgr.clock_state[category] &= ~mask;

    return 0;
}


struct clksrc_usr {
    int usr_id;
    unsigned int cg_mask;       /* mask in CG */
    int sel_offset;
    unsigned int sel_mask;      /* mask in PERI_MSDC_SRC_CLK_SEL */
    unsigned int max_sel;
};


#define CLKSRC_USR_NUM              6
#define CLKSRC_USR_MSDC(id)         ((0x1 << (id)) & 0xF)
#define CLKSRC_USR_USB(id)          ((0x1 << (id)) & 0x30)
#define INVALID_CLKSRC_USR_ID(id)   ((id) < 0 || (id) > CLKSRC_USR_USB2)


#define CG_MSDC_MASK                0x78000
#define CG_USB_MASK                 0x00600
#define CG_MASK_ALL                 (CG_MSDC_MASK | CG_USB_MASK)


#define WPLL_EN                     (0x1 << 0)
#define WPLL_197M_EN                (0x1 << 1)      // for MSDC Clock
#define WPLL_281M_EN                (0x1 << 2)      // for 3D Backup Clock
#define WPLL_492M_EN                (0x1 << 3)      // for GDC Backup Clock
#define WPLL_61M_EN                 (0x1 << 14)     // for USB and Irda Clock
#define WPLL_CLKSRC_MASK_ALL        (WPLL_61M_EN | WPLL_197M_EN | WPLL_281M_EN | WPLL_EN)


static struct clksrc_usr clksrc_usr[CLKSRC_USR_NUM];
static unsigned int clksrc_selection = 0;       /* SELECT */
static unsigned int wpll_usr_mask = 0;          /* CG */
static unsigned short wpll_clksrc_state = 0;    /* WPLL_CON0 */


static unsigned int __get_clksrc_selection(int id)
{
    return clksrc_selection & clksrc_usr[id].sel_mask;
}


unsigned int get_clksrc_selection(int id)
{
    unsigned int selection = 0;

    BUG_ON(INVALID_CLKSRC_USR_ID(id));
    selection = __get_clksrc_selection(id);
    selection = (selection & clksrc_usr[id].sel_mask) >> clksrc_usr[id].sel_offset;
    return selection;
}


void clksrc_select(int id, unsigned int selection)
{
    unsigned long flags;
    unsigned int old_selection = 0;
    unsigned int new_selection = 0;

    BUG_ON(INVALID_CLKSRC_USR_ID(id));
    BUG_ON(selection > clksrc_usr[id].max_sel);

    spin_lock_irqsave(&clkmgr_lock, flags);

    old_selection = __get_clksrc_selection(id);
    new_selection = (selection << clksrc_usr[id].sel_offset) & clksrc_usr[id].sel_mask;

    clksrc_selection = (clksrc_selection & (~clksrc_usr[id].sel_mask)) | new_selection; 

    if (unlikely(old_selection == new_selection)) {
        return;
    }

    clk_info("[%s] %d change clksrc: 0x%x -> 0x%x\n", 
            __func__, id, old_selection, new_selection);
    DRV_WriteReg32(PERI_MSDC_SRC_CLK_SEL, clksrc_selection);

    if ((CLKSRC_USR_MSDC(id) && selection == CLKSRC_MSDC_197MHz) || 
            (CLKSRC_USR_USB(id) && selection == CLKSRC_USB_61MHz)) {
        wpll_usr_mask |= clksrc_usr[id].cg_mask;
    } else {
        wpll_usr_mask &= ~clksrc_usr[id].cg_mask;
    }

    spin_unlock_irqrestore(&clkmgr_lock, flags);
    return;
}


/* parameter mask is mask from CG register */
static unsigned short should_power_on_wpll_clksrc(unsigned int cg_mask)
{
    unsigned short wpll_clksrc_mask = 0;

    if (!(cg_mask & wpll_usr_mask)) {
        return 0;
    }
    
    if (cg_mask & CG_MSDC_MASK) {
        wpll_clksrc_mask = WPLL_197M_EN;
    } else if (cg_mask & CG_USB_MASK) {
        wpll_clksrc_mask = WPLL_61M_EN;
    } 

    if (wpll_clksrc_mask & wpll_clksrc_state) {
        return 0;
    } 

    /* power on clksrc from wpll */
    return wpll_clksrc_mask;
}

static unsigned short should_power_off_wpll_clksrc(unsigned int cg_mask)
{
    unsigned short wpll_clksrc_mask = 0;

    if (!(cg_mask & wpll_usr_mask)) {
        return 0;
    }

    if (cg_mask & CG_MSDC_MASK) {
        if (!(clk_mgr.clock_state[0] & CG_MSDC_MASK)) {
            wpll_clksrc_mask = WPLL_197M_EN;
        }
    } else if (cg_mask & CG_USB_MASK) {
        if (!(clk_mgr.clock_state[0] & CG_USB_MASK)) {
            wpll_clksrc_mask = WPLL_61M_EN;
        }
    } 

    return wpll_clksrc_mask;
}

/* parameter mask is mask from WPLL_CON0 register */
static void enable_wpll_clksrc_op(unsigned short wpll_clksrc_mask)
{
    unsigned short reg_val = wpll_clksrc_mask;
    int wait_for_stable = 0;    

    if (!(wpll_clksrc_state & WPLL_EN)) {
        reg_val |= WPLL_EN;
        wait_for_stable = 1;
    }

    DRV_SetReg16(WPLL_CON0, reg_val);
    if (wait_for_stable) {
        udelay(30);
    }

    wpll_clksrc_state |= reg_val;

#if 0
    clk_info(
            "[%s] wpll_clksrc_mask=0x%x, wpll_clksrc_state=0x%x, WPLL_CON=0x%x\n", 
            __func__, wpll_clksrc_mask, wpll_clksrc_state, DRV_Reg16(WPLL_CON0));
#endif
}


static void disable_wpll_clksrc_op(unsigned short wpll_clksrc_mask)
{
    unsigned short reg_val = wpll_clksrc_mask;

    if (!(wpll_clksrc_state & 0xFFFE & (~wpll_clksrc_mask))) {
        reg_val |= WPLL_EN;
    }

    DRV_ClrReg16(WPLL_CON0, reg_val);

    wpll_clksrc_state &= ~reg_val;

#if 0
    clk_info(
            "[%s] wpll_clksrc_mask=0x%x, wpll_clksrc_state=0x%x, WPLL_CON=0x%x\n", 
            __func__, wpll_clksrc_mask, wpll_clksrc_state, DRV_Reg16(WPLL_CON0));
#endif
}


#define WPLL_TEMP_SOLUTION
static void clksrc_selection_init(void)
{
#ifdef WPLL_TEMP_SOLUTION
    /* MSDC0~3 and USB0~1 */
    wpll_usr_mask = 0x78600;
#else
    int i = 0;
    int idx = 0;

    clksrc_selection = DRV_Reg(PERI_MSDC_SRC_CLK_SEL);

    for (i = MT65XX_PDN_PERI_MSDC0; i <= MT65XX_PDN_PERI_MSDC3; i++) {
        clksrc_usr[idx].usr_id = i;
        clksrc_usr[idx].cg_mask = 0x1 << (i % 32);
        if (unlikely(get_chip_eco_ver() == CHIP_E1)) {
            clksrc_usr[idx].sel_offset = 0;
            clksrc_usr[idx].sel_mask = 0x3;
        } else {
            clksrc_usr[idx].sel_offset = (idx == 0 ? 0 : (idx*2 + 1));
            clksrc_usr[idx].sel_mask = 0x3 << clksrc_usr[idx].sel_offset;
        }
        clksrc_usr[idx].max_sel = CLKSRC_MSDC_197MHz;
        if (__get_clksrc_selection(i) == CLKSRC_MSDC_197MHz << clksrc_usr[idx].sel_offset) {
            wpll_usr_mask |= clksrc_usr[idx].cg_mask;
        }
        idx++;
    }

    for (i = MT65XX_PDN_PERI_USB1; i <= MT65XX_PDN_PERI_USB2; i++) {
        clksrc_usr[idx].usr_id = i;
        clksrc_usr[idx].cg_mask = 0x600;
        clksrc_usr[idx].sel_offset = 2;
        clksrc_usr[idx].sel_mask = 0x4;
        clksrc_usr[idx].max_sel = CLKSRC_USB_61MHz;
        if (__get_clksrc_selection(i) == CLKSRC_USB_61MHz << clksrc_usr[idx].sel_offset) {
            wpll_usr_mask |= clksrc_usr[idx].cg_mask;
        }
        idx++;
    }

#endif

    wpll_clksrc_state = DRV_Reg16(WPLL_CON0) & WPLL_CLKSRC_MASK_ALL;
    clk_info(
            "[%s] clksrc_selection = 0x%x, wpll_clksrc_state = 0x%x, wpll_usr_mask = 0x%x\n", 
            __func__, clksrc_selection, wpll_clksrc_state, wpll_usr_mask);
}

#define MT65XX_LARB_CLOCK_COUNT     11
#define MM_CLOCK_CATEGORY_COUNT     5

enum mt65xx_larb_group {
    MT65XX_LARB0                    = 0,
    MT65XX_LARB1                    = 1,
    MT65XX_LARB2                    = 2,
    MT65XX_LARB3                    = 3,
    MT65XX_LARB_GROUP_COUNT         = 4,
};

/* larb clocks mask origanized by clock category */
static unsigned int larb_clock_mask[MM_CLOCK_CATEGORY_COUNT] = {
    0x000001F8,
    0x00000000,
    0x000001C0,
    0x00020001,
    0x00000000,
};

/* clocks that doesn't depend on LARB0/1/2/3 */ 
static unsigned int nondepend_clock_mask[MM_CLOCK_CATEGORY_COUNT] = {
    0x00000001,
    0x26B7608A,
    0x00000009,
    0x00048000,
    0x00010000,
};

static unsigned int dependency_mask[MM_CLOCK_CATEGORY_COUNT][MT65XX_LARB_GROUP_COUNT] = {
    /* depends on 
     *  LARB0        LARB1       LARB2      LARB3     
     */
    { 0x00000100, 0x00000080, 0x0000007E, 0x00000000},     /* MMSYS1_PDN0 */
    { 0xD9009014, 0x00480F61, 0x00000000, 0x00000000},     /* MMSYS1_PDN1 */
    { 0x00000100, 0x000002B6, 0x00000040, 0x00000000},     /* MMSYS1_PDN2 */
    { 0x00000000, 0x00000000, 0x00000000, 0x000B401D},     /* MMSYS2_PDN0 */
    { 0x00000000, 0x00000000, 0x00000000, 0x00000064},     /* AUDIO_PDN */
};

struct larb_clock {
    int clk_id;
    unsigned int mask; 
};

static struct larb_clock larb_clocks[MT65XX_LARB_CLOCK_COUNT] = {
    {
        .clk_id = MT65XX_PDN_MM_SMI_LARB0_EMI,          /* 72  @ MM1_0, 260MHz EMI Domain */
        .mask   = 0x00000100,
    }, {
        .clk_id = MT65XX_PDN_MM_SMI_LARB0,              /* 136 @ MM1_2, 130MHz Domain */
        .mask   = 0x01000000,
    }, {
        .clk_id = MT65XX_PDN_MM_SMI_LARB1_EMI,          /* 71  @ MM1_0, 260MHz EMI Domain */
        .mask   = 0x00000080,
    }, {
        .clk_id = MT65XX_PDN_MM_SMI_LARB1,              /* 135 @ MM1_2, 130MHz Domain */
        .mask   = 0x00800000,
    }, {
        .clk_id = MT65XX_PDN_MM_SMI_LARB2_ACP_BUS,      /* 67  @ MM1_0, 260MHz Domain */
        .mask   = 0x00000008,
    }, {
        .clk_id = MT65XX_PDN_MM_SMI_LARB2_260MHZ,       /* 68  @ MM1_0, 260MHz Domain */
        .mask   = 0x00000010,
    }, {
        .clk_id = MT65XX_PDN_MM_SMI_LARB2_EMI,          /* 69  @ MM1_0, 260MHz EMI Domain */
        .mask   = 0x00000020,
    }, {
        .clk_id = MT65XX_PDN_MM_SMI_LARB2_ACP_BUS_EMI,  /* 70  @ Mm1_0, 260MHz EMI Domain */
        .mask   = 0x00000040,
    }, {
        .clk_id = MT65XX_PDN_MM_SMI_LARB2,              /* 134 @ MM1_2, 130MHz Domain */
        .mask   = 0x00400000,
    }, {
        .clk_id = MT65XX_PDN_MM_SMI_LARB3_FULL,         /* 160 @ MM2  , 260MHz EMI Domain */
        .mask   = 0x00000001,
    }, {
        .clk_id = MT65XX_PDN_MM_SMI_LARB3_HALF,         /* 177 @ MM2  , 130MHz FMM Domain */
        .mask   = 0x00020000,
    }, 
};

struct larb_module {
    int mod_id;        
    int number;         /* number of larb clock bits */
    int start;          /* indicating starts from which larb clock in array larb_clocks */
    unsigned int mask;  
    int dirty;          /* flag for whether backup LARBx */
};

static struct larb_module larb_modules[MT65XX_LARB_GROUP_COUNT] = {
    {
        .mod_id = MT65XX_LARB0,   /* LARB 0 */
        .number = 2,
        .start  = 0,
        .mask   = 0x01000100,     /* LARB0 Clock Mask: High 16 Bits: MMSYS1_PDN2, Low 16 Bits: MMSYS1_PDN0 */
        .dirty  = 1,
    }, {
        .mod_id = MT65XX_LARB1,   /* LARB 1 */
        .number = 2,
        .start  = 2,
        .mask   = 0x00800080,     /* LARB1 Clock Mask: High 16 Bits: MMSYS1_PDN2, Low 16 Bits: MMSYS1_PDN0 */
        .dirty  = 1,
    }, {
        .mod_id = MT65XX_LARB2,   /* LARB 2 */
        .number = 5,
        .start  = 4,
        .mask   = 0x00400078,     /* LARB2 Clock Mask: High 16 Bits: MMSYS1_PDN2, Low 16 Bits: MMSYS1_PDN0 */
        .dirty  = 1,
    }, {
        .mod_id = MT65XX_LARB3,   /* LARB 3 */
        .number = 2,
        .start  = 9,
        .mask   = 0x00020001,     /* LARB3 Clock Mask: MMSYS2 */
        .dirty  = 1,
    },
};


//static DEFINE_MUTEX(larb_monitor_lock);
static LIST_HEAD(larb_monitor_handlers);

void register_larb_monitor(struct larb_monitor *handler)
{
	struct list_head *pos;

	//mutex_lock(&larb_monitor_lock);
	list_for_each(pos, &larb_monitor_handlers) {
		struct larb_monitor *l;
		l = list_entry(pos, struct larb_monitor, link);
		if (l->level > handler->level)
			break;
	}
	list_add_tail(&handler->link, pos);
	//mutex_unlock(&larb_monitor_lock);
}
EXPORT_SYMBOL(register_larb_monitor);


void unregister_larb_monitor(struct larb_monitor *handler)
{
	//mutex_lock(&larb_monitor_lock);
	list_del(&handler->link);
	//mutex_unlock(&larb_monitor_lock);
}
EXPORT_SYMBOL(unregister_larb_monitor);

int get_larb_monitor_handler_number(void)
{
    int number = 0;
	struct list_head *pos;

	//mutex_lock(&larb_monitor_lock);
	list_for_each(pos, &larb_monitor_handlers) {
		number++;
	}
	//mutex_unlock(&larb_monitor_lock);

    return number;
}

struct larb_monitor* get_larb_monitor_handler(int handler_idx)
{
    struct larb_monitor* h = NULL;    
	struct list_head *pos;
    int i = 0;

	//mutex_lock(&larb_monitor_lock);
	list_for_each(pos, &larb_monitor_handlers) {
        if (i == handler_idx) {
    		h = list_entry(pos, struct larb_monitor, link);
            break;
        }
		i++;
	}
	//mutex_unlock(&larb_monitor_lock);

    return h;
}

void get_larb_monitor_handlers(struct larb_monitor **array)
{
	struct list_head *pos;
    int i = 0;

	//mutex_lock(&larb_monitor_lock);
	list_for_each(pos, &larb_monitor_handlers) {
        array[i++] = list_entry(pos, struct larb_monitor, link);
	}
	//mutex_unlock(&larb_monitor_lock);

    return;
}

static void larb_backup(int larb_idx)
{
	struct larb_monitor *pos;

	if (!larb_modules[larb_idx].dirty) {
		return;
	}
	/*help mm enalbe larb*/
	clk_info("[%s]: enable larb %d for backup\n", __func__, larb_idx);
	enable_larb_internal(larb_idx,"clkmgr_larb_backup");
  //if (clkmgr_debug_mask & DEBUG_LARB) {
	clk_info("[%s]: start to callback larb %d\n", __func__, larb_idx);
	//}

	//mutex_lock(&larb_monitor_lock);

	list_for_each_entry(pos, &larb_monitor_handlers, link) {
		if (pos->backup != NULL)
			pos->backup(pos, larb_idx);
	}

	//mutex_unlock(&larb_monitor_lock);
	disable_larb_internal(larb_idx,"clkmgr_larb_backup");

	larb_modules[larb_idx].dirty = 0;
}

static void larb_restore(int larb_idx)
{
	struct larb_monitor *pos;

	if (larb_modules[larb_idx].dirty) {
		return;
	}
	clk_info("[%s]: enalbe larb %d for restore\n", __func__, larb_idx);
	enable_larb_internal(larb_idx,"clkmgr_larb_restore");
	//if (clkmgr_debug_mask & DEBUG_LARB) {
	clk_info("[%s]: start to restore larb %d\n", __func__, larb_idx);
	//}

	larb_modules[larb_idx].dirty = 1;

	//mutex_lock(&larb_monitor_lock);

	list_for_each_entry(pos, &larb_monitor_handlers, link) {
		if (pos->restore != NULL)
			pos->restore(pos, larb_idx);
	}

	//mutex_unlock(&larb_monitor_lock);

	disable_larb_internal(larb_idx,"clkmgr_larb_restore");
}


static int index_of(int mm_category, unsigned int mask)
{
    int idx = 0;

    for (idx = 0; idx < MT65XX_LARB_GROUP_COUNT; idx++) {
        if (dependency_mask[mm_category][idx] & mask) {
            break;
        }
    }

    return idx;
}


static void enable_larb_internal(int larb_idx, char* mod_name)
{
    int i = 0;
    int category = 0;
    int offset = 0;
    unsigned int mask = 0;
    unsigned int mask_tmp = 0;

    for (i = larb_modules[larb_idx].start; i < (larb_modules[larb_idx].start + larb_modules[larb_idx].number); i++) {
        category = larb_clocks[i].clk_id / CLK_REG_WIDTH;
        offset = larb_clocks[i].clk_id % CLK_REG_WIDTH;
#ifdef CLK_MGR_DEBUG
        clk_stat_locked(category, offset, mod_name, 1);
#endif
        BUG_ON(clk_mgr.clock[category][offset].count < 0);
        if (clk_mgr.clock[category][offset].count == 0) {
            mask |= larb_clocks[i].mask;
        }

        if (partial_log_mask[category] & larb_clocks[i].mask) {
            clk_info("[%s]: before ++, clock[%d].count = %d\n",
                    __func__, larb_clocks[i].clk_id, clk_mgr.clock[category][offset].count);
        }
        clk_mgr.clock[category][offset].count++;
    }

    if ((clkmgr_debug_mask & DEBUG_LARB) && mask && mask != larb_modules[larb_idx].mask) {
        clk_info("[%s]: larb%d mask: 0x%08x, mask: 0x%08x\n", 
                __func__, larb_idx, larb_modules[larb_idx].mask, mask);
    }   
    
    if (mask) {
        switch (larb_idx) {
            case MT65XX_LARB0:
            case MT65XX_LARB1:
            case MT65XX_LARB2:
                mask_tmp = mask & 0xFFFF;
                if (mask_tmp) {
                    enable_clock_internal(MT65XX_CLOCK_MMSYS1_PDN0, mask_tmp);
                }
                mask_tmp = (mask >> 16) & 0xFFFF;
                if (mask_tmp) {
                    enable_clock_internal(MT65XX_CLOCK_MMSYS1_PDN2, mask_tmp);
                }
                break;
            case MT65XX_LARB3:
                mask_tmp = mask;
                if (mask_tmp) {
                    enable_clock_internal(MT65XX_CLOCK_MMSYS2_PDN, mask_tmp);
                }
                break;
            /* should never run into default branch */
            default:
                clk_warn("[%s]: larb_idx %d\n", __func__, larb_idx);
        }
    }

    return;
}



static void disable_larb_internal(int larb_idx, char *mod_name)
{
    int i = 0;
    int category = 0;
    int offset = 0;
    unsigned int mask = larb_modules[larb_idx].mask;
    unsigned int mask_tmp = 0;

    for (i = larb_modules[larb_idx].start; i < (larb_modules[larb_idx].start + larb_modules[larb_idx].number); i++) {
        category = larb_clocks[i].clk_id / CLK_REG_WIDTH;
        offset = larb_clocks[i].clk_id % CLK_REG_WIDTH;
#ifdef CLK_MGR_DEBUG
        clk_stat_locked(category, offset, mod_name, 0);
#endif
        if (partial_log_mask[category] & larb_clocks[i].mask) {
            clk_info("[%s]: before --, clock[%d].count = %d\n", 
                    __func__, larb_clocks[i].clk_id, clk_mgr.clock[category][offset].count);
        }
        BUG_ON(clk_mgr.clock[category][offset].count <= 0);
        clk_mgr.clock[category][offset].count--;
        if (clk_mgr.clock[category][offset].count != 0) {
            mask &= ~larb_clocks[i].mask;
        }
    }

    if ((clkmgr_debug_mask & DEBUG_LARB) && mask && mask != larb_modules[larb_idx].mask) {
        clk_info("[%s]: larb%d mask: 0x%08x, mask: 0x%08x\n", 
                __func__, larb_idx, larb_modules[larb_idx].mask, mask);
    }   
 
    if (!mask) {
        return;
    }

    switch (larb_idx) {
        case MT65XX_LARB0:
        case MT65XX_LARB1:
        case MT65XX_LARB2:
#ifdef FORCE_UNGATE_DEBUG
            mask_tmp = (mask & 0xFFFF) & (~force_ungate_mask[MT65XX_CLOCK_MMSYS1_PDN0]);
#else
            mask_tmp = (mask & 0xFFFF);
#endif
            if (mask_tmp) {
                disable_clock_internal(MT65XX_CLOCK_MMSYS1_PDN0, mask_tmp);
            }

#ifdef FORCE_UNGATE_DEBUG
            mask_tmp = ((mask >> 16) & 0xFFFF) & (~force_ungate_mask[MT65XX_CLOCK_MMSYS1_PDN2]);
#else
            mask_tmp = ((mask >> 16) & 0xFFFF);
#endif
            if (mask_tmp) {
                disable_clock_internal(MT65XX_CLOCK_MMSYS1_PDN2, mask_tmp);
            }
            break;

        case MT65XX_LARB3:
#ifdef FORCE_UNGATE_DEBUG
            mask_tmp = mask & (~force_ungate_mask[MT65XX_CLOCK_MMSYS2_PDN]);
#else
            mask_tmp = mask;
#endif
            if (mask_tmp) {
                disable_clock_internal(MT65XX_CLOCK_MMSYS2_PDN, mask_tmp);
            }
            break;

            /* should never run into default branch */
        default:
            clk_warn("[%s]: larb_idx %d\n", __func__, larb_idx);
    }

    return;
}

#define INVALID_LARB_ID(id)     ((id) < 0 || (id) >= MT65XX_LARB_GROUP_COUNT)


bool larb_is_on(int larb_idx)
{
    unsigned long flags;
	unsigned int mask = 0; 
	unsigned int clock_state = 0;
	bool ret = false;

	BUG_ON(INVALID_LARB_ID(larb_idx));

	spin_lock_irqsave(&clkmgr_lock, flags);
	if (larb_idx != 2) {
		mask = larb_modules[larb_idx].mask;
	} else {
		mask = 0x30;	
	}
	switch (larb_idx) {
		case MT65XX_LARB0:
		case MT65XX_LARB1:
			clock_state = (clk_mgr.clock_state[2] & 0xFFFF) |
					((clk_mgr.clock_state[4] << 16) & 0xFFFF0000);
			break;
		case MT65XX_LARB2:
			clock_state = clk_mgr.clock_state[2];
			break;

		case MT65XX_LARB3:
			clock_state = clk_mgr.clock_state[5];
			break;

		default:
			clk_info("[%s]bad larb id %d\n", __func__, larb_idx);
			BUG_ON(1);
	}

	if ((clock_state & mask) == mask) {
		ret = true;
	}

    spin_unlock_irqrestore(&clkmgr_lock, flags);
	return ret;
}
EXPORT_SYMBOL(larb_is_on);


int enable_larb(int larb_idx, char *mod_name)
{
    unsigned long flags;

#ifdef OP_MODE_DEBUG
    if (!op_mode) {
        return 0;
    }
#endif

    BUG_ON(!mod_name);

    if (!initialized) {
        clk_warn("[%s]: clkmgr not initialized yet! [id=%d] [mod=%s]\n",
                 __func__, larb_idx, mod_name);
        return 1;
    }

#ifdef BUG_ON_DEBUG
    BUG_ON(INVALID_LARB_ID(larb_idx));
#else
    if (INVALID_LARB_ID(larb_idx)) {
        clk_warn("[%s]: %s pass an invalid id [%d]\n",
                __func__, mod_name, larb_idx);
        return 1;
    }
#endif

    if (clkmgr_debug_mask & DEBUG_LARB) {
        clk_info("[%s]: [id=%d] [mod=%s]\n", 
                __func__, larb_idx, mod_name);
    }

    spin_lock_irqsave(&clkmgr_lock, flags);

    if (larb_idx < MT65XX_LARB3) {
        mm_power_prepare(MT65XX_SUBSYS_MM1, "larb", larb_idx);
    } else if (larb_idx == MT65XX_LARB3) {
        mm_power_prepare(0, "larb", larb_idx);
    }

    enable_larb_internal(larb_idx, mod_name);
    if (!larb_modules[larb_idx].dirty) {
        if (clkmgr_debug_mask & DEBUG_LARB) {
            clk_info("[%s]: larb%d clock is 1st enabled\n", __func__, larb_idx);
        }
        larb_restore(larb_idx);
    }

    spin_unlock_irqrestore(&clkmgr_lock, flags);
    return 0;
}


int disable_larb(int larb_idx, char *mod_name)
{
    unsigned long flags;

#ifdef OP_MODE_DEBUG
    if (!op_mode) {
        return 0;
    }
#endif

    BUG_ON(!mod_name);

    if (!initialized) {
        clk_warn("[%s]: clkmgr not initialized yet! [id=%d] [mod=%s]\n",
                 __func__, larb_idx, mod_name);
        return 1;
    }

#ifdef BUG_ON_DEBUG
    BUG_ON(INVALID_LARB_ID(larb_idx));
#else
    if (INVALID_LARB_ID(larb_idx)) {
        clk_warn("[%s]: %s pass an invalid id [%d]\n",
                __func__, mod_name, larb_idx);
        return 1;
    }
#endif

    if (clkmgr_debug_mask & DEBUG_LARB) {
        clk_info("[%s]: [id=%d] [mod=%s]\n", 
                __func__, larb_idx, mod_name);
    }

    spin_lock_irqsave(&clkmgr_lock, flags);

    disable_larb_internal(larb_idx, mod_name);

    if (larb_idx < MT65XX_LARB3) {
        mm_power_finish(MT65XX_SUBSYS_MM1, "larb", larb_idx);
    } else if (larb_idx == MT65XX_LARB3) {
        mm_power_finish(0, "larb", larb_idx);
    }

    spin_unlock_irqrestore(&clkmgr_lock, flags);
    return 0;
}


#define AUDIO_F26M_CLK_OFFSET   (MT65XX_PDN_MM_F26M % CLK_REG_WIDTH)
#define AUDIO_CORE_CLK_OFFSET   (MT65XX_PDN_MM_AUDIO_CORE % CLK_REG_WIDTH)

static void enable_audio_core(void)
{
    unsigned int mask = 0;

    BUG_ON(clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_CORE_CLK_OFFSET].count < 0);
    BUG_ON(clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_F26M_CLK_OFFSET].count < 0);
   
    /* audio core clock */ 
    if (clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_CORE_CLK_OFFSET].count == 0) {
        mask |= (0x1 << AUDIO_CORE_CLK_OFFSET);
    }
    clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_CORE_CLK_OFFSET].count++;

    /* audio f26m clock */
    if (clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_F26M_CLK_OFFSET].count == 0) {
        mask |= (0x1 << AUDIO_F26M_CLK_OFFSET);
    }
    clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_F26M_CLK_OFFSET].count++;

    if (mask) {
        enable_clock_internal(MT65XX_CLOCK_MMSYS2_PDN, mask);
    }
}

static void disable_audio_core(void)
{
    unsigned int mask = 0;

    BUG_ON(clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_CORE_CLK_OFFSET].count <= 0);
    BUG_ON(clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_F26M_CLK_OFFSET].count <= 0);
   
    /* audio core clock */ 
    clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_CORE_CLK_OFFSET].count--;
    if (clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_CORE_CLK_OFFSET].count == 0) {
        mask |= (0x1 << AUDIO_CORE_CLK_OFFSET);
    }

    /* audio f26m clock */
    clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_F26M_CLK_OFFSET].count--;
    if (clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][AUDIO_F26M_CLK_OFFSET].count == 0) {
        mask |= (0x1 << AUDIO_F26M_CLK_OFFSET);
    }

#ifdef  FORCE_UNGATE_DEBUG
    mask &= (~force_ungate_mask[MT65XX_CLOCK_MMSYS2_PDN]);
#endif

    if (mask) {
        disable_clock_internal(MT65XX_CLOCK_MMSYS2_PDN, mask);
    }
}


int enable_clock(enum mt65xx_clock_id id, char *mod_name)
{
    unsigned long flags;
    int ret = 0;
    int category = id / CLK_REG_WIDTH;
    int offset = id % CLK_REG_WIDTH;
    unsigned int mask = 0x1 << offset;
    unsigned short wpll_clksrc_mask = 0;
    int mm_category = 0;
    int larb_idx = 0;
    int restore = 0;
    char str_tmp[MAX_NAME_LEN];
    int subsys_id = 0;

#ifdef OP_MODE_DEBUG
    if (!op_mode) {
        return 0;
    }
#endif

    /* mod_name is only for debug, but null string is not allowed */
    BUG_ON(!mod_name);

    /* no operation is allowed before clock manager initialization done */
    if (!initialized) {
        clk_warn("[%s]: clkmgr not initialized yet! [id=%d] [mod=%s]\n",
                 __func__, id, mod_name);
        return 1;
    }

    /* check parameter validation */
#ifdef BUG_ON_DEBUG
    BUG_ON(INVALID_CATEGORY_ID(category));
    BUG_ON(INVALID_CLOCK_ID(category, mask));
#else
    if (INVALID_CATEGORY_ID(category) || INVALID_CLOCK_ID(category, mask)) {
        clk_warn("[%s]: %s pass an invalid id [%d]: [%d][%d]\n",
                __func__, mod_name, id, category, offset);
        return 1;
    }
#endif

    /* 
     * log for debug, 2 debug switches 
     */
    if ((clkmgr_debug_mask & DEBUG_CLOCK) || (partial_log_mask[category] & mask)) {
        clk_info("[%s]: [id=%d] [category=%d] [offset=%d] [name=%s] [mod=%s]\n", 
                __func__, id, category, offset, 
                clk_mgr.clock[category][offset].name, mod_name);
    }

    spin_lock_irqsave(&clkmgr_lock, flags);

#ifdef CLK_MGR_DEBUG
    clk_stat_locked(category, offset, mod_name, 1);
#endif

#ifdef MSDC_DEBUG_ENABLED
    if ((category == 0) && (mask & 0x78000)) {
        if (get_clock_state(category, mask, LOCAL_STATE) !=
            get_clock_state(category, mask, REG_STATE)) {
            clk_info("[%s]: local_state!=reg_state (%d, %d)\n", 
                __func__, id, clk_mgr.clock[category][offset].count);
            WARN_ON(1);
        }
    }
#endif

    if ((clkmgr_debug_mask & DEBUG_CLOCK) || 
            (partial_log_mask[category] & mask)) {
        clk_info("[%s]: before ++: clock[%d].count = %d\n", 
                __func__, id, clk_mgr.clock[category][offset].count);
    }
    clk_mgr.clock[category][offset].count++;

    if (clk_mgr.clock[category][offset].count > 1) {
        spin_unlock_irqrestore(&clkmgr_lock, flags);
        return 0;
    } else if (clk_mgr.clock[category][offset].count < 1) {
        clk_err("[%s]: after ++: clk_mgr.clock[%d].count = %d\n", 
                __func__, id, clk_mgr.clock[category][offset].count);
        spin_unlock_irqrestore(&clkmgr_lock, flags);
        return 1;
    }

    if (CATEGORY_AP1(category)) {
        wpll_clksrc_mask = should_power_on_wpll_clksrc(mask);
        if (wpll_clksrc_mask) {
            //clk_info("[%s] enable wpll clksrc 0x%x for clock %d\n", __func__, wpll_clksrc_mask, id);
            enable_wpll_clksrc_op(wpll_clksrc_mask);
        }
    }

    /* 
     * for the case that clock is gated, so we really need to turn on it
     */
    if (CATEGORY_MM(category)) {
        if (CATEGORY_MM1(category)) {
            subsys_id = MT65XX_SUBSYS_MM1;
        } else if (CATEGORY_MM2(category) && (mask & MFG_CHECK_MASK)) {
            subsys_id = MT65XX_SUBSYS_MM2;
        }
        /* enable subsys if necessary */
        mm_power_prepare(subsys_id, "clock", id);

        mm_category = category - 2;

        /* enable larb clock if necessary */
        if (!(nondepend_clock_mask[mm_category] & mask)) {
            larb_idx = index_of(mm_category, mask); 
            BUG_ON(larb_idx >= MT65XX_LARB_GROUP_COUNT);
            if (unlikely(larb_clock_mask[mm_category] & mask)) {
                if (!larb_modules[larb_idx].dirty) {
                    if (clkmgr_debug_mask & DEBUG_LARB) {
                        clk_info("[%s]: larb%d clock[%d] is 1st enabled\n", 
                                __func__, larb_idx, id);
                    }
                    restore = 1;
                }
            } else {
                if (clkmgr_debug_mask & DEBUG_LARB) {
                    clk_info("[%s]: clock[%d] depends on larb %d\n", 
												__func__, id, larb_idx);
								}
								sprintf(str_tmp, "%s_%s_%d", "clkmgr", mod_name, id);
								enable_larb_internal(larb_idx, str_tmp);
								if (!larb_modules[larb_idx].dirty) {
									if (clkmgr_debug_mask & DEBUG_LARB) {
										clk_info("[%s]: larb%d clock is 1st enabled\n", __func__, larb_idx);
									}
									restore = 1;
								}
						}
				}
		}

    /* 
     * once request to open clock @ AUDIO_TOP_CON0, 
     * we need turn on audio_core clock and f26m_ck
     */
    if (CATEGORY_AUD(category)) {
        enable_audio_core();
    }

    ret = enable_clock_internal(category, mask);

    if (!ret && restore) {
        larb_restore(larb_idx);
    }

    spin_unlock_irqrestore(&clkmgr_lock, flags);

    return ret;
}
EXPORT_SYMBOL(enable_clock);


int disable_clock(enum mt65xx_clock_id id, char *mod_name)
{
    unsigned long flags;
    int ret = 0;
    int category = id / CLK_REG_WIDTH;
    int offset = id % CLK_REG_WIDTH;
    unsigned int mask = 0x1 << offset;
    unsigned short wpll_clksrc_mask = 0;
    int mm_category = 0;
    int larb_idx = 0; 
    char str_tmp[MAX_NAME_LEN];
    int subsys_id = 0;

#ifdef OP_MODE_DEBUG
    if (!op_mode) {
        return 0;
    }
#endif

    BUG_ON(!mod_name);

    if (!initialized) {
        clk_warn("[%s]: clkmgr not initialized yet! [id=%d] [mod=%s]\n",
                 __func__, id, mod_name);
        return 1;
    }

#ifdef BUG_ON_DEBUG
    BUG_ON(INVALID_CATEGORY_ID(category));
    BUG_ON(INVALID_CLOCK_ID(category, mask));
#else
    if (INVALID_CATEGORY_ID(category) || INVALID_CLOCK_ID(category, mask)) {
        clk_warn("[%s]: %s pass an invalid id [%d]: [%d][%d]\n",
                __func__, mod_name, id, category, offset);
        return 1;
    }
#endif

    if ((clkmgr_debug_mask & DEBUG_CLOCK) || (partial_log_mask[category] & mask)) {
        clk_info("[%s]: [id=%d] [category=%d] [offset=%d] [name=%s] [mod=%s]\n", 
                __func__, id, category, offset, 
                clk_mgr.clock[category][offset].name, mod_name);
    }

    spin_lock_irqsave(&clkmgr_lock, flags);

#ifdef CLK_MGR_DEBUG
    clk_stat_locked(category, offset, mod_name, 0);
#endif

    if (clk_mgr.clock[category][offset].count <= 0) {
        clk_warn("[%s]: clock[%d].count = %d, %s request to disable it\n", 
                __func__, id, clk_mgr.clock[category][offset].count, mod_name);
        spin_unlock_irqrestore(&clkmgr_lock, flags);
        return 1;
    }

#ifdef MSDC_DEBUG_ENABLED
    if ((category == 0) && (mask & 0x78000)) {
        if (get_clock_state(category, mask, LOCAL_STATE) !=
            get_clock_state(category, mask, REG_STATE)) {
            clk_info("[%s]: local_state!=reg_state (%d, %d)\n", 
                __func__, id, clk_mgr.clock[category][offset].count);
            WARN_ON(1);
        }
    }
#endif

    if ((clkmgr_debug_mask & DEBUG_CLOCK) || 
            (partial_log_mask[category] & mask)) {
        clk_warn("[%s]: before --: clock[%d].count = %d\n", 
                __func__, id, clk_mgr.clock[category][offset].count);
    }

    clk_mgr.clock[category][offset].count--;

    if (clk_mgr.clock[category][offset].count > 0) {
        spin_unlock_irqrestore(&clkmgr_lock, flags);
        return 0;
    }

#ifdef FORCE_UNGATE_DEBUG
    if ((force_ungate_mask[category] & mask)) {
        clk_info("[%s]: id = %d, it's a force ungated clock bit\n", __func__, id);

        if (CATEGORY_AP1(category)) {
            wpll_clksrc_mask = should_power_off_wpll_clksrc(mask);
            if (wpll_clksrc_mask) {
                //clk_info("[%s] disable wpll clksrc 0x%x for clock %d\n", __func__, wpll_clksrc_mask, id);
                disable_wpll_clksrc_op(wpll_clksrc_mask);
            }
        }

        spin_unlock_irqrestore(&clkmgr_lock, flags);
        return 0;
    }
#endif

    /* 
     * for the case that clock is ungated, so we really need to turn off it
     */
    ret = disable_clock_internal(category, mask);
    if (ret) {
#ifdef MSDC_DEBUG_ENABLED
        if ((category == 0) && (mask & 0x78000)) {
            clk_err("[%s]: disable_clock_internal failed(%d, %d)\n", 
                __func__, id, clk_mgr.clock[category][offset].count);
        }
#endif
        goto exit;
    }

    if (CATEGORY_AP1(category)) {
        wpll_clksrc_mask = should_power_off_wpll_clksrc(mask);
        if (wpll_clksrc_mask) {
            //clk_info("[%s] disable wpll clksrc 0x%x for clock %d\n", __func__, wpll_clksrc_mask, id);
            disable_wpll_clksrc_op(wpll_clksrc_mask);
        }
    }

    if (CATEGORY_AUD(category)) {
        disable_audio_core();
    }

    if (CATEGORY_MM(category)) {
        mm_category = category - 2;
        if (!(nondepend_clock_mask[mm_category] & mask)) {
            larb_idx = index_of(mm_category, mask); 
            //clk_info("[%s]larb_idx = %d for clock %d\n", __func__, larb_idx, id);
            BUG_ON(larb_idx >= MT65XX_LARB_GROUP_COUNT);

            if (!(larb_clock_mask[mm_category] & mask)) {
                if (clkmgr_debug_mask & DEBUG_LARB) {
                    clk_info("[%s]: clock[%d] depends on larb %d\n", 
                            __func__, id, larb_idx);
                }
                sprintf(str_tmp, "%s_%s_%d", "clkmgr", mod_name, id);
                disable_larb_internal(larb_idx, str_tmp);
            }
        }

        if (CATEGORY_MM1(category)) {
            subsys_id = MT65XX_SUBSYS_MM1;
        } else if (CATEGORY_MM2(category) && (mask & MFG_CHECK_MASK)) {
            subsys_id = MT65XX_SUBSYS_MM2;
        }
        mm_power_finish(subsys_id, "clock", id);
    }

exit:
    spin_unlock_irqrestore(&clkmgr_lock, flags);
    return 0;

}
EXPORT_SYMBOL(disable_clock);


struct mipi mipi[MT65XX_MIPI_COUNT];
static const char mt65xx_mipi_name[MT65XX_MIPI_COUNT][MAX_NAME_LEN] = {
    "MIPI_TX",                    /* bit 0 */
    "MIPI_RX",
};


void dump_mipi_regs(void)
{
    clk_info("[%s] PLL_CON1    = 0x%04x\n", __func__, DRV_Reg16(PLL_CON1));
    clk_info("[%s] MIPITX_CON0 = 0x%04x\n", __func__, DRV_Reg16(MIPI_TX_CON0));
    clk_info("[%s] MIPITX_CON6 = 0x%04x\n", __func__, DRV_Reg16(MIPI_TX_CON6));
    clk_info("[%s] MIPITX_CON9 = 0x%04x\n", __func__, DRV_Reg16(MIPI_TX_CON9));
    clk_info("[%s] MIPIRX_CON1 = 0x%04x\n", __func__, DRV_Reg16(MIPI_RX_CON1));
    clk_info("[%s] MIPIRX_CON4 = 0x%04x\n", __func__, DRV_Reg16(MIPI_RX_CON4));
    clk_info("[%s] MIPIRX_CON5 = 0x%04x\n", __func__, DRV_Reg16(MIPI_RX_CON5));
    clk_info("[%s] MIPI_CON    = 0x%08x\n", __func__, DRV_Reg32(MIPI_CON));
}


static void enable_mipi_op(int id, int tx_in_use, int rx_in_use)
{
    if (tx_in_use + rx_in_use == 1) {
        /* set ANA_26M_EN = 1 */
        DRV_SetReg16(PLL_CON1, 0x1 << 4);
        /* set MIPI_ISO = 0 */
        DRV_ClrReg32(MIPI_CON, 0x1 << 4);
    }

    switch (id) {
    case MT65XX_MIPI_TX:
        if (!rx_in_use) {
            /* set PLL_CKLR_EN = 1, PLL_EN = 1 */
            DRV_SetReg16(MIPI_TX_CON0, 0x3);
            /* set RG_LNT_BGR_CHPEN = 1, RG_LNT_BGR_EN = 1 */
            DRV_SetReg16(MIPI_TX_CON6, 0x3);
        } else {
            /* only set PLL_EN = 1 */
            DRV_SetReg16(MIPI_TX_CON0, 0x1);
        }
        /* set RG_TXLDO_VOCAL = 4, RG_TXLDO_EN = 1 */
        DRV_SetReg16(MIPI_TX_CON9, 0x4 << 12 | 0x1 << 3);
        /* set TX_EN = 1 */
        DRV_SetReg32(MIPI_CON, 0x1 << 8);

        return;

    case MT65XX_MIPI_RX:
        if (!tx_in_use) {
            /* set PLL_CKLR_EN = 1, PLL_EN = x */
            DRV_SetReg16(MIPI_TX_CON0, 0x2);
            /* set RG_LNT_BGR_CHPEN = 1, RG_LNT_BGR_EN = 1 */
            DRV_SetReg16(MIPI_TX_CON6, 0x3);
        }
        /* set RG_RXLDO_VOCAL = 4, RG_RXLDO_EN = 1 */
        DRV_SetReg16(MIPI_RX_CON5, 0x4 << 12 | 0x1 << 3);
        /* set RG_VOS_CAL_EN = 1 */
        DRV_SetReg16(MIPI_RX_CON4, 0x1 << 5);
        /* set RX_EN = 1 */
        DRV_SetReg32(MIPI_CON, 0x1 << 12);
        
        return;

    default:
        clk_info("[%s]bad mipi id %d\n", __func__, id);
        BUG();
    }

    return;
}


static void disable_mipi_op(int id, int tx_in_use, int rx_in_use)
{
    switch (id) {
    case MT65XX_MIPI_TX:
        if (!rx_in_use) {
            /* set PLL_CKLR_EN = 0, PLL_EN = 0 */
            DRV_ClrReg16(MIPI_TX_CON0, 0x3);
            /* set RG_LNT_BGR_CHPEN = 0, RG_LNT_BGR_EN = 0 */
            DRV_ClrReg16(MIPI_TX_CON6, 0x3);
        } else {
            /* only set PLL_EN = 0 */
            DRV_ClrReg16(MIPI_TX_CON0, 0x1);
        }
        /* set RG_TXLDO_EN = 0 */
        DRV_ClrReg16(MIPI_TX_CON9, 0x1 << 3);
        /* set TX_EN = 0 */
        DRV_ClrReg32(MIPI_CON, 0x1 << 8);

        break;

    case MT65XX_MIPI_RX:
        if (!tx_in_use) {
            /* set PLL_CKLR_EN = 0, PLL_EN = x */
            DRV_ClrReg16(MIPI_TX_CON0, 0x2);
            /* set RG_LNT_BGR_CHPEN = 0, RG_LNT_BGR_EN = 0 */
            DRV_ClrReg16(MIPI_TX_CON6, 0x3);
        }
        /* set RG_RXLDO_EN = 0 */
        DRV_ClrReg16(MIPI_RX_CON5, 0x1 << 3);
        /* set RG_VOS_CAL_EN = 0 */
        DRV_ClrReg16(MIPI_RX_CON4, 0x1 << 5);
        /* set RX_EN = 0 */
        DRV_ClrReg32(MIPI_CON, 0x1 << 12);
        
        break;

    default:
        clk_info("[%s]bad mipi id %d\n", __func__, id);
        BUG();
    }

    if (tx_in_use + rx_in_use == 0) {
        /* set ANA_26M_EN = 0 */
        DRV_ClrReg16(PLL_CON1, 0x1 << 4);
        /* set MIPI_ISO = 1 */
        DRV_SetReg32(MIPI_CON, 0x1 << 4);
    }
    return;
}


#define INVALID_MIPI_ID(id) (((id) != MT65XX_MIPI_TX) && ((id) != MT65XX_MIPI_RX))  

int enable_mipi(enum mt65xx_mipi_id id, char *mod_name)
{
    unsigned long flags;

    BUG_ON(!mod_name);

    if (!initialized) {
        clk_warn("[%s]: clkmgr not initialized yet! [id=%d] [mod=%s]\n",
                 __func__, id, mod_name);
        return 1;
    }

#ifdef BUG_ON_DEBUG
    BUG_ON(INVALID_MIPI_ID(id));
#else
    if (INVALID_MIPI_ID(id)) { 
        clk_warn("[%s]: %s pass an invalid id (%d)\n", __func__, mod_name, id);
        return 1;
    }
#endif

    if (clkmgr_debug_mask & DEBUG_MIPI) {
        clk_info("[%s]: [id=%d] [name=%s] [mod=%s]\n", 
                __func__, id, mipi[id].name, mod_name);
    }
    
    spin_lock_irqsave(&clkmgr_lock, flags);

#ifdef CLK_MGR_DEBUG
    //mipi_stat_locked(id, mod_name, 1);
#endif
    
    if (clkmgr_debug_mask & DEBUG_MIPI) {
        clk_info("[%s]: before ++: mipi[%d].count = %d\n", 
                __func__, id, mipi[id].count);
    }
    BUG_ON(mipi[id].count < 0);
    mipi[id].count++;

    if (mipi[id].count > 1) {
        spin_unlock_irqrestore(&clkmgr_lock, flags);
        return 0;
    }

    mm_power_prepare(MT65XX_SUBSYS_MM1, "mipi", id);

    if (clkmgr_debug_mask & DEBUG_MIPI) {
        clk_info("[%s]: <== enable mipi %s\n", __func__, mipi[id].name);
        dump_mipi_regs();
    }
    
    enable_mipi_op(id, mipi[0].count, mipi[1].count);

    if (clkmgr_debug_mask & DEBUG_MIPI) {
        clk_info("[%s]: ==> enable mipi %s\n", __func__, mipi[id].name);
        dump_mipi_regs();
    }

    clk_mgr.mipi_state |= 1 << id;
       
    spin_unlock_irqrestore(&clkmgr_lock, flags);
    
    return 0;
}
EXPORT_SYMBOL(enable_mipi);


int disable_mipi(enum mt65xx_mipi_id id, char *mod_name)
{
    unsigned long flags;

    BUG_ON(!mod_name);

    if (!initialized) {
        clk_warn("[%s]: clkmgr not initialized yet! [id=%d] [mod=%s]\n",
                 __func__, id, mod_name);
        return 1;
    }

#ifdef BUG_ON_DEBUG
    BUG_ON(INVALID_MIPI_ID(id));
#else
    if (INVALID_MIPI_ID(id)) { 
        clk_warn("[%s]: %s pass an invalid id (%d)\n", __func__, mod_name, id);
        return 1;
    }
#endif

    if (clkmgr_debug_mask & DEBUG_MIPI) {
        clk_info("[%s]: [id=%d] [name=%s] [mod=%s]\n", 
                __func__, id, "mipi", mod_name);
    }
    
    spin_lock_irqsave(&clkmgr_lock, flags);

#ifdef CLK_MGR_DEBUG
    //mipi_stat_locked(id, mod_name, 0);
#endif    
    
    if (clkmgr_debug_mask & DEBUG_MIPI) {
        clk_info("[%s]: before --: mipi[%d].count = %d\n", 
                __func__, id, mipi[id].count);
    }
    BUG_ON(mipi[id].count <= 0);
    mipi[id].count--;

    if (mipi[id].count > 0) {
        spin_unlock_irqrestore(&clkmgr_lock, flags);
        return 0;
    }

    if (clkmgr_debug_mask & DEBUG_MIPI) {
        clk_info("[%s]: <== disable mipi %s\n", __func__, mipi[id].name);
        dump_mipi_regs();
    }        

    disable_mipi_op(id, mipi[0].count, mipi[1].count);

    if (clkmgr_debug_mask & DEBUG_MIPI) {
        clk_info("[%s]: ==> disable mipi %s\n", __func__, mipi[id].name);
        dump_mipi_regs();
    }        

    clk_mgr.mipi_state &=  ~(1 << id);

    mm_power_finish(MT65XX_SUBSYS_MM1, "mipi", id);
        
    spin_unlock_irqrestore(&clkmgr_lock, flags);
    
    return 0;
}
EXPORT_SYMBOL(disable_mipi);



#define UPLL_48M_EN     (0x1 << 1)      //For USB
#define MDPLL_297M_EN   (0x1 << 1)      //For 3D Clock

/* PLL lock time without frequency hopping */
static int mt65xx_pll_locktime[MT65XX_PLL_COUNT] = {
    /* in unit of us */
    30,         /* ARMPLL */
    30,         /* MAINPLL */
    20,         /* IPLL */
    20,         /* UPLL */
    20,         /* MDPLL */
    20,         /* TVDDS */
    30,         /* WPLL */
    10*1000,    /* AUDPLL */
    20,         /* MEMPLL */
};

#define PLL_LOCK_TIME_WITH_FREQ_HOPPING     80  /* in unit of us */

#define TVDDS_FREQ_H    (0x0A8E)
#define TVDDS_FREQ_L    (0x38E4)


static void enable_pll_op(enum mt65xx_pll_id id)
{
    if (id == MT65XX_AUDPLL) {
        rtc_enable_abb_32k();
    }

    if (id == MT65XX_WPLL || id == MT65XX_AUDPLL) {
        DRV_SetReg16(mt65xx_pll_regs[id], PLL_STATE_MASK); 
    } else if (id == MT65XX_TVDDS) {
        //DRV_SetReg16(mt65xx_pll_regs[id] + 0x4 * 2, TVDDS_STATE_MASK); 

        /* temp workaround solution for enable TVDDS fail sometimes */
        DRV_WriteReg16(mt65xx_pll_regs[id] + 0x4 * 2, 0x7C57); 
    } else {
        DRV_ClrReg16(mt65xx_pll_regs[id], PLL_STATE_MASK); 
    }

#ifdef AUDPLL_UDELAY    
    udelay(mt65xx_pll_locktime[id]); 
#else
    if (id == MT65XX_AUDPLL) {
        mdelay(mt65xx_pll_locktime[id]/1000); 
    } else {
        udelay(mt65xx_pll_locktime[id]); 
    }
#endif

/*
    switch(id)
    {
    case MT65XX_UPLL:
        DRV_SetReg16(mt65xx_pll_regs[id], UPLL_48M_EN); 
    case MT65XX_WPLL:
        //DRV_SetReg16(mt65xx_pll_regs[id], MDPLL_297M_EN); 
    case MT65XX_WPLL:
        //DRV_SetReg16(mt65xx_pll_regs[id], 0)
    default:
        break;
    }
*/
}

static void disable_pll_op(enum mt65xx_pll_id id)
{
    if (id == MT65XX_WPLL || id == MT65XX_AUDPLL) {
        DRV_ClrReg16(mt65xx_pll_regs[id], PLL_STATE_MASK); 
    } else if (id == MT65XX_TVDDS) {
        //DRV_ClrReg16(mt65xx_pll_regs[id] + 4 * 2, TVDDS_STATE_MASK); 

        /* temp workaround solution for enable TVDDS fail sometimes */
        DRV_WriteReg16(mt65xx_pll_regs[id] + 4 * 2, 0x4057); 
    } else {
        DRV_SetReg16(mt65xx_pll_regs[id], PLL_STATE_MASK); 
    }
    if (id == MT65XX_AUDPLL) {
        rtc_disable_abb_32k();
    }
}

/*
 * w/o spin_lock version, used internally
 */
static int enable_pll_locked(enum mt65xx_pll_id id, char *mod_name)
{
#ifdef CLK_MGR_DEBUG
    pll_stat_locked(id, mod_name, 1);
#endif

    if (clkmgr_debug_mask & DEBUG_PLL) { 
        clk_info("[%s]: before ++: pll[%d].count = %d\n", 
                __func__, id, clk_mgr.pll[id].count);
    }
    clk_mgr.pll[id].count++;

    if (clk_mgr.pll[id].count > 1) {
        return 0;
    } else if (clk_mgr.pll[id].count < 1) {
        clk_warn("[%s]: after ++: clk_mgr.pll[%d].count = %d\n", 
                __func__, id, clk_mgr.pll[id].count);
        return 1;
    }

#ifdef BUG_ON_DEBUG
    BUG_ON(get_pll_state(id, LOCAL_STATE) != get_pll_state(id, REG_STATE));
#endif

    if (get_pll_state(id, LOCAL_STATE) == STATE_ON) {
        return 0;
    }

    if (id == MT65XX_TVDDS) {
        clk_info("[%s]: enable UPLL first before enable TVDDS\n", __func__);
        enable_pll_locked(MT65XX_UPLL, "clkmgr");
    }

    enable_pll_op(id);

    if (get_pll_state(id, REG_STATE) == STATE_OFF) {
        clk_err("[%s]: enable %s fail\n", 
                __func__, clk_mgr.pll[id].name);
        return 1;
    }

    clk_mgr.pll_state |= 0x1 << id;
    return 0; 
}

int enable_pll(enum mt65xx_pll_id id, char *mod_name)
{
		int ret = 0;
    unsigned long flags;

#ifdef OP_MODE_DEBUG
    if (!op_mode) {
        return 0;
    }
#endif

    BUG_ON(!mod_name);

    if (!initialized) {
        clk_warn("[%s]: clkmgr not initialized yet! [id=%d] [mod=%s]\n",
                 __func__, id, mod_name);
        return 1;
    }

#ifdef BUG_ON_DEBUG
    BUG_ON(INVALID_PLL_ID(id));
#else
    if (INVALID_PLL_ID(id)) {
        clk_warn("[%s]: %s pass an invalid id [%d]\n", 
                __func__, mod_name, id);
        return 1;
    }
#endif

    if (clkmgr_debug_mask & DEBUG_PLL) {
        clk_info("[%s]: [id=%d] [name=%s] [mod=%s]\n", 
                __func__, id, clk_mgr.pll[id].name, mod_name);
    }

    spin_lock_irqsave(&clkmgr_lock, flags);
		ret = enable_pll_locked(id,mod_name);
    spin_unlock_irqrestore(&clkmgr_lock, flags);
    return ret;
}
EXPORT_SYMBOL(enable_pll);

/*
 * w/o spin_lock version, used internally
 */
static int disable_pll_locked(enum mt65xx_pll_id id, char *mod_name)
{
#ifdef CLK_MGR_DEBUG
    pll_stat_locked(id, mod_name, 0);
#endif

    if (clk_mgr.pll[id].count <= 0) {
        clk_warn("[%s]: pll[%d].count = %d, %s request to disable it\n", 
            __func__, id, clk_mgr.pll[id].count, mod_name);
        return 1;
    }

    if (clkmgr_debug_mask & DEBUG_CLOCK) { 
        clk_info("[%s]: before --: pll[%d].count = %d\n", 
                __func__, id, clk_mgr.pll[id].count);
    }

    clk_mgr.pll[id].count--;

    if (clk_mgr.pll[id].count > 0) {
        return 0;
    }    
    
#ifdef BUG_ON_DEBUG
    BUG_ON(get_pll_state(id, LOCAL_STATE) != get_pll_state(id, REG_STATE));
#endif

    if (get_pll_state(id, LOCAL_STATE) == STATE_OFF) {
        return 0;
    }

    disable_pll_op(id);

    if (get_pll_state(id, REG_STATE) == STATE_ON) {
        clk_err("[%s]: disable %s fail\n", 
                __func__, clk_mgr.pll[id].name);
        return 1;
    }

    if (id == MT65XX_TVDDS) {
        clk_info("[%s]: disable UPLL after disable TVDDS\n", __func__);
        disable_pll_locked(MT65XX_UPLL, "clkmgr");
    }

    clk_mgr.pll_state &= ~(0x1 << id);
		return 0;
}

int disable_pll(enum mt65xx_pll_id id, char *mod_name)
{
    unsigned long flags;
		int ret;

#ifdef OP_MODE_DEBUG
    if (!op_mode) {
        return 0;
    }
#endif

    BUG_ON(!mod_name);

    if (!initialized) {
        clk_warn("[%s]: clkmgr not initialized yet! [id=%d] [mod=%s]\n",
                 __func__, id, mod_name);
        return 1;
    }

#ifdef BUG_ON_DEBUG
    BUG_ON(INVALID_PLL_ID(id));
#else
    if (INVALID_PLL_ID(id)) {
        clk_warn("[%s]: %s pass an invalid id [%d]\n", 
                __func__, mod_name, id);
        return 1;
    }
#endif

    if (clkmgr_debug_mask & DEBUG_PLL) {
        clk_info("[%s]: [id=%d] [name=%s] [mod=%s]\n", 
                __func__, id, clk_mgr.pll[id].name, mod_name);
    }

    spin_lock_irqsave(&clkmgr_lock, flags);
		ret = disable_pll_locked(id, mod_name);
		spin_unlock_irqrestore(&clkmgr_lock, flags);

    return ret; 
}
EXPORT_SYMBOL(disable_pll);


unsigned int get_pll_value(enum mt65xx_pll_id id)
{
#ifdef BUG_ON_DEBUG
    BUG_ON(get_pll_state(id, LOCAL_STATE) != get_pll_state(id, REG_STATE));
#endif

    if (get_pll_state(id, LOCAL_STATE) == STATE_OFF) {
        return -1;
    }

    return ((DRV_Reg16(mt65xx_pll_regs[id]) << 16) | DRV_Reg16(mt65xx_pll_regs[id] + 4));
}
EXPORT_SYMBOL(get_pll_value);


#define FIXED_FREQ_PLLS \
    ((0x1 << MT65XX_UPLL) | (0x1 << MT65XX_MDPLL) | (0x1 << MT65XX_WPLL))

int pll_fsel(enum mt65xx_pll_id id, unsigned int pll_value)
{
    unsigned long flags;
    int old_state;
    int new_state;
    unsigned int hi_val = (pll_value & 0xFFFF);
    unsigned int lo_val = ((pll_value >> 16) & 0xFFFF);

#ifdef OP_MODE_DEBUG
    if (!op_mode) {
        return 0;
    }
#endif

    if ((0x1 << id) & FIXED_FREQ_PLLS) {
        clk_warn("[%s]: %s center frequence fixed\n", 
                __func__, clk_mgr.pll[id].name);
        return 1;
    }

    spin_lock_irqsave(&clkmgr_lock, flags);

#ifdef BUG_ON_DEBUG
    BUG_ON(get_pll_state(id, LOCAL_STATE) != get_pll_state(id, REG_STATE));
#endif

    old_state = new_state = get_pll_state(id, LOCAL_STATE);  

    if (id == MT65XX_AUDPLL && old_state == STATE_ON) {
        disable_pll_op(id);
        new_state = STATE_OFF;
    }

    if (id == MT65XX_TVDDS) {
        DRV_WriteReg16(mt65xx_pll_regs[id], hi_val); 
        DRV_WriteReg16(mt65xx_pll_regs[id] + 4, lo_val); 
    } else {
        /* we only adjust FBDIV and PREDIV */
        DRV_WriteReg16(mt65xx_pll_regs[id], 
            (DRV_Reg16(mt65xx_pll_regs[id]) & 0x3F) | (hi_val & 0x7FC0)); 
    }

    if (id == MT65XX_AUDPLL && old_state == STATE_ON && new_state == STATE_OFF) {
        enable_pll_op(id);
    } else if (old_state == STATE_ON) {
        if (id == MT65XX_TVDDS) {
            DRV_SetReg16(mt65xx_pll_regs[id] + 4 * 2, TVDDS_STATE_MASK); 
            udelay(mt65xx_pll_locktime[id]); 
        } else {
            udelay(PLL_LOCK_TIME_WITH_FREQ_HOPPING);
        }
    }

    spin_unlock_irqrestore(&clkmgr_lock, flags);
    return 0;
}
EXPORT_SYMBOL(pll_fsel);

/* freq_meter is used only for debug */
unsigned int freq_meter(int clk_sel, unsigned int delay)
{
    unsigned int data = 0;
    unsigned int freq = 0;

    DRV_WriteReg16(AP_ABIST_FQMTR_CON1, (clk_sel & 0x3F));
    /* enable freq meter and reset the circuit */
    DRV_WriteReg16(AP_ABIST_FQMTR_CON0, 0xC800);
    /* set the circuit in normal operation */
    DRV_WriteReg16(AP_ABIST_FQMTR_CON0, 0x8800);

    if (unlikely(delay)) {
        mdelay(delay);
    } else {
        while (DRV_Reg16(AP_ABIST_FQMTR_CON1) & 0x8000);
    }

    data = DRV_Reg16(AP_ABIST_FQMTR_DATA);
    freq = ((data + 1) * 26 / 2048);
    
    clk_info("[%s] data = %d, freq = %d MHz\n", __func__, data, freq);    

    return freq;
}
EXPORT_SYMBOL(freq_meter);


static void enable_mmsys_clksrc_op(void)
{
    if (unlikely(get_chip_eco_ver() == CHIP_E1)) {
        rtc_enable_abb_32k();
        DRV_WriteReg16(AUDPLL_CON0, 0x7200);
        DRV_WriteReg16(AUDPLL_CON0, 0x7201);
        mdelay(10);
    } else {
        DRV_WriteReg16(MDPLL_CON0, 0x1310);
        udelay(20);
        DRV_SetReg16(TOP_CKMUXSEL2, 0xC000);
    }
}

static void disable_mmsys_clksrc_op(void)
{
    if (unlikely(get_chip_eco_ver() == CHIP_E1)) {
        DRV_WriteReg16(AUDPLL_CON0, 0x7200);
        rtc_disable_abb_32k();
    } else {
        DRV_ClrReg16(TOP_CKMUXSEL2, 0x8000);
        DRV_WriteReg16(MDPLL_CON0, 0x1311);
    }
}

/*
 * w/o spin_lock version, be called by holding spin_lock
 */
static int __request_mmsys_clksrc(int opcode, int is_mfg)
{
	if ((g_chip12 == 1) && is_mfg){
		if (unlikely(clk_mgr.mmsys_state_mfg == !!opcode)) {
			clk_info("[%s]: is_mfg = %d, opcode = %d, mmsys_state = %d, return directly\n",
					__func__, is_mfg, opcode, clk_mgr.mmsys_state);
			return 0;
		}

		/* opcode: non-zero, request mmsys clksrc */
		if (opcode) {
			//enable_mmsys_clksrc_op();
			enable_wpll_clksrc_op(WPLL_281M_EN);
			clk_mgr.mmsys_state_mfg = 1;
			/* opcode: zero, release mmsys clksrc */
		} else {
			/*if (!(clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][MT65XX_PDN_MM_MFG % CLK_REG_WIDTH].count == 0 && 
					clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][MT65XX_PDN_MM_G3D % CLK_REG_WIDTH].count == 0 && 
					clk_mgr.clock[MT65XX_CLOCK_MMSYS2_PDN][MT65XX_PDN_MM_MFG_HALF % CLK_REG_WIDTH].count == 0)) {
				clk_warn("[%s]: mfg mmsys clksrc is still used, so skip\n", __func__);
				return 1;
			}*/
			//disable_mmsys_clksrc_op();
			disable_wpll_clksrc_op(WPLL_281M_EN);
			clk_mgr.mmsys_state_mfg = 0;
		}
	} else {
		if (unlikely(clk_mgr.mmsys_state == !!opcode)) {
			clk_info("[%s]: opcode = %d, mmsys_state = %d, return directly\n",
					__func__, opcode, clk_mgr.mmsys_state);
			return 0;
		}

		/* opcode: non-zero, request mmsys clksrc */
		if (opcode) {
			enable_mmsys_clksrc_op();
			clk_mgr.mmsys_state = 1;
			/* opcode: zero, release mmsys clksrc */
		} else {
			if (!could_power_off_mmsys_clksrc()) {
				clk_warn("[%s]: mmsys clksrc is still used, so skip\n", __func__);
				return 1;
			}
			disable_mmsys_clksrc_op();
			clk_mgr.mmsys_state = 0;
		} 
	}

    return 0;
}

int request_mmsys_clksrc(int opcode, int is_mfg) {
    unsigned long flags;
    int ret;

    if (!initialized) {
        return 1;
    }
    
    spin_lock_irqsave(&clkmgr_lock, flags);
    ret = __request_mmsys_clksrc(opcode, is_mfg);
    spin_unlock_irqrestore(&clkmgr_lock, flags);

    return ret;
}


static void switch_mmsys_clksrc(void) 
{
    if (unlikely(get_chip_eco_ver() == CHIP_E1)) {
        DRV_ClrReg32(TOP_CKMUXSEL, 0x30);   /* switch MMSYS to 26MHz first */ 
        DRV_ClrReg32(TOP_CKDIV23, 0x1);     /* disable clock divider 2 */
        __request_mmsys_clksrc(1,0);
        DRV_SetReg32(TOP_CKMUXSEL, 0x30);   /* switch MMSYS to AUDPLL */
    } else {
        clk_mgr.mmsys_state = 1;
    }

    if (clkmgr_debug_mask & DEBUG_INIT) {
        freq_meter(0x11, 0);
        freq_meter(0x20, 0);
        clk_info("[%s]: TOP_CKMUXSEL  = 0x%x\n", __func__, DRV_Reg32(TOP_CKMUXSEL));
        clk_info("[%s]: TOP_CKDIV23   = 0x%x\n", __func__, DRV_Reg32(TOP_CKDIV23));
        clk_info("[%s]: TOP_CKMUXSEL2 = 0x%x\n", __func__, DRV_Reg16(TOP_CKMUXSEL2));
    }
}

enum mtcmos_offset {
    PWR_RST_B = 0,
    PWR_ISO,
    PWR_ON,
    PWR_MEM_OFF,
    PWR_CLK_DIS,
    PWR_MEM_SLPB,
    PWR_REQ_EN,
    PWR_CTRL,
};

static void enable_subsys_op(enum mt65xx_subsys_id id)
{
    volatile unsigned int reg_val = 0; 

    /* power controlled by SW */
    DRV_SetReg32(mt65xx_subsys_regs[id], (0x1 << PWR_CTRL));

    /* pull up PWR_ON and pull down PWR_MEM_OFF */
    reg_val = DRV_Reg32(mt65xx_subsys_regs[id]);
    reg_val = (reg_val & ~(0x1 << PWR_MEM_OFF)) | (0x1 << PWR_ON);
    DRV_WriteReg32(mt65xx_subsys_regs[id], reg_val);

    /* pull down PWR_CLK_DIS */
    DRV_ClrReg32(mt65xx_subsys_regs[id], (0x1 << PWR_CLK_DIS));

    /* pull down PWR_ISO */
    DRV_ClrReg32(mt65xx_subsys_regs[id], (0x1 << PWR_ISO));

    /* pull up PWR_RST_B */
    DRV_SetReg32(mt65xx_subsys_regs[id], (0x1 << PWR_RST_B));

    /* wait for stable and PWR_STA update */
#if 0
    udelay(5);
#else
    while (!(DRV_Reg32(SC_PWR_STA) & (0x1 << id))) {
    }
#endif
}


static void disable_subsys_op(enum mt65xx_subsys_id id)
{
    volatile unsigned int reg_val = 0; 

    /* Power Controlled by SW */
    DRV_SetReg32(mt65xx_subsys_regs[id], (0x1 << PWR_CTRL));

    /* pull up PWR_ISO */
    DRV_SetReg32(mt65xx_subsys_regs[id], (0x1 << PWR_ISO));

    if (id != MT65XX_SUBSYS_CPU && id != MT65XX_SUBSYS_CPUSYS) {
        /* pull down PWR_RST_B, pull up PWR_MEM_OFF and PWR_CLK_DIS */
        reg_val = DRV_Reg32(mt65xx_subsys_regs[id]);
        reg_val = (reg_val & ~(0x1 << PWR_RST_B)) | (0x1 << PWR_MEM_OFF) | (0x1 << PWR_CLK_DIS);
        DRV_WriteReg32(mt65xx_subsys_regs[id], reg_val);
    } else {
        /* pull up PWR_MEM_OFF */
        DRV_SetReg32(mt65xx_subsys_regs[id], (0x1 << PWR_MEM_OFF));

        /* pull down PWR_RST_B and pull up PWR_CLK_DIS */
        reg_val = DRV_Reg32(mt65xx_subsys_regs[id]);
        reg_val = (reg_val & ~(0x1 << PWR_RST_B)) | (0x1 << PWR_CLK_DIS);
        DRV_WriteReg32(mt65xx_subsys_regs[id], reg_val);
    } 

    /* pull down PWR_ON */
    DRV_ClrReg32(mt65xx_subsys_regs[id], (0x1 << PWR_ON));

    /* wait for PWR_STA update */
#if 0
    udelay(5);
#else
    while ((DRV_Reg32(SC_PWR_STA) & (0x1 << id))) {
    }
#endif
}


static int enable_subsys_locked(enum mt65xx_subsys_id id, char *mod_name)
{
#ifdef BUG_ON_DEBUG
    BUG_ON(get_subsys_state(id, LOCAL_STATE) != get_subsys_state(id, REG_STATE));
#endif

    if (get_subsys_state(id, LOCAL_STATE) == STATE_ON) {
        if (clkmgr_debug_mask & DEBUG_SUBSYS) {
            clk_info("[%s]: %s is already ON now\n", 
                    __func__, clk_mgr.subsys[id].name);
        }
        return 0;
    }

    mm_power_prepare(0, "subsys", id);
  
    enable_subsys_op(id);

    if (get_subsys_state(id, REG_STATE) == STATE_OFF) {
        clk_err("[%s]: enable %s fail\n", 
                __func__, clk_mgr.subsys[id].name);
        return 1;
    }

    clk_mgr.subsys_state |= 0x1 << id;

    if (id == MT65XX_SUBSYS_MM1) {
        clk_info("[%s]: gate all MM1 clocks after power on MMSYS1\n", __func__);
        DRV_WriteReg32(MMSYS1_CG_SET0, 0xFFFFFFFF);
        DRV_WriteReg32(MMSYS1_CG_SET1, 0xFFFFFFFF);
        DRV_WriteReg32(MMSYS1_CG_SET2, 0xFFFFFFFF);

        /* workaround E1 for M4U receive false interrupt if MMSYS1 is power off */
        if (unlikely(get_chip_eco_ver() == CHIP_E1)) {
            clk_info("[%s]: unmask M4U irq after power on MMSYS1\n", __func__);
            mt6577_irq_set_polarity(MT6577_SMI_LARB0_MMU_IRQ_ID, MT65xx_POLARITY_HIGH);
            mt6577_irq_set_polarity(MT6577_SMI_LARB1_MMU_IRQ_ID, MT65xx_POLARITY_HIGH);
            mt6577_irq_set_polarity(MT6577_SMI_LARB2_MMU_IRQ_ID, MT65xx_POLARITY_HIGH);
            enable_irq(MT6577_SMI_LARB0_MMU_IRQ_ID);    
            enable_irq(MT6577_SMI_LARB1_MMU_IRQ_ID);    
            enable_irq(MT6577_SMI_LARB2_MMU_IRQ_ID);
        }
    }
	return 0;
}


int enable_subsys(enum mt65xx_subsys_id id, char *mod_name)
{
    unsigned long flags;
    int ret;

#ifdef OP_MODE_DEBUG
    if (!op_mode) {
        return 0;
    }
#endif

    BUG_ON(!mod_name);

    if (!initialized) {
        clk_warn("[%s]: clkmgr not initialized yet! [id=%d] [mod=%s]\n",
                 __func__, id, mod_name);
        return 1;
    }

#ifdef BUG_ON_DEBUG
    BUG_ON(INVALID_SUBSYS_ID(id));
#else
    if (INVALID_SUBSYS_ID(id)) {
        clk_warn("[%s]: %s pass an invalid id [%d]\n", 
                __func__, mod_name, id);
        return 1;
    }
#endif

    if (clkmgr_debug_mask & DEBUG_SUBSYS) {
        clk_info("[%s]: [id=%d] [name=%s] [mod=%s]\n", 
                __func__, id, clk_mgr.subsys[id].name, mod_name);
    }

    spin_lock_irqsave(&clkmgr_lock, flags);
    ret = enable_subsys_locked(id, mod_name);
    spin_unlock_irqrestore(&clkmgr_lock, flags);
    return ret; 
}
EXPORT_SYMBOL(enable_subsys);


static int disable_subsys_locked(enum mt65xx_subsys_id id, char *mod_name)
{
    if (!could_power_off_subsys(id) && strcmp(mod_name, "subsys_test")) {
        clk_warn("[%s]: %s is still used, so skip\n", 
                __func__, clk_mgr.subsys[id].name);
        return 1;
    }

#ifdef BUG_ON_DEBUG
    BUG_ON(get_subsys_state(id, LOCAL_STATE) != get_subsys_state(id, REG_STATE));
#endif

    if (get_subsys_state(id, LOCAL_STATE) == STATE_OFF) {
        if (clkmgr_debug_mask & DEBUG_SUBSYS) {
            clk_info("[%s]: %s is already OFF now\n", 
                    __func__, clk_mgr.subsys[id].name);
        }
        return 0;
    }
    
    if (id == MT65XX_SUBSYS_MM1) {
        clk_info("[%s]: backup larb0/1/2 if needed\n", __func__);
        mm1_backing_up = 1;
        larb_backup(MT65XX_LARB0);
        larb_backup(MT65XX_LARB1);
        larb_backup(MT65XX_LARB2);
        mm1_backing_up = 0;

        /* workaround for M4U receive false interrupt if MMSYS1 is power off */
        if (unlikely(get_chip_eco_ver() == CHIP_E1)) {
            clk_info("[%s]: mask M4U irq before power off MMSYS1\n", __func__);
            disable_irq(MT6577_SMI_LARB0_MMU_IRQ_ID);    
            disable_irq(MT6577_SMI_LARB1_MMU_IRQ_ID);    
            disable_irq(MT6577_SMI_LARB2_MMU_IRQ_ID);    
            mt6577_irq_set_polarity(MT6577_SMI_LARB0_MMU_IRQ_ID, MT65xx_POLARITY_LOW);
            mt6577_irq_set_polarity(MT6577_SMI_LARB1_MMU_IRQ_ID, MT65xx_POLARITY_LOW);
            mt6577_irq_set_polarity(MT6577_SMI_LARB2_MMU_IRQ_ID, MT65xx_POLARITY_LOW);
        }
#if 0
    } else if (id == MT65XX_SUBSYS_MM2) {
        clk_info("[%s]: backup larb3 if needed\n", __func__);
        mm2_backing_up = 1;
        larb_backup(MT65XX_LARB3);
        mm2_backing_up = 0;
#endif
    }

    disable_subsys_op(id);

    if (get_subsys_state(id, REG_STATE) == STATE_ON) {
        clk_err("[%s]: disable %s fail\n", 
                __func__, clk_mgr.subsys[id].name);
        return 1;
    }
    
    clk_mgr.subsys_state &= ~(0x1 << id);

    mm_power_finish(0, "subsys", id);

	return 0;
}



int disable_subsys(enum mt65xx_subsys_id id, char *mod_name)
{
    unsigned long flags;
    int ret;

#ifdef OP_MODE_DEBUG
    if (!op_mode) {
        return 0;
    }
#endif

    BUG_ON(!mod_name);

    if (!initialized) {
        clk_warn("[%s]: clkmgr not initialized yet! [id=%d] [mod=%s]\n",
                 __func__, id, mod_name);
        return 1;
    }

#ifdef BUG_ON_DEBUG
    BUG_ON(INVALID_SUBSYS_ID(id));
#else
    if (INVALID_SUBSYS_ID(id)) {
        clk_warn("[%s]: %s pass an invalid id [%d]\n", 
                __func__, mod_name, id);
        return 1;
    }
#endif

    if (clkmgr_debug_mask & DEBUG_SUBSYS) {
        clk_info("[%s]: [id=%d] [name=%s] [mod=%s]\n", 
                __func__, id, clk_mgr.subsys[id].name, mod_name);
    }

    spin_lock_irqsave(&clkmgr_lock, flags);
    ret = disable_subsys_locked(id, mod_name);
    spin_unlock_irqrestore(&clkmgr_lock, flags);
    return ret; 
}
EXPORT_SYMBOL(disable_subsys);


void set_partial_log_mask(int category, unsigned int mask)
{
    BUG_ON(!initialized);
    BUG_ON(INVALID_CATEGORY_ID(category));
    //BUG_ON(INVALID_CLOCK_ID(category, mask));

    partial_log_mask[category] |= mask;
}

void clr_partial_log_mask(int category, unsigned int mask)
{
    BUG_ON(!initialized);
    BUG_ON(INVALID_CATEGORY_ID(category));
    //BUG_ON(INVALID_CLOCK_ID(category, mask));

    partial_log_mask[category] &= ~mask;
}

void set_partial_log_bit(enum mt65xx_clock_id id)
{
    int category = id / CLK_REG_WIDTH;
    int offset = id % CLK_REG_WIDTH;

    set_partial_log_mask(category, 0x1 << offset);
}

void clr_partial_log_bit(enum mt65xx_clock_id id)
{
    int category = id / CLK_REG_WIDTH;
    int offset = id % CLK_REG_WIDTH;

    clr_partial_log_mask(category, 0x1 << offset);
}

#ifdef FORCE_UNGATE_DEBUG
void set_force_ungate_mask(int category, unsigned int mask)
{
    BUG_ON(!initialized);
    BUG_ON(INVALID_CATEGORY_ID(category));
    //BUG_ON(INVALID_CLOCK_ID(category, mask));

    force_ungate_mask[category] |= mask;
}

void clr_force_ungate_mask(int category, unsigned int mask)
{
    BUG_ON(!initialized);
    BUG_ON(INVALID_CATEGORY_ID(category));
    //BUG_ON(INVALID_CLOCK_ID(category, mask));

    force_ungate_mask[category] &= ~mask;
}

void set_force_ungate_bit(enum mt65xx_clock_id id)
{
    int category = id / CLK_REG_WIDTH;
    int offset = id % CLK_REG_WIDTH;

    set_force_ungate_mask(category, 0x1 << offset);
}

void clr_force_ungate_bit(enum mt65xx_clock_id id)
{
    int category = id / CLK_REG_WIDTH;
    int offset = id % CLK_REG_WIDTH;

    clr_force_ungate_mask(category, 0x1 << offset);
}
#endif


#define INITIAL_PERI0_STATE     (0xF0F857FF)    /* UART0~3, MSDC0~3, GPT, CCCI */
#define INITIAL_PERI1_STATE     (0xFFFFFFFF)    
#define INITIAL_MM1_0_STATE     (0xFFFFFE07)    /* LARB0/1/2 */ 
#define INITIAL_MM1_1_STATE     (0xFFFFFFDF)    /* LCD */
#define INITIAL_MM1_2_STATE     (0xFFFFFC0F)    /* LARB0/1/2, LCD_EMI, DSI, DPI */
#define INITIAL_MM2_STATE       (0xFFF9FFFE)    /* LARB3, Audio */
#define INITIAL_AUDIO_STATE     (0x00010064)


static void mt6577_cg_init(void)
{
    volatile unsigned int reg_val = 0;

    if (clkmgr_debug_mask & DEBUG_INIT) {
        clk_info("[%s]: <== reset clock gating bit\n", __func__);
        dump_clk_regs();
    }
    reg_val = DRV_Reg32(PERI_GLOBALCON_PDN0);
    reg_val = (reg_val & 0x7C) | (INITIAL_PERI0_STATE & 0xFFFFFF83);
    DRV_WriteReg32(PERI_GLOBALCON_PDN0, reg_val);
    DRV_WriteReg32(PERI_GLOBALCON_PDN1, INITIAL_PERI1_STATE);
    DRV_WriteReg32(MMSYS1_CG_SET0, INITIAL_MM1_0_STATE);
    DRV_WriteReg32(MMSYS1_CG_SET1, INITIAL_MM1_1_STATE);
    DRV_WriteReg32(MMSYS1_CG_SET2, INITIAL_MM1_2_STATE);
    DRV_WriteReg32(MMSYS2_CG_SET, INITIAL_MM2_STATE);
    DRV_WriteReg32(AUDIO_TOP_CON0, INITIAL_AUDIO_STATE);

    if (clkmgr_debug_mask & DEBUG_INIT) {
        clk_info("[%s]: ==> reset clock gating bit\n", __func__);
        dump_clk_regs();
    }
}


static void mt6577_clock_init(void)
{
    int i = 0, j = 0;

    mt6577_cg_init();

    /* initialize clock info */
    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {
        /* in register,     0: STATE_ON,  1: STATE_OFF
         * in clock_state,  0: STATE_OFF, 1: STATE_ON
         */
        clk_mgr.clock_state[i] = (~DRV_Reg32(mt65xx_cg_con_regs[i])) & valid_clock_mask[i]; 
        if (clkmgr_debug_mask & DEBUG_INIT) {
            clk_info("[%s]: clock_state[%d] = 0x%08x", 
                    __func__, i, clk_mgr.clock_state[i]);
        }

        for (j = 0; j < mt65xx_clock_number_per_category[i]; j++) {
            clk_mgr.clock[i][j].count = 0;
            clk_mgr.clock[i][j].init_state = 
                    (clk_mgr.clock_state[i] & (0x1 << j)) ? STATE_ON : STATE_OFF;
            sprintf(clk_mgr.clock[i][j].name, "%s", mt65xx_clock_name_idx[i] + j * MAX_NAME_LEN);

            if (clkmgr_debug_mask & DEBUG_INIT) {
                clk_info("[%s]: clock[%d][%d].name = %s, init_state = %s\n", 
                        __func__, i, j, clk_mgr.clock[i][j].name, 
                        clk_mgr.clock[i][j].init_state == STATE_ON ? "on" : "off");
            }

#ifdef CLK_MGR_DEBUG
            INIT_LIST_HEAD(&clk_mgr.clock[i][j].list);
#endif
        }
    } 
}

static void mt6577_pll_init(void)
{
    int i = 0;

    /* initialize pll info */
    if (clkmgr_debug_mask & DEBUG_INIT) {
        dump_pll_regs();
    }

    /* Switch MMSYS clock source to AUDPLL(E1) / MDPLL(E2) */
    switch_mmsys_clksrc();

    clk_mgr.pll_state = 0;  
    for (i = 0; i < MT65XX_PLL_COUNT; i++) {
        clk_mgr.pll[i].count = 0;
        clk_mgr.pll[i].init_state = get_pll_state(i, REG_STATE);
        
        if (clk_mgr.pll[i].init_state == STATE_ON) {        
            clk_mgr.pll_state |= 0x1 << i;
        }

        sprintf(clk_mgr.pll[i].name, "%s", mt65xx_pll_name[i]);

        if (clkmgr_debug_mask & DEBUG_INIT) {
            clk_info("[%s]: pll[%d].name = %s, init_state = %s\n", 
                    __func__, i, clk_mgr.pll[i].name, 
                    clk_mgr.pll[i].init_state == STATE_ON ? "on" :"off");
        }

#ifdef CLK_MGR_DEBUG
        INIT_LIST_HEAD(&clk_mgr.pll[i].list);
#endif
    } 
}

static void mt6577_subsys_init(void)
{
    int i = 0;
    /* initialize subsys info */
    clk_mgr.subsys_state = DRV_Reg32(SC_PWR_STA);  
    for (i = 0; i < MT65XX_SUBSYS_COUNT; i++) {
        clk_mgr.subsys[i].init_state = 
                (clk_mgr.subsys_state & (0x1 << i)) ? STATE_ON : STATE_OFF;
        sprintf(clk_mgr.subsys[i].name, "%s", mt65xx_subsys_name[i]);

        if (clkmgr_debug_mask & DEBUG_INIT) {
            clk_info("[%s]: subsys[%d].name = %s, init_state = %s\n", 
                    __func__, i, clk_mgr.subsys[i].name,
                    clk_mgr.subsys[i].init_state == STATE_ON ? "on" : "off");
        }
    } 
}

static void mt6577_mipi_init(void)
{
    int i = 0;

    if (clkmgr_debug_mask & DEBUG_INIT) {
        dump_mipi_regs();
    }

    clk_mgr.mipi_state = 0;  
    for (i = MT65XX_MIPI_TX; i < MT65XX_MIPI_COUNT; i++) {
        mipi[i].count = 0;
        sprintf(mipi[i].name, "%s", mt65xx_mipi_name[i]);

#ifdef CLK_MGR_DEBUG
        INIT_LIST_HEAD(&mipi[i].list);
#endif

    }
}


void mt6577_clk_mgr_init(void)
{
    if (initialized) {
        clk_info("[%s]: clock manager initialized already!\n", __func__);
        return;
    }
    
    mt6577_clock_init();

    mt6577_pll_init();

    clksrc_selection_init();

    mt6577_subsys_init();

    mt6577_mipi_init();

    /* initialize partial log and force ungate mask array */
    memset(partial_log_mask, 0, sizeof(unsigned int) * MT65XX_CLOCK_CATEGORY_COUNT);
#ifdef FORCE_UNGATE_DEBUG
    memset(force_ungate_mask, 0, sizeof(unsigned int) * MT65XX_CLOCK_CATEGORY_COUNT);
#endif

    /* clk mgr initialization done */
    initialized = 1;	
    clk_info("[%s]: clock manager initialization success!\n", __func__);

    /* 
     * Workaround for MT6575 E1 Issue
     * 1. UART1-4       id = 24 ~ 27 
     */
    if (unlikely(get_chip_eco_ver() == CHIP_E1)) {
        set_force_ungate_mask(MT65XX_CLOCK_PERI_PDN0, 0x0F000000);
		}

		if ((DRV_Reg32(HW_RESV) & (0x1 << 23)) && ((DRV_Reg32(HW_RESV) & (0x1 << 20)) == 0)){
			g_chip12 = 1;
		}
}

int get_clkmgr_init_state(void)
{
    return initialized;
}

#if 1
/* For Compatibility Only */
kal_bool hwEnableClock(MT65XX_CLOCK clockId, char *mod_name)
{
    if (!enable_clock(clockId, mod_name))
        return TRUE;
    else 
        return FALSE;
}
EXPORT_SYMBOL(hwEnableClock);

kal_bool hwDisableClock(MT65XX_CLOCK clockId, char *mod_name)
{
    if (!disable_clock(clockId, mod_name))
        return TRUE;
    else 
        return FALSE;
}
EXPORT_SYMBOL(hwDisableClock);
#endif
