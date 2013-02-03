#include <linux/proc_fs.h>
#include <linux/list.h>
#include <asm/uaccess.h>

#include "mach/mt6577_clock_manager.h"
#include "mach/mt6577_clkmgr_internal.h"

#define USING_XLOG


#ifdef USING_XLOG 

#include <linux/xlog.h>
#define TAG     "Power/pmlog"

#define pmlog_err(fmt, args...)     \
    xlog_printk(ANDROID_LOG_ERROR, TAG, fmt, ##args)
#define pmlog_warn(fmt, args...)    \
    xlog_printk(ANDROID_LOG_WARN, TAG, fmt, ##args)
#define pmlog_info(fmt, args...)    \
    xlog_printk(ANDROID_LOG_INFO, TAG, fmt, ##args)
#define pmlog_dbg(fmt, args...)     \
    xlog_printk(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define pmlog_ver(fmt, args...)     \
    xlog_printk(ANDROID_LOG_VERBOSE, TAG, fmt, ##args)

#else

#define TAG     "[Power/pmlog] "

#define pmlog_err(fmt, args...)     \
    printk(KERN_ERR TAG);           \
    printk(KERN_CONT fmt, ##args) 
#define pmlog_warn(fmt, args...)    \
    printk(KERN_WARNING TAG);       \
    printk(KERN_CONT fmt, ##args)
#define pmlog_info(fmt, args...)    \
    printk(KERN_NOTICE TAG);        \
    printk(KERN_CONT fmt, ##args)
#define pmlog_dbg(fmt, args...)     \
    printk(KERN_INFO TAG);          \
    printk(KERN_CONT fmt, ##args)
#define pmlog_ver(fmt, args...)     \
    printk(KERN_DEBUG TAG);         \
    printk(KERN_CONT fmt, ##args)

#endif


extern int clkmgr_debug_mask;
#if defined(CONFIG_WAKELOCK) && defined(CONFIG_USER_WAKELOCK) && defined(CONFIG_EARLYSUSPEND) 
extern int wakelock_debug_mask;
extern int Userwakelock_debug_mask;
extern int Earlysuspend_debug_mask;
extern int early_suspend_count; 
extern int forbid_id;
#endif
extern int console_suspend_enabled;

#ifdef OP_MODE_DEBUG
extern int op_mode; 
#endif


/*
 * pm_flag_read  is used to show power management debug level
 * pm_flag_write is used to set  power management debug level
 * a. clkmgr
 * b. wakelock
 * c. Userwakelock
 * d. Earlysuspend
 * f. console_suspend or not
 */

static int pm_flag_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;

    p += sprintf(p, "Mediatek Power Management Debug Mask\n" );
    p += sprintf(p, "=========================================\n" );
    p += sprintf(p, "clkmgr_debug_mask       = 0x%x\n", clkmgr_debug_mask);
#if defined(CONFIG_WAKELOCK) && defined(CONFIG_USER_WAKELOCK) && defined(CONFIG_EARLYSUSPEND) 
    p += sprintf(p, "wakelock_debug_mask     = 0x%x\n", wakelock_debug_mask);
    p += sprintf(p, "Userwakelock_debug_mask = 0x%x\n", Userwakelock_debug_mask);
    p += sprintf(p, "Earlysuspend_debug_mask = 0x%x\n", Earlysuspend_debug_mask);
#endif
    p += sprintf(p, "console_suspend_enabled = 0x%x\n", console_suspend_enabled);	

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int pm_flag_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    int new_clkmgr_debug_mask = 0;
#if defined(CONFIG_WAKELOCK) && defined(CONFIG_USER_WAKELOCK) && defined(CONFIG_EARLYSUSPEND) 
    int new_wakelock_debug_mask = 0;
    int new_userwakelock_debug_mask = 0;
    int new_earlysuspend_debug_mask = 0;
#endif
    int new_console_suspend_enabled = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

#if defined(CONFIG_WAKELOCK) && defined(CONFIG_USER_WAKELOCK) && defined(CONFIG_EARLYSUSPEND) 
    if (sscanf(desc, "%x %x %x %x %x", &new_clkmgr_debug_mask, &new_wakelock_debug_mask, 
        &new_userwakelock_debug_mask, &new_earlysuspend_debug_mask, &new_console_suspend_enabled) == 5) { 
#else
    if (sscanf(desc, "%x %x", &new_clkmgr_debug_mask, &new_console_suspend_enabled) == 2) { 
#endif
        clkmgr_debug_mask = new_clkmgr_debug_mask;
#if defined(CONFIG_WAKELOCK) && defined(CONFIG_USER_WAKELOCK) && defined(CONFIG_EARLYSUSPEND) 
        wakelock_debug_mask = new_wakelock_debug_mask;
        Userwakelock_debug_mask = new_userwakelock_debug_mask;
        Earlysuspend_debug_mask = new_earlysuspend_debug_mask;
#endif
        console_suspend_enabled = new_console_suspend_enabled;
    }
    return count;
}

#if defined(CONFIG_WAKELOCK) && defined(CONFIG_USER_WAKELOCK) && defined(CONFIG_EARLYSUSPEND) 
static int early_suspend_ctrl_read(char *page, char **start, off_t off,
                               int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;

    p += sprintf(p, "Early Suspend Control Debug\n");
    p += sprintf(p, "=========================================\n");
    p += sprintf(p, "forbid_id = 0x%x\n", forbid_id);
    p += sprintf(p, "early_suspend_count = %d\n", early_suspend_count);

    p += sprintf(p, "\n********** Command Help *********\n");
    p += sprintf(p, "Disable 5st early_suspend callback: echo 10(decimal) > /proc/pm_early_suspend_control\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}


static int early_suspend_ctrl_write(struct file *file, const char *buffer,
			unsigned long count, void *data)
{
    char desc[32];
    int len = 0;
    int mask = 0;

    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len))
		return 0;
    desc[len] = '\0';

    if (sscanf(desc, "%x", &mask) == 1) {
		forbid_id = mask;
		pmlog_ver("============PM early suspend control========\n");
        pmlog_ver("     forbid_id=[0x%x]\n", forbid_id);       
		pmlog_ver("============================================\n");
    }
    return count;
}
#endif

/*
 * pm_reg_dump_read  is used to show register setting for low power checking
 */

static int pm_reg_dump_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;

    p += sprintf(p, "Mediatek Power Management Register Dump\n" );
    
    p += sprintf(p, "=========================================\n");
    p += sprintf(p, "=========================================\n");
    
    p += sprintf(p, "\nUL/DL\n");
    p += sprintf(p, "===============================\n");
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0A00), DRV_Reg32(0xFD114000 + 0x0A00));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0AB4), DRV_Reg32(0xFD114000 + 0x0AB4));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0100), DRV_Reg32(0xFD114000 + 0x0100));
    
    p += sprintf(p, "\nAudio AuEnc\n");
    p += sprintf(p, "===============================\n");
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0444), DRV_Reg32(0xFD114000 + 0x0444));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0448), DRV_Reg32(0xFD114000 + 0x0448));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC0007000 + 0x0020), DRV_Reg32(0xF0007000 + 0x0020));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x03AC), DRV_Reg32(0xFD114000 + 0x03AC));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0380), DRV_Reg32(0xFD114000 + 0x0380));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0384), DRV_Reg32(0xFD114000 + 0x0384));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0388), DRV_Reg32(0xFD114000 + 0x0388));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x038C), DRV_Reg32(0xFD114000 + 0x038C));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x03B8), DRV_Reg32(0xFD114000 + 0x03B8));
    
    p += sprintf(p, "\nAudio AuDec (Silent input)\n");
    p += sprintf(p, "===============================\n");
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0400), DRV_Reg32(0xFD114000 + 0x0400));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0404), DRV_Reg32(0xFD114000 + 0x0404));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0440), DRV_Reg32(0xFD114000 + 0x0440));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0444), DRV_Reg32(0xFD114000 + 0x0444));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0448), DRV_Reg32(0xFD114000 + 0x0448));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0480), DRV_Reg32(0xFD114000 + 0x0480));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0484), DRV_Reg32(0xFD114000 + 0x0484));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x04C4), DRV_Reg32(0xFD114000 + 0x04C4));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0300), DRV_Reg32(0xFD114000 + 0x0300));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0304), DRV_Reg32(0xFD114000 + 0x0304));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0308), DRV_Reg32(0xFD114000 + 0x0308));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x030C), DRV_Reg32(0xFD114000 + 0x030C));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0314), DRV_Reg32(0xFD114000 + 0x0314));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0318), DRV_Reg32(0xFD114000 + 0x0318));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x031C), DRV_Reg32(0xFD114000 + 0x031C));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0320), DRV_Reg32(0xFD114000 + 0x0320));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0324), DRV_Reg32(0xFD114000 + 0x0324));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0338), DRV_Reg32(0xFD114000 + 0x0338));
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0344), DRV_Reg32(0xFD114000 + 0x0344));
    
    p += sprintf(p, "\nPLL\n");
    p += sprintf(p, "===============================\n");
    p += sprintf(p, "0x%x = 0x%x\n", (0xC0007020), DRV_Reg32(0xF0007020));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC0007100), DRV_Reg32(0xF0007100));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC0007120), DRV_Reg32(0xF0007120));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC0007140), DRV_Reg32(0xF0007140));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC0007160), DRV_Reg32(0xF0007160));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC0007180), DRV_Reg32(0xF0007180));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC00071C0), DRV_Reg32(0xF00071C0));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC00071E0), DRV_Reg32(0xF00071E0));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC0007200), DRV_Reg32(0xF0007200));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC000704C), DRV_Reg32(0xF000704C));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC0007044), DRV_Reg32(0xF0007044));
    p += sprintf(p, "0x%x = 0x%x\n", (0xC000720C), DRV_Reg32(0xF000720C));
    
    p += sprintf(p, "\nSlowDAC\n");
    p += sprintf(p, "===============================\n");
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0100), DRV_Reg32(0xFD114000 + 0x0100));
    
    p += sprintf(p, "\nVDAC\n");
    p += sprintf(p, "===============================\n");
    p += sprintf(p, "0x%x = 0x%x\n", (0xC0007600), DRV_Reg32(0xF0007600));
    
    p += sprintf(p, "\nTSABB\n");
    p += sprintf(p, "===============================\n");
    p += sprintf(p, "0x%x = 0x%x\n", (0xC0007808), DRV_Reg32(0xF0007808));
    
    p += sprintf(p, "\nVA12\n");
    p += sprintf(p, "===============================\n");
    p += sprintf(p, "0x%x = 0x%x\n", (0x81140000 + 0x0448), DRV_Reg32(0xFD114000 + 0x0448));
    
    p += sprintf(p, "=================================================\n");
    p += sprintf(p, "=================================================\n");
    
    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

/*
 * dump_read is used to show which clock are on and its count
 *           on/off states is read from register
 */

static int dump_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    int i = 0, j = 0;
    unsigned int reg_val = 0;

    p += sprintf(p, "********** Clock Infomation Dump *********\n");
    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {
        p += sprintf(p, "\n***** %s *****\n", mt65xx_category_name[i]);
        reg_val = (~DRV_Reg32(mt65xx_cg_con_regs[i])) & valid_clock_mask[i]; 

        for (j = 0; j < mt65xx_clock_number_per_category[i]; j++) {
            if ((reg_val >> j) & 0x1) {
                p += sprintf(p, "clock[%3d](%d,%2d): %s is on, count = %d\n", 
                        (i*32+j), i, j, clk_mgr.clock[i][j].name, clk_mgr.clock[i][j].count);
            }
        }
    }

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

/*
 * detail_read is used to show clock detailed info, 
 *          including id/name/default on or not/count/module_info
 * detail_write is used to set which clock detailed info will be shown.
 *          no clock bits will be shown for default settings
 */

static unsigned int show_detail_mask[MT65XX_CLOCK_CATEGORY_COUNT] = {
    0x00000000,     /* MT65XX_CLOCK_PERI_PDN0 */
    0x00000000,     /* MT65XX_CLOCK_PERI_PDN1 */
    0x000001F8,     /* MT65XX_CLOCK_MMSYS1_PDN0 */
    0x00000000,     /* MT65XX_CLOCK_MMSYS1_PDN1 */
    0x000001C0,     /* MT65XX_CLOCK_MMSYS1_PDN2 */
    0x00020001,     /* MT65XX_CLOCK_MMSYS2_PDN */
    0x00000000,     /* MT65XX_CLOCK_AUDIO_PDN */
};

static void set_show_detail_mask(int category, unsigned int mask)
{
    BUG_ON(INVALID_CATEGORY_ID(category));
    //BUG_ON(INVALID_CLOCK_ID(category, mask));

    show_detail_mask[category] |= mask;
}

static void clr_show_detail_mask(int category, unsigned int mask)
{
    BUG_ON(INVALID_CATEGORY_ID(category));
    //BUG_ON(INVALID_CLOCK_ID(category, mask));

    show_detail_mask[category] &= ~mask;
}

static void set_show_detail_bit(enum mt65xx_clock_id id)
{
    int category = id / 32;
    int offset = id % 32;

    set_show_detail_mask(category, 0x1 << offset);
}

static void clr_show_detail_bit(enum mt65xx_clock_id id)
{
    int category = id / 32;
    int offset = id % 32;

    clr_show_detail_mask(category, 0x1 << offset);
}

static int detail_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    int i = 0, j = 0;
    struct list_head *pos;
    struct module_info *info;

    p += sprintf(p, "********** Detail Infomation Dump *********\n");
    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {
        p += sprintf(p, "\n***** %s *****\n", mt65xx_category_name[i]);

        for (j = 0; j < mt65xx_clock_number_per_category[i]; j++) {
            if (show_detail_mask[i] & (0x1 << j)) {
                p += sprintf(p, "clock[%3d](%d,%2d): %s, default %s, count = %d\n", 
                        (i*32+j), i, j, clk_mgr.clock[i][j].name, 
                        clk_mgr.clock[i][j].init_state == STATE_ON ? "on":"off",
                        clk_mgr.clock[i][j].count);
#ifdef CLK_MGR_DEBUG
                list_for_each(pos, &clk_mgr.clock[i][j].list) {
                    info = list_entry(pos, struct module_info, link);
                    p += sprintf(p, "\t\t\tmod: %s, en_cnt=%d, dis_cnt=%d\n", 
                            info->module_name, info->enable_cnt, info->disable_cnt);
                }
#endif
            }
        }
    }

    p += sprintf(p, "\n********** Detail Command *********\n");
    p += sprintf(p, "enable  detail: echo enable  id > /proc/clkmgr/detail\n");
    p += sprintf(p, "disable detail: echo disable id > /proc/clkmgr/detail\n");
    p += sprintf(p, "enable  detail: echo enable  category mask > /proc/clkmgr/detail\n");
    p += sprintf(p, "disable detail: echo disable category mask > /proc/clkmgr/detail\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int detail_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    char command[10];
    int id = -1;
    unsigned int mask = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %d %x", command, &id, &mask) == 3) { 
        if (!strcmp(command, "enable")) {
            set_show_detail_mask(id, mask);    /* here id is clock category id */
        } else if (!strcmp(command, "disable")) {
            clr_show_detail_mask(id, mask);
        }
    } else if (sscanf(desc, "%s %d", command, &id) == 2) { 
        if (!strcmp(command, "enable")) {   
            set_show_detail_bit(id);   /* here is id clock bit id */
        } else if (!strcmp(command, "disable")) {
            clr_show_detail_bit(id);
        }
    }

    return count;
}


/*
 * inspect_read is used to show clock/pll/subsys local state and register state 
 *          to inspect whether someone accessing registers directly without APIs 
 */


static int inspect_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    int i = 0;
    int local_state = -1;
    int reg_state = -1;

    p += sprintf(p, "********** Inspect Local and Regs States *********\n");

    p += sprintf(p, "\n***** Clock Category *****\n");
    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {
        p += sprintf(p, "[%d][%s]: [LOCAL] = 0x%08x  [REGS] = 0x%08x\n", i, 
                mt65xx_category_name[i], ~clk_mgr.clock_state[i], DRV_Reg32(mt65xx_cg_con_regs[i]));
    }

    p += sprintf(p, "\n***** PLL *****\n");
    for (i = 0; i < MT65XX_PLL_COUNT; i++) {
        local_state = get_state(TYPE_PLL, i, LOCAL_STATE); 
        reg_state = get_state(TYPE_PLL, i, REG_STATE); 
        p += sprintf(p, "[%d][%s]: [LOCAL] %-3s  [REGS] %-3s\n", i, mt65xx_pll_name[i], 
                local_state == STATE_ON ? "on" : "off",
                reg_state == STATE_ON ? "on" : "off");
    }

    p += sprintf(p, "\n***** Subsys *****\n");
    for (i = MT65XX_SUBSYS_MM1; i < MT65XX_SUBSYS_COUNT; i++) {
        local_state = get_state(TYPE_SUBSYS, i, LOCAL_STATE); 
        reg_state = get_state(TYPE_SUBSYS, i, REG_STATE); 
        p += sprintf(p, "[%d][%s]: [LOCAL] %-3s  [REGS] %-3s\n", i, mt65xx_subsys_name[i], 
                local_state == STATE_ON ? "on" : "off",
                reg_state == STATE_ON ? "on" : "off");
    }

    p += sprintf(p, "\nmmsys_clksrc_state = %d\n", clk_mgr.mmsys_state);

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int inspect_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    return count;
}

#define CATEGORY_AP(category)           ((0x1 << (category)) & 0x3)     /* 0b0000011 */

static int clock_test_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    int i = 0;

    p += sprintf(p, "********** Clock Registers Dump *********\n");

    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {
        p += sprintf(p, "[%d][%s / 0x%08x] = 0x%08x\n", i, mt65xx_category_name[i],
                mt65xx_cg_con_regs[i], DRV_Reg32(mt65xx_cg_con_regs[i]));
        if (CATEGORY_AP(i)) {
            p += sprintf(p, "[%d][MD_SIDE   / 0x%08x] = 0x%08x\n", i, 
                    (mt65xx_cg_con_regs[i] + 0x8), DRV_Reg32(mt65xx_cg_con_regs[i] + 0x8));
        }
    }

    p += sprintf(p, "\n********** Clock Test Command *********\n");
    p += sprintf(p, "enable  clock: echo enable  id [module_name] > /proc/clkmgr/clock_test\n");
    p += sprintf(p, "disable clock: echo disable id [module_name] > /proc/clkmgr/clock_test\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int clock_test_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    char command[10];
    char mod_name[10];
    int id = -1;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %d %s", command, &id, mod_name) == 3) {
        if (!strcmp(command, "enable")) {
            enable_clock(id, mod_name);
        } else if (!strcmp(command, "disable")) {
            disable_clock(id, mod_name);
        }
    } else if (sscanf(desc, "%s %d", command, &id) == 2) { 
        if (!strcmp(command, "enable")) {
            enable_clock(id, "clk_test");
        } else if (!strcmp(command, "disable")) {
            disable_clock(id, "clk_test");
        }
    }

    return count;
}


static int pll_test_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    int i = 0;

    p += sprintf(p, "********** PLL Registers Dump *********\n");
    for (i = 0; i < MT65XX_PLL_COUNT; i++) {
        p += sprintf(p, "[%d][%s_CON0 / 0x%08x] = 0x%04x\n", i, mt65xx_pll_name[i], 
                    mt65xx_pll_regs[i], DRV_Reg16(mt65xx_pll_regs[i]));
        if (i == MT65XX_WPLL) {
            p += sprintf(p, "[%d][%s_CON1 / 0x%08x] = 0x%04x\n", i, mt65xx_pll_name[i], 
                    (mt65xx_pll_regs[i] + 0x4), DRV_Reg16(mt65xx_pll_regs[i] + 0x4));
        } else if (i == MT65XX_TVDDS) {
            p += sprintf(p, "[%d][%s_CON1 / 0x%08x] = 0x%04x\n", i, mt65xx_pll_name[i], 
                    (mt65xx_pll_regs[i] + 0x4), DRV_Reg16(mt65xx_pll_regs[i] + 0x4));
            p += sprintf(p, "[%d][%s_CON2 / 0x%08x] = 0x%04x\n", i, mt65xx_pll_name[i], 
                    (mt65xx_pll_regs[i] + 0x8), DRV_Reg16(mt65xx_pll_regs[i] + 0x8));
        }
    }
    p += sprintf(p, "\n********** PLL Test Command *********\n");
    p += sprintf(p, "enable  pll: echo enable  id [module_name] > /proc/clkmgr/pll_test\n");
    p += sprintf(p, "disable pll: echo disable id [module_name] > /proc/clkmgr/pll_test\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int pll_test_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    char command[10];
    char mod_name[10];
    int id = -1;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %d %s", command, &id, mod_name) == 3) { 
        if (!strcmp(command, "enable")) {
            enable_pll(id, mod_name);
        } else if (!strcmp(command, "disable")) {
            disable_pll(id, mod_name);
        }
    } else if (sscanf(desc, "%s %d", command, &id) == 2) { 
        if (!strcmp(command, "enable")) {
            enable_pll(id, "pll_test");
        } else if (!strcmp(command, "disable")) {
            disable_pll(id, "pll_test");
        }
    }

    return count;
}


static int subsys_test_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    int i = 0;

    p += sprintf(p, "********** Subsys Registers Dump *********\n");
    for (i = 0; i < MT65XX_SUBSYS_COUNT; i++) {
        p += sprintf(p, "[%d]%s: [PWR_CON%d / 0x%08x] = 0x%08x\n", 
                i, mt65xx_subsys_name[i], i, mt65xx_subsys_regs[i],
                DRV_Reg32(mt65xx_subsys_regs[i]));
    }
    p += sprintf(p, "[%d]%-13s: [PWR_STA  / 0x%08x] = 0x%08x\t", i, "REG_STATES", 
            SC_PWR_STA, DRV_Reg32(SC_PWR_STA));

    p += sprintf(p, "\n********** Subsys Test Command *********\n");
    p += sprintf(p, "enable  subsys: echo enable  id [module_name] > /proc/clkmgr/subsys_test\n");
    p += sprintf(p, "disable subsys: echo disable id [module_name] > /proc/clkmgr/subsys_test\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}


static int subsys_test_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    char command[10];
    char mod_name[10];
    int id = -1;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %d %s", command, &id, mod_name) == 3) { 
        if (!strcmp(command, "enable")) {
            enable_subsys(id, mod_name);
        } else if (!strcmp(command, "disable")) {
            disable_subsys(id, mod_name);
        }
    } else if (sscanf(desc, "%s %d", command, &id) == 2) { 
        if (!strcmp(command, "enable")) {
            enable_subsys(id, "subsys_test");
        } else if (!strcmp(command, "disable")) {
            disable_subsys(id, "subsys_test");
        }
    }

    return count;
}


static int pll_fsel_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    int i = 0;
    unsigned int pll_value = -1;

    for (i = 0; i < MT65XX_PLL_COUNT; i++) {
        pll_value = get_pll_value(i);
        if (pll_value == -1) {
            p += sprintf(p, "[%d][%s]=[-1]\n", i, mt65xx_pll_name[i]);
        } else {
            p += sprintf(p, "[%d][%s]=[0x%x]\n", i, mt65xx_pll_name[i], pll_value);
        }
    }

    p += sprintf(p, "\n********** PLL fsel Test Command *********\n");
    p += sprintf(p, "select pll frequency:  echo id frequency > /proc/clkmgr/pll_fsel\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int pll_fsel_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    int id = -1;
    int pll_value = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%d %x", &id, &pll_value) == 2) { 
        pll_fsel(id, (unsigned int)pll_value);
    }

    return count;
}

static int freq_meter_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;

    p += sprintf(p, "[0x00:0] [0x01:26MHz] [0x02:32KHz] [0x03:MD_ROSC]\n");
    p += sprintf(p, "[0x04:CA9_ROSC] [0x05:ARMPLL] [0x06:MAINPLL] [0x07:IPLL]\n"); 
    p += sprintf(p, "[0x08:UPLL] [0x09:MDPLL_1040] [0x0A:MDPLL_297] [0x0B:TVDDS]\n");
    p += sprintf(p, "[0x0C:3GPLL_61] [0x0D:3GPLL_123] [0x0E:3GPLL_281] [0x0F:3GPLL_197]\n");
    p += sprintf(p, "[0x10:3GPLL_492] [0x11:AUDPLL] [0x12:ANA_26] [0x13:0]\n");

    p += sprintf(p, "\n********** PLL fsel Test Command *********\n");
    p += sprintf(p, "measure clock frequency:  echo id > /proc/clkmgr/freq_meter\n");
    p += sprintf(p, "measure clock frequency:  echo id delay(msec) > /proc/clkmgr/freq_meter\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int freq_meter_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    int clk_sel = 0;
    unsigned int delay = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%x %d", &clk_sel, &delay) == 2) {
        freq_meter(clk_sel, delay);
    } else if (sscanf(desc, "%x", &clk_sel) == 1) { 
        freq_meter(clk_sel, 0);
    }

    return count;
}

void backup_test(struct larb_monitor *h, int larb_idx)
{
    pmlog_ver("[%s]: backup larb %d\n", __func__, larb_idx);
    return;
}

void restore_test(struct larb_monitor *h, int larb_idx)
{
    pmlog_ver("[%s]: restore larb %d\n", __func__, larb_idx);
    return;
}

static struct larb_monitor test = {
    .level      = LARB_MONITOR_LEVEL_MEDIUM,
    .backup     = backup_test,
    .restore    = restore_test,
};

static int larb_monitor_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    struct larb_monitor** array = NULL;
    int number = 0;
    int i = 0;

    number = get_larb_monitor_handler_number();
    p += sprintf(p, "There are %d larb monitor handlers.\n", number);

    array = kzalloc(sizeof(struct larb_monitor*) * number, GFP_KERNEL);
    if(array) {
        for (i = 0; i < number; i++) {
            get_larb_monitor_handlers(array);
        }

        for (i = 0; i < number; i++) {
            if (array[i]) {
                p += sprintf(p, "larb_monitor[%d]: hanlder address = 0x%x, level = %d\n", i, (unsigned int)array[i], array[i]->level);
            } else {
                p += sprintf(p, "larb_monitor[%d]: handler address = NULL\n", i);
            }
        }

        kfree(array);
    }

    p += sprintf(p, "larb_monitor test: hanlder = 0x%x, level = %d\n", (unsigned int)&test, test.level);

    p += sprintf(p, "\n********** LARB Monitor Test Command *********\n");
    p += sprintf(p, "register   larb monitor handler: echo register   [addr] > /proc/clkmgr/larb_monitor\n");
    p += sprintf(p, "unregister larb monitor handler: echo unregister [addr] > /proc/clkmgr/larb_monitor\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int larb_monitor_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    char command[10];
    unsigned int addr = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %x", command, &addr) == 2) {
        if (!strcmp(command, "register")) {
            register_larb_monitor((struct larb_monitor*)addr);
        } else if (!strcmp(command, "unregister")) {
            unregister_larb_monitor((struct larb_monitor*)addr);
        }
    } else if (sscanf(desc, "%s", command) == 1) {
        if (!strcmp(command, "register")) {
            register_larb_monitor(&test);
        } else if (!strcmp(command, "unregister")) {
            unregister_larb_monitor(&test);
        }
    }

    return count;
}


static int mipi_test_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    int i = 0;


    p += sprintf(p, "MIPITX_CON0 = 0x%04x\n", DRV_Reg16(MIPI_TX_CON0));
    p += sprintf(p, "MIPITX_CON6 = 0x%04x\n", DRV_Reg16(MIPI_TX_CON6));
    p += sprintf(p, "MIPITX_CON9 = 0x%04x\n", DRV_Reg16(MIPI_TX_CON9));
    p += sprintf(p, "MIPIRX_CON1 = 0x%04x\n", DRV_Reg16(MIPI_RX_CON1));
    p += sprintf(p, "MIPIRX_CON4 = 0x%04x\n", DRV_Reg16(MIPI_RX_CON4));
    p += sprintf(p, "MIPIRX_CON5 = 0x%04x\n", DRV_Reg16(MIPI_RX_CON5));
    p += sprintf(p, "MIPI_CON    = 0x%08x\n", DRV_Reg32(MIPI_CON));

    for (i = 0; i < MT65XX_MIPI_COUNT; i++) { 
        p += sprintf(p, "%s count = %d\n", mipi[i].name, mipi[i].count);
    }

    p += sprintf(p, "\n********** MIPI Test Command *********\n");
    p += sprintf(p, "enable  mipi: echo enable  id [module_name] > /proc/clkmgr/mipi_test\n");
    p += sprintf(p, "disable mipi: echo disable id [module_name] > /proc/clkmgr/mipi_test\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int mipi_test_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    char command[10];
    char mod_name[10];
    int id = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %d %s", command, &id, mod_name) == 3) {
        if (!strcmp(command, "enable")) {
            enable_mipi(id, mod_name);
        } else if (!strcmp(command, "disable")) {
            disable_mipi(id, mod_name);
        }
    } else if (sscanf(desc, "%s %d", command, &id) == 2) { 
        if (!strcmp(command, "enable")) {
            enable_mipi(id, "mipi_test");
        } else if (!strcmp(command, "disable")) {
            disable_mipi(id, "mipi_test");
        }
    }

    return count;
}


static int mmsys_clksrc_test_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;

    p += sprintf(p, "TOP_CKMUXSEL   = 0x%x\n", DRV_Reg32(TOP_CKMUXSEL));
    p += sprintf(p, "TOP_CKDIV23    = 0x%x\n", DRV_Reg32(TOP_CKDIV23));
    p += sprintf(p, "TOP_CKMUXSEL2  = 0x%x\n", DRV_Reg16(TOP_CKMUXSEL2));
    p += sprintf(p, "AUDPLL_CON0    = 0x%x\n", DRV_Reg16(AUDPLL_CON0));
    p += sprintf(p, "MDPLL_CON0     = 0x%x\n", DRV_Reg16(MDPLL_CON0));

    p += sprintf(p, "\nmmsys_clksrc_state = %d\n", clk_mgr.mmsys_state);

    p += sprintf(p, "\n********** MMSYS CLKSRC Test Command *********\n");
    p += sprintf(p, "enable  clksrc: echo enable  > /proc/clkmgr/mmsys_clksrc_test\n");
    p += sprintf(p, "disable clksrc: echo disable > /proc/clkmgr/mmsys_clksrc_test\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int mmsys_clksrc_test_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    char command[10];
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s", command) == 1) {
        if (!strcmp(command, "enable")) {
            request_mmsys_clksrc(1,0);
            request_mmsys_clksrc(1,1);
        } else if (!strcmp(command, "disable")) {
            request_mmsys_clksrc(0,0);
            request_mmsys_clksrc(0,1);
        }
    }

    return count;
}


static int partial_log_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    int i = 0;

    p += sprintf(p, "********** Partial Log Bits Dump *********\n");
    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {
        p += sprintf(p, "[%d][%s] = 0x%08x\n", i, mt65xx_category_name[i], partial_log_mask[i]);
    }
    p += sprintf(p, "\n********** Partial Log Command *********\n");
    p += sprintf(p, "enable  partial log: echo enable  id > /proc/clkmgr/partial_log\n");
    p += sprintf(p, "disable partial log: echo disable id > /proc/clkmgr/partial_log\n");
    p += sprintf(p, "enable  partial log: echo enable  category mask > /proc/clkmgr/partial_log\n");
    p += sprintf(p, "disable partial log: echo disable category mask > /proc/clkmgr/partial_log\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int partial_log_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    char command[10];
    int id = -1;
    unsigned int mask = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %d %x", command, &id, &mask) == 3) { 
        if (!strcmp(command, "enable")) {
            set_partial_log_mask(id, mask);    /* here id is clock category id */
        } else if (!strcmp(command, "disable")) {
            clr_partial_log_mask(id, mask);
        }
    } else if (sscanf(desc, "%s %d", command, &id) == 2) { 
        if (!strcmp(command, "enable")) {   
            set_partial_log_bit(id);   /* here is id clock bit id */
        } else if (!strcmp(command, "disable")) {
            clr_partial_log_bit(id);
        }
    }
    return count;
}

#ifdef OP_MODE_DEBUG
static int mgr_mode_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;

    p += sprintf(p, "Clock Manager Current Work Mode is %s\n", op_mode == 0 ? "no actual job": "internal test");
    p += sprintf(p, "********** How to Switch Work Mode *********\n");
    p += sprintf(p, "enable internal test:  echo 1 > /proc/clkmgr/mgr_mode\n");
    p += sprintf(p, "enable no actual job:  echo 0 > /proc/clkmgr/mgr_mode\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int mgr_mode_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    int new_op_mode = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%x", &new_op_mode) == 1) { 
        op_mode = new_op_mode;
    }
    return count;
}
#endif

#ifdef FORCE_UNGATE_DEBUG
static int force_ungate_read(char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;
    int i = 0;

    p += sprintf(p, "********** Force Ungate Bits Dump *********\n");
    for (i = 0; i < MT65XX_CLOCK_CATEGORY_COUNT; i++) {
        p += sprintf(p, "[%d][%s] = 0x%08x\n", i, mt65xx_category_name[i], force_ungate_mask[i]);
    }
    p += sprintf(p, "\n********** Force Ungate Command *********\n");
    p += sprintf(p, "enable  ungate: echo enable  id > /proc/clkmgr/force_ungate\n");
    p += sprintf(p, "disable ungate: echo disable id > /proc/clkmgr/force_ungate\n");
    p += sprintf(p, "enable  ungate: echo enable  category mask > /proc/clkmgr/force_ungate\n");
    p += sprintf(p, "disable ungate: echo disable category mask > /proc/clkmgr/force_ungate\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int force_ungate_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;
    char command[10];
    int id = -1;
    unsigned int mask = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %d %x", command, &id, &mask) == 3) { 
        if (!strcmp(command, "enable")) {
            set_force_ungate_mask(id, mask);    /* here id is clock category id */
        } else if (!strcmp(command, "disable")) {
            clr_force_ungate_mask(id, mask);
        }
    } else if (sscanf(desc, "%s %d", command, &id) == 2) { 
        if (!strcmp(command, "enable")) {   
            set_force_ungate_bit(id);   /* here is id clock bit id */
        } else if (!strcmp(command, "disable")) {
            clr_force_ungate_bit(id);
        }
    }
    return count;
}
#endif


void mt6577_pm_log_init(void)
{
    struct proc_dir_entry *entry = NULL;
    struct proc_dir_entry *clkmgr_dir = NULL;

    entry = create_proc_entry("pm_flag", S_IRUGO | S_IWUSR, NULL);
    if (entry) {
        entry->read_proc = pm_flag_read;
        entry->write_proc = pm_flag_write;
    }

#if defined(CONFIG_WAKELOCK) && defined(CONFIG_USER_WAKELOCK) && defined(CONFIG_EARLYSUSPEND) 
	/* early_suspend_control */
    entry = create_proc_entry("pm_early_suspend_control", 0, NULL);
    if (entry) {
        entry->read_proc = early_suspend_ctrl_read;
        entry->write_proc = early_suspend_ctrl_write;
    }
#endif
    
    entry = create_proc_entry("pm_reg_dump", S_IRUGO | S_IWUSR, NULL);
    if (entry) {
        entry->read_proc = pm_reg_dump_read;
    }

    clkmgr_dir = proc_mkdir("clkmgr", NULL);
    if (!clkmgr_dir) {
        pmlog_err("[%s]: mkdir /proc/clkmgr failed\n", __func__);
        return;
    }

    entry = create_proc_entry("dump", S_IRUGO, clkmgr_dir);
    if (entry) {
        entry->read_proc = dump_read;
    }

    entry = create_proc_entry("detail", S_IRUGO, clkmgr_dir);
    if (entry) {
        //memset(show_detail_mask, 0, sizeof(unsigned int) * MT65XX_CLOCK_CATEGORY_COUNT);
        entry->read_proc = detail_read;
        entry->write_proc = detail_write;
    }

    entry = create_proc_entry("inspect", S_IRUGO | S_IWUSR, clkmgr_dir);
    if (entry) {
        entry->read_proc = inspect_read;
        entry->write_proc = inspect_write;
    }

    entry = create_proc_entry("clock_test", S_IRUGO | S_IWUSR, clkmgr_dir);
    if (entry) {
        entry->read_proc = clock_test_read;
        entry->write_proc = clock_test_write;
    }

    entry = create_proc_entry("pll_test", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, clkmgr_dir);
    if (entry) {
        entry->read_proc = pll_test_read;
        entry->write_proc = pll_test_write;
    }

    entry = create_proc_entry("subsys_test", S_IRUGO | S_IWUSR, clkmgr_dir);
    if (entry) {
        entry->read_proc = subsys_test_read;
        entry->write_proc = subsys_test_write;
    }

    entry = create_proc_entry("pll_fsel", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, clkmgr_dir);
    if (entry) {
        entry->read_proc = pll_fsel_read;
        entry->write_proc = pll_fsel_write;
    }
    
    entry = create_proc_entry("freq_meter", S_IRUGO | S_IWUSR, clkmgr_dir);
    if (entry) {
        entry->read_proc = freq_meter_read;
        entry->write_proc = freq_meter_write;
    }

    entry = create_proc_entry("larb_monitor", S_IRUGO | S_IWUSR, clkmgr_dir);
    if (entry) {
        entry->read_proc = larb_monitor_read;
        entry->write_proc = larb_monitor_write;
    }

    entry = create_proc_entry("mipi_test", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, clkmgr_dir);
    if (entry) {
        entry->read_proc = mipi_test_read;
        entry->write_proc = mipi_test_write;
    }

    entry = create_proc_entry("mmsys_clksrc_test", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, clkmgr_dir);
    if (entry) {
        entry->read_proc = mmsys_clksrc_test_read;
        entry->write_proc = mmsys_clksrc_test_write;
    }

    entry = create_proc_entry("partial_log", S_IRUGO | S_IWUSR, clkmgr_dir);
    if (entry) {
        entry->read_proc = partial_log_read;
        entry->write_proc = partial_log_write;
    }

#ifdef OP_MODE_DEBUG
    entry = create_proc_entry("mgr_mode", S_IRUGO | S_IWUSR, clkmgr_dir);
    if (entry) {
        entry->read_proc = mgr_mode_read;
        entry->write_proc = mgr_mode_write;
    }
#endif

#ifdef FORCE_UNGATE_DEBUG
    entry = create_proc_entry("force_ungate", S_IRUGO | S_IWUSR, clkmgr_dir);
    if (entry) {
        entry->read_proc = force_ungate_read;
        entry->write_proc = force_ungate_write;
    }
#endif
}
